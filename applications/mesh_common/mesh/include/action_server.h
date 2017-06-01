/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file action_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh Action model
 */
/******************************************************************************/

#ifndef __ACTION_SERVER_H__
#define __ACTION_SERVER_H__

/*! \addtogroup Action_Server
 * @{
 */
#include "action_model.h"

/*============================================================================*
    Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * ActionModelInit
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
extern CSRmeshResult ActionModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * ActionPrepareAction
 */
/*! \brief To set an action on a node the following functions should be called in sequence. ActionPrepareAction() followed by the model API whose action need to be set followed by ActionSetAction(). ActionPrepareAction function indicates the action model and the mesh stack that next model message being prepared is for action set message and would not be transmitted over the air.
 * 
 * This function prepares the action model and the mesh stack such that the next 
 * call to the model API would be for setting the action and not to be sent over
 * the network. 
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionPrepareAction(void);


/*----------------------------------------------------------------------------*
 * ActionSetAction
 */
/*! \brief To set an action on a node the following functions should be called in sequence. ActionPrepareAction() followed by the model API whose action need to be set followed by ActionSetAction(). ActionSetAction() function forms the action set msgs with the action characteristics and the model message API previously called and sends it over the network onto the node with dest_id address. This is a multiple part message, where each part will be acknowledged through an ACTION_SET_ACK (0x51).
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
extern CSRmeshResult ActionSetAction(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_SET_ACTION_INFO_T *p_params);

/*----------------------------------------------------------------------------*
 * ActionSetActionAck
 */
/*! \brief Every parts received is acknowledged, providing both Transaction ID and ActionID (which includes Part Number)
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_SET_ACTION_ACK_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionSetActionAck(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_SET_ACTION_ACK_T *p_params);

/*----------------------------------------------------------------------------*
 * ActionActionStatus
 */
/*! \brief This provides a bitmask identifying all the currently allocated ActionID. The Action Supported field provides an indication on how many actions this device has capacity for. Note that a device will never have more than 32 actions.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_ACTION_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionActionStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_ACTION_STATUS_T *p_params);

/*----------------------------------------------------------------------------*
 * ActionDeleteAck
 */
/*! \brief This message confirms the delete commands and report which actions have been removed. The provided transaction ID will match the one supplied by ACTION_DELETE.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_DELETE_ACK_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionDeleteAck(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ACTION_DELETE_ACK_T *p_params);
/*----------------------------------------------------------------------------*
 * ActionSendMessage
 */
/*! \brief Sends CSRmesh message that was received as an action
 *
 *   This function sends a CSRmesh message to a destination device as set by
 *   the \ref CSRMESH_ACTION_SET message.
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ACTION_ACTION_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ActionSendMessage(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CsrUint8 *msg,
                                  CsrUint8 len);

/*!@} */
#endif /* __ACTION_SERVER_H__ */

