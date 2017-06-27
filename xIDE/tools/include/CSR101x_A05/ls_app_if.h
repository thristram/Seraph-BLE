/*! \file ls_app_if.h
 *  \brief  Link Supervisor interface to Applications
 *
 *  White listing and other miscellaneous low-level functionality.
 *
 * Copyright (c) 2010 - 2014 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __LS_APP_IF_H__
#define __LS_APP_IF_H__


/*============================================================================*
Header Files
*============================================================================*/
#include "types.h"
#include "bluetooth.h"
#include "bt_event_types.h"
#include "ls_types.h"
#include "ls_err.h"
#include "ls_app_if_event.h"

/*============================================================================*
Public Definitions
*============================================================================*/

/*! \addtogroup LSA
 * @{
 */

/*! \name Advertising Data Flags
 *
 * Definitions for the Advertising Data Flags data type (AD_TYPE_FLAGS)
 *
 * @{
 */
#define AD_FLAG_LE_LIMITED_DISCOVERABLE     0x01
#define AD_FLAG_LE_GENERAL_DISCOVERABLE     0x02
#define AD_FLAG_BR_EDR_NOT_SUPPORTED        0x04
#define AD_FLAG_SIMUL_LE_BREDR_CONTROLLER   0x08
#define AD_FLAG_SIMUL_LE_BREDR_HOST         0x10
/* @} */

/*! \name Security Manager Flags
 *
 * Definitions for the Security Manager OOB Flags data type (AD_TYPE_SM_FLAGS)
 *
 * @{
 */
#define AD_SM_FLAG_OOB_PRESENT              0x01
#define AD_SM_FLAG_LE_SUPPORTED_HOST        0x02
#define AD_SM_FLAG_SIMUL_LE_BREDR_HOST      0x04
#define AD_SM_FLAG_RANDOM_ADDRESS           0x08
/* @} */

/*! @} */

/*! \addtogroup LSAPP
 * @{
 */

/*! \name Default GAP Connection Establishment Parameters
 *
 *  \brief These are the default values configured when the device powers up.
 *  The values are typically changed by a GATT Client prior to connecting or
 *  (after having discovered a GATT Server's Preferred Connection Parameters)
 *  reconnecting.
 *
 *  These values are defined in the Bluetooth Specification (Vol.3 Part C
 *  Section 16). However, these values are only "recommended" and it is
 *  likely that profiles will define their own preferred parameters.
 *
 * @{
 */
#if 0
/* B-108411: Not currently using actual GAP defaults in Bluetooth Spec */
#define LS_CON_DEFAULT_MIN_INT          400     /*!< Default Min Connection Interval (500ms) */
#define LS_CON_DEFAULT_MAX_INT          1024    /*!< Default Max Connection Interval (1.28s) */
#define LS_CON_DEFAULT_SLAVE_LATENCY    0       /*!< Default Slave Latency */
#define LS_CON_DEFAULT_SUPER_TIMEOUT    2000    /*!< Default Link Supervision Timeout (20s) */
#else
#define LS_CON_DEFAULT_MIN_INT          20      /*!< Default Min Connection Interval (25ms) */
#define LS_CON_DEFAULT_MAX_INT          20      /*!< Default Max Connection Interval (25ms) */
#define LS_CON_DEFAULT_SLAVE_LATENCY    0       /*!< Default Slave Latency */
#define LS_CON_DEFAULT_SUPER_TIMEOUT    200     /*!< Default Link Supervision Timeout (2s) */
#endif

#define LS_CON_DEFAULT_SCAN_INTERVAL    4100    /*!< Default Scan Interval */
#define LS_CON_DEFAULT_SCAN_WINDOW      4096    /*!< Default Scan Window */
#define LS_CON_DEFAULT_MIN_CE_LENGTH    0       /*!< Default Minimum Connection Event Length */
#define LS_CON_DEFAULT_MAX_CE_LENGTH    0       /*!< Default Maximum Connection Event Length */
/* @} */

/*! \name Connection Establishment Parameters Valid Ranges
 *
 *  \brief Valid ranges for connection parameters.
 *
 *  It is an error for an application to attempt to set connection establishment
 *  parameters that are not within the allowed range. Furthermore it is an
 *  error for an application to set a minimum connection interval that is
 *  greater than the maximum connection interval, to set a scan window that is
 *  longer than the scan interval, or to set a minimum CE interval that is
 *  greater than the maximum CE interval.
 *
 * @{
 */
#define LS_CON_INTERVAL_MIN             6       /*!< Minimum allowed connection interval (7.5ms) */
#define LS_CON_INTERVAL_MAX             3200    /*!< Maximum allowed connection interval (4s) */
#define LS_CON_SLAVE_LATENCY_MAX        499     /*!< Maximum allowed slave latency */
#define LS_CON_TIMEOUT_MIN              10      /*!< Minimum allowed supervision timeout (100ms) */
#define LS_CON_TIMEOUT_MAX              3200    /*!< Maximum allowed supervision timeout (32s) */
#define LS_CON_SCAN_MIN                 0x0004  /*!< Minimum scan window/interval (2.5ms) */
#define LS_CON_SCAN_MAX                 0x4000  /*!< Minimum scan window/interval (10.24s) */
/* @} */


/*! \name General definitions
 *
 * @{
 */
#define LS_MIN_TRANSMIT_POWER_LEVEL     0       /*!< Minimum allowed TX power level (range 0-7) */
#define LS_MAX_TRANSMIT_POWER_LEVEL     7       /*!< Maximum allowed TX power level (range 0-7) */
/* @} */

/*! @} */


/*============================================================================*
Public Data Types
*============================================================================*/



/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*! \addtogroup LSAPP
 * @{
 */
/*----------------------------------------------------------------------------*
 *  LsRadioEventNotification
 */
/*! \brief  Enable or disable notification to the application of radio events
 *  for a given GATT connection.
 *
 *  In some specific sensor-polling applications it can be useful to trigger
 *  the application to run once per radio event. This feature can be used to
 *  minimise the current consumption of the device, as sensor polling and radio
 *  activity can be performed within a single "wakeup" of the chip, and remove
 *  the need for the application to run a separate timer at the same rate as
 *  the link's Connection Interval.
 *
 *  For each radio event that occurs, the firmware will send an
 *  #LS_RADIO_EVENT_IND event to the application event handler,
 *  AppProcessLmEvent(), with message payload type LS_RADIO_EVENT_IND_T.
 *
 *  When a connection is established, the application by default will not be
 *  notified of any radio activity, as for most purposes it is not useful (e.g.
 *  normal CFM messages for GATT messages are sufficient to keep the application
 *  running).
 *  
 *  Note that when operating as a BLE slave, events are only generated upon 
 *  successful RX transactions. No events will be generated if the master was not
 *  heard or if the slave is not listening due to latency.
 *
 *  \param  cid  GATT Connection Identifier for the link
 *  \param  activity  Level of activity to report for the link
 *
 *  \returns  ls_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsRadioEventNotification(uint16 cid, radio_event evt);


/*----------------------------------------------------------------------------*
 *  LsHoldTxUntilRx
 */
/*! \brief  Enable or disable delayed data packet transmission at the radio.
 *
 *  Enabling this mode will delay transmitting data packets until a data packet
 *  has been received from the peer. This mode is not for general use as it can
 *  stall the BLE connection, causing protocol stack timeouts and link loss.
 *
 *  \param  cid  GATT Connection Identifier for the link
 *  \param  mode  Boolean flag to enable delayed data TX
 *
 *  \returns  ls_err_none on success
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsHoldTxUntilRx(uint16 cid, bool mode);

/*----------------------------------------------------------------------------*
 *  LsRxTimingReport
 */
/*! \brief  Enable or disable reporting to the application various packet timing
 *  parameters when data packets are received at the radio.
 *
 *  \warning This feature is only supoprted when operating as a BLE Master.
 *  Enabling this feature as a slave may result in undefined behaviour.
 *
 *  \param  cid  GATT Connection Identifier for the link
 *  \param  bool  enable (T) or disable (F) the feature
 *
 *  \returns  ls_err_none on success
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsRxTimingReport(uint16 cid, bool mode);


/*----------------------------------------------------------------------------*
 *  LsReadWhiteListMaxSize
 */
/*! \brief  Read the capacity of the whitelist.
 *
 *  \param  sz  Pointer to a variable into which the maximum size will be written.
 *
 *  \returns  ls_err_none on success
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsReadWhiteListMaxSize(uint8 *sz);


/*----------------------------------------------------------------------------*
 *  LsResetWhiteList
 */
/*! \brief  Reset and clear the whitelist.
 *
 *  \returns ls_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsResetWhiteList(void);


/*----------------------------------------------------------------------------*
 *  LsAddWhiteListDevice
 */
/*! \brief  Add a device to the whitelist.
 *
 *  \param  addrt  The typed Bluetooth address of the device to add to
 *      the whitelist.
 *
 *  \returns le_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsAddWhiteListDevice(TYPED_BD_ADDR_T * const addrt);


/*----------------------------------------------------------------------------*
 *  LsDeleteWhiteListDevice
 */
/*! \brief  Delete a device from the whitelist.
 *
 *  \param  addrt  The typed Bluetooth address of the device to add to
 *      the whitelist.
 *
 *  \returns le_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsDeleteWhiteListDevice(TYPED_BD_ADDR_T * const addrt);


/*----------------------------------------------------------------------------*
 *  LsReadRemoteVersionInformation
 */
/*! \brief Trigger a remote version information exchange with the connected peer.
 *
 *  The remote version information will be returned via an
 *  #LM_EV_REMOTE_VERSION_INFO message sent to the application event handler,
 *  AppProcessLmEvent(), with message payload type LM_EV_REMOTE_VERSION_INFO_T.
 *  This event looks very similar to the corresponding HCI event (refer to
 *  Bluetooth Specification v4.0 Volume 2 Part E Section 7.7.12 for full details
 *  of this event). However, the HCI connection handle parameter is instead
 *  mapped onto the GATT Connection Identifier supplied in the 'cid' parameter.
 *
 *  \param  cid  GATT Connection Identifier for the link
 *
 *  \returns  ls_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsReadRemoteVersionInformation(uint16 cid);


/*----------------------------------------------------------------------------*
 *  LsReadRssi
 */
/*! \brief  Return the last Received Signal Strength Indication for a connection.
 *
 *  The RSSI value is an absolute receiver signal strength value in dBm, to
 *  6dBm accuracy. If the RSSI cannot be read, the (maximum) value 127 will be
 *  returned.
 *
 *  \param  cid  GATT Connection Identifier for the link
 *  \param  rssi_val  Pointer to a variable into which the RSSI value shall be stored
 *
 *  \returns  ls_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsReadRssi(uint16 cid, int8 *rssi_val);


/*----------------------------------------------------------------------------*
 *  LsReadTransmitPowerLevel
 */
/*! \brief  Retrieve the current transmit power level setting.
 *
 *  LE does not automatically alter transmission power levels unlike
 *  BR/EDR, so the value retrieved will be whatever has been configured.
 *  The initial power level is set by the CS key \c tx_power_level 
 *  ("Transmit power level") but this may be updated during system operation
 *  using the LsSetTransmitPowerLevel() function.
 *
 *  \warning The returned power level is an approximate transmit power level in
 *  dBm estimated from the current power level setting; the actual transmitted
 *  power will depend on the physical characteristics of the board components
 *  and layout.
 *
 *  \param  tx_power_lvl  Pointer to a variable into which the transmit power
 *                        level shall be stored.
 *
 *  \returns  ls_err_none on success.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsReadTransmitPowerLevel(int8 *tx_power_lvl);


/*----------------------------------------------------------------------------*
 *  LsSetTransmitPowerLevel
 */
/*! \brief  Update the current transmit power level setting.
 *
 *  This function allows the Application code to change the transmit power
 *  dynamically.  The level argument is not a level in dBm, it's an index
 *  value into the transmit power table.  
 *
 *  This function can be called by the Application at any time.  If the radio
 *  is active with transmissions then the power will alter immediately, if the
 *  radio is inactive the new power level will be observed when the radio next
 *  starts transmitting.
 *
 *  \warning There are a number of configurable levels. The limits are defined
 *  by LS_MIN_TRANSMIT_POWER_LEVEL and LS_MAX_TRANSMIT_POWER_LEVEL
 *  Settings outside this range are not legal and the results are undefined. 
 *
 *  \param  tx_power_lvl  Power level setting index, default from the 
 *  CS key \c tx_power_level 
 *
 *  \returns  ls_err_none on success.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsSetTransmitPowerLevel(uint8 tx_power_lvl);


/*----------------------------------------------------------------------------*
 *  LsReadRemoteUsedFeatures
 */
/*! \brief  Trigger a Remote Used Features exchange with the connected peer.
 *
 *  The remote version information will be returned via an
 *  #LM_EV_REMOTE_USED_FEATURES message sent to the application event handler,
 *  AppProcessLmEvent(), with message payload type LM_EV_REMOTE_USED_FEATURES_T.
 *  This event looks very similar to the corresponding HCI event (refer to
 *  Bluetooth Specification v4.0 Volume 2 Part E Section 7.7.65.4 for full details
 *  of this event). However, the HCI connection handle parameter is instead
 *  mapped onto the GATT Connection Identifier supplied in the 'cid' parameter.
 *
 *  NOTE: This function can only be used when the device is connected as a BLE
 *  master. An error will be returned if it is called while the device is a
 *  slave.
 *
 *  \param  cid  GATT Connection Identifier for the link
 *
 *  \returns  ls_err_none on success, or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsReadRemoteUsedFeatures(uint16 cid);


/*----------------------------------------------------------------------------*
 *  LsSetNewConnectionParamReq
 */
/*! \brief Set connection parameters for new connections.
 *
 *  Devices operating as a BLE master will use these parameters for all
 *  subsequent connections. Changing these parameters will not affect existing
 *  connections - use LsConnectionParamUpdateReq() to do that.
 *
 *  This function is not used on slave-role devices.
 *
 *  \param  conn_params     Connection parameters (minimum & maximum interval,
 *                          slave latency, & supervision timeout)
 *  \param  con_min_ce_len  Expected minimum Connection Event length (can be 0)
 *  \param  con_max_ce_len  Expected maximum Connection Event length (can be 0)
 *  \param  con_scan_interval  Scan interval during connection establishment
 *  \param  con_scan_window Scan window during connection establishment
 *
 *  \returns  Status. Invalid parameters will be rejected
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsSetNewConnectionParamReq(ble_con_params* conn_params,
                                         uint16 con_min_ce_len,
                                         uint16 con_max_ce_len,
                                         uint16 con_scan_interval,
                                         uint16 con_scan_window);


/*----------------------------------------------------------------------------*
 *  LsDisableSlaveLatency
 */
/*! \brief Ignore slave latency value in a connection
 *
 *  In some cases, a device operating as BLE slave may wish to not respect the
 *  master's wish for slave latency. This function allows the slave to select
 *  whether it obeys the master's requests to use latency.
 *
 *  Calling this function affects all subsequent connections made as a slave
 *  device. If called during a connection, it will take effect at the end of
 *  the next latency period (or when it would have been had latency been
 *  enabled).
 *
 *  If slave latency is enabled during a connection, then the latency will be
 *  set to the value last requested by the master (either at connection time or
 *  in a subsequent connection parameter update).
 *
 *  NOTE: Disabling slave latency must only be done after careful consideration,
 *  and for as short a period as possible, as it will have a detrimental effect
 *  on power consumption.
 *
 *  \param  disable     Set TRUE to ignore slave latency on the link, and
 *                      FALSE to allow it.
 *
 *  \return ls_err_none on success or an appropriate error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsDisableSlaveLatency(bool disable);


/*----------------------------------------------------------------------------*
 *  LsConnectionParamUpdateReq
 */
/*! \brief Request an update to the connection parameters.
 *
 *  The Connection Parameter Update procedure in initiated as described in
 *  Bluetooth Specification v4.0 Volume 6 Part B Section 5.1.1
 *
 *  When the procedure finishes, an #LS_CONNECTION_PARAM_UPDATE_CFM event
 *  is raised
 *
 *  \param  bdAddr      Typed Bluetooth address of the connected peer,
 *                      identifying the link to update
 *  \param  new_params  New connection parameters (minimum & maximum interval,
 *                      slave latency, & supervision timeout)
 *
 *  \returns  ls_err_none for success, or an error code if the procedure cannot
 *            be initiated.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsConnectionParamUpdateReq(TYPED_BD_ADDR_T *bdAddr,
                                       ble_con_params* new_params);


/*----------------------------------------------------------------------------*
 *  LsConnectionUpdateSignalingRsp
 */
/*! \brief  Application response to an #LS_CONNECTION_UPDATE_SIGNALLING_IND event.
 *
 *  \param  con_handle  The connection handle this response applies
 *      to.  Should be the same as the \a con_handle received in the
 *      #LS_CONNECTION_UPDATE_SIGNALLING_IND event.
 *  \param sig_identifier An identifier for the specific connection
 *      update signal that this response applies to.  Should be the
 *      same as the \a sig_identifier received in the
 *      #LS_CONNECTION_UPDATE_SIGNALLING_IND event.
 *  \param  accepted  TRUE if the updated parameters are acceptable to
 *      the application, FALSE otherwise.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void LsConnectionUpdateSignalingRsp(uint16 con_handle,
                                           uint16 sig_identifier,
                                           bool accepted);
/*! @} */


/*! \addtogroup LSA
 * @{
 */

/*----------------------------------------------------------------------------*
 *  LsStoreAdvScanData
 */
/*! \brief Set Advertising or Scan Response data.
 *
 *  This function is called by the application to add either advertising
 *  or scan response data. Each call to the function will add a single
 *  AD Structure (refer to Bluetooth specification Vol.3 Part C Section 11).
 *  Repeated calls will append new structures, to build up the data content.
 *
 *  The application should not include the "length" parameter within the
 *  supplied octet array - the GAP layer will add the length field in the
 *  appropriate position. The first octet of the array should be the AD Type
 *  field (see #ad_type)
 *
 *  The data will be stored within GAP as AD structures, to a maximum of
 *  31 octets (including the length octets); if the application exceeds this
 *  capacity an error is returned and the AD Structure is not stored.
 *
 *  NOTE: The GAP layer will automatically add the AD Flags structure to the
 *  start of the Advertising data and manage the flags values. The
 *  application itself is not allowed to add this AD Type.
 *
 *  An extended inquiry response or advertising data packet should not contain
 *  more than one instance for each Service UUID data size.
 *
 *  If the application wishes to clear any existing data it should call the
 *  function with the length parameter set to zero. This will clear all
 *  stored data including the AD Flags structure. (It is therefore not
 *  possible to define a string that only contains the AD Flags structure).
 *
 *  Advertising data is only used when the device is in the Broadcaster or
 *  Peripheral role. However, advertising data can be set while in any role,
 *  e.g. an application using the Observer role can update the advertising
 *  data.
 *
 *  \param  len  The number of bytes of data supplied.
 *  \param  data A byte array of the AD Structure to add. The first byte will
 *               be the AD Type field (see #ad_type). It is the caller's
 *               responsibility to ensure that the remainder of the data is
 *               correctly formatted and consistent with the specified AD Type.
 *
 *  \param  src  A flag indicating whether the data supplied is advertising
 *      data (ad_src_advertise) or scan response data (ad_src_scan_rsp).
 *
 *  \returns  ls_err_none for success, or an error code if the store fails.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsStoreAdvScanData(uint16 const len,
                                 uint8 * const data,
                                 ad_src const src);

/*----------------------------------------------------------------------------*
 *  LsStoreAdvDataNoAdFlags
 */
/*! \brief Set Advertising with no AD FLAGS. (ONLY USE FOR NON-CONNECTABLE ADVERTISING)
 *
 *  This function can be used by the application to add non connectable advertising data
 *  without the flags AD Structure.  Each call to the function will add a single AD
 *  Structure (refer to Bluetooth specification Vol.3 Part C Section 11).
 *  Repeated calls will append new structures, to build up the data content.
 *
 *  The application should not include the "length" parameter within the
 *  supplied octet array - the GAP layer will add the length field in the
 *  appropriate position. The first octet of the array should be the AD Type
 *  field (see #ad_type)
 *
 *  The data will be stored within GAP as AD structures, to a maximum of
 *  31 octets (including the length octets); if the application exceeds this
 *  capacity an error is returned and the AD Structure is not stored.
 *
 *  An advertising data packet should not contain more than one instance for each
 *  Service UUID data size.
 *
 *  If the application wishes to clear any existing data it should call the
 *  function with the length parameter set to zero. This will clear all
 *  stored data.
 *
 *  Calling this function with the length set to a non zero value will stop the
 *  application from doing connectable adverts, until this function or
 *  LsStoreAdvScanData is called with the length parameter set to zero.
 *
 *  Advertising data is only used when the device is in the Broadcaster or
 *  Peripheral role. However, advertising data can be set while in any role,
 *  e.g. an application using the Observer role can update the advertising
 *  data.
 *
 *  \param  len  The number of bytes of data supplied.
 *  \param  data A byte array of the AD Structure to add. The first byte will
 *               be the AD Type field (see #ad_type). It is the caller's
 *               responsibility to ensure that the remainder of the data is
 *               correctly formatted and consistent with the specified AD Type.
 *
 *  \returns  ls_err_none for success, or an error code if the store fails.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsStoreAdvDataNoAdFlags(uint16 const len,
                                      uint8 * const data);

/*----------------------------------------------------------------------------*
 *  LsStartStopAdvertise
 */
/*! \brief Start or stop advertising, with or without a whitelist.
 *
 *  Configuration is stored in GAP; the application may have called
 *  LsStoreAdvScanData() before calling this function.
 *
 *  \param  go  TRUE (re)starts advertising, FALSE stops advertising.
 *  \param  wl_mode  Whether or not to use the whitelist.
 *  \param  addr_type  Whether to advertise using the device's public
 *     Bluetooth address, or a private "random" address.
 *
 *  \returns  ls_err_none on success, or an error code if the action fails.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsStartStopAdvertise(bool const go,
                                   whitelist_mode const wl_mode,
                                   ls_addr_type const addr_type);

/*----------------------------------------------------------------------------*
 *  LsStartStopScan
 */
/*! \brief Start or stop scanning, with or without a whitelist.
 *
 *  If the device is configured in the GAP Observer role, then the scan will
 *  return all advertising events received from other devices. In this role the
 *  whitelist can optionally be used to filter out events from device the
 *  application is not interested in.
 *
 *  If the device is configured in the GAP Central role, then the device will
 *  use the discovery mode (set by the discovery mode parameter to GapSetMode()
 *  at the same time as enabling the GAP Central role) to determine what type
 *  of scan should be performed. If the discovery mode is Limited Discovery or
 *  General Discovery then the advertising events will be filtered according to
 *  the rules of those procedures. Any other discovery mode is considered
 *  invalid for a GAP Central device, but will simply result in an un-filtered
 *  scan (useful if a GAP Central device wants to perform an Observer-style scan
 *  with or without whitelisting).
 *
 *  \param  go  TRUE (re)starts scanning, FALSE stops scanning.
 *  \param  wl_mode  Whether or not to use the whitelist (not used with the
 *      Limited Discovery or General Discovery procedures of the Central role).
 *  \param  addr_type  Whether to scan using the device's public
 *      Bluetooth address, or a private "random" address.
 *
 *  \returns  ls_err_none on success, or an error code if the action fails.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsStartStopScan(bool go, whitelist_mode wl_mode, ls_addr_type addr_type);


/*----------------------------------------------------------------------------*
 *  GapLsFindAdType
 */
/*! \brief Search an advertising report for the specified GAP AD type.
 *
 *  This helper function can be used to parse an advertising or scan response
 *  data field for GAP-defined AD types (see also \ref ad_type). If the
 *  requested type is found, the associated data (excluding the AD type header
 *  byte) will be copied into the buffer supplied by the caller (up to a
 *  maximum length). The total length copied is then returned.
 *
 *  \param  evt_data  Pointer to an advertising report event
 *  \param  type  The AD type to search for
 *  \param  ad_field  Pointer to a buffer to copy AD data into (as packed data)
 *  \param  max_length  The maximum number of bytes to copy
 *
 *  \returns  Number of bytes copied (0 if type was not found or had empty data).
 */
/*---------------------------------------------------------------------------*/
extern uint16 GapLsFindAdType(HCI_EV_DATA_ULP_ADVERTISING_REPORT_T * const evt_data,
                              ad_type type,
                              uint16* ad_field,
                              uint16 max_length);


/*----------------------------------------------------------------------------*
 *  LsSetTgapConnParamTimeout
 */
/*! \brief Set TGAP(conn_param_timeout) to a new value.
 *
 *  This function enables a slave to initiate master-slave data bursts, or an
 *  operating system to quickly negotiate optimal connection parameters.
 *  It sets TGAP(conn_param_timeout) to a user-selected value.
 *
 *  The slave shall not send an L2CAP Connection Parameter Update Request
 *  within TGAP(conn_param_timeout) of an L2CAP Connection Parameter Update
 *  Response being received (refer to Bluetooth Specification v4.0 Volume 3
 *  Part C Section 9.3.9.2).
 *
 *  NOTE: Using this function can reduce battery life. It is the application's
 *  responsibility to restore the recommended value as soon as possible after
 *  using this function.
 *
 *  \param  timeout  The timeout value in units of 10ms. Minimum is zero.
 *                   Maximum is the recommended value of 30 seconds (refer to
 *                   Bluetooth Specification v4.0 Volume 3 Part C Section 16).
 *
 *  \returns  ls_err_none on success, or an error code if the action fails.
 */
/*---------------------------------------------------------------------------*/
extern ls_err LsSetTgapConnParamTimeout(uint16 timeout);

/*! @} */

#endif /* __LS_APP_IF_H__ */

