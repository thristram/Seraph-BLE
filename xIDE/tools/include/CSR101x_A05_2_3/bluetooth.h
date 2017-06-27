/*! \file bluetooth.h  -  generally useful Bluetooth types and defines
 *
 *  \brief Bluetooth specific type definitions.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 */

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "types.h"

/*============================================================================*
Public Defines
*============================================================================*/
#define LL_VER_BLUETOOTH_V1_0b 0x00
#define LL_VER_BLUETOOTH_V1_1  0x01
#define LL_VER_BLUETOOTH_V1_2  0x02
#define LL_VER_BLUETOOTH_V2_0  0x03
#define LL_VER_BLUETOOTH_V2_1  0x04
#define LL_VER_BLUETOOTH_V3_0  0x05
#define LL_VER_BLUETOOTH_V4_0  0x06
#define LL_VER_BLUETOOTH_V4_1  0x07

/*! \brief Link Layer Version number for Single Mode Bluetooth Low Energy */
#define LL_VER_BTLE         LL_VER_BLUETOOTH_V4_1

/*! \brief HCI Version number for Single Mode Bluetooth Low Energy */
#define HCI_VER_BTLE        ((hci_version_t)LL_VER_BTLE)
#define HCI_VER_INVALID     ((hci_version_t)0xff)

/*! \brief Start of ATT & GATT primitives */
#define ATT_PRIM_BASE       0x0900
#define GATT_PRIM_BASE      0x0a00

/*! \brief Start of internal system primitives (for CSR testing only) */
#define SYSTEM_PRIM_BASE    0x0b00

/*! \brief Start of application system primitives */
#define SYS_APP_PRIM_BASE   0x0c00


/*============================================================================*
Public Data Types
*============================================================================*/

typedef uint16    con_handle_t;     /*!< \brief HCI Connection handle (14 bit) */

/*! \brief Standard Bluetooth Address type */
typedef struct
{
    uint24 lap;                     /*!< \brief Lower Address Part 00..23 */
    uint8  uap;                     /*!< \brief Upper Address Part 24..31 */
    uint16 nap;                     /*!< \brief Non-significant    32..47 */
} BD_ADDR_T;

/*! \brief Typed Bluetooth Address type for LE */
typedef struct
{
    uint16    type;                 /*!< \brief Address type (public or random) */
    BD_ADDR_T addr;                 /*!< \brief Standard Bluetooth Address */
} TYPED_BD_ADDR_T;

/* Where TYPED_BD_ADDR_T.type == 1 */
#define BD_ADDR_NAP_RANDOM_TYPE_MASK        0xC000
#define BD_ADDR_NAP_RANDOM_TYPE_NONRESOLV   0x0000
#define BD_ADDR_NAP_RANDOM_TYPE_RESOLVABLE  0x4000
#define BD_ADDR_NAP_RANDOM_TYPE_RESERVED    0x8000
#define BD_ADDR_NAP_RANDOM_TYPE_STATIC      0xC000

/*! \brief Define radio events that may be reported to the application
 */
typedef enum
{
    radio_event_none,               /*!< Do not report any radio events */
    radio_event_tx_data,            /*!< Report each transmitted packet */
    radio_event_connection_event,   /*!< Report once per connection event, even if no data was sent */
    radio_event_first_tx,           /*!< Report only first TX data packet in a connection event */
} radio_event;

/*! \brief Structure for common Bluetooth low energy Connection Parameters,
 *  used for new connections and connection updates.
 */
typedef struct
{
    uint16 con_min_interval;        /*!< Connection Interval Minimum */
    uint16 con_max_interval;        /*!< Maximum Connection Interval */
    uint16 con_slave_latency;       /*!< Slave Latency */
    uint16 con_super_timeout;       /*!< Supervision Timeout */
} ble_con_params;

/*! \brief Security Key Manager Encryption Key information.
 *
 *  This structure defines all the information to be stored for an encryption
 *  key for a single device. This information is provided to the application
 *  by the Security Manager when a key is generated via the SM_KEYS_IND message,
 *  and may be requested via the SM_KEY_REQUEST_IND message.
 */
typedef struct
{
    TYPED_BD_ADDR_T bd_addr;        /*!< Bluetooth Device Address */
    uint16  flags;                  /*!< Internal flags */
    uint16  ltk[8];                 /*!< Long-Term Key */
    uint16  ediv;                   /*!< Encrypted Diversifier */
    uint16  rand[4];                /*!< Random Number */
} skm_encryption_key;


#endif /* __BLUETOOTH_H__ */
