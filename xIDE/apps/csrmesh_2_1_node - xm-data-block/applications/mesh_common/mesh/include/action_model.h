/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Action_model.h
 *
 *  \brief Defines CSRmesh Action Model specific data structures\n
 */
/******************************************************************************/

#ifndef __ACTION_MODEL_H__
#define __ACTION_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Action_Model 
 *  \brief Action Model API
 *
 * 
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Action Model message types */

/*! \brief This message defines the action which needs to be taken. This is a multiple parts message, each part will be acknowledged through an ACTION_SET_ACK (0x51). */
typedef struct
{
    CsrUint8 actionid; /*!< \brief Unique Action Identifier. Action Identifier is of size 5 bits, Hence Max of 31 actions */
    CsrUint8 mcp_type; /*!< \brief b’G0000000’, ‘G’ is 0 if the ACTION_SET describes an action to be SET into the destination ID. ‘G’ is 1 if this message is issued by a node as a response to an ACTION_GET */
    CsrUint16 actiondestid; /*!< \brief The destination id for which the action needs to be transmitted */
    CsrUint8 timetype; /*!< \brief Time type 0x02: Absolute time in seconds from 1st January 2015 – not recurring, 0x03: Absolute time in seconds from 1st January 2015 – recurring. The relative time (3 bytes) has to be provided – this will be used to express the number of second to wait after each triggering, 0x04: Relative time in seconds from the point of reception - first field (4 bytes) will be used to express time. Not recurring, 0x05: Relative time in seconds from the point of reception - first field (4 bytes) will be used to express time. recurring. The relative time (3 bytes) has to be provided – this will be used to express the number of second to wait after each triggering */
    CsrUint32 starttime; /*!< \brief The start time at which the action must be triggerred */
    CsrUint24 repeattime;/*!< \brief The repeat time in seconds after which the action must be triggerred repeatedly */
    CsrUint16 noofrepeat; /*!< \brief The number of times the action must be repeated. The value of 0xFFFF makes the actions repeat forever */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_SET_ACTION_INFO_T;

/*! \brief This message defines the action to be sent when GET_ACTION is received. This is a multiple parts message, each part will be acknowledged through an ACTION_SET_ACK (0x51).This also contains the buffer to send the action message through it. */
typedef struct
{
    CSRMESH_ACTION_SET_ACTION_INFO_T set_action; /*!< \brief Set Action strcture containing the action information to be filled.  */
    uint8 buf[11]; /*!< \brief The buffer to store the action.*/
    uint8 buf_size; /*!< \brief The size of the action buffer stored.*/
} CSRMESH_ACTION_SET_ACTION_INFO_EXT_T;

/*! \brief This message defines the action which needs to be taken. This is a multiple parts message, each part will be acknowledged through an ACTION_SET_ACK (0x51). */
typedef struct
{
    CsrUint8 actionid; /*!< \brief Binary: b'PPCAAAAA' : PP part number (max of 4), C is 1 if this is the last part, AAAAA 5bits of Action Identifier. */
    CsrUint8 payload[8/size_in_bytes(CsrUint8)]; /*!< \brief Structured payload */
    CsrUint8 payload_len; /*!< \brief Length of the payload field */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_SET_ACTION_T;

/*! \brief Every parts received is acknowledged, providing both Transaction ID and ActionID (which includes Part Number) */
typedef struct
{
    CsrUint8 actionid; /*!< \brief  */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_SET_ACTION_ACK_T;

/*! \brief Request towards a node about the various Actions currently handled. This will be answered through an ACTION_STATUS. A Transaction ID is provided to ensure identification of response with request. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_GET_ACTION_STATUS_T;

/*! \brief This provides a bitmask identifying all the currently allocated ActionID. The Action Supported field provides an indication on how many actions this device has capacity for. Note that a device will never have more than 32 actions. */
typedef struct
{
    CsrUint32 actionids; /*!< \brief Bit mask indicating which Actions are stored. The bit position (for clarity 00000000 00000000 00000000 00000001 = actionID #1 only) determines the Action ID (from 1 to 31) */
    CsrUint8 maxactionssupported; /*!< \brief Max number of  actions supported by a Node device */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_ACTION_STATUS_T;

/*! \brief This message allows a set of actions to be removed. Actions are identified through a bitmask reflecting the ActionID one wishes to delete. This message will be acknowledged through ACTION_DELETE_ACK. */
typedef struct
{
    CsrUint32 actionids; /*!< \brief Bit mask indicating which Action to be deleted. The bit position (for clarity 00000000 00000000 00000000 00000001 = actionID #1 only) determines the Action ID (from 1 to 31) */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_DELETE_T;

/*! \brief This message confirms the delete commands and report which actions have been removed. The provided transaction ID will match the one supplied by ACTION_DELETE. */
typedef struct
{
    CsrUint32 actionids; /*!< \brief bit mask indicating which Action which have been deleted. The bit position (LSB = actionID #1) determines the Action ID (from 1 to 31). */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_DELETE_ACK_T;

/*! \brief Using this message, it is possible to interrogate a node about the specific payload associated with an Action ID. This will be answered by and ACTION_SET_ACTION message, ACTION_SET_ACTION_ACK will need to be issued in order to collect all parts. */
typedef struct
{
    CsrUint8 actionid; /*!< \brief  */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ACTION_GET_T;


/*!@} */
#endif /* __ACTION_MODEL_H__ */

