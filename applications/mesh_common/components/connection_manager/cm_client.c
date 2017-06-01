/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_client.c
 *
 *  DESCRIPTION
 *      This file defines connection manager client functionality
 *
 *
 ******************************************************************************/
#if defined (CLIENT)

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <gatt.h>
#include <bt_event_types.h>
#include <mem.h>

/*============================================================================*
 *  Local Header File
 *============================================================================*/

#include "cm_client.h"
#include "cm_private.h"
#include "cm_api.h"

 /*============================================================================*
 *  Private Data Types
 *============================================================================*/

/* Discovery procedure data type */
typedef struct
{
    CM_SERVICE_INSTANCE            *cur_serv_instance;

    /* variable which tells the index of the service structure currently being
     * discovered in the g_service_list
     */
    uint16                          service_index;

    /* variable which tells the index of the characteristic structure
     * currently being discovered.
     */
    uint16                          char_index;

    /* Boolean flag which tells if the discovery procedure is ongoing or not */
    bool                            is_discovering;

}DISCOVERY_PROC_T;

/* CM Client data type */
typedef struct
{
    /* Discovery procedure data structure */
    DISCOVERY_PROC_T                disc_info;

    CM_CLIENT_INFO_T                *client_info;

    /* Maximum of client handlers */
    uint16                          max_client_services;

    /* Total Number client handlers registered */
    uint16                          num_reg_services;

    /* Ongoing Write handle */
    uint16                          write_handle;

    /* Ongoing Read handle */
    uint16                          read_handle;

}CM_CLIENT_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

 /* CM Client data */
static CM_CLIENT_DATA_T g_client_data;

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* Notifies the client services and the main application about
 * the discovery complete
 */
static void notifyDiscoveryComplete(device_handle_id device_id,
                              cm_status_code discovery_result);

/* Gets the client instance and handler from the device id */
static bool getInstanceAndHandler(device_handle_id device_id,
                                  uint16 handle,
                                  CM_HANDLERS_T *client_handler,
                                  uint16 *client_instance);

/* Checks if the service instance is free*/
static bool instanceFree(CM_SERVICE_INSTANCE *instance,
                         device_handle_id device_id);

/* Searches for the mandatory service in the discovered services */
static bool continueDiscovery(device_handle_id device_id);

/* Finds the descriptors for characterisitcs of the current service */
static bool moreCharDiscriptors(void);

/* Finds the available characteristics in the services to discover */
static bool moreCharacteristics(device_handle_id device_id);

/* Discovers all the primary services present in a device */
static void discoverAllPrimaryServices(uint16 cid);

/* Discovers all the characteristics of the current service */
static void discoverServiceChar(uint16 cid);

/*  Handles the discovery procedure complete */
static void discoveryComplete(device_handle_id device_id,
                              cm_status_code discovery_result);

/* Handles the signal HAL_GATT_SERV_INFO_IND */
static void handleGenericDiscoverServiceInd(h_gatt_serv_info_ind_t *p_prim);

/* Handles the signal HAL_GATT_DISC_ALL_PRIM_SERV_CFM_T */
static void handleGenericServiceDiscoverAllPrimaryServiceCfm(
                                     h_gatt_disc_all_prim_serv_cfm_t *p_prim);

/* Handles the signal HAL_GATT_CHAR_DECL_INFO_IND */
static void handleGenericGattServiceCharacteristicDeclarationInfoInd(
                                            h_gatt_char_decl_info_ind_t *ind);

/* Handles HAL_GATT_DISC_SERVICE_CHAR_CFM messages. */
static void handleGenericGattDiscoverServiceCharacteristicCfm(
                                        h_gatt_disc_service_char_cfm_t *pInd);

/* Handles HAL_GATT_CHAR_DESC_INFO_IND messages. */
static void handleGenericGattCharacteristicDescriptorInfoInd(
                                        h_gatt_char_desc_info_ind_t *p_prim);

/* Handles HAL_GATT_DISC_ALL_CHAR_DESC_CFM messages. */
static void handleGenericGattCharacteristicDescriptorCfm(
                                    h_gatt_disc_all_char_desc_cfm_t *p_prim);

/* Handles HAL_GATT_WRITE_CHAR_VAL_CFM_T messages. */
static void handleSignalWriteCharValCfm(
                                    h_gatt_write_char_val_cfm_t *p_event_data);

/* Handles HAL_GATT_READ_CHAR_VAL_CFM messages. */
static void handleSignalReadCharValCfm(
                                    h_gatt_read_char_val_cfm_t *p_event_data);

/* Handles HAL_GATT_NOTIFICATION_IND messages. */
static void handleSignalNotificationCharValInd(
                                        h_gatt_char_val_ind_t *p_event_data);

/* Handles HAL_GATT_INDICATION_IND messages. */
static void handleSignalIndicationCharValInd(
                                        h_gatt_char_val_ind_t *p_event_data);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      notifyDiscoveryComplete
 *
 *  DESCRIPTION
 *      Notifies the client services and the main
 *      application about the discovery complete
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void notifyDiscoveryComplete(device_handle_id device_id,
                                 cm_status_code discovery_result)
{
    CM_DISCOVERY_COMPLETE_T cm_discovery_complete;
    CM_SERVICE_T *service_data = NULL;
    uint16 index;
    uint16 conn_index;

    /* Create an event for the higher layers */
    cm_discovery_complete.status = discovery_result;
    cm_discovery_complete.device_id = device_id;

    /* Notify the corresponding client services first */
    for(index = 0; index < g_client_data.num_reg_services; index++ )
    {
        service_data = &g_client_data.client_info[index].service_data;

        /* Go through each instance of the service */
        for(conn_index = 0;
            conn_index < service_data->nInstances;
            conn_index++)
        {
            /* Compare device id and check if the service is found */
            if((service_data->serviceInstances[conn_index]
                .device_id == device_id))
            {
                cm_discovery_complete.instance = conn_index;
                g_client_data.client_info[index].client_handler
                        .pCallback(CM_DISCOVERY_COMPLETE,
                                   (CM_EVENT_T *)&cm_discovery_complete);
            }
        }
    }

    /* Just fill 0xFFFF as not applicable */
    cm_discovery_complete.instance = 0xFFFF;

    /* Now send discovery complete event to the application */
    CMNotifyEventToApplication(CM_DISCOVERY_COMPLETE,
                              (CM_EVENT_T *)&cm_discovery_complete);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      getInstanceAndHandler
 *
 *  DESCRIPTION
 *      Gets the client instance and handler from the device id
 *
 *  RETURNS
 *      TRUE/FALSE
 *
 *---------------------------------------------------------------------------*/
static bool getInstanceAndHandler(device_handle_id device_id,
                                  uint16 handle,
                                  CM_HANDLERS_T *client_handler,
                                  uint16 *client_instance)
{
    CM_SERVICE_T *service_data = NULL;
    uint16 index;
    uint16 conn_index;

    /* Go through all the services */
    for(index = 0; index < g_client_data.num_reg_services; index++ )
    {
        service_data = &g_client_data.client_info[index].service_data;

        /* Go through each instance of the service */
        for(conn_index = 0;
            conn_index < service_data->nInstances;
            conn_index++)
        {
            /* Compare device id and check if the service is found */
            if((service_data->serviceInstances[conn_index]
                .device_id == device_id) &&
               (CMClientCheckHandleRange(
                       &service_data->serviceInstances[conn_index], handle)))
            {
                *client_handler = g_client_data.client_info[index]
                                 .client_handler;
                *client_instance = conn_index;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      instanceFree
 *
 *  DESCRIPTION
 *      Checks if the service instance is free
 *
 *  RETURNS
 *      TRUE if the instance is free
 *
 *---------------------------------------------------------------------------*/
static bool instanceFree(CM_SERVICE_INSTANCE *instance,
                         device_handle_id device_id)
{
    if(instance->device_id == CM_INVALID_DEVICE_ID &&
            instance->bond_id == CM_INVALID_BOND_ID)
    {
        return TRUE;
    }
    else if(instance->bond_id != CM_INVALID_BOND_ID)
    {
        if(instance->bond_id == CMGetBondIdFromDeviceId(device_id))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      continueDiscovery
 *
 *  DESCRIPTION
 *      Checks the whether the discovery to be continued
 *
 *  RETURNS
 *      bool: TRUE if the discovery to be continued
 *
 *---------------------------------------------------------------------------*/

static bool continueDiscovery(device_handle_id device_id)
{
    CM_SERVICE_T *service_data = NULL;
    uint16 index;
    uint16 conn_index;

    uint16 num_mand_serv_required   = 0;
    uint16 num_mand_serv_found      = 0;
    uint16 num_serv_found           = 0;

    /* Go through all the services */
    for(index = 0; index < g_client_data.num_reg_services; index++ )
    {
        service_data = &g_client_data.client_info[index].service_data;

        if(service_data->mandatory == TRUE)
        {
            num_mand_serv_required++;
        }

        /* Go through each instance of the service */
        for(conn_index = 0; conn_index < service_data->nInstances; conn_index++)
        {
            /* Compare device id and check if the service is found */
            if((service_data->serviceInstances[conn_index]
                .device_id == device_id))
            {
                /* increment the services found */
                num_serv_found++;

                if(service_data->mandatory == TRUE)
                {
                    /* Mandatory service found */
                    num_mand_serv_found++;
                }
            }
        }
    }

    /* Check can discovery be continued? */
    if(num_serv_found > 0)
    {
        if ((!num_mand_serv_required) ||
                (num_mand_serv_found > 0))
        {
            /* Either no mandatory services required or
             * one or more mandatory services found
             */
            return TRUE; /* Continue discovery */
        }
    }

    /* Can't continue discovery */
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      moreCharDiscriptors
 *
 *  DESCRIPTION
 *      Finds the descriptors for characterisitcs of the current service
 *
 *  RETURNS
 *      bool: TRUE if descriptors are available
 *
 *---------------------------------------------------------------------------*/

static bool moreCharDiscriptors(void)
{
    uint16 index;

    /* Go through the remaining chracteristics of the service instance */
    for(index = g_client_data.disc_info.char_index + 1;
        index < g_client_data.disc_info.cur_serv_instance->nCharacteristics;
        index++)
    {
        /* Are there any descriptors required for this characteristic */
        if(g_client_data.disc_info.cur_serv_instance->
                characteristics[index].nDescriptors > 0)
        {
            /* Decriptors required for current characteristic */
            g_client_data.disc_info.char_index = index;

            return TRUE;
        }
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      moreCharacteristics
 *
 *  DESCRIPTION
 *      Finds the available characteristics in the services to discover
 *
 *  RETURNS
 *      bool: TRUE if the characteristics are available
 *
 *---------------------------------------------------------------------------*/

static bool moreCharacteristics(device_handle_id device_id)
{
    CM_SERVICE_T *service_data = NULL;
    uint16 index;
    uint16 conn_index;

    /* Go through the remaining services */
    for(index = g_client_data.disc_info.service_index + 1;
            index < g_client_data.num_reg_services; index++ )
    {
        service_data = &g_client_data.client_info[index].service_data;

        /* Go through each instance of the service */
        for(conn_index = 0; conn_index < service_data->nInstances; conn_index++)
        {
            /* Compare with the device id of this service */
            if(service_data->serviceInstances[conn_index]
               .device_id == device_id)
            {
                /* Characteristic discovery required for this service instance */
                g_client_data.disc_info.cur_serv_instance =
                        &service_data->serviceInstances[conn_index];

                /* Save the current service index */
                g_client_data.disc_info.service_index = index;

                return TRUE;
            }
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      discoveryComplete
 *
 *  DESCRIPTION
 *      Handles the discovery procedure complete
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void discoveryComplete(device_handle_id device_id,
                              cm_status_code discovery_result)
{
    /* Service discovery is complete. Set the is_discovering flag to FALSE */
    g_client_data.disc_info.is_discovering = FALSE;
    
    /* Notify the discovery complete */
    notifyDiscoveryComplete(device_id, discovery_result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      discoverAllPrimaryServices
 *
 *  DESCRIPTION
 *      Discovers all the primary services present in a device
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void discoverAllPrimaryServices(uint16 cid)
{
    /* Starts the service discovery */
    GattDiscoverAllPrimaryServices(cid);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      discoverServiceChar
 *
 *  DESCRIPTION
 *      Discovers all the characteristics of the current service
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void discoverServiceChar(uint16 cid)
{
    /* Discover service characteristics */
    GattDiscoverServiceChar(cid,
                        g_client_data.disc_info.cur_serv_instance->start_handle,
                        g_client_data.disc_info.cur_serv_instance->end_handle,
                        GATT_UUID_NONE,
                        NULL);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      discoverCharacteristicDesc
 *
 *  DESCRIPTION
 *      Discovers all the the characteristics descriptors of the current
 *      characteristic
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void discoverCharacteristicDesc(uint16 cid)
{
    CM_CHARACTERISTIC_T *characteristic =
            &g_client_data.disc_info.cur_serv_instance->
            characteristics[g_client_data.disc_info.char_index];

    uint16 end_handle = characteristic->value_handle +
                        characteristic->nDescriptors;

    /* Characteristic discovery is successful and at least one
     * characteristic has descriptor to be discovered
     */
    GattDiscoverAllCharDescriptors(cid,
                       characteristic->value_handle,
                       end_handle);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGenericDiscoverServiceInd
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_SERV_INFO_IND_T. It copies the start and end
 *      handles of the discovered service if they are present in the service
 *      list registered by the client services
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleGenericDiscoverServiceInd(h_gatt_serv_info_ind_t *p_prim)
{
    CM_SERVICE_T *service_data = NULL;
    CM_UUID_T uuid_in = {p_prim->uuid_type, p_prim->uuid};
    CM_UUID_T uuid_temp;
    uint16 index;
    uint16 conn_index;

    for(index = 0; index < g_client_data.num_reg_services; index++ )
    {
        service_data = &g_client_data.client_info[index].service_data;

        uuid_temp.uuid_type = g_client_data.client_info[index].service_data
                              .uuid_type;
        uuid_temp.uuid = g_client_data.client_info[index].service_data.uuid;

        if(CMUuidEqual(&uuid_in, &uuid_temp))
        {
            /* We have found the supported service in the peer device.
             * Now check where the dev disc data needs to be stored
             */
            for(conn_index = 0;
                conn_index < service_data->nInstances;
                conn_index++)
            {
                device_handle_id device_id =
                        CMGetDeviceId(HAL_GATT_SERV_INFO_IND, (void*)p_prim);

                if(instanceFree(
                        &service_data->serviceInstances[conn_index],device_id))
                {
                    service_data->serviceInstances[conn_index].end_handle =
                           p_prim->end_handle;

                    service_data->serviceInstances[conn_index].device_id =
                            device_id;

                    HALParseDiscoverServiceInd(
                            &service_data->serviceInstances[conn_index],
                            p_prim);

                    break;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGenericServiceDiscoverAllPrimaryServiceCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_DISC_ALL_PRIM_SERV_CFM_T
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleGenericServiceDiscoverAllPrimaryServiceCfm(
                                        h_gatt_disc_all_prim_serv_cfm_t *p_prim)
{
    /* If any mandatory service is found then go ahead with the char discovery
       of first service else send an error response with discovery fail as
       mandatory service not supported. The failure may also be because of
       no storage space in the service for storing device information.
     */

    CM_SERVICE_T *service_data = NULL;
    cm_status_code discovery_result = cm_status_success;
    uint16 index;
    uint16 conn_index;
    device_handle_id device_id = CMGetDeviceId(HAL_GATT_DISC_ALL_PRIM_SERV_CFM,
                                     (void*)p_prim);

    if(p_prim->result == sys_status_success)
    {
        /* Primary services discovery successful */

        if(continueDiscovery(device_id))
        {
            /* Start characteristic discovery from first discovered service */
            for(index = 0; index < g_client_data.num_reg_services; index++ )
            {
                service_data = &g_client_data.client_info[index].service_data;
                bool exitLoop = FALSE;

                for(conn_index = 0;
                    conn_index < service_data->nInstances;
                    conn_index++)
                {
                    if((service_data->serviceInstances[conn_index].device_id
                        == device_id))
                    {
                       /* Save the current instance and its index */
                       g_client_data.disc_info.cur_serv_instance =
                                &service_data->serviceInstances[conn_index];

                       g_client_data.disc_info.service_index = index;
                       exitLoop = TRUE;
                       break;
                    }
                }
                if(exitLoop)
                    break;
            }
        }
        else
        {
            /* Mandatory service not found */
            discovery_result = cm_status_mandatory_serv_not_found;
        }
    }
    else
    {
        /* Primary service discovery failed */
        discovery_result = cm_status_disc_primary_serv_failed;
    }

    if(discovery_result == cm_status_success)
    {
         /* Continue discovering service characteristics */
        discoverServiceChar(p_prim->cid);
    }
    else
    {
        discoveryComplete(device_id, discovery_result);
    }

}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGenericGattServiceCharacteristicDeclarationInfoInd
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_CHAR_DECL_INFO_IND
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleGenericGattServiceCharacteristicDeclarationInfoInd(
                                                h_gatt_char_decl_info_ind_t *ind)
{
    CM_UUID_T uuid_in = {ind->uuid_type, ind->uuid};
    CM_UUID_T uuid_temp;
    uint16 index;

    /* Go through all the characteristics of the current service instance */
    for(index = 0; index <
        g_client_data.disc_info.cur_serv_instance->nCharacteristics; index++ )
    {
        uuid_temp.uuid_type = g_client_data.disc_info.
                          cur_serv_instance->characteristics[index].uuid_type;
        uuid_temp.uuid = g_client_data.disc_info.
                          cur_serv_instance->characteristics[index].uuid;

        if(CMUuidEqual(&uuid_in, &uuid_temp))
        {
            /* Interested characteristic found */
            g_client_data.disc_info.cur_serv_instance->
                    characteristics[index].value_handle = ind->val_handle;

            g_client_data.disc_info.cur_serv_instance->
                    characteristics[index].properties = ind->prop;

            break;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGenericGattDiscoverServiceCharacteristicCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_DISC_SERVICE_CHAR_CFM
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

static void handleGenericGattDiscoverServiceCharacteristicCfm(
                                          h_gatt_disc_service_char_cfm_t *pInd)
{
    /* Characteristics discovery failed */
    device_handle_id device_id = CMGetDeviceId(HAL_GATT_DISC_SERVICE_CHAR_CFM,
                                     (void*)pInd);

    /* Check for if the device id is valid */
    if(device_id == CM_INVALID_DEVICE_ID)
        return;

    /* Initialise the current characteristic index */
    g_client_data.disc_info.char_index = 0xFFFF;

    if(moreCharDiscriptors())
    {
        /* Descriptors to be discovered */
        discoverCharacteristicDesc(pInd->cid);
    }
    else if(moreCharacteristics(device_id))
    {
        /* Continue discovery of characteristics of next discovered service */
        discoverServiceChar(pInd->cid);
    }
    else
    {
        /* Discovery procedure completed */
        discoveryComplete(device_id, cm_status_success);
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGenericGattCharacteristicDescriptorInfoInd
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_CHAR_DESC_INFO_IND
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleGenericGattCharacteristicDescriptorInfoInd(
                                        h_gatt_char_desc_info_ind_t *p_prim)
{
    CM_CHARACTERISTIC_T *characteristic =
            &g_client_data.disc_info.cur_serv_instance->
                    characteristics[g_client_data.disc_info.char_index];
    uint16 index;

    /* Go through all the descriptors of the current characteristic */
    for(index = 0; index < characteristic->nDescriptors; index++)
    {
        if(characteristic->descriptors[index].uuid_type == p_prim->uuid_type)
        {
            if(characteristic->descriptors[index].uuid == p_prim->uuid[0])
            {
                /* Interested descriptor found */
                characteristic->descriptors[index].desc_handle =
                        p_prim->desc_handle;
                break;
            }
        }
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGenericGattCharacteristicDescriptorCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_DISC_ALL_CHAR_DESC_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleGenericGattCharacteristicDescriptorCfm(
                                        h_gatt_disc_all_char_desc_cfm_t *p_prim)
{
    /* Characteristics discovery failed */
    device_handle_id device_id = CMGetDeviceId(HAL_GATT_DISC_ALL_CHAR_DESC_CFM,
                                     (void*)p_prim);

    /* Check for if the device id is valid */
    if(device_id == CM_INVALID_DEVICE_ID)
        return;

    if(moreCharDiscriptors())
    {
        /* Descriptors to be discovered */
        discoverCharacteristicDesc(p_prim->cid);
    }
    else if(moreCharacteristics(device_id))
    {
        /* Continue discovery of characteristics of next discovered service */
        discoverServiceChar(p_prim->cid);
    }
    else
    {
        /* Discovery procedure completed */
        discoveryComplete(device_id, cm_status_success);
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalWriteCharValCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_WRITE_CHAR_VAL_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleSignalWriteCharValCfm(
                                    h_gatt_write_char_val_cfm_t *p_event_data)
{
    CM_WRITE_CFM_T write_cfm;
    CM_HANDLERS_T client_handler;
    uint16 client_instance;

    write_cfm.device_id = CMGetDeviceId(HAL_GATT_WRITE_CHAR_VAL_CFM,
                                        (void*)p_event_data);

    /* Check for if the device id is valid */
    if(write_cfm.device_id == CM_INVALID_DEVICE_ID)
    {
        /* Clear the pending write handle */
        g_client_data.write_handle = CM_INVALID_ATT_HANDLE;
        return;
    }

    /* Write the write confirmation handle */
    write_cfm.handle = g_client_data.write_handle;
    
    /* Clear the pending write handle */
    g_client_data.write_handle = CM_INVALID_ATT_HANDLE;
    
    /* Get the client instance handler */
    if(!getInstanceAndHandler(write_cfm.device_id,
                              write_cfm.handle,
                              &client_handler,
                              &client_instance))
    {
        /* No client avaialble */
        return;
    }

    /* Create the write confirmation event */    
    write_cfm.status = p_event_data->result;
    write_cfm.instance = client_instance;

    /* Notify clients */
    CMClientNotifyGattEvent(CM_WRITE_CFM,
                            (CM_EVENT_T *)&write_cfm,
                            &client_handler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalReadCharValCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_READ_CHAR_VAL_CFM
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void handleSignalReadCharValCfm(
                                    h_gatt_read_char_val_cfm_t *p_event_data)
{
    CM_READ_CFM_T read_cfm;
    CM_HANDLERS_T client_handler;
    uint16 client_instance;

    read_cfm.device_id = CMGetDeviceId(HAL_GATT_READ_CHAR_VAL_CFM,
                                        (void*)p_event_data);

    /* Check for if the device id is valid */
    if(read_cfm.device_id == CM_INVALID_DEVICE_ID)
    {
        /* Clear the pending write handle */
        g_client_data.read_handle = CM_INVALID_ATT_HANDLE;
        return;
    }

    /* Write the read confirmation handle */
    read_cfm.handle = g_client_data.read_handle;
        
    /* Clear the pending write handle */
    g_client_data.read_handle = CM_INVALID_ATT_HANDLE;
    
    /* Get the client instance handler */
    if(!getInstanceAndHandler(read_cfm.device_id,
                              read_cfm.handle,
                              &client_handler,
                              &client_instance))
    {
        /* No client avaialble */
        return;
    }

    /* Parse read confirmation*/
    HALParseReadCharValCfm(&read_cfm, p_event_data);
    read_cfm.instance = client_instance;

    /* Notify clients */
    CMClientNotifyGattEvent(CM_READ_CFM,
                            (CM_EVENT_T *)&read_cfm,
                            &client_handler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalNotificationCharValInd
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_NOTIFICATION_IND
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

static void handleSignalNotificationCharValInd(
                                            h_gatt_char_val_ind_t *p_event_data)
{
    CM_NOTIFICATION_T notif_ind;
    CM_HANDLERS_T client_handler;
    uint16 client_instance;

    notif_ind.handle        = p_event_data->handle;
    notif_ind.device_id     = CMGetDeviceId(HAL_GATT_NOTIFICATION_IND,
                                            (void*)p_event_data);

    /* Check for if the device id is valid */
    if(notif_ind.device_id == CM_INVALID_DEVICE_ID)
        return;

    /* Get the client instance handler */
    if(!getInstanceAndHandler(notif_ind.device_id,
                              p_event_data->handle,
                              &client_handler,
                              &client_instance))
    {
        /* No client avaialble */
        return;
    }

    /* Parse the Notification */
    HALParseNotifInd(&notif_ind, p_event_data);
    notif_ind.instance = client_instance;

    CMClientNotifyGattEvent(CM_NOTIFICATION,
                            (CM_EVENT_T *)&notif_ind,
                            &client_handler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalIndicationCharValInd
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_INDICATION_IND
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

static void handleSignalIndicationCharValInd(
                                            h_gatt_char_val_ind_t *p_event_data)
{
    CM_INDICATION_T indication_ind;
    CM_HANDLERS_T client_handler;
    uint16 client_instance;

    indication_ind.handle       = p_event_data->handle;
    indication_ind.device_id    = CMGetDeviceId(HAL_GATT_NOTIFICATION_IND,
                                            (void*)p_event_data);

    /* Check for if the device id is valid */
    if(indication_ind.device_id == CM_INVALID_DEVICE_ID)
        return;

    /* Get the client instance handler */
    if(!getInstanceAndHandler(indication_ind.device_id,
                              p_event_data->handle,
                              &client_handler,
                              &client_instance))
    {
        /* No client avaialble */
        return;
    }

    HALParseNotifInd(&indication_ind, p_event_data);
    indication_ind.instance = client_instance;

    CMClientNotifyGattEvent(CM_INDICATION,
                            (CM_EVENT_T *)&indication_ind,
                            &client_handler);
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientInit
 *
 *  DESCRIPTION
 *     Initialises the CM client entity
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMClientInit(CM_INIT_PARAMS_T *cm_init_params)
{
    /* Install GATT Client functionality */
    GattInstallClientRole();

    /* Save the client service pointer */
    g_client_data.client_info = cm_init_params->client_info;
    g_client_data.max_client_services = cm_init_params->max_client_services;
    g_client_data.num_reg_services = 0;
    g_client_data.write_handle = CM_INVALID_ATT_HANDLE;
    g_client_data.read_handle = CM_INVALID_ATT_HANDLE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientInitRegisterHandler
 *
 *  DESCRIPTION
 *      Registers Client Information
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMClientInitRegisterHandler(CM_CLIENT_INFO_T *cm_client_info)
{
    if(g_client_data.num_reg_services < g_client_data.max_client_services)
    {
        /* Add the client service onto the service list. */
        g_client_data.client_info[g_client_data.num_reg_services]
                = *cm_client_info;

        g_client_data.num_reg_services++;
    }
    else
    {
        /* Panic as the clients being registered are exceeding the max
           allocated.
         */
        CMReportPanic(cm_panic_client_service_size_exceeded);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientNotifyGenericEvent
 *
 *  DESCRIPTION
 *      Sends the generic event to all the registered clients
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMClientNotifyGenericEvent(cm_event event_type,
                                CM_EVENT_T *client_event)
{
    uint16 index;

    for(index = 0; index < g_client_data.num_reg_services; index++)
    {
       if(g_client_data.client_info[index].client_handler.pCallback == NULL)
       {
           /* Ignore NULL Callback */
           continue;
       }

       /* Notify Clients */
       g_client_data.client_info[index].client_handler
               .pCallback(event_type, client_event);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientNotifyGattEvent
 *
 *  DESCRIPTION
 *      Sends the gatt event to the right client handler
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMClientNotifyGattEvent(cm_event event_type,
                                CM_EVENT_T *client_event,
                                CM_HANDLERS_T *client_handler)
{
    if((client_handler != NULL) && (client_handler->pCallback != NULL))
    {
        client_handler->pCallback(event_type, client_event);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientWriteRequest
 *
 *  DESCRIPTION
 *      Sends the write request to the remote device
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMClientWriteRequest(device_handle_id device_id,
                                           uint16 req_type, uint16 handle,
                                           uint16 length, uint8* data)
{
    if(g_client_data.write_handle != CM_INVALID_ATT_HANDLE  ||
       g_client_data.read_handle != CM_INVALID_ATT_HANDLE)
        return cm_status_busy;

    /* Save the current write handle */
    g_client_data.write_handle = handle;

    /* Write the Characteristic value */
    sys_status status = GattWriteCharValueReq(CMGetConnId(device_id),
                          req_type,
                          handle,
                          length,
                          data);

    return status;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientReadRequest
 *
 *  DESCRIPTION
 *      Sends the read request to the remote device
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMClientReadRequest(device_handle_id device_id,
                                          uint16 handle)
{
    if(g_client_data.read_handle != CM_INVALID_ATT_HANDLE ||
       g_client_data.write_handle != CM_INVALID_ATT_HANDLE )
        return cm_status_busy;

    /* Save the current read handle */
    g_client_data.read_handle = handle;

    /* Read the Characteristic value */
    GattReadCharValue(CMGetConnId(device_id),
                      handle);

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientStartDiscovery
 *
 *  DESCRIPTION
 *      Starts the primary services discovery procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *----------------------------------------------------------------------------*/
extern cm_status_code CMClientStartDiscovery(device_handle_id device_id)
{
    /* Discovery already in progress return an error */
    if(g_client_data.disc_info.is_discovering == TRUE)
    {
        return cm_status_discovery_in_progress;
    }

    /* This discovery procedure discovers all the primary services listed
     * in the array discovery_array. Start with the service at index 0.
     */
    g_client_data.disc_info.service_index = 0;
    g_client_data.disc_info.is_discovering = TRUE;
    g_client_data.disc_info.cur_serv_instance = NULL;

    /* Start GATT discovery procedure */
    discoverAllPrimaryServices(CMGetConnId(device_id));

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientFindDevice
 *
 *  DESCRIPTION
 *      This function finds the connected device in the client service
 *
 *  RETURNS
 *      TRUE/FALSE
 *----------------------------------------------------------------------------*/

extern int8 CMClientFindDevice(CM_SERVICE_T *client_service,
                                  device_handle_id device_id)
{
    int8 index;
    for(index = 0; index < (int8)client_service->nInstances ; index++)
    {
        if(client_service->serviceInstances[index].device_id == device_id)
        {
            return index;
        }
    }
    return -1;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientCheckHandleRange
 *
 *  DESCRIPTION
 *      This function checks whether the handle falls in the handle range of
 *      specific service instance
 *
 *  RETURNS
 *      TRUE .if the handle falls in the client service range.
 *----------------------------------------------------------------------------*/

extern bool CMClientCheckHandleRange(CM_SERVICE_INSTANCE *instance,
                                        uint16 handle)
{
    if(handle >= instance->start_handle &&
       handle <= instance->end_handle)
    {
        return TRUE;
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMIsClientBusy
 *
 *  DESCRIPTION
 *      Checks if client is busy in read/write operations
 *
 *  RETURNS
 *      TRUE/FALSE
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code CMIsClientBusy()
{
    if(g_client_data.read_handle != CM_INVALID_ATT_HANDLE ||
       g_client_data.write_handle != CM_INVALID_ATT_HANDLE)
        return TRUE;
    else
        return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMClientHandleProcessLMEvent
 *
 *  DESCRIPTION
 *      Handles the firmware events related to the client role
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMClientHandleProcessLMEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
        case HAL_GATT_SERV_INFO_IND:
        {
            /* Primary Service Discovered Indication */
            handleGenericDiscoverServiceInd(
                    (h_gatt_serv_info_ind_t*)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_DISC_ALL_PRIM_SERV_CFM:
        {
            /* Primary Service Discovery Complete Indication */
            handleGenericServiceDiscoverAllPrimaryServiceCfm(
                    (h_gatt_disc_all_prim_serv_cfm_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_CHAR_DECL_INFO_IND:
        {
            /* Characteristic Discovered Indication */
            handleGenericGattServiceCharacteristicDeclarationInfoInd(
                    (h_gatt_char_decl_info_ind_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_DISC_SERVICE_CHAR_CFM:
        {
            /* Characteristic Discovery Complete Indication */
            handleGenericGattDiscoverServiceCharacteristicCfm(
                    (h_gatt_disc_service_char_cfm_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_CHAR_DESC_INFO_IND:
        {
            /* Characteristic Descriptor Discovered Indication */
            handleGenericGattCharacteristicDescriptorInfoInd(
                    (h_gatt_char_desc_info_ind_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_DISC_ALL_CHAR_DESC_CFM:
        {
            /* Characteristic Descriptor Discovery Complete Indication */
            handleGenericGattCharacteristicDescriptorCfm(
                    (h_gatt_disc_all_char_desc_cfm_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_WRITE_CHAR_VAL_CFM:
        {
            /* Write Confirmation */
            handleSignalWriteCharValCfm(
                    (h_gatt_write_char_val_cfm_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_READ_CHAR_VAL_CFM:
        {
            /* Read Confirmation */
            handleSignalReadCharValCfm(
                    (h_gatt_read_char_val_cfm_t *)HALGetMsg(msg));
        }
        break;
        case HAL_GATT_NOTIFICATION_IND:
        {
            /* Notificaiton */
            handleSignalNotificationCharValInd(
                    (h_gatt_char_val_ind_t *)HALGetMsg(msg));
        }
        break;

        case HAL_GATT_INDICATION_IND:
        {
            /* Indication */
            handleSignalIndicationCharValInd(
                    (h_gatt_char_val_ind_t *)HALGetMsg(msg));
        }
        break;
        default:
        break;
    }
}

#endif /* CLIENT */
