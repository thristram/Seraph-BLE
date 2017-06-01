/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_util.h
 *
 *
 ******************************************************************************/
#ifndef __APP_UTIL_H__
#define __APP_UTIL_H__

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/* Application defined panic codes 
 * Persistent storage which is used to hold panic code is intialized to zero, 
 * so the application shall not use 0 for panic codes
 */
typedef enum
{
    /* Failure while setting advertisement parameters */
    app_panic_set_advert_params = 1,

    /* Failure while setting advertisement data */
    app_panic_set_advert_data,
    
    /* Failure while setting scan response data */
    app_panic_set_scan_rsp_data,

    /* Failure while establishing connection */
    app_panic_connection_est,

    /* Failure while registering GATT DB with firmware */
    app_panic_db_registration,

    /* Failure while reading NVM */
    app_panic_nvm_read,

    /* Failure while writing NVM */
    app_panic_nvm_write,

    /* Failure while reading Tx Power Level */
    app_panic_read_tx_pwr_level,

    /* Failure while deleting device from whitelist */
    app_panic_delete_whitelist,

    /* Failure while adding device to whitelist */
    app_panic_add_whitelist,

    /* Failure while triggering connection parameter update procedure */
    app_panic_con_param_update,

    /* Event received in an unexpected application state */
    app_panic_invalid_state,

    /* Unexpected beep type */
    app_panic_unexpected_beep_type,

    /* Failure while setting advertisement parameters */
    app_panic_gap_set_mode,

    /* Not supported UUID*/
    app_panic_uuid_not_supported,
    
    /* Failure while setting scan parameters */
    app_panic_set_scan_params,

    /* Failure while erasing NVM */
    app_panic_nvm_erase,

}app_panic_code;

/* Default network id of the application */
#define DEFAULT_NW_ID                  (0)

/* The broadcast id for MESH is defined as 0 */
#define MESH_BROADCAST_ID              (0)

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function calls firmware panic routine and gives a single point 
 * of debugging any application level panics
 */
extern void ReportPanic(app_panic_code panic_code);

#ifdef DEBUG_ENABLE
/* Print a number in decimal. */
extern void PrintInDecimal(uint32 val);
#else
#define PrintInDecimal(n)
#endif /* DEBUG_ENABLE */

#endif /* __APP_UTIL_H__ */
