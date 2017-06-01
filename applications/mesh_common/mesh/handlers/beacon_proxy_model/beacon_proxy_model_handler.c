/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      beacon_proxy_model_handler.c
 *
 *  DESCRIPTION
 *      Implements handler function for beacon model messages
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <time.h>
#include <timer.h>
#include <mem.h>
#ifdef CSR101x
#include <ls_app_if.h>
#else
#include <ls_api.h>
#endif

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "iot_hw.h"
#include "csr_mesh.h"
#include "beacon_proxy_model_handler.h"
#include "beaconproxy_server.h"
#include "app_mesh_handler.h"
#include "core_mesh_handler.h"
#include "nvm_access.h"
#include "main_app.h"
#ifdef ENABLE_BEACON_MODEL
#include "beacon_model_handler.h"
#endif
#ifdef ENABLE_BEACON_PROXY_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* numdevices encoding:
 * Bits 0..2 How many device are in this message.
 * *  If bit 7 is set, the first device address is a GROUP address, 
 *    to which the other device will be assigned. 
 * *  If bit 8 is set, queued messages for these devices will be cleared
 */
#define BEACON_COUNT_MASK             (0x07)
#define HAS_GROUP_ADDRESS             (1 << 6)
#define CLEAR_MESSAGE_QUEUE           (1 << 7)

/*============================================================================*
 *  Private Data
 *============================================================================*/

static BEACON_PROXY_HANDLER_DATA_T* p_beacon_proxy_data;
BEACON_PROXY_DEVICE_GROUP_LIST_T device_group_list;

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeBeaconProxyDevIdOntoNvm
 *
 *  DESCRIPTION
 *      This function writes beacon device id data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void writeBeaconProxyDevIdOntoNvm(uint8 index)
{
    Nvm_Write((uint16*)(&device_group_list.dev_id[index]), 
              sizeof(uint16),
              GET_DEV_ID_NVM_OFFSET(index));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeBeaconProxyGrpIdOntoNvm
 *
 *  DESCRIPTION
 *      This function writes beacon proxy group id data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void writeBeaconProxyGrpIdOntoNvm(uint8 index)
{
    Nvm_Write((uint16*)(&device_group_list.group_id[index]), 
              sizeof(uint16),
              GET_GRP_ID_NVM_OFFSET(index));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeBeaconProxyDevGrpBitmaskOntoNvm
 *
 *  DESCRIPTION
 *      This function writes beacon proxy device group bitmask data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void writeBeaconProxyDevGrpBitmaskOntoNvm(uint8 index)
{
    Nvm_Write((uint16*)(&device_group_list.dev_grp_bitmask[index]), 
              sizeof(uint16),
              GET_DEV_GRP_BITMASK_NVM_OFFSET(index));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      isProxyConfigured
 *
 *  DESCRIPTION
 *      This function returns whether the proxy is configured for any device or
 *      not.
 *
 *  RETURNS
 *      TRUE if the device configured and FALSE if not.
 *
 *---------------------------------------------------------------------------*/
static bool isProxyConfigured(void)
{
    CsrUint16 index;

    for(index=0; index < MAX_MANAGED_BEACON_DEVS; index++)
    {
        if(device_group_list.dev_id[index] != 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      updateBeaconProxyStatus
 *
 *  DESCRIPTION
 *      update the beacon proxy status with the latest values to be sent across
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void updateBeaconProxyStatus(void)
{
    CsrUint16 index;
    MemSet(&p_beacon_proxy_data->proxy_status,
           0, 
           sizeof(CSRMESH_BEACONPROXY_PROXY_STATUS_T));

    for(index = 0; index < MAX_MANAGED_BEACON_GRPS; index++)
    {
        if(device_group_list.group_id[index] != 0)
        {
            /* Add the number of groups here */
            p_beacon_proxy_data->proxy_status.nummanagedgroups++;
        }
    }

    for(index = 0; index < MAX_MANAGED_BEACON_DEVS; index++)
    {
        if(device_group_list.dev_id[index] != 0)
        {
            p_beacon_proxy_data->proxy_status.nummanagednodes++;
        }
    }

#ifdef ENABLE_BEACON_MODEL
    p_beacon_proxy_data->proxy_status.numqueuedtxmsgs = GetQueuedTxMsgStats();
#endif

    /* We dont queue any rx messages, hence setting to 0 */
    p_beacon_proxy_data->proxy_status.numqueuedrxmsgs = 0;

    /* Queue size supported to store the beacons */
    p_beacon_proxy_data->proxy_status.maxqueuesize = MAX_BEACONS_SUPPORTED;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      addBeaconDevice
 *
 *  DESCRIPTION
 *      Add a new beacon device to the managed beacon list
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void addBeaconDevice(CSRMESH_BEACONPROXY_ADD_T *p_dev)
{
    CsrUint8 i=0, j=0;
    CsrUint16 group_id  = 0, device_id = 0;
    CsrUint8 devices_to_add;
    CsrUint8 group_index = 0xFF;
    bool device_present = FALSE;
    bool clear_queue = FALSE;
    p_beacon_proxy_data->cmd_status.tid  = p_dev->tid;

    /* Get the total number of device ids packed in the message */
    devices_to_add = p_dev->numdevices & BEACON_COUNT_MASK;

    if(p_dev->numdevices & CLEAR_MESSAGE_QUEUE)
        clear_queue = TRUE;

    /* Check if it has a group address */
    if((p_dev->numdevices & HAS_GROUP_ADDRESS) && (devices_to_add > 0))
    {
        group_id = p_dev->deviceaddresses[1] << 8;
        group_id |= p_dev->deviceaddresses[0];
        j = 2;

        for(i=0; i < MAX_MANAGED_BEACON_GRPS; i++)
        {
            if(device_group_list.group_id[i] == group_id)
            {
                /* group is already present, so break */
                group_index = i;
#ifdef ENABLE_BEACON_MODEL
                if(clear_queue == TRUE)
                RemoveBeaconInfoOfDevice(group_id);
#endif
                break;
            }
        }

        /* If its a new group add it onto the list */
        if(group_index == 0xFF)
        {
            for(i=0; i < MAX_MANAGED_BEACON_GRPS; i++)
            {
                if(device_group_list.group_id[i] == 0)
                {
                    /* device is added, so break */
                    device_group_list.group_id[i] = group_id;
                    writeBeaconProxyGrpIdOntoNvm(i);
                    group_index = i;
                    break;
                }
            }
        }
    }

    for( ;j < (devices_to_add*2) ; j=j+2)
    {
        device_id = p_dev->deviceaddresses[j+1] << 8;
        device_id |= p_dev->deviceaddresses[j];
        device_present = FALSE;
        
        for(i = 0 ; i < MAX_MANAGED_BEACON_DEVS ; i++)
        {
            /* If the device received is already present in the list, then just
             * update the dev grp bitmask for this group.
             */
            if( device_group_list.dev_id[i] == device_id)
            {
                if(group_id)
                {
                    device_group_list.dev_grp_bitmask[i] |= 1 << group_index;
                    writeBeaconProxyDevGrpBitmaskOntoNvm(i);
                }
                device_present = TRUE;
#ifdef ENABLE_BEACON_MODEL
                if(clear_queue == TRUE)
                RemoveBeaconInfoOfDevice(device_id);
#endif
                break;
            }
        }
        /* If device is not present then add it onto an empty slot in the 
         * device list.
         */
        if(!device_present)
        {
            for(i = 0 ; i < MAX_MANAGED_BEACON_DEVS ; i++)
            {
                if( device_group_list.dev_id[i] == 0 )
                {
                    /* Found an empty slot in the device list */
                    device_group_list.dev_id[i] = device_id;
                    writeBeaconProxyDevIdOntoNvm(i);

                    if(group_id)
                    {
                        device_group_list.dev_grp_bitmask[i] |= 1 << group_index;
                        writeBeaconProxyDevGrpBitmaskOntoNvm(i);
                    }
                    break;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      removeBeaconDevice
 *
 *  DESCRIPTION
 *      Remove a new beacon device from the managed beacon list
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void removeBeaconDevice(CSRMESH_BEACONPROXY_REMOVE_T *p_dev)
{
    CsrUint8 i=0, j=0, k=0;
    CsrUint16 group_id = 0, device_id = 0;
    CsrUint8 devices_to_remove;
    CsrUint8 group_index = 0xFF;
    p_beacon_proxy_data->cmd_status.tid  = p_dev->tid;
    bool old_proxy_config = isProxyConfigured();

    /* Get the total number of device ids packed in the message */
    devices_to_remove = p_dev->numdevices & BEACON_COUNT_MASK;

    /* Check if it has a group address */
    if((p_dev->numdevices & HAS_GROUP_ADDRESS) && (devices_to_remove > 0))
    {
        group_id = p_dev->deviceaddresses[1] << 8;
        group_id |= p_dev->deviceaddresses[0];
        j = 2;

        for(i=0; i < MAX_MANAGED_BEACON_GRPS; i++)
        {
            if(device_group_list.group_id[i] == group_id && group_id != 0)
            {
                /* This group index is used to remove the devices from list */
                group_index = i;

                /* If complete group is removed then only remove the grp, 
                 * otherwise just remove the group references in devices.
                 */
                if(devices_to_remove == 1)
                {
                    device_group_list.group_id[i] = 0;
                    writeBeaconProxyGrpIdOntoNvm(i);
#ifdef ENABLE_BEACON_MODEL
                    RemoveBeaconInfoOfDevice(group_id);
#endif
                    /* Remove all the group references here as complete grp
                     * is getting removed
                     */
                     for(k = 0; k < MAX_MANAGED_BEACON_DEVS; k++)
                     {
                        /* Remove the device from the group bitmask */
                        device_group_list.dev_grp_bitmask[k] &=
                                                        (~(1 << group_index));
                        writeBeaconProxyDevGrpBitmaskOntoNvm(k);
                     }
                }
                break;
            }
        }
    }

    /* we can ignore if we have received a valid group id but group id is not
     * present in our list.
     */
    if((group_index != 0xFF && group_id != 0) || (group_id == 0))
    {
        for( ;j < (devices_to_remove*2) ; j = j+ 2)
        {
            device_id = p_dev->deviceaddresses[j+1] << 8;
            device_id |= p_dev->deviceaddresses[j];

            for(i = 0 ; i < MAX_MANAGED_BEACON_DEVS ; i++)
            {
                if(device_group_list.dev_id[i] == device_id && device_id != 0)
                {
                    /* This means that just the devices are being removed */
                    if(group_id == 0)
                    {
                        /* Found the device in the list, remove the device */
                        device_group_list.dev_id[i] = 0;
                        device_group_list.dev_grp_bitmask[i] = 0;
                        writeBeaconProxyDevIdOntoNvm(i);
                        writeBeaconProxyDevGrpBitmaskOntoNvm(i);
#ifdef ENABLE_BEACON_MODEL
                        RemoveBeaconInfoOfDevice(device_id);
#endif
                        break;
                    }
                    else
                    {
                        /* Remove the device from the group bitmask */
                        device_group_list.dev_grp_bitmask[i] &=
                                                        (~(1 << group_index));
                        writeBeaconProxyDevGrpBitmaskOntoNvm(i);
                        break;
                    }
                }
                /* If we have received a device address with 0 then we should
                 * not be proxy for any device.
                 */
                else if(device_id == 0 && old_proxy_config == TRUE)
                {
                    MemSet(&device_group_list,
                           0x0000,
                           sizeof(BEACON_PROXY_DEVICE_GROUP_LIST_T));
                    WriteBeaconProxyModelDataOntoNVM();
                    break;
                }
            }
        }
    }

#ifdef ENABLE_BEACON_MODEL
    /* If all the devices are being removed from the beacon proxy then we can
     * re-initialise the beacon information in the proxy device.
     */
    if(old_proxy_config && !isProxyConfigured())
    {
        BeaconModelDataInit(NULL);
    }
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      beaconProxyModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Beacon Proxy Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult beaconProxyModelEventHandler(
                                             CSRMESH_MODEL_EVENT_T event_code, 
                                             CSRMESH_EVENT_DATA_T* data,
                                             CsrUint16 length,
                                             void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_BEACONPROXY_ADD:
        {
            addBeaconDevice((CSRMESH_BEACONPROXY_ADD_T *)(data->data));

            if (state_data != NULL)
            {
                *state_data = (void *)&p_beacon_proxy_data->cmd_status;
            }
        }
        break;

        case CSRMESH_BEACONPROXY_REMOVE:
        {
            removeBeaconDevice((CSRMESH_BEACONPROXY_REMOVE_T *)(data->data));

            if (state_data != NULL)
            {
                *state_data = (void *)&p_beacon_proxy_data->cmd_status;
            }
        }
        break;

        case CSRMESH_BEACONPROXY_GET_STATUS:
        {
            updateBeaconProxyStatus();

            if (state_data != NULL)
            {
                *state_data = (void *)&p_beacon_proxy_data->proxy_status;
            }
        }
        break;

        default:
        break;
    }
    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadBeaconProxyModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads beacon proxy model data from NVM onto the global
 *      variable.
 *
 *  RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/
extern void ReadBeaconProxyModelDataFromNVM(void)
{
    uint8 index;

    for(index=0; index < MAX_MANAGED_BEACON_DEVS; index++)
    {
        Nvm_Read((uint16 *)&device_group_list.dev_id[index],
                 sizeof(uint16), 
                 GET_DEV_ID_NVM_OFFSET(index));

        Nvm_Read((uint16 *)&device_group_list.dev_grp_bitmask[index],
                 sizeof(uint16), 
                 GET_DEV_GRP_BITMASK_NVM_OFFSET(index));
    }

    for(index=0; index < MAX_MANAGED_BEACON_GRPS; index++)
    {
        Nvm_Read((uint16 *)&device_group_list.group_id[index],
                 sizeof(uint16), 
                 GET_GRP_ID_NVM_OFFSET(index));
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteBeaconProxyModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes beacon proxy model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void WriteBeaconProxyModelDataOntoNVM(void)
{
    uint8 index;
    for(index=0; index < MAX_MANAGED_BEACON_DEVS; index++)
    {
        writeBeaconProxyDevIdOntoNvm(index);
        writeBeaconProxyDevGrpBitmaskOntoNvm(index);
    }

    for(index=0; index < MAX_MANAGED_BEACON_GRPS; index++)
    {
        writeBeaconProxyGrpIdOntoNvm(index);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IsAGroupDevice
 *
 *  DESCRIPTION
 *      This function checks whether the passed device is in the group list.
 *
 *  RETURNS
 *      TRUE if the device is present and FALSE if not.
 *
 *---------------------------------------------------------------------------*/
extern bool IsAGroupDevice(CsrUint16 dev_id)
{
    CsrUint16 index;

    for(index=0; index < MAX_MANAGED_BEACON_GRPS; index++)
    {
        if(device_group_list.group_id[index] == dev_id)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetBeaconGroups
 *
 *  DESCRIPTION
 *      This function returns the group information for the passed beacon device.
 *
 *  RETURNS
 *      The number of groups the beacon device is grouped under.
 *
 *---------------------------------------------------------------------------*/
extern CsrUint16 GetBeaconGroups(CsrUint16 dev_id, uint16 grps[])
{
    CsrUint16 index, grp_bitmask = 0, mask=0, grp_cnt = 0;

    for(index=0; index < MAX_MANAGED_BEACON_DEVS; index++)
    {
        if(device_group_list.dev_id[index] == dev_id)
        {
            grp_bitmask = device_group_list.dev_grp_bitmask[index];
            break;
        }
    }

    index = 0;
    while(grp_bitmask > 0)
    {
        mask = grp_bitmask & 0x01;
        grp_bitmask >>= 1;
        if(mask == 1)
        {
            grps[grp_cnt++] = device_group_list.group_id[index];
        }
        index++;
    }
    return grp_cnt;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CheckForDeviceInterest
 *
 *  DESCRIPTION
 *      This function checks whether the passed device is of any interest to the
 *      beacon proxy
 *
 *  RETURNS
 *      TRUE if the device is of interest and FALSE if not.
 *
 *---------------------------------------------------------------------------*/
extern bool CheckForDeviceInterest(CsrUint16 dev_id)
{
    CsrUint16 index;
    bool is_dev_present = FALSE;

    for(index=0; index < MAX_MANAGED_BEACON_DEVS; index++)
    {
        if(device_group_list.dev_id[index] == dev_id)
        {
            return TRUE;
        }
        else if(device_group_list.dev_id[index] != 0)
        {
            is_dev_present = TRUE;
        }
    }

    /* Check for the group id comparision only if there is any valid device
     * present in the device group.
     */
    if(is_dev_present)
    {
        for(index=0; index < MAX_MANAGED_BEACON_GRPS; index++)
        {
            if(device_group_list.group_id[index] == dev_id)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BeaconProxyModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Beacon model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BeaconProxyModelHandlerInit(CsrUint8 nw_id,
                                        CsrUint16 model_groups[],
                                        CsrUint16 num_groups)
{
    /* Initialize Beacon Model */
    BeaconProxyModelInit(nw_id, model_groups, num_groups,
                         beaconProxyModelEventHandler);

    /* Setup the device list to be accessed by the model */
    BeaconProxySetupBeaconList((CsrUint16 *)&device_group_list,
                                MAX_MANAGED_BEACON_GRPS,
                                MAX_MANAGED_BEACON_DEVS);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BeaconProxyModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Beacon Proxy Model data on the global 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BeaconProxyModelDataInit(
                    BEACON_PROXY_HANDLER_DATA_T* beacon_proxy_handler_data)
{
    if(beacon_proxy_handler_data != NULL)
    {
        p_beacon_proxy_data = beacon_proxy_handler_data;
    }

    MemSet(&device_group_list,
           0x0000,
           sizeof(BEACON_PROXY_DEVICE_GROUP_LIST_T));
}

#endif /* ENABLE_BEACON_PROXY_MODEL */

