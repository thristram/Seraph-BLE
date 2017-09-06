/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *****************************************************************************/
/*! \file csr_mesh_ps_ifce.c
 *  \brief defines and functions for csr_mesh_ps_ifce.c
 *
 *   This file contains the function and definations for csr_mesh_ps_ifce.c
 */
 /*****************************************************************************/
#include "csr_mesh_nvm.h"

/*============================================================================*
 *  Private Types
 *============================================================================*/

typedef struct
{
    CsrUint8 key;
    CsrUint16 offset;
}CSR_MESH_PSKEY_OFFSET_T;


/*============================================================================*
 *  Private Definitions
 *============================================================================*/


/* Size of the NVM Offsets table */
#define SIZE_OF_TABLE (sizeof(csr_mesh_pskey_offset_table)/sizeof(CSR_MESH_PSKEY_OFFSET_T))

/* CSRmesh NVM sanity magic  */
#define CSR_MESH_NVM_SANITY_WORD              0x0050

/*============================================================================*
 *  Private Data
 *============================================================================*/

static CSR_MESH_PSKEY_OFFSET_T csr_mesh_pskey_offset_table[] = 
{
    {CSR_MESH_NVM_SANITY_KEY,       CSR_MESH_NVM_SANITY_WORD_OFFSET},
    {CSR_MESH_DEVICE_UUID_KEY,      CSR_MESH_NVM_DEVICE_UUID_OFFSET},
    {CSR_MESH_DEVICE_AUTHCODE_KEY,  CSR_MESH_NVM_DEVICE_AUTH_CODE_OFFSET},
    {CSR_MESH_MTL_NW_KEY_AVL_KEY,   CSR_MESH_MTL_NW_KEY_AVL_NVM_OFFSET},
    {CSR_MESH_MTL_MCP_NWKEY_KEY,    CSR_MESH_MTL_MCP_NW_KEY_NVM_OFFSET},
    {CSR_MESH_MASP_RELAY_STATE_KEY, CSR_MESH_MASP_RELAY_STATE_NVM_OFFSET},
    {CSR_MESH_MCP_SEQ_NUM_KEY,      CSR_MESH_MCP_SEQ_NUM_NVM_OFFSET},
    {CSR_MESH_MCP_GEN_SEQ_NUM_KEY,  CSR_MESH_MCP_GEN_SEQ_NUM_NVM_OFFSET},
    {CSR_MESH_CONFIG_LAST_ETAG_KEY, CSR_MESH_CONFIG_LAST_ETAG_NVM_OFFSET},
#if (CSR_MESH_NO_OF_NWKS == 1)
    {CSR_MESH_DEVICE_ID_KEY,        CSR_MESH_DEVICE_ID_OFFSET},
    {CSR_MESH_MTL_RELAY_STATUS_KEY, CSR_MESH_MTL_RELAY_STATUS_NVM_OFFSET},
    {CSR_MESH_DEVICE_DHM_KEY,       CSR_MESH_DHM_KEY_NVM_OFFSET},
    {CSR_MESH_NW_IV_KEY,            CSR_MESH_IV_NVM_OFFSET},
#endif
};


/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      getPsKeyOffset
 *
 *  DESCRIPTION
 *      Initialise Persistent Store
 *
 *  RETURNS/MODIFIES
 *      CSRmeshResult - Indicating if PS has been initialised
 *      successfully
 *
 *----------------------------------------------------------------------------*/
static CsrUint16 getPsKeyOffset(CsrUint8 key)
{
    CsrUint8 count;
    for(count = 0; count < SIZE_OF_TABLE; count++)
    {
        if(csr_mesh_pskey_offset_table[count].key == key)
        {
            break;
        }
    }
    return csr_mesh_pskey_offset_table[count].offset;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsInit
 *
 *  DESCRIPTION
 *      Initialise Persistent Store
 *
 *  RETURNS/MODIFIES
 *      CSRmeshResult - Indicating if PS has been initialised
 *      successfully
 *
 *----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsInit(void)
{
    /* No initialization required here as the NVM is already initialized 
     * by the application. Return SUCCESS.
     */
    return CSR_MESH_RESULT_SUCCESS;
}


/*----------------------------------------------------------------------------*
 * NAME 
 *     CSRmeshIsPsReadyForAccess
 *
 * DESCRIPTION
 *     This function returns TRUE if the PS is ready for access
 *
 * RETURNS/Modifies 
 *     TRUE if PS is ready for access
 *
 *----------------------------------------------------------------------------*/
extern CsrBool CSRmeshIsPsReadyForAccess()
{
    CsrBool result = FALSE;
    CsrUint16 csrmesh_nvm_sanity;
    
    Nvm_Read(&csrmesh_nvm_sanity, 1, CSR_MESH_NVM_SANITY_WORD_OFFSET);
    
    if( csrmesh_nvm_sanity == CSR_MESH_NVM_SANITY_WORD)
    {
        result = TRUE;
    }
    else
    {
        csrmesh_nvm_sanity = CSR_MESH_NVM_SANITY_WORD;
        Nvm_Write(&csrmesh_nvm_sanity, 1, CSR_MESH_NVM_SANITY_WORD_OFFSET);
    }
    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsReadSecureKey
 *
 *  DESCRIPTION
 *      Read the encryption key from secure store
 *
 *  RETURNS/MODIFIES
 *      CsrUint16 * - Returns pointer to the secure key, present in application
 *
 *----------------------------------------------------------------------------*/
extern CsrUint16 * CSRmeshPsReadSecureKey(void)
{
    return Nvm_Read_Secure_Key();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsRead
 *
 *  DESCRIPTION
 *      Read from Persistent Store
 *
 *  RETURNS/MODIFIES
 *      CSRmeshResult - Indicating if PS-read was successful or not
 *
 *----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsRead(CsrUint8 key, CsrUint16 *valueBuffer, CsrUint16 length)
{
    Nvm_Read(valueBuffer, length, getPsKeyOffset(key));
    return CSR_MESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsSecureRead
 *
 *  DESCRIPTION
 *      Read from Persistent Store
 *
 *  RETURNS/MODIFIES
 *      CSRmeshResult - Indicating if PS-read was successful or not
 *
 *----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsSecureRead(CsrUint8 key, CsrUint16 *valueBuffer, CsrUint16 length)
{
    /* There is no special secure memory on CSR10xx platform so storing the
     * the values in the same NVM as other values.
     */
    Nvm_Read(valueBuffer, length, getPsKeyOffset(key));
    return CSR_MESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsWrite
 *
 *  DESCRIPTION
 *      Write to Persistent Store
 *
 *  RETURNS/MODIFIES
 *      CSRmeshResult - Indicating if PS-write was successful or not
 *
 *----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsWrite(CsrUint8 key, CsrUint16 *valueBuffer, CsrUint16 length)
{
    Nvm_Write(valueBuffer, length, getPsKeyOffset(key));
    return CSR_MESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsSecureWrite
 *
 *  DESCRIPTION
 *      Securely Write to Persistent Store
 *
 *  RETURNS/MODIFIES
 *      CSRmeshResult - Indicating if PS-write was successful or not
 *
 *----------------------------------------------------------------------------*/
extern CSRmeshResult CSRmeshPsSecureWrite(CsrUint8 key, CsrUint16 *valueBuffer, CsrUint16 length)
{
    /* There is no special secure memory on CSR10xx platform so storing the
     * the values in the same NVM 
     */
    Nvm_Write(valueBuffer, length, getPsKeyOffset(key));
    return CSR_MESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRmeshPsDeInit
 *
 *  DESCRIPTION
 *      De-Initialise Persistent Store
 *
 *  RETURNS/MODIFIES
 *      void
 *----------------------------------------------------------------------------*/
extern void CSRmeshPsDeInit(void)
{
    /* Nothing to be done here */
}

