 /******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *****************************************************************************/
/*! \file csr_mesh_types.h
 *  \brief CSRmesh library data types
 *
 *   This file contains the different data types used in CSRmesh library
 *
 *   NOTE: This library includes the Mesh Transport Layer, Mesh Control
 *   Layer and Mesh Association Layer functionality.
 */
 /*****************************************************************************/
#ifndef _CSR_MESH_TYPES_H_
#define _CSR_MESH_TYPES_H_

#include "csr_types.h"
#include "csr_mesh_result.h"

#if (CSR_MESH_ON_CHIP == 1) 
#include "ls_types.h"
#endif

/*! \addtogroup CSRmesh
 * @{
 */
 
/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief Number of timers required for CSRmesh library to be reserved by
 * the application.
 */
#define CSR_MESH_MAX_NO_TIMERS              (7) /*!< \brief User application needs
                                                * to reserve these many timers along
                                                * with application timers. Required
                                                * for CSR1010 only.\n
                                                * Example:\n
                                                * \code #define MAX_APP_TIMERS (3 + CSR_MESH_MAX_NO_TIMERS)
                                                * \endcode
                                                */

/*! \brief Maximum number of mesh network the CSRmesh stack supports.<br>
 *         The CSRmesh Stack library is built with this configuration. <br>
 *         ** This should not be modified **.
 */
#define CSR_MESH_NO_OF_NWKS                         (1) 

#define CSR_MESH_CONFIRMATION_BUFFER_SIZE_IN_BYTES  (8)
#define CSR_MESH_AUTH_CODE_SIZE_IN_BYTES            (8)

/*! \brief Invalid network id
 */
#define CSR_MESH_INVALID_NWK_ID              (0xFF)

/*! \brief short name for the device */
#define CSR_MESH_SHORT_NAME_LENGTH           (9)

/*! \brief This constant is used in the main server app to define array that is 
 *  large enough to hold the advertisement data.
 */
#define MAX_ADV_DATA_LEN                        (31)
/*! \brief CSRmesh Network key in bytes */
#define CSR_MESH_NETWORK_KEY_SIZE_BYTES         (16)
#define CSR_MESH_NETWORK_KEY_SIZE_WORDS         (8)
/*!\brief CSRmesh Mesh_Id size in bytes */
#define CSR_MESH_MESH_ID_SIZE_BYTES             (16)

/*!\brief LE Bearer Type */
#define LE_BEARER_ACTIVE                        (1)
/*!\brief GATT Bearer Type */
#define GATT_SERVER_BEARER_ACTIVE               (2)

#define CSR_MESH_CONFORMANCE_SIGNATURE_LEN      (8)
#define CSR_MESH_STACK_MAJOR_LEN                (3)
#define CSR_MESH_STACK_MINOR_LEN                (3)
#define CSR_MESH_STACK_VARIANT_LEN              (2)

/*! \brief Device Signature Size in Words and Bytes. */
#define CSR_MESH_DEVICE_SIGN_SIZE_IN_BYTES      (24)
#define CSR_MESH_DEVICE_SIGN_SIZE_IN_WORDS      (CSR_MESH_DEVICE_SIGN_SIZE_IN_BYTES/2)

/*! \brief Flag determining the type of the device */
typedef enum
{
    CSR_MESH_CONFIG_DEVICE                    = 0, /*!< \brief CSRmesh configuring device type */
    CSR_MESH_NON_CONFIG_DEVICE                = 1, /*!< \brief CSRmesh non-configuring device type */
    CSR_MESH_CONFIG_DEVICE_WITH_AUTH_CODE     = 2, /*!<\brief CSRmesh config device with auth code type */
    CSR_MESH_NON_CONFIG_DEVICE_WITH_AUTH_CODE = 3, /*!<\brief CSRmesh non-config device with auth code type */
} CSR_MESH_CONFIG_FLAG_T;

/*! \brief Operation status passed with App-Callback to indicate the result of an
 *  asynchronous operation or to inform the App that the callback is mader to request info
 */
typedef enum
{
    CSR_MESH_OPERATION_SUCCESS                = 0x00, /*!< \brief Operation status success  */
    CSR_MESH_OPERATION_STACK_NOT_INITIALIZED  = 0x01, /*!< \brief Operation status stack not initialized */
    CSR_MESH_OPERATION_NOT_PERMITTED          = 0x02, /*!< \brief Operation status operation not permited */
    CSR_MESH_OPERATION_MEMORY_FULL            = 0x03, /*!< \brief Operation status memory full */
    CSR_MESH_OPERATION_REQUEST_FOR_INFO       = 0x04, /*!< \brief Operation status request for info to app */
    CSR_MESH_OPERATION_GENERIC_FAIL           = 0xFF, /*!< \brief Operation status generic fail */
} CSR_MESH_OPERATION_STATUS_T;

/******************************************************************************/

/*! \brief 128-bit UUID type */
typedef struct
{
    CsrUint8 uuid[16]; /*!< \brief CSRmesh 128-bit UUID */
} CSR_MESH_UUID_T;

/*! \brief CSRmesh Product ID, Vendor ID and Version Information. */
typedef struct
{
    CsrUint16 vendor_id;   /*!< \brief Vendor Identifier.  */
    CsrUint16 product_id;  /*!< \brief Product Identifier. */
    CsrUint32 version;     /*!< \brief Version Number.     */
} CSR_MESH_VID_PID_VERSION_T;

/*! \brief CSRmesh conformance signature Information. */
typedef struct
{
    /*!< \brief Conformance signature. */
    CsrUint16 conformance_sig[CSR_MESH_CONFORMANCE_SIGNATURE_LEN/2];
} CSR_MESH_CONFORMANCE_SIGNATURE_T;

/*! \brief CSRmesh stack version Information. */
typedef struct
{
    CsrUint8 major[CSR_MESH_STACK_MAJOR_LEN];    /*!< \brief stack version major. */
    CsrUint8 minor[CSR_MESH_STACK_MINOR_LEN];    /*!< \brief stack version minor. */
    CsrUint8 variant[CSR_MESH_STACK_VARIANT_LEN];/*!< \brief stack version variant. */
} CSR_MESH_STACK_VERSION_T;

/*! \brief Client config info */
typedef struct
{
    CsrBool auth_code_considered_for_cc; /*!< \brief flag to indicate if Confirmation code is 
            computed using auth code or not. This is only used if type is CSR_MESH_CONFIRMATION_CODE*/
    CsrUint8 confirmaion[CSR_MESH_CONFIRMATION_BUFFER_SIZE_IN_BYTES];
}CSR_MESH_CC_INFO_T;

typedef struct 
{
    CsrUint16 *a_x; /*!< \brief x co-ordinate of public key */
    CsrUint16 *a_y; /*!< \brief y co-ordinate of public key */
    CsrUint8 *cliRandom; /*!< \brief Client Random number */
    CsrUint8 *confirmation_buffer; /*!< \brief Client confirmation code */
}CSR_MESH_MASP_CLIENT_CONFIRMATION_T;

/*! \brief 64 bit Authorisation Code type */
typedef struct
{
    /*!< \brief CSRmesh 64 bit Authorisation Code */
    CsrUint8 auth_code[CSR_MESH_AUTH_CODE_SIZE_IN_BYTES]; 
}CSR_MESH_AUTH_CODE_T;

/*! \brief CSRmesh Scan and Advertising Parameters */
typedef struct
{
    CsrUint16 scan_duty_cycle;      /*!< \brief CSRmesh scan duty cycle (0 - 100 percent) */
    CsrUint16 advertising_interval; /*!< \brief CSRmesh advertising interval in milliseconds */
    CsrUint16 advertising_time;     /*!< \brief CSRmesh advertising time  in milliseconds */
    CsrInt8 tx_power;                /*!< \brief CSRmesh tx_power */
    CsrUint8 default_ttl;           /*!< \brief CSRmesh default ttl */
}CSR_MESH_CONFIG_BEARER_PARAM_T;

/*! \brief CSRmesh Scan and Advertising Parameters */
typedef struct
{
    CsrUint8 tx_queue_size;       /*!< \brief CSRmesh Max number of messages in transmit queue */
    CsrUint8 relay_repeat_count;  /*!< \brief CSRmesh Number of times to transmit a relayed message */
    CsrUint8 device_repeat_count; /*!< \brief CsRmesh Number of times to transmit a message from this device */
}CSR_MESH_CONFIG_MSG_PARAMS_T;

/*! \brief CSRmesh Message types */
typedef enum
{
    CSR_MESH_MESSAGE_ASSOCIATION, /*!< \brief CSRmesh Association message. */
    CSR_MESH_MESSAGE_CONTROL      /*!< \brief CSRmesh Control message. */
} CSR_MESH_MESSAGE_T;

/*! \brief CSRmesh event types */
typedef enum
{
    CSR_MESH_INIT_EVENT                    = 0x0001,    /*!< \brief Type Mesh stack init event */
    CSR_MESH_REGISTER_APP_CB_EVENT         = 0x0002,    /*!< \brief Type Register-App event */
    CSR_MESH_RESET_EVENT                   = 0x0003,    /*!< \brief Type Reset event */
    CSR_MESH_START_EVENT                   = 0x0004,    /*!< \brief Type Start event */
    CSR_MESH_STOP_EVENT                    = 0x0005,    /*!< \brief Type Stop event */
    CSR_MESH_TRANSMIT_STATE_EVENT          = 0x0006,    /*!< \brief Type Bearer State event */
    CSR_MESH_START_DEVICE_INQUIRY_EVENT    = 0x0007,    /*!< \brief Type Device Enquiry event */
    CSR_MESH_ASSOC_STARTED_EVENT           = 0x0008,    /*!< \brief Type Assoc Started event */
    CSR_MESH_ASSOC_COMPLETE_EVENT          = 0x0009,    /*!< \brief Type Assoc Complete event */
    CSR_MESH_SEND_ASSOC_COMPLETE_EVENT     = 0x000A,    /*!< \brief Type Send Assoc Complete event */
    CSR_MESH_GET_DEVICE_ID_EVENT           = 0x000B,    /*!< \brief Type Get Device Id event */
    CSR_MESH_GET_DEVICE_UUID_EVENT         = 0x000C,    /*!< \brief Type Get Device Uuid event */
    CSR_MESH_MASP_DEVICE_IND_EVENT         = 0x000D,    /*!< \brief Type Masp Device Ind event */
    CSR_MESH_MASP_DEVICE_APPEARANCE_EVENT  = 0x000E,    /*!< \brief Type Masp Device Appearance event */
    CSR_MESH_NETWORK_ID_LIST_EVENT         = 0x000F,    /*!< \brief Type Network Id List event */
    CSR_MESH_SET_MAX_NO_OF_NETWORK_EVENT   = 0x0010,    /*!< \brief Type Max No of Network event */
    CSR_MESH_SET_PASSPHRASE_EVENT          = 0x0011,    /*!< \brief Type Set Passphrase event */
    CSR_MESH_SET_NETWORK_KEY_EVENT         = 0x0012,    /*!< \brief Type Set Network Key event */
    CSR_MESH_CONFIG_RESET_DEVICE_EVENT     = 0x0013,    /*!< \brief Type Config Reset Device event */
    CSR_MESH_CONFIG_SET_PARAMS_EVENT       = 0x0014,    /*!< \brief Type Config Set Params event */
    CSR_MESH_CONFIG_GET_PARAMS_EVENT       = 0x0015,    /*!< \brief Type Config Get Params event */
    CSR_MESH_GET_VID_PID_VERSTION_EVENT    = 0x0016,    /*!< \brief Type Get Vid-Pid Version event */
    CSR_MESH_GET_DEVICE_APPEARANCE_EVENT   = 0x0017,    /*!< \brief Type Get Device Appearance event */
    CSR_MESH_GROUP_SET_MODEL_GROUPID_EVENT = 0x0018,    /*!< \brief Type Group Set Model Group-id event */
    CSR_MESH_SEND_RAW_MCP_MSG_EVENT        = 0x0019,    /*!< \brief Type Send Raw MCP Msg event */
    CSR_MESH_SEND_MCP_MSG_EVENT            = 0x001A,    /*!< \brief Type Send MCP Msg event */
    CSR_MESH_MCP_REGISTER_MODEL_EVENT      = 0x001B,    /*!< \brief Type MCP Register Model event */
    CSR_MESH_MCP_REGISTER_MODEL_CLIENT_EVENT = 0x001C,  /*!< \brief Type MCP Register Client Model event */
    CSR_MESH_REMOVE_NETWORK_EVENT          = 0x001D,    /*!< \brief Type Remove Network event */
    CSR_MESH_GET_DIAG_DATA_EVENT           = 0x001E,    /*!< \brief Type Get Diagnostic Data event */
    CSR_MESH_RESET_DIAG_DATA_EVENT         = 0x001F,    /*!< \brief Type Reset Diagnostic Data event */
    CSR_MESH_REGISTER_SNIFFER_APP_CB_EVENT = 0x0020,    /*!< \brief Type Register Sniffer App Cb event */
    CSR_MESH_GET_MESH_ID_EVENT             = 0x0021,    /*!< \brief Type Get Mesh Id Data event */
    CSR_MESH_GET_NET_ID_FROM_MESH_ID_EVENT = 0x0022,    /*!< \brief Type Get Network id from Mesh Id event */
    CSR_MESH_ASSOCIATION_ATTENTION_EVENT   = 0x0023,    /*!< \brief Type Association attention event */
    CSR_MESH_BEARER_STATE_EVENT            = 0x0024,    /*!< \brief Type Bearer state is updated */
    CSR_MESH_NW_IV_UPDATED_ALREADY         = 0x0025,    /*!< \brief Type nw_iv update status event */
    CSR_MESH_NW_IV_UPDATE_STARTED          = 0x0026,    /*!< \brief Type nw_iv update start status event */
    CSR_MESH_NW_IV_WRONG_RSP_RECEIVED      = 0x0027,    /*!< \brief Type nw_iv update start status event */
    CSR_MESH_NW_KEY_IV_COMPLETE_EVENT      = 0x0028,    /*!< \brief Type Rekeying Complete event */
    CSR_MESH_NW_KEY_IV_FAILED_EVENT        = 0x0029,    /*!< \brief Type Rekeying failed event */
    CSR_MESH_GET_CONFORMANCE_SIGNATURE_EVENT = 0x0030,   /*!< \brief Type Get conformance signature event */
    CSR_MESH_GET_NETWORK_KEY_EVENT          = 0x0031,    /*!< \brief Type Get Network Key */
    CSR_MESH_GET_DHM_KEY_EVENT              = 0x0032,    /*!< \brief Type Get DHM Key */
    CSR_MESH_DEVICE_ID_UPDATE_EVENT         = 0x0033,    /*!< \brief Type Device id update event */
    CSR_MESH_GET_DEFAULT_TTL_EVENT          = 0x0034,   /*!< \brief Type Get Default ttl event */
    CSR_MESH_CONFIG_SET_MSG_PARAMS_EVENT    = 0x0035,   /*!< \brief Type Config Set Message Params event */
    CSR_MESH_CONFIG_GET_MSG_PARAMS_EVENT    = 0x0036,   /*!< \brief Type Config Get Message Params event */
    CSR_MESH_SEQ_NUM_ROLL_BACK_EVENT        = 0x0037,   /*!< \brief Type Sequence number roll back event */
    CSR_MESH_MASP_DEVICE_CONF_ARGS_EVENT    = 0x0038,   /*!< \brief device confirmation arguments event */
    CSR_MESH_MASP_CLIENT_CONFIRMATION_EVENT = 0x0039,   /*!< \brief device confirmation arguments event */
    CSR_MESH_INVALID_EVENT                  = 0xFFFF     /*!< \brief Type Invalid event */
} CSR_MESH_EVENT_T;



/*! \brief CSR Mesh Network Id List */
typedef struct
{
    /*!< \brief List of existing mesh network IDs. Each octet 
     * value identifies one distinguished mesh network. This is 
     * defined as a flexible array and a field of this type will 
     * be part of union CSR_MESH_APP_CB_DATA_T. The array netIdList
     * will have maximum number of array elements i.e. allowed by
     * the maximum size of the union. An invalid network id is 
     * defined as 0xFF.
     */
    CsrUint8 length; /*!< \brief length of netid list */
    CsrUint8 netIDList[CSR_MESH_NO_OF_NWKS]; /*!< \brief list of Net-ids */
} CSR_MESH_NETID_LIST_T;

/*! \brief CSR Mesh transmit state */
typedef struct
{
    CsrUint8 netId;/*!< \brief Nwk id for which realy is enable */
    CsrBool  enable;/*!< \brief realy information */
} CSR_MESH_NW_RELAY_T;

/*! \brief CSR Mesh transmit state */
typedef struct
{
    CsrUint16 bearerRelayActive;/*!< \brief Bearer realy information */
    CsrUint16 bearerEnabled;/*!< \brief Bearer Enable information */
    CsrUint16 bearerPromiscuous;/*!< \brief Bearer Promiscuous information */
    CSR_MESH_NW_RELAY_T relay;/*!< \brief Mcp realy information */
    CsrBool maspRelayEnable;/*!< \brief Masp realy information */
} CSR_MESH_TRANSMIT_STATE_T;

/*! \brief CSR Mesh TTL */
typedef struct
{
    CSR_MESH_MESSAGE_T msgType; /*!< \brief Msg type - Whether MASP or MCP */
    CsrUint8               ttl; /*!< \brief TTL Value */
} CSR_MESH_TTL_T;

/*! \brief CSRmesh Device Appearance. The Appearance is a 24-bit value that is 
 *   composed of an "organization" and an "organization appearance". 
 */
typedef struct
{
    CsrUint8  organization; /*!< \brief Identifies the organization which assigns
                               device appearance values */
    CsrUint16 value;        /*!< \brief Appearance value */
    CsrUint32 deviceHash;  /*!<\brief deviceHash of remote device */
}CSR_MESH_APPEARANCE_T;

/*! \brief Device Appearance data type */
typedef struct
{
    CsrUint8 shortName[CSR_MESH_SHORT_NAME_LENGTH]; /*!< \brief short name of the device */
    CSR_MESH_APPEARANCE_T appearance; /*!< \brief deviceApperance of the device */
}CSR_MESH_DEVICE_APPEARANCE_T;

/*! \brief Device indication data type */
typedef struct 
{
    CsrUint32         deviceHash; /*!<\brief deviceHash of remote device */
    CSR_MESH_UUID_T   uuid;       /*!< \brief CSRmesh 128-bit UUID of remote device*/
}CSR_MESH_MASP_DEVICE_IND_T;

/*! \brief Association attention request data type */
typedef struct
{
    CsrUint8 attract_attention; /*!< \brief Enable or disable attracting attention\n
                                 *          0 - Do not attract attention\n
                                 *          1 - Attract attention
                                 *    Refer to \ref XAP_8bit_handler_page
                                */
    CsrUint16 duration;         /*!< \brief Duration for which the attention is 
                                 *          requested. 
                                 */
} CSR_MESH_ASSOCIATION_ATTENTION_DATA_T;

/*! \brief CSR Mesh Group Id Related Data - To provide to app while handling Group model data in core mesh */
typedef struct
{
    CsrUint8              netId; /*!< \brief network Id of the nwk for 
                                       which the group is set */
    CsrUint8              model; /*!< \brief MCP Model No */
    CsrUint8              gpIdx;     /*!< \brief Group Idx */
    CsrUint8           instance;     /*!< \brief Group Instance */
    CsrUint16              gpId;     /*!< \brief Group Id */
} CSR_MESH_GROUP_ID_RELATED_DATA_T;

/*! \brief CSR Mesh Bearer state Type */
typedef struct 
{
    CsrUint16 bearerRelayActive; /*!< \brief Mask of bearer Relay state.    */
    CsrUint16 bearerEnabled;     /*!< \brief Mask of bearer Active state.   */
    CsrUint16 bearerPromiscuous; /*!< \brief Mask of promiscuous mode. */
} CSR_MESH_BEARER_STATE_DATA_T;

#if (CSR_MESH_ON_CHIP != 1)
#define CSR_MESH_MAX_NO_OF_SUPPORTED_BEARER          (2)
/*! \brief CSR Mesh Bearer specific diagnostic data Type */
typedef struct
{
    CsrUint16 bearerMask; /*!< \brief bitmask specifying bearers */
    CsrUint32 numRxMeshMsg[CSR_MESH_MAX_NO_OF_SUPPORTED_BEARER]; /*!< \brief num of mesh msg rcvd in a particular bearer */
    CsrUint32 numTxMeshMsg[CSR_MESH_MAX_NO_OF_SUPPORTED_BEARER]; /*!< \brief num of mesh msg txed in a particular bearer */
} CSR_MESH_BEARER_SPECIFIC_DIAGNOSTIC_DATA_T;
#endif /* (CSR_MESH_ON_CHIP != 1) */

/*!\brief CSR Mesh <<SRC,SEQ>> Lookup Table */
typedef struct
{
    CsrUint8  counter;  /*!< \brief Counter to keep track of oldest cache entry. */
    CsrUint16 src;      /*!< \brief Source Id */
    CsrUint32 sqn;      /*!< \brief Sequence Number */
} CSR_MESH_SQN_LOOKUP_TABLE_T;

/*!\brief CSR mesh <<SRC,SEQ>> Cache Structure. */
typedef struct{
    CsrUint8   seq_deviation;                       /*!< \brief Maximum Allowed threshold for Older Sequence Numbers. */
    CsrUint8   cached_dev_count;                    /*!< \brief Maximum Number of stored devices in the Look Up Table. */
    CSR_MESH_SQN_LOOKUP_TABLE_T *seq_lookup_table;  /*!< \brief Pointer to the memory allocated for Look Up Table.
                                                     * This should match to fit the cached_dev_count number of devices.
                                                     */
} CSR_MESH_SEQ_CACHE_T;

/*!\brief CSR Mesh mesh_id data Type */
typedef struct
{
    CsrUint8 meshId[CSR_MESH_MESH_ID_SIZE_BYTES];/*!< \brief CSR Mesh mesh_id */
} CSR_MESH_MESH_ID_DATA_T;

/*!\brief CSR Mesh device id update data Type */
typedef struct
{
    CsrUint16 deviceId;
    CsrUint8 netId;
}CSR_MESH_DEVICE_ID_UPDATE_T;

/*! \brief CSR Mesh App Event Data - to provide event, status * app data to app */
typedef struct
{
    CSR_MESH_EVENT_T              event;   /*!< \brief CSR Mesh Event */
    CSR_MESH_OPERATION_STATUS_T   status;  /*!< \brief CSR Mesh Operation Status */
    void *appCallbackDataPtr;  /*!< \brief App Data */
}CSR_MESH_APP_EVENT_DATA_T;

/*! \brief CSRmesh Device Identifier Data and Signature. */
typedef struct
{
    CsrUint8    netId;            /*!< \brief Network ID */
    CsrUint16   deviceId;         /*!< \brief Device ID  */
    CsrUint16  *deviceSign;       /*!< \brief Pointer to Device Signature.
                                   * Size of device signature is CSR_MESH_DEVICE_SIGN_SIZE_IN_WORDS.
                                   * This signature is used to securely update network parameters.
                                   */
} CSR_MESH_MASP_DEVICE_DATA_T;

/*! \brief CSRmesh Device Signature Data Linked List node. */
typedef struct CSR_MESH_MASP_DEVICE_SIGN_LIST_NODE
{
    CsrUint16  deviceId;                                           /*!< \brief Device ID  */
    CsrUint16  deviceSign[CSR_MESH_DEVICE_SIGN_SIZE_IN_WORDS];     /*!< \brief Device Signature.
                                                                    * Size of device signature is CSR_MESH_DEVICE_SIGN_SIZE_IN_WORDS.
                                                                    * This signature is used to securely update network parameters.
                                                                    */
    struct CSR_MESH_MASP_DEVICE_SIGN_LIST_NODE *next;              /*!< \brief Pointer to the next element of the linked list. */
} CSR_MESH_MASP_DEVICE_SIGN_LIST_NODE_T;

/*!\brief CSRmesh Application callback handler function*/
typedef void (*CSR_MESH_APP_CB_T) (CSR_MESH_APP_EVENT_DATA_T eventDataCallback);

/*!@} */
#endif /* _CSR_MESH_TYPES_H_ */

