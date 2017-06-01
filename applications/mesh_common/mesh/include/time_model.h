/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Time_model.h
 *
 *  \brief Defines CSRmesh Time Model specific data structures\n
 */
/******************************************************************************/

#ifndef __TIME_MODEL_H__
#define __TIME_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Time_Model 
 *  \brief Time Model API
 *
 * <br>The Time Model is used to distribute coordinated universal time, and time zone information, within a mesh network.<h4>Time State</h4> The time model has the following states:<ul style='list-style:square;list-style-position: inside;'><li style='list-style-type:square;'>CurrentTime</li><li style='list-style-type:square;'>TimeInterval</li><li style='list-style-type:square;'>TimeZone</li></ul>  <h5>CurrentTime</h5>The current time state is the number of milliseconds that have passed since midnight on January 1st, 1970 at the Zulu time zone. The current time is represented as a 48-bit unsigned integer; it is not possible to represent any time before 1970.<h5> TimeInterval</h5> The time interval state is the number of seconds between time broadcasts. This is represented in seconds as a 16-bit unsigned integer.<h5> Time Zone</h5>The time zone state is the number of 15-minute increments from UTC. This is an 8-bit signed integer.<h4> Time Background Behaviour</h4> If the TimeInterval state value is set to a non-zero value, then the server transmits a TIME_BROADCAST message approximately every TimeInterval seconds. The TTL is set to 0. The message is repeated,  but each repeat uses the current value of the clock. The MasterFlag is set to 1 if this message is sent by the clock master, or set to 0 if this message is being relayed by another node.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Time Model message types */

/*! \brief Setting Time Broadcast Interval: Upon receiving a TIME_SET_STATE message, the device saves the TimeInterval field into the appropriate state value. It then responds with a TIME_STATE message with the current state information. */
typedef struct
{
    CsrUint16 interval; /*!< \brief Interval between time broadcasts */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TIME_SET_STATE_T;

/*! \brief Getting Time Broadcast Interval: Upon receiving a TIME_GET_STATE message, the device responds with a TIME_STATE message with the current state information. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TIME_GET_STATE_T;

/*! \brief Set time broadcast interval */
typedef struct
{
    CsrUint16 interval; /*!< \brief Interval between time broadcasts */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TIME_STATE_T;

/*! \brief Synchronise wall clock time from client device: This message is always sent with TTL=0. This message is sent at intervals by the clock master. It is always sent with TTL=0. It is repeated, but the time is updated before each repeat is sent. The clock master repeats the message 5 times, relaying stations repeat it 3 times. When a node receives a clock broadcast its behaviour depends on the current clock state: n MASTER: Ignore broadcasts.n INIT: Start the clock; relay this message. Set state to NO_RELAY if MasterFlag set, otherwise RELAY_MASTER. Start relay timer.n RELAY: Correct clock if required. Relay this message. Set state to NO_RELAY if MasterFlag set, otherwise RELAY_MASTER. Start relay timer.n NO_RELAY: Ignore. State will be reset to RELAY when the relay timer goes off.n  RELAY_MASTER: Relay message only if it is from the clock master and set state to NO_RELAY.n The relay timer is by default 1/4 of the clock broadcast interval (15 seconds if the interval is 60 seconds). This means that each node will relay a message only once, and will give priority to messages from the clock master (which always causes the clock to be corrected). Messages from other nodes will only cause clock correction if they exceed the max clock skew (250ms). */
typedef struct
{
    CsrUint8 currenttime[6/size_in_bytes(CsrUint8)]; /*!< \brief Current local time in milliseconds from 1970-01-01 00:00Z */
    CsrInt8 timezone; /*!< \brief Time zone offset of current time from UTC (in 15 minute increments) */
    CsrUint8 masterclock; /*!< \brief 1 if sent by clock master, 0 if not */
} CSRMESH_TIME_BROADCAST_T;


/*!@} */
#endif /* __TIME_MODEL_H__ */

