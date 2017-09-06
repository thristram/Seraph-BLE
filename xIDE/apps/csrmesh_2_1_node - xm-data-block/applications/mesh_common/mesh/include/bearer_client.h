/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Bearer_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Bearer model 
 */
/******************************************************************************/

#ifndef __BEARER_CLIENT_H__
#define __BEARER_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Bearer_Client
 * @{
 */
#include "bearer_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * BearerModelClientInit
 */
/*! \brief 
 *      Initialises Bearer Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BearerModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * BearerSetState
 */
/*! \brief Setting Bearer State: Upon receiving a BEARER_SET_STATE message, where the destination address is the device ID of this device, the device saves the BearerRelayActive, BearerEnabled, and BearerPromiscuous fields into the appropriate state value. Then the device responds with a BEARER_STATE message with the current state information.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEARER_STATE 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEARER_SET_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BearerSetState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEARER_SET_STATE_T *p_params );

/*----------------------------------------------------------------------------*
 * BearerGetState
 */
/*! \brief Getting Bearer State: Upon receiving a BEARER_GET_STATE message, where the destination address is the device ID of this device, the device responds with a BEARER_STATE message with the current state information.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEARER_STATE 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEARER_GET_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BearerGetState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEARER_GET_STATE_T *p_params );

/*!@} */
#endif /* __BEARER_CLIENT_H__ */

