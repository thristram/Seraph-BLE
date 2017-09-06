/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gatt_service.c
 *
 *  DESCRIPTION
 *      This file defines routines for using gatt service
 *
 ****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *===========================================================================*/
#include <gatt.h>
#include <gatt_prim.h>
#include <gatt.h>
#include <gatt_uuid.h>
#include <buf_utils.h>

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "gatt_service.h"
#include "nvm_access.h"
#include "app_gatt_db.h"
#include "cm_types.h"
#include "user_config.h"

/*============================================================================*
 *  Private Data Types
 *===========================================================================*/

/* Service data type */
typedef struct
{
    /* Bond id */
    bond_handle_id          bond_id;

    /* Service Changed Flag */
    uint16                  serv_changed;

    /* The current configuration for the Service Changed characteristic */
    gatt_client_config      serv_changed_config;

    /* NVM Offset at which gatt data is stored */
    uint16                  nvm_offset;
    
    /* Temporary pairing flag */
    bool                    temporary_pairing;

} SERVICE_DATA_T;

/*============================================================================*
 *  Private Definitions
 *===========================================================================*/

/* The position of the Bond ID */
#define BOND_ID_OFFSET                      (0)

/* The position of the Service Changed configuration information in the NVM */
#define SERV_CHANGED_CLIENT_CONFIG_OFFSET   (1)

/* The position of the "this device might have been updated" flag in the NVM */
#define SERV_CHANGED_SEND_IND_OFFSET        (2)

/* Number of words of NVM memory used by gatt service */
#define GATT_SERVICE_NVM_MEMORY_WORDS       (3)

/* Highest possible handle for ATT database. */
#define ATT_HIGHEST_POSSIBLE_HANDLE         (0xFFFF)

/*============================================================================*
 *   Private Function Prototypes
 *===========================================================================*/

/* This function writes battery data to NVM */
static void writeToNvm(void);

/* This function is used to read data from the NVM */
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset);

#if defined (CSR101x_A05)
/*  This function should be called when a bonded host connects */
static void onConnected(device_handle_id device_id);
#endif /* CSR101x_A05 */

/* This function handles the connection notification */
static void handleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data);

/* This function handles the bonding notification */
static void handleBondNotify(CM_BONDING_NOTIFY_T *cm_event_data);

/* This function handles read operation */
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data);

/* This function handles write */
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data);

/* This function handles the events from the connection manager */
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data);

/*============================================================================*
 *  Private Data
 *===========================================================================*/

/* Service data */
static SERVICE_DATA_T g_service_data;

/* Service handler */
static CM_HANDLERS_T g_server_handler =
{
    .pCallback = &handleConnMgrProcedureEvent
};

/* Service Information */
static CM_SERVER_INFO_T g_service_info;

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeToNvm
 *
 *  DESCRIPTION
 *      This function writes gatt data to NVM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void writeToNvm(void)
{
    /* Write Bond Id */
    Nvm_Write((uint16*)&g_service_data.bond_id,
            sizeof(g_service_data.bond_id),
            g_service_data.nvm_offset +
            BOND_ID_OFFSET);

    /* Write Service Changed client configuration */
    Nvm_Write((uint16*)&(g_service_data.serv_changed_config),
            sizeof(g_service_data.serv_changed_config),
            (g_service_data.nvm_offset +
             SERV_CHANGED_CLIENT_CONFIG_OFFSET));

    /* Write Service Has Changed flag */
    Nvm_Write((uint16*)&(g_service_data.serv_changed),
            sizeof(g_service_data.serv_changed),
            (g_service_data.nvm_offset + SERV_CHANGED_SEND_IND_OFFSET));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      readDataFromNVM
 *
 *  DESCRIPTION
 *      This function is used to read data from the NVM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset)
{
    /* Save the NVM offset */
    g_service_data.nvm_offset = *nvm_offset;

    /* Read NVM only if the nvm is not fresh */
    if(!nvm_start_fresh)
    {
        /* Read Bond Id */
        Nvm_Read((uint16*)&g_service_data.bond_id,
                sizeof(g_service_data.bond_id),
                g_service_data.nvm_offset +
                BOND_ID_OFFSET);

        /* Read Service Changed client configuration */
        Nvm_Read((uint16*)&(g_service_data.serv_changed_config),
                sizeof(g_service_data.serv_changed_config),
                (g_service_data.nvm_offset +
                 SERV_CHANGED_CLIENT_CONFIG_OFFSET));

        /* Read Service Has Changed flag */
        Nvm_Read((uint16*)&(g_service_data.serv_changed),
                sizeof(g_service_data.serv_changed),
                (g_service_data.nvm_offset + SERV_CHANGED_SEND_IND_OFFSET));
    }
    else
    {
        g_service_data.bond_id = CM_INVALID_BOND_ID;
        g_service_data.serv_changed_config = gatt_client_config_none;
        g_service_data.serv_changed = FALSE;

        /* Write data to NVM */
        writeToNvm();

    }

    /* Increment the offset by the number of words of NVM memory required
     * by gatt service
     */
    *nvm_offset += GATT_SERVICE_NVM_MEMORY_WORDS;

}

#if defined (CSR101x_A05)
/*-----------------------------------------------------------------------------*
 *  NAME
 *      onConnected
 *
 *  DESCRIPTION
 *      This function sends the gatt service changed indication
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void onConnected(device_handle_id device_id)
{
    uint8 service_changed_data[4];

    service_changed_data[0] = WORD_LSB((HANDLE_GATT_SERVICE_END+1));
    service_changed_data[1] = WORD_MSB((HANDLE_GATT_SERVICE_END+1));
    service_changed_data[2] = WORD_LSB(ATT_HIGHEST_POSSIBLE_HANDLE);
    service_changed_data[3] = WORD_MSB(ATT_HIGHEST_POSSIBLE_HANDLE);

    if((g_service_data.serv_changed) &&
       (g_service_data.serv_changed_config == gatt_client_config_indication))
    {
        CM_VALUE_NOTIFICATION_T cm_value_notify;

        cm_value_notify.device_id = device_id;
        cm_value_notify.handle = HANDLE_SERVICE_CHANGED;
        cm_value_notify.size_value = sizeof(service_changed_data);
        cm_value_notify.value = service_changed_data;

        /*  Send an indication */
        CMSendValueIndication(&cm_value_notify);

        /* Now that the indication has been sent, clear the flag in the NVM */
        g_service_data.serv_changed = FALSE;

        if(!g_service_data.temporary_pairing)
        {
            Nvm_Write((uint16*)&(g_service_data.serv_changed),
                      sizeof(g_service_data.serv_changed),
                      (g_service_data.nvm_offset + SERV_CHANGED_SEND_IND_OFFSET));
        }
    }
}
#endif /* CSR101x_A05 */

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnNotify
 *
 *  DESCRIPTION
 *      This function handles the connection notification
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data)
{
    if(cm_event_data->result == cm_conn_res_success)
    {
        if((g_service_data.bond_id != CM_INVALID_BOND_ID) &&
           (g_service_data.bond_id
            == CMGetBondIdFromDeviceId(cm_event_data->device_id)))
        {
#if defined (CSR101x_A05)
            onConnected(cm_event_data->device_id);
#endif /*CSR101x_A05  */
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleBondNotify
 *
 *  DESCRIPTION
 *      This function handles the bonding notification
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleBondNotify(CM_BONDING_NOTIFY_T *cm_event_data)
{
    if(cm_event_data->result == cm_bond_res_success)
    {
        g_service_data.bond_id = cm_event_data->bond_id;
    }
    else if(cm_event_data->result == cm_unbond_res_success)
    {
        g_service_data.bond_id = CM_INVALID_BOND_ID;
        g_service_data.serv_changed_config = gatt_client_config_none;
        g_service_data.serv_changed = FALSE;
    }    
    if(!g_service_data.temporary_pairing)
    {
        /* Write data to NVM */
        writeToNvm();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleAccessRead
 *
 *  DESCRIPTION
 *      This function handles read operation
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data)
{
    CM_ACCESS_RESPONSE_T cm_access_rsp;
    uint16  data_length = 0;
    uint8   value[2];
    uint8   *p_value = NULL;
    sys_status rc = gatt_status_read_not_permitted;

    switch(p_event_data->handle)
    {
        case HANDLE_SERVICE_CHANGED_CLIENT_CONFIG:
        {
            /* Service changed client characteristic configuration
             * descriptor read has been requested
             */
            data_length = 2;
            p_value = value;
            BufWriteUint16((uint8 **)&p_value,
                           g_service_data.serv_changed_config);

            rc = sys_status_success;
        }
        break;

        default:
            /* No more IRQ characteristics */
            rc = gatt_status_read_not_permitted;
        break;

    }

    cm_access_rsp.device_id         = p_event_data->device_id;
    cm_access_rsp.handle            = p_event_data->handle;
    cm_access_rsp.rc                = rc;
    cm_access_rsp.size_value        = data_length;
    cm_access_rsp.value             = value;

    /* Send the access response */
    CMSendAccessRsp(&cm_access_rsp);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles write operation
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data)
{
    CM_ACCESS_RESPONSE_T cm_access_rsp;
    uint8 *p_value = p_event_data->data;
    uint16 client_config;
    sys_status rc = sys_status_success;

    switch(p_event_data->handle)
    {
        case HANDLE_SERVICE_CHANGED_CLIENT_CONFIG:
        {
            client_config = BufReadUint16(&p_value);

            if((client_config == gatt_client_config_indication) ||
               (client_config == gatt_client_config_none))
            {
                g_service_data.serv_changed_config = client_config;
                g_service_data.bond_id =
                        CMGetBondIdFromDeviceId(p_event_data->device_id);
                
                if((!g_service_data.temporary_pairing) && 
                   g_service_data.bond_id != CM_INVALID_BOND_ID)
                {
                    /* Write to NVM */
                    writeToNvm();
                }
            }
            else
            {
                rc = gatt_status_desc_improper_config;
            }
        }
        break;

        default:
            rc = gatt_status_write_not_permitted;
        break;
    }

    cm_access_rsp.device_id     = p_event_data->device_id;
    cm_access_rsp.handle        = p_event_data->handle;
    cm_access_rsp.rc            = rc;
    cm_access_rsp.size_value    = 0;
    cm_access_rsp.value         = NULL;

    /* Send the access response */
    CMSendAccessRsp(&cm_access_rsp);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnMgrProcedureEvent
 *
 *  DESCRIPTION
 *       This function handles the events from the connection manager.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data)
{
    switch(event_type)
    {
        case CM_CONNECTION_NOTIFY:
            handleConnNotify((CM_CONNECTION_NOTIFY_T *)p_event_data);
        break;

        case CM_BONDING_NOTIFY:
            handleBondNotify((CM_BONDING_NOTIFY_T *)p_event_data);
        break;

        case CM_READ_ACCESS:
            handleAccessRead((CM_READ_ACCESS_T *)p_event_data);
        break;

        case CM_WRITE_ACCESS:
            handleAccessWrite((CM_WRITE_ACCESS_T *)p_event_data);
        break;

        default:
        break;

    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      GattInitServerService
 *
 *  DESCRIPTION
 *      This function initialises the gatt service
 *
 *  RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/
extern void GattInitServerService(bool nvm_start_fresh, uint16 *nvm_offset)
{
    g_service_data.temporary_pairing = FALSE;
    
    /* Read data from the NVM */
    readDataFromNVM(nvm_start_fresh, nvm_offset);

    /* Assign the service handler */
    g_service_info.server_handler = g_server_handler;
    g_service_info.start_handle = HANDLE_GATT_SERVICE;
    g_service_info.end_handle = HANDLE_GATT_SERVICE_END;

    /* Register the service with the CM */
    CMServerInitRegisterHandler(&g_service_info);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GattExtInitServerService
 *
 *  DESCRIPTION
 *      This function initialises the gatt service in case temporary pairing is
 *      required.
 *
 *  RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/
extern void GattExtInitServerService(bool nvm_start_fresh, uint16 *nvm_offset,
                                     bool temporary_pairing)
{
    g_service_data.temporary_pairing = temporary_pairing;
    
    if(temporary_pairing)
    {
        g_service_data.bond_id = CM_INVALID_BOND_ID;
        g_service_data.serv_changed_config = gatt_client_config_none;
        g_service_data.serv_changed = FALSE;
    }
    else
    {
        /* Read data from the NVM */
        readDataFromNVM(nvm_start_fresh, nvm_offset);
    }

    /* Assign the service handler */
    g_service_info.server_handler = g_server_handler;
    g_service_info.start_handle = HANDLE_GATT_SERVICE;
    g_service_info.end_handle = HANDLE_GATT_SERVICE_END;

    /* Register the service with the CM */
    CMServerInitRegisterHandler(&g_service_info);
}

#if defined (CSR101x_A05)
/*-----------------------------------------------------------------------------*
 *  NAME
 *      GattOnOtaSwitch
 *
 *  DESCRIPTION
 *      This function should be called when the device is switched into
 *      over-the-air update mode
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void GattOnOtaSwitch(void)
{
    if(g_service_data.serv_changed_config == gatt_client_config_indication)
    {
        /* Record that a Service Changed indication will likely need to be sent
         * to the bonded device next time it connects.
         */
        g_service_data.serv_changed = TRUE;

        if(!g_service_data.temporary_pairing)
        {
            Nvm_Write((uint16*)&(g_service_data.serv_changed),
                      sizeof(g_service_data.serv_changed),
                      (g_service_data.nvm_offset + SERV_CHANGED_SEND_IND_OFFSET));
        }
    }
}
#endif /* CSR101x_A05 */

/*-----------------------------------------------------------------------------*
 *  NAME
 *      GattServiceChangedIndActive
 *
 *  DESCRIPTION
 *      This function allows other modules to read whether the bonded device
 *      has requested indications on the Service Changed characteristic.
 *
 *  RETURNS
 *      True if indications are requested, false otherwise.
 *
 *----------------------------------------------------------------------------*/
extern bool GattServiceChangedIndActive(void)
{
    return (g_service_data.serv_changed_config ==
                gatt_client_config_indication);
}
