/*!
        Copyright Cambridge Silicon Radio Limited and its licensors 2011.
        All rights reserved

\file   security.h

\brief  Exposes the Security Manager interface to the application.
*/


#ifndef __SECURITY_H__
#define __SECURITY_H__

/*============================================================================*
Header Files
*============================================================================*/
#include "gap_app_if.h"
#include "crypt.h"

/*============================================================================*
Default Configuration
*============================================================================*/

#define SM_DEFAULT_IO_CAP               SM_IO_CAP_NO_INPUT_NO_OUTPUT
#define SM_DEFAULT_MAX_ENC_KEY_SIZE     SM_MAX_ENC_KEY_SIZE
#define SM_DEFAULT_MIN_ENC_KEY_SIZE     SM_MIN_ENC_KEY_SIZE
#define SM_DEFAULT_OOB_DATA_PRESENT     SM_OOB_DATA_NOT_PRESENT

/*============================================================================*
Basic Types
*============================================================================*/

/*! \addtogroup SM
 * @{
 */

/*-----------------------------------------------------------------------------*
 *   Security Manager
 *   Combined Input/Output Capabilities
 */
/*! \brief  Defines of the I/O capabilities of a device */
typedef enum
{
    /*! Devices which only have a display */
    SM_IO_CAP_DISPLAY_ONLY = 0,

    /*! Devices which have a display plus a yes and no button */
    SM_IO_CAP_DISPLAY_YES_NO,

    /*! Devices which only have a (numeric) keyboard */
    SM_IO_CAP_KEYBOARD_ONLY,

    /*! Devices which have neither input nor output*/
    SM_IO_CAP_NO_INPUT_NO_OUTPUT,

    /*! Devices which have both display and (numeric) keyboard */
    SM_IO_CAP_KEYBOARD_DISPLAY
} sm_io_capabilities;

/*! \brief  Currently unimplemented */
typedef enum
{
    SM_OOB_DATA_NOT_PRESENT = 0,
    SM_OOB_DATA_PRESENT,
} sm_oob_data_present;

/*! \brief  Response codes for the diversifier verdict*/
typedef enum
{
    SM_DIV_APPROVED = 0,    /*!< Diversifier and the corresponding LTK valid */
    SM_DIV_REVOKED          /*!< Diversifier and the corresponding LTK has been revoked*/
} sm_div_verdict;

/*-----------------------------------------------------------------------------*
 *   Security Manager
 *   Security Information Types
 *
 *------------------------------------------------------------------------------*/
/*! \brief  Defines types of security information present in an #SM_KEYSET_T by
 *          the (1<<#sm_key_type) bit being set in #SM_KEYSET_T::keys_present
 */
typedef enum
{
    SM_KEY_TYPE_NONE,             /*!< Not currently supported */
    SM_KEY_TYPE_ENC_CENTRAL,      /*!< Peer LTK + EDIV + RAND */
    SM_KEY_TYPE_DIV,              /*!< Local DIV sent to peer */
    SM_KEY_TYPE_SIGN,             /*!< Reserved */
    SM_KEY_TYPE_ID,               /*!< Peer IRK */
    SM_BD_ADDR                    /*!< Peer public/static BD_ADDR */
} sm_key_type;

/*! \brief  Defines the size, in octets, of the encryption key.
 * @{
 */
typedef uint8   sm_enc_key_size;

/*============================================================================*
Public Definitions
*============================================================================*/

/*-----------------------------------------------------------------------------*
 *   Security Manager
 *   Max/Min encryption key size allowed.
 *
 *------------------------------------------------------------------------------*/
#define SM_MAX_ENC_KEY_SIZE                     ((sm_enc_key_size)0x10)
#define SM_MIN_ENC_KEY_SIZE                     ((sm_enc_key_size)0x07)
/*! @} */

/*============================================================================*
Public Function Prototypes
*============================================================================*/
/*! \brief      Initialising the Security Manager. The application has to
 *              supply SMinit() with the latest distributed diversifier, in
 *              order to maintain unique keys after a power cycle. This
 *              function shall be called after GattInit().
 *
 * \note        If the application does not intend to revoke keys and
 *              does not consider key uniqueness important, this call can
 *              be left out.
 *
 *  \param      div latest distributed diversifier.
 *
 */
extern void SMInit(uint16 div);

/*! \brief      Runtime configuration of Security Manager features. On-chip
 *              applications will never need to call this function, as for
 *              those applications SM features are derived at link time.
 *              However, some classes of application that communicate with an
 *              off-chip host processor may find this function useful.
 *
 *  \note       If both enable_key_storage and enable_keys_request
 *              are TRUE, then the internal persistent memory will be checked
 *              for valid keys. If no keys exist then the application will be
 *              asked to supply the keys if it knows about them.
 *
 *              This function cannot be called before GattInit() has been called.
 *
 *  \param      enable_key_storage If TRUE then the SM will uses its internal
 *              persistent memory block for key storage.
 *
 *  \param      enable_keys_request If TRUE then the SM will request security
 *              keys from the application if it.
 *
 *  \param      enable_div_approval If TRUE then the SM will request DIV approval
 *              from the application before re-establishing a secure connection.
 *
 *  \param      enable_pairing_auth If TRUE then the SM will request pairing authorisation
 *              from the application before allowing the peer to start pairing.
 *
 *  \param      enable_long_term_key If TRUE then the SM will request an LTK
 *              from the application, otherwise it will recreate internally.
 *
 */
extern void SMFeaturesReq(bool enable_key_storage,
                          bool enable_keys_request,
                          bool enable_div_approval,
                          bool enable_pairing_auth,
                          bool enable_long_term_key);

/*! \brief      Starts security procedures on the link. For a master device
 *              this may involve initiating Encryption or Pairing. For a slave
 *              device this will send a Security Request to the master.
 *
 *  \param      bd_addr peer address
 *
 *  \returns    FALSE if GAP security mode is NONE or the device is not
 *              connected to bd_addr.
 */
extern bool SMRequestSecurityLevel(TYPED_BD_ADDR_T  *bd_addr);

/*! \brief Supplies a key previously stored by the application
 *
 *  Should only be called in response to #SM_KEY_REQUEST_IND
 *
 *  \param bd_addr from #SM_KEY_REQUEST_IND
 *  \param keyset Pointer to security keys or NULL if none available
 *
 *  \returns none
 */
extern void SMKeyRequestResponse(const TYPED_BD_ADDR_T *bd_addr,
                                 const SM_KEYSET_T *keyset);

/*! \brief Stores a key in the Security Manager persistent storage
 *
 *  This may be called by the application in response to an
 *  SM_KEYS_IND or at any other time the application wishes to
 *  store a key in the Security Manager persistent storage
 *
 *  Where this call causes another key to be removed from the
 *  persistent store, it will be reported back to the application
 *  as per SMRemoveStoredKey()
 *
 *  \param bd_addr peer address used as an index for this data
 *  \param keyset Pointer to security keys
 *
 *  \returns TRUE if keys not already stored, otherwise FALSE
 */
extern bool SMAddStoredKey(const TYPED_BD_ADDR_T *bd_addr,
                           const SM_KEYSET_T *keyset);

/*! \brief Retrieves a key from the Security Manager persistent storage
 *
 *  This may be called with keyset pointer NULL if a simple true/false
 *  return code is required.
 *
 *  \param bd_addr index address used in SMAddStoredKey()
 *  \param keyset Pointer to security key buffer or NULL
 *
 *  \returns TRUE if key available, otherwise FALSE
 */
extern bool SMReadStoredKey(const TYPED_BD_ADDR_T *bd_addr,
                            SM_KEYSET_T *keyset);

/*! \brief Remove a key from the Security Manager persistent storage
 *
 *  \param bd_addr index address used in SMAddStoredKey()
 *
 *  \returns TRUE if address found in key store, otherwise FALSE
 */
extern bool SMRemoveStoredKey(const TYPED_BD_ADDR_T *bd_addr);

/*! \brief This function set the I/O capabilities of the device.
 *
 * When initialising the security manager as well as when no security request
 * are activate is possible to set the I/O capabilities of the device.
 *
 *  \param     io_capabilities    I/O capabilities of the device:
 *                          #SM_IO_CAP_NO_INPUT_NO_OUTPUT,
 *                          #SM_IO_CAP_DISPLAY_ONLY,
 *                          #SM_IO_CAP_DISPLAY_YES_NO,
 *                          #SM_IO_CAP_KEYBOARD_ONLY,
 *                          #SM_IO_CAP_KEYBOARD_DISPLAY
 */
extern void SMSetIOCapabilities(sm_io_capabilities    io_capabilities);

/*! \brief This function set the maximum accepted encryption key size.
 *
 * When initialising the security manager as well as when no security request
 * are activate is possible to set the maximum encryption key size. The key
 * size will be negotiated with the peer and the smaller value of the maximum
 * encryption key length will be used as key size. If this value is small than
 * one of the set minimum encryption key size the paring will fail.
 * \ref Security Manager section 2.3.4
 *
 * The maximum key size currently support in Low Energy Bluetooth is 16 octet
 * corresponding to a 128 bit key.
 *
 * \note Changing the key size while already encrypted will not change the keys
 * before a key refresh request are send.
 *
 *  \param     key_size    The maximum key size in octets or zero to set default
 */
extern void SMSetMaxEncKeySize(sm_enc_key_size key_size);

/*! \brief This function set the minimum accepted encryption key size.
 *
 * When initialising the security manager as well as when no security request
 * are activate is possible to set the mimimum encryption key size. The key
 * size will be negotiated with the peer and the smaller value of the maximum
 * encryption key size will be used as key size. If this value is smaller than
 * the set minimum encryption key size the paring will fail.
 * \ref Security Manager section 2.3.4
 *
 * The minimum key size currently support in Low Energy Bluetooth is 7 octet
 * corresponding to a 56 bit key.
 *
 * \note Changing the key size while already encrypted will not change the keys
 * before a key refresh request are send.
 *
 *  \param     key_size    The minimum key size in octets to zero to set default
 */
extern void SMSetMinEncKeySize(sm_enc_key_size key_size);

/*! \brief Indicate whether the Security Manager should request distribution of
 * the master's Long Term Key during bonding.
 *
 * During bonding, the peer devices negotiate which keys to distribute to each
 * other. This function allows the application to decide whether the LTK, EDIV
 * and Rand should be distributed by the master of the connection. It can be used
 * when the local device is the master or when it is the slave.
 *
 * The default is for the Security Manager to not request distribution of the
 * master key, as typically this key is only required if the master and slave
 * devices are likely to swap roles but wish to retain the existing bond.
 *
 * \param distribute_ltk Boolean flag indicating whether or not to distribute
 * the master's LTK, EDIV, and Rand.
 */
extern void SMDistributeMasterLtk(bool distribute_ltk);

/*! \brief Generate and set a new resolvable private address
 *
 *         This function may be called at any time to change the current
 *         random address to a new resolvable private address. If called
 *         whilst Advertising with a random address, the new address takes
 *         immediate effect and advertising need not be re-started.
 *
 *  \param random random part of address or if zero an internal random number
 *         generator is used
 *  \returns FALSE on failure or if random >= 0x3FFFFF
 */
extern bool SMPrivacyRegenerateAddress(uint24 random);

/*! \brief Attempt to resolve an address against a list of IRKs
 *  \param addr address to resolve 
 *  \param irk pointer to first entry in list of IRKs. Each IRK is 128-bits and
 *         so consists of 8 consecutive uint16 values, which are stored word wise
 *         little endian.
 *
 *  \param num_irk number of IRKs supplied - suggested default 1
 *  \param size_irk size in words of each IRK record - suggested default 8,
 *         though if the list is actually an array of a larger structure
 *         it should be the sizeof array element
 *  \returns zero-based index of the first IRK record that is consistent
 *         with addr or negative if none are
 *  \note that this function returns immediately with a negative return code
 *         if the type of \a addr is not private resolvable.
 *
 *  \par Example of key:
 *         The irk 0x000102030405060708090a0b0c0d0e0f (MSB -> LSB) is stored:
 *  \code
 *         uint16 irk[] = {0x0e0f, 0x0c0d, 0x0a0b, 0x0809, 0x0607, 0x0405, 0x0203, 0x0001};
 *  \endcode
 *
 *  \par Example simple usage:
 *  \code
 *         uint16 bonded_irk[8];
 *
 *         bool IsBondedPeer(const TYPED_BD_ADDR_T* addr)
 *         {
 *             return (SMPrivacyMatchAddress(addr, bonded_irk, 1, 8) == 0);
 *         }
 *  \endcode
 *
 *  \par Example advanced usage:
 *  \code
 *         struct
 *         {
 *             MyData data;
 *             SM_KEYSET_T keys;
 *         } peer_data[MAX_PEERS];
 *
 *         const TYPED_BD_ADDR_T* PeerResolve(const TYPED_BD_ADDR_T* addr)
 *         {
 *             int known_peer = SMPrivacyMatchAddress(addr, peer_data[0].keys.irk, MAX_PEERS, sizeof(*peer_data));
 *             if (known_peer < 0)
 *                 return addr;
 *             return &peer_data[known_peer].keys.id_addr;
 *         }
 *  \endcode
 */
extern int SMPrivacyMatchAddress(const TYPED_BD_ADDR_T *addr, const uint16 *irk, uint16 num_irk, uint16 size_irk);

/*! \brief Currently unsupported
 */
extern void SMSetOOBDataPresent(sm_oob_data_present oob_data_present);

/*! \brief After receiving an #SM_PASSKEY_INPUT_IND this function call
 *         indicates to the Security Manager that the user input a
 *         passkey.
 *  \param bd_addr from #SM_PASSKEY_INPUT_IND
 *  \param passkey pointer to passkey value input
 *  \returns None
 */
extern void SMPasskeyInput(TYPED_BD_ADDR_T *bd_addr, const uint32 *passkey);

/*! \brief After receiving an #SM_PASSKEY_INPUT_IND or #SM_PASSKEY_DISPLAY_IND
 *         this function call indicates to the Security Manager that the user
 *         cancelled passkey pairing.
 *  \param bd_addr from #SM_PASSKEY_INPUT_IND or #SM_PASSKEY_DISPLAY_IND
 *  \returns None
 */
extern void SMPasskeyInputNeg(TYPED_BD_ADDR_T *bd_addr);

/*! \brief After receiving #SM_PASSKEY_DISPLAY_IND this function call
 *         confirms that the passkey has been displayed.
 *  \note  If the application wishes to display a different passkey from
 *         the one suggested in #SM_PASSKEY_DISPLAY_IND it should call
 *         SMPasskeyInput() instead to tell the Security Manager the new
 *         passkey.
 *  \param bd_addr from #SM_PASSKEY_DISPLAY_IND
 *  \returns None
 */
#define SMPasskeyDisplayed(bd_addr) SMPasskeyInput(bd_addr, NULL)

/*! \brief  Approve or reject encrypting the link with the LTK referred to by
 *          diversifier in the #SM_DIV_APPROVAL_IND.
 *
 *  \note Should only be called in response to #SM_DIV_APPROVAL_IND, and
 *  needs to be called immediately after receiving that event.
 *
 *  \param cid      Connection identifier for established BLE-U connection
 *  \param verdict  Set to #SM_DIV_APPROVED if the diversifier refers to an
 *                  LTK which are valid for encrypting the link. Set to
 *                  #SM_DIV_REVOKED, if the diversifier referencing to a
 *                  revoked LTK.
 *
 *  \returns None
 */
extern void SMDivApproval(uint16 cid, sm_div_verdict verdict);

/*! \brief  Authorise or reject a pairing request from the peer device.
 *
 *  If the application does not call (or reference) this function, then
 *  all pairing requests will be handled automatically by the firmware.
 *  *
 *  The 'data' parameter should be copied from the SM_PAIRING_AUTH_IND event
 *  sent to the application.
 *
 *  \note Should only be called in response to #SM_PAIRIING_AUTH_IND, and
 *  needs to be called immediately after receiving that event.
 *
 *  \param data        The data parameter from the original IND event
 *  \param authorised  Boolean flag: TRUE indicates the pairing may proceed.
 *                     FALSE rejects the pairing request.
 *
 *  \returns None
 */
extern void SMPairingAuthRsp(void* data, bool authorised);

/*! \brief  Called by the application in response to an SM_LONG_TERM_KEY_IND to
 *          provide the firmware with an externally-generated Long Term Key
 *          for the current connection (if it has one) or to indicate that it
 *          does not have an LTK available for this connection.
 *
 *          The application can use the EDIV and RAND parameters from the master
 *          for anything it likes. However they are not required, and can be set
 *          to zero at the master side, and ignored at the slave.
 *
 *          If the application has an LTK, it should provide a pointer to key.
 *          The 128-bit key 0x000102030405060708090a0b0c0d0e0f (MSB -> LSB) is stored:
 *  \code
 *          uint16 key[] = {0x0e0f, 0x0c0d, 0x0a0b, 0x0809, 0x0607, 0x0405, 0x0203, 0x0001};
 *  \endcode
 *
 *          The application can indicate it does not have a key by setting
 *          \a long_term_key to NULL, or by setting \a key_size to 0.
 *          In this case the Security Manager will recreate the LTK internally
 *          using the EDIV and RAND. This may result in a subsequent
 *          SM_DIV_APPROVAL_IND message for the application to respond to.
 *
 *          The application should indicate the desired key size. For a 128-bit
 *          key, the key size should be set to 16. For shorter keys (for example
 *          to meet government export restrictions), the key size can be reduced
 *          to any length down to 7 bytes. In this case the firmware will zero
 *          the trailing bytes of the key. For example, if the application
 *          supplies key 0x123456789abcdef0123456789abcdef0 and states it wants
 *          a key size of 8, the firmware will shorten the key to
 *          0x123456789abcdef00000000000000000.
 *
 *  \note This function should only be called in response to #SM_LONG_TERM_KEY_IND,
 *  and shall be called immediately after receiving that event.
 *
 *  \note Unlike internally-generated LTKs, the firmware does not compare the
 *  current security mode (unauthenticated or authenticated) against the MITM
 *  protection flag for the supplied key.
 *
 *  \param cid              Connection identifier for established BLE-U connection
 *  \param long_term_key    Pointer to an LTK for this connection, or NULL if
 *                          the application does not have an LTK for the link. The
 *                          LTK is an 8-word array.
 *  \param mitm_protection  TRUE if the LTK includes Man-in-the-Middle protection.
 *                          If the LTK was randomly-generated then this should
 *                          be TRUE.
 *  \param key_size         Encryption key size (7 to 16 bytes) of the LTK
 *
 *  \returns Nothing
 */
extern void SMLongTermKeyRsp(uint16 cid, uint16* long_term_key, bool mitm_protection, uint8 key_size);

/*! \brief  Get device's own IRK.
 *
 *  \param irk     The IRK is 128-bits and consists of 8 consecutive uint16
 *                 values, which are stored word wise little endian.  This
 *                 memory should be allocated by the caller. For example:
 *  \code
 *                 uint16 irk[8];
 *  \endcode
 *
 *  \returns None
 */
extern void SMPrivacyGetOwnIrk(uint16 *irk);
/*! @} */


#endif /* __SECURITY_H__ */
