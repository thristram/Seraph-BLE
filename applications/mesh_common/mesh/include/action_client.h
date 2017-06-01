/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Action_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Action model 
 */
/******************************************************************************/

#ifndef __ACTION_CLIENT_H__
#define __ACTION_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Action_Client
 * @{
 */
#include "action_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * ActionModelClientInit
 */
/*! \brief 
 *      Initialises Action Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);

/*----------------------------------------------------------------------------*
 * ActionClientPrepareAction
 */
/*! \brief To set an action on a node the following functions should be called in sequence. ActionClientPrepareAction() followed by the model API whose action need to be set followed by ActionClientSetAction(). ActionClientPrepareAction() function indicates the action model and the mesh stack that next model message being prepared is for action set message and would not be transmitted over the air.
 * 
 * This function prepares the action model and the mesh stack such that the next 
 * call to the model API would be for setting the action and not to be sent over
 * the network. 
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionClientPrepareAction(void);

/*----------------------------------------------------------------------------*
 * ActionClientSetAction
 */
/*! \brief To set an action on a node the following functions should be called in sequence. ActionClientPrepareAction() followed by the model API whose action need to be set followed by ActionClientSetAction(). ActionClientSetAction() function forms the action set msgs with the action characteristics and the model message API previously called and sends it over the network onto the node with dest_id address. This is a multiple part message, where each part will be acknowledged through an ACTION_SET_ACK (0x51).
 * 
 * This function forms the action set msgs with the action characteristics and
 * the model message API previously called and sends it over the network onto 
 * the node with dest_id address. This is a multiple part message, where each 
 * part will be acknowledged through an \ref CSRMESH_ACTION_SET_ACTION_ACK (0x51).
 * 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_ACTION_SET_ACTION_ACK 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_SET_ACTION_INFO_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionClientSetAction(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_SET_ACTION_INFO_T *p_params );

/*----------------------------------------------------------------------------*
 * ActionGetActionStatus
 */
/*! \brief Request towards a node about the various Actions currently handled. This will be answered through an ACTION_STATUS. A Transaction ID is provided to ensure identification of response with request.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_ACTION_ACTION_STATUS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_GET_ACTION_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionGetActionStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_GET_ACTION_STATUS_T *p_params );

/*----------------------------------------------------------------------------*
 * ActionDelete
 */
/*! \brief This message allows a set of actions to be removed. Actions are identified through a bitmask reflecting the ActionID one wishes to delete. This message will be acknowledged through ACTION_DELETE_ACK.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_ACTION_DELETE_ACK 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_DELETE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionDelete(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_DELETE_T *p_params );

/*----------------------------------------------------------------------------*
 * ActionGet
 */
/*! \brief Using this message, it is possible to interrogate a node about the specific payload associated with an Action ID. This will be answered by and ACTION_SET_ACTION message, ACTION_SET_ACTION_ACK will need to be issued in order to collect all parts.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_ACTION_SET_ACTION 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_GET_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionGet(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_GET_T *p_params );

/*!@} */
#endif /* __ACTION_CLIENT_H__ */

