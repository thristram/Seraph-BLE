/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file extension_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh Extension model
 */
/******************************************************************************/

#ifndef __EXTENSION_SERVER_H__
#define __EXTENSION_SERVER_H__

/*! \addtogroup Extension_Server
 * @{
 */
#include "extension_model.h"

/*============================================================================*
    Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * ExtensionModelInit
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
extern CSRmeshResult ExtensionModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * ExtensionConflict
 */
/*! \brief Response to a REQUEST - only issued if a conflict is noticed. This message indicates that the proposed OpCode is already in use within the node processing the request message. Nodes receiving conflict extension will process this message and remove the conflicting OpCode from the list of OpCodes to handle. All conflict messages are relayed, processed or not. If a node receiving a REQUEST is able to match the hash of the provider previously assigned to an existing OpCode, but different to the proposed one, it responds with a CONFLICT with a reason combining the top bit with the previously associated range (0x80 | <old range>). In such cases, the previously used OpCode (start of range) will be placed in the ProposedOpCode. Nodes receiving this conflict message with the top bit raised, will discard the initially proposed OpCode and replace it with the proposed code supplied in the conflict message.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_EXTENSION_CONFLICT_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult ExtensionConflict(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_EXTENSION_CONFLICT_T *p_params);

/*----------------------------------------------------------------------------*
 * ExtensionVerifyOpcodeConflictWithMesh
 */
/*! \brief Verify the opcodes passed Conflict with Mesh Opcodes
 *
 * The function checks whether the proposed opcodes conflict with the ones
 * supported by the Mesh or not.
 *
 *
 * \param opcode_start Start of the opcode range that needs to be checked.
 * \param opcode_end End of the opcode range that needs to be checked
 *
 * \returns TRUE if the opcodes conflict otherwise returns FALSE.
 */
/*---------------------------------------------------------------------------*/
extern bool ExtensionVerifyOpcodeConflictWithMesh(CsrUint16 opcode_start,
                                                  CsrUint16 opcode_end);

/*----------------------------------------------------------------------------*
 *     ExtensionServerSetupOpcodeList
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
extern CSRmeshResult ExtensionServerSetupOpcodeList(uint16* opcode_list,
                                                    CsrUint8 max_opcode_ranges);

/*!@} */
#endif /* __EXTENSION_SERVER_H__ */

