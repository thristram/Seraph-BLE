/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file time_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh Time model
 */
/******************************************************************************/

#ifndef __TIME_SERVER_H__
#define __TIME_SERVER_H__

/*! \addtogroup Time_Server
 * @{
 */
#include "time_model.h"

/*============================================================================*
    Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * TimeModelInit
 */
/*! \brief Model Initialisation
 *  
 *   Registers the model handler with the CSRmesh. Sets the CSRmesh to report
 *   num_groups as the maximum number of groups supported for the model
 *
 *  \param nw_id Identifier of the network to which the model has to be
                 registered.
 *  \param group_id_list Pointer to a uint16 array to hold assigned group_ids. 
 *                       This must be NULL if no groups are supported
 *  \param num_groups Size of the group_id_list. This must be 0 if no groups
 *                    are supported.
 *  \param app_callback Pointer to the application callback function. This
 *                    function will be called to notify all model specific messages
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TimeModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * TimeState
 */
/*! \brief Set time broadcast interval
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TIME_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TimeState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TIME_STATE_T *p_params);

/*----------------------------------------------------------------------------*
 * TimeBroadcast
 */
/*! \brief Synchronise wall clock time from client device: This message is always sent with TTL=0. This message is sent at intervals by the clock master. It is always sent with TTL=0. It is repeated, but the time is updated before each repeat is sent. The clock master repeats the message 5 times, relaying stations repeat it 3 times. When a node receives a clock broadcast its behaviour depends on the current clock state: n MASTER: Ignore broadcasts.n INIT: Start the clock; relay this message. Set state to NO_RELAY if MasterFlag set, otherwise RELAY_MASTER. Start relay timer.n RELAY: Correct clock if required. Relay this message. Set state to NO_RELAY if MasterFlag set, otherwise RELAY_MASTER. Start relay timer.n NO_RELAY: Ignore. State will be reset to RELAY when the relay timer goes off.n  RELAY_MASTER: Relay message only if it is from the clock master and set state to NO_RELAY.n The relay timer is by default 1/4 of the clock broadcast interval (15 seconds if the interval is 60 seconds). This means that each node will relay a message only once, and will give priority to messages from the clock master (which always causes the clock to be corrected). Messages from other nodes will only cause clock correction if they exceed the max clock skew (250ms).
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TIME_BROADCAST_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TimeBroadcast(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TIME_BROADCAST_T *p_params);

/*!@} */
#endif /* __TIME_SERVER_H__ */

