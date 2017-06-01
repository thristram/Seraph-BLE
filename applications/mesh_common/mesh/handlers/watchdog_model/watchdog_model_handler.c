/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      watchdog_model_handler.c
 *
 *  DESCRIPTION
 *      This file defines routines for using the watchdog model.
 *
 *****************************************************************************/
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>
#include <random.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "app_debug.h"
#include "nvm_access.h"
#include "watchdog_model_handler.h"
#include "app_mesh_handler.h"
#include "csr_mesh_model_common.h"
#include "app_util.h"
#include "main_app.h"

#ifdef ENABLE_WATCHDOG_MODEL
/*============================================================================*
 *  Private Data Types
 *============================================================================*/
#define MAX_WATCHDOG_RSP_SIZE                     (9)
/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Pointer to watchdog handler data */
static WATCHDOG_HANDLER_DATA_T*                    p_wdog_hdlr_data;

/* Watchdog msg to be used for response of a watchdog message */
static CSRMESH_WATCHDOG_MESSAGE_T                  rsp_wdog_msg;

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      wdogIntervalTimeoutHandler
 *
 *  DESCRIPTION
 *      This function keeps track of interval and sends watchdog message
 *      at every interval.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void wdogIntervalTimeoutHandler(timer_id tid)
{
    CSRMESH_WATCHDOG_MESSAGE_T wdog_msg;

    if (tid == p_wdog_hdlr_data->second_tid)
    {
        /* Increment Seconds Counter. */
        p_wdog_hdlr_data->second_count++;

        /* Start timer for next second. */
        p_wdog_hdlr_data->second_tid = TimerCreate(SECOND, TRUE,
                                                 wdogIntervalTimeoutHandler);

        /* Check if the interval number of seconds elapsed. */
        if (p_wdog_hdlr_data->second_count
                                    >= p_wdog_hdlr_data->interval.interval)
        {
            /* Send Watch dog Message if wdog state is running. */
            if (app_wdog_running == p_wdog_hdlr_data->wdog_state)
            {
                uint8 idx;
                /* Generate random message and random length */
                wdog_msg.randomdata_len = 
                               ((uint16)Random16())%sizeof(wdog_msg.randomdata);
                for (idx = 0; idx < wdog_msg.randomdata_len; idx++)
                {
                    wdog_msg.randomdata[idx] = (Random16()) & 0xFF;
                }

                /* Reset the interval counter. */
                p_wdog_hdlr_data->second_count = 0;

                /* Start Listening if the active after interval is non-zero. */
                if (p_wdog_hdlr_data->interval.activeaftertime)
                {
#ifdef ENABLE_DUTY_CYCLE_CHANGE_SUPPORT
                    /* Start Listening to CSRmesh messages. */
                    EnableHighDutyScanMode(TRUE);
                    DEBUG_STR("WATCHDOG: LISTENING in high scan duty mode\r\n");
#endif
                }

                WatchdogMessage(CSR_MESH_DEFAULT_NETID, 
                                p_wdog_hdlr_data->groups[0], 
                                AppGetCurrentTTL(), 
                                &wdog_msg);
            }
        }
        else if (p_wdog_hdlr_data->second_count == 
                                    p_wdog_hdlr_data->interval.activeaftertime)
        {
            /* If active_after duration number of seconds elapsed
             * and watchdog is running.
             */
            if (app_wdog_running == p_wdog_hdlr_data->wdog_state)
            {
#ifdef ENABLE_DUTY_CYCLE_CHANGE_SUPPORT
                EnableHighDutyScanMode(FALSE); 
#endif
                DEBUG_STR("WATCHDOG: LISTENING in low scan duty mode\r\n");
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      watchdogModelEventHandler
 *
 *  DESCRIPTION
 *      This function handles watchdog model commands.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult watchdogModelEventHandler(CSRMESH_MODEL_EVENT_T event,
                                               CSRMESH_EVENT_DATA_T* msg,
                                               CsrUint16 len,
                                               void **state_data)
{
    bool update_lastetag = FALSE;
    switch (event)
    {
        case CSRMESH_WATCHDOG_SET_INTERVAL:
        {
            CSRMESH_WATCHDOG_SET_INTERVAL_T *p_msg = 
                                  (CSRMESH_WATCHDOG_SET_INTERVAL_T *)msg->data;

            /* Check if interval is not zero and interval is greater
             * than active after duration.
             */
            if ((p_msg->interval > 0) && 
                (p_msg->interval >= p_msg->activeaftertime))
            {
                p_wdog_hdlr_data->interval.interval = p_msg->interval;
                p_wdog_hdlr_data->interval.activeaftertime = 
                                                      p_msg->activeaftertime;

                WriteWatchdogModelDataOntoNVM();

                update_lastetag = TRUE;
            }

            /* Check if state_data pointer is NULL. */
            if (state_data)
            {
                *state_data = (void *)&p_wdog_hdlr_data->interval;
            }
        }
        break;

        case CSRMESH_WATCHDOG_MESSAGE:
        {
            CSRMESH_WATCHDOG_MESSAGE_T *p_msg = 
                                  (CSRMESH_WATCHDOG_MESSAGE_T *)msg->data;

            if(p_msg->rspsize != 0 && p_msg->rspsize <= (MAX_WATCHDOG_RSP_SIZE + 1))
            {
                uint16 idx;

                rsp_wdog_msg.rspsize = 0;
                rsp_wdog_msg.randomdata_len = p_msg->rspsize-1;

                for (idx = 0; idx < rsp_wdog_msg.randomdata_len; idx++)
                {
                    rsp_wdog_msg.randomdata[idx] = (Random16()) & 0xFF;
                }

                /* Check if state_data pointer is NULL. */
                if (state_data)
                {
                    *state_data = (void *)&rsp_wdog_msg;
                }
            }
            else
            {
                if (state_data)
                {
                    *state_data = NULL;
                }
            }
        }
        break;

        default:
        break;
    }

    return update_lastetag;
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      WatchdogModelHandlerInit
 *
 *  DESCRIPTION
 *      This function initializes watchdog model.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WatchdogModelHandlerInit(CsrUint8 nw_id, 
                                     uint16 *groups,
                                     uint16 num_groups)
{
    uint16 index;

    WatchdogModelInit(nw_id, groups, num_groups, watchdogModelEventHandler);

    for(index = 0; index < num_groups; index ++)
    {
        p_wdog_hdlr_data->groups[index] = groups[index];
    }
    WatchdogModelDataInit(NULL);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WatchdogModelDataInit
 *
 *  DESCRIPTION
 *      This function initializes watchdog model data.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WatchdogModelDataInit(WATCHDOG_HANDLER_DATA_T* wdog_handler_data)
{
    if(wdog_handler_data != NULL)
    {
        p_wdog_hdlr_data = wdog_handler_data;
    }

    WatchdogStop();

    p_wdog_hdlr_data->interval.activeaftertime = DEFAULT_WDOG_ACTIVE_AFTER;
    p_wdog_hdlr_data->interval.interval     = DEFAULT_WDOG_INTERVAL;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadWatchdogModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads data from NVM during initialization.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ReadWatchdogModelDataFromNVM(void)
{
    Nvm_Read(&p_wdog_hdlr_data->interval.interval, sizeof(CsrUint16),
                NVM_WDOG_DATA_OFFSET + NVM_OFFSET_WDOG_INTERVAL);

    Nvm_Read(&p_wdog_hdlr_data->interval.activeaftertime, sizeof(CsrUint16), 
                NVM_WDOG_DATA_OFFSET + NVM_OFFSET_WDOG_ACTIVE_AFTER);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteWatchdogModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function write data from NVM during initialization.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WriteWatchdogModelDataOntoNVM(void)
{
    Nvm_Write(&p_wdog_hdlr_data->interval.interval, 
              sizeof(CsrUint16),
              NVM_WDOG_DATA_OFFSET + NVM_OFFSET_WDOG_INTERVAL);

    Nvm_Write(&p_wdog_hdlr_data->interval.activeaftertime,
              sizeof(CsrUint16), 
              NVM_WDOG_DATA_OFFSET + NVM_OFFSET_WDOG_ACTIVE_AFTER);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WatchdogStart
 *
 *  DESCRIPTION
 *      This function starts watchdog. Needs to be called after
 *      association completion.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WatchdogStart(void)
{
    if (app_wdog_stopped == p_wdog_hdlr_data->wdog_state)
    {
        /* Delete timer just to make sure things work properly
         * if start is called multiple times.
         */
        TimerDelete(p_wdog_hdlr_data->second_tid);

        /* Start the interval timer. */
        p_wdog_hdlr_data->second_tid = TimerCreate(SECOND, TRUE,
                                               wdogIntervalTimeoutHandler);
    }

    /* Set State to Running. */
    p_wdog_hdlr_data->wdog_state = app_wdog_running;
    DEBUG_STR("WATCHDOG : RUNNING STATE\r\n");
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WatchdogStop
 *
 *  DESCRIPTION
 *      This function stops watchdog.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WatchdogStop(void)
{
    TimerDelete(p_wdog_hdlr_data->second_tid);
    p_wdog_hdlr_data->second_tid   = TIMER_INVALID;
    p_wdog_hdlr_data->second_count = 0;
    p_wdog_hdlr_data->wdog_state = app_wdog_stopped;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WatchdogPause
 *
 *  DESCRIPTION
 *      This function pauses watchdog.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WatchdogPause(void)
{
    p_wdog_hdlr_data->wdog_state = app_wdog_paused;
#ifdef ENABLE_DUTY_CYCLE_CHANGE_SUPPORT
    EnableHighDutyScanMode(TRUE);
#endif
    DEBUG_STR("WATCHDOG PAUSED : LISTENING in high scan duty mode\r\n");
}

#endif /* ENABLE_WATCHDOG_MODEL */
