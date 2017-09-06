 /******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *****************************************************************************/
/*! \file csr_mesh_ps_iface.h
 *  \brief This file contains the Persistent Store API functions and definitions
 *         that is used by the CSRmesh Stack.
 *
 *   This file contains the PS interface related function and type defines.
 */
 /******************************************************************************/

#ifndef __CSR_MESH_PS_IFCE_H__
#define __CSR_MESH_PS_IFCE_H__

#include "csr_types.h"
#include "csr_mesh_types.h"
#include "csr_mesh_result.h"

/*! \addtogroup CSRmesh
 * @{
 */

/*! \brief Access Keys for Stack parameters to be saved on persistent store */
typedef enum
{
    CSR_MESH_NVM_SANITY_KEY =       0x00,/*!< \brief PS access key for sanity word */
    CSR_MESH_DEVICE_UUID_KEY =      0x01,/*!< \brief PS access key for Device UUID */
    CSR_MESH_DEVICE_AUTHCODE_KEY =  0x02,/*!< \brief PS access key for Device Authorization Code */
    CSR_MESH_MTL_NW_KEY_AVL_KEY =   0x03,/*!< \brief PS access key for network key validity flag */
    CSR_MESH_MTL_MCP_NWKEY_KEY =    0x04,/*!< \brief PS access key for MCP NWKEY */
    CSR_MESH_MASP_RELAY_STATE_KEY = 0x05,/*!< \brief PS access key for Association messages relay state flag */
    CSR_MESH_MCP_SEQ_NUM_KEY  =     0x06,/*!< \brief PS access key for last stored sequence number */
    CSR_MESH_MCP_GEN_SEQ_NUM_KEY =  0x07,/*!< \brief PS access key for intial sequence number */
    CSR_MESH_CONFIG_LAST_ETAG_KEY = 0x08,/*!< \brief PS access key for Config Last e-tag */
#if (CSR_MESH_NO_OF_NWKS == 1)
    CSR_MESH_DEVICE_ID_KEY =        0x09,/*!< \brief PS access key for device identifier. Used only when CSRmesh stack supports single network */
    CSR_MESH_MTL_RELAY_STATUS_KEY = 0x0A,/*!< \brief PS access key for MTL relay status setting. Used only when CSRmesh stack supports single network*/
    CSR_MESH_DEVICE_DHM_KEY =       0x0B,/*!< \brief PS access key for device DHM_KEY. Used only when CSRmesh stack supports single network */
    CSR_MESH_NW_IV_KEY           =  0x0C,/*!< \brief PS access key for network IV. Used only when CSRmesh stack supports single network */
#endif
} CSR_MESH_STACK_PS_KEY;


/*! \brief Size definitions for persistent store values in words */
#define CSR_MESH_NVM_SANITY_PSKEY_SIZE         (1)/*!< \brief NVM Sanity word size in words */
#define CSR_MESH_DEVICE_UUID_PSKEY_SIZE        (8)/*!< \brief Device UUID size in words */
#define CSR_MESH_DEVICE_AUTHCODE_PSKEY_SIZE    (4)/*!< \brief Device Authorization code size in words */
#define CSR_MESH_MTL_NW_KEY_AVL_PSKEY_SIZE     (1)/*!< \brief Network key avaialble flag size in words */
#if (CSR_MESH_NO_OF_NWKS == 1)
#define CSR_MESH_MTL_MCP_NWKEY_PSKEY_SIZE      (8)/*!< \brief Network key size in words */
#else
#define CSR_MESH_MTL_MCP_NWKEY_PSKEY_SIZE      (CSR_MESH_NO_OF_NWKS * 26) /*!< \brief Network key size in words */
#endif
#define CSR_MESH_MASP_RELAY_STATE_PSKEY_SIZE    (1)/*!< \brief Association messages relay state flag size in words */
#define CSR_MESH_MCP_SEQ_NUM_KEY_PSKEY_SIZE     (CSR_MESH_NO_OF_NWKS * 2)/*!< \brief Device last tx sequence number size in words */
#define CSR_MESH_MCP_GEN_SEQ_NUM_KEY_PSKEY_SIZE (CSR_MESH_MCP_SEQ_NUM_KEY_PSKEY_SIZE)
#define CSR_MESH_CONFIG_LAST_ETAG_PSKEY_SIZE    (4)/*!< \brief Device last e-tag size in words */
#define CSR_MESH_DEVICE_ID_KEY_PSKEY_SIZE       (1)/*!< \brief Device ID size in words  */
#define CSR_MESH_MTL_RELAY_STATUS_PSKEY_SIZE    (1)/*!< \brief Message relay status flag size in words */
#define CSR_MESH_DEVICE_DHM_PSKEY_SIZE          (12)/*!< \brief DHM Key size in words*/
#define CSR_MESH_NW_IV_PSKEY_SIZE               (4)/*!< \brief Network IV size in words */


/*----------------------------------------------------------------------------*
 * CSRmeshPsInit
 */
/*! \brief This method is called by the stack to initialize the PS interface
 *         if any.
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsInit(void);

/*----------------------------------------------------------------------------*
 * CSRmeshIsPsReadyForAccess
 */
/*! \brief CSRmesh stack calls this function to check if the persistant store
 *         contains valid values.
 *         The platform must validate the contents of the persistent store. 
 *         If the persistent store is corrupt or it is brought up for the
 *         first time, this function must return FALSE. True otherwise.
 *         If this function returns FALSE the stack will reset its PS values and
 *         writes them on the store.
 *
 *  \returns \ref TRUE if the contents of the Persistent store are valid.
 */
/*----------------------------------------------------------------------------*/
extern CsrBool CSRmeshIsPsReadyForAccess(void);

/*----------------------------------------------------------------------------*
 * CSRmeshPsRead
 */
/*! \brief This function is called to read a value associated with the key
 *         from the persistant store.
 *
 *  \param key   Identifier of the Persistent store parameter to be read
 *  \param p_value Pointer to a buffer to copy the value
 *  \param length  Length of the paramter in words
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsRead(CsrUint8 key, CsrUint16 *p_value, CsrUint16 length);

/*----------------------------------------------------------------------------*
 * CSRmeshPsWrite
 */
/*! \brief This function is called to write a value associated with the key
 *         from the persistant store.
 *
 *  \param key   Identifier of the Persistent store parameter to be written
 *  \param p_value Pointer to a buffer containing the value to be written
 *  \param length  Length of the paramter in words
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsWrite(CsrUint8 key, CsrUint16 *p_value, CsrUint16 length);

/*----------------------------------------------------------------------------*
 * CSRmeshPsSecureWrite
 */
/*! \brief This function is called to write a value associated with the key
 *         from a secure persistant store. This function is called for parameters
 *         such as passphrase, network keys and other security keys.<br>
 *         If the platform supports a secure storage, this value must be 
 *         stored on the secure store.
 *
 *  \param key     Identifier of the Persistent store parameter to be written
 *  \param p_value Pointer to a buffer containing the value to be written
 *  \param length  Length of the paramter in words
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsSecureWrite(CsrUint8 key, CsrUint16 *p_value, CsrUint16 length);

/*----------------------------------------------------------------------------*
 * CSRmeshPsSecureRead
 */
/*! \brief This function is called to read a value associated with the key
 *         from a secure persistant store.<br>
 *         If the platform supports a secure storage, this value must be 
 *         read from the secure store.
 *
 *  \param key     Identifier of the Persistent store parameter to be written
 *  \param p_value Pointer to a buffer to copy the value
 *  \param length  Length of the paramter in words
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsSecureRead(CsrUint8 key, CsrUint16 *valueBuffer, CsrUint16 length);

/*----------------------------------------------------------------------------*
 * CSRmeshPsDeInit
 */
/*! \brief This function is called when the CSRmesh stack is reset.
 *         This function may reset any variables initialized for the CSRmesh 
 *         persistent store API and free any allocated buffers.
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern void CSRmeshPsDeInit(void);

/*----------------------------------------------------------------------------*
 * CSRmeshPsReadSecureKey
 */
/*! \brief The CSRmesh stack obfuscates the secure paramters before writing 
 *         to the platform persistent store. 
 *         If the platform supports 
 *
 *  Application uses this API to generate MeshId
 *
 *  \param netId         Network id of the network for which MeshId generation
 *  is required.
 *  \param eventData     On successful completion a secret MeshId be
 *  available to the app.
 *   as a field in this 'result parameter'
 *
 *  \returns \ref CSRmeshResult
 */
/*----------------------------------------------------------------------------*/
extern CsrUint16 * CSRmeshPsReadSecureKey(void);

/*!@} */
#endif /* __CSR_MESH_PS_IFCE_H__ */

