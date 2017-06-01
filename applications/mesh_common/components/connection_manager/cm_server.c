/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_server.c
 *
 *  DESCRIPTION
 *      This file defines routines for connection manager server role.
 *
 *
 ******************************************************************************/
#if defined (SERVER)

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

#include "cm_private.h"
#include "cm_server.h"
#include "cm_types.h"
#include "cm_api.h"
#include "cm_hal.h"

/*============================================================================*
 *  Private Data Type
 *============================================================================*/

/* CM Server data type */
typedef struct
{
    CM_SERVER_INFO_T                *server_info;

    /* Maximum of server handlers */
    uint16                          max_server_services;

    /* Total Number server handlers registered */
    uint16                          num_reg_services;

} CM_SERVER_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* CM Client data */
static CM_SERVER_DATA_T  g_server_data;

/*============================================================================*
 *  Private Function Declarations
 *============================================================================*/

/* Handles the signal GATT_ADD_DB_CFM_T */
static void handleSignalGattAddDbCfm(h_gatt_add_db_cfm_t* p_event_data);

#ifndef THIN_CM4_MESH_NODE
/* Handles the HAL_GATT_CHAR_VAL_NOTIFICATION_CFM event */
static void handleGattCharValNotificationCfm
        (h_gatt_char_val_ind_cfm_t *p_event_data);

/* Handles the HAL_GATT_CHAR_VAL_INDICATION_CFM event */
static void handleGattCharValIndicationCfm(
        h_gatt_char_val_ind_cfm_t *p_event_data);
#endif /* THIN_CM4_MESH_NODE */

/* This function handles GATT_ACCESS_IND message. */
static void handleSignalGattAccessInd(h_gatt_access_ind_t* p_event_data);

/*============================================================================*
 *  Private Function Implementation
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalGattAddDbCfm
 *
 *  DESCRIPTION
 *      Handles the signal GATT_ADD_DB_CFM_T
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalGattAddDbCfm(h_gatt_add_db_cfm_t *p_event_data)
{
    if(p_event_data->result != sys_status_success)
    {
        CMReportPanic(cm_panic_db_registration);
    }
    else
    {
        CM_INIT_CFM_T cm_init_cfm;
        cm_init_cfm.status = sys_status_success;

        /* Send CM init confirmation event to the application */
        CMNotifyEventToApplication(CM_INIT_CFM,
                                   (CM_EVENT_T *)&cm_init_cfm);
    }
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGattCharValNotificationCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_CHAR_VAL_NOTIFICATION_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleGattCharValNotificationCfm(h_gatt_char_val_ind_cfm_t *p_event_data)
{
    CM_NOTIFICATION_CFM_T cm_notification_cfm;
    device_handle_id device_id = CMGetDeviceId(HAL_GATT_CHAR_VAL_NOTIFICATION_CFM,
            (void*)p_event_data);

    cm_notification_cfm.device_id = device_id;
    cm_notification_cfm.status = p_event_data->result;
    cm_notification_cfm.handle = p_event_data->handle;

    /* Send notification confirmation to server */
    CMServerNotifyGattEvent(CM_NOTIFICATION_CFM,
                   (CM_EVENT_T *)&cm_notification_cfm,
                   CMServerGetHandler(p_event_data->handle));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleGattCharValIndicationCfm
 *
 *  DESCRIPTION
 *      Handles the signal HAL_GATT_CHAR_VAL_INDICATION_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleGattCharValIndicationCfm(h_gatt_char_val_ind_cfm_t *p_event_data)
{
    CM_INDICATION_CFM_T cm_indication_cfm;
    device_handle_id device_id = CMGetDeviceId(HAL_GATT_CHAR_VAL_NOTIFICATION_CFM,
            (void*)p_event_data);

    cm_indication_cfm.device_id = device_id;
    cm_indication_cfm.status = p_event_data->result;
    cm_indication_cfm.handle = p_event_data->handle;

    /* Send indication confirmation to server */
    CMServerNotifyGattEvent(CM_INDICATION_CFM,
                   (CM_EVENT_T *)&cm_indication_cfm,
                   CMServerGetHandler(p_event_data->handle));
}
#endif /* THIN_CM4_MESH_NODE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalGattAccessInd
 *
 *  DESCRIPTION
 *      Handles the signal GATT_ACCESS_IND
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleSignalGattAccessInd(h_gatt_access_ind_t *p_event_data)
{
    CM_SERVER_ACCESSED_T cm_server_accessed;
    device_handle_id device_id = CMGetDeviceId(HAL_GATT_ACCESS_IND,
                                               (void*)p_event_data);
    cm_dev_state state = CMGetDevState(device_id);

    cm_server_accessed.device_id = device_id;
    cm_server_accessed.handle = p_event_data->handle;
    cm_server_accessed.offset = p_event_data->offset;

    /* Send access event to the application */
    CMNotifyEventToApplication(CM_SERVER_ACCESSED,
                               (CM_EVENT_T *)&cm_server_accessed);

    switch(state)
    {
        case dev_state_connected:
        {
            /* Received GATT ACCESS IND with write access */
            if(p_event_data->flags ==
                (ATT_ACCESS_WRITE |
                 ATT_ACCESS_PERMISSION |
                 ATT_ACCESS_WRITE_COMPLETE))
            {
                CM_WRITE_ACCESS_T cm_server_write;

                /* Prepare the send write access message to notify the server */
                cm_server_write.device_id = device_id;
                cm_server_write.handle = p_event_data->handle;
                cm_server_write.offset = p_event_data->offset;

                /*  Parse Server Write Indication */
                HALParseAccessWriteInd(&cm_server_write,p_event_data);

                /* Send write access event to the server */
                CMServerNotifyGattEvent(CM_WRITE_ACCESS,
                              (CM_EVENT_T *)&cm_server_write,
                              CMServerGetHandler(p_event_data->handle));
            }
            /* Received GATT ACCESS IND with read access */
            else if(p_event_data->flags ==
                (ATT_ACCESS_READ |
                ATT_ACCESS_PERMISSION))
            {
                CM_READ_ACCESS_T cm_server_read;

                /* Prepare the send read access message to notify the server */
                cm_server_read.device_id = device_id;
                cm_server_read.handle = p_event_data->handle;
                cm_server_read.offset = p_event_data->offset;

                /*  Parse Server Write Indication */
                HALParseAccessReadInd(&cm_server_read, p_event_data);

                /* Send read access event to the server */
                CMServerNotifyGattEvent(CM_READ_ACCESS,
                               (CM_EVENT_T *)&cm_server_read,
                               CMServerGetHandler(p_event_data->handle));
            }
            else
            {
                /* Request not supported */
                GattAccessRsp(p_event_data->cid, p_event_data->handle,
                              gatt_status_request_not_supported,
                              0, NULL);
            }
        }
        break;

        default:
            /* The event has been received in an invalid state.
             * Report panic
             */
            CMReportPanic(cm_panic_invalid_state);
        break;
    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMServerInit
 *
 *  DESCRIPTION
 *      Initialises the CM server entity
 *
 *  RETURNS
 *      bool: TRUE if gatt database is being registered, FALSE otherwise
 *
 *---------------------------------------------------------------------------*/

extern bool CMServerInit(CM_INIT_PARAMS_T *cm_init_params)
{
    /* Initialises the server write */
    HALServerWriteInit();

    /* Save the server service information */
    g_server_data.server_info = cm_init_params->server_info;
    g_server_data.max_server_services = cm_init_params->max_server_services;

    /* Initialise number of register handlers */
    g_server_data.num_reg_services = 0;

    /* Register the GATT database */
    if((cm_init_params->db_length > 0) && (cm_init_params->gatt_db != NULL))
    {
        GattAddDatabaseReq(cm_init_params->db_length, cm_init_params->gatt_db);
        return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMServerInitRegisterHandler
 *
 *  DESCRIPTION
 *      Registers Server Information
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMServerInitRegisterHandler(
                            CM_SERVER_INFO_T *cm_server_info)
{
    if(g_server_data.num_reg_services >= g_server_data.max_server_services)
    {
        CMReportPanic(cm_panic_server_service_size_exceeded);
    }

    g_server_data.server_info[g_server_data.num_reg_services] = *cm_server_info;

    ++g_server_data.num_reg_services;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMServerGetHandler
 *
 *  DESCRIPTION
 *      Gets the server handler
 *
 *  RETURNS
 *      CM_HANDLERS_T
 *
 *---------------------------------------------------------------------------*/
extern CM_HANDLERS_T *CMServerGetHandler(uint16 handle)
{
    uint16 index;

    for(index = 0; index < g_server_data.num_reg_services; index++)
    {
        if((handle >= g_server_data.server_info[index].start_handle) &&
           (handle <= g_server_data.server_info[index].end_handle))
        {
            return &g_server_data.server_info[index].server_handler;
        }
    }

    return NULL;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMServerNotifyGenericEvent
 *
 *  DESCRIPTION
 *      Sends the generic event to all the registered servers
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMServerNotifyGenericEvent(cm_event event_type,
                                CM_EVENT_T *server_event)
{
    uint16 index;
    CM_HANDLERS_T server_handler;

    for(index = 0; index < g_server_data.num_reg_services; index++)
    {
       server_handler = g_server_data.server_info[index].server_handler;

       if(server_handler.pCallback == NULL)
           continue; /* NULL Callback */

       /* Notify the servers */
       server_handler.pCallback(event_type, server_event);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMServerNotifyGattEvent
 *
 *  DESCRIPTION
 *      Sends the gatt events to all the registered servers
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMServerNotifyGattEvent(cm_event event_type,
                                CM_EVENT_T *server_event,
                                CM_HANDLERS_T *server_handler)
{
    if((server_handler != NULL) && (server_handler->pCallback != NULL))
    {
        server_handler->pCallback(event_type, server_event);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSendAccessRsp
 *
 *  DESCRIPTION
 *      Sends the access response to the remote device
 *
 *  RETURNS/MODIFIES
 *      cm_status_code: cm_status_success if the request is processed
 *
 *----------------------------------------------------------------------------*/
extern cm_status_code CMSendAccessRsp(CM_ACCESS_RESPONSE_T *cm_access_rsp)
{
    uint16 cid = CMGetConnId(cm_access_rsp->device_id);

    /* Send ACCESS RESPONSE */
    GattAccessRsp(cid, cm_access_rsp->handle, cm_access_rsp->rc,
                  cm_access_rsp->size_value, cm_access_rsp->value);

    return cm_status_success;
}

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSendValueNotificationExt
 *
 *  DESCRIPTION
 *      Sends the Notification Value to the remote device
 *
 *  RETURNS/MODIFIES
 *      sys_status 
 *
 *----------------------------------------------------------------------------*/
extern sys_status CMSendValueNotificationExt(
                            CM_VALUE_NOTIFICATION_T *cm_value_notify)
{
     /* Handle the procedure in HAL */
    return HALSendValueNotificationExt(cm_value_notify);
}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSendValueNotification
 *
 *  DESCRIPTION
 *      Sends the Notification Value to the remote device
 *
 *  RETURNS/MODIFIES
 *      cm_status_code: cm_status_success if the request is processed
 *
 *----------------------------------------------------------------------------*/
extern cm_status_code CMSendValueNotification(CM_VALUE_NOTIFICATION_T
                                              *cm_value_notify)
{
    /* Handle the procedure in HAL */
    HALSendValueNotification(cm_value_notify);

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSendValueIndication
 *
 *  DESCRIPTION
 *      Sends the Indication Value to the remote device
 *
 *  RETURNS/MODIFIES
 *      cm_status_code: cm_status_success if the request is processed
 *
 *----------------------------------------------------------------------------*/
extern cm_status_code CMSendValueIndication(CM_VALUE_NOTIFICATION_T
                                              *cm_value_notify)
{
    /* Handle the procedure in HAL */
    return (HALSendValueIndication(cm_value_notify));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMServerHandleProcessLMEvent
 *
 *  DESCRIPTION
 *      Handles the firmware events related to the server role
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMServerHandleProcessLMEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
        case HAL_GATT_ADD_DB_CFM:
        {
            handleSignalGattAddDbCfm(
                    (h_gatt_add_db_cfm_t*)HALGetMsg(msg));
        }
        break;
#ifndef THIN_CM4_MESH_NODE
        case HAL_GATT_CHAR_VAL_NOTIFICATION_CFM:
        {
            handleGattCharValNotificationCfm((h_gatt_char_val_ind_cfm_t *)
                    HALGetMsg(msg));
        }
        break;

        case HAL_GATT_CHAR_VAL_INDICATION_CFM:
        {
            handleGattCharValIndicationCfm((h_gatt_char_val_ind_cfm_t *)
                    HALGetMsg(msg));
        }
        break;
#endif /* THIN_CM4_MESH_NODE */
        case GATT_ACCESS_IND:
        {
            /* Access Indication */
            handleSignalGattAccessInd(
                    (h_gatt_access_ind_t*) HALGetMsg(msg));
        }
        break;
        default:
        break;
    }

}

#endif /* SERVER */