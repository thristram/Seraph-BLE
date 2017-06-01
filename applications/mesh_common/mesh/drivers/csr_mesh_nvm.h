/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *****************************************************************************/
/*! \file csr_mesh_nvm.h
 *  \brief defines values and offsets for CSRmesh Stack NVM values
 *
 *   This file contains the function and definations for csr_mesh_ps_ifce.c
 */
 /*****************************************************************************/
#include "nvm_access.h"
#include "csr_mesh_ps_ifce.h"

/*============================================================================*
 *  Public Definitions 
 *============================================================================*/

#define GAIA_OTA_SERVICE_SIZE          (150)
#define MESH_APP_SERVICES_SIZE         (50)
#define CM_SIZE                        (60)

/* NVM offset for theGAIA OTA service */
#define NVM_OFFSET_GAIA_OTA_SERVICE    (0)

/* NVM offset for the Application services */
#define NVM_OFFSET_MESH_APP_SERVICES   (NVM_OFFSET_GAIA_OTA_SERVICE + GAIA_OTA_SERVICE_SIZE)

/* NVM offset for the CM Initialisation */
#define NVM_OFFSET_CM_INITIALISATION   (NVM_OFFSET_MESH_APP_SERVICES + MESH_APP_SERVICES_SIZE)

#define NVM_OFFSET_MESH_STACK          (NVM_OFFSET_CM_INITIALISATION + CM_SIZE)

/* Set the NVM offsets for the CSRmesh stack PS values */
#ifdef CSR101x_A05
#define CSR_MESH_NVM_SANITY_WORD_OFFSET      (0)
#else
#define CSR_MESH_NVM_SANITY_WORD_OFFSET      (NVM_OFFSET_MESH_STACK)
#endif

#define CSR_MESH_NVM_DEVICE_UUID_OFFSET      (CSR_MESH_NVM_SANITY_WORD_OFFSET + \
                                              CSR_MESH_NVM_SANITY_PSKEY_SIZE)

#define CSR_MESH_NVM_DEVICE_AUTH_CODE_OFFSET  (CSR_MESH_NVM_DEVICE_UUID_OFFSET + \
                                              CSR_MESH_DEVICE_UUID_PSKEY_SIZE)
                                            
#define CSR_MESH_MTL_NW_KEY_AVL_NVM_OFFSET   (CSR_MESH_NVM_DEVICE_AUTH_CODE_OFFSET + \
                                              CSR_MESH_DEVICE_AUTHCODE_PSKEY_SIZE)

#define CSR_MESH_MTL_MCP_NW_KEY_NVM_OFFSET   (CSR_MESH_MTL_NW_KEY_AVL_NVM_OFFSET + \
                                              CSR_MESH_MTL_NW_KEY_AVL_PSKEY_SIZE)

#define CSR_MESH_MASP_RELAY_STATE_NVM_OFFSET (CSR_MESH_MTL_MCP_NW_KEY_NVM_OFFSET + \
                                              CSR_MESH_MTL_MCP_NWKEY_PSKEY_SIZE)

#define CSR_MESH_MCP_SEQ_NUM_NVM_OFFSET      (CSR_MESH_MASP_RELAY_STATE_NVM_OFFSET + \
                                              CSR_MESH_MASP_RELAY_STATE_PSKEY_SIZE)

#define CSR_MESH_MCP_GEN_SEQ_NUM_NVM_OFFSET  (CSR_MESH_MCP_SEQ_NUM_NVM_OFFSET + \
                                              CSR_MESH_MCP_SEQ_NUM_KEY_PSKEY_SIZE)

#define CSR_MESH_CONFIG_LAST_ETAG_NVM_OFFSET (CSR_MESH_MCP_GEN_SEQ_NUM_NVM_OFFSET + \
                                              CSR_MESH_MCP_GEN_SEQ_NUM_KEY_PSKEY_SIZE)

#define CSR_MESH_DEVICE_ID_OFFSET            (CSR_MESH_CONFIG_LAST_ETAG_NVM_OFFSET + \
                                              CSR_MESH_CONFIG_LAST_ETAG_PSKEY_SIZE)

#define CSR_MESH_MTL_RELAY_STATUS_NVM_OFFSET (CSR_MESH_DEVICE_ID_OFFSET + \
                                              CSR_MESH_DEVICE_ID_KEY_PSKEY_SIZE)

#define CSR_MESH_DHM_KEY_NVM_OFFSET          (CSR_MESH_MTL_RELAY_STATUS_NVM_OFFSET + \
                                              CSR_MESH_MTL_RELAY_STATUS_PSKEY_SIZE)

#define CSR_MESH_IV_NVM_OFFSET               (CSR_MESH_DHM_KEY_NVM_OFFSET + \
                                              CSR_MESH_DEVICE_DHM_PSKEY_SIZE)

#define CSR_MESH_NVM_SIZE                    (CSR_MESH_IV_NVM_OFFSET + \
                                              CSR_MESH_NW_IV_PSKEY_SIZE)
