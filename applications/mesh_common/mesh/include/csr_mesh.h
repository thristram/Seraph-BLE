/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *****************************************************************************/
/*! \file csr_mesh.h
 *  \brief CSRmesh library configuration and control functions
 *
 *   This file contains the functions to provide the application with
 *   access to the CSRmesh library
 *
 *   NOTE: This library includes the Mesh Transport Layer, Mesh Control
 *   Layer and Mesh Association Layer functionality.
 */
 /*****************************************************************************/

#ifndef __CSR_MESH_H__
#define __CSR_MESH_H__

#include "csr_mesh_types.h"

/*! \addtogroup CSRmesh
 * @{
 */

/*============================================================================*
Public Function Implementations
*============================================================================*/

/**************************CSRmesh Generic APIs*******************************/

/*----------------------------------------------------------------------------*
 * CSRmeshInit
 */
/*! \brief Initialize CSRmesh core mesh stack
 *
 *  Initialize MASP, MCP & MTL Layers.
 *
 *  \param configFlag  The configuration flag identifies the role of
 *  the device (configuring / non-configuring).
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*---------------------------------------------------------------------------*/
CSRmeshResult CSRmeshInit(CSR_MESH_CONFIG_FLAG_T configFlag);

/*----------------------------------------------------------------------------*
 * CSRmeshRegisterAppCallback
 */
/*! \brief Register application callback
 *
 *  This API registers an application call-back to the stack. All the stack
 *  events are notified to this registerd call-back. For supporting mutiple
 *  applications a seperate multiplexer module is required. The multiplexer
 *  module should take care of notifying the stack event to all the interested
 *  applications.
 *
 *  \param callback application call-back to register
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*---------------------------------------------------------------------------*/
CSRmeshResult CSRmeshRegisterAppCallback(CSR_MESH_APP_CB_T callback);

/*----------------------------------------------------------------------------*
 * CSRmeshStart
 */
/*! \brief Start the CSRmesh system
 *
 *  Initializes Bearer Layer Successful completion is indication that stack
 *  is ready for Rx/Tx.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*---------------------------------------------------------------------------*/
CSRmeshResult CSRmeshStart(void);

/*----------------------------------------------------------------------------*
 * CSRmeshStop
 */
/*! \brief Stop the CSRmesh system
 *
 *  Stops processing the CSRmesh messages and does not allow sending
 *  mesh messages. It does not stop LE Scanning. The Scheduler API 
 *  \ref CSRSchedEnableListening should be called to disable scanning.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*---------------------------------------------------------------------------*/
CSRmeshResult CSRmeshStop(void);

/*----------------------------------------------------------------------------*
 * CSRmeshReset
 */
/*! \brief Reset the CSRmesh library
 *
 *  Resets the CSRmesh library and clears all data relevant to each layer.
 *  The network association info is retained which can be removed only using
 *  the config model.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshReset(void);

/*----------------------------------------------------------------------------*
 * CSRmeshSetDefaultTTL
 */
/*! \brief Sets a default ttl value
 *
 *  This API sets the default ttl value to be used by the core models to send
 *  ACK message.
 *
 *  \param ttl Time to live value to be used for ACK messages.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*---------------------------------------------------------------------------*/
CSRmeshResult CSRmeshSetDefaultTTL(CsrUint8 ttl);


/*----------------------------------------------------------------------------*
 * CSRmeshGetDefaultTTL
 */
/*! \brief Gets the default ttl value 
 *
 *  This API gets the default ttl value being used by the core models to send
 *  ACK message.
 *
 *  \param eventData On successful completion the default ttl will be available
 *  to the app as part of eventData param.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*---------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGetDefaultTTL(CSR_MESH_APP_EVENT_DATA_T *eventData);

/*----------------------------------------------------------------------------*
 * CSRmeshRemoveNetwork
 */
/*! \brief Remove a network
 *
 *  This API removes the network key associated with the network id from
 *  the device.
 *
 *  \param netId  Network id of the network to be removed
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshRemoveNetwork(CsrUint8 netId);

#if (CSR_MESH_ON_CHIP != 1)
/*----------------------------------------------------------------------------*
 * CSRmeshGetNetIDList
 */
/*! \brief Set a passphrase to create a network and network key internally
 *
 *
 *  \param eventData   On successful completion a network id list will be
 *  available to the app as a field in this 'result parameter'. This is the list
 *  of the network ids which maps to each network managed by the device
 *  and has direct mapping to all the network keys inside the core stack.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGetNetIDList(CSR_MESH_APP_EVENT_DATA_T *eventData);

#endif /* (CSR_MESH_ON_CHIP != 1) */

/*----------------------------------------------------------------------------*
 * CSRmeshGetDeviceID
 */
/*! \brief Gets the 16-bit Device Identifier of the CSRmesh device
 *
 *  \param netId
 *  \param eventData   On successful completion a device id will be available
 *  to the app as a field in this 'result parameter'.
 *
 *  \returns CSRmeshResult. Refer to CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGetDeviceID( CsrUint8 netId, CSR_MESH_APP_EVENT_DATA_T *eventData);

/*----------------------------------------------------------------------------*
 * CSRmeshGetDeviceUUID
 */
/*! \brief Get the CSRmesh library 128 bit UUID.
 *
 *  \param eventData   On successful completion a uuid id will be available
 *  to the app as a field in this 'result parameter'.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGetDeviceUUID(CSR_MESH_APP_EVENT_DATA_T *eventData);

/*----------------------------------------------------------------------------*
 * CSRmeshSetTransmitState
 */
/*! \brief CSRmeshSetTransmitState .
 *
 *  This API sets the bearer relay, bearer enable and promiscuous states for
 *  Mesh Bearer layer, also sets relay and promiscuous state of MTL.
 *  MTL and Mesh Bearer layer based on this configuration will allow
 *  reception, transmission of mesh messages and also relay of known
 *  and unknown mesh messages. Parameters of this API identifies a 16- bit
 *  bitfield for bearer relay active, bearer enabled and bearer
 *  promiscuous arguments.
 *
 *  Note: Setting bit '0' in the corresponding bit mask will disable
 *  the corresponding functionality if it is already active.
 *
 *  \param transmitStateArg      structure containing all the transmit state
 *  arguments.
 *  \param eventData   On successful completion, transmit state will be
 *  available to the app.
 *  as a field in this 'result parameter'.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*-----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshSetTransmitState(CSR_MESH_TRANSMIT_STATE_T *transmitStateArg,
                                                          CSR_MESH_APP_EVENT_DATA_T *eventData);

/*----------------------------------------------------------------------------*
 * CSRmeshGetTransmitState
 */
/*! \brief CSRmeshGetTransmitState .
 *
 *  This function reads transmit state arguments of CSRmesh network.
 *
 *  \param netId       Network id of the network for which the relay is
 *  required to be enabled/disabled.
 *  \param eventData   On successful completion transmit state will be
 *  available to the app.
 *  as a field in this 'result parameter'.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*-----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGetTransmitState(CsrUint8 netId, CSR_MESH_APP_EVENT_DATA_T *eventData);

/*----------------------------------------------------------------------------*
 * CSRmeshAssociateToANetwork
 */
/*! \brief Advertises a CSRmesh device identification message.
 *
 *  Application uses this API to appear as a New Device.
 *  On successful execution of this API the device sends
 *  MASP_DEVICE_IDENTIFICATION message with its 128bit UUID.
 *
 *  \param deviceAppearance   The deviceAppearance of the device.
 *   This is a pointer to \ref CSR_MESH_DEVICE_APPEARANCE_T structure.
 *  \param ttl        Time to live value used for MASP.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshAssociateToANetwork(CSR_MESH_DEVICE_APPEARANCE_T *deviceAppearance, CsrUint8 ttl);

/*----------------------------------------------------------------------------*
 * CSrMeshCalculateSHA256Hash
 */
/*! \brief This function generates a 32 byte hash.
 *
 *  Application uses this API to generate a 32 byte secure hash.
 *
 *  \param string   The data for which hash to be generated.
 *  \param length   The length of the data.
 *  \param hash     The generated hash output.
 *
 *  \returns Nothing.
 */
/*----------------------------------------------------------------------------*/
void CSRmeshCalculateSHA256Hash(const CsrUint8 *string, CsrUint8 length, CsrUint16 *hash);

/*----------------------------------------------------------------------------*
 * CSRmeshSetMeshDuplicateMessageCacheSize
 */
/*! \brief Controls the depth of the duplicate check cache in MTL
 *
 *  Application uses this API to control the depth of message cache in
 *..MTL (dynamically) [0..255]
 *
 *  \param cacheSize      Size of message cache [0 to 255]
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshSetMeshDuplicateMessageCacheSize(CsrUint8 cacheSize);

/*----------------------------------------------------------------------------*
 * CSRmeshSendKeyIVStatusRequest
 */
/*! \brief Transmit Key-IV request.
 *
 *  This API Transmit KeyIV status request for Newtork re-keying or for IV Update.
 *
 * \param devData Pointer to MASP device data structure. This needs to be filled
 *                with relevant data like Network Id, self (for NON-CONFIG role)
 *                or remote (for CONFIG role) Device Id and Device Signature
 *                (Only for CONFIG role and NULL for NON-CONFIG role).
 *
 * \param ttl TTL for sending this message.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshSendKeyIVStatusRequest(const CSR_MESH_MASP_DEVICE_DATA_T *devData, CsrUint8 ttl);

/*----------------------------------------------------------------------------*
 * CSRmeshSetSrcSequenceCache
 */
/*! \brief CSRmeshSetSequenceSrcCache.
 *
 *  This function configures the src , sqn cache in mesh stack.
 *
 *  \param netId        Id of the network to which Sequence Table is used.
 *
 *  \param seqCache     Pointer to the memory allocated for the source, sequence
 *                      cache. This parameter can be NULL to disable the Sequence
 *                      Cache for a network. In this parameter, all the data members
 *                      need to be filled in by the user. Example usage shown here:
 *  \code
 *  #define HOME_NW_ID              (0)
 *  #define NUM_SEQ_LUT_ENTRIES     (16)
 *
 *  CSRmeshResult result;
 *  CSR_MESH_SQN_LOOKUP_TABLE_T home_nw_seq_lut[NUM_SEQ_LUT_ENTRIES];
 *  CSR_MESH_SEQ_CACHE_T home_nw_seq_cache;
 *
 *  home_nw_seq_cache.cached_dev_count = NUM_SEQ_LUT_ENTRIES;
 *  home_nw_seq_cache.seq_deviation    = 32;
 *  home_nw_seq_cache.seq_lookup_table = home_nw_seq_lut;
 *
 *  result = CSRmeshSetSrcSequenceCache(HOME_NW_ID, &home_nw_seq_cache);
 *
 *  \endcode
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*-----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshSetSrcSequenceCache(CsrUint8 netId, CSR_MESH_SEQ_CACHE_T *seqCache);

#if (CSR_MESH_ON_CHIP != 1)
/*----------------------------------------------------------------------------*
 * CSRmeshGenerateMeshIdFromNetId
 */
/*! \brief Generates a MeshId for a given network id
 *
 *  Application uses this API to generate MeshId
 *
 *  \param netId         Network id of the network for which MeshId generation
 *  is required.
 *  \param eventData     On successful completion a secret MeshId be
 *  available to the app.
 *   as a field in this 'result parameter'
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGenerateMeshIdFromNetId(CsrUint8 netId,
                                              CSR_MESH_APP_EVENT_DATA_T *eventData);

/*----------------------------------------------------------------------------*
 * CSRmeshGetNetIdFromMeshId
 */
/*! \brief Obtains the network id for a given MeshId
 *
 *  Application uses this API to get the network id for a particular MeshId
 *
 *  \param meshId      Received MeshId required to find the associated
 *  network id.
 *  \param eventData     On successful completion the network id will be
 *..available to the app as a field in this 'result parameter'.
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
CSRmeshResult CSRmeshGetNetIdFromMeshId(CsrUint8 *meshId,
                                        CSR_MESH_APP_EVENT_DATA_T *eventData);

#endif /* (CSR_MESH_ON_CHIP != 1) */

/*!@} */
#endif /* __CSR_MESH_H__ */

