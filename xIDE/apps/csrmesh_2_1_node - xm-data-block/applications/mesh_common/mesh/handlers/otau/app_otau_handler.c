/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_otau_handler.c
 *
 *
 ******************************************************************************/

#ifdef GAIA_OTAU_SUPPORT
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <mem.h>
#include <timer.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "app_debug.h"
#include "gaia.h"
#include "gaia_otau_api.h"
#include "app_otau_handler.h"
#include "connection_handler.h"
#include "byte_utils.h"
#include "app_conn_params.h"
#include "conn_param_update.h"
#include "csr_mesh.h"
#include "csr_sched.h"
#include "app_mesh_handler.h"
#ifdef GAIA_OTAU_RELAY_SUPPORT
#include "gaia_otau_client_api.h"
#endif
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#define UPGRADE_HEADER_UPGRADE_VERSION_SIZE         (4)
#define UPGRADE_HEADER_UPGRADE_VERSION_MAJOR_SIZE   (2)
#define UPGRADE_HEADER_UPGRADE_VERSION_MINOR_SIZE   (2)
#define UPGRADE_HEADER_COMPANY_CODE_SIZE            (2)
#define UPGRADE_HEADER_DEVICE_VARIANT_SIZE          (1)
#define UPGRADE_HEADER_UPGRADE_TYPE_SIZE            (1)
#define UPGRADE_HEADER_DEVICE_APPEARANCE_SIZE       (1)
#define UPGRADE_HEADER_DEVICE_VERSION_SIZE          (3)
#define UPGRADE_HEADER_NVM_VERSION_SIZE             (2)
#define COMPANY_CODE_OFFSET                         (4)   
#define DEVICE_VARIANT_OFFSET                       (6)
#define UPGRADE_TYPE_OFFSET                         (7)
#define DEVICE_APPEARANCE_OFFSET                    (8)
#define DEVICE_VERSION_OFFSET                       (9)
#define NVM_VERSION_OFFSET                          (12)
#define MESH_OTAU_HEADER_ID                         "APPUHDR4"
#define MESH_OTAU_HEADER_ID_LEN                     (8)

/*============================================================================*
 *  Private Data Types
 *============================================================================*/

typedef enum {
    upgrade_header_upgrade_version = 0x0,
    upgrade_header_company_code,
    upgrade_header_device_variant,
    upgrade_header_upgrade_type,
    upgrade_header_device_appearance,
    upgrade_header_device_version,
    upgrade_header_nvm_version
} UPGRADE_HEADER_DATA_TYPE;

/* This structure defines the local data store for this module */
typedef struct
{
    UPGRADE_HEADER_DATA_TYPE data_type;     /* used to manage the VM upgrade header data */
    uint16 upgrade_version_major;
    uint16 upgrade_version_minor;
    uint16 company_code;
    uint8 device_variant;
    int8 upgrade_type;
    uint8 device_appearance;
    uint16 device_version;
    uint16 nvm_version;
    bool commit_cfm;
#ifdef GAIA_OTAU_RELAY_SUPPORT
    uint8 signature[32];
    uint8 hdr_data[18];
    su_store_type_t store_type;             /* type of store */
    store_id_t store_id;                    /* store ID */
#endif
} APP_OTA_DATA_T;

/*============================================================================*
 *  Private Data Types
 *============================================================================*/
static APP_OTA_DATA_T g_app_otau_data;

uint16 old_version;
/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnParamUpdateCallback
 *
 *  DESCRIPTION
 *      This function handles the connection parameter update callback
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleConnParamUpdateCallback(bool params_applied)
{
    /* Unregister callback */
    ConnectionParamUpdateSetCallback(NULL);
    
    /* Continue with upgrade */
    if (IsGaiaOtauInProgress())
    {
        GaiaOtauContinueUpgrade();
    }
}
 
/*----------------------------------------------------------------------------*
 *  NAME
 *      otauCallbackUpdateStarting
 *
 *  DESCRIPTION
 *      Callback for indicating the start of OTAu
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void otauCallbackUpdateStarting(GAIA_OTAU_EVENT_UPGRADE_STARTING_T *event_data)        
{
    CONN_PARAMS_T conn_params;

#ifdef GAIA_OTAU_RELAY_SUPPORT
    GaiaOtauSetCommitStatus(FALSE);
#endif
    
    /* Stop connection parameter update if there was one pending */
    StopConnParamsUpdate();
    
    if (IsGaiaOtauUpgradedApplication())
    {
        /* No more data to download, no need to change connection parameters */
        event_data->continue_immediately = TRUE;
    }
    else
    {
        if ((GetUpdatedConnParams(&conn_params) &&
            conn_params.conn_interval <= OTAU_APPLE_MAX_CON_INTERVAL)|| 
            ((!GetUpdatedConnParams(&conn_params)) && 
             (GetConnInterval() != 0) && (GetConnInterval() <= OTAU_APPLE_MAX_CON_INTERVAL)))
        {
            /* Connection interval already low enough, no need to change */
            event_data->continue_immediately = TRUE;
        }
        else
        {
            /* Request faster connection parameters for downloading update */
            ConnectionParamUpdateSetCallback(handleConnParamUpdateCallback);
            DEBUG_STR("\r\nRequesting New Connection Parameters");
            AppRequestConnParamUpdate(GetConnectedDeviceId());

            event_data->continue_immediately = FALSE;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      updateStoreType
 *
 *  DESCRIPTION
 *      Update the store type to User Store
 *
 *  RETURNS
 *      Status
 *
 *---------------------------------------------------------------------------*/
static bool updateStoreType(void)
{
    bool status = FALSE;
#if defined(GAIA_OTAU_RELAY_SUPPORT)
     handle_t user_handle;
     status_t store_status = Storage_FindStore(USER_STORE_GAIA_OTAU_ID, USER_STORE,
                                         &user_handle);
     if(store_status == sys_status_success)
     {
         g_app_otau_data.store_type = USER_STORE;
         g_app_otau_data.store_id = USER_STORE_GAIA_OTAU_ID;
         status = TRUE;
     }
#endif
    return status;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      otauCallbackValidateHeaderID
 *
 *  DESCRIPTION
 *      Callback for validating header id
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void otauCallbackValidateHeaderID(GAIA_OTAU_EVENT_VALIDATE_HEADER_T *event_data)
{
    const uint8 expected_header_id[] = MESH_OTAU_HEADER_ID;

    /* Check header ID string matches */
    if (MemCmp(event_data->header, expected_header_id, MESH_OTAU_HEADER_ID_LEN) != 0)
    {
        event_data->header_valid = FALSE;
    }
    else
    {
        event_data->header_valid = TRUE;
    }
    
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      otauCallbackValidateHeaderBody
 *
 *  DESCRIPTION
 *      Callback for validating header body
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void otauCallbackValidateHeaderBody(GAIA_OTAU_EVENT_VALIDATE_HEADER_BODY_T *event_data)
{
    bool status = TRUE;
    uint8 size = event_data->portion_size;
    uint8 *header_body = event_data->header_body;
    while(size != 0)
    {
        switch (g_app_otau_data.data_type)
        {
            case upgrade_header_upgrade_version:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                MemCopy(g_app_otau_data.hdr_data,header_body,UPGRADE_HEADER_UPGRADE_VERSION_SIZE);
#endif
                g_app_otau_data.upgrade_version_major = 
                        GetUint16FromArray(header_body);
                g_app_otau_data.upgrade_version_minor = 
                        GetUint16FromArray(header_body + 
                                           UPGRADE_HEADER_UPGRADE_VERSION_MAJOR_SIZE); 
                size -= UPGRADE_HEADER_UPGRADE_VERSION_SIZE;
                header_body += UPGRADE_HEADER_UPGRADE_VERSION_SIZE;
                g_app_otau_data.data_type = upgrade_header_company_code;
            }
            break;
            case upgrade_header_company_code:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                MemCopy(&g_app_otau_data.hdr_data[COMPANY_CODE_OFFSET],header_body,UPGRADE_HEADER_COMPANY_CODE_SIZE);
#endif
                g_app_otau_data.company_code = 
                        GetUint16FromArray(header_body);
                size -= UPGRADE_HEADER_COMPANY_CODE_SIZE;
                header_body += UPGRADE_HEADER_COMPANY_CODE_SIZE;

                if(g_app_otau_data.company_code != 0x00D7 && 
                   g_app_otau_data.company_code != 0x000A)
                {
                    status = updateStoreType();
                }
                if(status)
                    g_app_otau_data.data_type = upgrade_header_device_variant; 
            }
            break;
            case upgrade_header_device_variant:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                g_app_otau_data.hdr_data[DEVICE_VARIANT_OFFSET] = header_body[0];
#endif
                g_app_otau_data.device_variant = header_body[0];
                size -= UPGRADE_HEADER_DEVICE_VARIANT_SIZE;
                header_body += UPGRADE_HEADER_DEVICE_VARIANT_SIZE;
                
                if(g_app_otau_data.device_variant != csr102x)
                {
                    status = updateStoreType();
                }
                if(status)
                    g_app_otau_data.data_type = upgrade_header_upgrade_type;                    
            }
            break;
            case upgrade_header_upgrade_type:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                g_app_otau_data.hdr_data[UPGRADE_TYPE_OFFSET] = header_body[0];
#endif
                g_app_otau_data.upgrade_type = header_body[0];
                size -= UPGRADE_HEADER_UPGRADE_TYPE_SIZE;
                header_body += UPGRADE_HEADER_UPGRADE_TYPE_SIZE;
                if(g_app_otau_data.upgrade_type != firmware_upgrade)
                {
                    status = updateStoreType();
                }
                if(status)
                    g_app_otau_data.data_type = upgrade_header_device_appearance;

            }
            break;
            case upgrade_header_device_appearance:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                g_app_otau_data.hdr_data[DEVICE_APPEARANCE_OFFSET] = header_body[0];
#endif
                g_app_otau_data.device_appearance = header_body[0];
                size -= UPGRADE_HEADER_DEVICE_APPEARANCE_SIZE;
                header_body += UPGRADE_HEADER_DEVICE_APPEARANCE_SIZE;
                if(g_app_otau_data.device_appearance != IMAGE_TYPE)
                {
                    status = updateStoreType();
                }
                if(status)
                    g_app_otau_data.data_type = upgrade_header_device_version;
                    
            }
            break;
            case upgrade_header_device_version:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                MemCopy(&g_app_otau_data.hdr_data[DEVICE_VERSION_OFFSET],&header_body[0],3);
#endif
                g_app_otau_data.device_version = ((((uint16)(header_body[0] & 0x3F)) << 10) |
                                                     (((uint16)(header_body[1] & 0x0F)) << 6) |
                                                     ((uint16)(header_body[2] & 0x3F)));
                
                old_version = (((APP_MAJOR_VERSION & 0x3F) << 10)|
                                      ((APP_MINOR_VERSION & 0x0F) << 6) |
                                      (APP_NEW_VERSION & 0x3F));
                              
                size -= UPGRADE_HEADER_DEVICE_VERSION_SIZE ;
                
                if(g_app_otau_data.device_version <= old_version)
                {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                    if(g_app_otau_data.store_type != USER_STORE)
                        status = FALSE;
#else
                    status = FALSE; 
#endif       
                }                
                size = 0;
                g_app_otau_data.data_type = upgrade_header_nvm_version;
            }
            break;
            case upgrade_header_nvm_version:
            {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                MemCopy(&g_app_otau_data.hdr_data[NVM_VERSION_OFFSET],&header_body[0],
                        UPGRADE_HEADER_NVM_VERSION_SIZE);
#endif
                g_app_otau_data.nvm_version = GetUint16FromArray(header_body);                
                 old_version = APP_NVM_VERSION;                              
                size -= UPGRADE_HEADER_NVM_VERSION_SIZE ;
                
                if(g_app_otau_data.nvm_version < old_version)
                {
#ifdef GAIA_OTAU_RELAY_SUPPORT
                    if(g_app_otau_data.store_type != USER_STORE)
                        status = FALSE;
#else
                    status = FALSE; 
#endif       
                }                
                size = 0;
                g_app_otau_data.data_type = upgrade_header_upgrade_version;  
#ifdef GAIA_OTAU_RELAY_SUPPORT
                if(status != FALSE)
                {
                    StoreHeaderDataToNVM(g_app_otau_data.hdr_data);
                    
                }
#endif
            }
            break;
            default:
            {
                status = FALSE; 
            }           
            break;
        }      
    }
    if(status == TRUE)
    {
        event_data->header_body_valid = TRUE;
    } 
    else
    {
        event_data->header_body_valid = FALSE;
        AppGaiaOtauDataInit();
        return;
    }    
}

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      otauCallbackHandlePartitionInfo
 *
 *  DESCRIPTION
 *      Callback for Partition Information
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void otauCallbackHandlePartitionInfo(GAIA_OTAU_EVENT_PARTITION_INFO_T *event_data)
{
    StorePartitionDataToNVM(event_data);
    if(g_app_otau_data.store_type == USER_STORE)
    {
        event_data->partition_type = g_app_otau_data.store_type;
        event_data->partition_id = g_app_otau_data.store_id;
    }
    StoreRelayStoreInfo(event_data);    
}

#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      appHandleGaiaOtauEvent
 *
 *  DESCRIPTION
 *      This function handles the events from the GAIA OTAU library
 *
 *  PARAMETERS
 *      event[in]       The type of event being received
 *      *data[in,out]   Pointer to the event data associated with the event
 *
 *  RETURNS
 *     sys_status: sys_status_success if event handled, or sys_status_failed.
 *----------------------------------------------------------------------------*/
static sys_status appHandleGaiaOtauEvent(gaia_otau_event event, GAIA_OTAU_EVENT_T *data)
{
    sys_status status = sys_status_success;
    
    switch (event)
    {
        case gaia_otau_event_upgrade_starting:
            GaiaOtauInProgress(TRUE);
            /* Process the event data */
            otauCallbackUpdateStarting(&data->upgrade_starting);
            break;

        case gaia_otau_event_validate_header:
            /* Process the header id */
            otauCallbackValidateHeaderID(&data->validate_header);
            break;

        case gaia_otau_event_validate_header_body:
            /* Process the event data */
            otauCallbackValidateHeaderBody(&data->validate_header_body);
            break;
            
        case gaia_otau_event_partition_info:
#ifdef GAIA_OTAU_RELAY_SUPPORT
            /* Process the event data */
            otauCallbackHandlePartitionInfo(&data->partition_info);
#endif
            break;      
            
        
        case gaia_otau_event_upgraded_application:
            /* Rebooted into a new application, but upgrade still in progress */
            GaiaOtauUpgradedApplication(TRUE);
            break;

        case gaia_otau_event_new_app_commit:
            /* Indicate that the upgrade has finished successfully, reset flags */
            GaiaOtauUpgradedApplication(FALSE);
            g_app_otau_data.commit_cfm = TRUE;
#ifdef GAIA_OTAU_RELAY_SUPPORT
            GaiaOtauSetRelayStore(TRUE);
#endif
            break;

        case gaia_otau_event_upgrade_failed:
            /* Indicate that upgrade failed, reset flags */
            GaiaOtauUpgradedApplication(FALSE);
#ifdef GAIA_OTAU_RELAY_SUPPORT
            GaiaOtauSetRelayStore(FALSE);
#endif
            break;
            
        case gaia_otau_event_reboot_warning:
            break;

        default:
            status = sys_status_failed;
            break;
    }
    
    return status;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      appHandleGaiaOtauClientEvent
 *
 *  DESCRIPTION
 *      This function handles the events from the GAIA OTAU Client library
 *
 *  PARAMETERS
 *      event[in]       The type of event being received
 *      *data[in,out]   Pointer to the event data associated with the event
 *
 *  RETURNS
 *     sys_status: sys_status_success if event handled, or sys_status_failed.
 *----------------------------------------------------------------------------*/
static sys_status appHandleGaiaEvent(gaia_event event, GAIA_EVENT_T *data)
{
    sys_status status = sys_status_success;
    
    switch (event)
    {
        case gaia_event_upgrade_connect:
            break;
            
        case gaia_event_upgrade_disconnect:    
            if(g_app_otau_data.commit_cfm)
            {
                g_app_otau_data.commit_cfm = FALSE;
                CMDisconnect(GetConnectedDeviceId());
            }
            break;

        default:
            status = sys_status_failed;
            break;
    }
    
    return status;
}

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      appHandleGaiaOtauClientEvent
 *
 *  DESCRIPTION
 *      This function handles the events from the GAIA OTAU Client library
 *
 *  PARAMETERS
 *      event[in]       The type of event being received
 *      *data[in,out]   Pointer to the event data associated with the event
 *
 *  RETURNS
 *     sys_status: sys_status_success if event handled, or sys_status_failed.
 *----------------------------------------------------------------------------*/
static sys_status appHandleGaiaOtauClientEvent(gaia_otau_client_event event, GAIA_OTAU_CLIENT_EVENT_T *data)
{
    sys_status status = sys_status_success;
    
    switch (event)
    {
        case gaia_otau_client_event_upgrade_starting:
            GaiaOtauInProgress(TRUE);
            break;

        default:
            break;
    }
    
    return status;
}
#endif

/*============================================================================*
 *  Public Function Implemtations
 *============================================================================*/
/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppGaiaOtauInit
 *
 *  DESCRIPTION
 *      This function is used to initialise callbacks and GAIA OTAu related data
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppGaiaOtauInit(void)
{
    GaiaRegisterCallback(appHandleGaiaEvent);
    GaiaOtauRegisterCallback(appHandleGaiaOtauEvent);
#ifdef GAIA_OTAU_RELAY_SUPPORT
    GaiaOtauClientRegisterCallback(appHandleGaiaOtauClientEvent);
#endif
    AppGaiaOtauDataInit();
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppGaiaOtauDataInit
 *
 *  DESCRIPTION
 *      This function is used to initialise GAIA OTAu related data
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppGaiaOtauDataInit(void)
{
   g_app_otau_data.data_type = upgrade_header_upgrade_version;
   g_app_otau_data.commit_cfm = FALSE;
   GaiaOtauUpgradedApplication(FALSE);
#ifdef GAIA_OTAU_RELAY_SUPPORT
   g_app_otau_data.store_type = APP_STORE;
#endif
}
#endif /* GAIA_OTAU_SUPPORT */

