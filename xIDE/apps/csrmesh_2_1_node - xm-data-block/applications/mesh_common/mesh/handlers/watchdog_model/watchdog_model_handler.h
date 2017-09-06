/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      watchdog_model_handler.h
 *
 *  DESCRIPTION
 *      Header definitions for Watchdog model functionality
 *
 *****************************************************************************/

#ifndef __WATCHDOG_MODEL_HANDLER_H__
#define __WATCHDOG_MODEL_HANDLER_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>
#include <timer.h>
/*============================================================================*
 *  CSRmesh Header Files
 *============================================================================*/
#include <watchdog_server.h>
#include <csr_mesh.h>

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Default Watchdog Model Timing Parameters. */
#define DEFAULT_WDOG_INTERVAL           (5)

#define DEFAULT_WDOG_ACTIVE_AFTER       (1)

#if (DEFAULT_WDOG_INTERVAL < DEFAULT_WDOG_ACTIVE_AFTER)
#error "Watchdog Interval less than Active After duration"
#endif

#if (DEFAULT_WDOG_INTERVAL == 0)
#error "Watchdog Interval can't be zero."
#endif

/* Macros for NVM access */
#define NVM_OFFSET_WDOG_INTERVAL        (0)

#define NVM_OFFSET_WDOG_ACTIVE_AFTER    (1)

/* Watchdog States. */
typedef enum
{
    app_wdog_stopped = 0,
    app_wdog_paused  = 1,
    app_wdog_running = 2
} APP_WDOG_STATE_T;


/* CSRmesh watchdog model data. */
typedef struct
{
    /* Timer ID for interval time. */
    timer_id                    second_tid;

    /* Count to keep track of interval time in seconds. */
    uint32                      second_count;

    /* Watch dog model Inteval data */
    CSRMESH_WATCHDOG_INTERVAL_T interval;

    /* Model group ID list */
    uint16                      groups[MAX_MODEL_GROUPS];

    /* Current State of Watchdog. */
    APP_WDOG_STATE_T            wdog_state;
} WATCHDOG_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* Initialize Watchdog Model Data.Call this API before ReadPersistentStore().
 */

/* Initialize Watchdog Model. Call this API after CsrMeshInit(). */
extern void WatchdogModelHandlerInit(CsrUint8 nw_id, 
                                     uint16 *groups,
                                     uint16 num_groups);

/* Initialize the Watchdog model data */
extern void WatchdogModelDataInit(WATCHDOG_HANDLER_DATA_T* wd_handler_data);

/* Reads Watchdog model Data from NVM. */
extern void ReadWatchdogModelDataFromNVM(void);

/* Write Watchdog model Data from NVM. */
extern void WriteWatchdogModelDataOntoNVM(void);

/* Start Watchdog. */
extern void WatchdogStart(void);

/* Stop Watchdog. */
extern void WatchdogStop(void);

/* Pause Watchdog to Scan Continously. */
extern void WatchdogPause(void);

#endif /* __WATCHDOG_MODEL_HANDLER_H__ */

