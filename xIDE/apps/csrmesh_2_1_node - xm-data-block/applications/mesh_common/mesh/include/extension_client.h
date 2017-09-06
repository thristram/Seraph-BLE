/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Extension_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Extension model 
 */
/******************************************************************************/

#ifndef __EXTENSION_CLIENT_H__
#define __EXTENSION_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Extension_Client
 * @{
 */
#include "extension_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * ExtensionModelClientInit
 */
/*! \brief 
 *      Initialises Extension Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ExtensionModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * ExtensionRequest
 */
/*! \brief Request for Extension OpCode to be approved by the whole Mesh. A device wanting to use an OpCode, makes a request to the entire Mesh Network. This message is issued to target identity 0. The device waits some time, proportional to the size of the Mesh network and only after this period, messages using these proposed OpCode are used. Device receiving this message and wanting to oppose the usage of such code will respond to the source node with a CONFLICT. In case no conflict is known and the OpCode is for a message the node is interested in implementing (through comparison with hash value), a record of the OpCode and its mapping is kept.  Request messages are relayed in cases of absence of conflict. The hash function is SHA-256, padded as per SHA-256 specifications2, for which the least significant 6 bytes will be used in the message. The range parameter indicates the maximum number of OpCode reserved from the based provided in the Proposed OpCode field. The last OpCode reserved is determined through the sum of the Proposed OpCode with the range value. This range parameter varies from 0 to 127, leaving the top bit free.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_EXTENSION_CONFLICT 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_EXTENSION_REQUEST_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ExtensionRequest(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_EXTENSION_REQUEST_T *p_params );

/*----------------------------------------------------------------------------*
 *     ExtensionClientSetupOpcodeList
 */
/*! \brief Set Up Opcode List to be accessible by the extension model
 *
 * The function sets up a list to manage added opcodes through extension 
 * model. The function tells the extension model about the list of opcodes
 * that is allocated for use with the extension model.
 * The list must be a global variable and device and must accessible 
 * any time when the device is active.
 *
 * \param opcode_list The List of the opcode, range and the hash string stored.
 * \param max_opcode_ranges The maximum opcode ranges supported by application
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ExtensionClientSetupOpcodeList(uint16* opcode_list,
                                                    CsrUint8 max_opcode_ranges);

/*----------------------------------------------------------------------------*
 * ExtensionSendMessage
 */
/*! \brief Sends CSRmesh message with an extension opcode
 *
 *   This function sends a CSRmesh message to a destination device with a 
 *   proposed extension opcode.
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl The time to live value to be set for the message
 * \param msg Pointer to the message parameters 
 * \param len Length of the message 
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ExtensionSendMessage(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CsrUint8 *msg,
                                  CsrUint8 len);

/*!@} */
#endif /* __EXTENSION_CLIENT_H__ */

