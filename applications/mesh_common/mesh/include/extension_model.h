/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Extension_model.h
 *
 *  \brief Defines CSRmesh Extension Model specific data structures\n
 */
/******************************************************************************/

#ifndef __EXTENSION_MODEL_H__
#define __EXTENSION_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Extension_Model 
 *  \brief Extension Model API
 *
 * <br>The Extension Model allows dynamic allocation of OpCode within a Mesh Network. This, permits extension of existing models without the risk of encoding clashes as per Data Model usage. A node wanting to introduce a new Model/Message or a new Message to an existing model, has to select an OpCode unique for that purpose. Given that there is no overall authority able to allocate a unique OpCode, this model proposes a collaborative method, within the existing Mesh Network, to obtain this unique information. Two messages are proposed, one performing a request, another informing of a conflict.<br><br> As per the nature of Mesh Network, there is never an absolute guarantee of a solution, it is therefore necessary for the request to be repeated periodically. If anything, the periodic issue of this message will allow for new nodes to have the ability to collect the information. The request message has two purposes, indication of the intended OpCode and the mapping of this OpCode with the extension it is aimed at supplying. Identification of the function cover by the OpCode is done through an identity supplied by the manufacturer.<br><br> In order to avoid the obvious denial of service through blocking of requests from specific manufacturers, the unique identity is calculated from the hashing of a sentence provided by the manufacturer. The one-way nature of the Hash makes it hard to reverse.<br><br> For nodes intending to support these new messages, matching of the identity with the request, allows them to determine the OpCode to be used. Requests are relayed from node to node. Only when a node determine that the proposed Opcodes are conflicting with some internally used ones, the message will not be relayed, instead a conflict will be issued. Upon reception of a request, a node required to implement this function, takes note of the OpCode and starts processing messages with such OpCodes. <br><br> Conflict messages carry the conflicting OpCodes and the reason for issuing of the conflict. Conflict messages are
ssed by all, thus allowing non-conflicting nodes to make of note of the fact that the OpCode is invalid and thus prevent their processing. Each node is expected to keep track of all the OpCodes it is currently using. These are either static or dynamic. A node wishing to introduce a new (model, message) pair can do so through a proposal to the Mesh Networks it belongs to, of the new range of Opcodes it intends to use.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Extension Model message types */

/*! \brief Request for Extension OpCode to be approved by the whole Mesh. A device wanting to use an OpCode, makes a request to the entire Mesh Network. This message is issued to target identity 0. The device waits some time, proportional to the size of the Mesh network and only after this period, messages using these proposed OpCode are used. Device receiving this message and wanting to oppose the usage of such code will respond to the source node with a CONFLICT. In case no conflict is known and the OpCode is for a message the node is interested in implementing (through comparison with hash value), a record of the OpCode and its mapping is kept.  Request messages are relayed in cases of absence of conflict. The hash function is SHA-256, padded as per SHA-256 specifications2, for which the least significant 6 bytes will be used in the message. The range parameter indicates the maximum number of OpCode reserved from the based provided in the Proposed OpCode field. The last OpCode reserved is determined through the sum of the Proposed OpCode with the range value. This range parameter varies from 0 to 127, leaving the top bit free. */
typedef struct
{
    CsrUint8 extensionhash[6/size_in_bytes(CsrUint8)]; /*!< \brief 48bits Hash (SHA-256) of Text supplied by OEM for characterisation of intended usage */
    CsrUint16 proposedopcode; /*!< \brief Proposed OpCode - start of range requested */
    CsrUint8 range; /*!< \brief Number of OpCode to be allocated, starting from ProposedOpCode */
} CSRMESH_EXTENSION_REQUEST_T;

/*! \brief Response to a REQUEST - only issued if a conflict is noticed. This message indicates that the proposed OpCode is already in use within the node processing the request message. Nodes receiving conflict extension will process this message and remove the conflicting OpCode from the list of OpCodes to handle. All conflict messages are relayed, processed or not. If a node receiving a REQUEST is able to match the hash of the provider previously assigned to an existing OpCode, but different to the proposed one, it responds with a CONFLICT with a reason combining the top bit with the previously associated range (0x80 | <old range>). In such cases, the previously used OpCode (start of range) will be placed in the ProposedOpCode. Nodes receiving this conflict message with the top bit raised, will discard the initially proposed OpCode and replace it with the proposed code supplied in the conflict message. */
typedef struct
{
    CsrUint8 extensionhash[6/size_in_bytes(CsrUint8)]; /*!< \brief 48bits Hash (SHA-256) of Text supplied by OEM for characterisation of intended usage */
    CsrUint16 proposedopcode; /*!< \brief Proposed OpCode - start of range requested */
    CsrUint8 reason; /*!< \brief Code describing reason for rejection */
} CSRMESH_EXTENSION_CONFLICT_T;


/*!@} */
#endif /* __EXTENSION_MODEL_H__ */

