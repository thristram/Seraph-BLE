/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file LargeObjectTransfer_model.h
 *
 *  \brief Defines CSRmesh LargeObjectTransfer Model specific data structures\n
 */
/******************************************************************************/

#ifndef __LARGEOBJECTTRANSFER_MODEL_H__
#define __LARGEOBJECTTRANSFER_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup LargeObjectTransfer_Model 
 *  \brief LargeObjectTransfer Model API
 *
 * Ability to exchange information between nodes such as to allow a peer-to-peer data transfer to be established after a negotiation phase.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh LargeObjectTransfer Model message types */

/*! \brief A node wanting to provide a large object to neighbouring Mesh Nodes issues an ANNOUNCE with the associated content type. This message will have TTL=0, thus will only be answered by its immediate neighbours. The ANNOUNCE has the total size of the packet to be issued. The format and encoding of the large object is subject to the provided type and is out of scope of this document. The destination ID can either be 0, a group or a specific Device ID. In case the destination ID is not zero, only members of the group (associated with the LOT model) or the device with the specified Device ID responds with the intent to download the object for their own consumption. Every other node either ignores or accepts the offer for the purpose of relaying the packet. */
typedef struct
{
    CsrUint16 companycode; /*!< \brief Bluetooth Company Code */
    CsrUint8 platformtype; /*!< \brief Platform this object is intended for (e.g. CSR1010, CSR1020) */
    CsrUint8 typeencoding; /*!< \brief Type description of intended payload (e.g. firmware, application) */
    CsrUint8 imagetype; /*!< \brief Type of image (e.g. Light, Beacon) */
    CsrUint8 size; /*!< \brief Number of kilobytes in the Large Object (0 = < 1K bytes, 1 = >=1K bytes, < 2K bytes, and so on) */
    CsrUint16 objectversion; /*!< \brief Object Version (e.g. Firmware version x.y.z). A node can use this to decide if it already has this version of the object. Major version is 6 bits. (MSB), Minor version is 4 bits, Revision is 6 bits (LSB). */
    CsrUint16 targetdestination; /*!< \brief Destination of the Large Object */
} CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE_T;

/*! \brief In case a node is ready to receive the proposed object, it responds with this message. The intended behaviour of the Large Object Transfer is to allow a Peer-to-Peer connection between the consumer and the producer. The consumer uses a ServiceID, part of which is randomly selected. The top 64 bits are 0x1122334455667788, the least significant 63 bits are randomly selected by the consumer node. The most significant bit of the least significant 64 bits is an encoding of the intent to relay the received data. Once this message has been issued, the consumer node starts advertising a connectable service with the 128-bits service composed through the concatenation of the fixed 64 bits and the randomly selected 63 bits. The duration of the advertisement is an implementation decision. */
typedef struct
{
    CsrUint8 serviceid[8/size_in_bytes(CsrUint8)]; /*!< \brief Least Significant 63 bits of Service ID to be used for transfer - Most Significant Bit set to 1 if the source intends to use the packet. */
} CSRMESH_LARGEOBJECTTRANSFER_INTEREST_T;


/*!@} */
#endif /* __LARGEOBJECTTRANSFER_MODEL_H__ */

