/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      connection_handler.c
 *
 *
 ******************************************************************************/
#include "user_config.h"

 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>
#include <mem.h>
#include <hci_event_types.h>
#include <timer.h>
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
#include <csr_ota.h>
#endif
/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "app_debug.h"
#include "app_util.h"
#include "app_gatt_db.h"
#include "advertisement_handler.h"
#include "connection_handler.h"
#include "cm_observer.h"
#include "main_app.h"
#include "app_conn_params.h"
#include "conn_param_update.h"
#include "app_mesh_handler.h"
#if defined(GAIA_OTAU_SUPPORT) || defined(GAIA_OTAU_RELAY_SUPPORT)
#include "gaia_client_service_event.h"
#endif
#ifdef GAIA_OTAU_RELAY_SUPPORT
#include "gaia_client_service.h"
#include "gaia_otau_client_api.h"
#include "scan_handler.h"
#endif
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

typedef struct
{
    /* connected device id  */
    device_handle_id               device_id;

     /* Boolean flag indicates whether the device is temporary paired or not */
    bool                           paired;

   /* Variable to store the current connection interval being used. */
    uint16                         conn_interval;

    /* Variable to store the current slave latency. */
    uint16                         conn_latency;

    /* Variable to store the current connection time-out value. */
    uint16                         conn_timeout;
 
#ifdef GAIA_OTAU_SUPPORT    
    /* Flag to indicate that this is the upgraded application */
    bool                           otau_upgraded_application;
#endif

#if defined(GAIA_OTAU_SUPPORT) || defined(GAIA_OTAU_RELAY_SUPPORT)
        /* Variable to check if Gaia Otau has started */
    bool                           otau_in_progress;
#endif
    
#ifdef GAIA_OTAU_RELAY_SUPPORT
    /* Discovery retry count */
    int16                          disc_retry_count;

    /* Advertisement timer */
    timer_id                        disc_tid;
    
    /* Device ID of the device which needs to be discovered for services */
    device_handle_id               disc_device_id;
    
    /* BD Addresss of the last connected device */
    TYPED_BD_ADDR_T                last_conn_addr;
    
    int16                          conn_retry_interval;
    
    timer_id                       conn_timeout_tid;
#endif
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
    bool                            ota_reset_required;
#endif
}APP_GATT_SERVICE_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/
 
/* Buffer to store unpacked data */
static uint8                unpackedData[MAX_ADV_DATA_LEN];

/* TTL of received message */
static uint8                rx_ttl;

/* Declare space for connection information */
static uint16               g_conn_info[CM_SIZEOF_CONN_INFO * MAX_CONNECTIONS];

/* Declare space for bonded device information */
static  uint16              g_bonded_devices[CM_SIZEOF_BOND_INFO * 
                                           MAX_PAIRED_DEVICES];

/* Declare space for the server services information */
static CM_SERVER_INFO_T     g_server_info[MAX_SERVER_SERVICES];

#ifdef GAIA_OTAU_RELAY_SUPPORT
/* Declare space for client services information */
static CM_CLIENT_INFO_T g_client_info[MAX_CLIENT_SERVICES];
#endif

/* Application GATT data */
static APP_GATT_SERVICE_DATA_T      gatt_data;

/* Mesh Core Stack Scheduler Gatt Event Data */
static CSR_SCHED_GATT_EVENT_DATA_T  gatt_event_data;

/* Current state of application */
static app_state                    state;
/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function is used to initialise conn manager initial parameters */
static void getCmInitParams(CM_INIT_PARAMS_T *init_params, bool nvm_fresh,
                                uint16 *nvm_offset);

/* This function handles the CM Init confirmation */
static void handleCmInitCfm(CM_INIT_CFM_T *cm_event_data);

/* This function handles the connection notification */
static void handleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data);

/* This function handles the bonding notification */
static void handleBondNotify(CM_BONDING_NOTIFY_T *cm_event_data);

/* This function handles the encryption change */
static void handleEncryptionChangeNotify(CM_ENCRYPTION_NOTIFY_T *cm_event_data);

/* This function handles the events from the connection manager */
static void appHandleConnMgrProcEvent(cm_event event_type,
                                      CM_EVENT_T *cm_event_data);

/* This function handles the RAW advertisement report received from the 
 * connection manager
 */
static bool handleCmRawAdvReportInd(CM_RAW_ADV_REPORT_IND_T *report);

#ifdef GAIA_OTAU_RELAY_SUPPORT
static void startGattDiscovery(device_handle_id device_id);
#endif

/* CallBack handler for connection manager events. */
static CM_HANDLERS_T g_cm_app_handler = 
{
    .pCallback = &appHandleConnMgrProcEvent
};

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      getCmInitParams
 *
 *  DESCRIPTION
 *      This function is used to initialise conn manager initial parameters
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void getCmInitParams(CM_INIT_PARAMS_T *init_params, bool nvm_fresh,
                                uint16 *nvm_offset)
{
    /* Application Callback pointer */
    init_params->cm_app_handler        = &g_cm_app_handler;

    /* Connection Information */
    init_params->conn_info              = g_conn_info;
    init_params->max_connections        = MAX_CONNECTIONS;

    /* Bonded Device List */
    init_params->bonded_devices         = g_bonded_devices;
    init_params->max_bonded_devices     = MAX_PAIRED_DEVICES;
    init_params->diversifier            = DIVERSIFIER;    

    /* Client Handlers and Service list */
#ifdef GAIA_OTAU_RELAY_SUPPORT
    init_params->client_info            = g_client_info;
    init_params->max_client_services    = MAX_CLIENT_SERVICES;
#else
    init_params->client_info            = NULL;
    init_params->max_client_services    = 0;
#endif
    
    /* Server Handlers and Service */
    init_params->server_info            = &g_server_info[0];
    init_params->max_server_services    = MAX_SERVER_SERVICES;

    /* NVM start offset */
    init_params->nvm_start_fresh        = nvm_fresh;
    init_params->nvm_offset             = nvm_offset;

    /* Get the gatt database */
    init_params->gatt_db = GattGetDatabase(&init_params->db_length);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCmInitCfm
 *
 *  DESCRIPTION
 *      This function handles the CM Init confirmation
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleCmInitCfm(CM_INIT_CFM_T *cm_event_data)
{
    /* Set the CM API to send raw advertising reports to the application*/
    CMObserverEnableRawReports(TRUE);

    /* Initialise the services supported by the application */
    InitAppSupportedServices();

    /* Initialize the application data structure */
    AppDataInit();

#ifdef USE_STATIC_RANDOM_ADDRESS
    GattSetRandomAddress();
#endif /* USE_STATIC_RANDOM_ADDRESS */

    /* Initialize Mesh */
    AppMeshInit();
    
#ifdef GAIA_OTAU_RELAY_SUPPORT
    /* Send LOT Announce Message */
    SendLOTAnnouncePacket();
#endif

    /* Start advertising */
    AppSetState(app_state_advertising);
}

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      connTimeoutHandler
 *
 *  DESCRIPTION
 *      This function is invoked when the connection timer expires. 
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void connTimeoutHandler(uint16 tid)
{
    if(tid == gatt_data.conn_timeout_tid )
    {
        
            DEBUG_STR("\r\nRetrying Post Transfer Connect");
            AppSendConnectRequest(AppGetLastConnectedAddress());
            gatt_data.conn_retry_interval++;
        }
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnNotify
 *
 *  DESCRIPTION
 *      This function handles the connection notification
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data)
{
    CM_REQ_RADIO_EVENT_T cm_radio_event;
#ifndef DISABLE_BEARER_SETTINGS
    CSR_MESH_TRANSMIT_STATE_T bearer_tx_state;
#endif /* DISABLE_BEARER_SETTINGS */

    if(cm_event_data->result == cm_conn_res_success)
    {
        CM_DEV_CONN_PARAM_T conn_params;

        gatt_data.device_id = cm_event_data->device_id;
#ifdef GAIA_OTAU_RELAY_SUPPORT
        StopScanning(TRUE);
#endif

        /* Get the connection parameters */
        CMGetDevConnParam(cm_event_data->device_id, &conn_params);

        gatt_data.conn_interval =   conn_params.conn_interval;
        gatt_data.conn_latency  =   conn_params.conn_latency;
        gatt_data.conn_timeout  =   conn_params.supervision_timeout;

        /* Inform CSRmesh that we are connected now */
        gatt_event_data.conn_interval = conn_params.conn_interval;
        gatt_event_data.is_gatt_bearer_ready = TRUE;
        gatt_event_data.cid = CMGetConnId(cm_event_data->device_id);

        CSRSchedNotifyGattEvent(CSR_SCHED_GATT_STATE_CHANGE_EVENT, 
                                &gatt_event_data, NULL);

#ifndef CSR101x_A05
        /* The scheduler requires an early wake up event before the connection
         * event to stop advertise and scan. the event is configured 7.5ms 
         * before each connection event */
        CMEnableEarlyWakeup(cm_event_data->device_id, 7500);
#endif

        /* Enable the radio event that indicates the beginning of a 
         * connection packet transmit during connection event.
         */
        cm_radio_event.device_id = cm_event_data->device_id;
        cm_radio_event.event_type = radio_event_connection_event;
        CMConfigureRadioEvent(&cm_radio_event);

#ifndef DISABLE_BEARER_SETTINGS
        /* The device may not have the network key used by the host when 
         * connected as a bridge. If so data received on the mesh control 
         * service.will not be forwarded on mesh.Enable promiscuous mode on both
         * bearers temporarily till the connection exists.
         */
        bearer_tx_state = AppGetStoredBearerState();
        bearer_tx_state.bearerPromiscuous = 
                           LE_BEARER_ACTIVE | GATT_SERVER_BEARER_ACTIVE;

        AppUpdateBearerState(&bearer_tx_state);
#endif /* DISABLE_BEARER_SETTINGS */

        /* Enter connected state */
        AppSetState(app_state_connected);
        
#ifdef GAIA_OTAU_RELAY_SUPPORT
        gatt_data.conn_retry_interval = 0;
        if(CMGetPeerDeviceRole(cm_event_data->device_id) == con_role_peripheral)
        {
            DEBUG_STR("\r\nDiscovery Started");
            
            /* Initialise the maximum retry count */
            gatt_data.disc_retry_count = MAX_DISC_RETRY_COUNT;
        
            /* Start the GATT discovery */
            startGattDiscovery(cm_event_data->device_id);
        }
#endif
        if(CMGetPeerDeviceRole(cm_event_data->device_id) == con_role_central)
        {
            /* Request the connection parameter update */
            AppRequestConnParamUpdate(cm_event_data->device_id);
        }

    }
    else if(cm_event_data->result == cm_disconn_res_success)
    {
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
        if(gatt_data.ota_reset_required)
        {
            OtaReset();
            /* The OtaReset function does not return */
        }
#endif
        /* Reset the connection parameter variables. */
        gatt_data.conn_interval = 0;
        gatt_data.conn_latency = 0;
        gatt_data.conn_timeout = 0;

        CSRSchedNotifyGattEvent(CSR_SCHED_GATT_STATE_CHANGE_EVENT, NULL, NULL);

        /* Disable radio event notifications */
        cm_radio_event.device_id = cm_event_data->device_id;
        cm_radio_event.event_type = radio_event_none;
        CMConfigureRadioEvent(&cm_radio_event);

#ifndef DISABLE_BEARER_SETTINGS
        /* Restore bearer promiscuous state */
        bearer_tx_state = AppGetStoredBearerState();
        AppUpdateBearerState(&bearer_tx_state);
#endif /* DISABLE_BEARER_SETTINGS */
#ifdef GAIA_OTAU_SUPPORT
#ifdef GAIA_OTAU_RELAY_SUPPORT 
          if(GaiaOtauClientGetState() != STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION)
#endif 
        {
            GaiaOtauInProgress(FALSE);
#ifdef GAIA_OTAU_RELAY_SUPPORT 
            scanning_ongoing = FALSE;
#endif
        }
#endif
        /*Handling signal as per current state */
        switch(state)
        {
            case app_state_connected:
            case app_state_disconnecting:
            {
                /* Connection is terminated either due to Link Loss or
                 * the local host terminated connection. In either case
                 * Initialise the app data and go to fast advertising.
                 */
                AppDataInit();
                
#ifdef GAIA_OTAU_RELAY_SUPPORT 
                if(GaiaOtauClientGetState() == 
                   STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION)
                { 
                    SendConnectRequest();  
                }
               /* Send LOT Announce Message */
               SendLOTAnnouncePacket();
#endif
                AppSetState(app_state_advertising);
            }
            break;

            default:
                /* Control should never come here */
                ReportPanic(app_panic_invalid_state);
            break;
        }
    }
#ifdef GAIA_OTAU_RELAY_SUPPORT
    else if(cm_event_data->result == cm_conn_res_cancelled || 
            cm_event_data->result == cm_conn_res_failed)
    {
        if((GaiaOtauClientGetState() == STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION) && 
           (gatt_data.conn_retry_interval < CONN_RETRY_COUNT))
        {
            SendConnectRequest();
        }
        else
        {
            DEBUG_STR("\r\nPost Transfer Connect Failed");
            gatt_data.conn_retry_interval = 0;      
            scanning_ongoing = FALSE;
            CSRmeshStart();
            CSRSchedEnableListening(TRUE);
            gatt_data.conn_timeout_tid = TIMER_INVALID; 
            GaiaOtauClientResetTransferState();
        }
    }
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleBondNotify
 *
 *  DESCRIPTION
 *      This function handles the bonding notification
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleBondNotify(CM_BONDING_NOTIFY_T *cm_event_data)
{
    if(cm_event_data->result == cm_bond_res_success)
    {
       gatt_data.paired = TRUE;
    }
    else
    {
        /* Pairing has failed.disconnect the link.*/
        AppSetState(app_state_disconnecting);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleEncryptionChangeNotify
 *
 *  DESCRIPTION
 *      This function handles the encryption change
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
static void handleEncryptionChangeNotify(CM_ENCRYPTION_NOTIFY_T *cm_event_data)
{
    if(!cm_event_data->status)
    {
        /* Disconnect the link. */
        CMDisconnect(cm_event_data->device_id);
        return;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCmConnectionUpdated
 *
 *  DESCRIPTION
 *      This function handles the signal CM_CONNECTION_UPDATED.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleCmConnectionUpdated(CM_CONNECTION_UPDATED_T* p_event_data)

{
    switch(state)
    {
        case app_state_connected:
        case app_state_disconnecting:
        {
            CM_REQ_RADIO_EVENT_T cm_radio_event;

            /* Store the new connection parameters. */
            gatt_data.conn_interval = p_event_data->conn_interval;
            gatt_data.conn_latency = p_event_data->conn_latency;
            gatt_data.conn_timeout = p_event_data->supervision_timeout;
            gatt_data.device_id = p_event_data->device_id;
            gatt_event_data.conn_interval = p_event_data->conn_interval;
            gatt_event_data.is_gatt_bearer_ready = TRUE;
            gatt_event_data.cid = CMGetConnId(p_event_data->device_id);
            CSRSchedNotifyGattEvent(CSR_SCHED_GATT_STATE_CHANGE_EVENT,
                                    &gatt_event_data, NULL);

            /* Enable the radio event that indicates the beginning of a 
             * connection packet transmit during connection event.
             */
#if defined(GAIA_OTAU_SUPPORT) || defined(GAIA_OTAU_RELAY_SUPPORT)            
            if(!IsGaiaOtauInProgress())
#endif
            {
                cm_radio_event.device_id = gatt_data.device_id;
                cm_radio_event.event_type = radio_event_connection_event;
                CMConfigureRadioEvent(&cm_radio_event);
            }
        }
        break;

        default:
            /* Connection parameter update indication received in unexpected
             * application state.
             */
            ReportPanic(app_panic_invalid_state);
        break;
    }
}

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCmAdvReport
 *
 *  DESCRIPTION
 *      Handles filtered advertisement reports
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleCmAdvReport(CM_ADV_REPORT_IND_T* cm_event_data)
{
    AppSendConnectRequest(cm_event_data->bd_addr);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startDiscoveryTimeout
 *
 *  DESCRIPTION
 *      Handles discovery timer expiry
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void startDiscoveryTimeout(timer_id tid)
{
    if(gatt_data.disc_tid == tid)
    {
        gatt_data.disc_tid = TIMER_INVALID;

        /* Perform service discovery */
        CMClientStartDiscovery(gatt_data.device_id);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startGattDiscovery
 *
 *  DESCRIPTION
 *      This function is used to trigger the gatt discovery
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void startGattDiscovery(device_handle_id device_id)
{
    /* Save the devie id */
    gatt_data.disc_device_id = device_id;

    /* Start discovery timer  */
    TimerDelete(gatt_data.disc_tid);
    gatt_data.disc_tid = TimerCreate(DISCOVERY_TIMER, TRUE,
                                      startDiscoveryTimeout);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleDiscoveryComplete
 *
 *  DESCRIPTION
 *      This function handles the signal CM_DISCOVERY_COMPLETE
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleDiscoveryComplete(CM_DISCOVERY_COMPLETE_T *cm_event_data)
{
    if(cm_event_data->status == cm_status_success)
    {
        DEBUG_STR("\r\nDiscovery Complete");
        
        /* Configure services */
        AppConfigureNextService(cm_event_data->device_id);
    }
    else if(cm_event_data->status != cm_status_mandatory_serv_not_found)
    {
        /* Gatt discovery has failed */

        DEBUG_STR("\r\nDiscovery Failed");
        /* Ignore the event if device is disconnected */
        if(CMGetDevState(gatt_data.disc_device_id) != dev_state_connected)
            return;

        /* Retry the gatt discovery */
        if(gatt_data.disc_retry_count > 0)
        {
            /* discrement the discovery retry count */
            --gatt_data.disc_retry_count;

            /* Start the GATT discovery */
            startGattDiscovery(cm_event_data->device_id);
        }
        else
        {
            CMDisconnect(cm_event_data->device_id);
        }
    }
    else
    {
	    DEBUG_STR("\r\nDiscovery Failed as mandatory service not found");
        CMDisconnect(cm_event_data->device_id);
    }
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleLEAdvMessage
 *
 *  DESCRIPTION
 *      Process a received message from the radio
 *
 * PARAMETERS
 *      None
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static bool handleCmRawAdvReportInd(CM_RAW_ADV_REPORT_IND_T *report)
{
    bool result = FALSE;
    uint16 *advertData;
    uint8 length;
    uint8 index = 0;

#ifndef CSR101x_A05
    HCI_EV_DATA_ULP_ADVERTISING_REPORT_T *data = &(report->report.hdr);
#else
    HCI_EV_DATA_ULP_ADVERTISING_REPORT_T *data = &(report->report.data);
#endif
    /* handle non connectable adv */
    if( data->event_type == ls_advert_non_connectable )
    {
        /* The advert data is supplied to us as a packed uint8 so unpack it
         * locally to allow easier analysis (to find the device name).
         *
         * Get a pointer to the actual data, which is after the control
         * information block and RSSI parameter (the last uint8).
         */
        advertData = (uint16*) data +
                     sizeof(HCI_EV_DATA_ULP_ADVERTISING_REPORT_T) +
                     sizeof(uint8);

        MemCopyUnPack(unpackedData, advertData, data->length_data);

        /* Loop until we find a block with manufacturer specific data */
        do
        {
            if(index + 4 < data->length_data)
            {
                /* Search for CSRmesh UUID data 
                 * Adv packets are of the form:
                 *  |L|T|D....|L|T|D....|...
                 *  L - Length  1 octet. Includes length of T(Type) + D(data)
                 *  T - AD type 1 octet
                 *  D - Data    L minus 1 octets
                 *  For mesh packets, data will be 16 bits UUID followed by mesh
                 *  payload
                 *  Mesh payload size = L minus 3 .
                 */
                if (!MemCmp(&unpackedData[index + 1], mesh_ad_data, 3))
                {
                    /* derive length first */
                    length = unpackedData[index];

                    if(length > MAX_ADV_DATA_LEN || length == 0)
                    {
                        /* We don't have enough data to process */
                        break;
                    }

                    /* Fill in TTL variables */
                    rx_ttl  = unpackedData[index + length];

                    /* Update Bearer Event Data structure with incoming Mesh 
                     * Data.
                     */
                    CSRSchedHandleIncomingData(
                                        CSR_SCHED_INCOMING_LE_MESH_DATA_EVENT,
                                        &unpackedData[index+4], (length-3), 
                                        report->report.rssi);
                    result = TRUE;
                    break;
                }
                else
                {
                    /* Not a mesh data */
                }
                index += 1;
            }
            else
            {
                /* We don't have enough data to process */
                break;
            }
        }while(index < data->length_data);
    }
    else
    {
         /* condition to handle connectable undirected adv, connectable
          * directed adv & scan response
          */
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      appHandleConnMgrProcEvent
 *
 *  DESCRIPTION
 *      This function handles the events from the connection manager to the
 *      application
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void appHandleConnMgrProcEvent(cm_event event_type,
                                           CM_EVENT_T *cm_event_data)
{
    switch(event_type)
    {
        case CM_INIT_CFM:
            handleCmInitCfm((CM_INIT_CFM_T*)cm_event_data);
        break;

        case CM_RAW_ADV_REPORT_IND:
            handleCmRawAdvReportInd((CM_RAW_ADV_REPORT_IND_T*)cm_event_data);
        break;

        case CM_CONNECTION_NOTIFY:
            handleConnNotify((CM_CONNECTION_NOTIFY_T *)cm_event_data);
        break;

        case CM_BONDING_NOTIFY:
            handleBondNotify((CM_BONDING_NOTIFY_T *)cm_event_data);
        break;

        case CM_ENCRYPTION_NOTIFY:
            handleEncryptionChangeNotify((CM_ENCRYPTION_NOTIFY_T*)cm_event_data);
        break;

        case CM_CONNECTION_UPDATED:
            handleCmConnectionUpdated((CM_CONNECTION_UPDATED_T*)cm_event_data);
            ConnectionParamUpdateEvent(event_type, cm_event_data);
        break;
        
        case CM_RADIO_EVENT_IND:
             CSRSchedNotifyGattEvent(CSR_SCHED_GATT_CONNECTION_EVENT,NULL,NULL);
        break;
        
        case CM_CONNECTION_UPDATE_SIGNALLING_IND:   /* FALLTHROUGH */
        case CM_CONNECTION_PARAM_UPDATE_CFM:        /* FALLTHROUGH */
        case CM_CONNECTION_PARAM_UPDATE_IND:        /* FALLTHROUGH */
        case CM_SERVER_ACCESSED:                    /* FALLTHROUGH */
        {
            ConnectionParamUpdateEvent(event_type, cm_event_data);
        }
        break;

#ifndef CSR101x_A05
#ifdef GAIA_OTAU_RELAY_SUPPORT
        case CM_ADV_REPORT_IND:
            handleCmAdvReport((CM_ADV_REPORT_IND_T*)cm_event_data);
            break;
         case CM_DISCOVERY_COMPLETE:

            /* Hanlde the gatt discovery complete indication */
            handleDiscoveryComplete((CM_DISCOVERY_COMPLETE_T*)cm_event_data);
            break;
#endif
        case CM_EARLY_WAKEUP_IND:
            CSRSchedNotifyGattEvent(CSR_SCHED_GATT_EARLY_WAKEUP_EVENT, NULL, NULL);
        break;
#endif

        default:
        break;
    }
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppCMInit
 *
 *  DESCRIPTION
 *      This function initialises the connection manager.
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void AppCMInit(bool nvm_fresh, uint16* nvm_offset)
{
    CM_INIT_PARAMS_T init_params;

    /* Initialise conn manager initial parameters */
    getCmInitParams(&init_params, nvm_fresh, nvm_offset);
    
    /* Initialize the connection manager. Wait for the event CM_INIT_CFM before 
     * accessing the CM procedures.
     */
    CMInit(&init_params);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      InitialiseGattData
 *
 *  DESCRIPTION
 *      This function initialises the GATT Data.
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void InitialiseGattData(void)
{
    /* Reset the device id and bond id */
    gatt_data.device_id = CM_INVALID_DEVICE_ID;

    /* Reset the connection parameter variables. */
    gatt_data.conn_interval = 0;
    gatt_data.conn_latency = 0;
    gatt_data.conn_timeout = 0;

    gatt_data.paired = FALSE;
    
     /* Clear upgrade data */
#ifdef GAIA_OTAU_SUPPORT
    gatt_data.otau_in_progress = FALSE;
#endif
#ifdef GAIA_OTAU_RELAY_SUPPORT 
    scanning_ongoing = FALSE;
    gatt_data.conn_retry_interval = 0;
    TimerDelete(gatt_data.conn_timeout_tid);
    gatt_data.conn_timeout_tid = TIMER_INVALID; 
#endif
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
    gatt_data.ota_reset_required = FALSE;
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetConnectedDeviceId
 *
 *  DESCRIPTION
 *      This function returns the connected device id.
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern uint16 GetConnectedDeviceId(void)
{
    return gatt_data.device_id;
}

 /*----------------------------------------------------------------------------*
 *  NAME
 *      GetDevicePairingStatus
 *
 *  DESCRIPTION
 *      This function returns the pairing status of the connected device
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern bool GetDevicePairingStatus(void)
{
    return gatt_data.paired;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppSetState
 *
 *  DESCRIPTION
 *      This function is used to set the state of the application.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AppSetState(app_state new_state)
{
    /* Check if the new state to be set is not the same as the present state
     * of the application.
     */
    app_state old_state = state;

    if (old_state != new_state)
    {
        /* Handle exiting old state */
        switch (old_state)
        {
            case app_state_disconnecting:
                /* Common things to do whenever application exits
                 * app_state_disconnecting state.
                 */

                /* Initialise the application data and services
                 * data structure while exiting Disconnecting state.
                 */
                AppDataInit();
            break;

            case app_state_advertising:
                /* Common things to do whenever application exits
                 * APP_*_ADVERTISING state.
                 */
                /* Stop on-going advertisements */
                GattStopAdverts();
            break;

            case app_state_connected:
                /* Do nothing here */
            break;

            case app_state_idle:
            {
            }
            break;

            default:
                /* Nothing to do */
            break;
        }

        /* Set new state */
        state = new_state;

        /* Handle entering new state */
        switch (new_state)
        {
            case app_state_advertising:
            {
                GattTriggerConnectableAdverts(NULL);
            }
            break;

            case app_state_idle:
            {
                /* Resume Scan and Non Connectable Advertisement */
            }
            break;

            case app_state_connected:
            {
                DEBUG_STR("\r\nConnected");
            }
            break;

            case app_state_disconnecting:
                CMDisconnect(GetConnectedDeviceId());
            break;

            default:
            break;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppRequestConnParamUpdate
 *
 *  DESCRIPTION
 *      This function triggers the connection parameter update procedure
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void AppRequestConnParamUpdate(device_handle_id device_id)
{
    ble_con_params new_params, new_params1;

#ifdef GAIA_OTAU_SUPPORT
    /* Stop connection parameter update if there was one pending */
    StopConnParamsUpdate();
    
    if (gatt_data.otau_in_progress && !gatt_data.otau_upgraded_application)
    {
        new_params.con_min_interval = OTAU_MIN_CON_INTERVAL;
        new_params.con_max_interval = OTAU_MAX_CON_INTERVAL;
        new_params.con_slave_latency = OTAU_SLAVE_LATENCY;
        new_params.con_super_timeout = OTAU_SUPERVISION_TIMEOUT;
        
        new_params1.con_min_interval = OTAU_APPLE_MIN_CON_INTERVAL;
        new_params1.con_max_interval = OTAU_APPLE_MAX_CON_INTERVAL;
        new_params1.con_slave_latency = OTAU_APPLE_SLAVE_LATENCY;
        new_params1.con_super_timeout = OTAU_APPLE_SUPERVISION_TIMEOUT;
        
        /* Request the new parameters */
        RequestConnParamsUpdateOnce(device_id, &new_params, &new_params1);
    }
    else
#endif
    {
        new_params.con_min_interval = PREFERRED_MIN_CON_INTERVAL;
        new_params.con_max_interval = PREFERRED_MAX_CON_INTERVAL;
        new_params.con_slave_latency = PREFERRED_SLAVE_LATENCY;
        new_params.con_super_timeout = PREFERRED_SUPERVISION_TIMEOUT;
        
        new_params1.con_min_interval = APPLE_MIN_CON_INTERVAL;
        new_params1.con_max_interval = APPLE_MAX_CON_INTERVAL;
        new_params1.con_slave_latency = APPLE_SLAVE_LATENCY;
        new_params1.con_super_timeout = APPLE_SUPERVISION_TIMEOUT;
        
        RequestConnParamsUpdate(device_id, &new_params, &new_params1); 
    }
}

#if defined(GAIA_OTAU_SUPPORT) || defined(GAIA_OTAU_RELAY_SUPPORT)
/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauInProgress
 *
 *  DESCRIPTION
 *      This function sets the value of otau_in_progress flag
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void GaiaOtauInProgress(bool in_progress)
{
    if(in_progress)
    {
        CM_REQ_RADIO_EVENT_T cm_radio_event;
        CSRmeshStop(); 
        CSRSchedEnableListening(FALSE);
#ifndef CSR101x_A05
        /* Disable Early Wakeup events */
        CMEnableEarlyWakeup(GetConnectedDeviceId(), 0);
#endif
        /* Disable radio event notifications */
        cm_radio_event.device_id = GetConnectedDeviceId();
        cm_radio_event.event_type = radio_event_none;
        CMConfigureRadioEvent(&cm_radio_event);
    }
    else
    {
        CSRmeshStart();
        CSRSchedEnableListening(TRUE);
    }
   gatt_data.otau_in_progress = in_progress;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IsGaiaOtauInProgress
 *
 *  DESCRIPTION
 *      This function gets the value of otau_in_progress flag
 *
 *  RETURNS
 *      True/False
 *----------------------------------------------------------------------------*/
extern bool IsGaiaOtauInProgress(void)
{
   return gatt_data.otau_in_progress;
}
#endif

#ifdef GAIA_OTAU_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaOtauUpgradedApplication
 *
 *  DESCRIPTION
 *      This function sets the value of otau_upgraded_application flag
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void GaiaOtauUpgradedApplication(bool upgraded_application)
{
   gatt_data.otau_upgraded_application = upgraded_application;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IsGaiaOtauUpgradedApplication
 *
 *  DESCRIPTION
 *      This function gets the value of otau_upgraded_application flag
 *
 *  RETURNS
 *      True/False
 *----------------------------------------------------------------------------*/
extern bool IsGaiaOtauUpgradedApplication(void)
{
   return gatt_data.otau_upgraded_application;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetConnInterval
 *
 *  DESCRIPTION
 *      Gets the connection interval set during connection
 *
 *  RETURNS
 *      Connection Interval
 *
 *---------------------------------------------------------------------------*/
extern uint16 GetConnInterval(void)
{
    return gatt_data.conn_interval;
}

#endif

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      AppConfigureNextService
 *
 *  DESCRIPTION
 *      This function starts configuring the next service
 *
 *  RETURNS
 *     Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppConfigureNextService(device_handle_id device_id)
{
    bool serviceFound = FALSE;

    if(GaiaClientIsServiceDiscovered(device_id))
    {
         serviceFound = TRUE;

         /* Configure GAIA Service */
         GaiaConfigureService(device_id);
   }

   if(!serviceFound)
   {
        if(CMGetPeerDeviceRole(device_id)
            == con_role_peripheral)
        {
            CMDisconnect(device_id);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      SendConnectRequest
 *
 *  DESCRIPTION
 *      This function sends the connect request after timer expiry
 *
 *  RETURNS
 *     Nothing
 *
 *----------------------------------------------------------------------------*/
extern void SendConnectRequest()
{
    gatt_data.conn_timeout_tid = TimerCreate(CONN_TIMEOUT,
                                             TRUE,
                                             connTimeoutHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppSendConnectRequest
 *
 *  DESCRIPTION
 *      This function sends the connect request
 *
 *  RETURNS
 *     Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppSendConnectRequest(TYPED_BD_ADDR_T conn_addr)
{
    CM_CENTRAL_CONN_INFO_T cm_central_conn_info;
    ble_con_params conn_params;


    /* Check the current state Baud Rate PIO and choose the connection
     * parameter
     */
    conn_params.con_max_interval  = OTAU_CENTRAL_MAX_CON_INTERVAL;
    conn_params.con_min_interval  = OTAU_CENTRAL_MIN_CON_INTERVAL;
    conn_params.con_slave_latency = OTAU_CENTRAL_SLAVE_LATENCY;
    conn_params.con_super_timeout = OTAU_CENTRAL_SUPERVISION_TIMEOUT;


    /* Update the default connection parameters */
    cm_central_conn_info.remote_bd_addr = conn_addr;
    cm_central_conn_info.conn_params = &conn_params;
    cm_central_conn_info.con_min_ce_len = CONN_EVENT_LENGTH_MIN;
    cm_central_conn_info.con_max_ce_len = CONN_EVENT_LENGTH_MAX;
    cm_central_conn_info.con_scan_interval = SCAN_INTERVAL;
    cm_central_conn_info.con_scan_window = SCAN_WINDOW;
    cm_central_conn_info.flags = L2CAP_CONNECTION_MASTER_DIRECTED | \
                   L2CAP_PEER_ADDR_TYPE_PUBLIC;

    /* stop the ongoing scan */
    StopScanning(FALSE);

    /* store the last connected address */
    MemCopy(&gatt_data.last_conn_addr,&conn_addr,sizeof(TYPED_BD_ADDR_T));
    
    /* Send the connection request to CM */
    CMCentralConnect(&cm_central_conn_info);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppGetLastConnectedAddress
 *
 *  DESCRIPTION
 *      This function gets the BD Address of the last connected device
 *
 *  RETURNS
 *     Nothing
 *
 *----------------------------------------------------------------------------*/
extern TYPED_BD_ADDR_T AppGetLastConnectedAddress(void)
{
    return gatt_data.last_conn_addr;
}

#endif

#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
/*----------------------------------------------------------------------------*
 *  NAME
 *      AppOtaReset
 *
 *  DESCRIPTION
 *      This function handles the OTA reset request
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void AppOtaReset(void)
{   
    if(GetConnectedDeviceId() != CM_INVALID_DEVICE_ID)
    {
        /* Record that the GATT database may be different 
         * after the device has reset.
         */
        //GattOnOtaSwitch();

        /*Set the reset the flag */
        gatt_data.ota_reset_required = TRUE;

        /* graceful disconnection */
        CMDisconnect(GetConnectedDeviceId());
    }
}
#endif