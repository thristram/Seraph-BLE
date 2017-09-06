/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
 /*! \file cm_types.h
 *  \brief Defines the connection manager public data structures
 *
 */

#ifndef __CM_TYPES_H__
#define __CM_TYPES_H__

/*! \addtogroup CMTypes
 * @{
 */

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <bluetooth.h>
#include <bt_event_types.h>
#include <time.h>
#ifndef CSR101x_A05
#include <hci_msg.h>
#endif

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*! \brief Maximum timers required by connection manager
 *  1. CMInit() unblock timer
 *  2. Adveritisement Timer
 *  3. Connection Attempt Timer
 */
#if defined (SERVER)
#define CM_MAX_TIMERS                           (5)
#else
#define CM_MAX_TIMERS                           (4)
#endif /* (SERVER) */

/*! \brief Invalid connection handle */
#define CM_GATT_INVALID_UCID                    (0xFFFF)

/*! \brief Invalid attribute handle */
#define CM_INVALID_ATT_HANDLE                   (0x0000)

/*! \brief Invalid bond ID */
#define CM_INVALID_BOND_ID                      (0xFFFF)

/*! \brief Invalid device id */
#define CM_INVALID_DEVICE_ID                    (0xFFFF)

/*! \brief  Maximum size of the Identity Root Key */
#define CM_MAX_WORDS_IRK                        (8)

/*! \brief  Maximum size of the Long Term Key */
#define CM_MAX_WORDS_LTK                        (8)

/*! \brief Maximum advertisement types */
#define CM_MAX_ADV_TYPES                        (4)

/*! \brief Size of each connection information */
#define CM_SIZEOF_CONN_INFO                     (0x10)

/*! \brief Size of each bonding information */
#define CM_SIZEOF_BOND_INFO                     (0x2E)
/*============================================================================*
 *  Public GATT Error Codes
 *============================================================================*/

/*! \brief  Improper configuration */
#define gatt_status_desc_improper_config    (STATUS_GROUP_GATT+ 0xFD)

/*! \brief  Same GATT procedure is in progress */
#define gatt_status_proc_in_progress        (STATUS_GROUP_GATT+ 0xFE)

/*! \brief  Value is out of the supported range */
#define gatt_status_att_val_oor             (STATUS_GROUP_GATT+ 0xFF)

/*! \brief Write request rejected */
#define gatt_access_not_permitted           (0x80| gatt_status_app_mask)

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/*! \brief Device id type */
typedef uint16 device_handle_id;

/*! \brief Bond id type */
typedef uint16 bond_handle_id;

/*! \brief Advertisement type */
#ifndef CSR101x_A05
typedef hci_advertising_report_t                        RAW_ADVERT_REPORT_IND_T;        
#else
typedef LM_EV_ADVERTISING_REPORT_T                      RAW_ADVERT_REPORT_IND_T;
#endif

/*! \brief HCI ULP Advertising Report event type */
typedef HCI_EV_DATA_ULP_ADVERTISING_REPORT_T            CM_EV_DATA_ULP_ADVERTISING_REPORT_T;

/*! \brief The client configuration bit field values */
typedef enum
{
    gatt_client_config_none         = 0x0000, /*!< \brief  None */
    gatt_client_config_notification = 0x0001, /*!< \brief  Notifications */
    gatt_client_config_indication   = 0x0002, /*!< \brief  Indications */
    gatt_client_config_reserved     = 0xFFF4  /*!< \brief  Reserved */

} gatt_client_config;

/*! \brief Connection Manager status codes */
typedef enum
{
    cm_status_success,                  /*!< \brief  Success */

    cm_status_failed,                   /*!< \brief  Failure */

    cm_status_busy,                     /*!< \brief  Busy */

    cm_status_scanning_in_progress,     /*!< \brief  Failure of the operation as scanning is in progress */

    cm_status_advertising_in_progress,  /*!< \brief  Failure of the operation as advertisement is in progress */

    cm_status_discovery_in_progress,    /*!< \brief  Failure of the operation as discovery is in progress */

    cm_status_disc_primary_serv_failed, /*!< \brief  Failure of discover primary services */

    cm_status_mandatory_serv_not_found, /*!< \brief  Failure of discovery procedure as mandatory service is not found */

    cm_status_discover_char_fail,       /*!< \brief  Failure of discovery as characteristic or descriptor disc failed */

}cm_status_code;

/*! \brief Connection Error Codes */
typedef enum
{
    cm_conn_res_success,            /*!< \brief  Connection Success */

    cm_conn_res_cancelled,          /*!< \brief  Connection Cancelled */

    cm_conn_res_failed,             /*!< \brief  Connection failed */

    cm_disconn_res_success          /*!< \brief  Disconnection success */

}cm_conn_result;

/*! \brief Bonding Error Codes */
typedef enum
{
    cm_bond_res_success,            /*!< \brief  Bonding success */

    cm_unbond_res_success,          /*!< \brief  Unbonding success */

    cm_bond_res_busy,               /*!< \brief  Bonding in progress */

    cm_bond_res_failed,             /*!< \brief  Bonding failed */

    cm_bond_res_rep_attempt_failed  /*!< \brief  Bonding repeated attempts failed */

}cm_bond_result;

/*! \brief Connection Manager advertisement states */
typedef enum
{
    cm_advert_state_idle,                      /*!< \brief  CM advertisement state idle */

    cm_advert_state_directed_advertising,      /*!< \brief  CM advertisement state directed advertising */

    cm_advert_state_fast_advertising,          /*!< \brief  CM advertisement state fast advertising */

    cm_advert_state_slow_advertising,          /*!< \brief  CM advertisement state slow advertising */

}cm_advert_state;

/*! \brief Connection Manager scan states */
typedef enum
{
    cm_scan_state_idle,                     /*!< \brief  CM scan state idle */

    cm_scan_state_scanning,                 /*!< \brief  CM scan state scanning */

}cm_scan_state;

/*! \brief Connection Manager sca states */
typedef enum
{
    cm_connect_state_idle,                  /*!< \brief  CM connect state idle */

    cm_connect_state_connecting,            /*!< \brief  CM connect state connecting */

    cm_connect_state_cancel_connecting,     /*!< \brief  CM connect state cancel connecting */

}cm_connect_state;

/*! \brief Bonding states */
typedef enum
{
    cm_dev_unbonded,    /*!< \brief  Unbonded */

    cm_dev_bonding,     /*!< \brief  Bonding */

    cm_dev_bonded,      /*!< \brief  Bonded */

    cm_dev_unbonding,   /*!< \brief  Unbonding */

}cm_dev_bond_state;

/*! \brief Device states */
typedef enum
{
    dev_state_init,             /*!< \brief  Initial State*/

    dev_state_connected,        /*!< \brief  Connected state */

    dev_state_disconnecting,    /*!< \brief  Disconnecting state */

    dev_state_disconnected,     /*!< \brief  Idle or disconnected State */

    dev_state_unknown           /*!< \brief  Unknown State */

} cm_dev_state;

/*! \brief GAP roles */
typedef enum
{
    con_role_peripheral,    /*!< \brief  Peripheral Role */

    con_role_central,       /*!< \brief  Central Role */

    con_role_invalid        /*!< \brief  Invalid Role */

} cm_peer_con_role;

/*! \brief Notification types */
typedef enum
{
    CM_NOTIF_TYPE_NOTIFICATION, /*!< \brief  GATT Notification */

    CM_NOTIF_TYPE_INDICATION    /*!< \brief  GATT Indication */

}cm_notif_type;

/*! \brief Advertisement flags */
typedef enum
{
    whitelist_supported             = 0x0001,   /*!< \brief  Bit 0 : White list advertisement supported */

    directed_advert_supported       = 0x0002,   /*!< \brief  Bit 1 : Directed advertisement supported */

    ldc_directed_advert_supported   = 0x0004,   /*!< \brief  Bit 2 : Low Duty Cycle Directed advertisement supported */

    fast_advert_supported           = 0x0008,   /*!< \brief  Bit 3 : Fast advertisement supported */

    slow_advert_supported           = 0x0010,   /*!< \brief  Bit 4 : Slow advertisement supported */

    name_present                    = 0x0020,   /*!< \brief  Bit 5 : Device name present in advert data */

    uuid_present                    = 0x0040,   /*!< \brief  Bit 6 : Device UUID present in advert data */

    appearance_present              = 0x0080,   /*!< \brief  Bit 7 : Device appearance present in advert data */

    tx_power_present                = 0x0100,   /*!< \brief  Bit 8 : Device Tx power present in advert data */

    local_address_random            = 0x0200,    /*!< \brief  Bit 9 : Local Bluetooth Address is random */

    manufacturer_data_present       = 0x0400,    /*!< \brief  Bit 10 : Manufacture specific data present */

    name_present_scan_rsp           = 0x0800,   /*!< \brief  Bit 5 : Device name present in scan response data */

    non_conn_advert_supported       = 0x1000,   /*!< \brief  Bit   : Non Connectable Adverts supported */
    
    uuid_128_present                = 0x2000,   /*!< \brief Device 128-bit UUID present in advert data */
}cm_adv_flags;

/*! \brief Connection manager events */
typedef enum
{
    CM_INIT_CFM,            /*!< \brief  CM Init confirmation */
            
    CM_RAW_ADV_REPORT_IND,  /*!< \brief  Raw Advertisement report indication  */

    CM_ADV_REPORT_IND,      /*!< \brief  Advertisement report indication  */

    CM_ADVERT_STATE_IND,    /*!< \brief  Advertising state indication */

    CM_SCAN_STATE_IND,      /*!< \brief  Scanning state indication */

    CM_CONNECT_STATE_IND,   /*!< \brief  Connecting state indication */

    CM_CONNECTION_NOTIFY,   /*!< \brief  Connection notification  */

    CM_BONDING_NOTIFY,      /*!< \brief  Bonding notification */

    CM_ENCRYPTION_NOTIFY,   /*!< \brief  Encryption notification */

    CM_BONDING_AUTH_IND,    /*!< \brief  Bonding authorisation indication */

    CM_PASSKEY_INPUT,       /*!< \brief  Passkey input notification */

    CM_RADIO_EVENT_IND,     /*!< \brief  Radio event indication */

#ifndef CSR101x_A05
    CM_EARLY_WAKEUP_IND,    /*!< \brief  Early Wakeup event indication */
#endif

    CM_DATA_RX_TIMING_IND,   /*!< \brief  Data Rx Timimg event indication */

    CM_DISCOVERY_COMPLETE,  /*!< \brief  Discovery complete */

    CM_WRITE_CFM,           /*!< \brief  GATT Write confirmation */

    CM_READ_CFM,            /*!< \brief  GATT Read confirmation */

    CM_NOTIFICATION,        /*!< \brief  GATT notification */

    CM_INDICATION,          /*!< \brief  GATT Indication */

    CM_NOTIFICATION_CFM,    /*!< \brief  GATT Notification confirmation */

    CM_INDICATION_CFM,      /*!< \brief  GATT Indication confirmation */

    CM_READ_ACCESS,         /*!< \brief  GATT read access */

    CM_WRITE_ACCESS,        /*!< \brief  GATT write access */

    CM_SERVER_ACCESSED,     /*!< \brief  server read/write accessed */

    CM_CONNECTION_UPDATE_SIGNALLING_IND,    /*!< \brief  Connection update signalling indication */

    CM_CONNECTION_PARAM_UPDATE_IND,         /*!< \brief  Connection parameter updated indication */

    CM_CONNECTION_PARAM_UPDATE_CFM,         /*!< \brief  Connection parameter update confirmation */

    CM_CONNECTION_UPDATED,                   /*!< \brief  Connection updated */

    CM_NUMBER_OF_COMPLETED_PKTS_IND         /*!< \brief  Number of completed packets from the HCI event. */

}cm_event;

/*! \brief UUID type */
typedef struct
{
    GATT_UUID_T                                 uuid_type;  /*!< \brief  16-bit or 128-bit UUID */

    uint16                                      *uuid;      /*!< \brief  Pointer to UUID */

} CM_UUID_T;

/*! \brief Scan Parameters */
typedef struct
{
    gap_mode_bond                               bond;               /*!< \brief Bonding mode Yes or No */

    gap_mode_security                           security;           /*!< \brief  Security level */

    uint32                                      scan_interval_us;   /*!< \brief  Scan Interval Range: 2.5 msec to 10.24 seconds (N * 0.625 msec) */

    uint32                                      scan_window_us;     /*!< \brief  Scan Window. Range: 2.5 msec to 10240 msec (N * 0.625 msec) */

    ls_scan_type                                scan_type;          /*!< \brief  Scan type */

    whitelist_mode                              use_whitelist;      /*!< \brief  Use Whitelist Yes/No */

    ls_addr_type                                addr_type;          /*!< \brief Address type */

    CM_UUID_T                                   uuid;

} CM_CENTRAL_SCAN_INFO_T;

/*! \brief Central Connection Parameters */
typedef struct
{
    ble_con_params                              *conn_params;       /*!< \brief Connection Parameters */

    uint16                                      con_min_ce_len;     /*!< \brief Minimum Connection Event length */

    uint16                                      con_max_ce_len;     /*!< \brief Maximum Connection Event length */

    uint16                                      con_scan_interval;  /*!< \brief  Scan interval during connection establishment. Range: 2.5 msec to 10240 msec(N * 0.625 msec) */

    uint16                                      con_scan_window ;   /*!< \brief  Scan window during connection establishment. Range: 2.5 msec to 10.24 sec (N * 0.625 msec) */

    TYPED_BD_ADDR_T                             remote_bd_addr;     /*!< \brief Bluetooth Address */

    uint16                                      flags;              /*!< \brief Flags */

} CM_CENTRAL_CONN_INFO_T;

/*! \brief UUID type */
typedef struct
{
    uint8                                       *uuid;      /*!< \brief Pointer to UUID */

    uint16                                      length;     /*!< \brief Length of UUID */

} CM_DEVICE_CM_UUID_T;


/*! \brief Name type */
typedef struct
{
    uint8                                       *name;      /*!< \brief Pointer to Name */

    uint16                                      length;     /*!< \brief Length of Name */

} CM_DEVICE_NAME_T;

/*! \brief Appearance type */
typedef struct
{
    uint8                                       *value;     /*!< \brief Pointer to Appearance Value */

    uint16                                      length;     /*!< \brief Length of Appearance Value */

} CM_DEVICE_APPEARANCE_T;

/*! \brief Manufacturer specific data type */
typedef struct
{
    uint8                                       *value;     /*!< \brief Pointer to Manufacturer specific data */

    uint16                                      length;     /*!< \brief Length of Manufacturer specific data */

} CM_DEVICE_MANUFACTURER_DATA_T;

/*! \brief Advertisement data */
typedef struct
{
     CM_DEVICE_CM_UUID_T                        device_uuid;        /*!< \brief UUID type */

     /*! \brief UUID for 128-bit data */
     CM_DEVICE_CM_UUID_T                        device_uuid_128;

     CM_DEVICE_APPEARANCE_T                     device_appearance;  /*!< \brief Appearance type */

     CM_DEVICE_NAME_T                           device_name;        /*!< \brief Name type */

     CM_DEVICE_MANUFACTURER_DATA_T              manuf_data;         /*!< \brief Manufacturer data type */

     int8                                       tx_power_level;     /*!< \brief Tx Power Level */

} CM_PERIPHERAL_ADVERT_DATA_T;

/*! \brief Advertisement type */
typedef struct
{
    gap_mode_discover                           discover;           /*!< \brief Discoverability mode */

    gap_mode_connect                            connect;            /*!< \brief Advertisement mode */

    cm_adv_flags                                advert_flags;       /*!< \brief Advertisement flags */

    uint32                                      adv_interval_min;   /*!< \brief  Minimum advertising interval. Range: 20 ms to 10.24 sec(N * 0.625 msec) */

    uint32                                      adv_interval_max;   /*!< \brief  Maximum advertising interval. Range: 20 ms to 10.24 sec(N * 0.625 msec) */

    uint32                                      timeout_value;      /*!< \brief Timeout Value */

    TYPED_BD_ADDR_T                             directed_bd_addr;   /*!< \brief Bluetooth Address for directed advertising */

} CM_PERIPHERAL_ADVERT_TYPE_T;

/*! \brief Advertisement parameters */
typedef struct
{
    gap_mode_bond                               bond;                           /*!< \brief Bonding mode Yes/No */

    gap_mode_security                           security;                       /*!< \brief Security level */

    gap_role                                    role;

    CM_PERIPHERAL_ADVERT_DATA_T                 advert_data;                    /*!< \brief Advertisement data */

    CM_PERIPHERAL_ADVERT_TYPE_T                 adv_type[CM_MAX_ADV_TYPES];     /*!< \brief Advertisement type */

    uint16                                      num_adv_types;                  /*!< \brief Number of advertisement types */

} CM_PERIPHERAL_ADVERT_PARAMETERS_T;

/*! \brief Connected Device type */
typedef struct
{
    TYPED_BD_ADDR_T                             bd_addr;    /*!< \brief  Bluetooth Address */

    cm_peer_con_role                            role;       /*!< \brief  GAP role */

    device_handle_id                            device_id;  /*!< \brief  Device id */

}CM_CONNECTED_DEVICE_T;

/*! \brief Bonded Device type */
typedef struct
{
    TYPED_BD_ADDR_T                             bd_addr;    /*!< \brief  Bluetooth Address */

    cm_peer_con_role                            role;       /*!< \brief  GAP role */

    bond_handle_id                              bond_id;    /*!< \brief  Bond id */

    uint16                                      div;        /*!< \brief  Diverisifier  */

    uint16                                      irk[8];     /*!< \brief  IRK */

    uint16                                      ltk[8];     /*!< \brief  LTK */

}CM_BONDED_DEVICE_T;

/*! \brief Bonded Device type */
typedef struct
{
    bool                                        bonded;     /*!< \brief  Boolean flag */

    TYPED_BD_ADDR_T                             bd_addr;    /*!< \brief  Bluetooth Address */

    cm_peer_con_role                            role;       /*!< \brief  GAP role */

    uint16                                      irk[8];     /*!< \brief  IRK */

    uint16                                      ltk[8];     /*!< \brief  LTK */

#ifndef CSR101x_A05
    gap_mode_security                           security_level; /*!< \brief  Security level*/
#endif /* !CSR101x_A05 */
    uint16                                      keys_present;   /*!< \brief  Types of security information present */

    uint16                                      encryption_key_size; /*!< \brief  Negotiated Encryption Key Size */
}CM_FACTORY_BOND_INFO_T;

/*! \brief Radio event Request type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    radio_event                                 event_type;     /*!< \brief  Radio event */

} CM_REQ_RADIO_EVENT_T;

/*! \brief Hold Tx event Request type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    bool                                        mode;           /*!< \brief  Mode */

} CM_REQ_HOLD_TX_EVENT_T;

/*! \brief Rx Timing Report event Request type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    bool                                        mode;           /*!< \brief  Mode */

} CM_REQ_RX_TIMING_REPORT_EVENT_T;

/*! \brief Connection Notification type */
typedef struct
{
    device_handle_id                            device_id;  /*!< \brief  Device id */

    cm_conn_result                              result;     /*!< \brief  Connection result */

    uint8                                       reason;     /*!< \brief  HCI error code */

}CM_CONNECTION_NOTIFY_T;

/*! \brief Bonding Notification type */
typedef struct
{
    /*! \brief  TYPED_BD_ADDR_T of the host to which device is bonded*/
    device_handle_id                            device_id;  /*!< \brief  Device id */

    TYPED_BD_ADDR_T                             bd_addr;    /*!< \brief  Bluetooth Address */

    bond_handle_id                              bond_id;    /*!< \brief  Bond id */

    cm_bond_result                              result;     /*!< \brief  Bonding result */

}CM_BONDING_NOTIFY_T;

/*! \brief Encryption Notification type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device Id */

    bool                                        status;         /*!< \brief  Status */

    bool                                        enc_enable;     /*!< \brief  Encryption state */

}CM_ENCRYPTION_NOTIFY_T;

/*! \brief Discovery Complete type */
typedef struct
{
    device_handle_id                            device_id;          /*!< \brief  Device id */

    cm_status_code                              status;             /*!< \brief  Status */

    uint16                                      instance;           /*!< \brief  Service Instance. Applicable for client services only */

}CM_DISCOVERY_COMPLETE_T;

/*! \brief Write confirmation type */
typedef struct
{
    device_handle_id                            device_id;  /*!< \brief  Device id */

    uint16                                      handle;     /*!< \brief  Characteristic handle */

    uint16                                      instance;   /*!< \brief  Service Instance */

    sys_status                                  status;     /*!< \brief  Status */

}CM_WRITE_CFM_T;

/*! \brief Read confirmation type */
typedef struct
{
    device_handle_id                            device_id;  /*!< \brief  Device id */

    uint16                                      handle;     /*!< \brief  Characteristic handle */

    uint16                                      instance;   /*!< \brief  Service Instance */

    sys_status                                  status;     /*!< \brief  Status */

    uint8                                       *data;      /*!< \brief  Pointer to data */

    uint16                                      length;     /*!< \brief  Length of the data */

}CM_READ_CFM_T;

/*! \brief Notification event type */
typedef struct
{
    device_handle_id                            device_id;  /*!< \brief  Device id */

    uint16                                      handle;     /*!< \brief  Characteristic handle */

    uint16                                      instance;   /*!< \brief  Service Instance */

    uint8                                       *data;      /*!< \brief  pointer to data */

    uint16                                      length;     /*!< \brief  Length of data */

} CM_NOTIFICATION_T;

/*! \brief Indication event type */
typedef CM_NOTIFICATION_T CM_INDICATION_T;

/*! \brief Notification confirmation type */
typedef struct
{
    device_handle_id                            device_id;  /*!< \brief  Device id */

    uint16                                      handle;     /*!< \brief  Characteristic handle */

    sys_status                                  status;     /*!< \brief  Status */

} CM_NOTIFICATION_CFM_T;

/*! \brief Indication confirmation event type */
typedef CM_NOTIFICATION_CFM_T CM_INDICATION_CFM_T;

/*! \brief Read access type */
typedef struct
{
    device_handle_id                            device_id;  /*!< \brief  Device id */

    uint16                                      handle;     /*!< \brief  Characteristic handle */

    uint8                                       *data;      /*!< \brief  Pointer to data */

    uint16                                      length;     /*!< \brief  Length of data */

    uint16                                      offset;     /*!< \brief  Offset */

}CM_READ_ACCESS_T;

/*! \brief Write access type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    uint16                                      handle;         /*!< \brief  Characteristic handle */

    uint8                                       *data;          /*!< \brief  Pointer to data */

    uint16                                      length;         /*!< \brief  Length */

    uint16                                      offset;         /*!< \brief  Offset */

}CM_WRITE_ACCESS_T;

/*! \brief Server read/write accessed */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    uint16                                      handle;         /*!< \brief  Characteristic handle */

    uint8                                       *data;          /*!< \brief  Pointer to data */

    uint16                                      length;         /*!< \brief  Length */

    uint16                                      offset;         /*!< \brief  Offset */

}CM_SERVER_ACCESSED_T;

/*! \brief Access response type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    uint16                                      handle;         /*!< \brief  Characteristic handle */

    uint8                                       *value;         /*!< \brief  Pointer to data */

    uint16                                      size_value;     /*!< \brief  Length of data */

    uint16                                      rc;             /*!< \brief  Status */

} CM_ACCESS_RESPONSE_T;

/*! \brief Notification value type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    uint16                                      handle;         /*!< \brief  Characteristic handle */

    uint8                                       *value;         /*!< \brief  Pointer to data */

    uint16                                      size_value;     /*!< \brief  Length of data */

} CM_VALUE_NOTIFICATION_T;

/*! \brief CM Init confirmation type */
typedef struct
{
    sys_status                                  status;         /*!< \brief  Status */

}CM_INIT_CFM_T;

/*! \brief Advertisement report indication type */
typedef struct
{
    TYPED_BD_ADDR_T                             bd_addr;        /*!< \brief  Bluetooth Address */

    int8                                        rssi;           /*!< \brief  RSSI value */

    char                                        name[21];       /*!< \brief  Device Name */

    uint8                                       type;           /*!< \brief  Type of report advertising report or scan response */

}CM_ADV_REPORT_IND_T;

/*! \brief Raw Advertisement report indication type */
typedef struct
{
    RAW_ADVERT_REPORT_IND_T                     report;    

} CM_RAW_ADV_REPORT_IND_T;

/*! \brief CM advertisement state type */
typedef struct
{
    cm_advert_state                             old_state;      /*!< \brief  Advertisement old state */

    cm_advert_state                             new_state;      /*!< \brief  Advertisement new state */

}CM_ADVERT_STATE_IND_T;

/*! \brief CM scanning state type */
typedef struct
{
    cm_scan_state                               old_state;      /*!< \brief  Scanning old state */

    cm_scan_state                               new_state;      /*!< \brief  Scanning new state */

}CM_SCAN_STATE_IND_T;

/*! \brief CM connection state type */
typedef struct
{
    cm_connect_state                            old_state;      /*!< \brief  Connecting old state */

    cm_connect_state                            new_state;      /*!< \brief  Connecting new state */

}CM_CONNECT_STATE_IND_T;

/*! \brief Radio event indication type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    radio_event                                 radio;          /*!< \brief  Radio event */

#ifndef CSR101x_A05
    uint16                                      delta_ls;       /*!< \brief  LS Word of time delta (us) since last event */

    uint16                                      delta_ms;       /*!< \brief  MS Word of time delta (us) since last event */
#endif /*! \brief  !CSR101x_A05 */

}CM_RADIO_EVENT_IND_T;

#ifndef CSR101x_A05
/*! \brief early wake up indication type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */

    uint16                                      interval;       /*!< Time before radio activation */

}CM_EARLY_WAKEUP_IND_T;
#endif

/*! \brief Data Rx Timimg event indication type */
typedef struct
{
    device_handle_id                            device_id;           /*!< \brief  Device id */

    uint16                                      tx_duration;         /*!< \brief  Duration of most recent TX preceding RX packet */

    time48                                      tx_event_offset;     /*!< \brief  Offset of most recent TX within Connection Event */

    time48                                      tx_transmit_offset;  /*!< \brief  Offset of most recent TX from first TX opportunity */

}CM_DATA_RX_TIMING_IND_T;

/*! \brief Bonding authorisation indication type */
typedef struct
{
    device_handle_id                            device_id;      /*!< \brief  Device id */
#if defined (CSR101x_A05)
    void                                        *data;          /*!< \brief  Pointer to data */
#endif /*! \brief  CSR101x_A05 */

}CM_BONDING_AUTH_IND_T;

/*! \brief Bonding authorisation response type */
typedef struct
{
   uint16                                       device_id;      /*!< \brief  Device id */

   bool                                         accept;         /*!< \brief  Accept or reject */
#if defined (CSR101x_A05)
    void                                        *data;          /*!< \brief  Pointer to data */
#endif /*! \brief  CSR101x_A05 */

}CM_AUTH_RESP_T;

/*! \brief Long Term Key response type */
typedef struct
{
   uint16                                       cid;            /*!< \brief  Connection identifie */

   uint16                                       *ltk;           /*!< \brief  Pointer to an LTK */

   uint8                                        key_size;       /*!< \brief  Encryption key size of LTK */

#ifndef CSR101x_A05
   gap_mode_security                            security_level; /*!< \brief  Pointer to data */
#endif /*! \brief  CSR101x_A05 */

}CM_LTK_RESP_T;

/*! \brief Device connection parameter type */
typedef struct
{
    uint16                                      conn_interval;          /*!< \brief Connection Event length */

    uint16                                      conn_latency;           /*!< \brief Slave latency */

    uint16                                      supervision_timeout;    /*!< \brief Supervision timeout */

}CM_DEV_CONN_PARAM_T;

/*! \brief Characteristic Descriptor type */
typedef struct
{
    GATT_UUID_T                                 uuid_type;          /*!< \brief UUID type */

    uint16                                      uuid;               /*!< \brief UUID */

    uint16                                      desc_handle;        /*!< \brief Descriptor handle */

} CM_DESCRIPTOR_T;

/*! \brief Characteristic type */
typedef struct
{
    uint16                                      uuid_type;          /*!< \brief UUID type */

    uint16                                      *uuid;              /*!< \brief Pointer to UUID */

    uint16                                      value_handle;       /*!< \brief Value handle */

    uint16                                      properties;         /*!< \brief Properties */

    CM_DESCRIPTOR_T                             *descriptors;       /*!< \brief Pointer to descriptors */

    uint16                                      nDescriptors;       /*!< \brief Number of descriptors */

} CM_CHARACTERISTIC_T;

/*! \brief Service instance type */
typedef struct
{
    device_handle_id                            device_id;          /*!< \brief Device id */

    bool                                        bond_id;            /*!< \brief  Bond id */

    uint16                                      start_handle;       /*!< \brief Start handle */

    uint16                                      end_handle;         /*!< \brief End handle */

    CM_CHARACTERISTIC_T                         *characteristics;   /*!< \brief Pointer to characteristics */

    uint16                                      nCharacteristics;   /*!< \brief Number of characteristics */

}CM_SERVICE_INSTANCE;

/*! \brief Service Type */
typedef struct
{
    uint16                                      uuid_type;          /*!< \brief UUID type */

    uint16                                      *uuid;              /*!< \brief Pointer to UUID */

    bool                                        mandatory;          /*!< \brief Mandatory service */

    CM_SERVICE_INSTANCE                        *serviceInstances;   /*!< \brief Pointer to service instances */

    uint16                                      nInstances;         /*!< \brief Number of service instances */

} CM_SERVICE_T;

/*! \brief Passkey Event Type */
typedef struct
{
    device_handle_id                            device_id;          /*!< \brief Device id */

} CM_PASSKEY_NOTIFY_T;

/*! \brief Connection Parameter Update Confirmation */
typedef struct
{
    device_handle_id                device_id;      /*!< \brief Device id */

    sys_status                      status;         /*!< sys_status_success or error */

    TYPED_BD_ADDR_T                 address;        /*!< address of peer */

} CM_CONNECTION_PARAM_UPDATE_CFM_T;

/*! \brief Connection Parameter Update Indication */
typedef struct
{
    device_handle_id                device_id;              /*!< \brief Device id */

    sys_status                      status;                 /*!< sys_status_success or error */

    TYPED_BD_ADDR_T                 address;                /*!< address of peer */

    uint16                          conn_interval;          /*!< Negotiated connection interval */

    uint16                          conn_latency;           /*!< Negotiated connection latency */

    uint16                          supervision_timeout;    /*!< Negotiated supervision timeout */

} CM_CONNECTION_PARAM_UPDATE_IND_T;

/*! \brief Connection Updated */
typedef struct
{
    device_handle_id                device_id;              /*!< \brief Device id */

    uint16                          conn_interval;          /*!< \brief Connection Event length */

    uint16                          conn_latency;           /*!< \brief Slave latency */

    uint16                          supervision_timeout;    /*!< \brief Supervision timeout */

} CM_CONNECTION_UPDATED_T;

/*! \brief Connection Parameter Update Signalling Indication */
typedef struct
{
    device_handle_id                device_id;              /*!< \brief Device id */

    uint16                          sig_identifier;         /*!< \brief An identifier for the specific connection update signal */

    uint16                          conn_interval_min;      /*!< \brief Connection Interval Minimum */

    uint16                          conn_interval_max;      /*!< \brief Connection Interval Maximum */

    uint16                          slave_latency;          /*!< \brief Slave Latency */

    uint16                          supervision_timeout;    /*!< \brief Supervision timeout */

}CM_CONNECTION_UPDATE_SIGNALLING_IND_T;

/*! \brief Connection Manager events types */
typedef union
{
    CM_INIT_CFM_T                               cm_init_cfm;                /*!< \brief  CM Init confirmation type */

    CM_ADV_REPORT_IND_T                         cm_adv_report_ind;          /*!< \brief  Advertisement report indication type */

    CM_RAW_ADV_REPORT_IND_T                     cm_raw_adv_report_ind;      /*!< \brief  Raw Advertisement report indication type */
    
    CM_ADVERT_STATE_IND_T                       cm_advert_state_ind;        /*!< \brief  Advertising state indication type */

    CM_SCAN_STATE_IND_T                         cm_scan_state_ind;          /*!< \brief  Scanning state indication type */

    CM_CONNECT_STATE_IND_T                      cm_connect_state_ind;       /*!< \brief  Connecting state indication type */

    CM_CONNECTION_NOTIFY_T                      cm_connection_notify;       /*!< \brief  Connection notification type */

    CM_BONDING_AUTH_IND_T                       cm_bonding_auth_ind;        /*!< \brief  Bonding authorisation indication type */

    CM_BONDING_NOTIFY_T                         cm_bonding_notify;          /*!< \brief  Bonding notification type */

    CM_ENCRYPTION_NOTIFY_T                      cm_encryption_notify;       /*!< \brief  Encryption notification type */

    CM_RADIO_EVENT_IND_T                        cm_radio_event_ind;         /*!< \brief  Radio event indication type */

#ifndef CSR101x_A05
    CM_EARLY_WAKEUP_IND_T                       cm_early_wakeup_ind;         /*!< \brief  Early Wakeup indication type */
#endif

    CM_DATA_RX_TIMING_IND_T                     cm_data_rx_timing_event_ind; /*!< \brief  Data Rx Timimg event indication type */

    CM_DISCOVERY_COMPLETE_T                     cm_disc_complete;           /*!< \brief  Discovery complete type */

    CM_WRITE_CFM_T                              cm_write_cfm;               /*!< \brief  Write confirmation type */

    CM_READ_CFM_T                               cm_read_cfm;                /*! \brief  Read confirmation type */

    CM_NOTIFICATION_T                           cm_notification;            /*! \brief  Notification type */

    CM_INDICATION_T                             cm_indication;              /*! \brief  Indication type */

    CM_NOTIFICATION_CFM_T                       cm_val_not_cfm;             /*! \brief  Notification confirmation type */

    CM_INDICATION_CFM_T                         cm_val_ind_cfm;             /*! \brief  Indication confirmation type */

    CM_READ_ACCESS_T                            cm_read_access;             /*! \brief  Read access type */

    CM_WRITE_ACCESS_T                           cm_write_access;            /*! \brief  Write access type */

    CM_SERVER_ACCESSED_T                        cm_server_accessed;         /*!< \brief  server read/write accessed */

    CM_CONNECTION_UPDATE_SIGNALLING_IND_T       cm_conn_update_signalling_ind;  /*! \brief  Connection parameter update signalling indication */

    CM_CONNECTION_PARAM_UPDATE_CFM_T            cm_conn_param_update_cfm;   /*! \brief  Connection parameter update confirmation */

    CM_CONNECTION_PARAM_UPDATE_IND_T            cm_conn_param_update_ind;   /*! \brief  Connection parameter update indication */

    CM_CONNECTION_UPDATED_T                     cm_conn_updated;            /*! \brief  Connection updated */

    CM_PASSKEY_NOTIFY_T                         cm_passkey_notify;          /*!< \brief Passkey notification type */

} CM_EVENT_T;

/*! \brief Connection manager handler type */
typedef struct
{
    void (*pCallback)(cm_event event_type, CM_EVENT_T *p_event_data);     /*!< \brief  Event handler type */

}CM_HANDLERS_T;

/*! \brief Client information type */
typedef struct
{
  CM_HANDLERS_T                                 client_handler;         /*!< \brief  Client handler */

  CM_SERVICE_T                                  service_data;           /*!< \brief  Pointer to client services list */

} CM_CLIENT_INFO_T;

/*! \brief Server information type */
typedef struct
{
    CM_HANDLERS_T                               server_handler;         /*!< \brief  Server handler */

    uint16                                      start_handle;           /*!< \brief  Start handle of the service */

    uint16                                      end_handle;             /*!< \brief  End handle of the service */

} CM_SERVER_INFO_T;

/*! \brief CM Init parameters type */
typedef struct
{
    CM_HANDLERS_T                               *cm_app_handler;        /*!< \brief  Pointer to application handlers */

    uint16                                      *conn_info;             /*!< \brief  Pointer to connection information */

    uint16                                      max_connections;        /*!< \brief  Number of connections */

    uint16                                      *bonded_devices;        /*!< \brief  Pointer to bonded device list */

    uint16                                      max_bonded_devices;     /*!< \brief  Maximum of bonded devices */

    CM_CLIENT_INFO_T                            *client_info;           /*!< \brief  Pointer to client information */

    uint16                                      max_client_services;    /*!< \brief  Maximum client services */

    CM_SERVER_INFO_T                            *server_info;           /*!< \brief  Pointer to server information */

    uint16                                      max_server_services;    /*!< \brief  Maximum server services */

    uint16                                      diversifier;            /*!< \brief  Initial diversifier */

    uint16                                      *gatt_db;               /*!< \brief  Pointer to GATT database */

    uint16                                      db_length;              /*!< \brief  Length of GATT database */

    bool                                        nvm_start_fresh;        /*!< \brief  NVM fresh start */

    uint16                                      *nvm_offset;            /*!< \brief  Pointer to NVM offset */

}CM_INIT_PARAMS_T;

/*! \brief CM Init parameters to open a GATT audio stream sink */
typedef struct
{
    device_handle_id device_id;      /*!< \brief  CM device id to transmit to */
    uint16  handle;                  /*!< \brief  GATT characterastic handle to notify on */
    uint16  char_len;                /*!< \brief  The length of the data to notify */
}cm_audio_open_param_t;
/*!@} */

#endif /*! \brief  __CM_TYPES_H__ */
