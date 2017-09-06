/******************************************************************************
 * Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 * Bluetooth Low Energy CSRmesh 2.1
 * CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_observer.c
 *
 *  DESCRIPTION
 *      This file defines connection manager observer role functionality
 *
 *
 ******************************************************************************/
#if defined (OBSERVER)

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
#include "cm_observer.h"
#include "cm_private.h"
#include "cm_hal.h"

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
          
    /* Return raw advertisement reports */ 
    bool                        rawAdvertReports;

} CM_OBSERVER_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Central data */
static CM_OBSERVER_DATA_T g_cm_observer_data;

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverInit
 *
 *  DESCRIPTION
 *      Initialises the observer module
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMObserverInit(void)
{
    g_cm_observer_data.scan_state = cm_scan_state_idle;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverSetScanState
 *
 *  DESCRIPTION
 *      Sets the Scanning state
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMObserverSetScanState(cm_scan_state new_state)
{

    cm_scan_state old_state = g_cm_observer_data.scan_state;

    if (old_state != new_state)
    {
        CM_SCAN_STATE_IND_T scan_state_ind;

        /* Save the new state */
        g_cm_observer_data.scan_state =  new_state;

        /* Notify the Application about the CM state change */
        scan_state_ind.new_state = new_state;
        scan_state_ind.old_state = old_state;
        CMNotifyEventToApplication(CM_SCAN_STATE_IND,
                                       (CM_EVENT_T *)&scan_state_ind);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverGetScanState
 *
 *  DESCRIPTION
 *      Gets the Scanning state
 *
 *  RETURNS
 *      cm_scan_state: Scanning state
 *
 *---------------------------------------------------------------------------*/

extern cm_scan_state CMObserverGetScanState(void)
{
    return g_cm_observer_data.scan_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverSetScanParams
 *
 *  DESCRIPTION
 *      Sets the scanning parameters
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMObserverSetScanParams(CM_CENTRAL_SCAN_INFO_T
                                             *cm_central_scan_data)
{
    /* Copy the Scan parameters */
    g_cm_observer_data.scan_data.bond
            = cm_central_scan_data->bond;

    g_cm_observer_data.scan_data.security
            = cm_central_scan_data->security;

    g_cm_observer_data.scan_data.scan_interval_us =
            cm_central_scan_data->scan_interval_us;

    g_cm_observer_data.scan_data.scan_window_us =
            cm_central_scan_data->scan_window_us;

    g_cm_observer_data.scan_data.scan_type
            = cm_central_scan_data->scan_type;

    g_cm_observer_data.scan_data.use_whitelist =
            cm_central_scan_data->use_whitelist;

    g_cm_observer_data.scan_data.addr_type
            = cm_central_scan_data->addr_type;

    g_cm_observer_data.scan_data.uuid.uuid_type
            = cm_central_scan_data->uuid.uuid_type;
    g_cm_observer_data.scan_data.uuid.uuid
            = cm_central_scan_data->uuid.uuid;

    return cm_status_success;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverStartScanning
 *
 *  DESCRIPTION
 *      Starts the scanning procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMObserverStartScanning(void)
{
    if(CMObserverGetScanState() == cm_scan_state_scanning)
        return cm_status_success; /* Scanning already in progress */

    /* Set the GAP mode for central */
    HALGapSetCentralMode(g_cm_observer_data.scan_data.bond,
                         g_cm_observer_data.scan_data.security);

    /* Set the Scan Intervals */
    GapSetScanInterval(g_cm_observer_data.scan_data.scan_interval_us,
                          g_cm_observer_data.scan_data.scan_window_us);

    /* Set the scan type as received from the application */
    GapSetScanType(g_cm_observer_data.scan_data.scan_type);


    /* Start scanning */
    LsStartStopScan(TRUE, g_cm_observer_data.scan_data.use_whitelist,
                     g_cm_observer_data.scan_data.addr_type);

    /* Set the connection manager state to scanning */
    CMObserverSetScanState(cm_scan_state_scanning);

    return cm_status_success;
 }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverStartScanningExt
 *
 *  DESCRIPTION
 *      Starts the extended scanning procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMObserverStartScanningExt(bool rawAdvertReports)
{    
    /* raw advertisement reports needed */
    g_cm_observer_data.rawAdvertReports = rawAdvertReports;
    
    return CMObserverStartScanning();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverEnableRawReports
 *
 *  DESCRIPTION
 *     To enable the raw advertising reports flag.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMObserverEnableRawReports(bool rawAdvertReports)
{
    g_cm_observer_data.rawAdvertReports = rawAdvertReports;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverStopScanning
 *
 *  DESCRIPTION
 *      Stops the scanning procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMObserverStopScanning()
{
    if(CMObserverGetScanState() == cm_scan_state_scanning)
    {
        /* Stop scanning */
        LsStartStopScan(FALSE, whitelist_disabled, ls_addr_type_public);

        /* Set the connection manager state to scanning */
        CMObserverSetScanState(cm_scan_state_idle);
        
        g_cm_observer_data.rawAdvertReports = FALSE;

        return cm_status_success;
    }

    return cm_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMObserverHandleProcessLMEvent
 *
 *  DESCRIPTION
 *      Handles the firmware events related to the observer role
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMObserverHandleProcessLMEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
        /* This event is received when the device receives an advertisement
        * packet or a scan response packet during scanning.
        */
        case HAL_ADVERTISING_REPORT_IND:
        {
#ifndef THIN_CM4_MESH_NODE
            CM_ADV_REPORT_IND_T cm_adv_report_ind;
#endif /* THIN_CM4_MESH_NODE */
            h_ls_advertising_report_ind_t *adv_ind =
                    (h_ls_advertising_report_ind_t *) HALGetMsg(msg);
            
            if(g_cm_observer_data.rawAdvertReports)
            {
                /* Send raw advertising report event to the application */
                CMNotifyEventToApplication(CM_RAW_ADV_REPORT_IND,
                                           (CM_EVENT_T *)adv_ind);
                break;
            }
#ifndef THIN_CM4_MESH_NODE
            /* Ignore if it is not advertisement report */
            if(!HALIsAdvertisingReport(adv_ind))
                break;

            /* Parse the advertising report indication */
            HALParseAdvertisingReportInd(&cm_adv_report_ind, adv_ind);

            /* Send advertising report event to the application */
            CMNotifyEventToApplication(CM_ADV_REPORT_IND,
            (CM_EVENT_T *)&cm_adv_report_ind);
#endif /* THIN_CM4_MESH_NODE */
        }
        break;

        default:
        break;
    }
}

#endif /* OBSERVER */
