/*! \file
 *  \brief  Link Supervisor type definitions
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd 2010 - 2011
 */

#ifndef __LS_TYPES_H__
#define __LS_TYPES_H__


/*============================================================================*
Header Files
*============================================================================*/


/*============================================================================*
Public Definitions
*============================================================================*/

/* Maximum size of an advertising or scan response event payload */
#define ADVSCAN_MAX_PAYLOAD 31


/*============================================================================*
Public Data Types
*============================================================================*/

/*! \addtogroup LSAPP
 * @{
 */
/*! \brief The type of a typed Bluetooth address */
typedef enum
{
    ls_addr_type_public = 0,
    ls_addr_type_random = 1,

    ls_addr_type_null = -1
} ls_addr_type;

/*! \brief The type of Scan to perform */
typedef enum
{
    ls_scan_type_passive = 0,
    ls_scan_type_active = 1,

    ls_scan_type_null = -1
} ls_scan_type;

/*! \brief Whitelist usage */
typedef enum
{
    whitelist_disabled,     /*!< Whitelist is not used */
    whitelist_enabled,      /*!< Whitelist is to be used */

    /* The following constants have been deprecated. They are now mapped to one
     * of the two constants defined above, for backwards-compatibility.
     */
    whitelist_mode_null = whitelist_disabled,
    whitelist_mode_scan_false = whitelist_disabled,
    whitelist_mode_scan_true = whitelist_enabled,
    whitelist_mode_adv_false = whitelist_disabled,
    whitelist_mode_adv_true = whitelist_enabled,
    whitelist_mode_conn_false = whitelist_disabled,
    whitelist_mode_conn_true = whitelist_enabled
} whitelist_mode;

/*! @} */


/*! \addtogroup LSA
 * @{
 */

/*! \brief  Type corresponding to advert types received in an Advertising Report */
typedef enum
{
   ls_advert_connectable_undirected = 0,
   ls_advert_connectable_directed   = 1,
   ls_advert_discoverable           = 2,
   ls_advert_non_connectable        = 3,
   ls_advert_scan_response          = 4,
} ls_advert_type;

/*! \brief AD Type values
 *
 *  \details Transmitted advertising data contains a sequence of AD Structures.
 *           Each contains a AD Type field which defines the remainder of the
 *           structure. See Bluetooth Specification Vol. 3 (GAP) Part C Section 11 and the
 *           latest Core Specification Supplement, CSS<I>n</I> Part A, for more details.
 */
typedef enum
{
    AD_TYPE_FLAGS                       = 0x01,   /*!< Advertising Data Flags as defined in CSS v4 Part A Section 1.3.*/
    AD_TYPE_SERVICE_UUID_16BIT          = 0x02,   /*!< Incomplete list of 16-bit Service Class UUIDs.                 */
    AD_TYPE_SERVICE_UUID_16BIT_LIST     = 0x03,   /*!< Complete list of 16-bit Service Class UUIDs.                   */
    AD_TYPE_SERVICE_UUID_32BIT          = 0x04,   /*!< Incomplete list of 32-bit Service Class UUIDs.                 */
    AD_TYPE_SERVICE_UUID_32BIT_LIST     = 0x05,   /*!< Complete list of 32-bit Service Class UUIDs.                   */
    AD_TYPE_SERVICE_UUID_128BIT         = 0x06,   /*!< Incomplete list of 128-bit Service Class UUIDs.                */
    AD_TYPE_SERVICE_UUID_128BIT_LIST    = 0x07,   /*!< Complete list of 128-bit Service Class UUIDs.                  */
    AD_TYPE_LOCAL_NAME_SHORT            = 0x08,   /*!< Shortened local device name. This supplied data must match the
                                                       first <I>n</I> characters of the complete name. See CSS v4
                                                       Part A Section 1.2.                                            */
    AD_TYPE_LOCAL_NAME_COMPLETE         = 0x09,   /*!< Complete local device name. See CSS v4 Part A Section 1.2.     */
    AD_TYPE_TX_POWER                    = 0x0A,   /*!< Transmitted power level. May be used with received RSSI to
                                                       estimate the RF path loss, see CSS v4 Part A Section 1.5.      */
    AD_TYPE_OOB_DEVICE_CLASS            = 0x0D,   /*!< Class of Device. See Bluetooth Assigned Numbers,
                                                       https://www.bluetooth.org/assigned-numbers.                    */
    AD_TYPE_OOB_SSP_HASH_C_192          = 0x0E,   /*!< Out of Band simple pairing hash, (P-192 elliptic curve). To be
                                                       sent OOB only, not over the air.                               */
    AD_TYPE_OOB_SSP_RANDOM_R_192        = 0x0F,   /*!< Out of Band simple pairing randomizer, (P-192 elliptic curve).
                                                       To be sent OOB only, not over the air.                         */
    AD_TYPE_SM_TK                       = 0x10,   /*!< Security Manager TK value. To be sent OOB only, not over the
                                                       air. See CSS v4 Part A Section 1.8.                            */
    AD_TYPE_SM_FLAGS                    = 0x11,   /*!< Security Manager Out of Band Flags. To be sent OOB only, not
                                                       over the air. See CSS v4 Part A Section 1.7.                   */
    AD_TYPE_SLAVE_CONN_INTERVAL         = 0x12,   /*!< Slave connection interval range. See CSS v4 Part A Section 1.9.*/
    AD_TYPE_SIGNED_DATA                 = 0x13,
    AD_TYPE_SERVICE_SOLICIT_UUID_16BIT  = 0x14,   /*!< List of 16-bit Service Solicitation UUIDs.                     */
    AD_TYPE_SERVICE_SOLICIT_UUID_128BIT = 0x15,   /*!< List of 128-bit Service Solicitation UUIDs.                    */
    AD_TYPE_SERVICE_DATA_UUID_16BIT     = 0x16,   /*!< Service Data - 16 bit UUID. See CSS v4 Part A Section 1.11.    */
    AD_TYPE_PUBLIC_TARGET_ADDRESS       = 0x17,   /*!< Public target address. See CSS v4 Part A Section 1.13.         */
    AD_TYPE_RANDOM_TARGET_ADDRESS       = 0x18,   /*!< Random target address. See CSS v4 Part A Section 1.14.         */
    AD_TYPE_APPEARANCE                  = 0x19,   /*!< The Appearance data type defines the external appearance
                                                       of the device. See CSS v4 Part A Section 1.12.                 */
    AD_TYPE_ADVERTISING_INTERVAL        = 0x1A,   /*!< Advertising interval. See CSS v4 Part A Section 1.15.          */
    AD_TYPE_LE_BT_ADDRESS               = 0x1B,   /*!< Local device's Bluetooth address and type. To be sent OOB only,
                                                       not over the air. See CSS v4 Part A Section 1.16.              */
    AD_TYPE_LE_ROLE                     = 0x1C,   /*!< Role of the local device. To be sent OOB only,
                                                       not over the air. See CSS v4 Part A Section 1.17.              */
    AD_TYPE_OOB_SSP_HASH_C_256          = 0x1D,   /*!< Out of Band simple pairing hash, (P-256 elliptic curve). To be
                                                       sent OOB only, not over the air.                               */
    AD_TYPE_OOB_SSP_RANDOM_R_256        = 0x1E,   /*!< Out of Band simple pairing randomizer, (P-256 elliptic curve).
                                                       To be sent OOB only, not over the air.                         */
    AD_TYPE_SERVICE_SOLICIT_UUID_32BIT  = 0x1F,   /*!< List of 32-bit Service Solicitation UUIDs.                     */
    AD_TYPE_SERVICE_DATA_UUID_32BIT     = 0x20,   /*!< Service Data - 32 bit UUID. See CSS v4 Part A Section 1.11.    */
    AD_TYPE_SERVICE_DATA_UUID_128BIT    = 0x21,   /*!< Service Data - 128 bit UUID. See CSS v4 Part A Section 1.11.   */
    AD_TYPE_MANUF                       = 0xFF    /*!< Manufacturer specific data. See CSS v4 Part A Section 1.4.     */
} ad_type;

typedef enum
{
    ad_src_advertise,
    ad_src_scan_rsp
} ad_src;

/*! @} */

#endif /* __LS_TYPES_H__ */

