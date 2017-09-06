/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Asset_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Asset model 
 */
/******************************************************************************/

#ifndef __ASSET_CLIENT_H__
#define __ASSET_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Asset_Client
 * @{
 */
#include "asset_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * AssetModelClientInit
 */
/*! \brief 
 *      Initialises Asset Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult AssetModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * AssetSetState
 */
/*! \brief Setting Asset State: Upon receiving an ASSET_SET_STATE message, the device saves the Interval, SideEffects, ToDestination, TxPower, Number of Announcements and AnnounceInterval  fields into the appropriate state values. It then responds with an ASSET_STATE message with the current state information.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_ASSET_STATE 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ASSET_SET_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult AssetSetState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ASSET_SET_STATE_T *p_params );

/*----------------------------------------------------------------------------*
 * AssetGetState
 */
/*! \brief Getting Asset State: Upon receiving an ASSET_GET_STATE message, the device responds with an ASSET_STATE message with the current state information.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_ASSET_STATE 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_ASSET_GET_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult AssetGetState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_ASSET_GET_STATE_T *p_params );

/*!@} */
#endif /* __ASSET_CLIENT_H__ */

