/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gaia_client_service.c
 *
 *  DESCRIPTION
 *      This file contains the GAIA Client Service implementation.
 *
 ******************************************************************************/

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <types.h>
#include <gatt.h>
#include <gatt_uuid.h>
#include <bt_event_types.h>
#include <mem.h>
#include <buf_utils.h>
#include <timer.h>
/*============================================================================*
 *  Local Header File
 *============================================================================*/
#include "cm_types.h"
#include "cm_api.h"
#include "nvm_access.h"
#include "gaia_uuids.h"
#include "gaia_client_service.h"
#include "gaia_otau_client_private.h"
#include "gaia_otau_client_api.h"
#include "gaia_client.h"
#include "user_config.h"

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

#define GAIA_CLIENT_SERVICE_INSTANCES                      (1)
/* Number of characters defined in the heart rate client service */
#define GAIA_NUM_CHARACTERS                                (3)

/* Maximum number of descriptors */
#define GAIA_NUM_DESCRIPTORS                               (1)

/* Characteristics Indexes */
#define COMMAND_CHAR                                       (0)
#define RESPONSE_CHAR                                      (1)
#define DATA_CHAR                                          (2)

/* Client Config Descriptor Indexes */
#define RESPONSE_CCD                                       (0)

/* Configuration states */
#define STATE_INIT                                         (0)

/*============================================================================*
 *  Private Data Type
 *============================================================================*/

/* Gaia Client Data */
typedef struct
{
   uint16      cur_state;

   device_handle_id device_id;
   
   cm_status_code  gaia_status;
   
   uint8 pending_data[20];
   
   uint16 pending_length;
   
   timer_id     conn_tid;

}GAIA_CLIENT_DATA_T;

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function initialises descriptor data */
static void initialiseDescriptor(CM_DESCRIPTOR_T *descriptor);

/* This function initialises the characteristics data */
static void initialiseCharacteristics(CM_CHARACTERISTIC_T
                                      *characteristics, uint16 instance);

/* This function initialises the client service data */
static void gaiaClientDataInit(void);

/* This function handles the connection notification */
static void handleCMClientConnectionNotify(
                                CM_CONNECTION_NOTIFY_T *p_event_data);

/* This function handles the HR Measurement notification */
static void handleCMClientNotification(
                                CM_NOTIFICATION_T *p_event_data);

/* This function handles the events from the connection manager */
static void gaiaClientHandleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data);

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* GAIA service uuid */
static uint16 g_gaia_client_service_uuid[] = {UUID_GAIA_SERVICE_1,
                                              UUID_GAIA_SERVICE_2,
                                              UUID_GAIA_SERVICE_3,
                                              UUID_GAIA_SERVICE_4,
                                              UUID_GAIA_SERVICE_5,
                                              UUID_GAIA_SERVICE_6,
                                              UUID_GAIA_SERVICE_7,
                                              UUID_GAIA_SERVICE_8};

/* GAIA command endpoint characteristic uuid */
static uint16 g_gaia_command_char_uuid[8];

/* GAIA response endpoint characteristic uuid */
static uint16 g_gaia_response_char_uuid[8];

/* GAIA data endpoint characteristic uuid */
static uint16 g_gaia_data_char_uuid[8];

/* GAIA descriptors */
static CM_DESCRIPTOR_T
        g_descriptors[GAIA_CLIENT_SERVICE_INSTANCES][GAIA_NUM_DESCRIPTORS];

/* GAIA characteristics */
static CM_CHARACTERISTIC_T
        g_characteristics[GAIA_CLIENT_SERVICE_INSTANCES][GAIA_NUM_CHARACTERS];

/* GAIA Service instance */
static CM_SERVICE_INSTANCE
        g_service_instances[GAIA_CLIENT_SERVICE_INSTANCES];

/* GAIA Client service */
static CM_SERVICE_T            g_gaia_client_service;

/* GAIA Client information */
static CM_CLIENT_INFO_T        g_gaia_client_service_info;

/* GAIA Client data */
static GAIA_CLIENT_DATA_T        g_gaia_client_data;

/* Handler */
static CM_HANDLERS_T g_gaia_client_handler =
{
    .pCallback = &gaiaClientHandleConnMgrProcedureEvent
};

/*============================================================================*
 *  Private Function Implementation
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      initialiseDescriptor
 *
 *  DESCRIPTION
 *      This function initialises descriptor data
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void initialiseDescriptor(CM_DESCRIPTOR_T *descriptor)
{
    descriptor->uuid_type     = GATT_UUID16;
    descriptor->uuid          = UUID_CLIENT_CHAR_CFG;
    descriptor->desc_handle   = CM_INVALID_ATT_HANDLE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      initialiseCharacteristics
 *
 *  DESCRIPTION
 *      This function initialises the characteristics data
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void initialiseCharacteristics(CM_CHARACTERISTIC_T
                                      *characteristics, uint16 instance)
{
    uint16 index;
    for(index = 0; index < GAIA_NUM_CHARACTERS; index++)
    {
        characteristics[index].uuid_type    = GATT_UUID128;
        characteristics[index].value_handle = CM_INVALID_ATT_HANDLE;
        characteristics[index].properties   = 0;
        characteristics[index].nDescriptors = 0;
        characteristics[index].descriptors  = NULL;
    }

    characteristics[COMMAND_CHAR].uuid = &g_gaia_command_char_uuid[instance];
    characteristics[COMMAND_CHAR].uuid[0] = UUID_GAIA_COMMAND_ENDPOINT_1;
    characteristics[COMMAND_CHAR].uuid[1] = UUID_GAIA_COMMAND_ENDPOINT_2;
    characteristics[COMMAND_CHAR].uuid[2] = UUID_GAIA_COMMAND_ENDPOINT_3;
    characteristics[COMMAND_CHAR].uuid[3] = UUID_GAIA_COMMAND_ENDPOINT_4;
    characteristics[COMMAND_CHAR].uuid[4] = UUID_GAIA_COMMAND_ENDPOINT_5;
    characteristics[COMMAND_CHAR].uuid[5] = UUID_GAIA_COMMAND_ENDPOINT_6;
    characteristics[COMMAND_CHAR].uuid[6] = UUID_GAIA_COMMAND_ENDPOINT_7;
    characteristics[COMMAND_CHAR].uuid[7] = UUID_GAIA_COMMAND_ENDPOINT_8;

    characteristics[RESPONSE_CHAR].uuid = &g_gaia_response_char_uuid[instance];
    characteristics[RESPONSE_CHAR].uuid[0] = UUID_GAIA_RESPONSE_ENDPOINT_1;
    characteristics[RESPONSE_CHAR].uuid[1] = UUID_GAIA_RESPONSE_ENDPOINT_2;
    characteristics[RESPONSE_CHAR].uuid[2] = UUID_GAIA_RESPONSE_ENDPOINT_3;
    characteristics[RESPONSE_CHAR].uuid[3] = UUID_GAIA_RESPONSE_ENDPOINT_4;
    characteristics[RESPONSE_CHAR].uuid[4] = UUID_GAIA_RESPONSE_ENDPOINT_5;
    characteristics[RESPONSE_CHAR].uuid[5] = UUID_GAIA_RESPONSE_ENDPOINT_6;
    characteristics[RESPONSE_CHAR].uuid[6] = UUID_GAIA_RESPONSE_ENDPOINT_7;
    characteristics[RESPONSE_CHAR].uuid[7] = UUID_GAIA_RESPONSE_ENDPOINT_8;
    characteristics[RESPONSE_CHAR].nDescriptors = 1;
    characteristics[RESPONSE_CHAR].descriptors  = &g_descriptors[instance][0];
    initialiseDescriptor(characteristics[RESPONSE_CHAR].descriptors);

    characteristics[DATA_CHAR].uuid = &g_gaia_data_char_uuid[instance];
    characteristics[DATA_CHAR].uuid[0] = UUID_GAIA_DATA_ENDPOINT_1;
    characteristics[DATA_CHAR].uuid[1] = UUID_GAIA_DATA_ENDPOINT_2;
    characteristics[DATA_CHAR].uuid[2] = UUID_GAIA_DATA_ENDPOINT_3;
    characteristics[DATA_CHAR].uuid[3] = UUID_GAIA_DATA_ENDPOINT_4;
    characteristics[DATA_CHAR].uuid[4] = UUID_GAIA_DATA_ENDPOINT_5;
    characteristics[DATA_CHAR].uuid[5] = UUID_GAIA_DATA_ENDPOINT_6;
    characteristics[DATA_CHAR].uuid[6] = UUID_GAIA_DATA_ENDPOINT_7;
    characteristics[DATA_CHAR].uuid[7] = UUID_GAIA_DATA_ENDPOINT_8;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      gaiaClientDataInit
 *
 *  DESCRIPTION
 *      This function initialises the client service data
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void gaiaClientDataInit(void)
{
    uint16 index;

    g_gaia_client_service.uuid_type = GATT_UUID128;
    g_gaia_client_service.uuid = g_gaia_client_service_uuid;
    g_gaia_client_service.mandatory = TRUE;
    g_gaia_client_service.nInstances = GAIA_CLIENT_SERVICE_INSTANCES;
    g_gaia_client_service.serviceInstances = g_service_instances;

    for(index = 0; index < GAIA_CLIENT_SERVICE_INSTANCES; index++)
    {
        g_service_instances[index].start_handle     =  CM_INVALID_ATT_HANDLE;
        g_service_instances[index].end_handle       =  CM_INVALID_ATT_HANDLE;
        g_service_instances[index].device_id        =  CM_INVALID_DEVICE_ID;
        g_service_instances[index].bond_id          =  CM_INVALID_BOND_ID;
        g_service_instances[index].nCharacteristics =  GAIA_NUM_CHARACTERS;
        g_service_instances[index].characteristics  = g_characteristics[index];

        initialiseCharacteristics(g_service_instances[index].characteristics,
                                  index);
    }

    /* Intialise the current state */
    g_gaia_client_data.cur_state = STATE_INIT;
    
    /* Intialise the timer */
    TimerDelete(g_gaia_client_data.conn_tid);
    g_gaia_client_data.conn_tid = TIMER_INVALID;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCMClientConnectionNotify
 *
 *  DESCRIPTION
 *      This function handles the connection notify message received from CM.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleCMClientConnectionNotify(CM_CONNECTION_NOTIFY_T *p_event_data)
{
    if(CMGetPeerDeviceRole(p_event_data->device_id) == con_role_peripheral)
    {
    if(p_event_data->result == cm_conn_res_success)
    {
        g_gaia_client_data.device_id = p_event_data->device_id;

    }
    else if(p_event_data->result == cm_disconn_res_success)
    {
        g_gaia_client_data.device_id = 0xFFFF;
        
         TimerDelete(g_gaia_client_data.conn_tid);
         g_gaia_client_data.conn_tid = TIMER_INVALID;
        
        /* Get the service instance index */
        int8 instance_index =
                CMClientFindDevice(&g_gaia_client_service,
                                   p_event_data->device_id);
        if(instance_index >= 0)
        {
            /* On disconnection remove the device
             * from the service instance
             */
            g_service_instances[instance_index].device_id = CM_INVALID_DEVICE_ID;
        }
    }
}
    GaiaClientOtauHandleConnNotify(p_event_data);

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCMClientNotification
 *
 *  DESCRIPTION
 *      This function handles the notification message received from CM.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleCMClientNotification(CM_NOTIFICATION_T *p_event_data)
{
    CM_SERVICE_INSTANCE *instance =
            &g_service_instances[p_event_data->instance];

    if(p_event_data->handle ==
       instance->characteristics[RESPONSE_CHAR].value_handle)
    {
        /*  Parse the GAIA Notification */
        GAIAClientProcessCommand(p_event_data->length,p_event_data->data);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleTimerExpiry
 *
 *  DESCRIPTION
 *      This function handles the start upgrade timer expiry
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleTimerExpiry(timer_id tid)
{
    if(g_gaia_client_data.conn_tid == tid)
    {
         StartUpgrade();
     }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCMClientWriteCfm
 *
 *  DESCRIPTION
 *      This function handles the write confirm message received from CM.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleCMClientWriteCfm(CM_WRITE_CFM_T *p_event_data)
{
    CM_SERVICE_INSTANCE *instance =
            &g_service_instances[p_event_data->instance];

    if(p_event_data->handle ==
           instance->characteristics[RESPONSE_CHAR].
                        descriptors[RESPONSE_CCD].desc_handle)
    {
        DEBUG_STR("\r\nGAIA Notifications Enabled");
        g_gaia_client_data.conn_tid = TimerCreate(CONN_INTERVAL,TRUE,handleTimerExpiry);
    }
    else if(p_event_data->handle ==
           instance->characteristics[COMMAND_CHAR].value_handle)
    {
        if(g_gaia_client_data.gaia_status == cm_status_busy)
        {
            GaiaWriteCommandEndpoint(g_gaia_client_data.pending_length,
                                     g_gaia_client_data.pending_data);
        }           

    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      gaiaClientHandleConnMgrProcedureEvent
 *
 *  DESCRIPTION
 *      This function handles the events from the connection manager.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void gaiaClientHandleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data)
{
    switch(event_type)
    {
        case CM_CONNECTION_NOTIFY:
            handleCMClientConnectionNotify(
                            (CM_CONNECTION_NOTIFY_T *)p_event_data);
        break;

        case CM_WRITE_CFM:
            handleCMClientWriteCfm(
                            (CM_WRITE_CFM_T *)p_event_data);
        break;

        case CM_NOTIFICATION:
            handleCMClientNotification(
                            (CM_NOTIFICATION_T *)p_event_data);
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
 *      GaiaInitClientService
 *
 *  DESCRIPTION
 *      This function intializes the Gaia Client Service. This function
 *      should be called during the application initialization.
 *
 *  RETURNS
 *      None
 *----------------------------------------------------------------------------*/
extern void GaiaInitClientService(bool nvm_start_fresh, uint16 *nvm_offset)
{
    gaiaClientDataInit();

#ifdef GAIA_OTAU_SUPPORT
    /* GAIA client support for the Over The Air Upgrade support */
    GaiaClientOtauInit(nvm_start_fresh, nvm_offset);
#endif
    /* Register the callback handler with CM with the callback function handler
       and the client structure to be filled during discovery procedure
     */

    g_gaia_client_service_info.client_handler = g_gaia_client_handler;
    g_gaia_client_service_info.service_data = g_gaia_client_service;
    CMClientInitRegisterHandler(&g_gaia_client_service_info);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaClientIsConnected
 *
 *  DESCRIPTION
 *      This function checks if the gaia client service is connected or not
 *
 *  RETURNS
 *      TRUE if connected
 *----------------------------------------------------------------------------*/
extern bool GaiaClientIsConnected(device_handle_id *device_id)
{
    bool connected = FALSE;

    if(g_service_instances[0].device_id != CM_INVALID_DEVICE_ID)
    {
       connected = TRUE;
    }

    if(connected && device_id != NULL)
    {
        *device_id = g_service_instances[0].device_id;
    }

    return connected;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaConfigureResponseConfig
 *
 *  DESCRIPTION
 *      This function configures the Gaia Response Endpoint CCD.
 *
 *  RETURNS
 *      TRUE if the initiation of the configuration procedure was successful
 *      FALSE if the remote device does not support the configuration
 *----------------------------------------------------------------------------*/
extern bool GaiaConfigureResponseConfig(device_handle_id device_id)
{
   int8 instance_index = CMClientFindDevice(&g_gaia_client_service, device_id);

   if(instance_index >= 0)
   {
        CM_SERVICE_INSTANCE *instance = &g_service_instances[instance_index];
        uint16 handle = instance->characteristics[RESPONSE_CHAR].
                        descriptors[RESPONSE_CCD].desc_handle;

        if(handle != CM_INVALID_ATT_HANDLE)
        {
            uint8 val[2], *p_val;
            p_val = val;
            BufWriteUint16(&p_val, gatt_client_config_notification);

            DEBUG_STR("\r\nCofiguring GAIA Notifications");
            cm_status_code status = CMClientWriteRequest(device_id,
                                 GATT_WRITE_REQUEST,
                                 handle,
                                 2,
                                 val);
            if(status == cm_status_busy)
            {
                DEBUG_STR("\r\nCM Busy");
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaWriteCommandEndpoint
 *
 *  DESCRIPTION
 *      This function writes the GAIA command endpoint
 *
 *  RETURNS
 *      TRUE if the initiation of the write procedure was successful
 *      FALSE if the remote device does not support the configuration
 *----------------------------------------------------------------------------*/
extern bool GaiaWriteCommandEndpoint(uint16 len,
                                     uint8* value)
{
   int8 instance_index = CMClientFindDevice(&g_gaia_client_service, 
                                            g_gaia_client_data.device_id);
   if(instance_index >= 0)
   {
        CM_SERVICE_INSTANCE *instance = &g_service_instances[instance_index];
        uint16 handle = instance->characteristics[COMMAND_CHAR].value_handle;

        if(handle != CM_INVALID_ATT_HANDLE)
        {
            g_gaia_client_data.gaia_status = 
                    CMClientWriteRequest(g_gaia_client_data.device_id,
                                         GATT_WRITE_REQUEST,
                                         handle,
                                         len,
                                         value);
            if(g_gaia_client_data.gaia_status == cm_status_busy)
            {
                MemCopy(g_gaia_client_data.pending_data,value,len);
                g_gaia_client_data.pending_length = len;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaReadResponseEndpoint
 *
 *  DESCRIPTION
 *      This function reads the GAIA response endpoint characteristic
 *
 *  RETURNS
 *      TRUE if the initiation of the read procedure was successful
 *      FALSE if the remote device does not support the configuration
 *----------------------------------------------------------------------------*/
extern bool GaiaReadResponseEndpoint(device_handle_id device_id)
{
   int8 instance_index = CMClientFindDevice(&g_gaia_client_service, device_id);
   if(instance_index >= 0)
   {
        CM_SERVICE_INSTANCE *instance = &g_service_instances[instance_index];
        uint16 handle = instance->characteristics[RESPONSE_CHAR].value_handle;

        if(handle != CM_INVALID_ATT_HANDLE)
        {
            CMClientReadRequest(device_id, handle);

            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaClientIsServiceDiscovered
 *
 *  DESCRIPTION
 *      TThis function checks if the GAIA service handles are discovered
 *
 *  RETURNS
 *     Nothing
 *
 *----------------------------------------------------------------------------*/
extern bool GaiaClientIsServiceDiscovered(device_handle_id device_id)
{
   int8 instance_index;

    if((instance_index = CMClientFindDevice(
                        &g_gaia_client_service,
                        device_id)) < 0)
    {
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaConfigureService
 *
 *  DESCRIPTION
 *      This function configures the notifications for Gaia service
 *
 *  RETURNS
 *     Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaConfigureService(device_handle_id device_id)
{
    switch(g_gaia_client_data.cur_state)
    {
        case STATE_INIT:
        {
            GaiaConfigureResponseConfig(device_id);
        }
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaDeviceDisconnect
 *
 *  DESCRIPTION
 *      This function disconnects the link
 *
 *  RETURNS
 
 *----------------------------------------------------------------------------*/
extern void GaiaDeviceDisconnect(void)
{
    CMDisconnect(g_gaia_client_data.device_id);
}
#endif