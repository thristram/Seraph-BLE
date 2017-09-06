/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file beacon_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh Beacon model
 */
/******************************************************************************/

#ifndef __BEACON_SERVER_H__
#define __BEACON_SERVER_H__

/*! \addtogroup Beacon_Server
 * @{
 */
#include "beacon_model.h"

/*============================================================================*
    Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * BeaconModelInit
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
extern CSRmeshResult BeaconModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * BeaconBeaconStatus
 */
/*! \brief This message issue issued by a Beacon as response to BEACON_SET_STATUS or BEACON_GET_STATUS. Furthermore, a Beacon appearing on the mesh sporadically, will issue such message (with destination ID set to 0) as soon as it re-joins the mesh. In this case, one message per active beacon type should be issued. This message will provide the Payload ID currently in used for the associated type.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_BEACON_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconBeaconStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 src_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_BEACON_STATUS_T *p_params);

/*----------------------------------------------------------------------------*
 * BeaconTypes
 */
/*! \brief Provides information on the set of beacon supported by the node. The battery level is reported as well as time since last message.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACON_TYPES_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconTypes(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_TYPES_T *p_params);

/*----------------------------------------------------------------------------*
 * BeaconSetPayload
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
extern CSRmeshResult BeaconSetPayload(
                                  CsrUint8 nw_id,
                                  CsrUint16 src_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_SET_PAYLOAD_T *p_params);

/*----------------------------------------------------------------------------*
 * BeaconPayloadAck
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
extern CSRmeshResult BeaconPayloadAck(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACON_PAYLOAD_ACK_T *p_params);

/*!@} */
#endif /* __BEACON_SERVER_H__ */

