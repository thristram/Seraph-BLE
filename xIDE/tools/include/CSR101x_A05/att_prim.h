/*!
   Copyright (c) 2009 - 2012 Qualcomm Technologies International, Ltd.
   All Rights Reserved.
   Qualcomm Technologies International, Ltd. Confidential and Proprietary.

\file   att_prim.h

\brief  Attribute Protocol application interface
*/
#ifndef _ATT_PRIM_H_
#define _ATT_PRIM_H_

#include "bluetooth.h"
#include "bt_event_types.h"

/*!
    \name Generic Attribute Profile properties
    
    \brief The Characteristic Properties bit field determines how the
    Characteristic Value can be used, or how the characteristic descriptors
    can be accessed.
    
    \{
*/
/* FIXME: change name to PROP */
/*! If set, permits broadcasts of the Characteristic Value using
  Characteristic Configuration Descriptor. */
#define ATT_PERM_CONFIGURE_BROADCAST            0x01
/*! If set, permits reads of the Characteristic Value. */
#define ATT_PERM_READ                           0x02
/*! If set, permit writes of the Characteristic Value without response. */
#define ATT_PERM_WRITE_CMD                      0x04
/*! If set, permits writes of the Characteristic Value with response. */
#define ATT_PERM_WRITE_REQ                      0x08
/*! If set, permits notifications of a Characteristic Value without
    acknowledgement. */
#define ATT_PERM_NOTIFY                         0x10
/*! If set, permits indications of a Characteristic Value with
    acknowledgement. */
#define ATT_PERM_INDICATE                       0x20
/*! If set, permits signed writes to the Characteristic Value. */
#define ATT_PERM_WRITE_SIGNED                   0x40
/*! If set, additional characteristic properties are defined in the
    Characteristic Extended Properties Descriptor. */
#define ATT_PERM_EXTENDED                       0x80
/*! This constant is deprecated. Please use #ATT_PERM_WRITE_SIGNED instead. */
#define ATT_PERM_AUTHENTICATED                  ATT_PERM_WRITE_SIGNED
/*! \} */

/*!
    \name Generic Attribute Profile extended properties
    
    \brief The Characteristic Extended Properties bit field describes
    additional properties on how the Characteristic Value can be used,
    or how the characteristic descriptors can be accessed.
    
    \{
*/
/*! If set, permits reliable writes of the Characteristic Value. */
#define ATT_PERM_RELIABLE_WRITE                 0x0001
/*! If set, permits writes to the characteristic descriptor. */
#define ATT_PERM_WRITE_AUX                      0x0002
/*! \} */

/*!
    \name UUID Types

    \{
*/
/*! UUID Type */
typedef uint16                                att_uuid_type_t;
/*! No UUID present. */
#define ATT_UUID_NONE                           0x0000
/*! UUID is a 16-bit Attribute UUID */
#define ATT_UUID16                              0x0001
/*! UUID is a 128-bit UUID */
#define ATT_UUID128                             0x0002
/* \} */

/*! \brief Flat DB attribute types */
typedef enum att_type_tag
{
    /* GATT Attribute Types */
    att_type_pri_service,       /*!< 0 Primary Service */
    att_type_sec_service,       /*!< 1 Secondary service */
    att_type_include,           /*!< 2 Include */
    att_type_declaration,       /*!< 3 Characteristic Declaration */
    att_type_ch_extended,       /*!< 4 Characteristic Extended Properties */
    att_type_ch_descr,          /*!< 5 Characteristic User Description */
    att_type_ch_c_config,       /*!< 6 Client Characteristic Configuration */
    att_type_ch_s_config,       /*!< 7 Server Characteristic Configuration */
    att_type_ch_format,         /*!< 8 Characteristic Format */
    att_type_ch_agg,            /*!< 9 Characteristic Aggregate Format */
    att_type_reserved_a,        /*!< a unused */
    att_type_handle_padding,    /*!< b Meta-Attribute to pad handle count */

    att_type_value128,          /*!< c Characteristic value */
    att_type_value,             /*!< d Characteristic value */

    /* full attribute data */
    att_type_full,              /*!< e Generic Attribute */
    att_type_full128            /*!< f Generic Attribute */
} att_type_t;

/*! \brief Full attribute type with 16-bit UUID (att_type_full) */
typedef struct 
{
    uint16    uuid;           /*!< UUID16 */
    uint16    perm;           /*!< Attribute permissions */
    uint16    data[1];        /*!< Attribute value */
} att_attr_full_t;

/*! \brief Full attribute type with 128-bit UUID (att_type_full128) */
typedef struct 
{
    uint32    uuid[4];        /*!< UUID128 */
    uint16    perm;           /*!< Attribute permissions */
    uint16    data[1];        /*!< Attribute value */
} att_attr_full128_t;
    
/*! \brief Attribute length can be changed. */
#define ATT_ATTR_DYNLEN         0x8
/*! \brief Access to the attribute sends ATT_ACCESS_IND to the application. */
#define ATT_ATTR_IRQ            0x4

#define ATT_ATTR_SEC_R_OFFS     6       /* 0x00c0 */
#define ATT_ATTR_SEC_W_OFFS     0       /* 0x0003 */
#define ATT_ATTR_SEC_MASK       0x3

/* macros for setting security flags */
#define ATT_ATTR_SEC_R_ENUM(value)      ((uint16)((value) & ATT_ATTR_SEC_MASK) << ATT_ATTR_SEC_R_OFFS)
#define ATT_ATTR_SEC_W_ENUM(value)      ((uint16)((value) & ATT_ATTR_SEC_MASK) << ATT_ATTR_SEC_W_OFFS)

/* macros for extracting security flags */
#define ATT_ATTR_SEC_R_EXTRACT_ENUM(value)      ((uint16)((value) >> ATT_ATTR_SEC_R_OFFS) & ATT_ATTR_SEC_MASK)
#define ATT_ATTR_SEC_W_EXTRACT_ENUM(value)      ((uint16)((value) >> ATT_ATTR_SEC_W_OFFS) & ATT_ATTR_SEC_MASK)

/*! \name Special security requirement options
  \{ */
typedef enum
{
    /*! \brief No security requirements */
    ATT_ATTR_SEC_NONE,
    /*! \brief Encrypted link is required for access. */
    ATT_ATTR_SEC_ENCRYPTION,
    /*! \brief Authenticated MITM protection is required for access. */
    ATT_ATTR_SEC_AUTHENTICATION,
    
    ATT_ATTR_SEC_RESERVED
} ATT_ATTR_SEC_T;
/*! \} */

/*! \brief Access to an attribute need Authorization from the application */
#define ATT_ATTR_AUTHORIZATION          0x0100
/*! \brief Access to an attribute will ask the application about the key requirements */
#define ATT_ATTR_ENC_KEY_REQUIREMENTS   0x0200
/* The following two attribute values are reserved on CSR1000 */
#define ATT_ATTR_RESERVED1              0x0400
#define ATT_ATTR_RESERVED2              0x0800

/*!
    \name Flags for Write Request
    \{
*/
/*! Send Write Request to the server */
#define ATT_WRITE_REQUEST               0x0000
/*! Send Write Command to the server. */
#define ATT_WRITE_COMMAND               0x0040
/*! Send Signed Write to the server. Only Write Command can be signed. */
#define ATT_WRITE_SIGNED                0x0080
/* \} */

/*!
    \name Flags for Access Indication
    \{
*/
/*! Read in progress. */
#define ATT_ACCESS_READ                 0x0001
/*! Write in progress. */
#define ATT_ACCESS_WRITE                0x0002
/*! An Access Response is required to grant access. */
#define ATT_ACCESS_PERMISSION           0x8000
/*! An Access Response is required to accept value(s) written. */
#define ATT_ACCESS_WRITE_COMPLETE       0x4000
/* \} */

/*
    \name Execute Write Request flags
    \{
*/
/*! Cancel all pending prepared writes */
#define ATT_EXECUTE_CANCEL              0x0000
/*! Immediately write all pending prepared values */
#define ATT_EXECUTE_WRITE               0x0001
/*! \} */

/*!
    \name ATT Handle definitions
    \{
*/
/*! Handle 0 is defined as invalid */
#define ATT_HANDLE_INVALID              0x0000
/*! Handle 0xFFFF is defined as the maximum */
#define ATT_HANDLE_MAX                  0xFFFF
/*! \} */

/*!
    \name Handle Value flags
    \{
*/
/*!
    \brief Send Notification to the client.
*/
#define ATT_HANDLE_VALUE_NOTIFICATION   0x0000
/*!
    \brief Send Indication to the client.
*/
#define ATT_HANDLE_VALUE_INDICATION     0x0001
/* \} */

#endif /* _ATT_PRIM_H_ */
