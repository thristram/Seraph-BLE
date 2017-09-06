/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file LargeObjectTransfer_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh LargeObjectTransfer model 
 */
/******************************************************************************/

#ifndef __LARGEOBJECTTRANSFER_CLIENT_H__
#define __LARGEOBJECTTRANSFER_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup LargeObjectTransfer_Client
 * @{
 */
#include "largeobjecttransfer_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * LargeObjectTransferModelClientInit
 */
/*! \brief 
 *      Initialises LargeObjectTransfer Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult LargeObjectTransferModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * LargeObjectTransferAnnounce
 */
/*! \brief A node wanting to provide a large object to neighbouring Mesh Nodes issues an ANNOUNCE with the associated content type. This message will have TTL=0, thus will only be answered by its immediate neighbours. The ANNOUNCE has the total size of the packet to be issued. The format and encoding of the large object is subject to the provided type and is out of scope of this document. The destination ID can either be 0, a group or a specific Device ID. In case the destination ID is not zero, only members of the group (associated with the LOT model) or the device with the specified Device ID responds with the intent to download the object for their own consumption. Every other node either ignores or accepts the offer for the purpose of relaying the packet.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_LARGEOBJECTTRANSFER_INTEREST 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult LargeObjectTransferAnnounce(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE_T *p_params );

/*!@} */
#endif /* __LARGEOBJECTTRANSFER_CLIENT_H__ */

