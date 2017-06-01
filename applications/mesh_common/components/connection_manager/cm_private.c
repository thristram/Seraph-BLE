/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_private.c
 *
 *  DESCRIPTION
 *      This file defines connection manager private functionality
 *
 *
 ******************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <gatt.h>
#include <security.h>
#include <panic.h>
#include <bluetooth.h>
#include <mem.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_private.h"
#include "cm_types.h"
#include "cm_api.h"
#include "cm_security.h"
#include "cm_client.h"
#include "cm_server.h"
#include "cm_central.h"
#include "cm_peripheral.h"
#include "cm_hal.h"
#include "cm_observer.h"

/* Macro to check whether the device is valid */
#define VALID_DEVICE_ID(device_id) (device_id < g_cm_main_data.max_connections)

/* Default ATT MTU SIZE */
#define APP_MTU_DEFAULT            (23)

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Connection manager main data structure which could be used by other
   connection manager components such as conn manager client, server, central,
   peripheral, security and applications */
static CM_MAIN_DATA_T g_cm_main_data;

/*============================================================================*
 *  Private Function Declarations
 *============================================================================*/
static device_handle_id addNewDevice(h_ls_connection_complete_ind_t
                                     *p_event_data);

/* Gets the device id from the Bluetooth Address during connecting state */
static device_handle_id getDeviceFromBdAddr(TYPED_BD_ADDR_T *bd_addr);

/* Sets the connection information in the active connections database  */
static device_handle_id setConnInfo(TYPED_BD_ADDR_T *bd_addr,
                            uint16 conn_id);

/* Handles the signal HCI_EV_DATA_ULP_CONNECTION_COMPLETE_T */
static void handleSignalLmEvConnectionComplete
                    (h_ls_connection_complete_ind_t* p_event_data);

/* Handles the signal GATT_CONNECT_CFM */
static void handleSignalGattConnectCfm(h_gatt_connect_cfm_t *p_event_data);

/* Handles the connection confirmation message
 * (HCI_EV_DATA_DISCONNECT_COMPLETE_T)
 */
static void handleSignalLmDisconnectComplete(h_disconnect_complete_ind_t
                                             *p_event_data);

/* Handles the signal LS_RADIO_EVENT_IND */
static void handleSignalLsRadioEventInd(h_ls_radio_event_ind_t* p_event_data);

/* Handles the signal HAL_LS_NUMBER_COMPLETED_PACKETS_IND */
static void handleSignaNumberOfCompletedPacketsEventInd(h_ls_number_completed_packets_ind_t* p_event_data);

#ifndef CSR101x_A05
/* Handles the signal LS_EARLY_WAKEUP_IND */
static void handleSignalLsEarlyWakeUpInd(h_ls_early_wakeup_ind_t *p_event_data);
#endif

/* This function handles HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND */
static void handleSignalLsConnUpdateSignallingInd(
        h_ls_connection_update_signalling_ind_t *p_event_data);

/* This function handles HAL_LS_CONNECTION_PARAM_UPDATE_IND */
static void handleSignalLsConnParamUpdateInd(
        h_ls_connection_param_update_ind_t *p_event_data);

/* This function handles HAL_LS_CONNECTION_PARAM_UPDATE_CFM */
static void handleSignalLsConnParamUpdateCfm(
        h_ls_connection_param_update_cfm_t *p_event_data);

/* This function handles HAL_LS_CONNECTION_UPDATE_IND */
static void handleSignalLsConnectionUpdate(
        h_ls_connection_update_ind_t *p_event_data);

#ifndef THIN_CM4_MESH_NODE
/* Handles the signal LS_DATA_RX_TIMING_IND */
static void handleSignalLsDataRxTimingInd(h_ls_data_rx_timing_ind_t *p_event_data);
#endif /* THIN_CM4_MESH_NODE */

/* This function handles HAL_GATT_EXCHANGE_MTU_IND */
static void handleSignalGattExchangeMtuInd(
        h_gatt_exchange_mtu_ind_t *p_event_data);

/*============================================================================*
 *  Private Function Implementation
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      addNewDevice
 *
 *  DESCRIPTION
 *      Adds a device onto active connections database
 *
 *  RETURNS
 *      device_handle_id: Device id of the newly added device
 *
 *---------------------------------------------------------------------------*/

static device_handle_id addNewDevice(h_ls_connection_complete_ind_t *p_event_data)
{
    bond_handle_id index;

    CM_CONN_INFO_T* p_conn_info = &g_cm_main_data.cm_conn_info[0];

    for(index = 0; index < g_cm_main_data.max_connections; index++)
    {
        /* Find the unconnected device in the GATT database */
        if(p_conn_info[index].cid == CM_GATT_INVALID_UCID)
        {
            HALAddNewDevice(&p_conn_info[index], p_event_data);
            return index;
        }
    }
    /* We did not find a slot to store the newly added device. Hence returning
           an invalid device id back
     */
    return CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getDeviceFromBdAddr
 *
 *  DESCRIPTION
 *      Gets the device id from the Bluetooth Address during connecting state
 *
 *  RETURNS
 *      device_handle_id: Index in the connected device list
 *
 *---------------------------------------------------------------------------*/

static device_handle_id getDeviceFromBdAddr(TYPED_BD_ADDR_T *bd_addr)
{
    device_handle_id index;

    /* Parse through the Db to find the device id comparing the BD Address */
    for(index = 0; index < g_cm_main_data.max_connections; index++)
    {
       if(!MemCmp(&g_cm_main_data.cm_conn_info[index].remote_bd_addr,
                       bd_addr, sizeof(TYPED_BD_ADDR_T)))
        {
            return index;
        }
    }

    return CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      setConnInfo
 *
 *  DESCRIPTION
 *      Sets the connection information in the active connections
 *      database
 *
 *  RETURNS
 *      device_handle_id: device id of the device
 *
 *---------------------------------------------------------------------------*/

static device_handle_id setConnInfo(TYPED_BD_ADDR_T *bd_addr,
                            uint16 conn_id)
{
    device_handle_id device_id = getDeviceFromBdAddr(bd_addr);
    if(device_id != CM_INVALID_DEVICE_ID)
    {
        g_cm_main_data.cm_conn_info[device_id].cid = conn_id;
    }
    else
    {
        CMReportPanic(cm_panic_invalid_device_id);
    }
    return device_id;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalLmEvConnectionComplete
 *
 *  DESCRIPTION
 *      Handles the signal HCI_EV_DATA_ULP_CONNECTION_COMPLETE_T
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLmEvConnectionComplete(
                        h_ls_connection_complete_ind_t *p_event_data)
{
    if(p_event_data->status == sys_status_success)
    {
        device_handle_id device_id;
        CM_DEV_CONN_PARAM_T connection_param;

        /* copy the connection parameters received for the connection */
        connection_param.conn_interval = p_event_data->conn_interval;
        connection_param.conn_latency = p_event_data->conn_latency;
        connection_param.supervision_timeout =
                p_event_data->supervision_timeout;

        /* Add a new device onto the device database. */
        device_id = addNewDevice(p_event_data);

        if(device_id != CM_INVALID_DEVICE_ID)
        {
            /* set the initial state */
            CMSetDeviceBondState(device_id, cm_dev_unbonded);

            /* Check if the device is already bonded */
            if(CMIsDeviceBonded(&g_cm_main_data.cm_conn_info[device_id].
                                remote_bd_addr))
            {
                /* update the device bond state */
                CMSetDeviceBondState(device_id, cm_dev_bonded);
            }

            /* Set the device connection parameters */
            CMSetConnParam(device_id, &connection_param);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalGattConnectCfm
 *
 *  DESCRIPTION
 *      Handles the signal GATT_CONNECT_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalGattConnectCfm(h_gatt_connect_cfm_t *p_event_data)
{
    CM_CONNECTION_NOTIFY_T cm_connect_notify;

    if(p_event_data->result == sys_status_success)
    {
        device_handle_id device_id;

        /* Store the connection identifer of the connection. */
        device_id = setConnInfo(&p_event_data->bd_addr,
                                  p_event_data->cid);

        /* Change the device state to connected. */
        CMSetDevState(device_id, dev_state_connected);

        /* Copy the device id */
        cm_connect_notify.device_id = device_id;

        /* Prepare the send connect confirm message */
        cm_connect_notify.result = cm_conn_res_success;
    }
    else if(p_event_data->result != HCI_ERROR_DIRECTED_ADVERTISING_TIMEOUT)
    {
        /* Copy the device id */
        cm_connect_notify.device_id = CM_INVALID_DEVICE_ID;

        /* Set the connection notify event state to connect failed */
        cm_connect_notify.result = cm_conn_res_failed;
    }
    else
    {
        /* It could be the high duty cycle directed
         * advertisement timeout error. Just ignore.
         */
        return;
    }

#if defined (SERVER)
    /* Send connect event to all the client handlers regd */
    CMServerNotifyGenericEvent(CM_CONNECTION_NOTIFY,
                     (CM_EVENT_T *)&cm_connect_notify);
#endif /* SERVER */

#if defined (CLIENT)
    /* Send connect event to all the client handlers regd*/
    CMClientNotifyGenericEvent(CM_CONNECTION_NOTIFY,
                          (CM_EVENT_T *)&cm_connect_notify);
#endif /* CLIENT */

    /* Send connect event to the application */
    CMNotifyEventToApplication(CM_CONNECTION_NOTIFY,
                         (CM_EVENT_T *)&cm_connect_notify);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalLmDisconnectComplete
 *
 *  DESCRIPTION
 *      Handles the connection confirmation message
 *      (HCI_EV_DATA_DISCONNECT_COMPLETE_T)
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLmDisconnectComplete(
        h_disconnect_complete_ind_t *p_event_data)
{
    CM_CONNECTION_NOTIFY_T  cm_connect_notify;
    TYPED_BD_ADDR_T bd_addr;

    device_handle_id device_id = CMGetDeviceId(HAL_DISCONNECT_COMPLETE_IND,
                                     (void*)p_event_data);
    if(device_id == CM_INVALID_DEVICE_ID)
        return;

    /* Get the bluetooth address of the device */
    CMGetBdAdressFromDeviceId(device_id, &bd_addr);

    /* Reset the connection parameter variables. */
    CMResetDevConnParam(device_id);

    switch(CMGetDevState(device_id))
    {
        case dev_state_connected:
        /* FALLTHROUGH */
        case dev_state_disconnecting:
        {
            /* Check whether we are waiting for disconnection of the link in
             * the case of device being unpaired
             */
            if(CMGetDeviceBondState(device_id) == cm_dev_unbonding)
            {
                ClearBondingInfo(CMGetBondId(&bd_addr));
            }

            /* Enter idle state. */
            CMSetDevState(device_id, dev_state_disconnected);

            /* Prepare the disconnect confirm message */
            cm_connect_notify.reason = p_event_data->reason;
            cm_connect_notify.device_id = device_id;
            cm_connect_notify.result = cm_disconn_res_success;

            /* Send connect event to all the server/client handlers regd */
#if defined (SERVER)
            CMServerNotifyGenericEvent(CM_CONNECTION_NOTIFY,
                                  (CM_EVENT_T *)&cm_connect_notify);
#endif /* SERVER */

#if defined (CLIENT)
            CMClientNotifyGenericEvent(CM_CONNECTION_NOTIFY,
                                  (CM_EVENT_T *)&cm_connect_notify);
#endif /*CLIENT  */

            /* Send disconnect event to application*/
            CMNotifyEventToApplication(CM_CONNECTION_NOTIFY,
                                     (CM_EVENT_T *)&cm_connect_notify);
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

/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsRadioEventInd
 *
 *  DESCRIPTION
 *       Handles the signal LS_RADIO_EVENT_IND.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsRadioEventInd(h_ls_radio_event_ind_t *p_event_data)
{
    CM_RADIO_EVENT_IND_T radio_event_ind;

    radio_event_ind.device_id = CMGetDeviceId(HAL_LS_RADIO_EVENT_IND,
                                     (void*)p_event_data);

    radio_event_ind.radio = p_event_data->radio;

    HALParseRadioEventInd(&radio_event_ind, p_event_data);

    /* Send radio event to the application */
    CMNotifyEventToApplication(CM_RADIO_EVENT_IND,
                               (CM_EVENT_T *)&radio_event_ind);

}


/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignaNumberOfCompletedPacketsEventInd
 *
 *  DESCRIPTION
 *       Handles the signal HAL_LS_NUMBER_COMPLETED_PACKETS_IND.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignaNumberOfCompletedPacketsEventInd(h_ls_number_completed_packets_ind_t *p_event_data)
{
    if (g_cm_main_data.num_completed_packets_enabled)
    {
        /* By design we only send out CM_NUMBER_OF_COMPLETED_PKTS_IND onces per 
         * enable request this is to prevent flood the system.
         */
	    g_cm_main_data.num_completed_packets_enabled = FALSE;
#if defined (SERVER)
        /* Send connect event to all the client handlers regd */
        CMServerNotifyGenericEvent(CM_NUMBER_OF_COMPLETED_PKTS_IND,
                         (CM_EVENT_T *)p_event_data);
#endif /* SERVER */

#if defined (CLIENT)
        /* Send connect event to all the client handlers regd*/
        CMClientNotifyGenericEvent(CM_NUMBER_OF_COMPLETED_PKTS_IND,
                              (CM_EVENT_T *)p_event_data);
#endif /* CLIENT */

        /* Send radio event to the application */
        CMNotifyEventToApplication(CM_NUMBER_OF_COMPLETED_PKTS_IND,
                                   (CM_EVENT_T *)p_event_data);
    }
}

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsEarlyWakeUpInd
 *
 *  DESCRIPTION
 *       Handles the signal LS_EARLY_WAKEUP_IND.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsEarlyWakeUpInd(h_ls_early_wakeup_ind_t *p_event_data)
{
    CM_EARLY_WAKEUP_IND_T early_wakeup_ind;

    early_wakeup_ind.device_id = CMGetDeviceId(HAL_LS_EARLY_WAKEUP_IND,
                                     (void*)p_event_data);
    early_wakeup_ind.interval = p_event_data->interval;

    /* Send radio event to the application */
    CMNotifyEventToApplication(CM_EARLY_WAKEUP_IND,
                               (CM_EVENT_T *)&early_wakeup_ind);

}
#endif

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsDataRxTimingInd
 *
 *  DESCRIPTION
 *       Handles the signal LS_DATA_RX_TIMING_IND.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsDataRxTimingInd(h_ls_data_rx_timing_ind_t *p_event_data)
{
    CM_DATA_RX_TIMING_IND_T rx_timing_event_ind;

    rx_timing_event_ind.device_id = CMGetDeviceId(HAL_LS_DATA_RX_TIMING_IND,
                                     (void*)p_event_data);

    rx_timing_event_ind.tx_duration = p_event_data->tx_duration;

    MemCopy(rx_timing_event_ind.tx_event_offset,p_event_data->tx_event_offset,3);

    MemCopy(rx_timing_event_ind.tx_transmit_offset,p_event_data->tx_transmit_offset,3);

    /* Send timimg event to the application */
    CMNotifyEventToApplication(CM_DATA_RX_TIMING_IND,
                               (CM_EVENT_T *)&rx_timing_event_ind);

}
#endif /* THIN_CM4_MESH_NODE */

/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsConnUpdateSignallingInd
 *
 *  DESCRIPTION
 *       This function handles HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsConnUpdateSignallingInd(
        h_ls_connection_update_signalling_ind_t
        *p_event_data)
{
    CM_CONNECTION_UPDATE_SIGNALLING_IND_T conn_update_signalling_ind;

    conn_update_signalling_ind.device_id = CMGetDeviceId(
            HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND, (void*)p_event_data);

    conn_update_signalling_ind.sig_identifier
            = p_event_data->sig_identifier;

    conn_update_signalling_ind.conn_interval_min
            = p_event_data->conn_interval_min;

    conn_update_signalling_ind.conn_interval_max
            = p_event_data->conn_interval_max;

    conn_update_signalling_ind.slave_latency
            = p_event_data->slave_latency;

    conn_update_signalling_ind.supervision_timeout
            = p_event_data->supervision_timeout;

    /* Send connection params update singalling ind to the application */
    CMNotifyEventToApplication(CM_CONNECTION_UPDATE_SIGNALLING_IND,
                               (CM_EVENT_T *)&conn_update_signalling_ind);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsConnParamUpdateInd
 *
 *  DESCRIPTION
 *       This function handles HAL_LS_CONNECTION_PARAM_UPDATE_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsConnParamUpdateInd(
        h_ls_connection_param_update_ind_t *p_event_data)
{
    CM_CONNECTION_PARAM_UPDATE_IND_T    conn_param_update_ind;

    conn_param_update_ind.device_id = CMGetDeviceId(
            HAL_LS_CONNECTION_PARAM_UPDATE_IND, (void*)p_event_data);

    conn_param_update_ind.status = p_event_data->status;

    MemCopy(&conn_param_update_ind.address, &p_event_data->address,
            sizeof(p_event_data->address));

    conn_param_update_ind.conn_interval = p_event_data->conn_interval;

    conn_param_update_ind.conn_latency = p_event_data->conn_latency;

    conn_param_update_ind.supervision_timeout = p_event_data->supervision_timeout;

    /* Send connection params update indication to the application */
    CMNotifyEventToApplication(CM_CONNECTION_PARAM_UPDATE_IND,
                               (CM_EVENT_T *)&conn_param_update_ind);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsConnParamUpdateCfm
 *
 *  DESCRIPTION
 *       This function handles HAL_LS_CONNECTION_PARAM_UPDATE_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsConnParamUpdateCfm(
        h_ls_connection_param_update_cfm_t *p_event_data)
{
    CM_CONNECTION_PARAM_UPDATE_CFM_T conn_param_update_cfm;

    conn_param_update_cfm.device_id = CMGetDeviceId(
            HAL_LS_CONNECTION_PARAM_UPDATE_CFM, (void*)p_event_data);

    conn_param_update_cfm.status = p_event_data->status;

    MemCopy(&conn_param_update_cfm.address, &p_event_data->address,
            sizeof(p_event_data->address));

    /* Send connection params update confirmation to the application */
    CMNotifyEventToApplication(CM_CONNECTION_PARAM_UPDATE_CFM,
                               (CM_EVENT_T *)&conn_param_update_cfm);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalLsConnectionUpdate
 *
 *  DESCRIPTION
 *       This function handles HAL_LS_CONNECTION_UPDATE_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsConnectionUpdate(
        h_ls_connection_update_ind_t *p_event_data)
{
    CM_CONNECTION_UPDATED_T conn_updated;
    CM_DEV_CONN_PARAM_T connection_param;
    device_handle_id device_id = CMGetDeviceId(
            HAL_LS_CONNECTION_UPDATE_IND, (void*)p_event_data);

    /* Parse the parameters */
    HALParseConnectionUpdateInd(p_event_data, &connection_param);

    /* Set the device connection parameters */
    CMSetConnParam(device_id, &connection_param);

    /* Create an event for the application */
    conn_updated.device_id = device_id;
    conn_updated.conn_interval = connection_param.conn_interval;
    conn_updated.conn_latency = connection_param.conn_latency;
    conn_updated.supervision_timeout = connection_param.supervision_timeout;

    /* Send connection params updated indication */
    CMNotifyEventToApplication(CM_CONNECTION_UPDATED,
                               (CM_EVENT_T *)&conn_updated);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *       handleSignalGattExchangeMtuInd
 *
 *  DESCRIPTION
 *       This function handles HAL_GATT_EXCHANGE_MTU_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalGattExchangeMtuInd(
        h_gatt_exchange_mtu_ind_t *p_event_data)
{
    GattExchangeMtuRsp(p_event_data->cid, APP_MTU_DEFAULT);
}

/*============================================================================*
 *  Public Function Implementation
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetMainData
 *
 *  DESCRIPTION
 *      Returns pointer to the connection database.
 *
 *  RETURNS
 *      CM_MAIN_DATA_T: pointer to g_cm_main_data.
 *---------------------------------------------------------------------------*/

extern CM_MAIN_DATA_T* CMGetMainData(void)
{
    return &g_cm_main_data;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMNotifyEventToApplication
 *
 *  DESCRIPTION
 *      Sends the connection manager event to the application.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMNotifyEventToApplication(cm_event core_event,
                                       CM_EVENT_T *p_event_data)
{
    if(g_cm_main_data.app_handlers->pCallback != NULL)
    {
        g_cm_main_data.app_handlers->pCallback(core_event, p_event_data);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPrivateInit
 *
 *  DESCRIPTION
 *      Initialises the CM Private.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMPrivateInit(CM_INIT_PARAMS_T *cm_init_params)
{
    uint16 index;

    index = sizeof(CM_CONN_INFO_T);

    /* Save the application handler */
    g_cm_main_data.app_handlers = cm_init_params->cm_app_handler;

    /* Save the connection information */
    g_cm_main_data.cm_conn_info = (CM_CONN_INFO_T*)cm_init_params->conn_info;
    g_cm_main_data.max_connections = cm_init_params->max_connections;

    /* Initialise active connections list */
    for(index = 0; index < g_cm_main_data.max_connections; index++)
    {
        g_cm_main_data.cm_conn_info[index].cid = CM_GATT_INVALID_UCID;
        g_cm_main_data.cm_conn_info[index].device_state
                = dev_state_disconnected;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetConnParam
 *
 *  DESCRIPTION
 *      Sets the connection parameters of the device
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMSetConnParam(device_handle_id device_id,
                              CM_DEV_CONN_PARAM_T *connection_param)
{

    if (VALID_DEVICE_ID(device_id) == FALSE)
    {
        CMReportPanic(cm_panic_invalid_device_id);
    }
    g_cm_main_data.cm_conn_info[device_id].conn_param = *connection_param;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMResetDevConnParam
 *
 *  DESCRIPTION
 *      Resets the device connection parameters
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMResetDevConnParam(device_handle_id device_id)
{
     g_cm_main_data.cm_conn_info[device_id].conn_param.conn_interval = 0;
     g_cm_main_data.cm_conn_info[device_id].conn_param.conn_latency = 0;
     g_cm_main_data.cm_conn_info[device_id].conn_param.supervision_timeout = 0;
}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetDeviceId
 *
 *  DESCRIPTION
 *      Gets the device id of the device
 *
 *  RETURNS
 *      device_handle_id: Device id of the device
 *
 *---------------------------------------------------------------------------*/
extern device_handle_id CMGetDeviceId(uint16 event_id, void *event)
{
    return HALGetDeviceId(event_id, event, &g_cm_main_data);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMIsDeviceIdValid
 *
 *  DESCRIPTION
 *      Checks the device id valid or not
 *
 *  RETURNS
 *      bool: TRUE for valid device id
 *
 *---------------------------------------------------------------------------*/
extern bool CMIsDeviceIdValid(device_handle_id device_id)
{
    return VALID_DEVICE_ID(device_id);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetConnId
 *
 *  DESCRIPTION
 *      Gets the connection id (cid) of the device
 *
 *  RETURNS
 *      uint16: cid
 *
 *---------------------------------------------------------------------------*/
extern uint16 CMGetConnId(device_handle_id device_id)
{
    if(VALID_DEVICE_ID(device_id) == FALSE)
    {
        CMReportPanic(cm_panic_invalid_device_id);
    }
    return g_cm_main_data.cm_conn_info[device_id].cid;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetDevState
 *
 *  DESCRIPTION
 *      Sets the device state in the connection manager
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMSetDevState(device_handle_id device_id, cm_dev_state new_state)
{
    cm_dev_state old_state =
            g_cm_main_data.cm_conn_info[device_id].device_state;

    if(VALID_DEVICE_ID(device_id) == FALSE)
    {
        CMReportPanic(cm_panic_invalid_device_id);
    }

    if(old_state == new_state)
        return;

    switch (new_state)
    {
        case dev_state_disconnecting:
        {
            GattDisconnectReq(CMGetConnId(device_id));
        }
        break;
        case dev_state_disconnected:
        {
            g_cm_main_data.cm_conn_info[device_id].cid =
                        CM_GATT_INVALID_UCID;
        }
        break;

        default:
            /* Nothing to do */
        break;
    }

    g_cm_main_data.cm_conn_info[device_id].device_state = new_state;

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetDeviceBondState
 *
 *  DESCRIPTION
 *      Sets the bonded flag for the device to true
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMSetDeviceBondState(device_handle_id device_id, cm_dev_bond_state bond_state)
{
    g_cm_main_data.cm_conn_info[device_id].bond_state = bond_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetDeviceEncryptState
 *
 *  DESCRIPTION
 *      Sets the encrypt state of the device to TRUE
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMSetDeviceEncryptState(device_handle_id device_id, bool enc_state)
{
    if(VALID_DEVICE_ID(device_id) == FALSE)
    {
        CMReportPanic(cm_panic_invalid_device_id);
    }
    g_cm_main_data.cm_conn_info[device_id].encryption_enabled = enc_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetDeviceEncryptState
 *
 *  DESCRIPTION
 *      Returns the device encrypt state for the device
 *
 *  RETURNS
 *      bool: TRUE if the device state is encrypted
 *
 *---------------------------------------------------------------------------*/
extern bool CMGetDeviceEncryptState(device_handle_id device_id)
{
    if(VALID_DEVICE_ID(device_id) == FALSE)
    {
        CMReportPanic(cm_panic_invalid_device_id);
    }
    return g_cm_main_data.cm_conn_info[device_id].encryption_enabled;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMReportPanic
 *
 *  DESCRIPTION
 *      Raises the panic
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void CMReportPanic(cm_panic_code panic_code)
{
    /* If we want any debug prints, we can put them here */
    Panic(panic_code);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMProcessEvent
 *
 *  DESCRIPTION
 *      This function is called whenever a LM-specific event is received by the
 *      system.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void CMProcessEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
#if defined (SERVER)
        case HAL_GATT_ADD_DB_CFM:
        case HAL_GATT_CHAR_VAL_NOTIFICATION_CFM:
        case HAL_GATT_CHAR_VAL_INDICATION_CFM:
        {
            CMServerHandleProcessLMEvent(msg);
        }
        break;
#endif /* SERVER */

#if defined (CENTRAL) || defined(OBSERVER)
        case HAL_ADVERTISING_REPORT_IND:
        {
#if defined (CENTRAL)        
            CMCentralHandleProcessLMEvent(msg);
#endif
#if defined (OBSERVER)
            CMObserverHandleProcessLMEvent(msg);
#endif /* CENTRAL */
        }
        break;
#endif /* (CENTRAL || OBSERVER) */

        case HAL_CONNECTION_COMPLETE_IND:
        {
            h_ls_connection_complete_ind_t conn_complete_ind;

            /* Parse the connection complete indication */
            HALParseConnCompleteInd(&conn_complete_ind, msg);

            handleSignalLmEvConnectionComplete(&conn_complete_ind);
        }
        break;

        case HAL_GATT_CONNECT_CFM:
        {
            handleSignalGattConnectCfm((h_gatt_connect_cfm_t*)HALGetMsg(msg));

#if defined (CENTRAL)
            CMCentralHandleProcessLMEvent(msg);
#endif /* CENTRAL */

#if defined (PERIPHERAL)
#ifndef THIN_CM4_MESH_NODE
            CMPeripheralHandleProcessLmEvent(msg);
#endif /* THIN_CM4_MESH_NODE */
#endif /* PERIPHERAL */

        }
        break;

        case HAL_DISCONNECT_COMPLETE_IND:
        {
            h_disconnect_complete_ind_t disconn_complete_ind;

#if defined (PERIPHERAL)
#ifndef THIN_CM4_MESH_NODE
            CMPeripheralHandleProcessLmEvent(msg);
#endif /* THIN_CM4_MESH_NODE */
#endif /* PERIPHERAL */

            /* Parse the disconnection complete indication */
            HALParseDisconnCompleteInd(&disconn_complete_ind, msg);

            handleSignalLmDisconnectComplete(&disconn_complete_ind);
        }
        break;

        /* SM and LM security events are forwarded to CM Security Handler */
        case HAL_SM_KEYS_IND:
            /* FALL THROUGH */
        case HAL_SM_LONG_TERM_KEY_IND:
           /* FALL THROUGH */
        case HAL_SM_PAIRING_AUTH_IND:
            /* FALL THROUGH */
        case HAL_SM_PAIRING_COMPLETE_IND:
            /* FALL THROUGH */
        case HAL_SM_DIV_APPROVAL_IND:
            /* FALL THROUGH */
        case HAL_SM_ENCRYPTION_CHANGE_IND:
            /* FALL THROUGH */
        case HAL_SM_PASSKEY_INPUT_IND:
            /* FALL THROUGH */
        case HAL_SM_KEY_REQUEST_IND:
        {
            CMSecurityHandleProcessLMEvent(msg);
        }
        break;

        /* The following event is received when the remote connected device
         * accesses a characteristic which is being handled by the application.
         */
        case HAL_GATT_ACCESS_IND:
        {
#if defined (PERIPHERAL)
#ifndef THIN_CM4_MESH_NODE

            CMPeripheralHandleProcessLmEvent(msg);
#endif /* THIN_CM4_MESH_NODE */
#endif /* PERIPHERAL */

#if defined (SERVER)
            CMServerHandleProcessLMEvent(msg);
#endif /* SERVER */
        }
        break;

        /* Radio event replacement - Application and services can be told when data has gone. */
        case HAL_LS_NUMBER_COMPLETED_PACKETS_IND:
            handleSignaNumberOfCompletedPacketsEventInd(
                      (h_ls_number_completed_packets_ind_t*)HALGetMsg(msg));
        break;
        
        /* Radio event- Application configures radio event on first tx of a
         * connection event.
         */
        case HAL_LS_RADIO_EVENT_IND:
        {
            /* Radio event- Application configures radio event on first tx of a
            * connection event.
            */
            handleSignalLsRadioEventInd(
                      (h_ls_radio_event_ind_t*)HALGetMsg(msg));
        }
        break;

#ifndef CSR101x_A05
        /* Handle the Early wake up indication received */
        case HAL_LS_EARLY_WAKEUP_IND:
        {
            handleSignalLsEarlyWakeUpInd(
                      (h_ls_early_wakeup_ind_t*)HALGetMsg(msg));
        }
        break;
#endif

#ifndef THIN_CM4_MESH_NODE
        case HAL_LS_DATA_RX_TIMING_IND:
        {
            /* Provides timing report to application on receiving data packet */
            handleSignalLsDataRxTimingInd((h_ls_data_rx_timing_ind_t *)HALGetMsg(msg));
        }
        break;
#endif /* THIN_CM4_MESH_NODE */

#if defined (CLIENT)
        /* The events related to the Discovery are forwarded to the connection
         * manager client handler function.
         */
        case HAL_GATT_SERV_INFO_IND:
        case HAL_GATT_DISC_ALL_PRIM_SERV_CFM:
        case HAL_GATT_CHAR_DECL_INFO_IND:
        case HAL_GATT_DISC_SERVICE_CHAR_CFM:
        case HAL_GATT_CHAR_DESC_INFO_IND:
        case HAL_GATT_DISC_ALL_CHAR_DESC_CFM:
        case HAL_GATT_WRITE_CHAR_VAL_CFM:
        case HAL_GATT_READ_CHAR_VAL_CFM:
        case HAL_GATT_NOTIFICATION_IND:
        case HAL_GATT_INDICATION_IND:
        {
            CMClientHandleProcessLMEvent(msg);
        }
        break;
#endif /* CLIENT */

        case HAL_GATT_CANCEL_CONNECT_CFM:
        {
#if defined (PERIPHERAL)
#ifndef THIN_CM4_MESH_NODE
            CMPeripheralHandleProcessLmEvent(msg);
#endif /* THIN_CM4_MESH_NODE */
#endif /* PERIPHERAL */

#if defined (CENTRAL)
            CMCentralHandleProcessLMEvent(msg);
#endif /* PERIPHERAL */
        }
        break;

        /* This event is received when the remote slave device sends a
         * L2CAP connection parameter request.
         */
        case HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND:
        {
            handleSignalLsConnUpdateSignallingInd(
                    (h_ls_connection_update_signalling_ind_t*)HALGetMsg(msg));
        }
        break;

        /* This event indicates completion of remotely triggered Connection
         * parameter update procedure
         */
        case HAL_LS_CONNECTION_PARAM_UPDATE_IND:
        {
            handleSignalLsConnParamUpdateInd(
                      (h_ls_connection_param_update_ind_t*)HALGetMsg(msg));
        }
        break;

        /* This event is received in response to the
         * connection parameter update request
         */
        case HAL_LS_CONNECTION_PARAM_UPDATE_CFM:
        {
            handleSignalLsConnParamUpdateCfm(
                      (h_ls_connection_param_update_cfm_t*)HALGetMsg(msg));
        }
        break;

        /* This event is sent by the controller on connection parameter
         * update.
         */
        case HAL_LS_CONNECTION_UPDATE_IND:
        {
            handleSignalLsConnectionUpdate(
                    (h_ls_connection_update_ind_t*)HALGetMsg(msg));
        }
        break;

        case HAL_GATT_EXCHANGE_MTU_IND:
        {
            handleSignalGattExchangeMtuInd(
                    (h_gatt_exchange_mtu_ind_t*)HALGetMsg(msg));
        }
        break;

        default:
            break;
    }
}
