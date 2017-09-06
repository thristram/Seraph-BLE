/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_common.c
 *
 *  DESCRIPTION
 *      This file implements some of the common CM API functions
 *
 ******************************************************************************/
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <gatt.h>
#include <bluetooth.h>
#include <mem.h>
#include <timer.h>
#ifndef CSR101x_A05
#include <streams.h>
#endif /* CSR101x_A05 */
#include <security.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"
#include "cm_private.h"
#include "cm_client.h"
#include "cm_server.h"
#include "cm_security.h"
#include "cm_peripheral.h"
#include "cm_central.h"
#include "cm_observer.h"


/*============================================================================*
 *  Private Function Declarations
 *============================================================================*/
static void asyncApiTimeout(timer_id tid);

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      asyncApiTimeout
 *
 *  DESCRIPTION
 *      Asynchronus api timeout handler
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void asyncApiTimeout(timer_id tid)
{
    CM_INIT_CFM_T cm_init_cfm;

    cm_init_cfm.status = sys_status_success;

    /* Send CM init confirmation event to the application */
    CMNotifyEventToApplication(CM_INIT_CFM,
                               (CM_EVENT_T *)&cm_init_cfm);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMInit
 *
 *  DESCRIPTION
 *      Initialises the Connection Manager
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void CMInit(CM_INIT_PARAMS_T *cm_init_params)
{
    bool db_reg_wait = FALSE;

    /* Initialise GATT entity.*/
    GattInit();

    /* Initialise CM Security */
    CMSecurityInit(cm_init_params);

    /* Initialise the CM Private */
    CMPrivateInit(cm_init_params);
    
#if defined (OBSERVER)
    CMObserverInit();
#endif /* CENTRAL */

#if defined (CENTRAL)
    CMCentralInit();
#endif /* CENTRAL */

#if defined (PERIPHERAL)
    CMPeripheralInit();
#endif /* PERIPHERAL */

#if defined (CLIENT)
    /* Initialise CM Client */
    CMClientInit(cm_init_params);
#endif /* CLIENT */

#if defined (SERVER)
    /* Initialise CM Server */
    db_reg_wait = CMServerInit(cm_init_params);
#endif /* SERVER */

    /* Check if the gatt database being registered */
    if(!db_reg_wait)
    {
        /* Start async api timer  */
        TimerCreate(0, TRUE, asyncApiTimeout);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMDisconnect
 *
 *  DESCRIPTION
 *      Disconnects the active link
 *
 *  RETURNS/MODIFIES
 *      cm_status_code: cm_status_success if the request is processed
 *
 *----------------------------------------------------------------------------*/
extern cm_status_code CMDisconnect(device_handle_id device_id)
{
    if(CMIsDeviceIdValid(device_id))
    {
        CMSetDevState(device_id, dev_state_disconnecting);

        return cm_status_success;
    }

    return cm_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetDeviceBondState
 *
 *  DESCRIPTION
 *      Gets the bonded state of the device
 *
 *  RETURNS
 *      cm_dev_bond_state: cm_dev_bonded if the device is bonded
 *
 *---------------------------------------------------------------------------*/
extern cm_dev_bond_state CMGetDeviceBondState(device_handle_id device_id)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();

    return cm_main_data->cm_conn_info[device_id].bond_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetDevState
 *
 *  DESCRIPTION
 *      Gets the device's current state
 *
 *  RETURNS
 *      cm_dev_state: dev_state_connected if the device is connected
 *
 *---------------------------------------------------------------------------*/
extern cm_dev_state CMGetDevState(device_handle_id device_id)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();

    if(!CMIsDeviceIdValid(device_id))
    {
        return dev_state_unknown;
    }

    return cm_main_data->cm_conn_info[device_id].device_state;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetPeerDeviceRole
 *
 *  DESCRIPTION
 *      Gets the connection role of the peer device
 *
 *  RETURNS
 *      peer_con_role: Role of the peer device
 *
 *---------------------------------------------------------------------------*/
extern cm_peer_con_role CMGetPeerDeviceRole(device_handle_id device_id)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();

    if(!CMIsDeviceIdValid(device_id))
    {
        return dev_state_unknown;
    }

    return cm_main_data->cm_conn_info[device_id].peer_con_role;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondIdFromDeviceId
 *
 *  DESCRIPTION
 *      Gets bond id of the device
 *
 *  RETURNS
 *      bond_handle_id: Index in the bonded device list
 *
 *---------------------------------------------------------------------------*/

extern bond_handle_id CMGetBondIdFromDeviceId(device_handle_id device_id)
{
    CM_MAIN_DATA_T* cm_main_data;
    cm_main_data = CMGetMainData();

    if(cm_main_data->cm_conn_info[device_id].bond_state == cm_dev_bonded)
    {
        return CMGetBondId(
                &cm_main_data->cm_conn_info[device_id].remote_bd_addr);
    }

    return CM_INVALID_BOND_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetDeviceIdFromBDAddress
 *
 *  DESCRIPTION
 *      Gets the device id from the Bluetooth Address
 *
 *  RETURNS
 *      device_handle_id: Index in the connected device list
 *
 *---------------------------------------------------------------------------*/
extern device_handle_id CMGetDeviceIdFromBDAddress(TYPED_BD_ADDR_T *bd_addr)
{
    device_handle_id index;
    CM_MAIN_DATA_T *cm_main_data = CMGetMainData();

    /* Parse through the Db to find the device id comparing the BD Address */
    for(index = 0; index < cm_main_data->max_connections; index++)
    {
       if(cm_main_data->cm_conn_info[index].cid != CM_GATT_INVALID_UCID &&
            !MemCmp(&cm_main_data->cm_conn_info[index].remote_bd_addr,
                       bd_addr,sizeof(TYPED_BD_ADDR_T)))
        {
            return index;
        }
    }

    return CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBdAdressFromDeviceId
 *
 *  DESCRIPTION
 *      Gets the Bluetooth Address of the device
 *
 *  RETURNS
 *      bool: TRUE if the request is processed
 *
 *---------------------------------------------------------------------------*/
extern bool CMGetBdAdressFromDeviceId(device_handle_id device_id,
                                             TYPED_BD_ADDR_T *bd_addr)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();

    if(!CMIsDeviceIdValid(device_id))
    {
        return FALSE;
    }

    MemCopy(bd_addr, &cm_main_data->cm_conn_info[device_id].remote_bd_addr,
            sizeof(TYPED_BD_ADDR_T));

    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMConnParamUpdateReq
 *
 *  DESCRIPTION
 *      Requests for the connection parameters update
 *
 *  RETURNS
 *      cm_status_code: Connection manager status code
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code CMConnParamUpdateReq(device_handle_id device_id,
                              ble_con_params *new_params)
{
    HALConnectionParamUpdateReq(device_id, new_params);

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMConnectionUpdateSignalingRsp
 *
 *  DESCRIPTION
 *      Response to the Connection Parameter Update Signalling Indication
 *
 *  RETURNS
 *      cm_status_code: Connection manager status code
 *
 *---------------------------------------------------------------------------*/
extern void CMConnectionUpdateSignalingRsp(device_handle_id device_id,
                                            uint16  sig_identifier,
                                            bool    accepted)
{
    HALConnectionUpdateSignallingRsp(device_id, sig_identifier, accepted);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetDevConnParam
 *
 *  DESCRIPTION
 *      Gets the device connection parameters
 *
 *  RETURNS
 *      bool: if the request is successful
 *
 *---------------------------------------------------------------------------*/
extern bool CMGetDevConnParam(device_handle_id device_id,
                              CM_DEV_CONN_PARAM_T *conn_params)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();

    if(!CMIsDeviceIdValid(device_id))
    {
        return FALSE;
    }

    *conn_params = cm_main_data->cm_conn_info[device_id].conn_param;

    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMConfigureRadioEvent
 *
 *  DESCRIPTION
 *      Configures for the Radio Events
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void CMConfigureRadioEvent(CM_REQ_RADIO_EVENT_T* cm_radio_event)
{
    uint16 cid = CMGetConnId(cm_radio_event->device_id);

    /* Enable or Disable Radio Tx events. */
    LsRadioEventNotification(cid,cm_radio_event->event_type);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMHoldTxEvent
 *
 *  DESCRIPTION
 *      Enables or disables delayed data packet transmission at the radio
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void CMHoldTxEvent(CM_REQ_HOLD_TX_EVENT_T* cm_hold_tx_event)
{
    uint16 cid = CMGetConnId(cm_hold_tx_event->device_id);

    /* Enable or Disable Radio Tx events. */
    LsRadioEventNotification(cid,cm_hold_tx_event->mode);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMReadRssi
 *
 *  DESCRIPTION
 *      Return the last Received Signal Strength Indication for a connection
 *
 *  RETURNS/MODIFIES
 *      ls_err error code
 *
 *----------------------------------------------------------------------------*/
extern ls_err CMReadRssi(device_handle_id device_id,int8* rssi)
{
    ls_err result;
    uint16 cid = CMGetConnId(device_id);
    result = LsReadRssi(cid,rssi);
    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMUuidEqual
 *
 *  DESCRIPTION
 *      Compares the given UUIDS
 *
 *  RETURNS
 *      bool: TRUE if the UUID's are eqaul
 *
 *---------------------------------------------------------------------------*/

extern bool CMUuidEqual(CM_UUID_T *uuid_1, CM_UUID_T *uuid_2)
{
    /* Check the UUID to see whether it is one of interest */
    if((uuid_1->uuid_type == GATT_UUID16 && uuid_2->uuid_type == GATT_UUID16))
    {
        return (uuid_1->uuid[0] == uuid_2->uuid[0]);
    }
    else if((uuid_1->uuid_type == GATT_UUID128 &&
             uuid_2->uuid_type == GATT_UUID128))
    {
        if( uuid_1->uuid[0] == uuid_2->uuid[0] &&
            uuid_1->uuid[1] == uuid_2->uuid[1] &&
            uuid_1->uuid[2] == uuid_2->uuid[2] &&
            uuid_1->uuid[3] == uuid_2->uuid[3] &&
            uuid_1->uuid[4] == uuid_2->uuid[4] &&
            uuid_1->uuid[5] == uuid_2->uuid[5] &&
            uuid_1->uuid[6] == uuid_2->uuid[6] &&
            uuid_1->uuid[7] == uuid_2->uuid[7] )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMRemoveBond
 *
 *  DESCRIPTION
 *      Deletes the bonding information
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMRemoveBond(bond_handle_id bond_id)
{
    CM_BONDED_DEVICE_INFO_T bonded_device;
    device_handle_id device_id;

    if(!CMIsBondIdValid(bond_id))
        return;

    /* Get the bonded device information */
    if(!CMGetBondedDevice(bond_id, &bonded_device))
        return;

    /* Get the connection status */
    device_id = CMGetDeviceIdFromBDAddress(&bonded_device.bd_addr);

#if defined(PERIPHERAL)
    /* Get the device role */
    if(CMGetBondedDeviceRole(&bonded_device.bd_addr) == con_role_central &&
                                       device_id == CM_INVALID_DEVICE_ID)
    {
        /* Let the peripheral entity handle */
        CMPeripheralRemovePairing(bond_id);

        return;
    }
#endif /* PERIPHERAL */

#if defined(CENTRAL)
    if(CMGetBondedDeviceRole(&bonded_device.bd_addr) == con_role_peripheral
                    && device_id == CM_INVALID_DEVICE_ID)
    {
        /* Device isn't connected so clear the bonding */
        ClearBondingInfo(bond_id);

        return;
    }
#endif /* CENTRAL */

    if(device_id != CM_INVALID_DEVICE_ID)
    {
        if(CMGetDevState(device_id) == dev_state_connected)
        {
            CMSetDeviceBondState(device_id, cm_dev_unbonding);

            /* disconnect the link */
            CMDisconnect(device_id);
        }
        else if(CMGetDevState(device_id) == dev_state_disconnecting)
        {
            CMSetDeviceBondState(device_id, cm_dev_unbonding);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetConnectedDevices
 *
 *  DESCRIPTION
 *      Gets the connected device list
 *
 *  RETURNS
 *      bool: TRUE if the request is processed
 *---------------------------------------------------------------------------*/
extern bool CMGetConnectedDevices(CM_CONNECTED_DEVICE_T *conn_dev,
                                                        uint16 *num_conn)
{
    uint16 index;
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();
    CM_CONN_INFO_T* p_conn_info = &cm_main_data->cm_conn_info[0];
    *num_conn = 0;

    if(conn_dev == NULL)
        return FALSE;

    /* Parse through the Db to find the device id comparing the BD Address */
    for(index = 0; index < cm_main_data->max_connections; index++)
    {
        if(p_conn_info[index].cid != CM_GATT_INVALID_UCID)
        {
            conn_dev[*num_conn].bd_addr = p_conn_info[index].remote_bd_addr;
            conn_dev[*num_conn].role =
                    p_conn_info[index].peer_con_role;
            conn_dev[*num_conn].device_id = index;

            ++(*num_conn);
        }
    }

    return TRUE;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CMIsAddressResolvableRandom
 *
 *  DESCRIPTION
 *      Checks if the Bluetooth address is resolvable random or not
 *
 *  RETURNS/MODIFIES
 *      bool - TRUE if the Bluetooth address is resolvable random
 *
 *----------------------------------------------------------------------------*/

extern bool CMIsAddressResolvableRandom(TYPED_BD_ADDR_T *bd_addr)
{
    if (bd_addr->type != L2CA_RANDOM_ADDR_TYPE ||
        (bd_addr->addr.nap & BD_ADDR_NAP_RANDOM_TYPE_MASK)
                                      != BD_ADDR_NAP_RANDOM_TYPE_RESOLVABLE)
    {
        /* This isn't a resolvable private address... */
        return FALSE;
    }
    return TRUE;
}

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMStreamGattSink
 *
 *  DESCRIPTION
 *      Create A Stream Gatt Sink
 *
 *  RETURNS/MODIFIES
 *      returns a handle to the stream endpoint
 *
 *----------------------------------------------------------------------------*/

extern handle_t CMStreamGattSink(cm_audio_open_param_t* open_info)
{
    gatt_open_param_t gatt_info;
    gatt_info.handle = open_info->handle;
    gatt_info.char_len = open_info->char_len;
    gatt_info.cid = CMGetConnId(open_info->device_id);
    return StreamGattSink(&gatt_info);
}
#endif /* !CSR101x_A05 */


/*----------------------------------------------------------------------------*
 *  NAME
 *      CMAddWhitelistDevice
 *
 *  DESCRIPTION
 *      Add a device to the whitelist
 *
 *  RETURNS/MODIFIES
 *      bool - TRUE on success
 *
 *----------------------------------------------------------------------------*/

extern bool CMAddWhitelistDevice(bond_handle_id bond_id)
{
    TYPED_BD_ADDR_T bd_addr;

    if(bond_id != CM_INVALID_BOND_ID)
    {
        /* Get the Bluetooth Address from Bond Id */
        CMGetBondedBDAddr(bond_id, &bd_addr);

        if(bd_addr.type != L2CA_RANDOM_ADDR_TYPE)
        {
            if(LsAddWhiteListDevice(&bd_addr) != ls_err_none)
                CMReportPanic(cm_panic_add_whitelist);
            else
                return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMRemoveWhitelistDevice
 *
 *  DESCRIPTION
 *      Removes a device from the whitelist
 *
 *  RETURNS/MODIFIES
 *      bool - TRUE on success
 *
 *----------------------------------------------------------------------------*/

extern bool CMRemoveWhitelistDevice(bond_handle_id bond_id)
{
    TYPED_BD_ADDR_T bd_addr;

    if(bond_id != CM_INVALID_BOND_ID)
    {
        /* Get the Bluetooth Address from Bond Id */
        CMGetBondedBDAddr(bond_id, &bd_addr);

        if(bd_addr.type != L2CA_RANDOM_ADDR_TYPE)
        {
            if(LsDeleteWhiteListDevice(&bd_addr) != ls_err_none)
                CMReportPanic(cm_panic_add_whitelist);
            else
                return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMMatchConnectedAddress
 *
 *  DESCRIPTION
 *      This function checks if the connected address matches with the known
 *      address
 *
 *  RETURNS/MODIFIES
 *      Boolean - True (If address matches)
 *
 *----------------------------------------------------------------------------*/

extern bool CMMatchConnectedAddress(TYPED_BD_ADDR_T *con_addr,
                                       TYPED_BD_ADDR_T *known_addr, uint16 *irk)
{
    bool status;

    status = HALMatchBluetoothAddress(con_addr,known_addr,irk);

    return status;
}

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMEnableEarlyWakeup
 *
 *  DESCRIPTION
 *      Enable or disable notifications to the application of an impending
 *      connection events.
 *
 *  RETURNS/MODIFIES
 *      ls_err error code
 *
 *----------------------------------------------------------------------------*/

extern ls_err CMEnableEarlyWakeup(device_handle_id device_id, uint16 interval)
{
    return LsEnableEarlyWakeup(CMGetConnId(device_id), interval);
}
#endif /* CSR101x_A05 */
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetRandomAddress
 *
 *  DESCRIPTION
 *      This functions sets the gap random address
 *
 *
 *  RETURNS/MODIFIES
 *      none
 *
 *----------------------------------------------------------------------------*/
extern void CMSetRandomAddress(BD_ADDR_T bd_addr)
{
    GapSetRandomAddress(&bd_addr);
}

 /*----------------------------------------------------------------------------*
 *  NAME
 *      CMSignalNumberOfCompletedPackets
 *
 *  DESCRIPTION
 *      Enable number of completed packets
 *
 *
 *  RETURNS/MODIFIES
 *      none
 *
 *----------------------------------------------------------------------------*/
extern void CMSignalNumberOfCompletedPackets(void)
{
    CM_MAIN_DATA_T* cm_main_data = CMGetMainData();
    cm_main_data->num_completed_packets_enabled = TRUE;
}
