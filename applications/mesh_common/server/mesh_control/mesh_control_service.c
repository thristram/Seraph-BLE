/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      mesh_control_service.c
 *
 *  DESCRIPTION
 *      This file defines routines for using the Mesh Control Service.
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <gatt.h>
#include <gatt_prim.h>
#include <mem.h>
#include <buf_utils.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "app_debug.h"
#include "app_gatt_db.h"
#include "app_gatt.h"
#include "mesh_control_service.h"
#include "cm_types.h"
#include "cm_server.h"
#include "cm_api.h"
/*============================================================================*
 *  CSRmesh Header Files
 *============================================================================*/
#include "csr_mesh.h"
#include "csr_sched_types.h"
#include "csr_sched.h"

/*============================================================================*
 *  Private Data Types
 *============================================================================*/

typedef struct
{
    uint16 length;

    uint8 mesh_data[MESH_LONGEST_MSG_LEN];
}MESH_MSG_T;

/* Structure for the Lock Unlock service */
typedef struct
{
    /* Client configuration for Mesh Control characteristic */
    gatt_client_config  mtl_cp_ccd;

    MESH_MSG_T mesh_data;

    uint16 device_id;

}MESH_SERVICE_DATA_T;

/* This function handles the events from the connection manager */
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data);

/*============================================================================*
 *  Private Data
 *============================================================================*/

MESH_SERVICE_DATA_T        g_mesh_svc_data;

static CM_HANDLERS_T g_mesh_server_handlers = 
{
    .pCallback = &handleConnMgrProcedureEvent
};

static CM_SERVER_INFO_T g_mesh_service_info;

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      MeshControlServiceDataInit
 *
 *  DESCRIPTION
 *      This function is used to initialise Mesh Control Service data 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void MeshControlServiceDataInit(void)
{
    /* Initialise Mesh Control Service Client Configuration Characterisitic
     * descriptor value to none.
     */
    g_mesh_svc_data.mtl_cp_ccd = gatt_client_config_none;
    g_mesh_svc_data.device_id = CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      MeshControlHandleAccessRead
 *
 *  DESCRIPTION
 *      This function handles read operations on the Mesh Control
 *      attributes maintained by the application and responds with the
 *      GATT_ACCESS_RSP message.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data)
{
    uint16 length = 0;
    uint8  *p_value = NULL;
    sys_status rc = sys_status_success;
    CSR_MESH_UUID_T devUUID;
    uint8 val[16];
    CM_ACCESS_RESPONSE_T cm_access_rsp;

    switch(p_event_data->handle)
    {
        case HANDLE_DEVICE_UUID:
        {
            p_value = val;
            MemCopy(val, devUUID.uuid, 16);
            length = 16;
        }
        break;

        case HANDLE_DEVICE_ID:
        {
            /* Do Nothing for now */
        }
        break;

        case HANDLE_MTL_TTL:
        {
            /* Do Nothing for now */
        }
        break;

        case HANDLE_MTL_CP_CLIENT_CONFIG:
        {
            p_value = val;
            BufWriteUint16(&p_value, g_mesh_svc_data.mtl_cp_ccd);
            p_value = val;
            length = 2;
        }

        default:
            /* No more IRQ characteristics */
            rc = gatt_status_read_not_permitted;
        break;

    }

    cm_access_rsp.device_id = p_event_data->device_id;
    cm_access_rsp.handle = p_event_data->handle;
    cm_access_rsp.rc = rc;
    cm_access_rsp.size_value = length;
    cm_access_rsp.value = p_value;

    CMSendAccessRsp(&cm_access_rsp);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      MeshControlHandleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles write operations on the Mesh Control
 *      service attributes maintained by the application and responds with the
 *      GATT_ACCESS_RSP message.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data)
{
    sys_status rc = sys_status_success;
    uint8  *pValue;
    bool csr_mesh_send_msg = FALSE;
    CM_ACCESS_RESPONSE_T cm_access_rsp;

    switch(p_event_data->handle)
    {
        case HANDLE_MTL_CP_CLIENT_CONFIG:
        case HANDLE_MTL_CP2_CLIENT_CONFIG:
        {
            CSR_SCHED_GATT_EVENT_DATA_T gatt_event_data;
            gatt_event_data.cid = CMGetConnId(p_event_data->device_id);
            pValue = p_event_data->data;
            g_mesh_svc_data.mtl_cp_ccd = BufReadUint16(&pValue);
            g_mesh_svc_data.device_id = p_event_data->device_id;

            /* Reset the reserved bits in any case */
            g_mesh_svc_data.mtl_cp_ccd &= ~gatt_client_config_reserved;

            /* Configure the scheduler with the ccd value written */
            if((g_mesh_svc_data.mtl_cp_ccd & gatt_client_config_notification)
                    == gatt_client_config_notification)
            {
                gatt_event_data.is_notification_enabled = TRUE;
                CSRSchedNotifyGattEvent(CSR_SCHED_GATT_CCCD_STATE_CHANGE_EVENT,
                                        &gatt_event_data, 
                                        MeshControlNotifyResponse);
            }
            else
            {
                gatt_event_data.is_notification_enabled = FALSE;
                CSRSchedNotifyGattEvent(CSR_SCHED_GATT_CCCD_STATE_CHANGE_EVENT,
                                        &gatt_event_data,
                                        NULL);
            }
        }
        break;

        case HANDLE_MTL_CONTINUATION_CP:
        {
            pValue = p_event_data->data;

            /* Reset the length of the mesh message */
            g_mesh_svc_data.mesh_data.length = 0;

            if(p_event_data->length)
            {
                MemCopy(g_mesh_svc_data.mesh_data.mesh_data + p_event_data->offset,
                        p_event_data->data, p_event_data->length);
                g_mesh_svc_data.mesh_data.length = p_event_data->length;
            }
        }
        break;

        case HANDLE_MTL_COMPLETE_CP:
        {
            pValue = p_event_data->data;

            if(p_event_data->length && ((p_event_data->length + 
                     g_mesh_svc_data.mesh_data.length) <= MESH_LONGEST_MSG_LEN))
            {
                MemCopy(g_mesh_svc_data.mesh_data.mesh_data + \
                        g_mesh_svc_data.mesh_data.length,
                        p_event_data->data, p_event_data->length);
                g_mesh_svc_data.mesh_data.length += p_event_data->length;

                /* Send message to CSRmesh Library. */
                csr_mesh_send_msg = TRUE;
            }
            else
            {
                g_mesh_svc_data.mesh_data.length = 0;
            }

        }
        break;

        case HANDLE_MTL_TTL:
        {
            uint8 ttl = 0x00;
            pValue = p_event_data->data;
            ttl =  BufReadUint8(&pValue);
        }
        break;

        default:
        {
            /* No more IRQ characteristics */
            rc = gatt_status_write_not_permitted;
        }
        break;
    }

    /* Send ACCESS RESPONSE */
    cm_access_rsp.device_id = p_event_data->device_id;
    cm_access_rsp.handle = p_event_data->handle;
    cm_access_rsp.rc = rc;
    cm_access_rsp.size_value = 0;
    cm_access_rsp.value = NULL;

    CMSendAccessRsp(&cm_access_rsp);

    if (csr_mesh_send_msg)
    {
        int8 rssi;

        /* Ignore the error as FW fills invalid RSSI value in case of error. */
        (void)CMReadRssi(p_event_data->device_id, &rssi);

        /* Send the MTL data as it is on the mesh */

        /* Update Bearer Event Data structure with incoming Mesh Data */
        if(g_mesh_svc_data.mesh_data.length <= sizeof(g_mesh_svc_data.mesh_data.mesh_data))
        {

             CSRSchedHandleIncomingData(CSR_SCHED_INCOMING_GATT_MESH_DATA_EVENT,
                                       g_mesh_svc_data.mesh_data.mesh_data,
                                       g_mesh_svc_data.mesh_data.length,
                                       rssi);

            /* Reset the length of the mesh message */
            g_mesh_svc_data.mesh_data.length = 0;
        }
    }
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
        break;

        case CM_BONDING_NOTIFY:
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
 *      MeshControlInitServerService
 *
 *  DESCRIPTION
 *      This function initialises the Mesh Control service.
 *
 *  RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/

extern void MeshControlInitServerService(bool nvm_start_fresh, uint16 *nvm_offset)
{
    g_mesh_service_info.server_handler = g_mesh_server_handlers;
    g_mesh_service_info.start_handle = HANDLE_MESH_CONTROL_SERVICE;
    g_mesh_service_info.end_handle = HANDLE_MESH_CONTROL_SERVICE_END;

    /* Register HR Server service */
    CMServerInitRegisterHandler(&g_mesh_service_info);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      MeshControlNotifyResponse
 *
 *  DESCRIPTION
 *      This function notifies responses received on the mesh to the GATT
 *      client.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void MeshControlNotifyResponse(uint16 conn_id, 
                                      uint8 *mtl_msg,
                                      uint8 length)
{
    /* Update the connected host if notifications are configured */
    if((g_mesh_svc_data.device_id != CM_INVALID_DEVICE_ID) &&
       (g_mesh_svc_data.mtl_cp_ccd == gatt_client_config_notification))
    {
        CM_VALUE_NOTIFICATION_T notification;
        /* If message is less than or equal to ATT_WRITE_MAX_DATA_LEN,
         * notify it using MTL_COMPLETE_CP. If it is greater than
         * ATT_WRITE_MAX_DATA_LEN notify first ATT_WRITE_MAX_DATA_LEN bytes
         * with MTL_CONTINUATION_CP and rest with MTL_COMPLETE_CP.
         */
        if (length <= ATT_WRITE_MAX_DATALEN)
        {
            notification.device_id = g_mesh_svc_data.device_id;
            notification.handle = HANDLE_MTL_COMPLETE_CP;
            notification.value = mtl_msg;
            notification.size_value = length;
            CMSendValueNotification(&notification);
        }
        else
        {
            /* Send first ATT_WRITE_MAX_DATALEN with MTL_CONTINUATION_CP */

            notification.device_id = g_mesh_svc_data.device_id;
            notification.handle = HANDLE_MTL_CONTINUATION_CP;
            notification.value = mtl_msg;
            notification.size_value = ATT_WRITE_MAX_DATALEN;
            CMSendValueNotification(&notification);

            /* Send rest of the message with MTL_COMPLETE_CP */
            notification.handle = HANDLE_MTL_COMPLETE_CP;
            notification.value = &mtl_msg[ATT_WRITE_MAX_DATALEN];
            notification.size_value = length - ATT_WRITE_MAX_DATALEN;
            CMSendValueNotification(&notification);
        }
    }
}
