/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_hal.c
 *
 *  DESCRIPTION
 *      This file defines connection manager hardware abstraction layer
 *
 *
 ******************************************************************************/
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <security.h>
#include <mem.h>
#include <gatt.h>
#include <timer.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_hal.h"
#include "cm_types.h"
#include "cm_security.h"
#include "cm_server.h"

/*============================================================================*
 *  Private Data
 *============================================================================*/

#if defined (SERVER)
#ifndef CSR101x_A05
 /* CM Notification Data */
static CM_NOTIFICATION_CFM_T cm_notification_cfm;

/*  CM Notification Confirm Timer */
static timer_id notif_cfm_tid;
#endif /* (!CSR101x_A05) */ 
#endif /* (SERVER) */

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
/* Gets device id from the cid */
static device_handle_id getDeviceIdFromCid(uint16 cid,
                                           CM_MAIN_DATA_T *p_main_data);

#if defined (CSR101x_A05)
/* Gets device id from the hci handle */
static device_handle_id getDeviceIdFromHciHandle(uint16 handle,
                                       CM_MAIN_DATA_T *p_main_data);

/* Gets the device id from the Bluetooth Address */
static device_handle_id getDeviceIdFromBDAddress(TYPED_BD_ADDR_T *bd_addr,
                                       CM_MAIN_DATA_T *p_main_data);

/* Gets the hci handle from the device id */
static uint16 getHciHandleFromDeviceId(device_handle_id device_id);

#endif /* CSR101x_A05 */

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      getDeviceIdFromCid
 *
 *  DESCRIPTION
 *      Gets device id from the cid.
 *
 *  RETURNS
 *      device_handle_id: Device index
 *
 *---------------------------------------------------------------------------*/

static device_handle_id getDeviceIdFromCid(uint16 cid,
                                           CM_MAIN_DATA_T *p_main_data)
{
    device_handle_id index;
    CM_CONN_INFO_T* p_conn_info = &(p_main_data->cm_conn_info[0]);

    /* Parse through the Db to find the device*/
    for(index = 0; index < p_main_data->max_connections; index++)
    {
        if(p_conn_info[index].cid == cid)
        {
            return index;
        }
    }
    return CM_INVALID_DEVICE_ID;
}

#if defined (CSR101x_A05)
/*----------------------------------------------------------------------------*
 *  NAME
 *      getDeviceIdFromHciHandle
 *
 *  DESCRIPTION
 *      Gets device id from the hci handle
 *
 *  RETURNS
 *      device_handle_id: Device Index
 *
 *---------------------------------------------------------------------------*/

static device_handle_id getDeviceIdFromHciHandle(uint16 handle,
                                       CM_MAIN_DATA_T *p_main_data)
{
    device_handle_id index;
    CM_CONN_INFO_T* p_conn_info = &(p_main_data->cm_conn_info[0]);

    /* Parse through the Db to find the device*/
    for(index = 0; index < p_main_data->max_connections; index++)
    {
        if(p_conn_info[index].hci_conn_handle == handle)
        {
            return index;
        }
    }
    return CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getDeviceIdFromBDAddress
 *
 *  DESCRIPTION
 *      Gets the device id from the Bluetooth Address
 *
 *  RETURNS
 *      device_handle_id: Device Index
 *
 *---------------------------------------------------------------------------*/

static device_handle_id getDeviceIdFromBDAddress(TYPED_BD_ADDR_T *bd_addr,
                                       CM_MAIN_DATA_T *p_main_data)
{
    device_handle_id index;
    CM_CONN_INFO_T* p_conn_info = &(p_main_data->cm_conn_info[0]);

    /* Parse through the Db to find the device id comparing the BD Address */
    for(index = 0; index < p_main_data->max_connections; index++)
    {
       /* evice at the cur index is not bonded */
       if(!MemCmp(&p_conn_info[index].remote_bd_addr,
                       bd_addr,sizeof(TYPED_BD_ADDR_T)))
        {
            return index;
        }
    }

    return CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getHciHandleFromDeviceId
 *
 *  DESCRIPTION
 *      Gets the hci handle from the device id
 *
 *  RETURNS
 *      bool: TRUE if the request is processed
 *
 *---------------------------------------------------------------------------*/
static uint16 getHciHandleFromDeviceId(device_handle_id device_id)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();

    return cm_main_data->cm_conn_info[device_id].hci_conn_handle;
}
#endif /* CSR101x_A05 */


#if defined (CENTRAL) || defined(OBSERVER)
#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      parseName
 *
 *  DESCRIPTION
 *      Parse the device name
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void parseName(CM_ADV_REPORT_IND_T *cm_adv_report_ind,
        h_ls_advertising_report_ind_t *adv_ind)
{
    uint16 data[ADVSCAN_MAX_PAYLOAD];
    uint16 size;
    uint16 src;
    uint16 dst;

    /* Initialise the name */
    MemSet(cm_adv_report_ind->name, 0, 21);

    /* Try to get the complete name */
    size = HALGapLsFindAdType(adv_ind,
                           AD_TYPE_LOCAL_NAME_COMPLETE,
                           data,
                           ADVSCAN_MAX_PAYLOAD);

    if(size == 0)
    {
        /* Try to get the short name */
        size = HALGapLsFindAdType(adv_ind,
                               AD_TYPE_LOCAL_NAME_SHORT,
                               data,
                               ADVSCAN_MAX_PAYLOAD);
    }

    /* Limit the name to 20 characters */
    if(size > 20) size = 20;

    if(size > 0)
    {
        /* Parse the name */
        for(src = 0, dst = 0; src < (size/2); src++)
        {
            cm_adv_report_ind->name[dst++] = (char)(data[src] & 0x00ff);
            cm_adv_report_ind->name[dst++] = (char)((data[src] & 0xff00) >> 8);
        }
        if(size % 2)
            cm_adv_report_ind->name[dst++] = (char)(data[src] & 0x00ff);


    }
    else
    {
       /* Assign the unknown name */
       StrNCopy(cm_adv_report_ind->name, "Unknown", 7);
    }
}
#endif /* THIN_CM4_MESH_NODE */
#endif /* (CENTRAL || OBSERVER ) */

#if defined (SERVER)
#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      sendNotificationCfmTimer
 *
 *  DESCRIPTION
 *      This function handles the timer expiry for sending notification confirm
 *      to the application
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void sendNotificationCfmTimer(timer_id tid)
{    
    if(tid == notif_cfm_tid)
    {
        /* Reinitailise the timer */
        notif_cfm_tid = TIMER_INVALID;
        
        /* Send notification confirmation to servers */
        CMServerNotifyGattEvent(CM_NOTIFICATION_CFM,
                                (CM_EVENT_T *)&cm_notification_cfm,
                                CMServerGetHandler(cm_notification_cfm.handle));        
        
    }
    /* Else Ignore. This may be due to some race condition */
} 
#endif /* (!CSR101x_A05) */ 
#endif /* (SERVER) */

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALGetMsgId
 *
 *  DESCRIPTION
 *      Gets the message id
 *
 *  RETURNS
 *      uint16: Message ID
 *
 *---------------------------------------------------------------------------*/

extern uint16 HALGetMsgId(h_msg_t *msg)
{
#ifndef CSR101x_A05
    return msg->msg->header.id;
#else
    return msg->msg->id;
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALGetMsg
 *
 *  DESCRIPTION
 *      Gets the message data
 *
 *  RETURNS
 *      void *: Message pointer
 *---------------------------------------------------------------------------*/

extern void *HALGetMsg(h_msg_t *msg)
{
#ifndef CSR101x_A05
    return (void*)&msg->msg->body;
#else
    return (void*)msg->msg->body;
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALAddNewDevice
 *
 *  DESCRIPTION
 *      Adds the new device to the connection info table
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/

extern void HALAddNewDevice(CM_CONN_INFO_T *p_conn_info,
                            h_ls_connection_complete_ind_t *p_event_data)
{
    /* Map the device address into storable form */
    p_conn_info->remote_bd_addr.type = p_event_data->peer_address_type;
    p_conn_info->remote_bd_addr.addr = p_event_data->peer_address;
    p_conn_info->peer_con_role = con_role_peripheral;
    if(p_event_data->role == 1)
    {
        p_conn_info->peer_con_role = con_role_central;
    }

#if defined (CSR101x_A05)
    p_conn_info->hci_conn_handle = p_event_data->connection_handle;
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALGetDeviceId
 *
 *  DESCRIPTION
 *     Gets the device id from the event
 *
 *  RETURNS
 *      device_handle_id: Device index
 *---------------------------------------------------------------------------*/

extern device_handle_id HALGetDeviceId(uint16 event_id, void *event,
                             CM_MAIN_DATA_T *p_main_data)
{
    switch(event_id)
    {
        case HAL_GATT_CONNECT_CFM:
        {
            h_gatt_connect_cfm_t *msg = (h_gatt_connect_cfm_t*)event;
            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;

        case HAL_DISCONNECT_COMPLETE_IND:
        {
            h_disconnect_complete_ind_t *msg  =
                    (h_disconnect_complete_ind_t *)event;

#ifndef CSR101x_A05
           return getDeviceIdFromCid(msg->cid, p_main_data);
#else
           return getDeviceIdFromHciHandle(msg->handle, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

#if defined (CLIENT)
        case HAL_GATT_SERV_INFO_IND:
        {
            h_gatt_serv_info_ind_t *msg  =
                    (h_gatt_serv_info_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
        case HAL_GATT_DISC_ALL_PRIM_SERV_CFM:
        {
            h_gatt_disc_all_prim_serv_cfm_t *msg  =
                    (h_gatt_disc_all_prim_serv_cfm_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
        case HAL_GATT_DISC_SERVICE_CHAR_CFM:
        {
            h_gatt_disc_service_char_cfm_t *msg  =
                    (h_gatt_disc_service_char_cfm_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
        case HAL_GATT_DISC_ALL_CHAR_DESC_CFM:
        {
            h_gatt_disc_all_char_desc_cfm_t *msg  =
                    (h_gatt_disc_all_char_desc_cfm_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
        case HAL_GATT_WRITE_CHAR_VAL_CFM:
        {
            h_gatt_write_char_val_cfm_t *msg  =
                    (h_gatt_write_char_val_cfm_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);

        }
        break;
        case HAL_GATT_READ_CHAR_VAL_CFM:
        {
            h_gatt_read_char_val_cfm_t *msg  =
                    (h_gatt_read_char_val_cfm_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
        case HAL_GATT_NOTIFICATION_IND:
        {
            h_gatt_char_val_ind_t *msg  =
                    (h_gatt_char_val_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
#endif /* CLIENT */

#if defined (SERVER)
        case HAL_GATT_ACCESS_IND:
        {
            h_gatt_access_ind_t *msg  =
                    (h_gatt_access_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;

        case HAL_GATT_CHAR_VAL_NOTIFICATION_CFM:
        {
            h_gatt_char_val_ind_cfm_t *msg =
                    (h_gatt_char_val_ind_cfm_t *)event;
            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
#endif /* SERVER */

#ifndef THIN_CM4_MESH_NODE
        case HAL_SM_KEYS_IND:
        {
            h_sm_keys_ind_t *msg  =
                    (h_sm_keys_ind_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromBDAddress(&msg->remote_addr, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_SM_LONG_TERM_KEY_IND:
        {
            h_sm_long_key_ind_t *msg  =
                    (h_sm_long_key_ind_t *)event;
            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;

        case HAL_SM_KEY_REQUEST_IND:
        {
            h_sm_key_request_ind_t *msg  =
                    (h_sm_key_request_ind_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromBDAddress(&msg->remote_addr, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;
#endif /* THIN_CM4_MESH_NODE */
        case HAL_SM_PAIRING_COMPLETE_IND:
        {
            h_sm_pairing_complete_ind_t *msg  =
                    (h_sm_pairing_complete_ind_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromBDAddress(&msg->bd_addr, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_SM_DIV_APPROVAL_IND:
        {
            h_sm_div_approval_ind_t *msg  =
                    (h_sm_div_approval_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
        case HAL_SM_PAIRING_AUTH_IND:
        {
#ifndef CSR101x_A05
            h_sm_pairing_auth_ind_t *msg  =
                    (h_sm_pairing_auth_ind_t *)event;
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return CM_INVALID_DEVICE_ID;
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_SM_ENCRYPTION_CHANGE_IND:
        {
            h_ls_encryption_change_ind_t *msg  =
                    (h_ls_encryption_change_ind_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromHciHandle(msg->handle, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_SM_PASSKEY_INPUT_IND:
        {
            h_sm_passkey_input_ind_t *msg = (h_sm_passkey_input_ind_t *)event;
#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromBDAddress(&msg->bd_addr, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_LS_RADIO_EVENT_IND:
        {
            h_ls_radio_event_ind_t *msg  =
                    (h_ls_radio_event_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;

#ifndef CSR101x_A05
        case HAL_LS_EARLY_WAKEUP_IND:
        {
            h_ls_radio_event_ind_t *msg  =
                    (h_ls_radio_event_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
#endif

#ifndef THIN_CM4_MESH_NODE
        case HAL_LS_DATA_RX_TIMING_IND:
        {
            h_ls_data_rx_timing_ind_t *msg  =
                    (h_ls_data_rx_timing_ind_t *)event;

            return getDeviceIdFromCid(msg->cid, p_main_data);
        }
        break;
#endif /* THIN_CM4_MESH_NODE */
        case HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND:
        {
            h_ls_connection_update_signalling_ind_t *msg  =
                    (h_ls_connection_update_signalling_ind_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromHciHandle(msg->con_handle, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_LS_CONNECTION_PARAM_UPDATE_CFM:
        case HAL_LS_CONNECTION_PARAM_UPDATE_IND:
        {
            h_ls_connection_param_update_cfm_t *msg  =
                    (h_ls_connection_param_update_cfm_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            return getDeviceIdFromBDAddress(&msg->address, p_main_data);
#endif /* !CSR101x_A05 */
        }
        break;

        case HAL_LS_CONNECTION_UPDATE_IND:
        {
            h_ls_connection_update_ind_t *msg  =
                    (h_ls_connection_update_ind_t *)event;

#ifndef CSR101x_A05
            return getDeviceIdFromCid(msg->cid, p_main_data);
#else
            HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T data  =
                    (HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T)msg->data;

            return getDeviceIdFromHciHandle(data.connection_handle, p_main_data);
#endif /* !CSR101x_A05 */
        }

        break;

        default:
            return CM_INVALID_DEVICE_ID;
            break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseConnCompleteInd
 *
 *  DESCRIPTION
 *      Parses the connection complete indication
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/

extern void HALParseConnCompleteInd(
                            h_ls_connection_complete_ind_t *conn_complete_ind,
                            h_msg_t *msg)
{
#ifndef CSR101x_A05
    *conn_complete_ind = *(h_ls_connection_complete_ind_t *)HALGetMsg(msg);
#else
    LM_EV_CONNECTION_COMPLETE_T *conn_complete =
            (LM_EV_CONNECTION_COMPLETE_T *)HALGetMsg(msg);

    conn_complete_ind->status = conn_complete->data.status;
    conn_complete_ind->connection_handle =
            conn_complete->data.connection_handle;
    conn_complete_ind->role = conn_complete->data.role;
    conn_complete_ind->peer_address_type =
            conn_complete->data.peer_address_type;
    conn_complete_ind->peer_address = conn_complete->data.peer_address;
    conn_complete_ind->conn_interval = conn_complete->data.conn_interval;
    conn_complete_ind->conn_latency = conn_complete->data.conn_latency;
    conn_complete_ind->supervision_timeout =
            conn_complete->data.supervision_timeout;
    conn_complete_ind->clock_accuracy = conn_complete->data.clock_accuracy;
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseDisconnCompleteInd
 *
 *  DESCRIPTION
 *      Parses the disconnection complete indication
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/

extern void HALParseDisconnCompleteInd(
                            h_disconnect_complete_ind_t *disconn_complete_ind,
                            h_msg_t *msg)
{
#ifndef CSR101x_A05
    *disconn_complete_ind = *(h_disconnect_complete_ind_t *)HALGetMsg(msg);
#if defined (SERVER)
    /* Delete the send notification confirm timer */
    TimerDelete(notif_cfm_tid);
    notif_cfm_tid = TIMER_INVALID;
#endif /* SERVER */
#else
    LM_EV_DISCONNECT_COMPLETE_T *disconn_complete =
            (LM_EV_DISCONNECT_COMPLETE_T *)HALGetMsg(msg);

    disconn_complete_ind->status = disconn_complete->data.status;
    disconn_complete_ind->handle =
            disconn_complete->data.handle;
    disconn_complete_ind->reason = disconn_complete->data.reason;

#endif /* !CSR101x_A05 */
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseEncChangeInd
 *
 *  DESCRIPTION
 *      Parses the encryption change indication
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/

extern void HALParseEncChangeInd(
                            h_ls_encryption_change_ind_t *enc_change_ind,
                            h_msg_t *msg)
{
#ifndef CSR101x_A05
    *enc_change_ind = *(h_ls_encryption_change_ind_t *)HALGetMsg(msg);
#else
    LM_EV_ENCRYPTION_CHANGE_T *encryption_change =
            (LM_EV_ENCRYPTION_CHANGE_T *)HALGetMsg(msg);

    enc_change_ind->status = encryption_change->data.status;
    enc_change_ind->handle = encryption_change->data.handle;
    enc_change_ind->enc_enable = encryption_change->data.enc_enable;
#endif /* !CSR101x_A05 */
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALParseRadioEventInd
 *
 *  DESCRIPTION
 *      Parses the radio event indication
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void HALParseRadioEventInd(CM_RADIO_EVENT_IND_T *radio_event_ind,
                                  h_ls_radio_event_ind_t *p_event_data)
{
#ifndef CSR101x_A05
    radio_event_ind->delta_ls    = p_event_data->delta_ls;
    radio_event_ind->delta_ms    = p_event_data->delta_ms;
#endif /* !CSR101x_A05 */

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseConnParamUpdate
 *
 *  DESCRIPTION
 *      Parses the connection parameter update
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

extern void HALParseConnParamUpdate(CM_DEV_CONN_PARAM_T *connection_param,
                                    h_ls_connection_update_ind_t* p_event_data)
{
#ifndef CSR101x_A05
    connection_param->conn_interval = p_event_data->conn_interval;
    connection_param->conn_latency = p_event_data->conn_latency;
    connection_param->supervision_timeout = p_event_data->supervision_timeout;
#else
    connection_param->conn_interval = p_event_data->data.conn_interval;
    connection_param->conn_latency = p_event_data->data.conn_latency;
    connection_param->supervision_timeout =
                                p_event_data->data.supervision_timeout;
#endif /* !CSR101x_A05 */
}

#if defined (CENTRAL) || defined(OBSERVER)
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALGapSetCentralMode
 *
 *  DESCRIPTION
 *      Sets the GAP for Central mode
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/
extern void HALGapSetCentralMode(gap_mode_bond bond_mode,
                                 gap_mode_security security_mode)
{
#ifndef CSR101x_A05
    /* Set default security mode */
    GapSetDefaultSecurity(bond_mode, security_mode);

    /* Set default discovery mode */
    GapSetScanDiscoveryMode(gap_mode_discover_no);
#else
    gap_role role;
    
#if defined (CENTRAL)
    role = gap_role_central;
#elif defined (OBSERVER)
    role = gap_role_observer;
#endif /* CENTRAL */
    
    /* Set GAP modes for Baldrick */
    GapSetMode(role,
               gap_mode_discover_no,
               gap_mode_connect_no,
               bond_mode,
               security_mode);
#endif /* !CSR101x_A05 */
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseAdvertisingReportInd
 *
 *  DESCRIPTION
 *      Parses the advertising report indication
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/

extern void HALParseAdvertisingReportInd(CM_ADV_REPORT_IND_T *cm_adv_report_ind,
        h_ls_advertising_report_ind_t *adv_ind)
{
#ifndef CSR101x_A05
    hci_advertising_report_t report = (hci_advertising_report_t)adv_ind->report;
    HCI_EV_DATA_ULP_ADVERTISING_REPORT_T report_data =
            (HCI_EV_DATA_ULP_ADVERTISING_REPORT_T)report.hdr;
    cm_adv_report_ind->bd_addr.type = report_data.address_type;
    cm_adv_report_ind->bd_addr.addr = report_data.address;
    cm_adv_report_ind->rssi = report.rssi;
    cm_adv_report_ind->type = report_data.event_type;
    parseName(cm_adv_report_ind, adv_ind);

#else
    cm_adv_report_ind->bd_addr.type =
            adv_ind->data.address_type;
    cm_adv_report_ind->bd_addr.addr =
            adv_ind->data.address;
    cm_adv_report_ind->rssi = adv_ind->rssi;
    cm_adv_report_ind->type = adv_ind->data.event_type;
    parseName(cm_adv_report_ind, adv_ind);
#endif /* !CSR101x_A05  */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALIsAdvertisingReport
 *
 *  DESCRIPTION
 *      Checks the event is advertising report or not
 *
 *  RETURNS
 *      TRUE/FALSE
 *
 *---------------------------------------------------------------------------*/
extern bool HALIsAdvertisingReport(h_ls_advertising_report_ind_t *adv_ind)
{
#ifndef CSR101x_A05
    hci_advertising_report_t report = (hci_advertising_report_t)adv_ind->report;
    HCI_EV_DATA_ULP_ADVERTISING_REPORT_T report_data =
            (HCI_EV_DATA_ULP_ADVERTISING_REPORT_T)report.hdr;
    
    return ((report_data.event_type == 0) || (report_data.event_type == 1));
#else
    return (adv_ind->data.event_type == 0);
#endif /* !CSR101x_A05  */

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALGapLsFindAdType
 *
 *  DESCRIPTION
 *      Finds the Ad Type in the advertisement report
 *
 *  RETURNS
 *      Size of the AD data
 *---------------------------------------------------------------------------*/
extern uint16 HALGapLsFindAdType(h_ls_advertising_report_ind_t *adv_ind,
                          uint16 adv_type,
                          uint16 *data,
                          uint16 size)
{
#ifndef CSR101x_A05
    return GapLsFindAdType(&adv_ind->report.hdr,
                               adv_type,
                               data,
                               size);
#else
    return GapLsFindAdType(&adv_ind->data,
                               adv_type,
                               data,
                               size);
#endif /* !CSR101x_A05  */

}
#endif /* THIN_CM4_MESH_NODE */
#endif /* (CENTRAL || OBSERVER) */


#if defined (PERIPHERAL)
#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALGapSetPeripheralMode
 *
 *  DESCRIPTION
 *      Sets the GAP for Peripheral mode
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/
extern void HALGapSetPeripheralMode(gap_role role,
                                    gap_mode_bond bond_mode,
                                    gap_mode_security security_mode,
                                    gap_mode_discover discover,
                                    gap_mode_connect connect)
{
#ifndef CSR101x_A05
    /* Set default security mode */
    GapSetDefaultSecurity(bond_mode, security_mode);

    /* Set default discovery mode */
    GapSetAdvDiscoveryMode(discover);
#else
    /* Set the GAP mode */
    GapSetMode(role, discover,
               connect, bond_mode, security_mode);
#endif /* !CSR101x_A05 */
}
#endif /* THIN_CM4_MESH_NODE */
#endif /* PERIPHERAL */


#if defined (SERVER)
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALServerWriteInit
 *
 *  DESCRIPTION
 *      Initialises the server write
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/
extern void HALServerWriteInit(void)
{
#if defined (CSR101x_A05)
    /* Install GATT Server support for Write procedures */
    GattInstallServerWrite();
#else
    /* Install the Server Exchange MTU */
    GattInstallServerExchangeMtu();
#endif /* CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseAccessWriteInd
 *
 *  DESCRIPTION
 *      Parses the Access Write Indication
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/
extern void HALParseAccessWriteInd(CM_WRITE_ACCESS_T *cm_write,
        h_gatt_access_ind_t *p_event_data)
{
#ifndef CSR101x_A05
     cm_write->data = p_event_data->payload.value;
     cm_write->length = p_event_data->payload.size_value;
#else
     cm_write->data = p_event_data->value;
     cm_write->length = p_event_data->size_value;
#endif /* !CSR101x_A05  */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseAccessReadInd
 *
 *  DESCRIPTION
 *      Parses the Access Read Indication
 *
 *  RETURNS
 *      Nothing
 *---------------------------------------------------------------------------*/
extern void HALParseAccessReadInd(CM_READ_ACCESS_T *cm_read,
        h_gatt_access_ind_t *p_event_data)
{
#ifndef CSR101x_A05
    cm_read->data = p_event_data->payload.value;
    cm_read->length = p_event_data->payload.size_value;
#else
    cm_read->data = p_event_data->value;
    cm_read->length = p_event_data->size_value;
#endif /* !CSR101x_A05  */
}
#endif /* SERVER */

#if defined (CLIENT)
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseDiscoverServiceInd
 *
 *  DESCRIPTION
 *      Parses the discovered service indication
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

extern void HALParseDiscoverServiceInd(CM_SERVICE_INSTANCE
                            *serv_instance, h_gatt_serv_info_ind_t *p_prim)
{
#ifndef CSR101x_A05
    serv_instance->start_handle = p_prim->start_handle;
#else
    serv_instance->start_handle =  p_prim->strt_handle;
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseReadCharValCfm
 *
 *  DESCRIPTION
 *      Parses the read confirmation
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

extern void HALParseReadCharValCfm(CM_READ_CFM_T *read_cfm,
                    h_gatt_read_char_val_cfm_t *p_event_data)
{
#ifndef CSR101x_A05
    read_cfm->length = p_event_data->payload.size_value;
    read_cfm->data = p_event_data->payload.value;
#else
    read_cfm->length = p_event_data->size_value;
    read_cfm->data = p_event_data->value;
#endif /* !CSR101x_A05 */
    read_cfm->status = p_event_data->result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseNotifInd
 *
 *  DESCRIPTION
 *      Parses notification and indication
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void HALParseNotifInd(CM_NOTIFICATION_T *notif_ind,
                h_gatt_char_val_ind_t *p_event_data)
{
#ifndef CSR101x_A05
    notif_ind->length       = p_event_data->payload.size_value;
    notif_ind->data         = p_event_data->payload.value;
#else
    notif_ind->length       = p_event_data->size_value;
    notif_ind->data         = p_event_data->value;
#endif /* !CSR101x_A05 */

}
#endif /* CLIENT */

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALSMInit
 *
 *  DESCRIPTION
 *      Initialises the security manager
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void HALSMInit(uint16 diversifier)
{
    /* Tell Security Manager module about the value it needs to initialise it's
     * diversifier to.
     */
    SMInit(diversifier);

#ifndef CSR101x_A05
    SMSetKeyDistributionMask(SM_KEY_DISTRIBUTION_IRK,
                         (SM_KEY_DISTRIBUTION_LTK | SM_KEY_DISTRIBUTION_IRK));
#endif /* !CSR101x_A05 */
}


#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseSmPairingAuthInd
 *
 *  DESCRIPTION
 *      Parses SM Pairing Auth Indication
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

extern void HALParseSmPairingAuthInd(CM_BONDING_AUTH_IND_T *bonding_auth_ind,
                                     h_sm_pairing_auth_ind_t *p_event_data)
{
#if defined (CSR101x_A05)
    bonding_auth_ind->data = p_event_data->data;
#endif /* CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALStartEncryption
 *
 *  DESCRIPTION
 *      Starts the encryption with the remote device
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

extern void HALStartEncryption(device_handle_id device_id)
{
#ifndef CSR101x_A05
    uint16 cid = CMGetConnId(device_id);
    SMRequestSecurityLevel(cid);
#else
    TYPED_BD_ADDR_T bd_addr;
    CMGetBdAdressFromDeviceId(device_id, &bd_addr);
    SMRequestSecurityLevel(&bd_addr);
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALSecuritySetConfiguration
 *
 *  DESCRIPTION
 *       Sets the security configurations for flashheart and
 *       baldrick accordingly
 *
 *  RETURNS
 *      sys_status
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code HALSecuritySetConfiguration(uint16 io_capability)
{
#ifndef CSR101x_A05
    sm_configuration config;

    config.io_capabilities = io_capability;
    config.max_key_size = SM_DEFAULT_MAX_ENC_KEY_SIZE;
    config.min_key_size = SM_DEFAULT_MIN_ENC_KEY_SIZE;
    config.enable_le_secure_connections = FALSE;
    config.enable_keypress_notifications = FALSE;
    if(SMSetConfiguration(&config) != sys_status_success)
        return cm_status_failed;
#else
    SMSetIOCapabilities(io_capability);
#endif /* !CSR101x_A05 */

    return cm_status_success;
}
#endif /* THIN_CM4_MESH_NODE */

#if defined (SERVER)
#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALSendValueNotificationExt
 *
 *  DESCRIPTION
 *       Sends the Notification Value to the remote device
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern sys_status HALSendValueNotificationExt(CM_VALUE_NOTIFICATION_T *cm_value_notify)
{
      uint16 cid = CMGetConnId(cm_value_notify->device_id);
      /* Send Notification */
      return GattCharValueNotification(cid,
                                       cm_value_notify->handle,
                                       cm_value_notify->size_value,
                                       cm_value_notify->value);
}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      HALSendValueNotification
 *
 *  DESCRIPTION
 *       Sends the Notification Value to the remote device
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void HALSendValueNotification(CM_VALUE_NOTIFICATION_T *cm_value_notify)
{
    uint16 cid = CMGetConnId(cm_value_notify->device_id);

#ifndef CSR101x_A05

    /* Send Notification */
    cm_notification_cfm.status = GattCharValueNotification(
                                      cid,
                                      cm_value_notify->handle,
                                      cm_value_notify->size_value,
                                      cm_value_notify->value);

    cm_notification_cfm.device_id = cm_value_notify->device_id;
    cm_notification_cfm.handle = cm_value_notify->handle;

    /* Delete the timer */
    TimerDelete(notif_cfm_tid);
    notif_cfm_tid = TIMER_INVALID;
    
    /* Create a timer to send the notification confirm */
    notif_cfm_tid = TimerCreate(0,
                                TRUE,
                                sendNotificationCfmTimer);
#else
    GattCharValueNotification(cid,
                              cm_value_notify->handle,
                              cm_value_notify->size_value,
                              cm_value_notify->value);
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALSendValueIndication
 *
 *  DESCRIPTION
 *       Sends the Notification Value to the remote device
 *
 *  RETURNS
 *      cm_status_code
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code HALSendValueIndication(CM_VALUE_NOTIFICATION_T *cm_value_notify)
{
    uint16 cid = CMGetConnId(cm_value_notify->device_id);

#ifndef CSR101x_A05
    /* Send Indication*/
    sys_status status = GattCharValueIndication(cid,
                                                cm_value_notify->handle,
                                                cm_value_notify->size_value,
                                                cm_value_notify->value);
    if(status == gatt_status_success_sent)
        return cm_status_success;
    else
        return cm_status_failed;
#else
    GattCharValueIndication(cid,
                             cm_value_notify->handle,
                             cm_value_notify->size_value,
                             cm_value_notify->value);
    return cm_status_success;
#endif /* !CSR101x_A05 */
}
#endif /* SERVER */

#ifndef THIN_CM4_MESH_NODE
/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALSendAuthRsp
 *
 *  DESCRIPTION
 *      Sends the bonding authorisation response
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void HALSendAuthRsp(CM_AUTH_RESP_T *auth_resp)
{
#ifndef CSR101x_A05
    SMPairingAuthRsp(CMGetConnId(auth_resp->device_id), auth_resp->accept);
#else
    SMPairingAuthRsp(auth_resp->data, auth_resp->accept);
#endif /* !CSR101x_A05 */
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALSendLongTermKeyAuthRsp
 *
 *  DESCRIPTION
 *      Sends the LTK response for current connection
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void HALSendLongTermKeyAuthRsp(CM_LTK_RESP_T *ltk_rsp)
{
#ifndef CSR101x_A05
    SMLongTermKeyRsp(ltk_rsp->cid,ltk_rsp->security_level,
                     ltk_rsp->key_size,ltk_rsp->ltk);
#else
    SMLongTermKeyRsp(ltk_rsp->cid,ltk_rsp->ltk,TRUE,ltk_rsp->key_size);
#endif /* !CSR101x_A05 */
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALSendPasskeyRsp
 *
 *  DESCRIPTION
 *      Handles the passkey response based on the hardware API's
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void HALSendPasskeyRsp(device_handle_id device_id,
                              const uint32 *pass_key)
{
#ifndef CSR101x_A05
    uint16 cid = CMGetConnId(device_id);
    if(pass_key)
    {
        SMPasskeyInputRsp(cid, pass_key);
    }
    else
    {
        SMPasskeyInputNegRsp(cid);
    }
#else
    TYPED_BD_ADDR_T bd_address;
    CMGetBdAdressFromDeviceId(device_id, &bd_address);
    if (pass_key)
    {
        SMPasskeyInput(&bd_address, pass_key);
    }
    else
    {
        SMPasskeyInputNeg(&bd_address);
    }
#endif /* !CSR101x_A05 */
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALSMKeyRequestResponse
 *
 *  DESCRIPTION
 *      Handles the Keys Request response based on the hardware API's
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void HALSMKeyRequestResponse(device_handle_id device_id,
                                    h_sm_keyset_t   *keys)
{

#ifndef CSR101x_A05
    uint16 cid = CMGetConnId(device_id);
    SMKeyRequestRsp(cid, keys);
#else
    TYPED_BD_ADDR_T bd_addr;
    CMGetBdAdressFromDeviceId(device_id, &bd_addr);
    SMKeyRequestResponse(&bd_addr, keys);
#endif /* !CSR101x_A05 */
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALSMDivApprovalRsp
 *
 *  DESCRIPTION
 *      Handles the SM diversifier approval response
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void HALSMDivApprovalRsp(uint16 cid, sm_div_verdict approve_div)
{
#ifndef CSR101x_A05
    SMDivApprovalRsp(cid, approve_div);
#else
    SMDivApproval(cid, approve_div);
#endif /* !CSR101x_A05 */
}
#endif /* THIN_CM4_MESH_NODE */

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALConnectionParamUpdateReq
 *
 *  DESCRIPTION
 *      Requests for the connection parameters update
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void HALConnectionParamUpdateReq(device_handle_id device_id,
                              ble_con_params *new_params)
{
#ifndef CSR101x_A05
    uint16 cid = CMGetConnId(device_id);
    LsConnectionParamUpdateCidReq(cid, new_params);
#else
    TYPED_BD_ADDR_T  bd_addr;
    CMGetBdAdressFromDeviceId(device_id, &bd_addr);
    LsConnectionParamUpdateReq(&bd_addr, new_params);
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALConnectionUpdateSignallingRsp
 *
 *  DESCRIPTION
 *      Response to the Connection Parameter Update Signalling Indication
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void HALConnectionUpdateSignallingRsp(device_handle_id device_id,
                                            uint16  sig_identifier,
                                            bool    accepted)
{
#ifndef CSR101x_A05
    uint16 cid = CMGetConnId(device_id);
    LsConnectionUpdateSignallingRsp(cid, sig_identifier, accepted);
#else
    uint16  con_handle = getHciHandleFromDeviceId(device_id);
    LsConnectionUpdateSignalingRsp(con_handle, sig_identifier, accepted);
#endif /* !CSR101x_A05 */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HALParseConnectionUpdateInd
 *
 *  DESCRIPTION
 *      Parses the connection update indication
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void HALParseConnectionUpdateInd(h_ls_connection_update_ind_t *p_event_data,
                                        CM_DEV_CONN_PARAM_T *connection_param)
{
#ifndef CSR101x_A05
            connection_param->conn_interval = p_event_data->conn_interval;
            connection_param->conn_latency = p_event_data->conn_latency;
            connection_param->supervision_timeout = p_event_data->supervision_timeout;
#else
            HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T data  =
                    (HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T)p_event_data->data;

            connection_param->conn_interval = data.conn_interval;
            connection_param->conn_latency = data.conn_latency;
            connection_param->supervision_timeout = data.supervision_timeout;
#endif /* !CSR101x_A05 */
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HALMatchBluetoothAddress
 *
 *  DESCRIPTION
 *      Matches the given Bluetooth Addresses
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern bool HALMatchBluetoothAddress(TYPED_BD_ADDR_T *bdaddr1,
                                     TYPED_BD_ADDR_T *bdaddr2,
                                     uint16 *irk)
{
#ifndef CSR101x_A05
    uint16 index;
#endif /* !CSR101x_A05 */

    if (!CMIsAddressResolvableRandom(bdaddr1) &&
        !MemCmp(&bdaddr1->addr,
                &(bdaddr2->addr),
                sizeof(BD_ADDR_T)))
    {
        return TRUE;
    }
#ifndef CSR101x_A05
    else if(CMIsAddressResolvableRandom(bdaddr1) &&
                 SMPrivacyMatchAddress(bdaddr1, irk, 1, CM_MAX_WORDS_IRK,
                                       &index) == sys_status_success)
#else
    else if(CMIsAddressResolvableRandom(bdaddr1) &&
                 SMPrivacyMatchAddress(bdaddr1, irk, 1, CM_MAX_WORDS_IRK ) >= 0)
#endif /* !CSR101x_A05 */
    {
        return TRUE;
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMProcessMsg
 *
 *  DESCRIPTION
 *      Implements the event handler for Flashheart
 *
 *  RETURNS
 *     status_t: STATUS_SUCCESS if successful
 *
 *----------------------------------------------------------------------------*/

#ifndef CSR101x_A05
void CMProcessMsg(msg_t *msg)
{
    h_msg_t h_msg;
    h_msg.msg = msg;

    /* Invoke the CM Handler */
    CMProcessEvent(&h_msg);
}
#endif /* !CSR101x_A05 */


/*----------------------------------------------------------------------------*
 *  NAME
 *      CMProcessMsg
 *
 *  DESCRIPTION
 *      Implements the event handler for Baldrick
 *
 *  RETURNS
 *      bool: TRUE if successful
 *
 *----------------------------------------------------------------------------*/

#if defined (CSR101x_A05)
extern bool CMProcessMsg(lm_event_code event_code,
                              LM_EVENT_T *p_event_data)
{
    h_msg_t h_msg;
    baldrick_msg_t msg = {event_code, p_event_data};

    /* Typecast the baldrick message to the HAL event */
    h_msg.msg = (baldrick_msg_t *)&msg;

    /* Invoke the CM Handler */
    CMProcessEvent(&h_msg);

    return TRUE;
}
#endif /* CSR101x_A05 */
