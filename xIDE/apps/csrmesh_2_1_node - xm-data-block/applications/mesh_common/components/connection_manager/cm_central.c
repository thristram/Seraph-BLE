/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_central.c
 *
 *  DESCRIPTION
 *      This file defines connection manager central functionality
 *
 *
 ******************************************************************************/
#if defined (CENTRAL)

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <gatt.h>
#include <timer.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"
#include "cm_central.h"
#include "cm_private.h"
#include "cm_hal.h"

/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#define CONN_ATTEMPT_TIMER            (5 * SECOND)
#define SIZEOF_GATT_UUID128_IN_BYTES  (16)
/*============================================================================*
 *  Private Data Type
 *============================================================================*/

/* CM Central data type */
typedef struct
{
    /* Scanning Parameters */
    CM_CENTRAL_SCAN_INFO_T      scan_data;

    /* Scanning state */
    cm_scan_state               scan_state;

    /* Connecting state */
    cm_connect_state            connect_state;

    /* Advertisement timer */
    timer_id                    conn_tid;
    
    /* Return adverts with UUID only */
    bool                        uuidonly;
          
    /* Return raw advertisement reports */ 
    bool                        rawAdvertReports;

} CM_CENTRAL_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Central data */
static CM_CENTRAL_DATA_T g_cm_central_data;

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* Handles connection attempt timer expiry */
static void connTimerHandler(timer_id tid);

/* The function filter the reports based on the interested uuid */
static bool interestedDevice(h_ls_advertising_report_ind_t *adv_ind);

/* Handles the signal GATT_CANCEL_CONNECT_CFM */
static void handleSignalLsCancelConnectCfm(h_gatt_cancel_connect_cfm_t
                                           *p_event_data);

/* Handles the signal GATT_CONNECT_CFM */
static void handleSignalGattConnectCfm(h_gatt_connect_cfm_t *p_event_data);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      connTimerHandler
 *
 *  DESCRIPTION
 *      Handles connection attempt timer expiry
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void connTimerHandler(timer_id tid)
{
    if(g_cm_central_data.conn_tid == tid)
    {
        g_cm_central_data.conn_tid = TIMER_INVALID;
        CMCentralCancelConnect();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      interestedDevice
 *
 *  DESCRIPTION
 *     The function filter the reports based on the interested uuid
 *
 *  RETURNS/MODIFIES
 *      bool: TRUE if bonded timer is found
 *
 *----------------------------------------------------------------------------*/
static bool interestedDevice(h_ls_advertising_report_ind_t *adv_ind)
{
    uint16 data[ADVSCAN_MAX_PAYLOAD] = {0};
    uint16 size = 0;
    uint16 index = 0;
    uint8  num_uuids = 0;
    uint8  num_uuid_found;

    if(g_cm_central_data.scan_data.uuid.uuid_type == GATT_UUID_NONE &&
       !g_cm_central_data.uuidonly)
        return TRUE; /* No Filter required */

    /* Extract service UUIDs from the advertisement */

    if(g_cm_central_data.scan_data.uuid.uuid_type == GATT_UUID16)
    {
        /* GapLsFindAdType returns zero if no data is present for the specified
         * criteria
         */
        size = HALGapLsFindAdType(adv_ind,
                               AD_TYPE_SERVICE_UUID_16BIT_LIST,
                               &data[0],
                               ADVSCAN_MAX_PAYLOAD);

        if(size == 0)
        {
            /* No service UUIDs list found - try single service UUID */
            size = HALGapLsFindAdType(adv_ind,
                                   AD_TYPE_SERVICE_UUID_16BIT,
                                   &data[0],
                                   ADVSCAN_MAX_PAYLOAD);
            if(size == 0)
            {
                /* No service data found at all */
                return FALSE;
            }
        }

        /* Check the service UUIDs */
        for(index = 0; index < (size / 2); index++)
        {
            /* Compare the 16-bit service UUID */
            if(g_cm_central_data.scan_data.uuid.uuid_type == GATT_UUID16
               && data[index] == g_cm_central_data.scan_data.uuid.uuid[0])
            {
                /* Interested service is present */
                return TRUE;
            }
        }
    }
    else
    {
        /* GapLsFindAdType returns zero if no data is present for the specified
         * criteria
         */
        size = HALGapLsFindAdType(adv_ind,
                               AD_TYPE_SERVICE_UUID_128BIT_LIST,
                               &data[0],
                               ADVSCAN_MAX_PAYLOAD);

        if(size == 0)
        {
            /* No service UUIDs list found - try single service UUID */
            size = HALGapLsFindAdType(adv_ind,
                                   AD_TYPE_SERVICE_UUID_128BIT,
                                   &data[0],
                                   ADVSCAN_MAX_PAYLOAD);
            if(size == 0)
            {
                /* No service data found at all */
                return FALSE;
            }
        }

         num_uuids = (size/SIZEOF_GATT_UUID128_IN_BYTES);

         /* Search for the required UUID one by one. */
         for(num_uuid_found = 0;num_uuid_found < num_uuids;num_uuid_found++)
         {
              uint8 uuid_start_index = 0;
              uuid_start_index = num_uuid_found *
                                            (SIZEOF_GATT_UUID128_IN_BYTES / 2);

                 /* Compare if the UUID is same as supported one. Advertisement
                  * data is in LSB first order, compare accordingly.
                  */
                    if(data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[7] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[6] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[5] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[4] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[3] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[2] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[1] &&
                       data[uuid_start_index++] == g_cm_central_data.scan_data.uuid.uuid[0])
                    {
                        /* Interested service is present */
                        return TRUE;
                    }
          }
      }


    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalGattConnectCfm
 *
 *  DESCRIPTION
 *      Handles the signal GATT_CONNECT_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalGattConnectCfm(h_gatt_connect_cfm_t *p_event_data)
{
    if(CMGetConnState() != cm_connect_state_connecting)
        return;

    /* Stop the connection attempt timer */
    if(g_cm_central_data.conn_tid != TIMER_INVALID)
    {
         TimerDelete(g_cm_central_data.conn_tid);
         g_cm_central_data.conn_tid = TIMER_INVALID;
    }

    /* Set the connecting state to idle */
    CMSetConnState(cm_connect_state_idle);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalLsCancelConnectCfm
 *
 *  DESCRIPTION
 *      Handles the GATT_CANCEL_CONNECT_CFM for peripheral
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsCancelConnectCfm(h_gatt_cancel_connect_cfm_t
                                           *p_event_data)
{
    if(CMGetConnState() == cm_connect_state_cancel_connecting)
    {
        CM_CONNECTION_NOTIFY_T cm_connect_notify;

        /* Change the connecting state to idle */
        CMSetConnState(cm_connect_state_idle);

        cm_connect_notify.result = cm_conn_res_cancelled;
        cm_connect_notify.device_id = CM_INVALID_DEVICE_ID;
        cm_connect_notify.reason = 0;

        /* Send connect event to the application */
        CMNotifyEventToApplication(CM_CONNECTION_NOTIFY,
                             (CM_EVENT_T *)&cm_connect_notify);
    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralInit
 *
 *  DESCRIPTION
 *      Initialises the central module
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMCentralInit(void)
{
    g_cm_central_data.scan_state = cm_scan_state_idle;
    g_cm_central_data.connect_state = cm_connect_state_idle;
    g_cm_central_data.conn_tid = TIMER_INVALID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetScanState
 *
 *  DESCRIPTION
 *      Sets the Scanning state
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMSetScanState(cm_scan_state new_state)
{

    cm_scan_state old_state = g_cm_central_data.scan_state;

    if (old_state != new_state)
    {
        CM_SCAN_STATE_IND_T scan_state_ind;

        /* Save the new state */
        g_cm_central_data.scan_state =  new_state;

        /* Notify the Application about the CM state change */
        scan_state_ind.new_state = new_state;
        scan_state_ind.old_state = old_state;
        CMNotifyEventToApplication(CM_SCAN_STATE_IND,
                                       (CM_EVENT_T *)&scan_state_ind);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetScanState
 *
 *  DESCRIPTION
 *      Gets the Scanning state
 *
 *  RETURNS
 *      cm_scan_state: Scanning state
 *
 *---------------------------------------------------------------------------*/

extern cm_scan_state CMGetScanState(void)
{
    return g_cm_central_data.scan_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetConnState
 *
 *  DESCRIPTION
 *      Sets the Connecting state
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMSetConnState(cm_connect_state new_state)
{
    cm_connect_state old_state = g_cm_central_data.connect_state;

    if (old_state != new_state)
    {
        CM_CONNECT_STATE_IND_T connect_state_ind;

        /* Save the new state */
        g_cm_central_data.connect_state =  new_state;

        /* Notify the Application about the CM state change */
        connect_state_ind.new_state = new_state;
        connect_state_ind.old_state = old_state;
        CMNotifyEventToApplication(CM_CONNECT_STATE_IND,
                                       (CM_EVENT_T *)&connect_state_ind);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetConnState
 *
 *  DESCRIPTION
 *      Gets the Connecting state
 *
 *  RETURNS
 *      cm_connect_state: Connecting state
 *
 *---------------------------------------------------------------------------*/

extern cm_connect_state CMGetConnState(void)
{
    return g_cm_central_data.connect_state;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralSetScanParams
 *
 *  DESCRIPTION
 *      Sets the scanning parameters
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMCentralSetScanParams(CM_CENTRAL_SCAN_INFO_T
                                             *cm_central_scan_data)
{
    /* Copy the Scan parameters */
    g_cm_central_data.scan_data.bond
            = cm_central_scan_data->bond;

    g_cm_central_data.scan_data.security
            = cm_central_scan_data->security;

    g_cm_central_data.scan_data.scan_interval_us =
            cm_central_scan_data->scan_interval_us;

    g_cm_central_data.scan_data.scan_window_us =
            cm_central_scan_data->scan_window_us;

    g_cm_central_data.scan_data.scan_type
            = cm_central_scan_data->scan_type;

    g_cm_central_data.scan_data.use_whitelist =
            cm_central_scan_data->use_whitelist;

    g_cm_central_data.scan_data.addr_type
            = cm_central_scan_data->addr_type;

    g_cm_central_data.scan_data.uuid.uuid_type
            = cm_central_scan_data->uuid.uuid_type;
    g_cm_central_data.scan_data.uuid.uuid
            = cm_central_scan_data->uuid.uuid;

    return cm_status_success;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralStartScanning
 *
 *  DESCRIPTION
 *      Starts the scanning procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMCentralStartScanning(void)
{
    if(CMGetScanState() == cm_scan_state_scanning)
        return cm_status_success; /* Scanning already in progress */

    /* Set the GAP mode for central */
    HALGapSetCentralMode(g_cm_central_data.scan_data.bond,
                         g_cm_central_data.scan_data.security);

    /* Set the Scan Intervals */
    GapSetScanInterval(g_cm_central_data.scan_data.scan_interval_us,
                          g_cm_central_data.scan_data.scan_window_us);

    /* Set the scan type as received from the application */
    GapSetScanType(g_cm_central_data.scan_data.scan_type);


    /* Start scanning */
    LsStartStopScan(TRUE, g_cm_central_data.scan_data.use_whitelist,
                     g_cm_central_data.scan_data.addr_type);

    /* Set the connection manager state to scanning */
    CMSetScanState(cm_scan_state_scanning);

    return cm_status_success;
 }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralStartScanningExt
 *
 *  DESCRIPTION
 *      Starts the extended scanning procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMCentralStartScanningExt(bool uuidonly, 
                                                bool rawAdvertReports)
{
    /* uuid only adverts needed */
    g_cm_central_data.uuidonly = uuidonly;
    
    /* raw advertisement reports needed */
    g_cm_central_data.rawAdvertReports = rawAdvertReports;
    
    return CMCentralStartScanning();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralStopScanning
 *
 *  DESCRIPTION
 *      Stops the scanning procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMCentralStopScanning()
{
    if(CMGetScanState() == cm_scan_state_scanning)
    {
        /* Stop scanning */
        LsStartStopScan(FALSE, whitelist_disabled, ls_addr_type_public);

        /* Set the connection manager state to scanning */
        CMSetScanState(cm_scan_state_idle);
        
        g_cm_central_data.uuidonly = FALSE;
        g_cm_central_data.rawAdvertReports = FALSE;

        return cm_status_success;
    }

    return cm_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralConnect
 *
 *  DESCRIPTION
 *      Initiates the connection request
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMCentralConnect(CM_CENTRAL_CONN_INFO_T
                                       *cm_central_conn_info)
{
    /* Check the connecting state */
    if(CMGetConnState() != cm_connect_state_idle)
    {
        return cm_status_busy;
    }

    if(CMGetScanState() == cm_scan_state_scanning)
    {
        /* Stop scanning */
        CMCentralStopScanning();
    }

    /* update connection parameters */
    LsSetNewConnectionParamReq(cm_central_conn_info->conn_params,
                               cm_central_conn_info->con_min_ce_len,
                               cm_central_conn_info->con_max_ce_len,
                               cm_central_conn_info->con_scan_interval,
                               cm_central_conn_info->con_scan_window);

    /* Send connection request */
    GattConnectReq(&(cm_central_conn_info->remote_bd_addr),
                   cm_central_conn_info->flags);

    /* Start connection attempt timer  */
    g_cm_central_data.conn_tid = TimerCreate(
                                    CONN_ATTEMPT_TIMER, TRUE,
                                    connTimerHandler);

    /* Set the connection manager state to connecting */
    CMSetConnState(cm_connect_state_connecting);

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralCancelConnect
 *
 *  DESCRIPTION
 *      Cancels the ongoing connection request
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMCentralCancelConnect(void)
{
    /* Check the CM state */
    if(CMGetConnState() == cm_connect_state_connecting)
    {
        /* Set the state to cancel connecting */
        CMSetConnState(cm_connect_state_cancel_connecting);

        /* Cancel the ongoing the connection request */
        GattCancelConnectReq();

        return cm_status_success;
    }

    return cm_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMConfigureRxTimingReport
 *
 *  DESCRIPTION
 *      Enable or disable reporting to the application various packet timing parameters
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMConfigureRxTimingReport(CM_REQ_RX_TIMING_REPORT_EVENT_T *cm_timing_event)
{
    uint16 cid = CMGetConnId(cm_timing_event->device_id);

    /* Enable or Disable reporting timing parameters */
    LsRxTimingReport(cid,cm_timing_event->mode);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMCentralHandleProcessLMEvent
 *
 *  DESCRIPTION
 *      Handles the firmware events related to the central role
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMCentralHandleProcessLMEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
        /* This event is received when the device receives an advertisement
        * packet or a scan response packet during scanning.
        */
        case HAL_ADVERTISING_REPORT_IND:
        {
            CM_ADV_REPORT_IND_T cm_adv_report_ind;

            if(CMGetScanState() != cm_scan_state_scanning)
                break;

            h_ls_advertising_report_ind_t *adv_ind =
                    (h_ls_advertising_report_ind_t *) HALGetMsg(msg);
            
            if(g_cm_central_data.rawAdvertReports)
            {
                /* Send raw advertising report event to the application */
                CMNotifyEventToApplication(CM_RAW_ADV_REPORT_IND,
                                           (CM_EVENT_T *)adv_ind);
                break;
            }            
            
            /* Ignore the un-intrested adv report */
            if(HALIsAdvertisingReport(adv_ind))
            {               
                if(!interestedDevice(adv_ind))
                    break;
            }
            else
                break;

            /* Parse the advertising report indication */
            HALParseAdvertisingReportInd(&cm_adv_report_ind, adv_ind);

            /* Send advertising report event to the application */
            CMNotifyEventToApplication(CM_ADV_REPORT_IND,
            (CM_EVENT_T *)&cm_adv_report_ind);
        }
        break;

        case HAL_GATT_CONNECT_CFM:
        {
            handleSignalGattConnectCfm((h_gatt_connect_cfm_t*)HALGetMsg(msg));
        }
        break;

        case HAL_GATT_CANCEL_CONNECT_CFM:
        {
            handleSignalLsCancelConnectCfm(
                    (h_gatt_cancel_connect_cfm_t*)HALGetMsg(msg));
        }
        break;

        default:
        break;
    }
}

#endif /* CENTRAL */
