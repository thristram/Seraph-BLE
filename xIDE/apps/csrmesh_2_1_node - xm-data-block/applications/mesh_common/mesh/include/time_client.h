/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Time_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Time model 
 */
/******************************************************************************/

#ifndef __TIME_CLIENT_H__
#define __TIME_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Time_Client
 * @{
 */
#include "time_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * TimeModelClientInit
 */
/*! \brief 
 *      Initialises Time Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TimeModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * TimeSetState
 */
/*! \brief Setting Time Broadcast Interval: Upon receiving a TIME_SET_STATE message, the device saves the TimeInterval field into the appropriate state value. It then responds with a TIME_STATE message with the current state information.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_TIME_STATE 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TIME_SET_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TimeSetState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TIME_SET_STATE_T *p_params );

/*----------------------------------------------------------------------------*
 * TimeGetState
 */
/*! \brief Getting Time Broadcast Interval: Upon receiving a TIME_GET_STATE message, the device responds with a TIME_STATE message with the current state information.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_TIME_STATE 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TIME_GET_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TimeGetState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TIME_GET_STATE_T *p_params );

/*!@} */
#endif /* __TIME_CLIENT_H__ */

