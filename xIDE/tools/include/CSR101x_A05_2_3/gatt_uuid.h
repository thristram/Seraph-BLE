/*! \file gatt_uuid.h
 *  \brief Common Bluetooth UUIDs and macros to help applications
 *     create in-code GATT databases.
 *
 *  The UUIDs are as specified by the Bluetooth SIG.  For more
 *  details, see the \ref UUIDS and \ref GATTHELP modules.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2010-2011
 */

#ifndef __GATT_UUID_H__
#define __GATT_UUID_H__

/*============================================================================*
Interface Header Files
*============================================================================*/
#include "att_prim.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/*! \addtogroup UUIDS
 * @{
 */
/*! These UUIDs are defined by the Bluetooth SIG. See the Bluetooth Assigned
 * Numbers website (Generic Attribute Profile section) for more information:
 * https://www.bluetooth.org/technical/assignednumbers/home.htm
 *
 */

/*! \name GATT Services
 *  @{
 */
#define UUID_GAP                    0x1800  /*!< \brief Generic Access Profile */
#define UUID_GATT                   0x1801  /*!< \brief Generic Attribute Profile */
/* @} */

/*! \name GATT Attribute Types
 * @{
 */
#define UUID_PRIMARY_SERVICE        0x2800  /*!< \brief Primary Service */
#define UUID_SECONDARY_SERVICE      0x2801  /*!< \brief Secondary Service */
#define UUID_INCLUDE                0x2802  /*!< \brief Include */
#define UUID_CHARACTERISTIC         0x2803  /*!< \brief Characteristic */
/* @} */


/*! \name GATT Characteristic Descriptors
 *  @{
 */
/*! \brief Characteristic Extended Properties */
#define UUID_CHAR_EXT_PROPS         0x2900
/*! \brief Characteristic User Description */
#define UUID_CHAR_USER_DESC         0x2901
/*! \brief Client Characteristic Configuration */
#define UUID_CLIENT_CHAR_CFG        0x2902
/*! \brief Server Characteristic Configuration */
#define UUID_SERVER_CHAR_CFG        0x2903
/*! \brief Characteristic Format */
#define UUID_CHAR_FORMAT            0x2904
/*! \brief Characteristic Aggregate Format */
#define UUID_CHAR_AGG_FORMAT        0x2905
/* @} */


/*! \name GATT Characteristic Types
 *  @{
 */
#define UUID_DEVICE_NAME            0x2A00  /*!< \brief Device Name */
#define UUID_APPEARANCE             0x2A01  /*!< \brief Appearance */
#define UUID_PER_PRIV_FLAG          0x2A02  /*!< \brief Peripheral Privacy Flag */
#define UUID_RECONNECTION_ADDR      0x2A03  /*!< \brief Reconnection Address */
/*! \brief Peripheral Preferred Connection Parameters */
#define UUID_PER_PREF_CONN_PARAMS   0x2A04
#define UUID_SERVICE_CHANGED        0x2A05  /*!< \brief Service Changed */
/* @} */

/*! \name Database Attribute Flags
 *  @{
 */
/*! \brief No attribute flag defined */
#define ATT_ATTR_NO_FLAG            0x0000
/*! \brief No additional attribute flag defined */
#define ATT_ATTR_NO_ADD_FLAG        0x0000
/* @} */
/* @} */


/*! The macros in this section are a set of generic and GATT-specific
 * macros which together build a library of macros that can be used
 * when constructing a static GATT server database. Using the macros
 * should remove some of the complexity in building standard
 * attributes, particularly in cases where 16-bit values such as a
 * UUID or handle are not word-aligned and therefore need to be
 * byte-swapped and split across two 16-bit integers.
 *
 * As an example of how to use the macros, here is the start of the
 * database definition of a simple GATT server.  We want to declare a
 * GAP service, putting the handle numbers of each attribute in braces
 * {like this}:
 *
 * \verbatim
{0001}   Primary Service Declaration (GAP)
{0002}   Characteristic, prop=[READ], handle=0x0003, uuid=DEVICE NAME
{0003}   Characteristic value: "Example Server"
{0004}   Characteristic, prop=[READ], handle=0x0005, uuid=APPEARANCE
{0005}   0x0011
{0006}   Characteristic, prop=[READ|SGWR], handle=0x0007, uuid=RECONNECTION ADDR
{0007}   0x0000 0x0000 0x0000
{0008}   Characteristic, prop=[SGWR], handle=0x0009, uuid=PERIPHERAL PRIVACY FLAG
{0009}   0x00
{000a}   Characteristic, prop=[READ|WREQ|WCMD], handle=0x000b, uuid=PREF. CONNECTION PARAMS
{000b}   min=6, max=6, latency=0, to=2000 (0x07D0)
{000c}   Characteristic Extended props (Reliable Write)
\endverbatim
 *
 * NOTE: While the final Characteristic is followed by a Characteristic
 *       Extended Properties attribute, the char property bit XTND is not
 *       set, therefore the extended properties will not be used.
 *
 * This translates to the following C code:
 *
 * \code
uint16 example_db[] =
{
    GATT_DECL_PRIM_SERV_UUID16(ATT_ATTR_NO_FLAG, UUID_GAP),
    GATT_DECL_CHAR16(ATT_ATTR_NO_FLAG, ATT_ATTR_NO_ADD_FLAG,
                     ATT_PERM_READ,
                     0x0003, UUID_DEVICE_NAME),
    GATT_DECL_CHAR_VALUE16(ATT_ATTR_NO_FLAG,
                           0x0E,
                           BYTE_JOIN_16('E','x'), BYTE_JOIN_16('a','m'),
                           BYTE_JOIN_16('p','l'), BYTE_JOIN_16('e',' '),
                           BYTE_JOIN_16('S','e'), BYTE_JOIN_16('r','v'),
                           BYTE_JOIN_16('e','r')),
    GATT_DECL_CHAR16(ATT_ATTR_NO_FLAG, ATT_ATTR_NO_ADD_FLAG,
                     ATT_PERM_READ,
                     0x0005, UUID_APPEARANCE),
    GATT_DECL_CHAR_VALUE16(ATT_ATTR_NO_FLAG,
                           0x02, BYTE_SWAP_16(0x0011)),
    GATT_DECL_CHAR16(ATT_ATTR_NO_FLAG, ATT_ATTR_NO_ADD_FLAG,
                     ATT_PERM_READ|ATT_PERM_WRITE_SIGNED,
                     0x0007, UUID_RECONNECTION_ADDR),
    GATT_DECL_CHAR_VALUE16(ATT_ATTR_NO_FLAG,
                           0x0006, 0x0000, 0x0000, 0x0000),
    GATT_DECL_CHAR16(ATT_ATTR_NO_FLAG, ATT_ATTR_NO_ADD_FLAG,
                     ATT_PERM_WRITE_SIGNED, 0x0009, UUID_PER_PRIV_FLAG),
    GATT_DECL_CHAR_VALUE16(ATT_ATTR_NO_FLAG, 0x0001, 0x0000),
    GATT_DECL_CHAR16(ATT_ATTR_NO_FLAG, ATT_ATTR_NO_ADD_FLAG,
                     ATT_PERM_READ|ATT_PERM_WRITE_REQ|ATT_PERM_WRITE_CMD,
                     0x000b, UUID_PER_PREF_CONN_PARAMS),
    GATT_DECL_CHAR_VALUE16(ATT_ATTR_NO_FLAG,
                           0x0008,
                           BYTE_SWAP_16(0x0006), BYTE_SWAP_16(0x0006),
                           0x0000, BYTE_SWAP_16(0x07d0)),
    GATT_DECL_CHAR_EXT_PROPS(ATT_ATTR_NO_FLAG,
                             ATT_PERM_RELIABLE_WRITE),
};
\endcode
 *
 * In a real application this would obviously just be the first of many
 * services we would declare, and the \c example_db array would continue
 * for a good deal longer!
 */

/*! \addtogroup GATTHELP
 *  @{
 */
/*! \name Word Assembly and Disassembly Macros
 *  @{
 */
/*! \brief Extract the MSB of a 16-bit integer, shifting it down to
 *  the lower 8-bits.
 */
#define WORD_MSB(_val)              ( ((_val) & 0xff00) >> 8 )
/*! \brief Extract the LSB of a 16-bit integer. */
#define WORD_LSB(_val)              ( ((_val) & 0x00ff) )

/* The following macros convert the 16-bit UUIDs above into a byte-swapped
 * 16-bit value or a byte-swapped pair of 8-bit values for use when creating the
 * GATT database or within applications.
 */

/*! \brief Swap the byte order of a 16-bit word. */
#define BYTE_SWAP_16(_val)          \
    ( ((uint16)WORD_LSB(_val) << 8) | WORD_MSB(_val) )

/*! \brief Split a 16-bit word into two bytes, LSB first. */
#define BYTE_SPLIT_16(_val)         WORD_LSB(_val), WORD_MSB(_val)

/*! \brief Combine two 8-bit values into a single 16-bit value, with
 *  masking to ensure stray bits don't get set.
 */
#define BYTE_JOIN_16(_msb, _lsb)    \
    ( ((uint16)WORD_LSB(_msb) << 8) | WORD_LSB(_lsb) )
/* @} */

/*! \name Generic Attribute Declaration.
 *
 * Applications should supply the attribute type and overall length in
 * bytes. These macros are used by further macros below to build complete
 * attributes including all data fields.
 *
 * The length is either a 6-bit or 8-bit field, depending on whether the
 * attribute type is or is not a Characteristic Declaration (as that attribute
 * type needs an extra 2 bits for the Additional Flags field). Rather than try
 * to cope with both cases in one macro we simply provide two related macros
 * and expect the caller to use the right one.
 *
 * @{
*/
/*! \brief Declare an attribute */
#define GATT_DECL_ATTR(_attr, _flags,  _len)             \
    ( (((uint16)(_attr) & 0x000f) << 12) | \
      (((uint16)(_flags) & 0x000f) << 8) | ((_len) & 0x00ff) )

/*! \brief Declare a characteristic */
#define GATT_DECL_ATTR_ADD(_attr, _flags, _add_flags, _len)     \
    ( (((uint16)(_attr) & 0x000f) << 12) |                      \
      (((uint16)(_flags) & 0x000f) << 8) |                      \
      (((uint16)(_add_flags) & 0x0003) << 6) | ((_len) & 0x003f) )
/* @} */

/*! \name Attribute Declarations
 *
 *  @{
 */

/*! \brief Primary Service 16-bit UUID (fixed length): 2 bytes UUID */
#define GATT_DECL_PRIM_SERV_UUID16(_flags, _uuid)       \
    GATT_DECL_ATTR(att_type_pri_service, _flags, 2),        \
        BYTE_SWAP_16(_uuid)

/*! \brief Primary Service 128-bit UUID (fixed length): 16 bytes UUID
 *
 *  NOTE: The UUID should be provided as 8 16-bit comma-separated values, with
 *        the Most Significant Word of the UUID placed first. The whole UUID
 *        will then be word- and byte-swapped
 */
#define GATT_DECL_PRIM_SERV_UUID128(_flags, _uuid1, _uuid2, _uuid3, _uuid4, _uuid5, _uuid6, _uuid7, _uuid8) \
    GATT_DECL_ATTR(att_type_pri_service, _flags, 16),        \
        BYTE_SWAP_16(_uuid8), BYTE_SWAP_16(_uuid7), BYTE_SWAP_16(_uuid6), BYTE_SWAP_16(_uuid5),     \
        BYTE_SWAP_16(_uuid4), BYTE_SWAP_16(_uuid3), BYTE_SWAP_16(_uuid2), BYTE_SWAP_16(_uuid1)

/*! \brief Secondary Service (fixed length): 2 bytes UUID */
#define GATT_DECL_SEC_SERV_UUID16(_flags, _uuid)        \
    GATT_DECL_ATTR(att_type_sec_service, _flags, 2),       \
        BYTE_SWAP_16(_uuid)

/*! \brief Include Service 16-bit UUID (fixed length):
 *  2 bytes include service handle, 2 bytes end group handle,
 *  2 bytes service UUID
 */
#define GATT_DECL_INCL_SERV_UUID16(_flags, _hndl, _end, _uuid)  \
    GATT_DECL_ATTR(att_type_include, _flags, 6),       \
        BYTE_SWAP_16(_hndl), BYTE_SWAP_16(_end), BYTE_SWAP_16(_uuid)

/*! \brief Include Service 128-bit UUID (fixed length):
 *  2 bytes include service handle, 2 bytes end group handle
 */
#define GATT_DECL_INCL_SERV_UUID128(_flags, _hndl, _end)  \
    GATT_DECL_ATTR(att_type_include, _flags, 4),       \
        BYTE_SWAP_16(_hndl), BYTE_SWAP_16(_end)

/*! \brief Characteristic Declaration 16-bit UUID (fixed length):
 *  1 byte properties, 2 bytes handle, 2 bytes UUID
 *
 *  \warning This macro is not intended to be used directly within the database
 *  definition. It is called from the GATT_DECL_CHAR16_FULL macro, which is used to
 *  create the Characteristic Declaration and Characteristic Value attributes in
 *  one step.
 */
#define GATT_DECL_CHAR16(_flags, _add_flags, _prop, _hndl, _uuid)       \
    GATT_DECL_ATTR_ADD(att_type_declaration, _flags, _add_flags, 5),    \
        ( ((uint16)WORD_LSB(_prop) << 8) | WORD_LSB(_hndl) ),           \
        ( ((uint16)WORD_MSB(_hndl) << 8) | WORD_LSB(_uuid) ),           \
        ( ((uint16)WORD_MSB(_uuid) << 8) )

/*! \brief Characteristic Value for 16-bit UUID (variable length):
 *  the application should supply the length, and is responsible for converting
 *  the following byte sequence to a set of 16-bit integers, e.g. using the
 *  BYTE_JOIN_16 macro. This macro is defined using a GCC variadic macro.
 *
 *  \warning This macro is not intended to be used directly within the database
 *  definition. It is called from the GATT_DECL_CHAR16_FULL macro, which is used to
 *  create the Characteristic Declaration and Characteristic Value attributes in
 *  one step.
 */
#define GATT_DECL_CHAR_VALUE16(_flags, _len, ...)       \
    GATT_DECL_ATTR(att_type_value, _flags, _len) , ##__VA_ARGS__  /* !! Leave space before , !! */

/*! \brief Characteristic Definition with 16bit UUID: At minimum a
 *  Characteristic definition includes a Characteristic Declaration immediately
 *  followed by a Characteristic Value. This macro creates both attributes in
 *  one go (and will thus "consume" two consecutive attribute handles).
 *
 *  \param _flags       Flags for the Characteristic Value:
 *                      #ATT_ATTR_IRQ
 *
 *  \param _read_sec    Read Security for the Characteristic Value:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param _write_sec   Write Security for the Characteristic Value:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param    _perm     GATT permissions for the Characteristic Value:
 *                      #ATT_PERM_CONFIGURE_BROADCAST,
 *                      #ATT_PERM_READ,
 *                      #ATT_PERM_WRITE_CMD,
 *                      #ATT_PERM_WRITE_REQ,
 *                      #ATT_PERM_NOTIFY,
 *                      #ATT_PERM_INDICATE,
 *                      #ATT_PERM_WRITE_SIGNED,
 *                      #ATT_PERM_EXTENDED
 *
 *  \param    _hndl     The handle of the Characteristic Value (16 bits).
 *                      Shall be the Characteristic Declaration handle + 1.
 *
 *  \param    _uuid     Characteristic Value 16-bit UUID.
 *
 *  \param    _len      Length of the Characteristic Value (in bytes).
 *
 *  \param    value     The application is responsible for converting
 *                      the 'value' byte sequence to a set of 16-bit integers,
 *                      e.g. using the #BYTE_JOIN_16 macro.
 */
#define GATT_DECL_CHAR16_FULL(_flags, _read_sec, _write_sec, _perm, _hndl,_uuid,_len, ...)   \
    GATT_DECL_CHAR16(ATT_ATTR_NO_FLAG, _read_sec, _perm, _hndl, _uuid),            \
    GATT_DECL_CHAR_VALUE16((_flags|_write_sec), _len , ##__VA_ARGS__ )

/*! \brief Characteristic Declaration 128-bit UUID (fixed length):
 *  1 byte properties, 2 bytes handle, 16 bytes UUID
 *
 *  \warning This macro is not intended to be used directly within the database
 *  definition. It is called from the GATT_DECL_CHAR128 macro, which is used to
 *  create the Characteristic Declaration and Characteristic Value attributes in
 *  one step.
 */
#define GATT_DECL_CHAR128(_flags, _add_flags, _prop, _hndl,             \
                               _uuid1, _uuid2, _uuid3, _uuid4, _uuid5, _uuid6, _uuid7, _uuid8)     \
    GATT_DECL_ATTR_ADD(att_type_declaration, _flags, _add_flags, 19),   \
        ( ((uint16)WORD_LSB(_prop)  << 8) | WORD_LSB(_hndl)  ), \
        ( ((uint16)WORD_MSB(_hndl)  << 8) | WORD_LSB(_uuid8) ), \
        ( ((uint16)WORD_MSB(_uuid8) << 8) | WORD_LSB(_uuid7) ), \
        ( ((uint16)WORD_MSB(_uuid7) << 8) | WORD_LSB(_uuid6) ), \
        ( ((uint16)WORD_MSB(_uuid6) << 8) | WORD_LSB(_uuid5) ), \
        ( ((uint16)WORD_MSB(_uuid5) << 8) | WORD_LSB(_uuid4) ), \
        ( ((uint16)WORD_MSB(_uuid4) << 8) | WORD_LSB(_uuid3) ), \
        ( ((uint16)WORD_MSB(_uuid3) << 8) | WORD_LSB(_uuid2) ), \
        ( ((uint16)WORD_MSB(_uuid2) << 8) | WORD_LSB(_uuid1) ), \
        ( ((uint16)WORD_MSB(_uuid1) << 8) /* Trailing byte */)

/*! \brief Characteristic Value for 128-bit UUID (variable length):
 *  the application should supply the length, and is responsible for converting
 *  the following byte sequence to a set of 16-bit integers, e.g. using the
 *  BYTE_JOIN_16 macro. This macro is defined using a GCC variadic macro.
 *
 *  \warning This macro is not intended to be used directly within the database
 *  definition. It is called from the GATT_DECL_CHAR128 macro, which is used to
 *  create the Characteristic Declaration and Characteristic Value attributes in
 *  one step.
 */
#define GATT_DECL_CHAR_VALUE128(_flags, _len, ...)         \
    GATT_DECL_ATTR(att_type_value128, _flags, _len) , ##__VA_ARGS__  /* !! Leave space before , !! */

/*! \brief Characteristic Definition with 128bits UUID: At minimum a
 *  Characteristic definition includes a Characteristic Declaration immediately
 *  followed by a Characteristic Value. This macro creates both attributes in
 *  one go (and will thus "consume" two consecutive attribute handles).
 *
 *  \param _flags       Flags for the Characteristic Value:
 *                      #ATT_ATTR_IRQ
 *
 *  \param _read_sec    Read Security for the Characteristic Value:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param _write_sec   Write Security for the Characteristic Value:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param    _perm     GATT permissions for the Characteristic Value:
 *                      #ATT_PERM_CONFIGURE_BROADCAST,
 *                      #ATT_PERM_READ,
 *                      #ATT_PERM_WRITE_CMD,
 *                      #ATT_PERM_WRITE_REQ,
 *                      #ATT_PERM_NOTIFY,
 *                      #ATT_PERM_INDICATE,
 *                      #ATT_PERM_WRITE_SIGNED,
 *                      #ATT_PERM_EXTENDED
 *
 *  \param    _hndl     The handle of the Characteristic Value (16 bits).
 *                      Shall be the Characteristic Declaration handle + 1.
 *
 *  \param    _uuid1-8  Characteristic Value 128-bit UUID.
 *                      The UUID should be provided as 8 16-bit comma-separated
 *                      values, with the Most Significant Word of the UUID
 *                      placed first. The whole UUID will then be word- and
 *                      byte-swapped, and properly aligned (because the length
 *                      is odd, the UUID starts half-way through a 16-bit word)
 *
 *  \param    _len      Length of the Characteristic Value (in bytes).
 *
 *  \param    value     The application is responsible for converting
 *                      the 'value' byte sequence to a set of 16-bit integers,
 *                      e.g. using the #BYTE_JOIN_16 macro.
 */
#define GATT_DECL_CHAR128_FULL(_flags, _read_sec, _write_sec, _perm, _hndl,     \
                          _uuid1, _uuid2, _uuid3, _uuid4,                       \
                          _uuid5, _uuid6, _uuid7, _uuid8,                       \
                          _len, ...)                                            \
    GATT_DECL_CHAR128(ATT_ATTR_NO_FLAG, _read_sec, _perm, _hndl,                \
                      _uuid1, _uuid2, _uuid3, _uuid4, _uuid5, _uuid6, _uuid7, _uuid8),   \
    GATT_DECL_CHAR_VALUE128((_flags|_write_sec), _len , ##__VA_ARGS__ )

/*! \brief Characteristic Extended Properties (fixed length):
 *  2 bytes extended properties
 */
#define GATT_DECL_CHAR_EXT_PROPS(_flags, _props)        \
    GATT_DECL_ATTR(att_type_ch_extended, _flags, 2),    \
        BYTE_SWAP_16(_props)

/*! \brief Characteristic User Description (variable length):
 *  the application should supply the length, and is responsible for converting
 *  the following byte sequence to a set of 16-bit integers, e.g. using the
 *  BYTE_JOIN_16 macro. This macro is defined using a GCC variadic macro.
 */
#define GATT_DECL_CHAR_USER_DESC(_flags, _len, ...)         \
    GATT_DECL_ATTR(att_type_ch_descr, _flags, _len) , ##__VA_ARGS__  /* !! Leave space before , !! */

/*! \brief Client Characteristic Configuration (fixed length):
 *  2 bytes configuration flags (bitfield)
 */
#define GATT_DECL_CHAR_CLIENT_CFG(_attr_flags, _flags)    \
    GATT_DECL_ATTR(att_type_ch_c_config, _attr_flags, 2), BYTE_SWAP_16(_flags)

/*! \brief Server Characteristic Configuration (fixed length):
 *  2 bytes configuration flags (bitfield)
 */
#define GATT_DECL_CHAR_SERVER_CFG(_attr_flags, _flags)    \
    GATT_DECL_ATTR(att_type_ch_s_config, _attr_flags, 2), BYTE_SWAP_16(_flags)

/*! \brief Characteristic Format (fixed length):
 *  1 byte format, 1 byte exponent, 2 bytes unit, 1 byte name space,
 *  2 bytes description
 */
#define GATT_DECL_CHAR_FORMAT(_flags, _fmt, _exp, _unit, _ns, _desc)        \
    GATT_DECL_ATTR(att_type_ch_format, _flags, 7),                          \
        BYTE_JOIN_16(_fmt, _exp), BYTE_SWAP_16(_unit),              \
        BYTE_JOIN_16(_ns, WORD_LSB(_desc)), BYTE_JOIN_16(WORD_MSB(_desc), 0)

/*! \brief Characteristic Aggregated Format:
 *
 *  \param  _flags      Flags for the Characteristic Value:
 *                      #ATT_ATTR_IRQ
 * *
 *  \param  _num        Number of handles in the handleList
 *
 *  \param  handleList  List of handles of which the aggregated
 *                      format is constructed
 */
#define GATT_DECL_CHAR_AGG_FORMAT(_flags, _num, ...)        \
    GATT_DECL_ATTR(att_type_ch_agg, _flags, _num*2) , ##__VA_ARGS__  /* !! Leave space before , !! */




/*! \brief Generic Attribute (16-bit UUID):
 *  2 bytes UUID, 1 word permissions, variable-length data
 *
 *  \warning It is not advised to use this macro to declare database attributes
 *  that can be specified using the macros above, as the generated attribute
 *  will take up more space in the ATT database.
 *
 *  \param _flags       Flags for the Attribute:
 *                      #ATT_ATTR_IRQ
 *
 *  \param _read_sec    Read Security for the Attribute:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param _write_sec   Write Security for the Attribute:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param    _perm     GATT permissions for the Attribute:
 *                      #ATT_PERM_CONFIGURE_BROADCAST,
 *                      #ATT_PERM_READ,
 *                      #ATT_PERM_WRITE_CMD,
 *                      #ATT_PERM_WRITE_REQ,
 *                      #ATT_PERM_NOTIFY,
 *                      #ATT_PERM_INDICATE,
 *                      #ATT_PERM_WRITE_SIGNED,
 *                      #ATT_PERM_EXTENDED
 *
 *  \param    _uuid     Attribute 16-bit UUID.
 *
 *  \param    _len      Length of the Attribute data (in bytes).
 *
 *  \param    value     The application is responsible for converting
 *                      the 'value' byte sequence to a set of 16-bit integers,
 *                      e.g. using the #BYTE_JOIN_16 macro.
 */
#define GATT_DECL_FULL(_flags, _read_sec, _write_sec, _uuid, _perm, _len, ...)  \
    GATT_DECL_ATTR(att_type_full, (_flags | _write_sec), _len),                 \
        _uuid, \
        (_read_sec<<14 | _perm) , ##__VA_ARGS__  /* !! Leave space before , !! */

/*! \brief Generic Attribute (128-bit UUID):
 *  16 bytes UUID, 1 word permissions, variable-length data
 *
 *  \warning It is not advised to use this macro to declare database attributes
 *  that can be specified using the macros above, as the generated attribute
 *  will take up more space in the ATT database.
 *
 *  \param _flags       Flags for the Attribute:
 *                      #ATT_ATTR_IRQ
 *
 *  \param _read_sec    Read Security for the Attribute:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param _write_sec   Write Security for the Attribute:
 *                      #ATT_ATTR_SEC_NONE,
 *                      #ATT_ATTR_SEC_ENCRYPTION,
 *                      #ATT_ATTR_SEC_AUTHENTICATION
 *
 *  \param    _perm     GATT permissions for the Attribute:
 *                      #ATT_PERM_CONFIGURE_BROADCAST,
 *                      #ATT_PERM_READ,
 *                      #ATT_PERM_WRITE_CMD,
 *                      #ATT_PERM_WRITE_REQ,
 *                      #ATT_PERM_NOTIFY,
 *                      #ATT_PERM_INDICATE,
 *                      #ATT_PERM_WRITE_SIGNED,
 *                      #ATT_PERM_EXTENDED
 *
 *  \param    _uuid1-8  Attribute 128-bit UUID.
 *                      The UUID should be provided as 8 16-bit comma-separated
 *                      values, with the Most Significant Word of the UUID
 *                      placed first. The UUID will then be packed into its
 *                      internal 32-bit representation.
 *
 *  \param    _len      Length of the Attribute (in bytes).
 *
 *  \param    value     The application is responsible for converting
 *                      the 'value' byte sequence to a set of 16-bit integers,
 *                      e.g. using the #BYTE_JOIN_16 macro.
 */
#define GATT_DECL_FULL128(_flags, _read_sec, _write_sec,                                    \
                          _uuid1, _uuid2, _uuid3, _uuid4, _uuid5, _uuid6, _uuid7, _uuid8,   \
                          _prop, _len, ...)                                                 \
    GATT_DECL_ATTR(att_type_full128, _flags, _len),                                         \
        _uuid1, _uuid2, _uuid3, _uuid4, _uuid5, _uuid6, _uuid7, _uuid8,                     \
        (_read_sec<<14 | _prop) , ##__VA_ARGS__  /* !! Leave space before , !! */


/*! \brief Meta-Attribute to pad handle counter, allowing gaps to be left for
 *  expansion in the database during development, or to allow us to use the
 *  recommended handle numbering specified for qualification test cases.
 *
 *  NOTE: This entry in the database is NOT an attribute and will not be
 *  discoverable by remote clients OR by local applications using the APIs to
 *  query the database.
 *
 *  \warning Padding meta-attributes are only intended to be placed *between*
 *  services. Placing them within the definition of a service is not supported
 *  and may lead to invalid behaviour.
 *
 *  2 bytes pad length
 */
#define GATT_DECL_HANDLE_PAD(_pad)      \
    GATT_DECL_ATTR(att_type_handle_padding, 0 /* No flags */, 2), _pad

/* @} */

/* @} */

#endif  /* ifndef __GATT_UUID_H__ */
