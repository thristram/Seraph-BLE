/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      largeobjecttransfer_model_handler.c
 *
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>
#include <random.h>
#include <mem.h>
#ifndef CSR101x_A05
#include <storage.h>
#include <store_update.h>
#endif /* !CSR101x_A05 */

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "main_app.h"
#include "largeobjecttransfer_server.h"
#include "largeobjecttransfer_client.h"
#include "largeobjecttransfer_model_handler.h"
#include "advertisement_handler.h"
#include "gaia_client_service_event.h"
#ifdef GAIA_OTAU_RELAY_SUPPORT
#include "scan_handler.h"
#endif

#ifdef ENABLE_LOT_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* LOT Announce Message Payload Length */
#define LOT_ANNOUNCE_MESSAGE_PAYLOAD_LENGTH     (10)

/* CSR Bluetooth Company Code */
#define COMPANY_CODE_CSR                        (0x000A)
#define COMPANY_CODE_QUAL                       (0x00D7)

/* LOT Announce ObjectVersion size */
#define LOT_OBJECTVERSION_SIZE                  (3)
/*============================================================================*
 *  Private Data Types
 *===========================================================================*/

/* Upgrade Type */
typedef enum
{
    firmware_upgrade = 0x0000,
} LOT_ANNOUNCE_TYPEENCODING_T;

/* Device Variant */
typedef enum {
   csr101x = 0,
   csr102x
} LOT_ANNOUNCE_PLATFORM_TYPE_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

#ifndef CSR101x_A05

/* Store Information */
store_info_ret_t store_info_appstore1;
store_info_ret_t store_info_appstore2;
store_info_ret_t store_info_userstore3;

/* Store Status */
status_t status_appstore1;
status_t status_appstore2;
status_t status_userstore3;

#endif /* !CSR101x_A05 */

/* LOT Interest Service ID used for advertisments */
static uint8 g_lot_consumer_service_id[16];

/* Announce Payload for SHA 256 */
uint8 g_lot_announce[10];

/* Interest Data */
CSRMESH_LARGEOBJECTTRANSFER_INTEREST_T interest_data;

#ifdef GAIA_OTAU_RELAY_SUPPORT
/* LOT Interest Service ID received  */
static uint8 g_lot_interest_service_id[16];
#endif

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function handles the CSRmesh Large Object Transfer Model messages */
static CSRmeshResult lotModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                            CSRMESH_EVENT_DATA_T* data,
                                            CsrUint16 length,
                                            void **state_data);

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      lotModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Large Object Transfer Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult lotModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                            CSRMESH_EVENT_DATA_T* data,
                                            CsrUint16 length,
                                            void **state_data)
{
    switch(event_code)
    {
        /* handling of Lot Model Event */
        case CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE:
        {
            uint16 g_lot_announce_hash[16],self_dev_id = 0;
            uint8 i,device_variant;
            bool user_store = FALSE;
            CSR_MESH_APP_EVENT_DATA_T get_dev_id_data;
            
            /* Get Self Device ID */
            get_dev_id_data.appCallbackDataPtr = &self_dev_id;
            CSRmeshGetDeviceID(CSR_MESH_DEFAULT_NETID, &get_dev_id_data);
            
            CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE_T *p_announce_data = 
            (CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE_T *)(((CSRMESH_EVENT_DATA_T *)data)->data);    
            
#ifdef CSR101x_A05
            device_variant = csr101x;
#else
            device_variant = csr102x;
#endif               
            /* Check if the node is interested in given payload type */
            if((p_announce_data->typeencoding != firmware_upgrade) || 
               (!(p_announce_data->companycode == COMPANY_CODE_CSR || 
                 p_announce_data->companycode == COMPANY_CODE_QUAL)) ||
               (p_announce_data->platformtype != device_variant) ||
               (p_announce_data->imagetype != IMAGE_TYPE))
            {
#if defined(GAIA_OTAU_RELAY_SUPPORT)
                user_store = TRUE;
#else
                break;
#endif
            }                         
            
            CsrUint16 old_version = (((APP_MAJOR_VERSION & 0x3F) << 10)|
                                      ((APP_MINOR_VERSION & 0x0F) << 6) |
                                      (APP_NEW_VERSION & 0x3F));
            
            if((p_announce_data->objectversion <= old_version) && (!user_store))
                break;           
 
#ifndef CSR101x_A05
            /* Store size available for update */
            uint32 store_size_available;
            
            if(user_store)
            {
#if defined(GAIA_OTAU_RELAY_SUPPORT)
                if(status_userstore3 == sys_status_success)
                    store_size_available = ((store_info_userstore3.size * 2) / 1024);
                else
                    break;
#else
                break;
#endif
            }
            else
            {
                store_id_t cur_app_id = StoreUpdate_GetAppId().id;        
                if(cur_app_id == 1)
                {
                    if(status_appstore2 == sys_status_success)
                        store_size_available = ((store_info_appstore2.size * 2) / 1024);
                    else
                        break;
                 }
                else
                {
                    if(status_appstore1 == sys_status_success)
                        store_size_available = ((store_info_appstore1.size * 2) / 1024);
                    else
                        break;
                }
            }
            
            if(p_announce_data->size >= store_size_available)
                break;
#endif                     
            /* Copy the Random data in Consumer Service ID */
            g_lot_consumer_service_id[8] = self_dev_id & 0xFF;
            g_lot_consumer_service_id[9] = (self_dev_id >> 8) & 0xFF;
                        
            g_lot_announce[0] = (p_announce_data->companycode) & 0xFF;
            g_lot_announce[1] = (p_announce_data->companycode >> 8) & 0xFF;
            g_lot_announce[2] = (p_announce_data->platformtype) & 0xFF;
            g_lot_announce[3] = (p_announce_data->typeencoding) & 0xFF;
            g_lot_announce[4] = (p_announce_data->imagetype) & 0xFF;
            g_lot_announce[5] = (p_announce_data->size) & 0xFF;
            g_lot_announce[6] = (p_announce_data->objectversion) & 0xFF;
            g_lot_announce[7] = (p_announce_data->objectversion >> 8) & 0xFF;
            g_lot_announce[8] = (p_announce_data->targetdestination) & 0xFF;
            g_lot_announce[9] = (p_announce_data->targetdestination >> 8) & 0xFF;
            
            /* Generate hash of Most Significant 64 bits of Consumer Service ID */
            CSRmeshCalculateSHA256Hash(g_lot_announce, 
                                       LOT_ANNOUNCE_MESSAGE_PAYLOAD_LENGTH ,
                                       g_lot_announce_hash);
            
            /* Add the hash value in Consumer Service ID */
            for (i = 0; i < 4; i++)
            {
                uint16 t;
                t = g_lot_announce_hash[11 - i];
                g_lot_consumer_service_id[(i << 1)] = ((t >> 8) & 0xFF);
                g_lot_consumer_service_id[(i << 1) + 1] = (t & 0xFF);
            }
                 
            /* Send Lot Interest Information to Model */
            for (i = 0; i < 4; i++)
            {
                uint16 t;
                t = g_lot_announce_hash[15 - i];
                interest_data.serviceid[(i << 1)] = ((t >> 8) & 0xFF);
                interest_data.serviceid[(i << 1) + 1] = (t & 0xFF);
            }
#ifdef GAIA_OTAU_RELAY_SUPPORT
            interest_data.serviceid[7] = interest_data.serviceid[7] | 0x80;
#else
            interest_data.serviceid[7] = interest_data.serviceid[7] & 0x7F;
#endif            
            if (state_data != NULL)
            {
                *state_data = (void *)&interest_data;
            }

            /* Trigger connectable advertisements with LOT Interest Service ID */
            GattTriggerLOTAdverts(g_lot_consumer_service_id);
        }
        break;        

#ifdef GAIA_OTAU_RELAY_SUPPORT
        case CSRMESH_LARGEOBJECTTRANSFER_INTEREST:
        {
            if(!scanning_ongoing)
            {                                
                /* Generate Least Significant 64 bits of Interest ServiceID */
                g_lot_interest_service_id[8] = (data->src_id) & 0xFF;
                g_lot_interest_service_id[9] = ((data->src_id) >> 8) & 0xFF;
                
                /* Start Scanning */
                StartScanning(g_lot_interest_service_id);
            }            
            
        }
        break;
#endif     
        default:
        break;
    }

    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      LotModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the large object transfer model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void LotModelHandlerInit(CsrUint8 nw_id,
                                  uint16 lot_model_groups[],
                                  CsrUint16 num_groups)
{
   
    /* Initialize LOT Model */
    LargeObjectTransferModelInit(nw_id, 
                                 lot_model_groups,
                                 num_groups,
                                 lotModelEventHandler);
    
    /* Generate Most Significant 48 bits of Consumer ServiceID */
    g_lot_consumer_service_id[10] = 0x06;
    g_lot_consumer_service_id[11] = 0x05;
    g_lot_consumer_service_id[12] = 0x04;
    g_lot_consumer_service_id[13] = 0x03;
    g_lot_consumer_service_id[14] = 0x02;
    g_lot_consumer_service_id[15] = 0x01;
       
    /* Initialize LOT Model Client */
#ifdef GAIA_OTAU_RELAY_SUPPORT
    LargeObjectTransferModelClientInit(lotModelEventHandler);
    /* Generate Most Significant 48 bits of Publisher ServiceID */
    g_lot_interest_service_id[10] = 0x06;
    g_lot_interest_service_id[11] = 0x05;
    g_lot_interest_service_id[12] = 0x04;
    g_lot_interest_service_id[13] = 0x03;
    g_lot_interest_service_id[14] = 0x02;
    g_lot_interest_service_id[15] = 0x01;   
    
#endif
    
#ifndef CSR101x_A05
     /* Store handle */
    handle_t store_handle;
            
    status_appstore1 = Storage_FindStore(1, APP_STORE,&store_handle);
    if(status_appstore1== sys_status_success)
        Storage_GetStoreInfo(store_handle,&store_info_appstore1);
    
    status_appstore2 = Storage_FindStore(2, APP_STORE,&store_handle);
    if(status_appstore2 == sys_status_success)
        Storage_GetStoreInfo(store_handle,&store_info_appstore2);
    
#ifdef GAIA_OTAU_RELAY_SUPPORT
    status_userstore3 = Storage_FindStore(USER_STORE_GAIA_OTAU_ID, USER_STORE,
                                                          &store_handle);
    if(status_userstore3 == sys_status_success)
        Storage_GetStoreInfo(store_handle,&store_info_userstore3);
#endif /* GAIA_OTAU_RELAY_SUPPORT */
#endif /* !CSR101x_A05 */
}

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      LotModelSendAnnounce
 *
 *  DESCRIPTION
 *      The Application function Initilises the large object transfer model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void LotModelSendAnnounce(LARGEOBJECTTRANSFER_ANNOUNCE_T *announce_payload)
{
    CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE_T announce_data;
    uint16 g_lot_announce_hash[16];
    uint8 g_lot_client_announce[10];
    uint8 i;
    
    MemCopy(&announce_data,announce_payload,sizeof(announce_data));
    
    g_lot_client_announce[0] = (announce_data.companycode) & 0xFF;
    g_lot_client_announce[1] = (announce_data.companycode >> 8) & 0xFF;
    g_lot_client_announce[2] = (announce_data.platformtype) & 0xFF;
    g_lot_client_announce[3] = (announce_data.typeencoding) & 0xFF;
    g_lot_client_announce[4] = (announce_data.imagetype) & 0xFF;
    g_lot_client_announce[5] = (announce_data.size) & 0xFF;
    g_lot_client_announce[6] = (announce_data.objectversion) & 0xFF;
    g_lot_client_announce[7] = (announce_data.objectversion >> 8) & 0xFF;
    g_lot_client_announce[8] = (announce_data.targetdestination) & 0xFF;
    g_lot_client_announce[9] = (announce_data.targetdestination >> 8) & 0xFF;
    
    /* Generate hash of Most Significant 64 bits of Consumer Service ID */
    CSRmeshCalculateSHA256Hash(g_lot_client_announce, 
                               LOT_ANNOUNCE_MESSAGE_PAYLOAD_LENGTH ,
                               g_lot_announce_hash);
    
    /* Add the hash value in Interset Service ID */
    for (i = 0; i < 4; i++)
    {
        uint16 t;
        t = g_lot_announce_hash[11 - i];
        g_lot_interest_service_id[(i << 1)] = ((t >> 8) & 0xFF);
        g_lot_interest_service_id[(i << 1) + 1] = (t & 0xFF);
    }
    
    LargeObjectTransferAnnounce(CSR_MESH_DEFAULT_NETID,0,0,&announce_data);   
}
#endif

#endif /* ENABLE_LOT_MODEL */

