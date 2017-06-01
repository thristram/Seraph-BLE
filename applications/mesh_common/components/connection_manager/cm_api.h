/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
 /*! \file cm_api.h
 *  \brief Defines the connection manager public API's
 */

#ifndef __CM_API_H__
#define __CM_API_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <bluetooth.h>
#include <bt_event_types.h>

/*============================================================================*
 *  local Header Files
 *============================================================================*/
 #include "cm_types.h"

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*! \addtogroup CM_Apis
 * @{
 */
/*----------------------------------------------------------------------------
 *  CMInit
 *----------------------------------------------------------------------------*/
/*! \brief Initialises the Connection Manager
 *
 * This function initialises the Connection Manager
 * \param[in] cm_init_params pointer to CM_INIT_PARAMS_T type
 * \returns Nothing
 *
 */
extern void CMInit(CM_INIT_PARAMS_T *cm_init_params);

/*----------------------------------------------------------------------------*
 *  CMClientInitRegisterHandler
 *----------------------------------------------------------------------------*/
/*! \brief Registers Client Information
 *
 * This function adds the client service and handlers to the CM client manager
 * \param[in] cm_client_info pointer to CM_CLIENT_INFO_T data structure
 * \returns Nothing
 *
 */
extern void CMClientInitRegisterHandler(
                                    CM_CLIENT_INFO_T *cm_client_info);

/*---------------------------------------------------------------------------*
 *  CMServerInitRegisterHandler
 *---------------------------------------------------------------------------*/
/*! \brief Registers Server Information
 *
 * This function adds the server handlers to the CM client manager
 * \param[in] cm_server_info pointer to CM_SERVER_INFO_T data structure
 * \returns Nothing
 *
 */
extern void CMServerInitRegisterHandler(
                            CM_SERVER_INFO_T *cm_server_info);

/*----------------------------------------------------------------------------*
 *  CMObserverSetScanParams
 *----------------------------------------------------------------------------*/
/*! \brief Sets the scanning parameters for observer
 *
 * This function sets the scanning parameters
 * \param[in] cm_observer_scan_data pointer to CM_CENTRAL_SCAN_INFO_T
 * data structure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMObserverSetScanParams(CM_CENTRAL_SCAN_INFO_T 
                                             *cm_observer_scan_data);

/*----------------------------------------------------------------------------*
 *  CMObserverStartScanning
 *----------------------------------------------------------------------------*/
/*! \brief Starts the scanning procedure for observer
 *
 * This function starts the scanning procedure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMObserverStartScanning(void);

/*----------------------------------------------------------------------------*
 *  CMObserverStartScanningExt
 *----------------------------------------------------------------------------*/
/*! \brief Starts the extended scanning procedure for observer
 *
 * This function starts the extended scanning procedure
 * \param[in] rawAdvertReports Boolean variable for sending raw advertisement reports
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMObserverStartScanningExt(bool rawAdvertReports);

/*----------------------------------------------------------------------------*
 *  CMObserverEnableRawReports
 *----------------------------------------------------------------------------*/
/*! \brief To enable the raw advertising reports flag
 *
 * This function is used to enable the raw advertising reports flag
 * \param[in] rawAdvertReports Boolean variable for sending raw advertisement reports
 * \returns Nothing
 *
 */
extern void CMObserverEnableRawReports(bool rawAdvertReports);

/*----------------------------------------------------------------------------*
 *  CMObserverStopScanning
 *----------------------------------------------------------------------------*/
/*! \brief Stops the scanning procedure for observer
 *
 * This function stops the scanning procedure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMObserverStopScanning(void);

/*----------------------------------------------------------------------------*
 *  CMCentralSetScanParams
 *----------------------------------------------------------------------------*/
/*! \brief Sets the scanning parameters
 *
 * This function sets the scanning parameters
 * \param[in] cm_central_scan_data pointer to CM_CENTRAL_SCAN_INFO_T
 * data structure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMCentralSetScanParams(CM_CENTRAL_SCAN_INFO_T 
                                             *cm_central_scan_data);

/*----------------------------------------------------------------------------*
 *  CMCentralStartScanning
 *----------------------------------------------------------------------------*/
/*! \brief Starts the scanning procedure
 *
 * This function starts the scanning procedure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMCentralStartScanning(void);

/*----------------------------------------------------------------------------*
 *  CMCentralStartScanningExt
 *----------------------------------------------------------------------------*/
/*! \brief Starts the extended scanning procedure
 *
 * This function starts the extended scanning procedure.This API can be used if application 
 * wants UUID based filtering or raw advertisement reports
 * \param[in] uuidonly Boolean variable for specifying UUID based filtering for 
 * advertisement reports
 * \param[in] rawAdvertReport Boolean variable for specifying only raw 
 * advertisement reports should be sent in advertisement reports
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMCentralStartScanningExt(bool uuidonly, 
                                                bool rawAdvertReport);

/*----------------------------------------------------------------------------*
 *  CMCentralStopScanning
 *----------------------------------------------------------------------------*/
/*! \brief Stops the scanning procedure
 *
 * This function stops the scanning procedure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMCentralStopScanning(void);

/*----------------------------------------------------------------------------*
 *  CMCentralConnect
 *----------------------------------------------------------------------------*/
/*! \brief Initiates the BLE connection
 *
 * This function initiates the BLE connection
 * \param[in] cm_central_conn_info Pointer to \ref CM_CENTRAL_CONN_INFO_T
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMCentralConnect(CM_CENTRAL_CONN_INFO_T 
                                       *cm_central_conn_info);

 /*----------------------------------------------------------------------------*
 *  CMCentralCancelConnect
 *----------------------------------------------------------------------------*/
/*! \brief Cancels the ongoing connection request
 *
 * This function cancels the ongoing connection request
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMCentralCancelConnect(void);

/*----------------------------------------------------------------------------*
 *  CMPeripheralSetAdvertParams
 *----------------------------------------------------------------------------*/
/*! \brief Sets the advertising parameters
 *
 * This function sets the advertising parameters
 * \param[in] advertising_parameters pointer to
 * CM_PERIPHERAL_ADVERT_PARAMETERS_T structure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMPeripheralSetAdvertParams(
                              CM_PERIPHERAL_ADVERT_PARAMETERS_T 
                              *advertising_parameters);

/*----------------------------------------------------------------------------*
 *  CMPeripheralStartAdvertising
 *----------------------------------------------------------------------------*/
/*! \brief Starts the advertising procedure
 *
 * This function starts the advertising procedure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMPeripheralStartAdvertising(void);

/*----------------------------------------------------------------------------*
 *  CMPeripheralStopAdvertising
 *----------------------------------------------------------------------------*/
/*! \brief Stops the advertising procedure
 *
 * This function stops the advertising procedure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMPeripheralStopAdvertising(void);

/*----------------------------------------------------------------------------*
 *  CMDisconnect
 *----------------------------------------------------------------------------*/
/*! \brief Disconnects the active link
 *
 * This function disconnects the active link
 * \param[in] device_id device handle id of the device
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMDisconnect(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMClientStartDiscovery
 *----------------------------------------------------------------------------*/
/*! \brief Starts the discovery procedure
 *
 * This function starts the discovery procedure
 * \param[in] device_id device handle id
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMClientStartDiscovery(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMClientWriteRequest
 *----------------------------------------------------------------------------*/
/*! \brief Sends the GATT write request
 *
 * This function sends the GATT write request
 * \param[in] device_id device handle id
 * \param[in] req_type type of the gatt write request
 * \param[in] handle characteristic handle for value or descriptor
 * \param[in] length size of the data array
 * \param[in] data pointer to an array containing characteristic in
 * LITTLE ENDIAN format
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMClientWriteRequest(device_handle_id device_id, 
                                           uint16 req_type, uint16 handle,
                                           uint16 length, uint8* data);

/*----------------------------------------------------------------------------*
 *  CMClientReadRequest
 *----------------------------------------------------------------------------*/
/*! \brief Sends the GATT read request
 *
 * This function sends the GATT read request
 * \param[in] device_id device id of the server
 * \param[in] handle characteristic handle which is to be read from server
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMClientReadRequest(device_handle_id device_id, uint16 handle);

/*----------------------------------------------------------------------------*
 *  CMClientFindDevice
 *----------------------------------------------------------------------------*/
/*! \brief Finds the device id in the client service
 *
 * This function finds the device id in the client service
 * \param[in] client_service Client Service
 * \param[in] device_id device id of the connected device
 * \returns Connection manager status code
 *
 */
extern int8 CMClientFindDevice(CM_SERVICE_T *client_service, 
                                  device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMClientCheckHandleRange
 *----------------------------------------------------------------------------*/
/*! \brief Checks whether the handle falls in the handle range of 
 * service instance
 *
 * This function checks whether the handle falls in the handle range of 
 * service instance
 * \param[in] instance Service Instance
 * \param[in] handle characteristic handle
 * \returns TRUE on success
 *
 */
extern bool CMClientCheckHandleRange(CM_SERVICE_INSTANCE *instance, 
                                        uint16 handle);

/*----------------------------------------------------------------------------*
 *  CMIsClientBusy
 *----------------------------------------------------------------------------*/
/*! \brief Checks if the client is busy in read/write operations
 *
 * This function checks if the client is busy in read/write operations 
 * \returns TRUE if busy
 *
 */
extern bool CMIsClientBusy(void);

/*----------------------------------------------------------------------------*
 *  CMSendAccessRsp
 *----------------------------------------------------------------------------*/
/*! \brief Sends the GATT access response
 *
 * This function sends the GATT access response
 * \param[in] cm_access_rsp pointer to CM_ACCESS_RESPONSE_T structure
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMSendAccessRsp(CM_ACCESS_RESPONSE_T *cm_access_rsp);

/*----------------------------------------------------------------------------*
 *  CMSendValueNotification
 *----------------------------------------------------------------------------*/
/*! \brief Sends the GATT Notification Value
 *
 * This function sends the GATT Notification Value
 * \param[in] cm_value_notify pointer to CM_VALUE_NOTIFICATION_T structure
 * with the value to be notified
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMSendValueNotification(
                            CM_VALUE_NOTIFICATION_T *cm_value_notify);

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  CMSendValueNotification
 *----------------------------------------------------------------------------*/
/*! \brief Sends the GATT Notification Value
 *
 * This function sends the GATT Notification Value
 * \param[in] cm_value_notify pointer to CM_VALUE_NOTIFICATION_T structure
 * with the value to be notified
 * \returns sys status code
 *
 */
extern sys_status CMSendValueNotificationExt(
                            CM_VALUE_NOTIFICATION_T *cm_value_notify);
#endif
/*----------------------------------------------------------------------------*
 *  CMSendValueIndication
 *----------------------------------------------------------------------------*/
/*! \brief Sends the GATT Indication Value
 *
 * This function sends the GATT Indication Value
 * \param[in] cm_value_notify pointer to CM_VALUE_NOTIFICATION_T structure
 * with the value to be notified
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMSendValueIndication(
                            CM_VALUE_NOTIFICATION_T *cm_value_notify);

/*----------------------------------------------------------------------------*
 *  CMStartEncryption
 *----------------------------------------------------------------------------*/
/*! \brief Starts encryption
 *
 * This function starts encryption. If not bonded initiates the pairing first.
 * \param[in] device_id device id of the remote device
 * \returns Connection manager status code (cm_status_success)
 *
 */
extern cm_status_code CMStartEncryption(device_handle_id device_id);


/*----------------------------------------------------------------------------*
 *  CMSecuritySetConfiguration
 *----------------------------------------------------------------------------*/
/*! \brief Sets the security configurations
 *
 * This function sets the security configurations
 * \param[in] io_capability Input output capabilities
 * \returns Connection manager status code (cm_status_success)
 *
 */
extern cm_status_code CMSecuritySetConfiguration(uint16 io_capability);

/*----------------------------------------------------------------------------*
 *  CMBondingAuthResponse
 *----------------------------------------------------------------------------*/
/*! \brief Sends bonding authorisation response
 *
 * This function sends bonding authorisation response
 * \param[in] auth_resp pointer to the response structure
 * \returns None
 *
 */
extern void CMBondingAuthResponse(CM_AUTH_RESP_T *auth_resp);

/*----------------------------------------------------------------------------*
 *  CMGetDeviceBondState
 *----------------------------------------------------------------------------*/
/*! \brief Gets the bonded state of the device
 *
 * This function gets the bonded state of the device
 * \param[in] device_id handle of the device for which the bonding state is
 * required
 * \returns Bonding status code
 *
 */
extern cm_dev_bond_state CMGetDeviceBondState(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMGetAdvState
 *----------------------------------------------------------------------------*/
/*! \brief Gets the CM Advertising state
 *
 * This function gets the Gets the CM Advertising state
 * \returns Advertising state
 *
 */
extern cm_advert_state CMGetAdvState(void);

/*----------------------------------------------------------------------------*
 *  CMGetScanState
 *----------------------------------------------------------------------------*/
/*! \brief Gets the CM Scanning state
 *
 * This function gets the CM Scanning state
 * \returns Scanning state
 *
 */
extern cm_scan_state CMGetScanState(void);

/*----------------------------------------------------------------------------*
 *  CMObserverGetScanState
 *----------------------------------------------------------------------------*/
/*! \brief Gets the CM Observer Scanning state
 *
 * This function gets the CM Observer Scanning state
 * \returns Scanning state
 *
 */
extern cm_scan_state CMObserverGetScanState(void);

/*----------------------------------------------------------------------------*
 *  CMGetConnState
 *----------------------------------------------------------------------------*/
/*! \brief Gets the CM Connecting state
 *
 * This function gets the CM Connecting state
 * \returns Connecting state
 *
 */
extern cm_connect_state CMGetConnState(void);

/*----------------------------------------------------------------------------*
 *  CMGetDevState
 *----------------------------------------------------------------------------*/
/*! \brief Gets the device's current state
 *
 * This function gets the device's current state
 * \param[in] device_id handle of the device for which the status is
 * required
 * \returns Device state code
 *
 */
extern cm_dev_state CMGetDevState(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMGetPeerDeviceRole
 *----------------------------------------------------------------------------*/
/*! \brief Gets the GAP connection role of the peer device
 *
 * This function gets the connection role of the peer device
 * \param[in] device_id handle of the device for which the conn role is
 * required
 * \returns Peer device's GAP connection role
 *
 */
extern cm_peer_con_role CMGetPeerDeviceRole(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMGetBondIdFromDeviceId
 *----------------------------------------------------------------------------*/
/*! \brief Gets bond id of the device
 *
 * This function gets bond id of the device
 * \param[in] device_id handle of the device for which the bond handle is
 * required
 * \returns Bond handle id
 *
 */
extern bond_handle_id CMGetBondIdFromDeviceId(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMGetDeviceIdFromBDAddress
 *----------------------------------------------------------------------------*/
/*! \brief Gets the device id from the Bluetooth Address
 *
 * This function gets the device id from the Bluetooth Address
 * \param[in] bd_addr pointer to bluetooh address structure
 * \returns Device handle id
 *
 */
extern device_handle_id CMGetDeviceIdFromBDAddress(TYPED_BD_ADDR_T *bd_addr);

/*----------------------------------------------------------------------------*
 *  CMGetBdAdressFromDeviceId
 *----------------------------------------------------------------------------*/
/*! \brief Gets the Bluetooth Address of the device
 * 
 * This function gets the Bluetooth Address of the device
 * \param[in] device_id device id of the device for which the Bluetooth Address
 * is required
 * \param[out] bd_addr pointer to structure (TYPED_BD_ADDR_T) where the
 * Bluetooth Address will be stored
 * \returns TRUE on success
 */
extern bool CMGetBdAdressFromDeviceId(device_handle_id device_id, 
                                             TYPED_BD_ADDR_T *bd_addr);

/*----------------------------------------------------------------------------*
 *  CMConnParamUpdateReq
 *----------------------------------------------------------------------------*/
/*! \brief Requests for the connection parameters update
 *
 * This function requests for the connection parameters update
 * \param[in] device_id device id of the device
 * \param[in] new_params pointer to ble_con_params
 * \returns Connection manager status code
 *
 */
extern cm_status_code CMConnParamUpdateReq(device_handle_id device_id,
                                      ble_con_params *new_params);

/*----------------------------------------------------------------------------*
 *  CMConnectionUpdateSignalingRsp
 *----------------------------------------------------------------------------*/
/*! \brief Response to the Connection Parameter Update Signalling Indication
 *
 * This function sends response to the Connection Parameter Update Signalling Indication
 * \param[in] device_id device id of the device
 * \param[in] sig_identifier An identifier for the specific connection
 *      update signal.
 * \param[in] accepted TRUE if the updated parameters are acceptable to
 *      the application, FALSE otherwise.
 * \returns Nothing
 *
 */
extern void CMConnectionUpdateSignalingRsp(device_handle_id device_id,
                                            uint16  sig_identifier,
                                            bool    accepted );

/*----------------------------------------------------------------------------*
 *  CMGetDevConnParam
 *----------------------------------------------------------------------------*/
/*! \brief Gets the device's connection parameters
 *
 * This function gets the device's connection parameters
 * \param[in] device_id device id for which the connection parameters are
 * required
 * \param[in] conn_params pointer to the connection parameters got 
 * filled
 * \returns TRUE or FALSE
 *
 */
extern bool CMGetDevConnParam(device_handle_id device_id, 
                              CM_DEV_CONN_PARAM_T *conn_params);

/*----------------------------------------------------------------------------*
 *  CMConfigureRadioEvent
 *----------------------------------------------------------------------------*/
/*! \brief Configures for the Radio Events
 *
 * This function configures the Radio Events
 * \param[in] cm_radio_event pointer to radio event structure
 * \returns Nothing
 *
 */
extern void CMConfigureRadioEvent(CM_REQ_RADIO_EVENT_T *cm_radio_event);

/*----------------------------------------------------------------------------*
 *  CMHoldTxEvent
 *----------------------------------------------------------------------------*/
/*! \brief Enables or disables delayed data packet transmission at the radio
 *
 * This function enables or disables delayed data packet transmission at the radio
 * \param[in] cm_hold_tx_event pointer to hold Tx event structure
 * \returns Nothing
 *
 */
extern void CMHoldTxEvent(CM_REQ_HOLD_TX_EVENT_T* cm_hold_tx_event);

/*----------------------------------------------------------------------------*
 *  CMAddWhitelistDevice
 *----------------------------------------------------------------------------*/
/*! \brief Adds a device to the whitelist
 *
 * This function adds a device to the whitelist
 * \param[in] bond_id bond_id of the bonded device
 * \returns TRUE on success
 *
 */
extern bool CMAddWhitelistDevice(bond_handle_id bond_id);

/*----------------------------------------------------------------------------*
 *  CMRemoveWhitelistDevice
 *----------------------------------------------------------------------------*/
/*! \brief Removes a device from the whitelist
 *
 * This function removes a device from the whitelist
 * \param[in] bond_id bond_id of the bonded device
 * \returns TRUE on success
 *
 */
extern bool CMRemoveWhitelistDevice(bond_handle_id bond_id);

/*----------------------------------------------------------------------------*
 *  CMMatchConnectedAddress
 *----------------------------------------------------------------------------*/
/*! \brief Checks if the connected address matches with the known address
 *
 * This function checks if the connected address matches with the known address 
 * \param[in] con_addr pointer to the connected address
 * \param[in] known_addr pointer to the bonded address
 * \param[in] irk irk of the bonded device 
 * \returns TRUE on success
 *
 */
extern bool CMMatchConnectedAddress(TYPED_BD_ADDR_T *con_addr, 
                                       TYPED_BD_ADDR_T *known_addr, uint16 *irk);

/*----------------------------------------------------------------------------*
 *  CMStoreFactoryBond
 *----------------------------------------------------------------------------*/
/*! \brief Stores the in-factory bonding information
 *
 * This function stores the in-factory bonding information
 * \param[in] factory_bond_info pointer to in-factory bonded device
 * \returns bond_handle_id
 *
 */
extern bond_handle_id CMStoreFactoryBond(CM_FACTORY_BOND_INFO_T *factory_bond_info);

/*----------------------------------------------------------------------------*
 *  CMNotifyFactoryBond
 *----------------------------------------------------------------------------*/
/*! \brief Notify in-factory bonding status
 *
 * This function notifies in-factory bonding status to all registered services
 * \param[in] device_id handle of the device
 * \returns Nothing
 *
 */
extern void CMNotifyFactoryBond(device_handle_id device_id);

/*----------------------------------------------------------------------------*
 *  CMConfigureRxTimingReport
 *----------------------------------------------------------------------------*/
/*! \brief Enable or disable reporting to the application various packet timing parameters 
 *
 * This function enables or disables reporting to the application various packet timing parameters
 * \param[in] cm_timing_event pointer to Rx Timing Report event structure
 * \returns Nothing
 *
 */
extern void CMConfigureRxTimingReport(CM_REQ_RX_TIMING_REPORT_EVENT_T *cm_timing_event);

/*----------------------------------------------------------------------------*
 *  CMReadRssi
 *----------------------------------------------------------------------------*/
/*! \brief Return the last Received Signal Strength Indication for a connection 
 *
 * This function returns the last Received Signal Strength Indication for a connection
 * \param[in] device_id handle of the device
 * \param[in] rssi Pointer to a variable into which the RSSI value shall be stored
 * \returns ls_err error code
 *
 */
extern ls_err CMReadRssi(device_handle_id device_id,int8* rssi);

/*----------------------------------------------------------------------------*
 *  CMIsAdvertising
 *----------------------------------------------------------------------------*/
/*! \brief Checks if CM is advertising
 *
 * This function checks if the connection manager is advertising
 * \returns TRUE if connection manager is in advertising state
 *
 */
extern bool CMIsAdvertising(void);

/*----------------------------------------------------------------------------*
 *  CMRemoveBond
 *----------------------------------------------------------------------------*/
/*! \brief Removes bonding information for the given bond id
 *
 * This function removes bonding information for the given bond id
 * \param[in] bond_id bond_id of the device to be unbonded
 *
 */
extern void CMRemoveBond(bond_handle_id bond_id);

/*----------------------------------------------------------------------------*
 *  CMGetBondedDevices
 *----------------------------------------------------------------------------*/
/*! \brief Gets all the bonded devices
 *
 * This function gets all the bonded devices
 * \param[out] bond_dev pointer to the array where the address and role info
 * is to be stored
 * \param[out] num_conn pointer to the integer where the number of bonded
 * devices is stored
 * \returns TRUE on success
 *
 */
extern bool CMGetBondedDevices(CM_BONDED_DEVICE_T *bond_dev, uint16 *num_conn);

/*----------------------------------------------------------------------------*
 *  CMGetBondedDeviceFromBondId
 *----------------------------------------------------------------------------*/
/*! \brief Get the bonded device from the bond id
 *
 * This function gets the bonded device from the bond id
 * \param[out] bond_id bond_id of the bonded device
 * \param[out] bond_dev pointer to the bonded device
 * \returns TRUE on success
 *
 */
extern bool CMGetBondedDeviceFromBondId(bond_handle_id bond_id, 
                              CM_BONDED_DEVICE_T *bond_dev);

/*----------------------------------------------------------------------------*
 *  CMIsDeviceBonded
 *----------------------------------------------------------------------------*/
/*! \brief Checks the given Bluetooth address is bonded or not
 *
 * This function checks the given Bluetooth address is bonded or not
 * \param[in] bdaddr pointer to the Bluetooth address
 * \returns TRUE on success
 *
 */
extern bool CMIsDeviceBonded(TYPED_BD_ADDR_T *bdaddr);

/*----------------------------------------------------------------------------*
 *  CMIsAddressResolvableRandom
 *----------------------------------------------------------------------------*/
/*! \brief Checks if the Bluetooth address is resolvable random or not
 *
 * This function checks if the Bluetooth address is resolvable random or not
 * \param[in] bd_addr pointer to the Bluetooth address
 * \returns TRUE on success
 *
 */
extern bool CMIsAddressResolvableRandom(TYPED_BD_ADDR_T *bd_addr);

/*----------------------------------------------------------------------------*
 *  CMGetBondId
 *----------------------------------------------------------------------------*/
/*! \brief Gets the bond id of the device
 *
 * This function gets the bond id of the device
 * \param[in] remote_bd_addr pointer to the Bluetooth address
 * \returns Bond handle Id
 *
 */
extern bond_handle_id CMGetBondId(TYPED_BD_ADDR_T *remote_bd_addr);

/*----------------------------------------------------------------------------*
 *  CMGetBondedDeviceRole
 *----------------------------------------------------------------------------*/
/*! \brief Gets the bonded device role
 *
 * This function gets the bonded device role
 * \param[in] bd_addr pointer to the Bluetooth address
 * \returns Peer device's GAP connection role
 *
 */
extern cm_peer_con_role CMGetBondedDeviceRole(TYPED_BD_ADDR_T *bd_addr);

/*----------------------------------------------------------------------------*
 *  CMGetBondedBDAddr
 *----------------------------------------------------------------------------*/
/*! \brief Gets the Bluetooth Address of the bonded device
 *
 * This function gets the Bluetooth Address of the bonded device
 * \param[in] bond_id bond_id of the device
 * \param[out] bdaddr pointer to the Bluetooth address
 * \returns TRUE on success
 *
 */
extern bool CMGetBondedBDAddr(bond_handle_id bond_id, TYPED_BD_ADDR_T *bdaddr);

/*----------------------------------------------------------------------------*
 *  CMIsBondIdValid
 *----------------------------------------------------------------------------*/
/*! \brief Checks the given bond id valid or not
 *
 * This function checks the given bond id valid or not
 * \param[in] bond_id bond_id of the device
 * \returns TRUE if the bond id is valid
 *
 */
extern bool CMIsBondIdValid(bond_handle_id bond_id);

/*----------------------------------------------------------------------------*
 *  CMUuidEqual
 *----------------------------------------------------------------------------*/
/*! \brief Compares the given UUIDS
 *
 * This function compares the given UUIDS
 * \param[in] uuid_1 pointer to first UUID
 * \param[in] uuid_2 pointer to second UUID
 * \returns TRUE if the UUID's are equal
 *
 */
extern bool CMUuidEqual(CM_UUID_T *uuid_1, CM_UUID_T *uuid_2);

/*----------------------------------------------------------------------------*
 *  CMGetConnectedDevices
 *----------------------------------------------------------------------------*/
/*! \brief Fills the connected devices' addresses 
 *
 * This function fills the connected devices' addresses and their role to the given buffer
 * \param[out] conn_dev pointer to the array where the address and role info
 * is to be stored
 * \param[out] num_conn pointer to the integer where the number of connected
 * devices is stored
 * \returns TRUE on success
 *
 */
extern bool CMGetConnectedDevices(CM_CONNECTED_DEVICE_T *conn_dev,
                                  uint16 *num_conn);

/*----------------------------------------------------------------------------*
 *  CMPassKeyInputResponse
 *----------------------------------------------------------------------------*/
/*! \brief Sends user response from passkey input state
 *
 * This function sends user response from passkey input state
 * \param[in] device_id device handle id of the device with which the keyboard
 * is connected
 * \param[in] pass_key pointer to the pass key value. Send NULL if pass key input
 * is cancelled by user (negative response)
 *
 * \returns Nothing
 *
 */
extern void CMPassKeyInputResponse(device_handle_id device_id,
                                   const uint32 *pass_key);

/*----------------------------------------------------------------------------*
 *  CMSetRandomAddress
 *----------------------------------------------------------------------------*/
/*! \brief Sets the random address for this device
 *
 * This function sets the random address for this device.The random address may subsequently
 * be used in advertise.
 * \param[in] bd_addr The Bluetooth address to use as the device's random address
 * 
 * \returns Nothing
 *
 */
extern void CMSetRandomAddress(BD_ADDR_T bd_addr);

/*----------------------------------------------------------------------------*
 *  CMProcessMsg
 *----------------------------------------------------------------------------*/
/*! \brief Implements the event handler for Baldrick and Flashheart
 *
 * This function implements the event handler for Baldrick
 * \returns Nothing
 *
 */
#ifndef CSR101x_A05
void CMProcessMsg(msg_t *msg);
#else
extern bool CMProcessMsg(lm_event_code event_code, 
                              LM_EVENT_T *p_event_data);
#endif /*! \brief CSR101x_A05 */

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  CMEnableEarlyWakeup
 *----------------------------------------------------------------------------*/
/*! \brief Enable or disable notifications to the application of an impending 
 *         connection events for Flashheart only.
 *
 *  The application may desire to be fore-warned that the radio will be 
 *  powering up to respond to a connection event.
 *
 *  The application can specify how much ahead of time it would like to 
 *  be notified of this, so it has sufficient time to fill radio buffers.
 *  For each connection event that is about to occur, the connection manager 
 *  will send an LS_EARLY_WAKEUP_IND event to the application event 
 *  handler. 
 *
 *  An interval value of 0 will disable the notifications.
 *
 *  \param[in]  device_id   device handle id of the device
 *  \param[in]  interval    How much warning (in uS) to give of impending 
 *  connection event 
 *
 *  \return Result of the operation, ls_err_none on success or an error code.
 *
 */
extern ls_err CMEnableEarlyWakeup(device_handle_id device_id, uint16 interval);
#endif /*! \brief CSR101x_A05 */

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  CMStreamGattSink
 *----------------------------------------------------------------------------*/
/*! \brief Implements the Stream Gatt Sink call and abstracts the CID
 *
 * Implements the Stream Gatt Sink call and abstracts the CID
 * \param[in] open_info A structure of the sink parameters for the device and 
 * characterastic handle to notify data on
 * \returns stream sink handle
 *
 */
extern handle_t CMStreamGattSink(cm_audio_open_param_t* open_info);
#endif /* CSR101x_A05 */

/*----------------------------------------------------------------------------*
 *  CMSignalNumberOfCompletedPackets
 *----------------------------------------------------------------------------*/
/*! \brief Enable number of completed packets once
 *
 * Send CM_NUMBER_OF_COMPLETED_PKTS_IND to the servers and clients 
 * when data has left the chip, this needs to be called each time
 * CM_NUMBER_OF_COMPLETED_PKTS_IND is needed by an application. 
 * \returns void
 *
 */
extern void CMSignalNumberOfCompletedPackets(void);

/* Gets the connection id (cid) of the device */
extern uint16 CMGetConnId(device_handle_id device_id);

/*!@} */

#endif /* __CM_API_H__ */
