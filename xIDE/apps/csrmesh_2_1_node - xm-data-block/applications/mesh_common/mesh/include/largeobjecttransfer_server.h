/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file largeobjecttransfer_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh LargeObjectTransfer model
 */
/******************************************************************************/

#ifndef __LARGEOBJECTTRANSFER_SERVER_H__
#define __LARGEOBJECTTRANSFER_SERVER_H__

/*! \addtogroup LargeObjectTransfer_Server
 * @{
 */
#include "largeobjecttransfer_model.h"

/*============================================================================*
    Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * LargeObjectTransferModelInit
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
extern CSRmeshResult LargeObjectTransferModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * LargeObjectTransferInterest
 */
/*! \brief In case a node is ready to receive the proposed object, it responds with this message. The intended behaviour of the Large Object Transfer is to allow a Peer-to-Peer connection between the consumer and the producer. The consumer uses a ServiceID, part of which is randomly selected. The top 64 bits are 0x1122334455667788, the least significant 63 bits are randomly selected by the consumer node. The most significant bit of the least significant 64 bits is an encoding of the intent to relay the received data. Once this message has been issued, the consumer node starts advertising a connectable service with the 128-bits service composed through the concatenation of the fixed 64 bits and the randomly selected 63 bits. The duration of the advertisement is an implementation decision.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_LARGEOBJECTTRANSFER_INTEREST_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult LargeObjectTransferInterest(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_LARGEOBJECTTRANSFER_INTEREST_T *p_params);

/*!@} */
#endif /* __LARGEOBJECTTRANSFER_SERVER_H__ */

