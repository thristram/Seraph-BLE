/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      scan_handler.c
 *
 *  DESCRIPTION
 *      This file defines scanning related functions
 *
 ******************************************************************************/

/*============================================================================*
 *  SDK Header File
 *============================================================================*/
#include <timer.h>
#include <mem.h>
#include <ls_api.h>

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*============================================================================*
 *  Local Header File
 *============================================================================*/
#include "cm_api.h"
#include "cm_types.h"
#include "user_config.h"
#include "scan_handler.h"
#include "csr_mesh.h"
#include "csr_sched.h"
#include "app_debug.h"
#include "gaia_client_service_event.h"

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* UUID array */
uint16  uuid[8];        
uint16 temp_uuid[8];

/*============================================================================*
 *  private Function Prototypes
 *============================================================================*/
/*  This function is invoked when the scan timer expires */
static void scanTimeoutHandler(uint16 tid);

/*============================================================================*
 *  Private Data
 *============================================================================*/
static timer_id scan_timeout_tid = TIMER_INVALID;

/*============================================================================*
 *  private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      scanTimeoutHandler
 *
 *  DESCRIPTION
 *      This function is invoked when the scan timer expires. The function would
 *      display the list of scanned devices on the UART and requests the user
 *      to connect to a scanned device. If the required device is not found
 *      in the scan then the user can select the option to rescan.
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void scanTimeoutHandler(uint16 tid)
{
    if(tid == scan_timeout_tid )
    {
        DEBUG_STR("\r\nScanning Timed Out");
        
        scan_timeout_tid = TIMER_INVALID;

        /* Stop scanning */
        CMCentralStopScanning();
        
        scanning_ongoing = FALSE;
        CSRmeshStart();
        CSRSchedEnableListening(TRUE);
        
    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      StartScanning
 *
 *  DESCRIPTION
 *      This function starts scanning
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void StartScanning(uint8* lot_service_id)
{
    CM_CENTRAL_SCAN_INFO_T  scan_info;

    TimerDelete(scan_timeout_tid);
    scan_timeout_tid = TIMER_INVALID;      
    /* Stop the ongoing scanning */
    StopScanning(FALSE); 
    LsStartStopScan(FALSE, whitelist_disabled, ls_addr_type_public);
    
    DEBUG_STR("\r\nScanning for LOT Adverts");
    
    /* Stop Mesh */
    CSRmeshStop();

    scan_info.addr_type             = ls_addr_type_public;
    scan_info.bond                  = gap_mode_bond_no;
    scan_info.security              = GAP_MODE_SECURITY;
    scan_info.scan_interval_us      = SCAN_INTERVAL;
    scan_info.scan_window_us        = SCAN_WINDOW;
    scan_info.scan_type             = ls_scan_type_active;
    scan_info.use_whitelist         = whitelist_disabled;
    scan_info.uuid.uuid_type        = GATT_UUID128;

    MemCopyPack(uuid,lot_service_id,16);

    temp_uuid[0] = uuid[7];
    temp_uuid[1] = uuid[6];
    temp_uuid[2] = uuid[5];
    temp_uuid[3] = uuid[4];
    temp_uuid[4] = uuid[3];
    temp_uuid[5] = uuid[2];
    temp_uuid[6] = uuid[1];
    temp_uuid[7] = uuid[0];
    
    scan_info.uuid.uuid = temp_uuid;

    /* Set the scan parameters */
    CMCentralSetScanParams(&scan_info);

    /* Start scanning */
    CMCentralStartScanningExt(TRUE,FALSE);

    /* Set the scanning flag */
    scanning_ongoing = TRUE;     
    
    scan_timeout_tid = TimerCreate(SCAN_TIMEOUT,
                                              TRUE,
                                              scanTimeoutHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      StopScanning
 *
 *  DESCRIPTION
 *      This function stops scanning
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void StopScanning(bool del_timer)
{  
    if(del_timer)
    {
        TimerDelete(scan_timeout_tid);
        scan_timeout_tid = TIMER_INVALID;          
    }
    else
    {
        /* Stop scanning */
        CMCentralStopScanning();        
    }
}
#endif