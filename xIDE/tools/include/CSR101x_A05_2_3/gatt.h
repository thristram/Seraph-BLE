/*! \file gatt.h
 *
 *  \brief Defines the GATT interface to the application.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 *
 */

#ifndef __GATT_H__
#define __GATT_H__

/*============================================================================*
ANSI C & System-wide Header Files
*============================================================================*/
#include "bluetooth.h"
#include "types.h"


/*============================================================================*
Interface Header Files
*============================================================================*/
#include "gatt_prim.h"


/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*! \addtogroup GCOM
 * @{
 */
/*----------------------------------------------------------------------------*
 *  GattInit
 */
/*! \brief Initialisation function for GATT module
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void GattInit(void);

/*----------------------------------------------------------------------------*
 *  GattConnectReq
 */
/*! \brief This function is used to map ATT fixed channel for BLE-U connection,
 *  for master role and slave role connections.
 *
 *  <ul>
 *  <li><b>Master Role</b>
 *
 *  In the master role, the device will attempt to establish a BLE-U connection
 *  towards the remote device if a connection doesn't already exist. The \c flags
 *  parameter is used to map onto the corresponding GAP Connection Procedures
 *  (see Vol.3 Part C Section 9.3 of the Bluetooth Specification) as follows:
 *
 *  <b>Directed Connection Establishment Procedure</b>: \c 'flags' must be set to
 *  L2CAP_CONNECTION_MASTER_DIRECTED. \c 'bd_addr' is used to specify the address
 *  of the slave device to connect to.
 *
 *  <b>Auto Connection Establishment Procedure</b>: \c 'flags' must be set to
 *  L2CAP_CONNECTION_MASTER_WHITELIST. \c 'bd_addr' is not used in this procedure
 *  and can be set to NULL. Prior to starting this procedure the application
 *  must have initialised the device whitelist using LsAddWhiteListDevice() etc.
 *
 *  <b>Selective Connection Establishment Procedure</b>: the application
 *  currently needs to implement this procedure manually, by: (a) populating the
 *  whitelist; (b) scanning for devices in the whitelist using LsStartStopScan();
 *  (c) stopping the scan when it gets an advert; (d) configuring connection
 *  setup parameters for that device using LsSetNewConnectionParamReq(); and
 *  then (e) performing the Directed Connection Establishment Procedure as
 *  described above.
 *
 *  <b>General Connection Establishment Procedure</b>: the application currently
 *  needs to implement this procedure manually, by: (a) scanning for devices
 *  using LsStartStopScan(); (b) stopping the scan when it gets an advert from
 *  a device it wishes to connect to (which could be the first device, or only
 *  after a user has confirmed a connection); and then (c) performing the
 *  Directed Connection Establishment Procedure as described above.
 *
 *  For all master connection setup procedures, the application can control the
 *  connection parameters by calling LsSetNewConnectionParamReq() prior to
 *  calling GattConnectReq(), even for GAP procedures that don't explicitly
 *  require such control.
 *
 *  <li><b>Slave Role</b>
 *
 *  In the slave role, the device will start Directed or Undirected Connectable
 *  advertising (subject to GAP connectable mode set by GapSetMode()) if a
 *  connection doesn't already exist. \c 'flags' must be set to
 *  L2CAP_CONNECTION_SLAVE. When using Undirected Connectable advertising, the
 *  application can set up the whitelist prior to calling this function and
 *  then set \c 'flags' to L2CAP_CONNECTION_SLAVE_WHITELIST instead.
 *
 *  For the slave role in Directed Connectable mode, the peer (master) address
 *  to direct the advertising at must be set by calling GapSetAdvAddress()
 *  before calling GattConnectReq(). In this case \c 'bd_addr' must be set to NULL.
 *
 *  </li></ul>
 *
 *  \param bd_addr  BD_ADDR of the remote device, for the master role Directed
 *  Connection Establishment Procedure only
 *
 *  \param flags  Specify the connection type (master or slave, master procedure,
 *  use of whitelisting)
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void GattConnectReq(TYPED_BD_ADDR_T *bd_addr, uint16 flags);

/*----------------------------------------------------------------------------*
 *  GattCancelConnectReq
 */
/*! \brief Cancel a connect request
 *
 *  This function is used to cancel on-going BLE-U connection setup in master
 *  role or to stop connectable directed or undirected advertisements in Slave
 *  role.
 *
*  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void GattCancelConnectReq(void);

/*----------------------------------------------------------------------------*
 *  GattDisconnectReq
 */
/*! \brief Terminates BLE-U connection
 *
 *  Un-maps ATT fixed channel and terminates BLE-U connection with remote device
 *  if not used for any other purpose.
 *
 *  This function terminates the connection with the HCI reason code
 *  "Remote User Terminate Connection". If the application needs to use an
 *  alternative reason code then it should call GattDisconnectReasonReq()
 *  instead.
 *
 *  \param cid Connection identifier as received in GATT_CONNECT_CFM_T for
 *         established BLE-U connection.
 *
\return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void GattDisconnectReq(uint16 cid);

/*----------------------------------------------------------------------------*
 *  GattDisconnectReasonReq
 */
/*! \brief Terminates BLE-U connection, specifying a non-default reason code.
 *
 *  Un-maps ATT fixed channel and terminates BLE-U connection with remote device
 *  if not used for any other purpose.
 *
 *  This function terminates the connection with the HCI reason code supplied
 *  in the \c disconnect_reason parameter. It is the responsibility of the
 *  application to ensure that a valid reason code is used. In most cases it
 *  will be sufficient to use GattDisconnectReq() instead, for the default
 *  disconnect reason "Remote User Terminate Connection". However, this function
 *  can be used, for example, if the application wishes to disconnect after a
 *  request to a remote master to update connection parameters has timed out.
 *  In that case the error code "Unacceptable Connection Interval" must be used.
 *
 *  This function will generate a GATT_DISCONNECT_CFM event when it completes.
 *
 *  \param cid Connection identifier as received in GATT_CONNECT_CFM_T for
 *         established BLE-U connection.
 *  \param disconnect_reason HCI reason code
 *
\return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void GattDisconnectReasonReq(uint16 cid, ls_err disconnect_reason);

/*! @} */

/*============================================================================*
 * GATT Client interface
 *============================================================================*/

/*! \addtogroup GCLI
 * @{
 */

/*---------------------------------------------------------------------------*
 *  GattInstallClientRole
 */
/*! \brief Install GATT Client functionality.
 *
 *  This will install all mandatory GATT Client procedures. It should be called 
 *  after GattInit() in any application that operates as a GATT Client. This
 *  function must be called before any other GATT Client functions are called
 *  to guarantee correct operation.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void GattInstallClientRole(void);

/*! \brief As client, exchange the maximum RX MTU supported by the device.
 *
 *  This function should preferably be used when the GATT client supports an 
 *  ATT_MTU value greater than default the ATT_MTU (23 octets) 
 *  for Attribute Protocol data. This procedure shall only be initiated once 
 *  during a connection.
 *
 *  Note: CSR1000 currently only supports default ATT_MTU (23 octets) value
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param client_rx_mtu Maximum RX MTU supported by GATT client
 *
 *  \returns       sys_status_success if successful or else an error code. 
 */
extern sys_status GattExchangeMtuReq(uint16 cid, uint16 client_rx_mtu);

/*! \brief As client, discover all primary services on a server.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattDiscoverAllPrimaryServices(uint16 cid);

/*! \brief As client, discover a specific primary service on a server when 
 *         only the Service UUID is known. 
 *
 *  The specific primary service may exist multiple times on a server.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param uuid_type 16-bit (GATT_UUID16) or 128-bit (GATT_UUID128) UUID
 *  \param uuid    Pointer to an array containing UUID value in big-endian format
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattDiscoverPrimaryServiceByUuid(uint16 cid,
                                                      GATT_UUID_T uuid_type,
                                                      uint16* uuid);

/*! \brief As client, find included service declarations within a service 
 *         definition on a server. 
 *
 *  The service specified is identified by the service handle range.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param start_handle Start handle of the specified service
 *  \param end_handle   End handle of the specified service
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattFindIncludedServices(uint16 cid,
                                              uint16 start_handle,
                                              uint16 end_handle);

/*! \brief As client, discover all service characteristics or discover 
 *         characteristics by UUID
 *
 *  If the uuid_type is GATT_UUID_NONE, and uuid is NULL, then
 *  discover all service characteristics within the handle range.
 *
 *  Otherwise, discover characteristics by UUID and service handle range.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param start_handle Start handle of the specified service
 *  \param end_handle   End handle of the specified service
 *  \param uuid_type    16-bit (GATT_UUID16) or 128-bit (GATT_UUID128) UUID
 *  \param uuid    Pointer to an array containing UUID value in big-endian format
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattDiscoverServiceChar(uint16 cid,
                                             uint16 start_handle,
                                             uint16 end_handle,
                                             GATT_UUID_T uuid_type,
                                             uint16* uuid);

/*! \brief As client, find characteristic descriptors by handle range.
 *
 *  Find all the characteristic descriptor's attribute handles and 
 *  attribute types within a characteristic definition when only 
 *  the characteristic handle range is known. 
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param start_handle Start handle of the specified characteristic 
 *  \param end_handle   End handle of the specified characteristic
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattDiscoverAllCharDescriptors(uint16 cid,
                                                    uint16 start_handle,
                                                    uint16 end_handle);

/*! \brief As client, stop a procedure.
 *
 *  Can be used to stop any of the following on-going procedures 
 *  before completion.
 *
 *  - \ref GattDiscoverAllPrimaryServices
 *  - \ref GattDiscoverPrimaryServiceByUuid
 *  - \ref GattFindIncludedServices
 *  - \ref GattDiscoverServiceChar
 *  - \ref GattDiscoverAllCharDescriptors
 *  - \ref GattReadCharUsingUuid
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *
 *  \returns       Nothing
 */
extern void GattStopCurrentProcCmd(uint16 cid);

/*! \brief As client, read a Characteristic from a server by Handle.
 *
 *  May be used to read a characteristic value or descriptor from a server 
 *  when the client knows the characteristic's Attribute handle.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *
 *  \param handle  Characteristic value handle (to read Characteristic Value)
 *                 OR
 *                 Characteristic descriptor handle (to read Characteristic 
 *                 Descriptor)
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattReadCharValue(uint16 cid, uint16 handle);


/*! \brief As client, read a Characteristic Value by UUID.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *
 *  \param start_handle Start handle of the specified service to which 
                   characteristic belongs
 *  \param end_handle   End handle of the specified service to which characteristic 
                   belongs
 *  \param uuid_type    16-bit (GATT_UUID16) or 128-bit (GATT_UUID128) UUID
 *  \param uuid    Pointer to an array containing the UUID value in big-endian format
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattReadCharUsingUuid(uint16 cid,
                                           uint16 start_handle,
                                           uint16 end_handle,
                                           GATT_UUID_T uuid_type,
                                           uint16* uuid);

/*! \brief As client, read a long Characteristic by handle.
 *
 *  May be used to read a characteristic value or descriptor from a server 
 *  when the client knows the characteristic's Attribute handle.
 *
 *  This procedure should be used if the length of the Characteristic Value 
 *  or Descriptor to be read is longer than can be sent in a single Read 
 *  Response Attribute Protocol message.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *
 *  \param handle  Characteristic value handle (to read Characteristic Value)
 *                 OR
 *                 Characteristic descriptor handle (to read Characteristic 
 *                 Descriptor)
 *  \param value_offset Offset within the characteristic to be read. To 
 *                 read the complete Characteristic set value_offset to 0x00.
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattReadLongCharValue(uint16 cid,
                                           uint16 handle,
                                           uint16 value_offset);

/*! \brief As client, read multiple Characteristic Values by handles.
 *
 *  Note: The characteristic values to be read should have a known fixed length 
 *  with the exception of the last value that can have variable length.
 *
 *  Note: A client should not request multiple Characteristic Values when the 
 *  response's Set Of Values parameter is equal to (ATT_MTU - 1) octets in 
 *  length since it is not possible to determine if the last Characteristic 
 *  Value was read or additional Characteristic Values exist but were truncated.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param size_handles Number of elements in the 'handles' array
 *  \param handles Pointer to an array of characteristic value handles, in 
 *                 the order that the values are required in response
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattReadMultipleCharValues(uint16 cid,
                                                uint16 size_handles,
                                                uint16 *handles);

/*! \brief As client, write a Characteristic by handle. 
 *
 *  May be used to write a characteristic value or descriptor on a server
 *  when the client knows the characteristic's handle.
 *
 *  Used in following GATT procedures. These procedures only write the first 
 *  (ATT_MTU - 3) octets of a characteristic value or descriptor.
 *
 *  - Write Without Response - used when the client does not 
 *    need an acknowledgement that the write was successfully performed. 
 *
 *  - Signed Write Without Response - used when the ATT bearer 
 *    is not encrypted. This procedure shall only be used if the Characteristic 
 *    Properties authenticated bit is enabled and the client and server device 
 *    share a bond. <b>NOT CURRENTLY SUPPORTED!</b>
 *
 *  - Write Characteristic Value - This function is used when the client needs 
 *    an acknowledgement that the write was successfully performed.
 *
 *  If the application wishes to stream data to the server it can use the
 *  Write Without Response procedure. This procedure checks to ensure that
 *  internal buffer exhaustion cannot occur as a direct result of the streaming
 *  Write Commands. If this check fails then the function will return status
 *  code #gatt_status_busy.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param flags   The following flags are used to distinguish the sub-procedures
 *                 - GATT_WRITE_COMMAND - Write Without Response
 *                 - GATT_WRITE_SIGNED  - Signed Write Without Response
 *                 - GATT_WRITE_REQUEST - Write Characteristic Value or Descriptor
 *  \param handle  Characteristic handle for value or descriptor
 *  \param size_value Size of characteristic value or descriptor in octets
 *  \param value   Pointer to an array containing characteristic 
 *                 in LITTLE ENDIAN format
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattWriteCharValueReq(uint16 cid,
                                        uint16 flags,
                                        uint16 handle,
                                        uint16 size_value,
                                        uint8 *value);


/*! \brief As client, write a long Characteristic value or descriptor by handle.
 *
 *  Used when the client knows the Characteristic value handle but the 
 *  length of the Characteristic value is longer than (ATT_MTU - 3) octets. 
 *
 *  This function is also used for reliable writes of a long Characteristic 
 *  value or descriptor. 
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param handle  Characteristic value handle (to write Characteristic Value)
 *                 OR
 *                 Characteristic descriptor handle (to write Characteristic 
 *                 Descriptor)
 *  \param offset  Characteristic offset to which the value is written
 *  \param size_value Size of characteristic value or descriptor in octets
 *  \param value   Pointer to an array containing characteristic value or
 *                 descriptor in LITTLE ENDIAN format. 
 *                 The application must preserve the characteristic value 
 *                 memory until the procedure is completed.
 *  \param reliable TRUE, if assurance is required that the correct 
 *                 Characteristic value is going to be written 
 *                 before the write is performed
 *
 *  \returns       sys_status_success if successful or else an error code.
 */
extern sys_status GattWriteLongCharValueReq(uint16 cid,
                                               uint16 handle,
                                               uint16 offset,
                                               uint16 size_value,
                                               uint8 *value,
                                               bool reliable);

/*! @} */


/*============================================================================*
 * GATT Server interface 
 *============================================================================*/


/*! \addtogroup GSER
 * @{
 */


/*! \brief Install GATT Server support for the optional Exchange MTU procedure.
 *
 *  When installed, the application will receive a #GATT_EXCHANGE_MTU_IND
 *  message when a remote client starts an MTU negotiation. If the procedure is
 *  not installed then the GATT layer will automatically negotiate the MTU
 *  based on the firmware default of #ATT_MTU_DEFAULT.
 *
 *  \returns       Nothing
 */
extern void GattInstallServerExchangeMtu(void);


/*! \brief Install GATT Server support for the optional Read Multiple
    Characteristic Values procedure.
 *
 *  If the procedure is not installed then the GATT layer will reject any
 *  attempt by a remote client to initiate it.
 *
 *  \returns       Nothing
 */
extern void GattInstallServerReadMultiple(void);


/*! \brief Install GATT Server support for the optional Write Characteristic
 *  Value, Write Without Response, Signed Write Without Response, and
 *  Write Characteristic Descriptors procedures.
 *
 *  This function does not need to be called if the application calls
 *  GattInstallServerWriteLongReliable().
 *
 *  If the procedures are not installed then the GATT layer will reject any
 *  attempt by a remote client to initiate them.
 *
 *  \returns       Nothing
 */
extern void GattInstallServerWrite(void);


/*! \brief Install GATT Server support for the optional Write Long
 *  Characteristic Value, Write Long Characteristic Descriptor, and
 *  Characteristic Value Reliable Write procedures.
 *
 *  If Write Long Characteristic Value is supported then it is mandatory to
 *  also support Write Characteristic Value on the server. Therefore calling
 *  this function will automatically install support for the other Write
 *  procedures.
 *
 *  If the procedures are not installed then the GATT layer will reject any
 *  attempt by a remote client to initiate them.
 *
 *  \returns       Nothing
 */
extern void GattInstallServerWriteLongReliable(void);


/*! \brief As server, add a complete attribute database for supported services
 *
 * Used in GATT server role to add a complete attribute 
 * database for supported services. Once the attribute database is registered, 
 * the application shall not try to update the attribute database directly.
 *
 * Calling this function will instantiate all mandatory GATT Server procedures.
 *
 * Note: the application shall preserve the attribute database memory. 
 * This memory shall not be used for any other purpose.
 *
 * \param size_db Size of data base in words
 * \param db      Pointer to a data base array of service attributes in 
 *                LITTLE ENDIAN format
 *
 * \returns       Nothing
 */
extern void GattAddDatabaseReq(uint16 size_db, uint16 *db);


/*! \brief As server, notify a characteristic value to a client, not expecting an ACK
 *
 *  Used in GATT server role to notify a Characteristic Value 
 *  to a client without expecting any Attribute Protocol layer acknowledgement.
 *
 *  Data streaming using Notifications is supported. The server will check that
 *  each request will not result in buffer exhaustion. If the check fails then
 *  the corresponding CFM message will return #gatt_status_busy.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param handle  Characteristic value handle
 *  \param size_value Size of characteristic value in octets
 *  \param value   Pointer to an array containing characteristic value in 
 *                 LITTLE ENDIAN format
 *
 *  \returns       Nothing
 */
extern void GattCharValueNotification(uint16 cid, uint16 handle, 
                                      uint16 size_value, uint8 *value);


/*! \brief As server, indicate a characteristic value to a client, expecting an ACK
 * 
 *  Used in GATT server role to indicate a characteristic 
 *  value to a client and expects an Attribute Protocol layer acknowledgement.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param handle  Characteristic value handle
 *  \param size_value Size of characteristic value in octets
 *  \param value   Pointer to an array containing characteristic value in 
 *                 LITTLE ENDIAN format
 *
 *  \returns       Nothing
 */
extern void GattCharValueIndication(uint16 cid, uint16 handle, 
                                    uint16 size_value, uint8 *value);


/*! \brief As server, respond to Exchange MTU
 *
 *  Used in GATT server role to respond to an Exchange MTU
 *  indication with the maximum MTU supported by the server.
 *
 *  Note: CSR1000 currently only supports the default ATT_MTU (23 octets) value.
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param server_rx_mtu Maximum RX MTU supported by the GATT server
 *
 *  \returns       Nothing
 */
extern void GattExchangeMtuRsp(uint16 cid, uint16 server_rx_mtu);


/*! \brief As server, respond to #GATT_ACCESS_IND event if the
 *         #ATT_ACCESS_PERMISSION and/or #ATT_ACCESS_WRITE_COMPLETE flags
 *         have been set
 *
 *  \warning If the application wishes to return a GATT Application Error Code
 *  in the range 0x80 - 0xFF (see Bluetooth Specification v4.0 Vol.3 Part F
 *  Section 3.4.1) then it *must* OR the value in that range with
 *  #gatt_status_app_mask. This ensures that the status code is located within
 *  the correct block of status codes within the firmware sys_status enumerated
 *  type. If the application error code is not ORed with this value then the
 *  resulting status code sent to tbe peer device will be "Unlikely Error".
 *
 *  \param cid     Connection identifier for established BLE-U connection
 *  \param handle  Attribute handle
 *  \param rc      GATT error code or success (#sys_status_success,
 *                 #gatt_status_irq_proceed, or Application Error code as
 *                 described in the warning above.)
 *  \param size_value Length of the value
 *  \param value   pointer to an array containing Attribute value in
 *                 LITTLE ENDIAN format
 *
 *  \returns       Nothing 
 *
 *  \note          Applications that handle the database access themselves
 *                 should return #sys_status_success, whereas applications
 *                 that want the ATT stack to handle database access should
 *                 return #gatt_status_irq_proceed. Either way, behaviour
 *                 is undefined if different success codes are returned for
 *                 different phases of a single transaction - for instance
 *                 in a Write Long request.
 *
 *                 The application receiving a #GATT_ACCESS_IND event shall
 *                 treat it as an atomic event and return GattAccessRsp()
 *                 immediately without any context switch or other GATT calls.
 */
extern void GattAccessRsp(uint16 cid, uint16 handle, sys_status rc, 
                          uint16 size_value, uint8 *value);

/****************************************************************/
/* The following functions are simple wrappers intended for test
 * purposes and consequently are excluded from published documentation
 */
/*! \cond TEST_WRAPPERS */

/**************************************************************************/
/*! \brief Wrapper function for the ATT protocol ATT_PREPARE_WRITE message.
 *
 *  \note  This function is included for CSR internal testing only,
 *         and is not part of the supported API.
 *
 *  \param cid        Connection identifier for established BLE-U connection
 *  \param handle     Attribute handle
 *  \param offset     Offset into the characteristic to commence writing to
 *  \param size_value Number of octets to write
 *  \param value      Pointer to an array containing Attribute value to write
 *                    LITTLE ENDIAN format. This should contain no more than
 *                    MTU-5 octets of data
 *
 *  \return Nothing
 */
extern void GattAttPrepareWriteReq( uint16 cid,
                                    uint16 handle,
                                    uint16 offset,
                                    uint16 size_value,
                                    uint8 *value );

/**************************************************************************/
/*! \brief Wrapper function for the ATT protocol ATT_EXECUTE_WRITE message.
 *
 *  \note  This function is included for CSR internal testing only,
 *        and is not part of the supported API.
 *
 *  \param cid        Connection identifier for established BLE-U connection
 *  \param flags      Set to either ATT_EXECUTE_CANCEL or ATT_EXECUTE_WRITE
 *
 *  \return Nothing
 */
extern void GattAttExecuteWriteReq( uint16 cid, uint16 flags );

/*! \endcond */

/*! @} */


#endif /* __GATT_H__ */
