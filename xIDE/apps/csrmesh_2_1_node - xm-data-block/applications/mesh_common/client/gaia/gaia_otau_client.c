/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gaia_otau_client.c
 *
 *  DESCRIPTION
 *      Upgrade Client library API implementation.
 *
 *****************************************************************************/
#ifdef GAIA_OTAU_RELAY_SUPPORT

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>
#include <gatt_prim.h>
#include <battery.h>
#include <buf_utils.h>
#include <mem.h>
#include <timer.h>
#include <storage.h>
#include <store_update.h>
#include <configstore_id.h>
#include <configstore_api.h>
#include <store_update_msg.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "gaia_client.h"
#include "gaia.h"
#include "byte_utils.h"
#include "gaia_otau_private.h"
#include "gaia_otau_client_private.h"
#include "gaia_otau_client_api.h"
#include "gaia_client_service.h"
#include "gaia_client_service_event.h"
#include "cm_types.h"
#include "cm_api.h"
#include "nvm_access.h"

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/

/* NVM Offsets for Upgrade Client Library */
#define NVM_PREV_HEADER_BODY_OFFSET          0
#define NVM_PREV_PARTITION_INFO_OFFSET       MESH_HEADER_BODY_SIZE
#define NVM_PREV_FOOTER_SIGNATURE_OFFSET     NVM_PREV_PARTITION_INFO_OFFSET + \
                                             PARTITION_INFO_SIZE
#define NVM_CUR_HEADER_BODY_OFFSET           NVM_PREV_FOOTER_SIGNATURE_OFFSET +\
                                             PARTITION_FOOTER_SIGNATURE_LENGTH
#define NVM_CUR_PARTITION_INFO_OFFSET        NVM_CUR_HEADER_BODY_OFFSET + MESH_HEADER_BODY_SIZE
#define NVM_CUR_FOOTER_SIGNATURE_OFFSET      NVM_CUR_PARTITION_INFO_OFFSET + \
                                             PARTITION_INFO_SIZE
                                             
#define NVM_OTAU_PREV_STORE_ID_OFFSET        NVM_CUR_FOOTER_SIGNATURE_OFFSET +\
                                             PARTITION_FOOTER_SIGNATURE_LENGTH
#define NVM_OTAU_PREV_STORE_TYPE_OFFSET      NVM_OTAU_PREV_STORE_ID_OFFSET + sizeof(uint16)
#define NVM_OTAU_PREV_PARTITION_SIZE_OFFSET  NVM_OTAU_PREV_STORE_TYPE_OFFSET + sizeof(uint16)
                                             
#define NVM_OTAU_CUR_STORE_ID_OFFSET         NVM_OTAU_PREV_PARTITION_SIZE_OFFSET +\
                                             sizeof(otaData.prev_partition_data_length)
#define NVM_OTAU_CUR_STORE_TYPE_OFFSET       NVM_OTAU_CUR_STORE_ID_OFFSET + sizeof(uint16)
#define NVM_OTAU_CUR_PARTITION_SIZE_OFFSET   NVM_OTAU_CUR_STORE_TYPE_OFFSET + sizeof(uint16)
#define NVM_OTAU_COMMIT_DONE_OFFSET          NVM_OTAU_CUR_PARTITION_SIZE_OFFSET + sizeof(otaData.cur_partition_data_length)
#define NVM_OTAU_CALCULATE_HASH_OFFSET       NVM_OTAU_COMMIT_DONE_OFFSET  + sizeof(otaData.commit_done)
#define GAIA_OTAU_CLIENT_SERVICE_NVM_MEMORY_WORDS    NVM_OTAU_CALCULATE_HASH_OFFSET  +\
                                                      sizeof(otaData.calculate_hash)
                                                      
/*============================================================================*
 *  Private Data Types
 *============================================================================*/

/* This structure defines the local data store for this module */
typedef struct {   
    memory_address_t prev_partition_data_length; 
    memory_address_t cur_partition_data_length; 
    uint16 nvm_offset;                      
    uint32 app_current_offset;
    uint32 header_current_offset;
    uint32 footer_current_offset;
    GAIA_VMUPGRADE_STATE mState;
    uint8 prev_header[HEADER_SIZE];
    uint8 prev_footer[FOOTER_SIZE];
    uint8 cur_header[HEADER_SIZE];
    uint8 cur_footer[FOOTER_SIZE];
    uint32 m_inprogress_id;   
    bool m_timeout_is_set;
    timer_id   ota_tid;
    bool m_request_reached_eof;
    uint16 cur_store_id;
    uint16 cur_store_type;
    uint16 prev_store_id;
    uint16 prev_store_type;
    struct {
        uint16 sha256_sum[SHA256_SIZE_WORDS];       /* sum of all the SHA256s */
        uint16 sha256_buffer[SHA256_SIZE_WORDS];    /* SHA256 that is calculated from the Flash */
        uint32 length;                      /* expected OEM signature length */
    } oem_signature;
    uint32 requested_num_bytes;
    timer_id announce_tid;
    store_id_t cs_id;
    store_id_t app_id;
    bool app_connected;
    timer_id conn_param_tid;
    uint8 announce_count;
    bool commit_done;
    bool calculate_hash;
    uint8 footer_signature[PARTITION_FOOTER_SIGNATURE_LENGTH];
    bool store_hash_request_sent;
    gaia_otau_client_event_handler callback;       /* callback to the application event handler */
} OTA_SERVICE_LOCAL_DATA_T;


/*=============================================================================*
 *  Private Data
 *============================================================================*/

/* This local data store for this module */
static OTA_SERVICE_LOCAL_DATA_T otaData;

/* App Store Handle */
static handle_t store_handle;

/* LOT Announce data */
LARGEOBJECTTRANSFER_ANNOUNCE_T lot_announce_data;

/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      resetFileOffsets
 *
 *  DESCRIPTION
 *      This function resets the file offsets for new upgrade
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void resetFileOffsets(void)
{
    otaData.app_current_offset = 0;
    otaData.footer_current_offset = 0;
    otaData.header_current_offset = 0;
    otaData.m_request_reached_eof = FALSE;
    otaData.m_timeout_is_set = FALSE; 
    TimerDelete(otaData.ota_tid);
    TimerDelete(otaData.conn_param_tid);
    otaData.requested_num_bytes = 0;
    otaData.conn_param_tid = TIMER_INVALID;
    otaData.announce_count = 0;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      notifyApplication
 *
 *  DESCRIPTION
 *      Notify the application of OTAuclient  events via the callback function.
 *
 *  PARAMETERS
 *      event [in]      Type of event to notify
 *      *data [in,out]  Pointer to any event-specific data that should be sent
 *
 *  RETURNS
 *      Return status of the callback
 *
 *---------------------------------------------------------------------------*/
static sys_status notifyApplication(gaia_otau_client_event event, GAIA_OTAU_CLIENT_EVENT_T *data)
{
    if (otaData.callback != NULL)
    {
        return otaData.callback(event, data);
    }

    return sys_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleStartCfm
 *
 *  DESCRIPTION
 *      This function handles the Start Confirm command
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleStartCfm(uint16 payload_len, uint8 *payload)
{
    if (payload_len == UPGRADE_HOST_START_CFM_PAYLOAD_SIZE)
    {
        uint8 status = payload[0];
        //uint16 batteryLevel = GetUint16FromArray(&payload[1]);

        if (otaData.mState == STATE_VM_UPGRADE_START_REQ)
        {
            // Acknowledge
            SetAcknowledgement(GAIA_STATUS_SUCCESS);

            DEBUG_STR("\r\nStart Confirm Command Received");
            
            if (status == UPGRADE_HOST_SUCCESS)
            {
                // Application ready for upgrade,send start data request
                uint16 response_length,byte_index;
                uint8 response[VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_START_DATA_REQ_SIZE];
                response_length = UPGRADE_HOST_START_DATA_REQ_SIZE;
                
                byte_index = 0;
                byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_START_DATA_REQ);
                byte_index += SetUint16InArray(response,byte_index,response_length);
                GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_START_DATA_REQ_SIZE,
                                            response);
                                      
                otaData.mState = STATE_VM_UPGRADE_DATA_REQ;
                
                DEBUG_STR("\r\nStart Data Request Command Sent");
            }
        }
        else if (otaData.mState == STATE_VM_UPGRADE_START_REQ_AFTER_REBOOT)
        {
            // Acknowledge
            SetAcknowledgement(GAIA_STATUS_SUCCESS);

            DEBUG_STR("\r\nStart Confirm Command Post Reboot Received");
            if (status == UPGRADE_HOST_SUCCESS)
            {
                // Application ready for commit then send InProgressRes message with ABORT = N
                uint16 response_length,byte_index;
                uint8 response[VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_INPROGRESS_RES_BYTE_SIZE];
                response_length = UPGRADE_HOST_INPROGRESS_RES_BYTE_SIZE;
                
                byte_index = 0;
                byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_IN_PROGRESS_RES);
                byte_index += SetUint16InArray(response,byte_index,response_length);
                byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_CONTINUE_UPGRADE);
                GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_INPROGRESS_RES_BYTE_SIZE,
                                            response);
                otaData.mState = STATE_VM_UPGRADE_IN_PROGRESS_REQ;
                DEBUG_STR("\r\nIn Progress Response Command Sent");
            }
        }
        else
        {
            SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
        }
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeSendData
 *
 *  DESCRIPTION
 *      This function sends the upgrade data
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeSendData(uint32 requested_num_bytes)
{
    uint8 upgrade_data[UPGRADE_MAX_DATA_BYTES_SENT + 1];
    upgrade_data[0] = 0;
    if(requested_num_bytes > UPGRADE_MAX_DATA_BYTES_SENT)
    {
        requested_num_bytes = UPGRADE_MAX_DATA_BYTES_SENT;
        otaData.requested_num_bytes -= UPGRADE_MAX_DATA_BYTES_SENT;
    }
    else
    {
        otaData.requested_num_bytes = 0;
    }
    if(otaData.header_current_offset < HEADER_SIZE)
    {
        uint32 remaining_header = HEADER_SIZE - otaData.header_current_offset;
        if(requested_num_bytes <= remaining_header)
        {
            MemCopy(&upgrade_data[1],&otaData.cur_header[otaData.header_current_offset],requested_num_bytes);
            otaData.header_current_offset += requested_num_bytes;
            if(requested_num_bytes == remaining_header)          
                    Storage_FindStore(otaData.cur_store_id, otaData.cur_store_type, &store_handle);
         }            
     }
    else if(otaData.app_current_offset < otaData.cur_partition_data_length)
        {
            uint32 remaining_app = otaData.cur_partition_data_length - otaData.app_current_offset;
            
            if(requested_num_bytes <= remaining_app)
            {
                uint16 buffer[6];
                uint8 temp[12];
                uint16 num_bytes = (requested_num_bytes/2);
                uint32 word_offset = (otaData.app_current_offset/2);
                Storage_BlockRead(store_handle,buffer,word_offset,&num_bytes);
                MemCopyUnPack(temp,buffer,requested_num_bytes);
                SwapBytes(requested_num_bytes, temp, &upgrade_data[1]);
                otaData.app_current_offset += requested_num_bytes;
            }   
        }
        else if(otaData.footer_current_offset < FOOTER_SIZE)
        {
            uint32 remaining_footer = FOOTER_SIZE - otaData.footer_current_offset;
            if(requested_num_bytes <= remaining_footer)
            {
                MemCopy(&upgrade_data[1],&otaData.cur_footer[otaData.footer_current_offset],requested_num_bytes);
                otaData.footer_current_offset += requested_num_bytes;
                if(otaData.footer_current_offset == FOOTER_SIZE)
                {
                    otaData.m_request_reached_eof = TRUE;
                    upgrade_data[0] =  0x01;
                    DEBUG_STR("\r\nReached End of File ");
                }
            } 
        }

        uint16 response_length,byte_index;
        uint8 response[VM_CONTROL_COMMAND_SIZE + UPGRADE_MAX_DATA_BYTES_SENT + 1];
        response_length = requested_num_bytes + 1;
         byte_index = 0;
         byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_DATA);
         byte_index += SetUint16InArray(response,byte_index,response_length);
         MemCopy(&response[byte_index],upgrade_data,requested_num_bytes + 1);
         GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE + requested_num_bytes + 1,
                                     response);
         otaData.mState = STATE_VM_UPGRADE_TRANSFER;
         DEBUG_STR("\r\nData Bytes Sent");
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleDataBytesReq
 *
 *  DESCRIPTION
 *      This function handles the data bytes request
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleDataBytesReq(uint16 payload_len, uint8 *payload)
{
    DEBUG_STR("\r\nData Bytes Request Received");
    if (otaData.mState == STATE_VM_UPGRADE_DATA_REQ)
    {
        if (payload_len == UPGRADE_HOST_DATA_BYTES_REQ_BYTE_SIZE)
        {
            otaData.requested_num_bytes = GetUint32FromArray(&payload[0]);
            //uint32 requested_data_offset = GetUint32FromArray(&payload[4]);
            otaData.m_request_reached_eof = FALSE;
        
           if ((otaData.header_current_offset + otaData.app_current_offset +
                otaData.footer_current_offset + otaData.requested_num_bytes) <= 
               (otaData.cur_partition_data_length + HEADER_SIZE + FOOTER_SIZE))
            {
                SetAcknowledgement(GAIA_STATUS_SUCCESS);
                upgradeSendData(otaData.requested_num_bytes);
            }
           else
           {
                SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
           }
        }
        else
        {
           SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
        }
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleAbortCfm
 *
 *  DESCRIPTION
 *      This function handles the abort confirm
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleAbortCfm(uint16 payload_len, uint8 *payload)
{
    SetAcknowledgement(GAIA_STATUS_SUCCESS);
    otaData.mState = STATE_VM_UPGRADE_ABORTING_DISCONNECT;
    GaiaClientSendDisconnectPacket();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleTransferCompleteInd
 *
 *  DESCRIPTION
 *      This function handles the transfer complete indication
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleTransferCompleteInd(uint16 payload_len, uint8 *payload)
{
    if (otaData.mState == STATE_VM_UPGRADE_TRANSFER ||
        otaData.mState == STATE_VM_UPGRADE_WAIT_VALIDATION_RSP || 
                        otaData.mState == STATE_VM_UPGRADE_WAIT_VALIDATION)
    {
        if(otaData.mState == STATE_VM_UPGRADE_WAIT_VALIDATION && 
           otaData.ota_tid != TIMER_INVALID)
        {
            otaData.m_timeout_is_set = FALSE;
            TimerDelete(otaData.ota_tid);
            otaData.ota_tid = TIMER_INVALID;
        }
        
        DEBUG_STR("\r\nTransfer Complete Indication Received ");  
        // device indicated that it has successfully received and validated all image data
        SetAcknowledgement(GAIA_STATUS_SUCCESS);

        // Send Transfer Complete Res message, continue with upgrade = TRUE
        uint16 response_length,byte_index;
        uint8 response[VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_TRANSFER_COMPLETE_RSP_BYTE_SIZE];
        response_length = UPGRADE_HOST_TRANSFER_COMPLETE_RSP_BYTE_SIZE;
                
        byte_index = 0;
        byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_TRANSFER_COMPLETE_RES);
        byte_index += SetUint16InArray(response,byte_index,response_length);
        byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOSTACTION_YES);
        GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_TRANSFER_COMPLETE_RSP_BYTE_SIZE,
                                            response);
        otaData.mState = STATE_VM_UPGRADE_WAIT_POST_TRANSFER_DISCONNECT;
        DEBUG_STR("\r\nTransfer Complete Response Sent ");
        
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleCommitReq
 *
 *  DESCRIPTION
 *      This function sends the commit confirm
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleCommitReq(uint16 payload_len, uint8 *payload)
{
    if (otaData.mState == STATE_VM_UPGRADE_IN_PROGRESS_REQ)
    {
        // Acknowledge
        SetAcknowledgement(GAIA_STATUS_SUCCESS);

        DEBUG_STR("\r\nCommit Request Received ");
        // Send Transfer Complete Res message, continue with upgrade = TRUE
        uint16 response_length,byte_index;
        uint8 response[VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_COMMIT_CFM_BYTE_SIZE];
        response_length = UPGRADE_HOST_COMMIT_CFM_BYTE_SIZE;
                
        byte_index = 0;
        byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_COMMIT_CFM);
        byte_index += SetUint16InArray(response,byte_index,response_length);
        byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_CONTINUE_UPGRADE);
        GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_COMMIT_CFM_BYTE_SIZE,
                                            response);
        otaData.mState = STATE_VM_UPGRADE_COMMIT_REQ;
        DEBUG_STR("\r\nCommit Confirm Sent ");

    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeHandleErrorWarnInd
 *
 *  DESCRIPTION
 *      This function handles the error and warning indication
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeHandleErrorWarnInd(uint16 payload_len, uint8 *payload)
{
    if (payload_len == UPGRADE_HOST_ERRORWARN_IND_BYTE_SIZE)
    {
        DEBUG_STR("\r\nError Indication Received ");
        SetAcknowledgement(GAIA_STATUS_SUCCESS);

        uint16 error_code = GetUint16FromArray(payload);
        // Send Transfer Complete Res message, continue with upgrade = TRUE
        if (error_code != UPGRADE_HOST_SUCCESS)
        {
            //Error code is not success, abort and reset
            otaData.mState = STATE_VM_UPGRADE_IDLE;
            GaiaDeviceDisconnect();
        }
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleCompleteInd
 *
 *  DESCRIPTION
 *      This function indicates that update has been completed
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleCompleteInd(uint16 payload_len, uint8 *payload)
{
    if (otaData.mState == STATE_VM_UPGRADE_COMMIT_REQ)
    {
        DEBUG_STR("\r\nTransfer Completed");
        SetAcknowledgement(GAIA_STATUS_SUCCESS);

        // Upgrade completed,send GAIA Disconnect
        GaiaClientSendDisconnectPacket();
        otaData.mState = STATE_VM_UPGRADE_DISCONNECT;
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendStartReq
 *
 *  DESCRIPTION
 *      This function handles the sync confirm command
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendStartReq(void)
{
    uint16 response_length,byte_index;
    uint8 response[VM_CONTROL_COMMAND_SIZE];
    response_length = 0;      
    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_START_REQ);
    byte_index += SetUint16InArray(response,byte_index,response_length);
    GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE,response);
    DEBUG_STR("\r\nStart Request Sent ");                   
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnTimerExpiry
 *
 *  DESCRIPTION
 *      This function handles the connection wait timer expiry
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void handleConnTimerExpiry(timer_id tid)
{
    if(tid == otaData.conn_param_tid)
    {
        upgradeDeviceSendStartReq();
        
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleSyncCfm
 *
 *  DESCRIPTION
 *      This function handles the sync confirm command
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleSyncCfm(uint16 payload_len, uint8 *payload)
{
    if (otaData.mState == STATE_VM_UPGRADE_SYNC_REQ)
    {
        if (payload_len == UPGRADE_HOST_SYNC_CFM_BYTE_SIZE)
        {
            uint8 resumePoint = payload[0];
            uint32 inProgressId = GetUint32FromArray(&payload[1]);
            uint8 procotolVersion = payload[5];
            
            if(procotolVersion == PROTOCOL_VERSION_3)
            {
                if(inProgressId == otaData.m_inprogress_id)
                {
                    DEBUG_STR("\r\nSync Confirm Received ");;
                    SetAcknowledgement(GAIA_STATUS_SUCCESS);
                    otaData.app_current_offset = 0;
                    otaData.header_current_offset = 0;
                    otaData.footer_current_offset = 0;
                    
                    if(resumePoint == UPGRADE_RESUME_POINT_POST_REBOOT)
                        otaData.mState = STATE_VM_UPGRADE_START_REQ_AFTER_REBOOT;
                    else if(resumePoint == UPGRADE_RESUME_POINT_START)
                        otaData.mState = STATE_VM_UPGRADE_START_REQ;
                    otaData.conn_param_tid = TimerCreate(CONN_INTERVAL,TRUE,
                                                         handleConnTimerExpiry);                    
                }
                else
                {
                    SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
                }
            }
            else
            {
                SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
            }
        }
        else
        {
            SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
        }
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeSendIsValidationDoneRequest
 *
 *  DESCRIPTION
 *      This function sends the validation done request
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeSendIsValidationDoneRequest(void)
{
    DEBUG_STR("\nSend Validation Request");
    uint16 response_length,byte_index;
    uint8 response[VM_CONTROL_COMMAND_SIZE];
    response_length = 0;
                
    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_IS_CSR_VALID_DONE_REQ);
    byte_index += SetUint16InArray(response,byte_index,response_length);
    GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE,response);
    
    otaData.mState = STATE_VM_UPGRADE_WAIT_VALIDATION_RSP;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleValidationTimerExpiry
 *
 *  DESCRIPTION
 *      This function handles the expiry of validation timer
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void handleValidationTimerExpiry(timer_id tid)
{
    if(otaData.ota_tid == tid && otaData.m_timeout_is_set)
    {
        otaData.m_timeout_is_set = FALSE;
        if(otaData.mState == STATE_VM_UPGRADE_WAIT_VALIDATION)
        {     
            DEBUG_STR("\r\nIs Validation Done Request Sent");
            upgradeSendIsValidationDoneRequest();   
        }
        else if(otaData.mState == STATE_VM_UPGRADE_WAIT_POST_TRANSFER_DISCONNECT)
        {
            device_handle_id device_id;
            if(GaiaClientIsConnected(&device_id))
            {
                DEBUG_STR("\r\nSending Upgrade Connect Request");
                GaiaClientSendConnectPacket();  
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceHandleIsValidationDoneCfm
 *
 *  DESCRIPTION
 *      This function handles the validation done confirmation
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceHandleIsValidationDoneCfm(uint16 payload_len, uint8 *payload)
{
    if (otaData.mState == STATE_VM_UPGRADE_WAIT_VALIDATION_RSP)
    {
        if (payload_len == UPGRADE_HOST_IS_CSR_VALID_DONE_CFM_BYTE_SIZE)
        {
            DEBUG_STR("\r\nValidation Done Confirm Received");
            uint8 back_off_time_ms = GetUint16FromArray(payload);
            SetAcknowledgement(GAIA_STATUS_SUCCESS);
            otaData.mState = STATE_VM_UPGRADE_WAIT_VALIDATION;
            otaData.ota_tid = 
                TimerCreate(back_off_time_ms * 1000,TRUE, 
                            handleValidationTimerExpiry);
        }
        else
        {
            SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
        }
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INCORRECT_STATE);
    }
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      readDataFromNVM
 *
 *  DESCRIPTION
 *      This function is used to read GAIA client service specific data store in NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset )
{
    otaData.nvm_offset = *nvm_offset;

    if(!nvm_start_fresh)
    {
         /* Read GAIA OTAu Header */
         Nvm_Read((uint16*)&otaData.prev_header[PARTITION_HEADER_BODY_OFFSET],
                 MESH_HEADER_BODY_SIZE,
                 otaData.nvm_offset + NVM_PREV_HEADER_BODY_OFFSET);
        
         Nvm_Read((uint16*)&otaData.prev_header[PARTITION_LENGTH_OFFSET],
              PARTITION_INFO_SIZE,
              otaData.nvm_offset + NVM_PREV_PARTITION_INFO_OFFSET);
        
         Nvm_Read((uint16*)&otaData.prev_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
                 PARTITION_FOOTER_SIGNATURE_LENGTH,
                 otaData.nvm_offset + NVM_PREV_FOOTER_SIGNATURE_OFFSET);
         
         Nvm_Read((uint16*)&otaData.cur_header[PARTITION_HEADER_BODY_OFFSET],
                 MESH_HEADER_BODY_SIZE,
                 otaData.nvm_offset + NVM_CUR_HEADER_BODY_OFFSET);
        
         Nvm_Read((uint16*)&otaData.cur_header[PARTITION_LENGTH_OFFSET],
              PARTITION_INFO_SIZE,
              otaData.nvm_offset + NVM_CUR_PARTITION_INFO_OFFSET);
        
         Nvm_Read((uint16*)&otaData.cur_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
                 PARTITION_FOOTER_SIGNATURE_LENGTH,
                 otaData.nvm_offset + NVM_CUR_FOOTER_SIGNATURE_OFFSET);
         
         Nvm_Read((uint16*)&otaData.cur_store_id,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_ID_OFFSET);
         
         Nvm_Read((uint16*)&otaData.cur_store_type,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_TYPE_OFFSET);
        
         Nvm_Read((uint16*)&otaData.cur_partition_data_length,
              sizeof(otaData.cur_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_CUR_PARTITION_SIZE_OFFSET);
         
         Nvm_Read((uint16*)&otaData.prev_store_id,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_PREV_STORE_ID_OFFSET);
         
         Nvm_Read((uint16*)&otaData.prev_store_type,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_PREV_STORE_TYPE_OFFSET);
        
         Nvm_Read((uint16*)&otaData.prev_partition_data_length,
              sizeof(otaData.prev_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_PREV_PARTITION_SIZE_OFFSET);
         
         Nvm_Read((uint16*)&otaData.commit_done,
              sizeof(otaData.commit_done),
              otaData.nvm_offset + NVM_OTAU_COMMIT_DONE_OFFSET);
         
         Nvm_Read((uint16*)&otaData.calculate_hash,
              sizeof(otaData.calculate_hash),
              otaData.nvm_offset + NVM_OTAU_CALCULATE_HASH_OFFSET);
    }
    else
    {
        /* Write GAIA OTAu InProgressIdentifier config */
        Nvm_Write((uint16*)&otaData.prev_header[PARTITION_HEADER_BODY_OFFSET],
                 MESH_HEADER_BODY_SIZE,
                 otaData.nvm_offset + NVM_PREV_HEADER_BODY_OFFSET);
        
        Nvm_Write((uint16*)&otaData.prev_header[PARTITION_LENGTH_OFFSET],
              PARTITION_INFO_SIZE,
              otaData.nvm_offset + NVM_PREV_PARTITION_INFO_OFFSET);
        
        Nvm_Write((uint16*)&otaData.prev_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
                 PARTITION_FOOTER_SIGNATURE_LENGTH,
                 otaData.nvm_offset + NVM_PREV_FOOTER_SIGNATURE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_header[PARTITION_HEADER_BODY_OFFSET],
                 MESH_HEADER_BODY_SIZE,
                 otaData.nvm_offset + NVM_CUR_HEADER_BODY_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_header[PARTITION_LENGTH_OFFSET],
              PARTITION_INFO_SIZE,
              otaData.nvm_offset + NVM_CUR_PARTITION_INFO_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
                 PARTITION_FOOTER_SIGNATURE_LENGTH,
                 otaData.nvm_offset + NVM_CUR_FOOTER_SIGNATURE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_store_id,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_ID_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_store_type,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_TYPE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_partition_data_length,
              sizeof(otaData.cur_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_CUR_PARTITION_SIZE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.prev_store_id,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_PREV_STORE_ID_OFFSET);
         
        Nvm_Write((uint16*)&otaData.prev_store_type,
                  sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_PREV_STORE_TYPE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.prev_partition_data_length,
              sizeof(otaData.prev_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_PREV_PARTITION_SIZE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.commit_done,
              sizeof(otaData.commit_done),
              otaData.nvm_offset + NVM_OTAU_COMMIT_DONE_OFFSET);
        
        Nvm_Write((uint16*)&otaData.calculate_hash,
              sizeof(otaData.calculate_hash),
              otaData.nvm_offset + NVM_OTAU_CALCULATE_HASH_OFFSET);
    }

    /* Increment the offset by the number of words of NVM memory required
     * by GAIA OTAu service
     */
    *nvm_offset += GAIA_OTAU_CLIENT_SERVICE_NVM_MEMORY_WORDS;
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      updatePreviousStore
 *
 *  DESCRIPTION
 *      This function is used to update previour store
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void updatePreviousStore(void)
{
    otaData.prev_partition_data_length = otaData.cur_partition_data_length;
    otaData.prev_store_type = otaData.cur_store_type;
    otaData.prev_store_id = otaData.cur_store_id;
    MemCopy(otaData.prev_header,otaData.cur_header,sizeof(otaData.prev_header));
        
    Nvm_Write((uint16*)&otaData.prev_store_id,
              sizeof(uint16),
              otaData.nvm_offset + NVM_OTAU_PREV_STORE_ID_OFFSET);
    
    Nvm_Write((uint16*)&otaData.prev_store_type,
              sizeof(uint16),
              otaData.nvm_offset + NVM_OTAU_PREV_STORE_TYPE_OFFSET);
    
    Nvm_Write((uint16*)&otaData.prev_partition_data_length,
              sizeof(otaData.prev_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_PREV_PARTITION_SIZE_OFFSET);
    
    Nvm_Write((uint16*)&otaData.prev_header[PARTITION_HEADER_BODY_OFFSET],
              MESH_HEADER_BODY_SIZE,
              otaData.nvm_offset + NVM_PREV_HEADER_BODY_OFFSET);
    
    Nvm_Write((uint16*)&otaData.prev_header[PARTITION_LENGTH_OFFSET],
              PARTITION_INFO_SIZE,
              otaData.nvm_offset + NVM_PREV_PARTITION_INFO_OFFSET);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startSHA256sum
 *
 *  DESCRIPTION
 *      Start a SHA256 calculation on the current partition
 *
 *  PARAMETERS
 *
 *  RETURNS
 *      None
 *
 *---------------------------------------------------------------------------*/
static void startSHA256sum(void)
{
    handle_t handle;
    uint8 i;
    
    for ( i = 0; i < SHA256_SIZE_WORDS; i++ )
    {
        otaData.oem_signature.sha256_sum[i] = 0;
    }

    otaData.store_hash_request_sent = TRUE;
    Storage_FindStore(otaData.cur_store_id, otaData.cur_store_type, &handle);
    StoreUpdate_HashStore(  handle,
                            /* header is not included */
                            STORE_HEADER_SIZE_WORDS,
                            /* convert from octets to 16bit words */
                            otaData.cur_partition_data_length / 2 - STORE_HEADER_SIZE_WORDS,
                            otaData.oem_signature.sha256_buffer);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      setCalculateHashStatus
 *
 *  DESCRIPTION
 *      This function sets the calculate hash status
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void setCalculateHashStatus(bool status)
{
    otaData.calculate_hash = status;
    Nvm_Write((uint16*)&otaData.calculate_hash,
              sizeof(otaData.calculate_hash),
              otaData.nvm_offset + NVM_OTAU_CALCULATE_HASH_OFFSET);
}

/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaClientOtauInit
 *
 *  DESCRIPTION
 *      This function is used to initialise upgrade client data
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GaiaClientOtauInit(bool nvm_start_fresh, uint16 *nvm_offset)
{
    /* Initialise otaData */
    resetFileOffsets();
    otaData.mState = STATE_VM_UPGRADE_IDLE;
    otaData.app_connected = FALSE;
    otaData.m_request_reached_eof = FALSE;
    otaData.commit_done = TRUE;
    otaData.calculate_hash = FALSE;
    otaData.store_hash_request_sent = FALSE;
    otaData.prev_partition_data_length = 0;
    otaData.cur_partition_data_length = 0;
    otaData.prev_store_id = 0; 
    otaData.cur_store_id = 0;
    otaData.prev_store_id = 0; 
    otaData.cur_store_id = 0;
    otaData.prev_store_type = 0; 
    otaData.cur_store_type = 0;
    otaData.requested_num_bytes = 0;    
    uint32 mesh_hdr_body_size = MESH_HEADER_BODY_SIZE;
    uint32 part_sig_size = PARTITION_FOOTER_SIGNATURE_LENGTH;
    TimerDelete(otaData.announce_tid);
    otaData.callback = NULL;
    
    /* Initialise Previous Store Header */
    MemCopy(otaData.prev_header,"APPUHDR4",PARTITION_HEADER_ID_SIZE);
    SetUint32InArray(otaData.prev_header, PARTITION_HEADER_LENGTH_OFFSET,mesh_hdr_body_size);
    MemSet(&otaData.prev_header[PARTITION_HEADER_BODY_OFFSET],0,MESH_HEADER_BODY_SIZE );
    MemCopy(&otaData.prev_header[PARTITION_ID_OFFSET],"PARTDATA",PARTITION_HEADER_ID_SIZE);    
    MemSet(&otaData.prev_header[PARTITION_LENGTH_OFFSET],0,PARTITION_INFO_SIZE);
    
    /* Initialise Previous Store Footer */
    MemCopy(&otaData.prev_footer,"APPUPFTR",PARTITION_FOOTER_ID_SIZE);
    SetUint32InArray(otaData.prev_footer, PARTITION_FOOTER_LENGTH_OFFSET,part_sig_size);
    MemSet(&otaData.prev_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],0,PARTITION_FOOTER_SIGNATURE_LENGTH);
    
    /* Initialise Current Store Header */
    MemCopy(otaData.cur_header,"APPUHDR4",HEADER_ID_SIZE );
    SetUint32InArray(otaData.cur_header, PARTITION_HEADER_LENGTH_OFFSET,mesh_hdr_body_size);
    MemSet(&otaData.cur_header[PARTITION_HEADER_BODY_OFFSET],0,MESH_HEADER_BODY_SIZE );
    MemCopy(&otaData.cur_header[PARTITION_ID_OFFSET],"PARTDATA",PARTITION_HEADER_ID_SIZE);    
    MemSet(&otaData.cur_header[PARTITION_LENGTH_OFFSET],0,PARTITION_INFO_SIZE);    
    
    /* Initialise User Store Footer */
    MemCopy(&otaData.cur_footer,"APPUPFTR",PARTITION_FOOTER_ID_SIZE);
    SetUint32InArray(otaData.cur_footer, PARTITION_FOOTER_LENGTH_OFFSET,part_sig_size);
    MemSet(&otaData.cur_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],0,PARTITION_FOOTER_SIGNATURE_LENGTH );
    
    readDataFromNVM(nvm_start_fresh,nvm_offset);
        
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauClientRegisterCallback
 *
 *  DESCRIPTION
 *      Set the callback so the OTAu Client library can inform the application
 *      of the progress and warn about special events.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauClientRegisterCallback(gaia_otau_client_event_handler callback)
{
    otaData.callback = callback;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      StarUpgrade
 *
 *  DESCRIPTION
 *      This function is used to start upgrade process
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/

extern void StartUpgrade(void)
{
    if(otaData.cur_store_id != 0)
    {
        notifyApplication(gaia_otau_client_event_upgrade_starting,NULL);
        otaData.mState = STATE_VM_UPGRADE_CONNECT;
        DEBUG_STR("\r\nUpgrade Connect Request Sent");
        GaiaClientSendConnectPacket();
    }
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      StoreHeaderDataToNVM
 *
 *  DESCRIPTION
 *      This function is used to store header data in NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void StoreHeaderDataToNVM(uint8* p_hdr_data)
{
    MemCopy(&otaData.cur_header[PARTITION_HEADER_BODY_OFFSET],p_hdr_data,MESH_HEADER_BODY_SIZE);
    
    /* Write to NVM */
    Nvm_Write((uint16*)&otaData.cur_header[PARTITION_HEADER_BODY_OFFSET],
              MESH_HEADER_BODY_SIZE,
              otaData.nvm_offset + NVM_CUR_HEADER_BODY_OFFSET);
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      StorePartitionDataToNVM
 *
 *  DESCRIPTION
 *      This function is used to store partition data info in NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void StorePartitionDataToNVM(GAIA_OTAU_EVENT_PARTITION_INFO_T* p_part_data)
{
    SetUint32InArray(otaData.cur_header,PARTITION_LENGTH_OFFSET,
                     (p_part_data->partition_size + 4));
    SetUint16InArray(otaData.cur_header,
                     PARTITION_LENGTH_OFFSET + 4,
                     p_part_data->partition_type);
    SetUint16InArray(otaData.cur_header,
                     PARTITION_LENGTH_OFFSET + 6,
                     p_part_data->partition_id);
    /* Write Partition information to NVM */
    Nvm_Write((uint16*)&otaData.cur_header[PARTITION_LENGTH_OFFSET],
              PARTITION_INFO_SIZE,
              otaData.nvm_offset + NVM_CUR_PARTITION_INFO_OFFSET);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      StoreRelayStoreInfo
 *
 *  DESCRIPTION
 *      This function is used to store the relay store information to NVM
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void StoreRelayStoreInfo(GAIA_OTAU_EVENT_PARTITION_INFO_T *p_store_info)
{
    otaData.cur_partition_data_length =  p_store_info->partition_size;
    otaData.cur_store_type = p_store_info->partition_type;
    otaData.cur_store_id = p_store_info->partition_id;
    
    Nvm_Write((uint16*)&otaData.cur_partition_data_length,
              sizeof(otaData.cur_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_CUR_PARTITION_SIZE_OFFSET);
    Nvm_Write((uint16*)&otaData.cur_store_type,
                 sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_TYPE_OFFSET);
    Nvm_Write((uint16*)&otaData.cur_store_id,
                 sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_ID_OFFSET);
    
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      StorePartitionSignatureToNVM
 *
 *  DESCRIPTION
 *      This function is used to store footer signatures in NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void StorePartitionSignatureToNVM(uint8* p_sig_data)
{
    MemCopy(&otaData.cur_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],p_sig_data,
            PARTITION_FOOTER_SIGNATURE_LENGTH );
    
    MemCopy(otaData.prev_footer,otaData.cur_footer,sizeof(otaData.prev_footer));
    
    Nvm_Write((uint16*)&otaData.cur_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
              PARTITION_FOOTER_SIGNATURE_LENGTH,
              otaData.nvm_offset + NVM_CUR_FOOTER_SIGNATURE_OFFSET);
    
    Nvm_Write((uint16*)&otaData.prev_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
              PARTITION_FOOTER_SIGNATURE_LENGTH,
              otaData.nvm_offset + NVM_PREV_FOOTER_SIGNATURE_OFFSET);

    GaiaOtauSetCommitStatus(TRUE);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauSetCommitStatus
 *
 *  DESCRIPTION
 *      This function sets the commit status of GAIA OTAu procedure
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GaiaOtauSetCommitStatus(bool status)
{
    otaData.commit_done = status;
    Nvm_Write((uint16*)&otaData.commit_done,
              sizeof(otaData.commit_done),
              otaData.nvm_offset + NVM_OTAU_COMMIT_DONE_OFFSET);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauSetRelayStore
 *
 *  DESCRIPTION
 *      This function updates the store type/store_id to be used for relay
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GaiaOtauSetRelayStore(bool commit_successful)
{
    if(commit_successful)
    {              
        if(otaData.cur_store_type == USER_STORE)
        {
            updatePreviousStore();
            /* Store signature */
            startSHA256sum();
        }
    }
    else
    {
        if(!((otaData.cur_store_type == otaData.prev_store_type) && 
             (otaData.cur_store_id = otaData.prev_store_id)))
        {
        otaData.cur_partition_data_length = otaData.prev_partition_data_length;
        otaData.cur_store_type = otaData.prev_store_type;
        otaData.cur_store_id = otaData.prev_store_id;
        MemCopy(otaData.cur_header,otaData.prev_header,sizeof(otaData.cur_header));
        MemCopy(otaData.cur_footer,otaData.prev_footer,sizeof(otaData.cur_footer));
        
        Nvm_Write((uint16*)&otaData.cur_partition_data_length,
              sizeof(otaData.cur_partition_data_length),
              otaData.nvm_offset + NVM_OTAU_CUR_PARTITION_SIZE_OFFSET);
        Nvm_Write((uint16*)&otaData.cur_store_type,
                 sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_TYPE_OFFSET);
        Nvm_Write((uint16*)&otaData.cur_store_id,
                 sizeof(uint16),
                 otaData.nvm_offset + NVM_OTAU_CUR_STORE_ID_OFFSET);
        
        /* Write to NVM */
        Nvm_Write((uint16*)&otaData.cur_header[PARTITION_HEADER_BODY_OFFSET],
              MESH_HEADER_BODY_SIZE,
              otaData.nvm_offset + NVM_CUR_HEADER_BODY_OFFSET);
        
        /* Write Partition information to NVM */
        Nvm_Write((uint16*)&otaData.cur_header[PARTITION_LENGTH_OFFSET],
                  PARTITION_INFO_SIZE,
                  otaData.nvm_offset + NVM_CUR_PARTITION_INFO_OFFSET);
        
        Nvm_Write((uint16*)&otaData.cur_footer[PARTITION_FOOTER_SIGNATURE_OFFSET],
                  PARTITION_FOOTER_SIGNATURE_LENGTH,
                  otaData.nvm_offset + NVM_CUR_FOOTER_SIGNATURE_OFFSET);
        
        GaiaOtauSetCommitStatus(TRUE);
    }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleAnnounceTimerExpiry
 *
 *  DESCRIPTION
 *      This function handles the announce timer expiry
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleAnnounceTimerExpiry(timer_id tid)
{
    if(tid == otaData.announce_tid)
    {
        SendLOTAnnouncePacket();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      SendLOTAnnouncePacket
 *
 *  DESCRIPTION
 *      This function is used to send LOT Announce Message
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void SendLOTAnnouncePacket(void)
{  
    TimerDelete(otaData.announce_tid);   
    otaData.announce_tid = TIMER_INVALID;
    
    if(otaData.calculate_hash)
    {
        updatePreviousStore();
        setCalculateHashStatus(FALSE);
        startSHA256sum();        
    }
     
    if((otaData.cur_store_id != 0) && (!otaData.app_connected) && (otaData.commit_done) &&
       ((otaData.cur_store_type == APP_STORE && otaData.cs_id == otaData.app_id) || 
        (otaData.cur_store_type == USER_STORE)) && 
       (otaData.mState != STATE_VM_UPGRADE_WAIT_POST_TRANSFER_DISCONNECT)
        && (otaData.mState != STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION)
        && (IsAppAssociated()) && (!scanning_ongoing))
    {
        lot_announce_data.companycode = GetUint16FromArray(&otaData.cur_header[PARTITION_HEADER_COMPANY_CODE_OFFSET]);
        lot_announce_data.platformtype = otaData.cur_header[PARTITION_HEADER_PLATFORMTYPE_OFFSET];
        lot_announce_data.typeencoding = otaData.cur_header[PARTITION_HEADER_TYPEENCODING_OFFSET];
        lot_announce_data.imagetype = otaData.cur_header[PARTITION_HEADER_IMAGETYPE_OFFSET];
        lot_announce_data.size = (otaData.cur_partition_data_length/1024);
        lot_announce_data.objectversion = ((((uint16)(otaData.cur_header[PARTITION_HEADER_APP_VERSION_OFFSET] & 0x3F)) << 10) |
                                                     (((uint16)(otaData.cur_header[PARTITION_HEADER_APP_VERSION_OFFSET+ 1] & 0x0F)) << 6) |
                                                     ((uint16)(otaData.cur_header[PARTITION_HEADER_APP_VERSION_OFFSET + 2] & 0x3F)));
        lot_announce_data.targetdestination = 0;
    
        if(otaData.announce_count < ANNOUNCE_COUNT)
        {
            DEBUG_STR("\r\nLOT Announce Sent");
            LotModelSendAnnounce(&lot_announce_data);
            otaData.announce_tid = TimerCreate(ANNOUNCE_COUNT_INTERVAL,TRUE, 
                                           handleAnnounceTimerExpiry);
            otaData.announce_count++;
        }
        else
        {
            otaData.announce_count = 0;
        }
        
    }  
    else
    {
        otaData.announce_count = 0;
    }
    
    if((!otaData.app_connected) && (otaData.announce_count == 0) && 
	(IsAppAssociated()))
    {
        otaData.announce_tid = TimerCreate(ANNOUNCE_INTERVAL,TRUE, 
                                           handleAnnounceTimerExpiry);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaClientOtauHandleConnNotify
 *
 *  DESCRIPTION
 *      This function is used to handle a connection event
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GaiaClientOtauHandleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data)
{     
    if(cm_event_data->result == cm_conn_res_success)
    {
        otaData.app_connected = TRUE;
        
        TimerDelete(otaData.announce_tid);
        otaData.announce_tid = TIMER_INVALID;
        
    }
    else
    {
        otaData.app_connected = FALSE;
    }
    
    if(CMGetPeerDeviceRole(cm_event_data->device_id) == con_role_peripheral)
    {
    if(cm_event_data->result == cm_conn_res_success)
    {
        if(otaData.mState == STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION)
        {
            DEBUG_STR("\r\nPost Transfer Reconnection Done");
            otaData.mState = STATE_VM_UPGRADE_IDLE;
        }
        else
        {
            DEBUG_STR("\r\nConnection Done");
        }
    }
    else if(cm_event_data->result == cm_disconn_res_success)
    {
        DEBUG_STR("\r\nDisconnection Done");
        switch (otaData.mState)
        {
            case STATE_VM_UPGRADE_WAIT_POST_TRANSFER_DISCONNECT:
            case STATE_VM_UPGRADE_WAIT_POST_TRANSFER_WAIT_DISCONNECT:
            {
                DEBUG_STR("\r\nPost Transfer Connect Request Sent");                
                otaData.mState = STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION;              
                otaData.m_timeout_is_set = FALSE;
            }
            break;
            case STATE_VM_UPGRADE_DISCONNECT:
            case STATE_VM_UPGRADE_COMPLETED:
            {
                resetFileOffsets();
                otaData.mState = STATE_VM_UPGRADE_IDLE;
            }
            break;
            case STATE_VM_UPGRADE_IDLE:
            {
                resetFileOffsets();
            }
            break;
            default:
            {
                resetFileOffsets();
                otaData.mState = STATE_VM_UPGRADE_IDLE;
            }
            break;      
         }
    }
}
}
    
/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleResponseNotification
 *
 *  DESCRIPTION
 *      This function handles the VM Upgrade Commands
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
extern void HandleResponseNotification(uint16 payload_len, uint8 *payload)
{
    if(payload_len >= 3)
    {
        /* get the payload */
        uint8 msg_id = GetUint8FromArray(payload);
        payload++;
        payload_len--;
        uint16 rsp_len = GetUint16FromArray(payload);
        payload += 2;
        payload_len -= 2;

        if (payload_len != rsp_len)
        {
            SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
        }
        else
        {
            switch (msg_id)
            {
                case UPGRADE_HOST_START_CFM:
                upgradeDeviceHandleStartCfm(payload_len, payload);
                break;
                
                case UPGRADE_HOST_DATA_BYTES_REQ:
                upgradeDeviceHandleDataBytesReq(payload_len, payload);
                break;

                case UPGRADE_HOST_ABORT_CFM:
                upgradeDeviceHandleAbortCfm(payload_len, payload);
                break;

                case UPGRADE_HOST_TRANSFER_COMPLETE_IND:
                upgradeDeviceHandleTransferCompleteInd(payload_len, payload);
                break;

                case UPGRADE_HOST_COMMIT_REQ:
                upgradeDeviceHandleCommitReq(payload_len, payload);
                break;

                case UPGRADE_HOST_ERRORWARN_IND:
                upgradeHandleErrorWarnInd(payload_len, payload);
                break;

                case UPGRADE_HOST_COMPLETE_IND:
                upgradeDeviceHandleCompleteInd(payload_len, payload);
                break;

                case UPGRADE_HOST_SYNC_CFM:
                upgradeDeviceHandleSyncCfm(payload_len, payload);
                break;

                case UPGRADE_HOST_IS_CSR_VALID_DONE_CFM:
                upgradeDeviceHandleIsValidationDoneCfm(payload_len, payload);
                break;

                default:
                SetAcknowledgement(GAIA_STATUS_NOT_SUPPORTED);
                break;
            }
        }
    }
    else
    {
        SetAcknowledgement(GAIA_STATUS_INVALID_PARAMETER);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleUpgradeConnectAck
 *
 *  DESCRIPTION
 *      The device handles upgrade conenct acknowledgement
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
extern void HandleUpgradeConnectAck(uint16 payload_length, uint8* payload)
{
    if (otaData.mState == STATE_VM_UPGRADE_CONNECT)
    {
        uint8 status = payload[0];
        if(status == GAIA_STATUS_SUCCESS)
        {
            DEBUG_STR("\r\nUpgrade Connect Request Received");
            // Send Sync Request
            uint16 response_length,byte_index;
            uint8 response[VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_SYNC_REQ_BYTE_SIZE];
            response_length = UPGRADE_HOST_SYNC_REQ_BYTE_SIZE;
            otaData.m_inprogress_id = 0x12345678;
                
            byte_index = 0;
            byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_SYNC_REQ);
            byte_index += SetUint16InArray(response,byte_index,response_length);
            byte_index += SetUint32InArray(response,byte_index,otaData.m_inprogress_id);
            GaiaClientSendCommandPacket(VM_CONTROL_COMMAND_SIZE + UPGRADE_HOST_SYNC_REQ_BYTE_SIZE,
                                            response);
                                            
            otaData.mState = STATE_VM_UPGRADE_SYNC_REQ;
            DEBUG_STR("\r\nSync Request Sent");
        }
        else
        {
            otaData.mState = STATE_VM_UPGRADE_IDLE;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleUpgradeControlRequestAck
 *
 *  DESCRIPTION
 *      This function handles upgrade control request acknowledgement
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
extern void HandleUpgradeControlRequestAck(uint16 payload_length, uint8* payload)
{
    switch (otaData.mState)
    {
        case STATE_VM_UPGRADE_TRANSFER:
        {
            uint8 status = payload[0];
            if(status == GAIA_STATUS_SUCCESS)
            {
                if(otaData.m_request_reached_eof)
                {
                    otaData.mState = STATE_VM_UPGRADE_WAIT_VALIDATION_RSP;
                    DEBUG_STR("\r\nValidation Request Sent");
                    upgradeSendIsValidationDoneRequest();
                }
                else
                {
                    if(otaData.requested_num_bytes > 0)
                    {
                        upgradeSendData(otaData.requested_num_bytes);
                    }
                    otaData.mState = STATE_VM_UPGRADE_DATA_REQ;
                }
            }
        }
        break;
        case STATE_VM_UPGRADE_WAIT_POST_TRANSFER_DISCONNECT:
        {
            DEBUG_STR("\r\nWaiting to reboot");
            TimerDelete(otaData.ota_tid);
            otaData.ota_tid = TIMER_INVALID;
            otaData.ota_tid = 
                TimerCreate(GAIA_VMUPGRADE_POST_TRANSFER_REBOOT_DELAY,TRUE, handleValidationTimerExpiry);
        }
        break;
        default:
        break;
    }    
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleUpgradeDisconnectAck
 *
 *  DESCRIPTION
 *      This function handles upgrade disconnect acknowledgement
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
extern void HandleUpgradeDisconnectAck(uint16 payload_length, uint8* payload)
{
    resetFileOffsets();
    otaData.mState = STATE_VM_UPGRADE_IDLE;
    DEBUG_STR("\r\nDisconnecting the device");
    GaiaDeviceDisconnect();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauClientConfigStoreMsg
 *
 *  DESCRIPTION
 *      This function handles the config store messages
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauClientConfigStoreMsg(msg_t *msg)
{
    configstore_msg_t *cs_msg = (configstore_msg_t*)msg;

    switch(msg->header.id)
    {
        /*
         * This will be returned when the GAIA OTAu library is initialised
         * and the CS key is read.
         */
        case CONFIG_STORE_READ_KEY_CFM:
        {
            if(cs_msg->body.read_key_cfm.status == STATUS_SUCCESS)
            {
                if(cs_msg->body.read_key_cfm.id == CS_ID_APP_STORE_ID)
                {
                    otaData.cs_id = cs_msg->body.read_key_cfm.value[0];
                    otaData.app_id = StoreUpdate_GetAppId().id;
                }
            }
        }
        break;

        default:
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauClientHandleStoreUpdateMsg
 *
 *  DESCRIPTION
 *      This function handles the config store messages
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauClientHandleStoreUpdateMsg(device_handle_id device_id, store_update_msg_t *msg)
{ 
    switch(msg->header.id)
    {
        case STORE_UPDATE_SET_STORE_ID_CFM:            
        {
            if(otaData.cs_id != otaData.app_id)
            {
                otaData.cs_id = otaData.app_id;
                if(otaData.app_connected)
                {
                    TimerDelete(otaData.announce_tid); 
                    otaData.announce_tid = TIMER_INVALID;
                }
                setCalculateHashStatus(TRUE);
            }   
        }
        break;
        case STORE_UPDATE_HASH_STORE_CFM:
        {
            int i;
            int MSB_sum, LSB_sum;
            status_t status = msg->body.hash_store_cfm.status;

            if(otaData.store_hash_request_sent)
                otaData.store_hash_request_sent = FALSE;
            else
                break;
                
            if ( status == sys_status_success )
            {
                /* sum this hash with the previous hash
                 * it is a byte sum, so split in to 8octet sums and don't carry
                 */
                for ( i = 0; i < SHA256_SIZE_WORDS; i++ )
                {
                    MSB_sum =   ((otaData.oem_signature.sha256_sum[i]&0xff00) +
                                (otaData.oem_signature.sha256_buffer[i]&0xff00) );
                    MSB_sum &= 0xff00;

                    LSB_sum =   ((otaData.oem_signature.sha256_sum[i]) +
                                (otaData.oem_signature.sha256_buffer[i]) );
                    LSB_sum &= 0x00ff;

                    otaData.oem_signature.sha256_sum[i] = MSB_sum | LSB_sum;
                }
                for ( i = 0; i < SHA256_SIZE_WORDS; i++)
                {
                    otaData.footer_signature[i * 2] = otaData.oem_signature.sha256_sum[i] >> 8;
                    otaData.footer_signature[(i * 2) + 1] = otaData.oem_signature.sha256_sum[i] & 0x00FF;
                }
                StorePartitionSignatureToNVM(otaData.footer_signature);
            }
        }
        break;
        default:
        break;
    }            
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauClientGetState
 *
 *  DESCRIPTION
 *      This function returns the transfer state of GAIA OTAu
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern uint16 GaiaOtauClientGetState(void)
{
    return otaData.mState;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauClientResetTransferState
 *
 *  DESCRIPTION
 *      This function resets the transfer state of GAIA OTAu Client
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauClientResetTransferState(void)
{
    resetFileOffsets();
    otaData.mState = STATE_VM_UPGRADE_IDLE;
}
#endif

