/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Beacon_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Beacon model 
 */
/******************************************************************************/

#ifndef __BEACON_CLIENT_H__
#define __BEACON_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Beacon_Client
 * @{
 */
#include "beacon_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * BeaconModelClientInit
 */
/*! \brief 
 *      Initialises Beacon Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * BeaconSetStatus
 */
/*! \brief May be sent to a beacon to set its status. More than one such command can be sent to set the intervals for different beacon types, which need not be the same. This message also allows for the wakeup intervals to be set if the beacon elects to only be on the mesh sporadically. The time is always with respect to the beacon’s internal clock and has no absolute meaning. This message will be answered through BEACON_STATUS.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACON_BEACON_STATUS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_SET_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconSetStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_SET_STATUS_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconGetBeaconStatus
 */
/*! \brief Message to be sent to a beacon in order to recover its current status. This message fetch information pertinent to a particular type. The transaction ID will help matching the pertinent BEACON_STATUS message – a different transaction ID shall be used for a different type (although the BEACON_STATUS message has the type pertinent to the status and thus could be used in conjunction to the Transaction ID for disambiguation).
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACON_BEACON_STATUS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_GET_BEACON_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconGetBeaconStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_GET_BEACON_STATUS_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconGetTypes
 */
/*! \brief Message allowing a node to fetch all supported types and associated information by a given Beacon.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACON_TYPES 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_GET_TYPES_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconGetTypes(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_GET_TYPES_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconClientSetPayload
 */
/*! \brief One or more of these packets may be sent to a beacon to set its payload. The content is either the raw advert data, or extra information (such as crypto cycle) which a beacon may require. The payload data is sent as a length and an offset, so the whole payload need not be sent if it has not changed. A beacon can support many beacon types - it can be sent as many different payloads as needed, one for each type. The first byte of the first payload packet contains the length and offset of the payload and the payload ID; this allows a beacon which already has the payload to send an immediate acknowledgement, saving traffic. This ID will be sent back in the 'Payload ACK' message if the beacon has received the whole payload. The payload may have to be split in several parts, in which case only the last part shall be acknowledged. Upon receiving a BEACON_SET_PAYLOAD, the beacon will update its corresponding beacon data – if data was previously available, it will be replaced by the provided payload, thus a beacon can only hold one payload per beacon type.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACON_PAYLOAD_ACK 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_SET_PAYLOAD_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconClientSetPayload(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_SET_PAYLOAD_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconClientPayloadAck
 */
/*! \brief Only one Acknowledgement message will occur for multiple BEACON_SET_PAYLOAD messages sharing the same transaction ID. Only when the last segment is received that such acknowledgement will be issued. Where missing payload messages exist, the list of their indices will be provided in the Ack field.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_PAYLOAD_ACK_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconClientPayloadAck(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_PAYLOAD_ACK_T *p_params );

/*----------------------------------------------------------------------------*
 * BeaconGetPayload
 */
/*! \brief Message allowing a node to retrieve the current payload held on a given beacon. This message shall be answered by one or more BEACON_SET_PAYLOAD messages.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_BEACON_SET_PAYLOAD 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_GET_PAYLOAD_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconGetPayload(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_GET_PAYLOAD_T *p_params );

/*!@} */
#endif /* __BEACON_CLIENT_H__ */

