/*! \file core_event_types.h
 *
 *  \brief Core type definitions used with all other event definitions.
 *
 * Copyright (c) 2011 - 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __CORE_EVENT_TYPES_H__
#define __CORE_EVENT_TYPES_H__

#include "bluetooth.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/*! \name GATT primitives segmentation and numbering
 *
 * \brief GATT primitives occupy the number space from GATT_PRIM_BASE to
 * (GATT_PRIM_BASE | 0x00FF). SM and LS primitives also occupy this space but
 * are partitioned into their own sub-sections.
 *
 * \{
 */
#define GATT_PRIM_DOWN          (GATT_PRIM_BASE)
#define SM_PRIM_DOWN            (GATT_PRIM_DOWN + 0x0040)
#define LS_PRIM_DOWN            (GATT_PRIM_DOWN + 0x0060)
/* Create a second  group of LS events lower down in one of the gaps */
#define LS2_PRIM_DOWN           (GATT_PRIM_DOWN + 0x0030)

#define GATT_PRIM_UP            (GATT_PRIM_DOWN | 0x0080)
#define SM_PRIM_UP              (SM_PRIM_DOWN | 0x0080)
#define LS_PRIM_UP              (LS_PRIM_DOWN | 0x0080)
#define LS2_PRIM_UP             (LS2_PRIM_DOWN | 0x0080)

/* \} */

/*! \name Application System primitives segmentation and numbering
 *
 * \brief Application system primitives consist of core system events not
 * directly associated with the Bluetooth stack, for example the Background Tick
 * event.
 *
 * \{
 */
#define SYS_APP_PRIM_DOWN       (SYS_APP_PRIM_BASE)
#define SYS_APP_PRIM_UP         (SYS_APP_PRIM_DOWN + 0x0040)
#define SYS_APP_PRIM_TEST       (SYS_APP_PRIM_DOWN + 0x0080)    /* For CSR test */

/* \} */


/*! \brief Definitions of the event identifiers used by LM_EVENT_T to select
 * the structure of each event.
 * This enumeration lists all possible requests that the host
 * application can make of or receive from the \muEnergy Command Interface.
 * \n Event codes 0x0000 - 0x003D are the standard Bluetooth HCI event codes,
 * and are not documented here. See Volume 2 Part E section 7.7 of
 * the Bluetooth Specification v4.1 for details.
 */

#ifdef _WIN32
typedef enum : uint16
#else
typedef enum
#endif
{
    /**** HCI Events *********************************************************/
    /*! The Disconnection Complete event is used to indicate that a connection is terminated.
     * See Volume 2 Part E section 7.7.5 of the Bluetooth Specification v4.1 for details.
     * See also LM_EV_DISCONNECT_COMPLETE_T.
     */
     LM_EV_DISCONNECT_COMPLETE       = 0x05,

    /*! The Encryption Change event is used to indicate that the change of the encryption
     * mode has been completed. See Volume 2 Part E section 7.7.8 of
     * the Bluetooth Specification v4.1 for details. See also LM_EV_ENCRYPTION_CHANGE_T.
     */
    LM_EV_ENCRYPTION_CHANGE         = 0x08,

    /*! \brief HCI Remote Version Info event. See Volume 2 Part E
     * section 7.7.12 of the spec. See also LM_EV_REMOTE_USED_FEATURES_T.
     */
    LM_EV_REMOTE_VERSION_INFO       = 0x0C,

    LM_EV_COMMAND_COMPLETE          = 0x0E, /*!< \brief For internal use. */
    LM_EV_COMMAND_STATUS            = 0x0F, /*!< \brief For internal use. */
    LM_EV_HARDWARE_ERROR            = 0x10,
    LM_EV_NUMBER_COMPLETED_PACKETS  = 0x13,
    LM_EV_BUFFER_OVERFLOW           = 0x1A,
    LM_EV_ENCRYPTION_KEY_REFRESH    = 0x30,

    /*! \brief BLE Connection Complete meta event, handled by GATT
     *  module. See Volume 2 Part E Section 7.7.65.1 of the spec.
     *  See also LM_EV_CONNECTION_COMPLETE_T.
     */
    LM_EV_CONNECTION_COMPLETE       = 0x3E,

    /*! \brief BLE Advertising Report meta event. See Volume 2 Part E
     *  section 7.7.65.2 of the spec. See also LM_EV_ADVERTISING_REPORT_T.
     */
    LM_EV_ADVERTISING_REPORT,

    /*! \brief BLE Connection Update meta event, handled by LS module.
     *  See Volume 2 Part E section 7.7.65.3 of the spec.
     *  See also LM_EV_CONNECTION_UPDATE_T.
     */
    LM_EV_CONNECTION_UPDATE,

    /*! \brief BLE Remote Used Features meta event. See Volume 2 Part
     *  E section 7.7.65.4 of the spec. See also LM_EV_REMOTE_USED_FEATURES_T.
     */
    LM_EV_REMOTE_USED_FEATURES,

    /*! \brief BLE Long Term Key Request meta event, handled internally by the
     * Security Manager module. See Volume 2 Part E section 7.7.65.5
     * of the spec. See also LM_EV_LONG_TERM_KEY_REQUESTED_T.
     */
    LM_EV_LONG_TERM_KEY_REQUESTED,

    /*! \brief For internal use */
    LM_EV_ACL_DATA_START,
    /*! \brief For internal use */
    LM_EV_ACL_DATA_CONT,

    /*! \brief Manufacturer-specific HCI event */
    LM_EV_MANUFACTURER_EXTENSION = 0xFF,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattAddDatabaseReq() */
    GATT_ADD_DB_REQ = GATT_PRIM_DOWN,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused */
    GATT_ADD_DB_SEGMENT_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Try to establish a connection with the specified remote device.
     * \note This command is appropriate only when this device has the Central 
     * role. It will result in this device being the link Master. 
	 * Corresponds to GattConnectReq() */
    GATT_CONNECT_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Cancel a connect attempt initiated using the 
     * \ref GATT_CONNECT_REQ message. Corresponds to GattCancelConnectReq() */
    GATT_CANCEL_CONNECT_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: Disconnect from the remote device (if any). corresponds to GattDisconnectReq() */
    GATT_DISCONNECT_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattDisconnectReasonReq() */
    GATT_DISCONNECT_REASON_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattExchangeMtuReq() */
    GATT_EXCHANGE_MTU_REQ,

    /*! \ingroup related_to_confirm_messages */
    /*! \brief Unused: corresponds to GattExchangeMtuRsp() */
    GATT_EXCHANGE_MTU_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattDiscoverAllPrimaryServices() */
    GATT_DISC_ALL_PRIM_SERV_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattDiscoverPrimaryServiceByUuid() */
    GATT_DISC_PRIM_SERV_BY_UUID_REQ,

    /*! \brief Unused: corresponds to GattFindIncludedServices() */
    GATT_FIND_INCLUDED_SERV_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattDiscoverServiceChar() */
    GATT_DISC_SERVICE_CHAR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattDiscoverAllCharDescriptors() */
    GATT_DISC_ALL_CHAR_DESC_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattReadCharValue() */
    GATT_READ_CHAR_VAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattReadCharUsingUuid() */
    GATT_READ_CHAR_USING_UUID_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattReadLongCharValue() */
    GATT_READ_LONG_CHAR_VAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattReadMultipleCharValues() */
    GATT_READ_MULTI_CHAR_VAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattWriteCharValueReq() */
    GATT_WRITE_CHAR_VAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattWriteLongCharValueReq() */
    GATT_WRITE_LONG_CHAR_VAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattCharValueNotification() */
    GATT_CHAR_VAL_NOTIFICATION_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattCharValueIndication() */
    GATT_CHAR_VAL_INDICATION_REQ,

    /*! \ingroup related_to_confirm_messages */
    /*! \brief Unused: corresponds to GattAccessRsp() */
    GATT_ACCESS_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattStopCurrentProcCmd() */
    GATT_STOP_CURRENT_PROC_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattAttPrepareWriteReq() */
    GATT_ATT_PREPARE_WRITE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GattAttExecuteWriteReq() */
    GATT_ATT_EXECUTE_WRITE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Used to manage automatic traffic generation for test 
     *         purposes. Supported in uci command interface from 
     *         version 0.4
     */
    GATT_TRAFFIC_GEN_REQ,

    /*! \ingroup related_to_confirm_messages */
    /*! \brief Confirmation that the attribute database has been
     * installed.
     *  see \ref GATT_ADD_DB_CFM_T
     */
    GATT_ADD_DB_CFM = GATT_PRIM_UP,

    /*! \brief Unused */
    GATT_ADD_DB_SEGMENT_CFM,

    /*! \brief Indicates completion of a GattConnectReq()
     *  see \ref GATT_CONNECT_CFM_T
     */
    GATT_CONNECT_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates remotely-initiated connection has completed
     *  see \ref GATT_CONNECT_IND_T
     */
    GATT_CONNECT_IND,

    /*! \ingroup related_to_confirm_messages */
    /*! \brief Indicates completion of a GattCancelConnectReq()
     *  see \ref GATT_CANCEL_CONNECT_CFM_T
     */
    GATT_CANCEL_CONNECT_CFM,

    /*! \brief Indicates completion of a GattDisconnectReq()
     *  see \ref GATT_DISCONNECT_CFM_T
     */
    GATT_DISCONNECT_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates a remotely-initiated disconnection has
     * occurred.
     *  see \ref GATT_DISCONNECT_IND_T
     */
    GATT_DISCONNECT_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Exchange MTU sub-procedure has completed.
     *  See Volume 3 Part G section 4.3.1 of the spec.
     *  see \ref GATT_EXCHANGE_MTU_CFM_T
     */
    GATT_EXCHANGE_MTU_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates client has initiated the Exchange MTU
     *  sub-procedure.  See Volume 3 Part G section 4.3.1 of the spec.
     *  The application must respond by calling GattExchangeMtuRsp().
     *  see \ref GATT_EXCHANGE_MTU_IND_T
     */
    GATT_EXCHANGE_MTU_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Discover All Primary Services
     *  sub-procedure (see Volume 3 Part G section 4.4.1 of the spec)
     *  has completed.  Service data is returned in GATT_SERV_INFO_IND
     *  messages.
     *  see \ref GATT_DISC_ALL_PRIM_SERV_CFM
     */
    GATT_DISC_ALL_PRIM_SERV_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Lists services discovered through the service discovery
     *  procedures.
     *  see \ref GATT_SERV_INFO_IND_T
     */
    GATT_SERV_INFO_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Discover Primary Service by Service UUID
     *  sub-procedure (see Volume 3 Part G section 4.4.2 of the spec)
     *  has completed.
     *  see \ref GATT_DISC_PRIM_SERV_BY_UUID_CFM_T
     */
    GATT_DISC_PRIM_SERV_BY_UUID_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Contains service data requested by
     * GattDiscoverPrimaryServiceByUuid().  See Volume 3 Part G
     * section 4.4.2 of the spec.
     *  see \ref GATT_DISC_PRIM_SERV_BY_UUID_IND_T
     */
    GATT_DISC_PRIM_SERV_BY_UUID_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Find Included Services sub-procedure (see
     * Volume 3 Part G section 4.5.1 of the spec) has completed.
     * Service data are returns in GATT_SERV_INFO_IND messages.
     *  see \ref GATT_FIND_INCLUDED_SERV_CFM_T
     */
    GATT_FIND_INCLUDED_SERV_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Discover All Characteristics of a Service
     *  sub-procedure (see Volume 3 Part G section 4.6.1 of the spec)
     *  or the Discover Characteristics by UUID sub-procedure (section
     *  4.6.2) has completed.  Service characteristics are returned in
     *  GATT_CHAR_DECL_INFO_IND messages.
     *  see \ref GATT_DISC_SERVICE_CHAR_CFM_T
     */
    GATT_DISC_SERVICE_CHAR_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Lists characteristics discovered through the
     *  characteristic discovery procedures.
     *  see \ref GATT_CHAR_DECL_INFO_IND_T
     */
    GATT_CHAR_DECL_INFO_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Discover All Characteristic Descriptors
     *  sub-procedure (see Volume 3 Part G section 4.7.1 of the spec)
     *  has completed.  Characteristic descriptors are returned in
     *  GATT_CHAR_DESC_INFO_IND messages.
     *  see \ref GATT_DISC_ALL_CHAR_DESC_CFM_T
     */
    GATT_DISC_ALL_CHAR_DESC_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Lists characteristic descriptors discovered through the
     *  characteristic discovery procedures.
     *  see \ref GATT_CHAR_DESC_INFO_IND_T
     */
    GATT_CHAR_DESC_INFO_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Contains the characteristic value requested by
     *  GattReadCharValue().  See Volume 3 Part G section 4.8.1 for
     *  details.
     *  see \ref GATT_READ_CHAR_VAL_CFM_T
     */
    GATT_READ_CHAR_VAL_CFM,

    /*! \brief Indicates the Read Using Characteristic UUID
     *  sub-procedure (see Volume 3 Part G section 4.8.2 of the spec)
     *  has completed.  Characteristic values are returned in
     *  GATT_CHAR_VAL_IND messages.
     *  see \ref GATT_READ_CHAR_USING_UUID_CFM_T
     */
    GATT_READ_CHAR_USING_UUID_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Contains the characteristic value requested by
     *  GattReadCharUsingUUid().
     *  see \ref GATT_CHAR_VAL_IND_T
     */
    GATT_UUID_CHAR_VAL_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Read Long Characteristic Values
     *  sub-procedure (see Volume 3 Part G section 4.8.3 of the spec)
     *  has completed.  Characteristic values are returned in
     *  GATT_LONG_CHAR_VAL_IND messages.
     *  see \ref GATT_READ_LONG_CHAR_VAL_CFM_T
     */
    GATT_READ_LONG_CHAR_VAL_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Contains the characteristic value requested through the
     *  Read Long Characteristic Values sub-procedure.
     *  see \ref GATT_LONG_CHAR_VAL_IND_T
     */
    GATT_LONG_CHAR_VAL_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Contains a characteristic values requested through the
     *  Read Multiple Characteristic Values sub-procedure (see Volume
     *  3 Part G section 4.8.4 of the spec).  One message will be
     *  generated for each value requested.
     *  see \ref GATT_READ_MULTI_CHAR_VAL_CFM_T
     */
    GATT_READ_MULTI_CHAR_VAL_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates that a Characteristic Value Write procedure
     *  other than the Write Long Characteristic Values sub-procedure
     *  has completed.  See Volume 3 Part G section 4.9 of the spec
     *  for details.
     *  see \ref GATT_WRITE_CHAR_VAL_CFM_T
     */
    GATT_WRITE_CHAR_VAL_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Write Long Characteristic Values
     *  sub-procedure (see Volume 3 Part G section 4.9.4 of the spec)
     *  has completed.
     *  see \ref GATT_WRITE_LONG_CHAR_VAL_CFM_T
     */
    GATT_WRITE_LONG_CHAR_VAL_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Indications sub-procedure (see Volume 3
     *  Part G section 4.11.1 of the spec) has completed.
     *  see \ref GATT_CHAR_VAL_IND_CFM_T
     */
    GATT_CHAR_VAL_IND_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the Notifications sub-procedure (see Volume 3
     *  Part G section 4.10.1 of the spec) has completed.
     *  see \ref GATT_CHAR_VAL_IND_CFM_T
     */
    GATT_CHAR_VAL_NOT_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates that an attribute controlled directly by the
     *  application (ATT_ATTR_IRQ attribute flag is set) is being read from or
     *  written to.  The application shall shall treat it as an atomic event
     *  and respond by calling GattAccessRsp()immediately without any context
     *  switch or calling other gatt functions.
     *  see \ref GATT_ACCESS_IND_T
     */
    GATT_ACCESS_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates the peer has indicated a characteristic value
     *  (see Volume 3 Part G section 4.11.1 of the spec).
     *  see \ref GATT_CHAR_VAL_IND_T
     */
    GATT_IND_CHAR_VAL_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates the peer has notified a characteristic value
     *  (see Volume 3 Part G section 4.10.1 of the spec).
     *  see \ref GATT_CHAR_VAL_IND_T
     */
    GATT_NOT_CHAR_VAL_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the peer has accepted the ATT prepare write request
     *  see \ref GATT_ATT_PREPARE_WRITE_CFM_T
     */
    GATT_ATT_PREPARE_WRITE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Indicates the peer has accepted the ATT execute write request
     *  see \ref GATT_ATT_EXECUTE_WRITE_CFM_T
     */
    GATT_ATT_EXECUTE_WRITE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Response to automatic traffic generation requests.
     *         Used in uci command interface from version 0.4
     */
    GATT_TRAFFIC_GEN_CFM,
    
    /*! \ingroup related_to_indication_messages */
    /*! \brief Indication of the completion of a automatic traffic 
     *         generation operation.
     *         Supported in uci command interface from version 0.4
     */
    GATT_TRAFFIC_GEN_IND,



    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates long term key request from the peer
     */
    LS_LONG_TERM_KEY_REQUESTED_IND = LS2_PRIM_UP,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates connection update request from the peer
     */
    LS_CONNECTION_UPDATE_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates remote used request from the peer
     */
    LS_REMOTE_USED_FEATURES_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirmation that the Tx power level has been
     * modified.
     */
    LS_SET_TRANSMIT_POWER_LEVEL_CFM,
    
    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Enables the send after receive feature
     */
    LS_HOLD_TX_UNTIL_RX_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Enables data timing reports on each RX packet (master only)
     */
    LS_RX_TIMING_REPORT_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Provides data timing report to application on receiving a data packet.
     *  see \ref LS_DATA_RX_TIMING_IND_T 
     */
    LS_DATA_RX_TIMING_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief reads the connection channel map */
    GAP_GET_CONNECTION_CHANNEL_MAP_CFM = 2745,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief set the connection channel map */
    GAP_SET_CONNECTION_CHANNEL_MAP_CFM = 2746,

	/*! \ingroup related_to_confirmation_messages */
    /*! \brief the static address generated using seed*/
    LS_GAP_SEED_STATIC_ADDR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief set the TGAP(conn_param_timeout) timer */
    LS_GAP_SET_TGAP_CONN_PARAM_TIMEOUT_CFM,
	
    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMRequestSecurityLevel() */
    SM_SECURITY_LEVEL_REQ = SM_PRIM_DOWN,

    /*! \brief Unused: corresponds to SMKeyRequestResponse() */
    SM_KEY_REQUEST_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMAddStoredKey() */
    SM_ADD_STORED_KEY_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMRemoveStoredKey() */
    SM_REMOVE_STORED_KEY_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMSetIOCapabilities(),
     * SMSetMaxEncKeySize(), SMSetMinEncKeySize() */
    SM_CONFIGURATION_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMPasskeyDisplayed() */
    SM_PASSKEY_DISPLAY_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief A response to a \ref SM_PASSKEY_DISPLAY_IND message. Corresponds to SMPasskeyInput() */
    SM_PASSKEY_INPUT_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMPasskeyInputNeg() */
    SM_PASSKEY_INPUT_NEG_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMPrivacyRegenerateAddress() */
    SM_PRIVACY_REGENERATE_ADDRESS_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to SMDivApproval() */
    SM_DIV_APPROVAL_RSP,

    /*! \brief Unused: corresponds to SMPairingAuthRsp() */
    SM_PAIRING_AUTH_RSP,

    /*! \brief Unused */
    SM_FEATURES_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request data be encrypted */
    SM_ENCRYPT_RAW_AES_REQ,

    /*! \brief Unused: corresponds to SMLongTermKeyRsp() */
    SM_LONG_TERM_KEY_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request device's own IRK */
    SM_PRIVACY_GET_OWN_IRK_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Indicate whether the Security Manager should request distribution
     * of the master's Long Term Key during bonding.
     *
     * During bonding, the peer devices negotiate which keys to distribute to each
     * other. This function allows the application to decide whether the LTK, EDIV
     * and Rand should be distributed by the master of the connection. It can be used
     * when the local device is the master or when it is the slave.
     *
     * The default is for the Security Manager to not request distribution of the
     * master key, as typically this key is only required if the master and slave
     * devices are likely to swap roles but wish to retain the existing bond.
     */
    SM_DISTRIBUTE_MASTER_LTK_REQ,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates the Pairing Feature Exchange has completed,
     *  successfully or otherwise.  See Volume 3 Part H section 2.3
     *  of the Bluetooth v4.1 specification for more information on pairing.
     *  see \ref SM_SIMPLE_PAIRING_COMPLETE_IND_T
     */
    SM_SIMPLE_PAIRING_COMPLETE_IND = SM_PRIM_UP,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    SM_SECURITY_LEVEL_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Currently unimplemented */
    SM_CSRK_COUNTER_CHANGE_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Contains the keys and associated security information
     *  used on a connection that has completed Short Term Key
     *  Generation or Transport Specific Key Distribution.  See Volume
     *  3 Part H section 2.1 of the Bluetooth v4.1 specification.
     *  see \ref SM_KEYS_IND_T
     */
    SM_KEYS_IND,
    
    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates that the Security Manager cannot find security
     *  keys for the host in its persistent store. Application responds
     *  with either a SM_KEYSET_T or NULL pointer in SMKeyRequestResponse()
     *  see \ref SM_KEY_REQUEST_IND_T
     */
    SM_KEY_REQUEST_IND,
    
    /*! \ingroup related_to_indication_messages */
    /*! \brief Currently unimplemented
     */
    SM_UNSTORED_KEY_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates that the Security Manager is in pairing mode, and need
     * the application to display the pass key which the peer has to enter.
     * Application shall respond with SMPasskeyDisplayed() when the key has
     * been displayed.
     *  see \ref SM_PASSKEY_DISPLAY_IND_T
     */
    SM_PASSKEY_DISPLAY_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates that the Security Manager is in pairing mode, and need
     * the user to enter the pass key displayed by the peer. The Application shall
     * respond with SMPasskeyInput() containing the entered pass key or SMPasskeyInputNeg()
     * to abort the pairing process.
     *  see \ref SM_PASSKEY_INPUT_IND_T
     */
    SM_PASSKEY_INPUT_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Reserved for future use
     */
    SM_PASSKEY_COMPARE_IND,

    SMx_DEBUG, /* TODO app build does not like this */

    /*! \ingroup related_to_indication_messages */
    /*! \brief  Indicates that the Security Manager has received a encryption
     *          request from the peer. The peer want to encrypt the link with
     *          the key corresponding with the supplied diversifier. The
     *          Application can either approve the use of the diversifier or
     *          revoke it. This is the only way for the application to inform
     *          the peer, that it has revoked the key and removed the bond. The
     *          Application shall treat this event atomic and respond with
     *          SMDivApproval() immediately without any context switch or other
     *          GATT calls.
     *          see \ref SM_DIV_APPROVE_IND_T
     */
    SM_DIV_APPROVE_IND,

    /*! \brief Indicates that a pairing request has been received from the peer
     * device, allowing the application to either authorise or reject the request.
     * This can be used, for example, to prevent pairing unless the user has
     * pressed a "Pairing" button on the local device.
     * See \ref SM_PAIRING_AUTH_IND_T
     */
    SM_PAIRING_AUTH_IND,

    /*! \brief Unused */
    SM_FEATURES_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief  Confirms and returns encrypted data */
    SM_ENCRYPT_RAW_AES_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief  Indicates that the Security Manager has received a encryption
     *          request from the peer. The application has indicated that it
     *          wants to manage some Long Term Keys independently of Security
     *          Manager pairing. If the application has an LTK for the current
     *          connection then it should call SMLongTermKeyRsp() and provide
     *          the key. If it does not have an LTK it should call
     *          SMLongTermKeyRsp() with appropriate status to pass handling of
     *          encryption back to the Security Manager (in which case SM will
     *          recreate the LTK using the EDIV and RAND, and optionally may
     *          then ask the application for DIV approval).
     *
     *          The Application shall treat this event as atomic and respond with
     *          SMDivApproval() immediately without any context switch or other
     *          API calls.
     *
     *          See also \ref SM_LONG_TERM_KEY_IND_T
     */
    SM_LONG_TERM_KEY_IND,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief  Returns device's own IRK */
    SM_PRIVACY_GET_OWN_IRK_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates that the device has lost the bond */
    SM_LOST_BOND_IND,

    /*! \brief Unused */
    SM_DISTRIBUTE_MASTER_LTK_CFM,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsReadWhiteListMaxSize() */
    LS_READ_WHITELIST_SIZE_REQ = LS_PRIM_DOWN,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsResetWhiteList() */
    LS_RESET_WHITELIST_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsAddWhiteListDevice() */
    LS_ADD_DEVICE_TO_WHITELIST_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsDeleteWhiteListDevice() */
    LS_DELETE_WHITELIST_DEVICE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsReadRemoteVersionInformation() */
    LS_READ_REMOTE_VERSION_INFO_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsReadRssi() */
    LS_READ_RSSI_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsReadTransmitPowerLevel() */
    LS_READ_TRANSMIT_POWER_LEVEL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsReadRemoteUsedFeatures() */
    LS_READ_REMOTE_USED_FEATURES_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Set the connection parameters for new connections.
	 Devices operating as a BLE master (Central devices, typically) will use
     * these parameters for all subsequent connections. This command does not 
     * change existing connections - use \ref LS_CONNECTION_PARAM_UPDATE_REQ 
     * to do that.
     * \note This function is not used on slave (peripheral) devices 
	 * Corresponds to LsSetNewConnectionParamReq() */
    LS_SET_NEW_CONNECTION_PARAM_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request an update to the connection parameters for an existing 
     * connection.
	 *  This command is valid for both master and slave devices. Corresponds to LsConnectionParamUpdateReq() */
    LS_CONNECTION_PARAM_UPDATE_REQ,

    /* ! \brief Unused: GAP corresponding to non-existent (for now) RSP message */
    LS_UNUSED_CONNECTION_PARAM_UPDATE_RSP,

    /*! \brief Unused: corresponds to LsConnectionUpdateSignalingRsp() */
    LS_CONNECTION_UPDATE_SIGNALLING_RSP,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsStoreAdvScanData() */
    LS_STORE_ADV_SCAN_DATA_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Start /stop advertising. Remote devices cannot connect to \muEnergy Command Interface unless it is
     * advertising.
     * \note This command is applicable only when the local device is a 
     * Peripheral device. Corresponds to LsStartStopAdvertise() */
    LS_ADVERTISE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: Start/stop scanning for other devices in the vicinity. This command is
     * appropriate only when this device has the Central role.
     * \note Enabling scanning at a high rate can result in a lot of messages 
     * being sent to the host application, since \muEnergy Command Interface does not perform
     * any filtering on the results. Corresponds to LsStartStopScan() */
    LS_SCAN_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetMode() */
    LS_GAP_SET_MODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetRandomAddress() */
    LS_GAP_SET_RANDOM_ADDR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetAdvAddress() */
    LS_GAP_SET_ADV_ADDR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetScanInterval() */
    LS_GAP_SET_SCAN_INTERVAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetAdvInterval() */
    LS_GAP_SET_ADV_INTERVAL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetScanType() */
    LS_GAP_SET_SCAN_TYPE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapSetAdvChanMask() */
    LS_GAP_SET_ADV_CHAN_MASK_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to GapGetRandomAddress() */
    LS_GAP_GET_RANDOM_ADDR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsRadioEventNotification() */
    LS_RADIO_EVENT_NOTIFICATION_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsSetTransmitPowerLevel() */
    LS_SET_TRANSMIT_POWER_LEVEL_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsHoldTxUntilRx() */
    LS_HOLD_TX_UNTIL_RX_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Unused: corresponds to LsRxTimingReport() */
    LS_RX_TIMING_REPORT_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief get the channel map */
    GAP_GET_CONNECTION_CHANNEL_MAP_REQ = 2685,

    /*! \ingroup related_to_request_messages */
    /*! \brief set the channel map */
    GAP_SET_CONNECTION_CHANNEL_MAP_REQ = 2686,

    /*! \ingroup related_to_request_messages */
    /*! \brief set the TGAP(conn_param_timeout) */
    LS_GAP_SET_TGAP_CONN_PARAM_TIMEOUT_REQ = LS2_PRIM_DOWN,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_READ_WHITELIST_SIZE_CFM = LS_PRIM_UP,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_RESET_WHITELIST_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_ADD_DEVICE_TO_WHITELIST_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_DELETE_WHITELIST_DEVICE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_READ_REMOTE_VERSION_INFO_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_READ_RSSI_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_READ_TRANSMIT_POWER_LEVEL_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_READ_REMOTE_USED_FEATURES_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_SET_NEW_CONNECTION_PARAM_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Response to LsConnectionParamUpdateReq()
     *  see \ref LS_CONNECTION_PARAM_UPDATE_CFM_T
     */
    LS_CONNECTION_PARAM_UPDATE_CFM,         /* (was LM_EV_CONNECTION_PARAM_UPDATE_CFM) */

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates remotely-triggered Connection Update has completed
     *  see \ref LS_CONNECTION_PARAM_UPDATE_IND_T
     */
    LS_CONNECTION_PARAM_UPDATE_IND,         /* (was LM_EV_CONNECTION_PARAM_UPDATE_IND) */

    /*! \ingroup related_to_indication_messages */
    /*! \brief L2CAP signal requesting a Connection Update has been
     * received. The application must accept or reject it by calling
     * LsConnectionUpdateSignalingRsp() appropriately.
     *  see \ref LS_CONNECTION_UPDATE_SIGNALLING_IND_T
     */
    LS_CONNECTION_UPDATE_SIGNALLING_IND,    /* (was LM_EV_CONNECTION_UPDATE_SIGNALING_IND) */

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_STORE_ADV_SCAN_DATA_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_ADVERTISE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_SCAN_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_MODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_RANDOM_ADDR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_ADV_ADDR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_SCAN_INTERVAL_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_ADV_INTERVAL_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_SCAN_TYPE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_SET_ADV_CHAN_MASK_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_GAP_GET_RANDOM_ADDR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Unused */
    LS_RADIO_EVENT_NOTIFICATION_CFM,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Optional radio activity event (see LsRadioEventNotification()
     *  and \ref LS_RADIO_EVENT_IND_T for further information).
     */
    LS_RADIO_EVENT_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates an advertising or scan report message has been received
     */
    LS_ADVERTISING_REPORT_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates disconnect with peer has completed
     */
    LS_DISCONNECT_COMPLETE_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates an encryption change has been initiated by the peer
     */
    LS_ENCRYPTION_CHANGE_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates key refresh has been initiated by the peer
     */
    LS_ENCRYPTION_KEY_REFRESH_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates number of completed sent to peer
     */
    LS_NUMBER_COMPLETED_PACKETS_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates the response to a remote version info request has been received
     */
    LS_REMOTE_VERSION_INFO_IND,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Indicates connect phase has completed
     */
    LS_CONNECTION_COMPLETE_IND,

    /**** System Events ******************************************************/
    /*! \brief Request the initiation of the background tick indication to the application. */
    SYS_BACKGROUND_TICK_REQ = SYS_APP_PRIM_DOWN,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the local Bluetooth address. */
    SYS_GET_LOCAL_ADDR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the version number of the current application plus library build info. */
    SYS_GET_LOCAL_VERSION_INFO_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of the event mask. */
    SYS_SET_EVENT_MASK_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the TX Power setting. */
    SYS_GET_TX_POWER_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request a given user key setting. */
    SYS_GET_USER_KEY_REQ,

    NOT_USED_SYS_SMFEATURES_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the chip temperature. */
    SYS_GET_TEMPERATURE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of a PIO. */
    SYS_SET_PIO_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current setting of a PIO. */
    SYS_GET_PIO_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of a number of PIOs. */
    SYS_SET_PIOS_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current setting of a number of PIOs. */
    SYS_GET_PIOS_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the direction setting of a PIO. */
    SYS_SET_PIO_DIR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current direction setting of a PIO. */
    SYS_GET_PIO_DIR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the direction setting of a number of PIOs. */
    SYS_SET_PIOS_DIR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current direction setting of a number of PIOs. */
    SYS_GET_PIOS_DIR_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of pull mode for a number of PIOs. */
    SYS_SET_PIOS_PULL_MODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the mode setting of a PIO. */
    SYS_SET_PIO_MODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the mode setting of a number of PIOs. */
    SYS_SET_PIOS_MODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the clock selection of any suitably configured PIO. */
    SYS_SET_PIO_ANA_MON_CLK_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of the event mode for a number of PIOs. */
    SYS_SET_PIOS_EVENT_MODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of the pull mode for the dedicated I2C PIO. */
    SYS_SET_PIO_I2C_PULL_MODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the configuration of 1 of the 4 PWM PIO. */
    SYS_SET_PIO_PWM_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the enable of 1 of the 4 PWM PIO. */
    SYS_SET_PIO_ENABLE_PWM_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of edge capture of all PIOs. */
    SYS_SET_PIO_ENABLE_EDGE_CAPTURE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current setting of edge capture of all PIOs. */
    SYS_GET_PIO_EDGE_CAPTURE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the enable of a given quadrature decoder PIO. */
    SYS_SET_PIO_QUADRATURE_DECODER_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the enable of a number of quadrature decoder PIOs. */
    SYS_SET_PIO_QUADRATURE_DECODERS_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current count of a quadrature decoder PIO. */
    SYS_GET_PIO_QUADRATURE_DECODER_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the validity of the persistent memory. */
    SYS_GET_PERSISTENT_MEM_VALID_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the size of the persistent memory. */
    SYS_GET_PERSISTENT_MEM_SIZE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the read of the persistent memory. */
    SYS_GET_PERSISTENT_MEM_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the write of the persistent memory. */
    SYS_SET_PERSISTENT_MEM_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the erasure of the persistent memory. */
    SYS_RESET_PERSISTENT_MEM_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request cpu warm start. */
    SYS_WARM_RESET_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the application software build id. */
    SYS_GET_BUILD_ID_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the rom software build id. */
    SYS_GET_ROM_BUILD_ID_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current battery voltage. */
    SYS_GET_BATTERY_VOLTAGE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the chip entry a panic state. */
    SYS_PANIC_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the setting of an AIO. */
    SYS_AIO_DRIVE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the current setting of an AIO. */
    SYS_AIO_READ_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request the disable of an AIO. */
    SYS_AIO_OFF_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request an AIO be used for digital output. */
    SYS_AIO_DIG_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request for last application panic code. */
    SYS_READ_APP_PANIC_CODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request for clearing application panic code. */
    SYS_CLEAR_APP_PANIC_CODE_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request for last fw fault ID. */
    SYS_READ_FW_FAULT_ID_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request for clearing fw fault. */
    SYS_CLEAR_FW_FAULT_ID_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request string identifying build */
    SYS_GET_BUILD_NAME_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request UCI version number. */
    SYS_GET_UCI_VERSION_REQ,

    /*! \ingroup related_to_request_messages */
    /*! \brief Request value of the Low battery Threshold CS Key. */
    SYS_GET_BATTERY_LOW_THRESHOLD_REQ,

    /*! \ingroup related_to_indication_messages */
    /*! \brief Optional background tick event (see AppBackgroundTick() for
     *  further information). This event has no parameters.
     */
    SYS_BACKGROUND_TICK_IND = SYS_APP_PRIM_UP,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms background tick request. */
    SYS_BACKGROUND_TICK_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Return the local address information. */
    SYS_GET_LOCAL_ADDR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief return the local version information. */
    SYS_GET_LOCAL_VERSION_INFO_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms event mask has been set. */
    SYS_SET_EVENT_MASK_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the TX Power setting. */
    SYS_GET_TX_POWER_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the given user key setting. */
    SYS_GET_USER_KEY_CFM,

    NOT_USED_SYS_SMFEATURES_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the chip temperature. */
    SYS_GET_TEMPERATURE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO request. */
    SYS_SET_PIO_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get PIO request. */
    SYS_GET_PIO_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIOs request. */
    SYS_SET_PIOS_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get PIOs request. */
    SYS_GET_PIOS_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO direction request. */
    SYS_SET_PIO_DIR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get PIO direction request. */
    SYS_GET_PIO_DIR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIOs direction request. */
    SYS_SET_PIOS_DIR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get PIOs direction request. */
    SYS_GET_PIOS_DIR_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIOs pull mode request. */
    SYS_SET_PIOS_PULL_MODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO mode request. */
    SYS_SET_PIO_MODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIOs mode request. */
    SYS_SET_PIOS_MODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO ana mon clk request. */
    SYS_SET_PIO_ANA_MON_CLK_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIOs event mode request. */
    SYS_SET_PIOS_EVENT_MODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO I2C pull mode request. */
    SYS_SET_PIO_I2C_PULL_MODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO pwm request. */
    SYS_SET_PIO_PWM_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO enable pwm request. */
    SYS_SET_PIO_ENABLE_PWM_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO enable edge capture request. */
    SYS_SET_PIO_ENABLE_EDGE_CAPTURE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get PIO edge capture request. */
    SYS_GET_PIO_EDGE_CAPTURE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO quadrature decoder request. */
    SYS_SET_PIO_QUADRATURE_DECODER_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set PIO quadrature decoders request. */
    SYS_SET_PIO_QUADRATURE_DECODERS_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get PIO quadrature decoder request. */
    SYS_GET_PIO_QUADRATURE_DECODER_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get persistent memory valid request. */
    SYS_GET_PERSISTENT_MEM_VALID_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get persistent memory size request. */
    SYS_GET_PERSISTENT_MEM_SIZE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get persistent memory request. */
    SYS_GET_PERSISTENT_MEM_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the set persistent memory request. */
    SYS_SET_PERSISTENT_MEM_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the reset persistent memory request. */
    SYS_RESET_PERSISTENT_MEM_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get build id request. */
    SYS_GET_BUILD_ID_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get rom build id request. */
    SYS_GET_ROM_BUILD_ID_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the get battery voltage request. */
    SYS_GET_BATTERY_VOLTAGE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the AIO drive request. */
    SYS_AIO_DRIVE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the AIO read request. */
    SYS_AIO_READ_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the AIO off request. */
    SYS_AIO_OFF_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms the AIO dig request. */
    SYS_AIO_DIG_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms request to get application panic code. */
    SYS_READ_APP_PANIC_CODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms clearing of application panic code. */
    SYS_CLEAR_APP_PANIC_CODE_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms request to retrtieve fw fault id. */
    SYS_READ_FW_FAULT_ID_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms clearing of fw fault id. */
    SYS_CLEAR_FW_FAULT_ID_CFM,

    /*! \ingroup related_to_request_messages */
    /*! \brief Confirms string identifying build request */
    SYS_GET_BUILD_NAME_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms UCI version number. */
    SYS_GET_UCI_VERSION_CFM,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms get value of battery low threshold CS Key request.*/
    SYS_GET_BATTERY_LOW_THRESHOLD_CFM,

    /*! \ingroup related_to_request_messages */
    /*! \brief Requests that random channel map are enabled/disabled. */
    SYS_TEST_CHANNEL_MAP_REQ = SYS_APP_PRIM_TEST,

    /*! \ingroup related_to_confirmation_messages */
    /*! \brief Confirms that random channel map request. */
    SYS_TEST_CHANNEL_MAP_CFM,
    
} lm_event_code;

/*! \brief  All LM_EVENT_T events have this common event header */
typedef struct 
{
    lm_event_code       event_code; /*!< Identifies the type of event
                                     *   and hence the corresponding member
                                     *   of LM_EVENT_T union */
} LM_EVENT_COMMON_T;

#endif /* __CORE_EVENT_TYPES_H__ */
