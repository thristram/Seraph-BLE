/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file   gaia_otau.c
 * \brief   GAIA OTAu protocol implementation
 *
 *  This library provides an Over The Air upgrade service using GAIA and the
 *  Upgrade Protocol. See CS-347923-SP Upgrade Protocol.
 *
 *  This file implements:
 *  + handle Upgrade Protocol message requests, confirmations and
 *    notifications
 *  + events between the Application and the OTAu Library
 *  + writing partitions to the memory device
 *  + validating the image on the memory device
 *  + rebooting in to the new application
 *  + committing the new application
 *
 */

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <gatt.h>
#include <gatt_prim.h>
#include <battery.h>
#include <buf_utils.h>
#include <mem.h>
#include <string.h>
#include <configstore_id.h>
#include <configstore_api.h>
#include <nvm_access.h>
#include <reset.h>
#include <time.h>
#include <timer.h>


/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "gaia_otau_private.h"
#include "gaia_otau_api.h"
#include "gaia_otau.h"
#include "gaia.h"
#include "byte_utils.h"
#include "cm_types.h"
#include "cm_api.h"


/*============================================================================*
 *  Private Data Types
 *============================================================================*/

/* This structure defines the local data store for this module */
typedef struct
{
    TRANSFER_CTRL transfer_state;           /* current state of the OTAu upgrade */
    TRANSFER_DATA_TYPE data_transfer_state; /* used to manage the VM upgrade protocol data transfer stages */
    uint32 bytes_transferred;               /* used to count bytes in handleDataTransfer() */
    bool validation_done;                   /* flag to indicated that validation is complete */
    bool disconnect_reset;                  /* reboot required, usually after a disconnect from the host */
    bool reboot_wait;                       /* wait before rebooting */
    store_id_t app_id;                      /* the current AppId */
    bool cs_id_read;                        /* the CS key AppId has been read */
    store_id_t cs_id;                       /* the CS key AppId */
    bool app_upgraded;                      /* application was upgraded */
    UPGRADE_RESUME_POINT resume_point;      /* current resume point */
    struct {
        su_store_type_t type;               /* type of store */
        store_id_t id;                      /* store ID */
        uint16 store_size;                  /* size of this store, encoded */
        memory_address_t data_length;       /* number of bytes expected from the host */
        bool last_chunk;                    /* last chunk written to Flash */
        bool last_partition;                /* this was the last partition */
    } partition;                            /* used to track the current partition type */
    uint32 header_length;                   /* expected header length */
    struct {
        uint16 sha256_sum[SHA256_SIZE_WORDS];       /* sum of all the SHA256s */
        uint16 sha256_buffer[SHA256_SIZE_WORDS];    /* SHA256 that is calculated from the Flash */
        uint32 length;                      /* expected OEM signature length */
    } oem_signature;
    uint16 nvm_offset;                      /* NVM offset at which OTAu data is stored */
    uint32 InProgressIdentifier;            /* unique ID first sent by the host for each upgrade */
    uint32 new_in_progress_id;              /* unique ID sent by the host when continuing an upgrade */
    device_handle_id device_id;             /* device ID of the connected host */
    gaia_otau_event_handler callback;       /* callback to the application event handler */
    timer_id commit_timeout_tid;            /* commit timeout tid */
} OTA_SERVICE_LOCAL_DATA_T;

#ifndef NVM_DONT_PAD
#define GAIA_OTAU_SERVICE_VERSION (0x0001)
#define GAIA_OTAU_SERVICE_PADDED_LEN (NVM_PAD_ROUND_UP_TO_32(GAIA_OTAU_SERVICE_NVM_MEMORY_WORDS))
#endif


/*=============================================================================*
 *  Private Data
 *============================================================================*/
static OTA_SERVICE_LOCAL_DATA_T g_otau_data;   /* state information for this file */
TIMER_RESERVE(commit_timeout)

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static bool isPartitionHeader( uint8* file_data );
static bool isFooterHeader( uint8* file_data );
static store_id_t getAppStoreToUpgrade(store_id_t app_id, store_id_t cs_id);
static void hostCommitTimeoutHandler(timer_id tid);
static void hostCommitTimeoutCreate(void);
static void hostCommitTimeoutDelete(void);


/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------
 *  NAME
 *      initStateVariables
 *
 *  DESCRIPTION
 *      Reset State variables to their defaults, ready to start the upgrade
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void initStateVariables(void)
{
    int i;

    for ( i = 0; i < SHA256_SIZE_WORDS; i++ )
    {
        g_otau_data.oem_signature.sha256_sum[i] = 0;
    }

    g_otau_data.validation_done = FALSE;
    g_otau_data.bytes_transferred = 0;
    g_otau_data.data_transfer_state = data_transfer_state_header_id;
    g_otau_data.disconnect_reset = FALSE;
    g_otau_data.reboot_wait = FALSE;
    g_otau_data.partition.last_partition = FALSE;
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      setDefaultNVM
 *
 *  DESCRIPTION
 *      Set the NVM to the default values
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void setDefaultNVM(void)
{
    /* Write GAIA OTAu InProgressIdentifier config */
    g_otau_data.InProgressIdentifier = GAIA_IN_PROGRESS_IDENTIFIER_DEFAULT;
    Nvm_Write((uint16*)&g_otau_data.InProgressIdentifier,
             sizeof(g_otau_data.InProgressIdentifier),
             g_otau_data.nvm_offset + GAIA_NVM_IN_PROGRESS_IDENTIFIER_OFFSET);

    /* Write GAIA OTAu Resume Point config */
    g_otau_data.resume_point = GAIA_RESUME_POINT_DEFAULT;
    Nvm_Write((uint16*)&g_otau_data.resume_point,
             sizeof(g_otau_data.resume_point),
             g_otau_data.nvm_offset + GAIA_NVM_RESUME_POINT_OFFSET);
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      saveCurrentNVM
 *
 *  DESCRIPTION
 *      Save all the current values to NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void saveCurrentNVM(void)
{
    Nvm_Write((uint16*)&g_otau_data.InProgressIdentifier,
             sizeof(g_otau_data.InProgressIdentifier),
             g_otau_data.nvm_offset + GAIA_NVM_IN_PROGRESS_IDENTIFIER_OFFSET);
    Nvm_Write((uint16*)&g_otau_data.resume_point,
             sizeof(g_otau_data.resume_point),
             g_otau_data.nvm_offset + GAIA_NVM_RESUME_POINT_OFFSET);
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      readDataFromNVM
 *
 *  DESCRIPTION
 *      This function is used to read GAIA service specific data store in NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset )
{
#ifndef NVM_DONT_PAD
    uint16 next_nvm_offset = *nvm_offset + GAIA_OTAU_SERVICE_PADDED_LEN;
    uint16 temp_nvm_offset = *nvm_offset;

    /* If the service cannot read or repair the data in NVM
       we treat it as a fresh start */
    if (!nvm_start_fresh && !Nvm_ValidateVersionedHeader(nvm_offset,
                                 GAIA_OTAU_SERVICE_ID,
                                 GAIA_OTAU_SERVICE_PADDED_LEN,
                                 GAIA_OTAU_SERVICE_VERSION,
                                 NULL))
    {
        nvm_start_fresh = TRUE;
    }
    if (nvm_start_fresh)
    {
        *nvm_offset = temp_nvm_offset;
        Nvm_WriteVersionedHeader(nvm_offset,
                               GAIA_OTAU_SERVICE_ID,
                               GAIA_OTAU_SERVICE_PADDED_LEN,
                               GAIA_OTAU_SERVICE_VERSION);
    }
#endif
    g_otau_data.nvm_offset = *nvm_offset;

    if(!nvm_start_fresh)
    {
        /* Read GAIA OTAu InProgressIdentifier config */
        Nvm_Read((uint16*)&g_otau_data.InProgressIdentifier,
                 sizeof(g_otau_data.InProgressIdentifier),
                 g_otau_data.nvm_offset + GAIA_NVM_IN_PROGRESS_IDENTIFIER_OFFSET);

        /* Read GAIA OTAu Resume Point config */
        Nvm_Read((uint16*)&g_otau_data.resume_point,
                 sizeof(g_otau_data.resume_point),
                 g_otau_data.nvm_offset + GAIA_NVM_RESUME_POINT_OFFSET);
    }
    else    /* write default values */
    {
        setDefaultNVM();
    }

    /* Increment the offset by the number of words of NVM memory required
     * by GAIA OTAu service
     */
    *nvm_offset += GAIA_OTAU_SERVICE_NVM_MEMORY_WORDS;
#ifndef NVM_DONT_PAD
#ifdef DEBUG_BUILD
    /* Error if we try to write more data than we have padded space for */
    ASSERT(*nvm_offset < next_nvm_offset);
#endif
    *nvm_offset = next_nvm_offset;
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      notifyApplication
 *
 *  DESCRIPTION
 *      Notify the application of OTAu events via the callback function.
 *
 *  PARAMETERS
 *      event [in]      Type of event to notify
 *      *data [in,out]  Pointer to any event-specific data that should be sent
 *
 *  RETURNS
 *      Return status of the callback
 *
 *---------------------------------------------------------------------------*/
static sys_status notifyApplication(gaia_otau_event event, GAIA_OTAU_EVENT_T *data)
{
    if (g_otau_data.callback != NULL)
    {
        return g_otau_data.callback(event, data);
    }

    return sys_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      notifyApplicationRebootWarning
 *
 *  DESCRIPTION
 *      Notify the application of a pending reboot.
 *
 *  PARAMETERS
 *      None
 *  RETURNS
 *      None
 *
 *---------------------------------------------------------------------------*/
static void notifyApplicationRebootWarning(void)
{
    GAIA_OTAU_EVENT_T event_data;

    event_data.reboot_warning.reboot_wait = FALSE;
    notifyApplication(gaia_otau_event_reboot_warning, &event_data);
    g_otau_data.reboot_wait = event_data.reboot_warning.reboot_wait;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      runNewApp
 *
 *  DESCRIPTION
 *      Handle changeover to the new app
 *
 *  PARAMETERS
 *      commit_flag - TRUE = make the change permanent
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void runNewApp(bool commit_flag)
{
    if ( commit_flag == FALSE ) /* temporarily run the new app */
    {
        /* temporarily run the new application when next reset */
        StoreUpdate_RunAppOnce( getAppStoreToUpgrade(   g_otau_data.app_id,
                                                        g_otau_data.cs_id));
    }
    else                      /* commit the new app */
    {
        StoreUpdate_RunApp( getAppStoreToUpgrade(   g_otau_data.app_id,
                                                    g_otau_data.cs_id));
        /*
         * the current running application is now committed and will be the
         * default application
         */
    }
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      isNewApp
 *
 *  DESCRIPTION
 *      Is the current application the new application that was run after calling
 *      StoreUpdate_RunAppOnce
 *
 *  RETURNS:
 *      TRUE  - the currently running application is the upgraded one but not
 *              yet committed
 *      FALSE - the application is the "original" application
 *
 *----------------------------------------------------------------------------*/
static bool isNewApp(void)
{
    return (g_otau_data.app_id != g_otau_data.cs_id);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      getAppStoreToUpgrade
 *
 *  DESCRIPTION
 *      Calculates which store ID needs to be updated
 *
 *  PARAMETERS
 *      app_id[in] - id of current running application
 *      cs_id[in]  - id of the application which will be run after a reset
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static store_id_t getAppStoreToUpgrade(store_id_t app_id, store_id_t cs_id)
{
    store_id_t next_store;

    if ( g_otau_data.app_id == UPGRADE_STORE_APPID_LAST )
    {
        if ( isNewApp() )    /* stage 2 commit stage */
        {
            next_store = UPGRADE_STORE_APPID_LAST;
        }
        else                 /* stage 1 write to flash stage */
        {
            next_store = UPGRADE_STORE_APPID_FIRST;
        }
    }
    else  /* is FIRST */
    {
        if ( isNewApp() )    /* stage 2 commit stage */
        {
            next_store = UPGRADE_STORE_APPID_FIRST;
        }
        else                 /* stage 1 write to flash stage */
        {
            next_store = UPGRADE_STORE_APPID_LAST;
        }
    }
    return next_store;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendErrorInd
 *
 *  DESCRIPTION
 *      The device may send error or warning codes to the host during the
 *      upgrade process.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendErrorInd(device_handle_id device_id, uint16 error_code)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_ERROR_IND_PAYLOAD_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_ERRORWARN_IND);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_ERROR_IND_PAYLOAD_SIZE);
    byte_index += SetUint16InArray(response,byte_index,error_code);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_ERROR_IND_PAYLOAD_SIZE,
                    response);

    notifyApplication(gaia_otau_event_upgrade_failed, NULL);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendValidationCfm
 *
 *  DESCRIPTION
 *      The device will respond to the UPGRADE_IS_VALIDATION_DONE_REQ message
 *      when validation has not yet been done
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendValidationCfm(device_handle_id device_id)
{
    uint16 byte_index ;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_VALIDATION_CFM_PAYLOAD_SIZE];
    uint16 back_off_time = UPGRADE_VALIDATION_BACKOFF_TIME_DEFAULT;

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_IS_CSR_VALID_DONE_CFM);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_VALIDATION_CFM_PAYLOAD_SIZE);
    byte_index += SetUint16InArray(response,byte_index,back_off_time);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_VALIDATION_CFM_PAYLOAD_SIZE,
                    response);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendTransferCompleteInd
 *
 *  DESCRIPTION
 *      The device will respond to the UPGRADE_IS_VALIDATION_DONE_REQ message
 *      when validation has been done
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendTransferCompleteInd(device_handle_id device_id)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_TRANSFER_COMPLETE_IND_PAYLOAD_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_TRANSFER_COMPLETE_IND);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_TRANSFER_COMPLETE_IND_PAYLOAD_SIZE);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_TRANSFER_COMPLETE_IND_PAYLOAD_SIZE,
                    response);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendStartCfm
 *
 *  DESCRIPTION
 *      Response sent by device to the UPGRADE_START_REQ message from the host
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendStartCfm(device_handle_id device_id, uint8 status)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_START_CFM_PAYLOAD_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_START_CFM);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_HOST_START_CFM_PAYLOAD_SIZE);
    byte_index += SetUint8InArray(response,byte_index,status);
    byte_index += SetUint16InArray(response,byte_index,BatteryReadVoltage());

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_START_CFM_PAYLOAD_SIZE,
                    response);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendDataBytesReq
 *
 *  DESCRIPTION
 *      The device uses this message to start data transfer of upgrade image
*       data from the host
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendDataBytesReq(device_handle_id device_id, uint32 num_bytes)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_DATA_BYTES_REQ_BYTE_SIZE];
    uint32 start_offset = 0;

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_DATA_BYTES_REQ);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_HOST_DATA_BYTES_REQ_BYTE_SIZE);
    byte_index += SetUint32InArray(response,byte_index,num_bytes);
    byte_index += SetUint32InArray(response,byte_index,start_offset);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_DATA_BYTES_REQ_BYTE_SIZE,
                    response);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendSyncCfm
 *
 *  DESCRIPTION
 *      The device will respond to the UPGRADE_SYNC_REQ message with this
 *      protocol message.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendSyncCfm(device_handle_id device_id, uint32 in_progress_id)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_SYNC_CFM_BYTE_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_SYNC_CFM);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_HOST_SYNC_CFM_BYTE_SIZE);

    /*
     * There are two resume states supported:
     * (1) UPGRADE_RESUME_POINT_START, resume point 0
     * (2) UPGRADE_RESUME_POINT_POST_REBOOT, resume point 3
     */
    byte_index += SetUint8InArray(response, byte_index, g_otau_data.resume_point);

    byte_index += SetUint32InArray(response, byte_index, in_progress_id);
    byte_index += SetUint8InArray(response, byte_index, PROTOCOL_CURRENT_VERSION);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_SYNC_CFM_BYTE_SIZE,
                    response);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendAbortCfm
 *
 *  DESCRIPTION
 *      Response sent by device to the UPGRADE_ABORT_REQ message from the host
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendAbortCfm(device_handle_id device_id)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_ABORT_CFM_PAYLOAD_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_ABORT_CFM);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_HOST_ABORT_CFM_PAYLOAD_SIZE);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_ABORT_CFM_PAYLOAD_SIZE,
                    response);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendUpdateCommitReq
 *
 *  DESCRIPTION
 *      Request sent by the device to request a commit confirmation
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendUpdateCommitReq(device_handle_id device_id)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_COMMIT_REQ_PAYLOAD_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_COMMIT_REQ);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_HOST_COMMIT_REQ_PAYLOAD_SIZE);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_COMMIT_REQ_PAYLOAD_SIZE,
                    response);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      upgradeDeviceSendUpdateCompleteInd
 *
 *  DESCRIPTION
 *      Indication sent by device to indicate that everything is complete
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
static void upgradeDeviceSendUpdateCompleteInd(device_handle_id device_id)
{
    uint16 byte_index;
    uint8 response[VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_UPDATE_COMPLETE_IND_PAYLOAD_SIZE];

    byte_index = 0;
    byte_index += SetUint8InArray(response,byte_index,GAIA_EVENT_VMUP_PACKET);
    byte_index += SetUint8InArray(response,byte_index,UPGRADE_HOST_COMPLETE_IND);
    byte_index += SetUint16InArray(response,byte_index,UPGRADE_HOST_UPDATE_COMPLETE_IND_PAYLOAD_SIZE);

    GaiaSendPacket( device_id,
                    GAIA_EVENT_NOTIFICATION,
                    VM_UPGRADE_MSG_SIZE + UPGRADE_HOST_UPDATE_COMPLETE_IND_PAYLOAD_SIZE,
                    response);
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

    Storage_FindStore( g_otau_data.partition.id, g_otau_data.partition.type, &handle);
    StoreUpdate_HashStore(  handle,
                            /* header is not included */
                            STORE_HEADER_SIZE_WORDS,
                            /* convert from octets to 16bit words */
                            g_otau_data.partition.data_length / 2 - STORE_HEADER_SIZE_WORDS,
                            g_otau_data.oem_signature.sha256_buffer);
}

/****************************************************************************/
/*! \brief   Encode a store size into a uint16.
*
*  \details The opposite function is storage_size_in_words().
*
*           Size is encoded as follows:
*           - bit 15:14 = b00 : size = bits 13:0 x 1  octets
*           - bit 15:14 = b01 : size = bits 13:0 x 16 octets
*           - bit 15:14 = b10 : size = bits 13:0 x 4K octets
*           - bit 15:14 = b11 : do not use, spare for future use
*
*  \param   size    A number to be encoded, in words.
*  \param   result  A pointer to the encoded number.
*
*  \return  #sys_status_success or an error code.
*/
/****************************************************************************/
static status_t storage_size_encode(memory_address_t size, uint16 *result)
{

    if (size <= MAX_SIZE_1B)
    {
         /* Encode in units of 1 byte */
         *result = (uint16)size | FLAG_1B;
         return sys_status_success;
    }

    if (size <= MAX_SIZE_16B)
    {
         /* Check number is an exact multiple of 16 bytes */
         if (size & REMAINDER_16B)
         {
             return sys_status_bad_value;
         }

         /* Encode in units of 16 bytes */
         size = size >> SHIFT_16B;
         *result = (uint16)size | FLAG_16B;
         return sys_status_success;
    }

    if (size <= MAX_SIZE_4KB)
    {
         /* Check number is an exact multiple of 4K bytes */
         if (size & REMAINDER_4KB)
         {
             return sys_status_bad_value;
         }

         /* Encode in units of 4K bytes */
         size = size >> SHIFT_4KB;
         *result = (uint16)size | FLAG_4KB;
         return sys_status_success;
    }

    /* If we get here the number is too large to encode, i.e. > 31.99 words.
    * However this is 4x larger than the max supported Flash size (16MB),
    * and if necessary we can define a multiplier for bits 15:14 = b11.
    */
    return sys_status_bad_value;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      isPartitionHeader, isFooterHeader
 *
 *  DESCRIPTION
 *      Searches an array for the Partition/Footer ID
 *
 *  PARAMETERS
 *      file_data [in]      the 8 character array to match
 *
 *  RETURNS
 *      bool - TRUE if the type matches
 *
 *---------------------------------------------------------------------------*/
static bool isPartitionHeader( uint8* file_data )
{
    return ( MemCmp(    file_data,
                        UPGRADE_PARTITION_ID,
                        StrLen(UPGRADE_PARTITION_ID) ) == 0 );
}

static bool isFooterHeader( uint8* file_data )
{
    return ( MemCmp(    file_data,
                        UPGRADE_FOOTER_ID,
                        StrLen(UPGRADE_FOOTER_ID) ) == 0 );
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleOtauFileTransfer
 *
 *  DESCRIPTION
 *      This is the part of the protocol where the contents of the OTAu file is
 *      transferred to the device, parsed, and then the partitions are stored
 *      in to the Flash memory
 *
 *  PARAMETERS
 *      device_id [in]      source handle
 *      more_data_flag [in] MORE_DATA field of the UPGRADE_DATA packet type
 *      length [in]         size of the image
 *      image_data [in]     IMAGE_DATA payload of the UPGRADE_DATA packet type
 *
 *  RETURNS
 *      None
 *
 *---------------------------------------------------------------------------*/
static void handleOtauFileTransfer( device_handle_id device_id,
                                    uint8 more_data_flag,
                                    uint16 length,
                                    uint8* file_data)
{
    if(g_otau_data.transfer_state != CTRL_IN_PROGRESS)
    {
        return;
    }

    switch (g_otau_data.data_transfer_state)
    {
        /*
         * This is the first 8 bytes of the VM upgrade file header.
         * Request the LENGTH parameter.
         */
        case data_transfer_state_header_id:
        {
            GAIA_OTAU_EVENT_T event_data;
            event_data.validate_header.header_valid = TRUE;
            event_data.validate_header.header = file_data;

            notifyApplication(gaia_otau_event_validate_header, &event_data);

            g_otau_data.data_transfer_state = data_transfer_state_header_length;
            if ( event_data.validate_header.header_valid )
            {
                upgradeDeviceSendDataBytesReq(device_id, HEADER_LENGTH_SIZE);
            }
            else
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_UPGRADE_HEADER);
            }
        }
            break;

        /*
         * Get the length of the rest of the header, then request the rest of the bytes
         * including the header for the first partition
         *
         * The file header has the following format
         * Offset  <Value> or Fixed Text
         * 0       <8 octet string>
         * 8       <Length bytes>
         * 12      <header body>
         */
        case data_transfer_state_header_length:
            g_otau_data.header_length = GetUint32FromArray(file_data);
            g_otau_data.data_transfer_state = data_transfer_state_header_body;
            upgradeDeviceSendDataBytesReq(device_id, g_otau_data.header_length);
            break;

        /*
         * Read the header body
         * There can be multiple UPGRADE_DATA() packets in response to a single
         * UPGRADE_DATA_BYTES_REQ() call because the header body can be larger
         * then the MTU size.
         */
        case data_transfer_state_header_body:
        {
            GAIA_OTAU_EVENT_T event_data;
            event_data.validate_header_body.header_body_valid = TRUE;
            event_data.validate_header_body.header_body_size = g_otau_data.header_length;
            event_data.validate_header_body.portion_size = length;
            event_data.validate_header_body.header_body = file_data;

            notifyApplication(gaia_otau_event_validate_header_body, &event_data);

            if ( event_data.validate_header_body.header_body_valid )
            {
                g_otau_data.bytes_transferred += length;
                if(g_otau_data.bytes_transferred < g_otau_data.header_length)
                {
                    break;  /* wait until the rest of the header has been received */
                }
                else        /* request the partition header */
                {
                    g_otau_data.bytes_transferred = 0;
                    g_otau_data.data_transfer_state = data_transfer_state_unknown_header_id;
                    upgradeDeviceSendDataBytesReq(device_id, UNKNOWN_TYPE_ID_SIZE);
                }
            }
            else  /* header validation has failed */
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_UPGRADE_HEADER);
            }
        }
            break;

        /* download the next header and determine next action accordingly
         * Supported types are: PARTITION_HEADER and FOOTER_HEADER
         */
        case data_transfer_state_unknown_header_id:
            if ( isPartitionHeader(file_data) )     /* process a partition */
            {
                g_otau_data.data_transfer_state = data_transfer_state_partition_header_body;
                upgradeDeviceSendDataBytesReq(device_id,
                                                PARTITION_HEADER_LENGTH_SIZE +
                                                PARTITION_HEADER_TYPE_SIZE +
                                                PARTITION_HEADER_NUMBER_SIZE );
            }
            else if ( isFooterHeader(file_data) )   /* no more partitions, read the footer */
            {
                g_otau_data.data_transfer_state = data_transfer_state_footer_length;
                upgradeDeviceSendDataBytesReq(device_id, FOOTER_LENGTH_SIZE );
            } else  /* unknown section ID */
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_HEADERS);
            }
            break;

        /*
         * Read the rest of the partition header, starting with the partition header
         *
         * Partition headers have the following format
         * Offset  <Value> or Fixed Text
         * 0       PARTDATA  <-- this was already transferred in previous state
         * 8       <Length>  <-- file_data points here
         * 12      <part type>
         * 14      <Partition num>
         * 16      <1st byte of data...>
         */
        case data_transfer_state_partition_header_body:
        {
            GAIA_OTAU_EVENT_T event_data;

            /* calculate the length of the partition  */
            g_otau_data.partition.data_length = GetUint32FromArray(file_data) -
                                                PARTITION_HEADER_TYPE_SIZE -
                                                PARTITION_HEADER_NUMBER_SIZE;
            g_otau_data.partition.type = GetUint16FromArray(file_data +
                                        PARTITION_HEADER_TYPE_OFFSET -
                                        PARTITION_HEADER_LENGTH_OFFSET );
            /*
             * App Store ID are Logical since they are swapped each upgrade,
             * User Store IDs are Physical
             */
            if ( g_otau_data.partition.type == APP_STORE )
            {
                g_otau_data.partition.id = getAppStoreToUpgrade(   g_otau_data.app_id,
                                                                g_otau_data.cs_id);

            } else
            {
                g_otau_data.partition.id = GetUint16FromArray(file_data +
                                            PARTITION_HEADER_NUMBER_OFFSET -
                                            PARTITION_HEADER_LENGTH_OFFSET );
            }
            /*
             * Notify the application of the partition information, and allow
             * the application to modify the ID and Type
             */
            event_data.partition_info.partition_type = g_otau_data.partition.type;
            event_data.partition_info.partition_id = g_otau_data.partition.id;
            event_data.partition_info.partition_size = g_otau_data.partition.data_length;
            notifyApplication(gaia_otau_event_partition_info, &event_data);
            g_otau_data.partition.type = event_data.partition_info.partition_type;
            g_otau_data.partition.id = event_data.partition_info.partition_id;

            /* convert the partition length to the internal format used by
             * the Store firmware */
            if ( storage_size_encode( g_otau_data.partition.data_length,
                                      &g_otau_data.partition.store_size) == sys_status_success )
            {
                /*
                 * SetStoreID will erase the Flash, and this can take a
                 * few seconds for large devices. Wait for the confirmation
                 * from the firmware before sending the confirmation to the
                 * host.
                 * See GaiaOtauHandleStoreUpdateMsg()
                 */
                switch ( g_otau_data.partition.type )
                {
                    /* Application Stores swap between two application partitions */
                    case APP_STORE:
                        g_otau_data.app_upgraded = TRUE;
                        StoreUpdate_SetStoreId( g_otau_data.partition.id,
                                                g_otau_data.partition.type,
                                                g_otau_data.partition.store_size);
                        break;

                    /* User stores aren't double buffered, so update in-place */
                    case USER_STORE:
                        StoreUpdate_SetStoreId( g_otau_data.partition.id,
                                                g_otau_data.partition.type,
                                                g_otau_data.partition.store_size);
                        break;

                    default:    /* unknown store type */
                        upgradeDeviceSendErrorInd(device_id,
                                UPGRADE_HOST_ERROR_PARTITION_TYPE_NOT_MATCHING);
                        break;
                }
            }
            else
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_PARTITION_HEADER2);
            }
            g_otau_data.data_transfer_state = data_transfer_state_partition_data;
        }
            break;

        /* Transfer the data which is stored in the partition. */
        case data_transfer_state_partition_data:
        {
            uint8 store_buffer[20];
            uint32 bytes_remaining;

            /*
             * Store the partition data in to the store
             * The first packet includes the "Store Header", which will
             * automatically be added by the Store firmware, so
             * this is skipped (4 Words, 8 octets)
             */
            if(g_otau_data.bytes_transferred == 0)  /* first packet, skip Store Header */
            {
                SwapBytes(  length - STORE_HEADER_SIZE_OCTETS,
                            file_data + STORE_HEADER_SIZE_OCTETS,
                            store_buffer);
                StoreUpdate_WriteChunk(length - STORE_HEADER_SIZE_OCTETS, store_buffer);
                g_otau_data.bytes_transferred += length;
                g_otau_data.partition.last_chunk = FALSE;
            }
            else    /* not the first packet, write to Flash as normal */
            {
                SwapBytes(length, file_data, store_buffer);
                StoreUpdate_WriteChunk(length, store_buffer);
                g_otau_data.bytes_transferred += length;
            }

            /*
             * continue to request packets until a complete partition has been
             * received
             */
            bytes_remaining = g_otau_data.partition.data_length - g_otau_data.bytes_transferred;
            if(bytes_remaining == 0)
            {
                g_otau_data.partition.last_chunk = TRUE;
                g_otau_data.bytes_transferred = 0;
                g_otau_data.data_transfer_state = data_transfer_state_unknown_header_id;
                upgradeDeviceSendDataBytesReq(device_id, UNKNOWN_TYPE_ID_SIZE );
            }
            else if(bytes_remaining < DATA_REQUEST_SIZE)
                upgradeDeviceSendDataBytesReq(device_id, bytes_remaining);
            else
                upgradeDeviceSendDataBytesReq(device_id, DATA_REQUEST_SIZE);
            break;
        }

        /*
         * Parse the footer to get the length of the OEM signature, then request
         * the signature bytes
         */
        case data_transfer_state_footer_length:
            g_otau_data.oem_signature.length = GetUint32FromArray(&file_data[FOOTER_LENGTH_OFFSET - FOOTER_ID_SIZE]);
            if ( g_otau_data.oem_signature.length == 0 )    /* no signature, so assume validation always succeeds */
            {
                g_otau_data.validation_done = TRUE;
            }
            else  /* request the signature bytes */
            {
                g_otau_data.bytes_transferred = 0;
                g_otau_data.data_transfer_state = data_transfer_state_footer_oem_signature;
                upgradeDeviceSendDataBytesReq(  device_id,
                                                min(g_otau_data.oem_signature.length, DATA_REQUEST_SIZE) );
            }
            break;

        /*
         * retrieve the oem signature bytes and compare to the calculated signature
         * this state may be accessed several times if the signature is larger
         * than the maximum request size
         */
        case data_transfer_state_footer_oem_signature:
        {
            uint16 i;
            uint16 SHA256_word;
            uint32 bytes_remaining;
            bool check_failed;

            /* check this chunk of signature against what has been calculated so far */
            check_failed = FALSE;
            for ( i = 0; i < length; i += 2  )
            {
                SHA256_word = ( file_data[i] << 8 )  + file_data[i+1];

                if ( g_otau_data.oem_signature.sha256_sum[(g_otau_data.bytes_transferred + i ) / 2] != SHA256_word  )
                {
                    /* signature check failed */
                    upgradeDeviceSendErrorInd(  device_id,
                                                UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_FOOTER);
                    check_failed = TRUE;
                    break;
                }
            }

            /* retrieve remaining signature bytes, and check for end */
            g_otau_data.bytes_transferred += length;
            bytes_remaining = g_otau_data.oem_signature.length - g_otau_data.bytes_transferred;
            if ( !check_failed )
            {
                if(more_data_flag == UPGRADE_DATA_LAST_PACKET)
                {
                    if ( bytes_remaining != 0)  /* not enough bytes received, fail */
                    {
                        upgradeDeviceSendErrorInd(  device_id,
                                                    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_FOOTER);
                    } else
                    {
                        g_otau_data.validation_done = TRUE;
                    }
                }
                else  /* continue to request signature bytes */
                {
                    if(bytes_remaining == 0)
                    {
                        /* signature received and all bytes matched */
                        g_otau_data.validation_done = TRUE;
                    }
                    else if(bytes_remaining < DATA_REQUEST_SIZE)
                    {
                        upgradeDeviceSendDataBytesReq(device_id, bytes_remaining);
                    }
                    else
                    {
                        upgradeDeviceSendDataBytesReq(device_id, DATA_REQUEST_SIZE);
                    }
                }
            }
        }
            break;

        default: break;
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      hostCommitTimeoutHandler
 *
 *  DESCRIPTION
 *      Handles the Host Commit Timeout timer. When this timer expires, the
 *      device should rollback and reboot
 *
 *  PARAMETERS
 *      tid [in]      timer ID
 *
 *  RETURNS
 *      none
 *
 *---------------------------------------------------------------------------*/
static void hostCommitTimeoutHandler(timer_id tid)
{
    g_otau_data.commit_timeout_tid = TIMER_INVALID;
    setDefaultNVM();
    g_otau_data.transfer_state = CTRL_WAITING_TO_REBOOT;
    notifyApplication(gaia_otau_event_upgrade_failed, NULL);
    notifyApplicationRebootWarning();

    /* reboot, unless the application blocks it temporarily */
    if ( !g_otau_data.reboot_wait )
    {
        WarmReset();
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      hostCommitTimeoutCreate, hostCommitTimeoutDelete
 *
 *  DESCRIPTION
 *      Create or Delete the timeout for the Host Commit timeout
 *
 *  PARAMETERS
 *
 *  RETURNS
 *      none
 *
 *---------------------------------------------------------------------------*/
static void hostCommitTimeoutCreate(void)
{
    if ( g_otau_data.commit_timeout_tid == TIMER_INVALID )
    {
        g_otau_data.commit_timeout_tid = TimerCreate(  GAIA_OTAU_COMMIT_HOST_TIMEOUT,
                                                    TRUE,
                                                    hostCommitTimeoutHandler);
    }
}


static void hostCommitTimeoutDelete(void)
{
    TimerDelete( g_otau_data.commit_timeout_tid );
    g_otau_data.commit_timeout_tid = TIMER_INVALID;
}


/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauInit
 *
 *  DESCRIPTION
 *      This function is used to initialise upgrade data
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GaiaOtauInit(bool nvm_start_fresh, uint16 *nvm_offset)
{
    /* Initialise g_otau_data */
    g_otau_data.transfer_state = CTRL_PRE_FAIL; /* assume failure until CS store keys can be read */
    initStateVariables();
    g_otau_data.commit_timeout_tid = TIMER_INVALID;
    g_otau_data.cs_id_read = FALSE;
    readDataFromNVM(nvm_start_fresh, nvm_offset);
    g_otau_data.callback = NULL;

    /*
     * request the current AppId and the CS key AppId because it might
     * be needed later
     */
    g_otau_data.app_id = StoreUpdate_GetAppId().id;
    ConfigStoreReadKey(CS_ID_APP_STORE_ID, STORE_ID_UNUSED);

    /* if this is the new, but temporary application, create a reboot timer in
     * case the Host never makes contact again
     */
    if ( g_otau_data.resume_point == UPGRADE_RESUME_POINT_POST_REBOOT )
    {
        hostCommitTimeoutCreate();
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauHandleConnNotify
 *
 *  DESCRIPTION
 *      This function is used to handle a connection event
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GaiaOtauHandleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data)
{
    if(cm_event_data->result == cm_conn_res_success)
    {
        /* Initialise g_otau_data */
        if ( g_otau_data.cs_id_read == FALSE )
        {
            g_otau_data.transfer_state = CTRL_PRE_FAIL; /* assume failure until CS store keys can be read */
        }
        else
        {
            g_otau_data.transfer_state = CTRL_WAIT_FOR_HOST;
        }
        initStateVariables();
    }
    else if(cm_event_data->result == cm_disconn_res_success)
    {
        /*
         * The reason for this disconnection could have been because the
         * the OTAu library is going to run the new application for the first time
         * or if an abort occurred while running the new application.
         */
        if ( g_otau_data.disconnect_reset == TRUE )
        {
            if ( g_otau_data.reboot_wait )
            {
                g_otau_data.transfer_state = CTRL_WAITING_TO_REBOOT;
            } else
            {
                WarmReset();
            }
        }
    }
}

static void handleVmUpgradeHostSyncReq(void)
{
    if ( g_otau_data.new_in_progress_id == GAIA_IN_PROGRESS_IDENTIFIER_NONE )
    {
        upgradeDeviceSendErrorInd(g_otau_data.device_id, UPGRADE_HOST_ERROR_INVALID_SYNC_ID);
    }
    else if (   g_otau_data.InProgressIdentifier == GAIA_IN_PROGRESS_IDENTIFIER_NONE ||
                g_otau_data.InProgressIdentifier == g_otau_data.new_in_progress_id )
    {
        if ( g_otau_data.InProgressIdentifier == GAIA_IN_PROGRESS_IDENTIFIER_NONE )
        {
            g_otau_data.InProgressIdentifier = g_otau_data.new_in_progress_id;
            Nvm_Write((uint16*)&g_otau_data.InProgressIdentifier,
                    sizeof(g_otau_data.InProgressIdentifier),
                    g_otau_data.nvm_offset + GAIA_NVM_IN_PROGRESS_IDENTIFIER_OFFSET);
        }

        upgradeDeviceSendSyncCfm(g_otau_data.device_id, g_otau_data.InProgressIdentifier);
    }
    else  /* In Progress Identifiers do not match */
    {
        upgradeDeviceSendErrorInd(g_otau_data.device_id, UPGRADE_HOST_WARN_SYNC_ID_IS_DIFFERENT);
    }

    g_otau_data.transfer_state = CTRL_WAIT_FOR_HOST;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauHandleVmUpgradeControlCommand
 *
 *  DESCRIPTION
 *      Handle a GAIA_COMMAND_VM_UPGRADE_CONTROL message. These contain the
 *      VM upgrade protocol message opcodes
 *      Type:  [VM upgrade msg opcode][length][data] ... [data]
 *      Offset: 0                       1-2     3
 *
 *  PARAMETERS
 *      device_id [in] originating device to respond to
 *      payload_size [in] size of the payload
 *      payload [in] data payload passed by the VM Upgrade Control message
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/
extern void GaiaOtauHandleVmUpgradeControlCommand(  device_handle_id device_id,
                                                    uint16 payload_size,
                                                    uint8 *payload)
{
    uint16 msgId;

    /* Reset the commit timeout */
    if ( g_otau_data.resume_point == UPGRADE_RESUME_POINT_POST_REBOOT )
    {
        hostCommitTimeoutDelete();
        hostCommitTimeoutCreate();
    }

    /* get the payload */
    msgId = GetUint8FromArray(payload);
    switch(msgId)
    {
        case UPGRADE_HOST_SYNC_REQ:
        {
            GAIA_OTAU_EVENT_T event_data;
            event_data.upgrade_starting.continue_immediately = TRUE;

            notifyApplication(gaia_otau_event_upgrade_starting, &event_data);

            /* Save local data for later use */
            g_otau_data.device_id = device_id;
            g_otau_data.new_in_progress_id = GetUint32FromArray(&payload[GAIA_COMMAND_VM_UPGRADE_CONTROL_DATA_OFFSET]);

            if ( event_data.upgrade_starting.continue_immediately )
            {
                handleVmUpgradeHostSyncReq();
            }
            else
            {
                /* Wait for application to call GaiaOtauContinueUpgrade */
                g_otau_data.transfer_state = CTRL_WAIT_FOR_DEVICE;
            }
        }
            break;

        case UPGRADE_HOST_START_REQ:
            if ( g_otau_data.transfer_state != CTRL_PRE_FAIL )
            {
                upgradeDeviceSendStartCfm(device_id, UPGRADE_HOST_SUCCESS);
            }
            else  /* most likely == CTRL_PRE_FAIL */
            {
                upgradeDeviceSendStartCfm(device_id, UPGRADE_HOST_ERROR_APP_NOT_READY);
            }
            break;

        /*
         * request the first 8 bytes of the VM upgrade file header,
         * this contains the ID/version string
         */
        case UPGRADE_HOST_START_DATA_REQ:
            g_otau_data.transfer_state = CTRL_IN_PROGRESS;
            g_otau_data.data_transfer_state = data_transfer_state_header_id;
            g_otau_data.app_upgraded = FALSE;
            upgradeDeviceSendDataBytesReq(device_id, HEADER_ID_SIZE);
            break;

        /*
         * this message contains part of the upgrade file, pass it on for further
         * processing
         */
        case UPGRADE_HOST_DATA:
        {
            uint16 upgrade_data_length;

            upgrade_data_length = GetUint16FromArray(&payload[GAIA_COMMAND_VM_UPGRADE_CONTROL_LENGTH_OFFSET]);
            /*
             * The Packet contains [MORE_DATA flag][UPGRADE DATA ...]
             * Remove the MORE_DATA flag, pass as an argument, then pass
             * the offset of just the upgrade data.
             */
            handleOtauFileTransfer( device_id,
                                    payload[UPGRADE_DATA_MORE_DATA_OFFSET],
                                    upgrade_data_length - UPGRADE_DATA_MORE_DATA_SIZE,
                                    &payload[UPGRADE_DATA_IMAGE_DATA_OFFSET]);
        }
            break;

        /*
         * Host indicates Abort
         * Clear any NVM variables, and respond.
         */
        case UPGRADE_HOST_ABORT_REQ:
            setDefaultNVM();
            upgradeDeviceSendAbortCfm(device_id);
            notifyApplication(gaia_otau_event_upgrade_failed, NULL);

            /*
             * If this is the new uncommitted application, rollback to the previous application
             * The reboot will happen once the disconnect has occurred
             */
            if ( isNewApp() )
            {
                g_otau_data.disconnect_reset = TRUE;
                notifyApplicationRebootWarning();
                CMDisconnect(device_id);
            } else
            {
                /* reset the state variables in case the host makes another attempt
                 * without reconnecting
                 */
                initStateVariables();
            }
            break;

        case UPGRADE_HOST_IS_CSR_VALID_DONE_REQ:
            if(g_otau_data.validation_done)
            {
                upgradeDeviceSendTransferCompleteInd(device_id);
            }
            else
            {
                upgradeDeviceSendValidationCfm(device_id);
            }
            break;

        /*
         * Midpoint of the upgrade, do the first reboot to run the app on a
         * temporary basis. Don't reboot if this is only a User Store upgrade
         */
        case UPGRADE_HOST_TRANSFER_COMPLETE_RES:
            if ( payload[UPGRADE_TRANSFER_COMPLETE_RES_ACTION_OFFSET] ==
                    UPGRADE_TRANSFER_COMPLETE_RES_ACTION_CONTINUE_UPGRADE )
            {
                g_otau_data.resume_point = UPGRADE_RESUME_POINT_POST_REBOOT;
                saveCurrentNVM();

                /*
                 * if the application was upgraded, then set the new application
                 * to run after the reset
                 */
                if ( g_otau_data.app_upgraded )
                {
                    runNewApp(FALSE);
                }

                /* reboot will occur after the disconnect */
                notifyApplicationRebootWarning();
                g_otau_data.disconnect_reset = TRUE;
                CMDisconnect(device_id);
            }
            break;

        case UPGRADE_HOST_IN_PROGRESS_RES:
            /* ACTION value is not used, this command will never trigger an abort */
            upgradeDeviceSendUpdateCommitReq(device_id);
            break;

        /*
         * The updated application or user store is committed at this point.
         * With an application store, this is the final chance to rollback to the
         * previous application.
         * With a user store, there is no rollback.
         */
        case UPGRADE_HOST_COMMIT_CFM:
        {
            uint8 action;

            /* clear the NVM variables because this upgrade has completed */
            setDefaultNVM();

            /* act on the result from the host */
            action = GetUint8FromArray(&payload[GAIA_COMMAND_VM_UPGRADE_CONTROL_DATA_OFFSET]);
            if ( action == UPGRADE_COMMIT_CFM_ACTION_COMMIT_UPGRADE )
            {
                hostCommitTimeoutDelete();
                upgradeDeviceSendUpdateCompleteInd(device_id);
                notifyApplication(gaia_otau_event_new_app_commit, NULL);

                /* Commit the application if we are the new application,
                 * and then delete the old application once it's committed */
                if ( isNewApp() )
                {
                    runNewApp(TRUE);
                }
            }
            else    /* don't commit */
            {
                /* roll back to the previous application, once the disconnect
                 * has gone through
                 */
                g_otau_data.disconnect_reset = TRUE;
                notifyApplication(gaia_otau_event_upgrade_failed, NULL);
                notifyApplicationRebootWarning();
                CMDisconnect(device_id);
            }
        }
            break;

        case UPGRADE_HOST_ERRORWARN_RES:
            break;

        default:
            break;
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauHandleStoreUpdateMsg
 *
 *  DESCRIPTION
 *      This function handles the config store messages
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauHandleStoreUpdateMsg(device_handle_id device_id, store_update_msg_t *msg)
{
    status_t status;

    switch(msg->header.id)
    {
        case STORE_UPDATE_SET_STORE_ID_CFM:
            /*
             * This message originates from handleOtauFileTransfer(),
             * Once the Store is ready, the rest of the application
             * can be requested
             */
            status = msg->body.set_store_id_cfm.status;
            if( status == sys_status_too_much_data ) /* file too big */
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_FILE_TOO_BIG);
            }
            else if ( status == sys_status_no_store )   /* no such store */
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_PARTITION_OPEN_FAILED);
            }
            else
            {
                /*
                 * begin transfer of the partition, otherwise this was the
                 * message for when the old application was deleted after a commit
                 */
                if ( !isNewApp() )
                {
                    upgradeDeviceSendDataBytesReq(device_id, DATA_REQUEST_SIZE);
                }
                else
                {
                    g_otau_data.cs_id = g_otau_data.app_id;
                }
            }
            break;

        case STORE_UPDATE_WRITE_CHUNK_CFM:
            /* start the SHA256 calculation on the last chunk */
            status = msg->body.write_chunk_cfm.status;
            if ( status == sys_status_success )
            {
                if ( g_otau_data.partition.last_chunk )
                {
                    startSHA256sum();
                }
            }
            else if(status != sys_status_success )
            {
                /* Abort after having sent the error code back over the GATT. */
                g_otau_data.transfer_state = CTRL_ABORTED;
                if( status == sys_status_too_much_data )
                {
                    upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_FILE_TOO_BIG);
                }
                else
                {
                    upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_UPDATE_FAILED);
                }
            }
            break;

        case STORE_UPDATE_CHECK_INTEGRITY_CFM:
            status = msg->body.check_integrity_cfm.status;
            g_otau_data.validation_done = TRUE;
            /* Check the status to see if the integrity check succeeded or not */
            if( status == sys_status_success )
            {
                /* Win: switch to the initial state where, the host can run the new app. */
                g_otau_data.transfer_state = CTRL_READY;
            }
            else    /* integrity check failed */
            {
                g_otau_data.transfer_state = CTRL_FAILED;
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_MEMORY);
            }
            break;

        case STORE_UPDATE_RUN_APP_CFM:
            status = msg->body.run_app_cfm.status;
            if( status == sys_status_success )
            {
                /*
                 * The new application has been committed
                 * Delete the old application if this is the new application
                 * Update the cs_id and the app_id, and recalculate the store to upgrade
                 * This is required in case the user does another upgrade before resetting
                 * the device.
                 */
                StoreUpdate_SetStoreId( g_otau_data.cs_id,
                                        APP_STORE,
                                        SETSTOREID_SIZE_MIN);
                initStateVariables();
            }
            break;

        case STORE_UPDATE_ABORT_IND:
            /* We received an abort from the memory subsystem. We behave accordingly. */
            status = msg->body.abort_ind.status;
            g_otau_data.transfer_state = CTRL_ABORTED;
            if( sys_status_too_much_data == status )
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_FILE_TOO_BIG);
            }
            else
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_UPDATE_FAILED);
            }
            break;

        case STORE_UPDATE_HASH_STORE_CFM:
        {
            int i;
            int MSB_sum, LSB_sum;
            status = msg->body.hash_store_cfm.status;

            if ( status == sys_status_success )
            {

                /* sum this hash with the previous hash
                 * it is a byte sum, so split in to 8octet sums and don't carry
                 */
                for ( i = 0; i < SHA256_SIZE_WORDS; i++ )
                {
                    MSB_sum =   ((g_otau_data.oem_signature.sha256_sum[i]&0xff00) +
                                (g_otau_data.oem_signature.sha256_buffer[i]&0xff00) );
                    MSB_sum &= 0xff00;

                    LSB_sum =   ((g_otau_data.oem_signature.sha256_sum[i]) +
                                (g_otau_data.oem_signature.sha256_buffer[i]) );
                    LSB_sum &= 0x00ff;

                    g_otau_data.oem_signature.sha256_sum[i] = MSB_sum | LSB_sum;
                }

                if ( g_otau_data.partition.last_partition )
                {
                    g_otau_data.validation_done = TRUE;
                }
            }
            else
            {
                upgradeDeviceSendErrorInd(device_id, UPGRADE_HOST_ERROR_INTERNAL_ERROR_1);
            }
        }
            break;

        default:
            /* This message is not to be handled by this function. Exit silently. */
            return;
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauConfigStoreMsg
 *
 *  DESCRIPTION
 *      This function handles the config store messages
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauConfigStoreMsg(msg_t *msg)
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
                    g_otau_data.cs_id_read = TRUE;
                    g_otau_data.cs_id = cs_msg->body.read_key_cfm.value[0];
                    g_otau_data.transfer_state = CTRL_WAIT_FOR_HOST;

                    /* notify the application if this is the new application */
                    if ( isNewApp() )
                    {
                        notifyApplication(gaia_otau_event_upgraded_application, NULL);
                    }
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
 *      GaiaOtauRegisterCallback
 *
 *  DESCRIPTION
 *      Set the callback so the OTAu library can inform the application
 *      of the progress and warn about special events.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauRegisterCallback(gaia_otau_event_handler callback)
{
    g_otau_data.callback = callback;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauUpgradeProgress
 *
 *  DESCRIPTION
 *      Returns the current progress of the application transfer
 *
 *  RETURNS/MODIFIES
 *      partition_size = bytes in the partition
 *      partition_progress = number of bytes transferred so far
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauUpgradeProgress( uint32 *partition_size, uint32 *partition_progress )
{
    if ( g_otau_data.data_transfer_state == data_transfer_state_partition_data )
    {
        *partition_size = g_otau_data.partition.data_length;
        *partition_progress = g_otau_data.bytes_transferred;
    }
    else
    {
        *partition_size = *partition_progress = 0;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauContinueUpgrade
 *
 *  DESCRIPTION
 *      Continues with the upgrade process after waiting for the application
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaOtauContinueUpgrade(void)
{
    switch ( g_otau_data.transfer_state )
    {
        case CTRL_WAIT_FOR_DEVICE:
            handleVmUpgradeHostSyncReq();
            break;

        case CTRL_WAITING_TO_REBOOT:
            WarmReset();
            break;

        default:
            break;
    }
}

