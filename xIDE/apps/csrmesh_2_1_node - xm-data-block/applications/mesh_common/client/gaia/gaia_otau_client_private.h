/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia_otau_client_private.h
 * \brief Private defines for the GAIA OTAu module
 *
 */
#ifndef GAIA_OTAU_CLIENT_PRIVATE_H_
#define GAIA_OTAU_CLIENT_PRIVATE_H_

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <types.h>
#include <debug.h>
#include "user_config.h"
#include "cm_types.h"

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/


#define UPGRADE_HOSTACTION_YES                                  0
#define UPGRADE_HOST_TRANSFER_COMPLETE_RSP_BYTE_SIZE            1
#define UPGRADE_HOST_COMMIT_CFM_BYTE_SIZE                       1
#define VM_CONTROL_COMMAND_SIZE                                (3)
#define UPGRADE_MAX_DATA_BYTES_SENT                            (12) // (20 -8)
#define UPGRADE_HOST_START_DATA_REQ_SIZE                       (0) 
#define UPGRADE_HOST_INPROGRESS_RES_BYTE_SIZE                  (1)
#define UPGRADE_HOST_CONTINUE_UPGRADE                          (0)
#define UPGRADE_HOST_SYNC_REQ_BYTE_SIZE                        (4)
#define GAIA_VMUPGRADE_POST_TRANSFER_REBOOT_DELAY              (2000000)
#define CONN_INTERVAL                                          (2000000)

/* Bin File Header Offsets */
#define PARTITION_HEADER_BODY_OFFSET                           (12)
#define PARTITION_HEADER_COMPANY_CODE_OFFSET                   (16)
#define PARTITION_HEADER_PLATFORMTYPE_OFFSET                   (18)
#define PARTITION_HEADER_TYPEENCODING_OFFSET                   (19)
#define PARTITION_HEADER_IMAGETYPE_OFFSET                      (20)
#define PARTITION_HEADER_APP_VERSION_OFFSET                    (21) 
#define PARTITION_ID_OFFSET                                    (26)
#define PARTITION_LENGTH_OFFSET                                (34)

/* Bin File Footer Offsets */
#define PARTITION_FOOTER_LENGTH_OFFSET                         (8)
#define PARTITION_FOOTER_SIGNATURE_OFFSET                      (12)

/* Bin File Header Values */
#define HEADER_SIZE                                            (42)
#define MESH_HEADER_BODY_SIZE                                  (14)
#define PARTITION_INFO_SIZE                                     (8)

/* Bin File Footer Values */
#define FOOTER_SIZE                                            (44)
#define PARTITION_FOOTER_ID_SIZE                               (8)
#define PARTITION_FOOTER_LENGTH_SIZE                           (4)
#define PARTITION_FOOTER_SIGNATURE_LENGTH                      (32)

#ifdef DEBUG_ENABLE
#define DEBUG_STR(s)  DebugWriteString(s)
#define DEBUG_U32(u)  DebugWriteUint32(u)
#define DEBUG_U16(u)  DebugWriteUint16(u)
#define DEBUG_U8(u)   DebugWriteUint8(u)
#define DEBUG_TEST_U8_ARR(x,offset,n)  do { \
    uint16 debug_len = offset; \
    while( debug_len < n) \
    { \
        DebugWriteUint8(x[debug_len]); \
        debug_len++; \
    } \
}while(0)
#else

#define DEBUG_STR(s)
#define DEBUG_U32(u)
#define DEBUG_U16(u)
#define DEBUG_U8(u)
#define DEBUG_TEST_U8_ARR(x,offset,n)
#endif /* DEBUG_ENABLE */
    
/*=============================================================================*
 *  Private Data Types
 *============================================================================*/

extern void GaiaClientOtauInit(bool nvm_start_fresh, uint16 *nvm_offset);
extern void HandleUpgradeDisconnectAck(uint16 payload_length, uint8* payload);
extern void HandleUpgradeControlRequestAck(uint16 payload_length, uint8* payload);
extern void HandleUpgradeConnectAck(uint16 payload_length, uint8* payload);
extern void HandleResponseNotification(uint16 payload_len, uint8 *payload);
extern void GaiaClientOtauHandleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data);

#endif

#endif /* GAIA_OTAU_CLIENT_PRIVATE_H_ */
