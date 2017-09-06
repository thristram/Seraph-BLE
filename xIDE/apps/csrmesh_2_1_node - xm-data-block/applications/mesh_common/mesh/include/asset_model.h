/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Asset_model.h
 *
 *  \brief Defines CSRmesh Asset Model specific data structures\n
 */
/******************************************************************************/

#ifndef __ASSET_MODEL_H__
#define __ASSET_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Asset_Model 
 *  \brief Asset Model API
 *
 * <br>The Asset Model announces the presence of an asset within a mesh network.<h4>Asset State</h4>The Asset model has the following states:<ul style='list-style:square;list-style-position: inside;'><li style='list-style-type:square;'>Interval</li> <li style='list-style-type:square;'>SideEffects</li> <li style='list-style-type:square;'>ToDestinationID</li></ul><h5>Interval Variable</h5>The Interval variable is an unsigned 16-bit integer in seconds representing times from 1 second to 65535 seconds. The value 0x0000 means that the asset will never transmit an asset broadcast message. Some devices will not be able to support every possible Interval value and therefore their Interval value will be rounded to the nearest supported Interval. The rounding of Interval is implementation specific and not specified further. <h5>SideEffects</h5>The SideEffects variable is a 16-bit bit field that represents one or more possible side-effects that this asset would like to be observed. The following side effect bits are defined:<ul style='list-style:square;list-style-position: inside;'><li style='list-style-type:square;'>Light=0</li> <li style='list-style-type:square;'> Audio=1</li> <li style='list-style-type:square;'>Movement=2</li> <li style='list-style-type:square;'>Stationary=3</li> <li style='list-style-type:square;'>Human=4 </li></ul> <h5>ToDestionation</h5>ToDestionation state is a GroupID that all asset broadcast messages are sent to. <H4>Asset Background Behaviour</h4>If the Interval state has a non-zero value then the device sends an ASSET_ANNOUNCE message once every Interval to the ToDestinationID with the SideEffects value. This message is sent with TTL 0, repeated Ã¢â‚¬ËœnumberOfAnnouncesÃ¢â‚¬â„¢ times with a gap of Ã¢â‚¬ËœannounceIntervalÃ¢â‚¬â„¢ between each repeat.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Asset Model message types */

/*! \brief Setting Asset State: Upon receiving an ASSET_SET_STATE message, the device saves the Interval, SideEffects, ToDestination, TxPower, Number of Announcements and AnnounceInterval  fields into the appropriate state values. It then responds with an ASSET_STATE message with the current state information. */
typedef struct
{
    CsrUint16 interval; /*!< \brief Interval between asset broadcasts */
    CsrUint16 sideeffects; /*!< \brief Side effects for this asset */
    CsrUint16 todestinationid; /*!< \brief Asset announce destination */
    CsrInt8 txpower; /*!< \brief Asset TxPower */
    CsrUint8 numannounces; /*!< \brief Number of times to send announce */
    CsrUint8 announceinterval; /*!< \brief Time between announce repeats */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ASSET_SET_STATE_T;

/*! \brief Getting Asset State: Upon receiving an ASSET_GET_STATE message, the device responds with an ASSET_STATE message with the current state information. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ASSET_GET_STATE_T;

/*! \brief Current asset state */
typedef struct
{
    CsrUint16 interval; /*!< \brief Interval between asset broadcasts */
    CsrUint16 sideeffects; /*!< \brief Side effects for this asset */
    CsrUint16 todestinationid; /*!< \brief Asset announce destination */
    CsrInt8 txpower; /*!< \brief Asset TxPower */
    CsrUint8 numannounces; /*!< \brief Number of times to send announce */
    CsrUint8 announceinterval; /*!< \brief Time between announce repeats */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_ASSET_STATE_T;

/*! \brief Asset announcement */
typedef struct
{
    CsrInt8 txpower; /*!< \brief Asset TxPower */
    CsrUint16 interval; /*!< \brief Interval between asset broadcasts */
    CsrUint16 sideeffects; /*!< \brief Side effects for this asset */
} CSRMESH_ASSET_ANNOUNCE_T;


/*!@} */
#endif /* __ASSET_MODEL_H__ */

