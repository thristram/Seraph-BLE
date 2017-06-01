/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file BeaconProxy_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh BeaconProxy model 
 */
/******************************************************************************/

#ifndef __BEACONPROXY_CLIENT_H__
#define __BEACONPROXY_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup BeaconProxy_Client
 * @{
 */
#include "beaconproxy_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * BeaconProxyModelClientInit
 */
/*! \brief 
 *      Initialises BeaconProxy Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * BeaconProxyAdd
 */
/*! \brief Message can be sent to a beacon-proxy to add to the list of devices it manages. This request will be acknowledged using a Proxy Command Status Devices. Up to four device ID can be specified in this message – Group can be defined. This message also permits the flushing of pending messages for managed IDs.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACONPROXY_ADD_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyAdd(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACONPROXY_ADD_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconProxyRemove
 */
/*! \brief May be sent to a proxy to remove from the list of devices it manages. Any message queued for those devices will be cleared. This request will be acknowledged using a Proxy Command Status Devices. Groups can be used, implying that all its members will be removed from the proxy management. Specifying 0 in the Device Addresses will be interpreted as stopping all Proxy activities on the targeted proxy.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACONPROXY_REMOVE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyRemove(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACONPROXY_REMOVE_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconProxyGetStatus
 */
/*! \brief Fetch the current status - this will be answered by a BEACON_PROXY_STATUS
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACONPROXY_PROXY_STATUS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyGetStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl );

/*----------------------------------------------------------------------------*
 * BeaconProxyGetDevices
 */
/*! \brief Fetch the current set of devices ID managed by a given proxy. This will be answered by one or more BEACON_PROXY_DEVICES messages.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACONPROXY_DEVICES 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACONPROXY_GET_DEVICES_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyGetDevices(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACONPROXY_GET_DEVICES_T *p_params );

/*!@} */
#endif /* __BEACONPROXY_CLIENT_H__ */

