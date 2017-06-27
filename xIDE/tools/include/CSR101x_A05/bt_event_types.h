/*! \file bt_event_types.h
 *
 *  \brief Type definitions for the handling of events that are
 *      related to RF activity.
 *
 *  Includes definitions of HCI, GATT, L2CAP, etc event IDs, and
 *  the structures that are used to carry the event data.
 *
 * Copyright (c) 2010 - 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __BT_EVENT_TYPES_H__
#define __BT_EVENT_TYPES_H__

#include "core_event_types.h"
#include "hci_event_types.h"
#include "ls_types.h"
#include "ls_err.h"
#include "gap_types.h"
#include "time.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief Type of UUID specified */
typedef enum
{
    GATT_UUID_NONE = 0,    /*!< \brief No UUID */
    GATT_UUID16,           /*!< \brief 16-bit UUID */
    GATT_UUID128           /*!< \brief 128-bit UUID */
} GATT_UUID_T;


/*! \brief L2CAP Connection Type */
typedef enum
{
    /*! \brief Connect as Master */
    L2CA_CONNECTION_LE_MASTER_DIRECTED = 0,
    /*! \brief Connect as Master using whitelist */
    L2CA_CONNECTION_LE_MASTER_WHITELIST,
    /*! \brief Start directed adverts */
    L2CA_CONNECTION_LE_SLAVE_DIRECTED,
    /*! \brief Use whitelist for undirected adverts */
    L2CA_CONNECTION_LE_SLAVE_WHITELIST,
    /*! \brief Start undirected adverts */
    L2CA_CONNECTION_LE_SLAVE_UNDIRECTED,
    /*! \brief Start low duty cycle directed adverts */
    L2CA_CONNECTION_LE_SLAVE_DIRECTED_LDC
} L2CA_CONNECTION_T;

/*! \brief L2CAP Bluetooth Address Type */
typedef enum
{
    L2CA_PUBLIC_ADDR_TYPE = 0,        /*!< \brief Public address type used */
    L2CA_RANDOM_ADDR_TYPE             /*!< \brief Random address type used */
} L2CA_ADDR_TYPE_T;

/* Mask for Connection data. Bits 0 to 3 of 16-bit flag are used. 2 bits
 * would have been sufficient, but reserving 2 additional bits for future
 * extensions.
 */
#define L2CAP_CONN_TYPE_ENUM_MASK 0x000f

/* Bit offset for connection type data */
#define L2CA_CONN_TYPE_OFFSET 0

/* Mask for own address type data. */
#define L2CAP_OWN_ADDR_TYPE_ENUM_MASK 0x0010
/* Bit offset for own address type data */
#define L2CA_OWN_ADDR_TYPE_OFFSET 4

/* Mask for peer address type data. */
#define L2CAP_PEER_ADDR_TYPE_ENUM_MASK 0x0020
/* Bit offset for peer address type data */
#define L2CA_PEER_ADDR_TYPE_OFFSET 5

/* Construction macros */
#define L2CAP_CONNECTION_FLAG_ENUM(value) \
                        ((uint16)(value) << L2CA_CONN_TYPE_OFFSET)
#define L2CAP_OWN_ADDR_TYPE_FLAG_ENUM(value) \
                        ((uint16)(value) << L2CA_OWN_ADDR_TYPE_OFFSET)
#define L2CAP_PEER_ADDR_TYPE_FLAG_ENUM(value) \
                        ((uint16)(value) << L2CA_PEER_ADDR_TYPE_OFFSET)

/*! \brief Flags used by a GATT server or client while mapping fixed
 * channels for a BLE-U connection.  The GATT server or client shall
 * select Connection role (L2CAP_CONNECTION_*) along with Own address
 * type (L2CAP_OWN_ADDR_TYPE_*) while mapping fixed channel (see
 * GattConnectReq()).  Peer address type (L2CAP_PEER_ADDR_TYPE_*) is
 * conveyed through the typed peer address.
 *
 * \name L2CAP Connection Flags
 */
/*! @{ */
typedef uint16 l2ca_conflags_t;
/*! \brief Used in master role to initiate directed connections */
#define L2CAP_CONNECTION_MASTER_DIRECTED \
        L2CAP_CONNECTION_FLAG_ENUM(L2CA_CONNECTION_LE_MASTER_DIRECTED)

/*! \brief Used in master role to initiate connections to devices in Whitelist */
#define L2CAP_CONNECTION_MASTER_WHITELIST \
        L2CAP_CONNECTION_FLAG_ENUM(L2CA_CONNECTION_LE_MASTER_WHITELIST)

/*! \brief Used in slave role to initiate connectable directed advertisements */
#define L2CAP_CONNECTION_SLAVE_DIRECTED \
        L2CAP_CONNECTION_FLAG_ENUM(L2CA_CONNECTION_LE_SLAVE_DIRECTED)

/*! \brief Used in slave role to initiate connectable advertisements
 *     for devices in Whitelist
 */
#define L2CAP_CONNECTION_SLAVE_WHITELIST \
        L2CAP_CONNECTION_FLAG_ENUM(L2CA_CONNECTION_LE_SLAVE_WHITELIST)

/*! \brief Used in slave role to initiate connectable undirected advertisements */
#define L2CAP_CONNECTION_SLAVE_UNDIRECTED \
        L2CAP_CONNECTION_FLAG_ENUM(L2CA_CONNECTION_LE_SLAVE_UNDIRECTED)
    
/*! \brief Used in slave role to initiate connectable low duty cycle directed advertisements */
#define L2CAP_CONNECTION_SLAVE_DIRECTED_LDC \
        L2CAP_CONNECTION_FLAG_ENUM(L2CA_CONNECTION_LE_SLAVE_DIRECTED_LDC)

/*! \brief Used to indicate an own device address type of "public". */
#define L2CAP_OWN_ADDR_TYPE_PUBLIC \
        L2CAP_OWN_ADDR_TYPE_FLAG_ENUM(L2CA_PUBLIC_ADDR_TYPE)

/*! \brief Used to indicate an own device address type of "random". */
#define L2CAP_OWN_ADDR_TYPE_RANDOM \
        L2CAP_OWN_ADDR_TYPE_FLAG_ENUM(L2CA_RANDOM_ADDR_TYPE)

/*! \brief Used to indicate a peer device address type of "public". */
#define L2CAP_PEER_ADDR_TYPE_PUBLIC \
        L2CAP_PEER_ADDR_TYPE_FLAG_ENUM(L2CA_PUBLIC_ADDR_TYPE)

/*! \brief Used to indicate a peer device address type of "public". */
#define L2CAP_PEER_ADDR_TYPE_RANDOM \
        L2CAP_PEER_ADDR_TYPE_FLAG_ENUM(L2CA_RANDOM_ADDR_TYPE)
/*! @} */

/* The maximum number of handle/count pairs that can be written into a single
 * NUMBER_COMPLETED_PACKETS event structure
 */
#define LM_EV_NUMBER_COMPLETED_PACKETS_MAX_HANDLES 3


/*! \brief The lower layer systems use events to communicate with the
 * GATT and SM stacks. Only Observer and Central Applications will use
 * LM_EV_ADVERTISING_REPORT events.
 * Note that there is no direct correlation between lower layer handles
 * and GATT Connection IDs
 *
 * \name HCI Events
 */
/*! @{ */

/*! \brief This event is raised when an advertisement or scan response is
 * received.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;                      /*!< == #LM_EV_ADVERTISING_REPORT */
    HCI_EV_DATA_ULP_ADVERTISING_REPORT_T    data;   /*!< */
    int8                                    rssi;   /*!< RSSI in signed 8-bit format.
                                                         Note that the sign bit is bit 7
                                                         and bits 15..8 should mimic
                                                         bit 7 following sign extension
                                                         applied in dbase_get_rssi(). */
} LM_EV_ADVERTISING_REPORT_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_CONN_COMPLETE_T     data;
} LM_EV_CONN_COMPLETE_T;

/*! \brief This event is raised when a connection is terminated.
 * \n See Volume 2 Part E section 7.7 of the Bluetooth Specification
 * v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_DISCONNECT_COMPLETE_T   data;
} LM_EV_DISCONNECT_COMPLETE_T;

/*! \brief This event is raised when change of encryption mode has been
 * completed. \n See Volume 2 Part E section 7.7 of the Bluetooth Specification
 * v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_ENCRYPTION_CHANGE_T     data;
} LM_EV_ENCRYPTION_CHANGE_T;

/*! \brief This event is raised to indicate the completion of the process of
 * obtaining the version information. \n See Volume 2 Part E section 7.7 of
 *the Bluetooth Specification v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_READ_REMOTE_VER_INFO_COMPLETE_T     data;
} LM_EV_REMOTE_VERSION_INFO_T;

/*! \brief This event is raised when a new connection has has been established.
 * \n See Volume 2 Part E section 7.7 of the Bluetooth Specification v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_ULP_CONNECTION_COMPLETE_T       data;
} LM_EV_CONNECTION_COMPLETE_T;

/*! \brief This event is raised when connection update has been completed
 * by the controller. \n See Volume 2 Part E section 7.7 of the Bluetooth
 * Specification v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T        data;
} LM_EV_CONNECTION_UPDATE_T;

/*! \brief This event is raised when encryption key has been refreshed due
 * to encryption being started or resumed. \n See Volume 2 Part E section 7.7 of the Bluetooth
 * Specification v4.1 for details.
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_ENCRYPTION_KEY_REFRESH_COMPLETE_T       data;
} LM_EV_ENCRYPTION_KEY_REFRESH_T;

/*! \brief This event is raised after the completion of the process of Link Manager
 * obtaining the supported features. \n See Volume 2 Part E section 7.7 of the Bluetooth
 * Specification v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_ULP_READ_REMOTE_USED_FEATURES_COMPLETE_T    data;
} LM_EV_REMOTE_USED_FEATURES_T;

/*! \brief The LE Long Term Key Request event indicates that the master device is
 * attempting to encrypt or re-encrypt the link and is requesting the Long Term
 * Key from the Host. \n See Volume 2 Part E section 7.7 of the Bluetooth
 * Specification v4.1 for details.
 */
typedef struct 
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_ULP_LONG_TERM_KEY_REQUESTED_T       data;
} LM_EV_LONG_TERM_KEY_REQUESTED_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_DATA_BUFFER_OVERFLOW_T       data;
} LM_EV_BUFFER_OVERFLOW_T;

/*! \brief This event is raised when there is a hardware failure. \n See 
 * Volume 2 Part E section 7.7 of the Bluetooth Specification v4.1 for details.
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_HARDWARE_ERROR_T     data;
} LM_EV_HARDWARE_ERROR_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    uint16              num_handles;
    HANDLE_COMPLETE_T   num_completed_pkts[LM_EV_NUMBER_COMPLETED_PACKETS_MAX_HANDLES];
} LM_EV_NUMBER_COMPLETED_PACKETS_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    con_handle_t        handle;
    uint8               num_bytes;
} LM_EV_ACL_DATA_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    uint8               length;
    HCI_EV_DATA_COMMAND_COMPLETE_T  data;
} LM_EV_COMMAND_COMPLETE_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_COMMAND_STATUS_T    data;
} LM_EV_COMMAND_STATUS_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    HCI_EV_DATA_MNFR_EXTENSION_T    data;
} LM_EV_MANUFACTURER_EXTENSION_T;
/*! @} */


/**** GATT Events ************************************************************/

/*! \brief This event is raised after a call to GattAddDatabaseReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_ADD_DB_CFM */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_ADD_DB_CFM_T;

/*! \brief This event is raised after a call to GattConnectReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_CONNECT_CFM */
    TYPED_BD_ADDR_T     bd_addr;        /*!< address of peer */
    uint16              cid;            /*!< Connection Identifier
                                             if result == #sys_status_success */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_CONNECT_CFM_T;

/*! \brief This event is raised after a call to GattCancelConnectReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_CANCEL_CONNECT_CFM */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_CANCEL_CONNECT_CFM_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_CONNECT_IND */
    TYPED_BD_ADDR_T     bd_addr;        /*!< address of peer */
    uint16              cid;            /*!< Connection Identifier */
} GATT_CONNECT_IND_T;

/*! \brief This event is raised after a call to GattDisconnectReq(). There
 * may be a race with the peer disconnecting so it is recommended that you
 * handle this event identically to #GATT_DISCONNECT_IND */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISCONNECT_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_DISCONNECT_CFM_T;

/*! \brief This event is raised after the peer disconnects. There may be a
 * race with a call to GattDisconnectReq() so it is recommended that you
 * handle this event identically to #GATT_DISCONNECT_IND */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISCONNECT_IND */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          reason;         /*!< ls_err_oetc_user or error */
} GATT_DISCONNECT_IND_T;

/*! \brief This event is raised after a call to GattExchangeMtuReq() when
 * the MTU Exchange Procedure is finished */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_EXCHANGE_MTU_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
    uint16              mtu;            /*!< Negotiated MTU for the connection */
} GATT_EXCHANGE_MTU_CFM_T;

/*! \brief This event is raised after the peer initiates a MTU Exchange
 * Procedure. It is handled by a call to GattExchangeMtuRsp() */
typedef struct 
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_EXCHANGE_MTU_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              client_mtu;     /*!< Max MTU support by peer */
} GATT_EXCHANGE_MTU_IND_T;

/*! \brief This event is raised after a call to GattDiscoverAllPrimaryServices()
 * to indicate that all discovered services have been reported by
 * #GATT_SERV_INFO_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISC_ALL_PRIM_SERV_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_DISC_ALL_PRIM_SERV_CFM_T;

/*! \brief Zero or more of these events may be raised
 * after a call to GattDiscoverAllPrimaryServices()
 * and before the #GATT_DISC_ALL_PRIM_SERV_CFM event,
 *               or,
 * after a call to GattFindIncludedServices()
 * and before the #GATT_FIND_INCLUDED_SERV_CFM event */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_SERV_INFO_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              strt_handle;    /*!< Start handle for the service */
    uint16              end_handle;     /*!< End handle for the service */
    GATT_UUID_T         uuid_type;      /*!< UUID type - 16-bit or 128-bit */
    uint16              uuid[8];        /*!< Service UUID.
                                         *   Where the UUID type is 16-bit, only
                                         *   the first entry in the array is
                                         *   valid */
} GATT_SERV_INFO_IND_T;

/*! \brief This event is raised after a call to GattDiscoverPrimaryServiceByUuid()
 * to indicate that all discovered services have been reported by
 * #GATT_DISC_PRIM_SERV_BY_UUID_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISC_PRIM_SERV_BY_UUID_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_DISC_PRIM_SERV_BY_UUID_CFM_T;

/*! \brief Zero or more of these events may be raised
 * after a call to GattDiscoverPrimaryServiceByUuid()
 * and before the #GATT_DISC_PRIM_SERV_BY_UUID_CFM event */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISC_PRIM_SERV_BY_UUID_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              strt_handle;    /*!< Start handle for the service */
    uint16              end_handle;     /*!< End handle for the service */
} GATT_DISC_PRIM_SERV_BY_UUID_IND_T;

/*! \brief This event is raised after a call to GattFindIncludedServices() to
 * indicate that all included services have been reported by
 * #GATT_SERV_INFO_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_FIND_INCLUDED_SERV_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_FIND_INCLUDED_SERV_CFM_T;

/*! \brief This event is raised after a call to GattDiscoverServiceChar() to
 * indicate that all service charactistics have been reported by
 * #GATT_CHAR_DECL_INFO_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISC_SERVICE_CHAR_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_DISC_SERVICE_CHAR_CFM_T;

/*! \brief Zero or more of these events may be raised
 * after a call to GattDiscoverServiceChar()
 * and before the #GATT_DISC_SERVICE_CHAR_CFM event */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_CHAR_DECL_INFO_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint8               prop;           /*!< Characteristic properties */
    uint16              val_handle;     /*!< Characteristic value handle */
    GATT_UUID_T         uuid_type;      /*!< UUID type - 16-bit or 128-bit */
    uint16              uuid[8];        /*!< Characteristic UUID.
                                         *   Where the UUID type is 16-bit, only
                                         *   the first entry in the array is
                                         *   valid */
} GATT_CHAR_DECL_INFO_IND_T;

/*! \brief This event is raised after a call to GattDiscoverAllCharDescriptors()
 * to indicate that all service charactistics have been reported by
 * #GATT_CHAR_DESC_INFO_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_DISC_ALL_CHAR_DESC_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_DISC_ALL_CHAR_DESC_CFM_T;

/*! \brief Zero or more of these events may be raised
 * after a call to GattDiscoverAllCharDescriptors()
 * and before the #GATT_DISC_ALL_CHAR_DESC_CFM event */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_CHAR_DESC_INFO_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              desc_handle;    /*!< Characteristic descriptor handle */
    GATT_UUID_T         uuid_type;      /*!< UUID type - 16-bit or 128-bit */
    uint16              uuid[8];        /*!< Characteristic descriptor UUID.
                                         *   Where the UUID type is 16-bit, only
                                         *   the first entry in the array is
                                         *   valid */
} GATT_CHAR_DESC_INFO_IND_T;

/*! \brief This event is raised after a call to GattReadCharValue() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_READ_CHAR_VAL_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
    uint16              size_value;     /*!< Characteristic value size in octets */
    uint8               *value;         /*!< Characteristic value as received */
} GATT_READ_CHAR_VAL_CFM_T;

/*! \brief This event is raised after a call to GattReadCharUsingUuid()
 * to indicate that all characteristic values have been reported by
 * #GATT_UUID_CHAR_VAL_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_READ_CHAR_USING_UUID_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_READ_CHAR_USING_UUID_CFM_T;

/*! \brief Zero or more of these events may be raised
 * after a call to GattReadCharUsingUuid()
 * and before the #GATT_READ_CHAR_USING_UUID_CFM event,
 *                or,
 * after receiving a Characteristic Value Indication
 * or Characteristic Value Notification from the server */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_UUID_CHAR_VAL_IND
                                          or == #GATT_NOT_CHAR_VAL_IND
                                          or == #GATT_IND_CHAR_VAL_IND  */
    uint16              cid;            /*!< Connection Identifier */
    uint16              handle;         /*!< Characteristic value handle */
    uint16              size_value;     /*!< Characteristic value size in octets */
    uint8               *value;         /*!< Characteristic value as received */
} GATT_CHAR_VAL_IND_T;

/*! \brief This event is raised after a call to GattReadLongCharValue()
 * to indicate that all value parts have been reported by
 * GATT_LONG_CHAR_VAL_IND events */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_READ_LONG_CHAR_VAL_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_READ_LONG_CHAR_VAL_CFM_T;

/*! \brief Zero or more of these events may be raised
 * after a call to GattReadLongCharValue()
 * and before the GATT_READ_LONG_CHAR_VAL_CFM event */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_LONG_CHAR_VAL_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              offset;         /*!< Offset of this part in the characteristic value */
    uint16              size_value;     /*!< Characteristic part value size in octets */
    uint8               *value;         /*!< Characteristic part value as received */
} GATT_LONG_CHAR_VAL_IND_T;

/*! \brief This event is raised after a call to GattReadMultipleCharValues() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_READ_MULTI_CHAR_VAL_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
    uint16              size_value;     /*!< Characteristic multi value size in octets */
    uint8               *value;         /*!< Characteristic value for multiple handles */
} GATT_READ_MULTI_CHAR_VAL_CFM_T;

/*! \brief This event is raised after a call to GattWriteCharValueReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_WRITE_CHAR_VAL_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_WRITE_CHAR_VAL_CFM_T;

/*! \brief This event is raised after a call to GattWriteLongCharValueReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_WRITE_LONG_CHAR_VAL_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_WRITE_LONG_CHAR_VAL_CFM_T;

/*! \brief This event is raised after a call to GattCharValueIndication()
 * or GattCharValueNotification() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_CHAR_VAL_IND_CFM
                                          or == #GATT_CHAR_VAL_NOT_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
    uint16              handle;         /*!< attribute handle from corresponding request */
} GATT_CHAR_VAL_IND_CFM_T;

/*! \brief This event requires handling by a call to GattAccessRsp()
 * if the ATT_ACCESS_PERMISSION or ATT_ACCESS_WRITE_COMPLETE flags are
 * set */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #GATT_ACCESS_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              handle;         /*!< The handle of the attribute */
    uint16              flags;          /*!< Flags - combination of
                                         *         #ATT_ACCESS_READ,
                                         *         #ATT_ACCESS_WRITE,
                                         *         #ATT_ACCESS_PERMISSION,
                                         *         #ATT_ACCESS_WRITE_COMPLETE */
    uint16              offset;         /*!< Offset of the first octet to be accessed */
    uint16              size_value;     /*!< Length to be accessed */
    uint8               *value;         /*!< NULL or pointer to content proposed to be written */
} GATT_ACCESS_IND_T;

/*! \brief This event is raised after a call to GattAttPrepareWriteReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #ATT_ATT_PREPARE_WRITE_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
    uint16              handle;         /*!< The handle of the attribute */
    uint16              offset;         /*!< Offset of the first octet to be written */
    uint16              size_value;     /*!< Number of octets to be written */
    uint8               *value;         /*!< NULL or pointer to data to be written */
} GATT_ATT_PREPARE_WRITE_CFM_T;

/*! \brief This event is raised after a call to GattAttExecuteWriteReq() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #ATT_ATT_EXECUTE_WRITE_CFM */
    uint16              cid;            /*!< Connection Identifier */
    sys_status          result;         /*!< sys_status_success or error */
} GATT_ATT_EXECUTE_WRITE_CFM_T;


/**** SM Events **************************************************************/

/*! \brief Security Information block */
typedef struct
{
    uint16              keys_present;   /*!< Bits (1<<#sm_key_type) set indicate valid fields */
    uint16              encryption_key_size;/*!< Negotiated Encryption Key Size
                                             *   \see SMSetMaxEncKeySize()
                                             *   \see SMSetMinEncKeySize() */
    uint16              div;            /*!< Local Encryption DIV */
    uint16              ediv;           /*!< Peer Encryption EDIV */
    uint16              rand[4];        /*!< Peer Encryption RAND */
    uint16              ltk[8];         /*!< Peer Encryption Long Term Key (LTK)
                                         *
                                         *   The LTK is stored as word wise little endian.
                                         *   I.e. ltk[0] = Least Significant Word, ltk[7] = Most Significant Word.
                                         *
                                         *   Example:
                                         *      The ltk 0x000102030405060708090a0b0c0d0e0f (MSB -> LSB) is stored
                                         * \code
                                         *      uint16 ltk[] = {0x0e0f, 0x0c0d, 0x0a0b, 0x0809, 0x0607, 0x0405, 0x0203, 0x0001};
                                         * \endcode
                                         */

    uint16              irk[8];         /*!< Peer Private Address Resolution IRK (Identity Resolving Key)
                                         *
                                         *   The IRK is stored as word wise little endian.
                                         *   I.e. irk[0] = Least Significant Word, irk[7] = Most Significant Word.
                                         *
                                         *   Example:
                                         *      The irk 0x000102030405060708090a0b0c0d0e0f (MSB -> LSB) is stored
                                         *   \code
                                         *      uint16 irk[] = {0x0e0f, 0x0c0d, 0x0a0b, 0x0809, 0x0607, 0x0405, 0x0203, 0x0001};
                                         *   \endcode
                                         */
    uint16              csrk[8];        /*!< reserved */
    uint16              sign_counter;   /*!< reserved */
    TYPED_BD_ADDR_T     id_addr;        /*!< Peer Public/Static Address ID */
} SM_KEYSET_T;

/*! \brief This event requires handling by a call to SMKeyRequestResponse()
 * but will only be raised if the Application links SMKeyRequestResponse()
 * otherwise an implicit 'none' response is assumed */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_KEY_REQUEST_IND */
    TYPED_BD_ADDR_T     remote_addr;    /*!< Current, possibly private, address of peer */
} SM_KEY_REQUEST_IND_T;

/*! \brief This event is raised after Bonding */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_KEYS_IND */
    TYPED_BD_ADDR_T     remote_addr;    /*!< Current, possibly private, address of peer */
    const SM_KEYSET_T  *keys;           /*!< Pointer to security information block */
} SM_KEYS_IND_T;

/*! \brief Unsupported */
typedef SM_KEYS_IND_T SM_UNSTORED_KEY_IND_T;

/*! \brief This event is raised after Link Encryption or Pairing to indicate an error
 * or the new link security level, which may be equal to or higher than the minimum
 * security level set by GapSetMode() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_SIMPLE_PAIRING_COMPLETE_IND */
    TYPED_BD_ADDR_T     bd_addr;        /*!< of the remote device */
    sys_status          status;         /*!< sys_status_success or error */
    uint16              flags;          /*!< reserved */
    gap_mode_security   security_level; /*!< The obtained security level */
} SM_SIMPLE_PAIRING_COMPLETE_IND_T;

/*! \brief Unsupported */
typedef struct
{
    LM_EVENT_COMMON_T   event;
    bool                local_csrk;
    TYPED_BD_ADDR_T     bd_addr;
    uint16              counter;
} SM_CSRK_COUNTER_CHANGE_IND_T;

/*! \brief This event is raised during Passkey Pairing and requires handling
 * by a call to SMPasskeyInput() or SMPasskeyInputNeg() */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_PASSKEY_INPUT_IND */
    TYPED_BD_ADDR_T     bd_addr;        /*!< of the remote device */
} SM_PASSKEY_INPUT_IND_T;

/*! \brief This event is raised during Passkey Pairing and requires handling
 * by a call to SMPasskeyDisplayed() or SMPasskeyInputNeg(). If the Application
 * displays an alternative passkey, SMPasskeyInput() is used instead of
 * SMPasskeyDisplayed() to inform the SM stack */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_PASSKEY_DISPLAY_IND */
    TYPED_BD_ADDR_T     bd_addr;        /*!< of the remote device */
    uint32              passkey;        /*!< Passkey to be displayed */
} SM_PASSKEY_DISPLAY_IND_T;

/*! \brief This event requires handling by a call to SMDivApproval(),
 * but the event will only be raised if the Application links SMDivApproval()
 * otherwise an implicit 'approved' response is assumed
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_DIV_APPROVE_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              div;            /*!< Diversifier referring to an LTK */
} SM_DIV_APPROVE_IND_T;

/*! \brief This event requires handling by a call to SMPairingAuthRsp(),
 * but the event will only be raised if the Application links SMPairingAuthRsp()
 * otherwise an implicit 'authorised' response is assumed.
 *
 * The 'type' parameter indicates which message from the peer requested pairing.
 * For a device operating as GAP Peripheral, 'type' will be 0x01 for Pairing
 * Request. For a device operating as a GAP Central, 'type' will be 0x0B for
 * Slave Security Request.
 *
 * The 'data' parameter is an internal firmware handle for the ongoing SM
 * procedure. It must be returned in the call to SMPairingAuthRsp().
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_PAIRING_AUTH_IND */
    uint8               type;           /*!< Type of pairing request */
    void*               data;           /*!< Internal handle for SM procedure */
} SM_PAIRING_AUTH_IND_T;

/*! \brief This event requires handling by a call to SMLongTermKeyRsp(),
 * but the event will only be raised if the Application links SMLongTermKeyRsp()
 * otherwise Long Term Key generation will be managed by the firmware Security
 * Manager as normal.
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_DIV_APPROVE_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              ediv;           /*!< Encrypted Diversifier from master */
    uint16              rand[4];        /*!< Random Number from master */
} SM_LONG_TERM_KEY_IND_T;

/*! \brief This is an information only event to application where local device
 * is link master. The event indicates that device has lost previous bond on 
 * link with connection identifier #cid. This event is generated when encryption
 * request is rejected by peer device with error code "PIN OR key Missing".
 * Applications are expected to handle lost bond indications on a link where
 * local device is a slave with help of #SM_KEYS_IND and keys available.
 * 
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #SM_LOST_BOND_IND*/
    uint16              cid;            /*!< Connection Identifier */
} SM_LOST_BOND_IND_T;


/**** LS Events **************************************************************/

/*! \brief This event is raised after a call to LsSetNewConnectionParamReq()
 * when the Connection Parameter Update procedure has finished */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #LS_CONNECTION_PARAM_UPDATE_CFM */
    sys_status          status;         /*!< sys_status_success or error */
    TYPED_BD_ADDR_T     address;        /*!< address of peer */
} LS_CONNECTION_PARAM_UPDATE_CFM_T;

/*! \brief This event is raised on a master after a slave initiates a
 * Connection Parameter Update procedure. It is handled by a call to
 * LsConnectionUpdateSignalingRsp() accepting or rejecting the proposed
 * connection parameters */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #LS_CONNECTION_UPDATE_SIGNALLING_IND */
    uint16              con_handle;     /*!< To be passed into LsConnectionUpdateSignallingRsp() */
    uint16              sig_identifier; /*!< To be passed into LsConnectionUpdateSignallingRsp() */
    uint16              conn_interval_min;  /*!< Proposed minimum connection interval */
    uint16              conn_interval_max;  /*!< Proposed maximum connection interval */
    uint16              slave_latency;      /*!< Proposed slave latency */
    uint16              supervision_timeout;/*!< Proposed supervision timeout */
} LS_CONNECTION_UPDATE_SIGNALLING_IND_T;

/*! \brief This event is raised when the Connection Parameter Update procedure
 * initiated by a peer has finished */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == #LS_CONNECTION_PARAM_UPDATE_IND */
    sys_status          status;         /*!< sys_status_success or error */
    TYPED_BD_ADDR_T     address;        /*!< address of peer */
    uint16              conn_interval;  /*!< Negotiated connection interval */
    uint16              conn_latency;   /*!< Negotiated connection latency */
    uint16              supervision_timeout;/*!< Negotiated supervision timeout */
} LS_CONNECTION_PARAM_UPDATE_IND_T;

/*! \brief This event is raised if the application has requested notification
 *  of specific radio events for a GATT connection.
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == LS_RADIO_EVENT_IND */
    uint16              cid;            /*!< Connection Identifier */
    radio_event         radio;          /*!< The radio event that occurred */
} LS_RADIO_EVENT_IND_T;
 
/*! \brief This event requires handling by a c
 */
typedef struct
{
    LM_EVENT_COMMON_T   event;          /*!< == LS_DATA_RX_TIMING_IND */
    uint16              cid;            /*!< Connection Identifier */
    uint16              tx_duration;    /*!< Duration of most recent TX preceding RX packet */
    time48              tx_event_offset;    /*!< Offset of most recent TX within Connection Event */
    time48              tx_transmit_offset; /*!< Offset of most recent TX from first TX opportunity */
} LS_DATA_RX_TIMING_IND_T;



/**** Union of all Events ****************************************************/

/*! \brief This union is the common datatype for all events received by
 * AppProcessLmEvent(). Events are identified by the value of the event
 * member, usually processed via a switch() statement. The datatype documentation
 * for each event describes the correlating value of the event member. */
typedef union
{
    LM_EVENT_COMMON_T                       event;

    /* HCI Events */
    LM_EV_DISCONNECT_COMPLETE_T             disconnect_complete;
    LM_EV_ENCRYPTION_CHANGE_T               enc_change;
    LM_EV_REMOTE_VERSION_INFO_T             remote_version_info;
    LM_EV_ENCRYPTION_KEY_REFRESH_T          enc_key_refresh;
    LM_EV_CONNECTION_COMPLETE_T             connection_complete;
    LM_EV_ADVERTISING_REPORT_T              adv_report;
    LM_EV_CONNECTION_UPDATE_T               connection_update;
    LM_EV_REMOTE_USED_FEATURES_T            remote_features;
    LM_EV_LONG_TERM_KEY_REQUESTED_T         long_term_key;
    LM_EV_BUFFER_OVERFLOW_T                 buffer_overflow;
    LM_EV_HARDWARE_ERROR_T                  hardware_error;
    LM_EV_NUMBER_COMPLETED_PACKETS_T        number_completed_packets;

    LM_EV_COMMAND_STATUS_T                  command_status;
    LM_EV_COMMAND_COMPLETE_T                command_complete;
    LM_EV_MANUFACTURER_EXTENSION_T          manufacturer_extension;

    LM_EV_ACL_DATA_T                        acl_data;


    /* GATT Events */
    GATT_ADD_DB_CFM_T                       prim_add_db_cfm;
    GATT_CONNECT_CFM_T                      prim_connect_cfm;
    GATT_CONNECT_IND_T                      prim_connect_ind;
    GATT_CANCEL_CONNECT_CFM_T               prim_cancel_connect_cfm;
    GATT_DISCONNECT_CFM_T                   prim_disconnect_cfm;
    GATT_DISCONNECT_IND_T                   prim_disconnect_ind;
    GATT_EXCHANGE_MTU_CFM_T                 prim_exchange_mtu_cfm;
    GATT_EXCHANGE_MTU_IND_T                 prim_exchange_mtu_ind;
    GATT_DISC_ALL_PRIM_SERV_CFM_T           prim_disc_all_prim_serv_cfm;
    GATT_SERV_INFO_IND_T                    prim_serv_info_ind;
    GATT_DISC_PRIM_SERV_BY_UUID_CFM_T       prim_disc_prim_serv_by_uuid_cfm;
    GATT_DISC_PRIM_SERV_BY_UUID_IND_T       prim_disc_prim_serv_by_uuid_ind;
    GATT_FIND_INCLUDED_SERV_CFM_T           prim_find_included_serv_cfm;
    GATT_CHAR_DECL_INFO_IND_T               prim_char_decl_info_ind;
    GATT_DISC_SERVICE_CHAR_CFM_T            prim_disc_service_char_cfm;
    GATT_CHAR_DESC_INFO_IND_T               prim_char_desc_info_ind;
    GATT_DISC_ALL_CHAR_DESC_CFM_T           prim_disc_all_char_desc_cfm;
    GATT_READ_CHAR_VAL_CFM_T                prim_read_char_val_cfm;
    GATT_CHAR_VAL_IND_T                     prim_char_val_ind;
    GATT_READ_LONG_CHAR_VAL_CFM_T           prim_read_long_char_val_cfm;
    GATT_LONG_CHAR_VAL_IND_T                prim_long_char_val_ind;
    GATT_READ_CHAR_USING_UUID_CFM_T         prim_read_char_using_uuid_cfm;
    GATT_READ_MULTI_CHAR_VAL_CFM_T          prim_read_multi_char_val_cfm;
    GATT_WRITE_CHAR_VAL_CFM_T               prim_write_char_val_cfm;
    GATT_WRITE_LONG_CHAR_VAL_CFM_T          prim_write_long_char_val_cfm;
    GATT_CHAR_VAL_IND_CFM_T                 prim_val_ind_cfm;
    GATT_ACCESS_IND_T                       prim_access_ind;
    GATT_ATT_PREPARE_WRITE_CFM_T            prim_att_prepare_write_cfm;
    GATT_ATT_EXECUTE_WRITE_CFM_T            prim_att_execute_write_cfm;

    /* SM Events */
    SM_SIMPLE_PAIRING_COMPLETE_IND_T        sm_simple_pairing_complete_ind;
    SM_CSRK_COUNTER_CHANGE_IND_T            sm_csrk_counter_change_ind;
    SM_KEYS_IND_T                           sm_keys_ind;
    SM_UNSTORED_KEY_IND_T                   sm_unstored_key_ind;
    SM_KEY_REQUEST_IND_T                    sm_key_request_ind;
    SM_PASSKEY_INPUT_IND_T                  sm_passkey_input_ind;
    SM_PASSKEY_DISPLAY_IND_T                sm_passkey_display_ind;
    SM_DIV_APPROVE_IND_T                    sm_div_approve_ind;
    SM_PAIRING_AUTH_IND_T                   sm_pairing_auth_ind;
    SM_LONG_TERM_KEY_IND_T                  sm_long_term_key_ind;
    SM_LOST_BOND_IND_T                      sm_lost_bond_ind;

    /* LS Events */
    LS_CONNECTION_UPDATE_SIGNALLING_IND_T   ls_conn_update_signal_ind;
    LS_CONNECTION_PARAM_UPDATE_CFM_T        ls_conn_param_update_cfm;
    LS_CONNECTION_PARAM_UPDATE_IND_T        ls_conn_param_update_ind;
    LS_RADIO_EVENT_IND_T                    ls_radio_event_ind;
    LS_DATA_RX_TIMING_IND_T                 ls_data_rx_timing_ind;

} LM_EVENT_T;

#endif /* __BT_EVENT_TYPES_H__ */
