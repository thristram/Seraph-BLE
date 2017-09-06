/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Group_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Group model 
 */
/******************************************************************************/

#ifndef __GROUP_CLIENT_H__
#define __GROUP_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Group_Client
 * @{
 */
#include "group_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * GroupModelClientInit
 */
/*! \brief 
 *      Initialises Group Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult GroupModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * GroupGetNumberOfModelGroupids
 */
/*! \brief Getting Number of Group IDs: Upon receiving a GROUP_GET_NUMBER_OF_MODEL_GROUPS message, where the destination address is the DeviceID of this device, the device responds with a GROUP_NUMBER_OF_MODEL_GROUPS message with the number of Group IDs that the given model supports on this device.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_GROUP_NUMBER_OF_MODEL_GROUPIDS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_GROUP_GET_NUMBER_OF_MODEL_GROUPIDS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult GroupGetNumberOfModelGroupids(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_GROUP_GET_NUMBER_OF_MODEL_GROUPIDS_T *p_params );

/*----------------------------------------------------------------------------*
 * GroupSetModelGroupid
 */
/*! \brief Setting Model Group ID: Upon receiving a GROUP_SET_MODEL_GROUPID message, where the destination address is the DeviceID of this device, the device saves the Instance and GroupID fields into the appropriate state value determined by the Model and GroupIndex fields. It then responds with a GROUP_MODEL_GROUPID message with the current state information held for the given model and the GroupIndex values.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_GROUP_MODEL_GROUPID 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_GROUP_SET_MODEL_GROUPID_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult GroupSetModelGroupid(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_GROUP_SET_MODEL_GROUPID_T *p_params );

/*----------------------------------------------------------------------------*
 * GroupGetModelGroupid
 */
/*! \brief Getting Model Group ID: Upon receiving a GROUP_GET_MODEL_GROUPID message, where the destination address is the DeviceID of this device, the device responds with a GROUP_MODEL_GROUPID message with the current state information held for the given Model and GroupIndex values.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_GROUP_MODEL_GROUPID 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_GROUP_GET_MODEL_GROUPID_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult GroupGetModelGroupid(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_GROUP_GET_MODEL_GROUPID_T *p_params );

/*!@} */
#endif /* __GROUP_CLIENT_H__ */

