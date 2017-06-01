/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      time_model_handler.c
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>
#include <mem.h>
#if defined (CSR101x_A05)
#include <config_store.h>
#endif

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "nvm_access.h"
#include "app_util.h"
#include "time_server.h"
#include "app_mesh_handler.h"
#include "time_model_handler.h"
#include "time_server.h"
#include "time_client.h"
#include "main_app.h"

#ifdef ENABLE_TIME_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
typedef enum
{
    time_state_master = 0,
    time_state_init,
    time_state_relay,
    time_state_no_relay,
    time_state_relay_master
} time_state;

#define MAX_CLOCK_SKEW                          (250)
#define MAX_32_BIT_VALUE                        (0xFFFFFFFF)
#define TICK_INTERVAL_MS                        (50)
#define TIME_BROADCAST_TTL                      (0)
#define RELAY_REPEATS                           (3)
#define REPEAT_DELAY_MS                         (50)
#define MASTER_REPEATS                          (5)
#define MAX_TIME_ZONE_VALUE                     (48)
#define MIN_TIME_ZONE_VALUE                     (-48)
#define TIME_LEN_IN_BYTES                       (6)

/*============================================================================*
 *  Private Data
 *============================================================================*/
 
/* Pointer to model handler Data */
static TIME_HANDLER_DATA_T* p_time_model_hdlr_data;

/* Timestamp taken for caleculation */
static uint32 last_updated_tstamp;

/* Timer for time stamp correction */
static timer_id tick_timer_tid = TIMER_INVALID;

/* Time state */
static time_state g_time_state;

/* Value to drive all timers */
static uint32   elapsed_ms;

/* Broadcast repeats counter */
static uint16   broadcast_repeats;

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      compTime48ForSkewDrift
 *
 *  DESCRIPTION
 *      This function compares the time1 array with the time2 array and returns
 *      TRUE if there is a clock drift of 250ms otherwise returns FALSE.
 *
 *  RETURNS/MODIFIES
 *      returns TRUE if time1 and time2 array would differ by more than 250ms.
 *      returns FALSE if time1 and time2 array would not differ by 250ms.
 *
 *----------------------------------------------------------------------------*/
static bool compTime48ForSkewDrift(uint8 time1[], uint8 time2[])
{
    uint16 t1[3], t2[3], t3[3];

    MemCopyPack(t1, time1, TIME_LEN_IN_BYTES);
    MemCopyPack(t2, time2, TIME_LEN_IN_BYTES);

    if(TimeCmp48LT(t2, t1))
    {
       TimeSub48(t3, t1, t2);
    }
    else
    {
       TimeSub48(t3, t2, t1);
    }

    if(t3[0] > 250 || t3[1] > 0 || t3[2] > 0)
    {
        return TRUE;
    }
    return FALSE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      checkForClockCorrection
 *
 *  DESCRIPTION
 *      This function checks whether the clock skew has exceeded more than 250ms
 *
 *  RETURNS/MODIFIES
 *      returns TRUE if clock skew has exceeded 250ms otherwise returns FALSE.
 *
 *----------------------------------------------------------------------------*/
static bool checkForClockCorrection(uint8 recvd_curr_time[])
{
    uint8 current_time[TIME_LEN_IN_BYTES];
    CsrInt8 timezone;

    TimeModelGetUTC(current_time, &timezone);

    if(compTime48ForSkewDrift(current_time, recvd_curr_time))
    {
        return TRUE;
    }
    return FALSE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      sendTimeBroadcast
 *
 *  DESCRIPTION
 *      This function sends the Time Boradcast onto the n/w.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void sendTimeBroadcast(bool new_update)
{
    CSRMESH_TIME_BROADCAST_T time_bcast;

    MemCopyUnPack(time_bcast.currenttime,  p_time_model_hdlr_data->currenttime,
                  TIME_LEN_IN_BYTES);

    time_bcast.masterclock = (g_time_state == time_state_master);
    time_bcast.timezone = p_time_model_hdlr_data->timezone;

    /* Send the Time Broadcast message */
    TimeBroadcast(DEFAULT_NW_ID, MESH_BROADCAST_ID, TIME_BROADCAST_TTL, 
                  &time_bcast);

    /* As the time has changed send an update to the application */
    if(new_update == TRUE)
    {
        AppHandleCurrentTimeUpdate(time_bcast.currenttime);
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      tickTimerHandler
 *
 *  DESCRIPTION
 *      This function handles reference time stamp timer time-out.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void tickTimerHandler(timer_id tid)
{
    if (tick_timer_tid == tid)
    {
        /* start a new timer for the next update */
        tick_timer_tid =  TimerCreate(TICK_INTERVAL_MS * MILLISECOND, 
                                      TRUE, tickTimerHandler);

        /* Update the current time by adding the elapsed time */
        TimeIncrement48(p_time_model_hdlr_data->currenttime, TICK_INTERVAL_MS);

        /* Elapsed MS is used to drive all the slower timers */
        elapsed_ms += TICK_INTERVAL_MS;

        /* update the last updated time stamp */
        last_updated_tstamp = TimeGet32();
        
        /* start broadcasting at the broadcast interval if you are a master */
        if(g_time_state == time_state_master)
        {
            if(elapsed_ms >= 
                (p_time_model_hdlr_data->time_model.interval*1000UL))
            {
                broadcast_repeats = MASTER_REPEATS;
                elapsed_ms = 0;
            }
        }
        else
        {
            /* Check for relay timer */
            if (g_time_state == time_state_no_relay || 
                g_time_state == time_state_relay_master)
            {
                if(elapsed_ms >= 
                  ((p_time_model_hdlr_data->time_model.interval*1000UL)/4))
                {
                    /* We relay received broadcasts again */
                    g_time_state = time_state_relay;
                    elapsed_ms = 0;
                }
            }
        }

        /* Check for broadcast repeats */
        if (broadcast_repeats)
        { 
            if (elapsed_ms % REPEAT_DELAY_MS == 0) 
            {
                /* It's time for a repeat */
                -- broadcast_repeats;
                sendTimeBroadcast(FALSE);
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      timeModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Time Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult timeModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                           CSRMESH_EVENT_DATA_T* data,
                                           CsrUint16 length,
                                           void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_TIME_SET_STATE:
        {
            CSRMESH_TIME_SET_STATE_T *p_event = 
                                    (CSRMESH_TIME_SET_STATE_T *)data->data;

            p_time_model_hdlr_data->time_model.interval = p_event->interval;
            p_time_model_hdlr_data->time_model.tid = p_event->tid;
            WriteTimeModelDataOntoNVM(NVM_OFFSET_TIME_INTERVAL);

            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_time_model_hdlr_data->time_model;
            }
        }
        break;

        case CSRMESH_TIME_GET_STATE:
        {
            CSRMESH_TIME_GET_STATE_T *p_event = 
                                    (CSRMESH_TIME_GET_STATE_T *)data->data;

            p_time_model_hdlr_data->time_model.tid = p_event->tid;
            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_time_model_hdlr_data->time_model;
            }
        }
        break;

        case CSRMESH_TIME_BROADCAST:
        {
            CSRMESH_TIME_BROADCAST_T *p_event = 
                                    (CSRMESH_TIME_BROADCAST_T *)data->data;
            bool update_n_relay = FALSE;

            if(g_time_state != time_state_master && 
               g_time_state != time_state_no_relay &&
               p_event->timezone <= MAX_TIME_ZONE_VALUE &&
               p_event->timezone >= MIN_TIME_ZONE_VALUE &&
               data->rx_ttl == 0)
            {
                if(g_time_state == time_state_init)
                {
                    /* update the new time in globals and then relay the msg */
                    update_n_relay = TRUE;

                    /* Update the state based on the msg received from master
                     * clock.
                     */
                    if(p_event->masterclock == 1)
                    {
                        g_time_state = time_state_no_relay;
                    }
                    else
                    {
                        g_time_state = time_state_relay_master;
                    }

                    /* Start the tick timer */
                    TimerDelete(tick_timer_tid);
                    tick_timer_tid = 
                        TimerCreate(TICK_INTERVAL_MS * MILLISECOND,
                                    TRUE, tickTimerHandler);
                }
                else if(g_time_state == time_state_relay)
                {
                    /* Update the state based on the msg received from master
                     * clock.
                     */
                    if(p_event->masterclock == 0)
                    {
                        update_n_relay = TRUE;
                        g_time_state = time_state_relay_master;
                    }
                    else
                    {
                        if(checkForClockCorrection(p_event->currenttime))
                        {
                            /* update the new time in globals and then relay 
                             * the msg 
                             */
                            update_n_relay = TRUE;
                        }
                        else
                        {
                            /* Dont Update just relay this message */
                            sendTimeBroadcast(TRUE);
                            elapsed_ms = 0;
                            broadcast_repeats = RELAY_REPEATS;
                        }
                        g_time_state = time_state_no_relay;
                    }
                }
                else if(g_time_state == time_state_relay_master)
                {
                    if(p_event->masterclock == 1)
                    {
                        /* update the new time in globals and then relay the 
                         * msg
                         */
                        update_n_relay = TRUE;
                        g_time_state = time_state_no_relay;
                    }
                }

                if(update_n_relay == TRUE)
                {
                    /* update the new time in globals and then relay the msg */
                    MemCopyPack(p_time_model_hdlr_data->currenttime, 
                                p_event->currenttime,
                                TIME_LEN_IN_BYTES);
                    p_time_model_hdlr_data->timezone = p_event->timezone;
                    sendTimeBroadcast(TRUE);
                    elapsed_ms = 0;
                    broadcast_repeats = RELAY_REPEATS;
                }
            }
        }
        break;


        default:
        break;
    }

    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      TimeModelGetUTC
 *
 *  DESCRIPTION
 *      This function is called to get the current UTC time from the time model.
 *      The current UTC time and the timezone are assigned in the model.
 *
 *  RETURNS/MODIFIES
 *      returns TRUE if the time is synchronized with UTC time otherwise returns
 *      FALSE.
 *
 *----------------------------------------------------------------------------*/
extern bool TimeModelGetUTC(uint8 current_time[], CsrInt8 *timezone)
{
    uint32 curr_tstamp, elapsed_time_ms;

    if(g_time_state != time_state_init)
    {
        curr_tstamp = TimeGet32();

        /* Check for roundoff */
        if(curr_tstamp > last_updated_tstamp)
        {
            elapsed_time_ms = (curr_tstamp - last_updated_tstamp)/1000;
        }
        else
        {
            elapsed_time_ms = (curr_tstamp + 
                           (MAX_32_BIT_VALUE - last_updated_tstamp))/1000;
        }

        /* Send the updated current time by adding the elapsed time */
        TimeIncrement48(p_time_model_hdlr_data->currenttime, elapsed_time_ms);
        MemCopyUnPack(current_time, p_time_model_hdlr_data->currenttime,
                      TIME_LEN_IN_BYTES);

        *timezone = p_time_model_hdlr_data->timezone;
        return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      TimeModelSetUTC
 *
 *  DESCRIPTION
 *      This function should be called to set the UTC time from the app.
 *
 *  RETURNS
 *      Returns TRUE if the new time passed is set otherwise returns FALSE.
 *
 *---------------------------------------------------------------------------*/
extern bool TimeModelSetUTC(uint8 utc_time[], CsrInt8 timezone)
{
    if(timezone <= MAX_TIME_ZONE_VALUE && timezone >= MIN_TIME_ZONE_VALUE)
    {
        /* Update the UTC time received */
        MemCopyPack(p_time_model_hdlr_data->currenttime, 
                    utc_time,
                    TIME_LEN_IN_BYTES);
        p_time_model_hdlr_data->timezone = timezone;

        /* set state to master */
        g_time_state = time_state_master;

        /* Broadcast here and then repeat the same for five times. */
        broadcast_repeats = MASTER_REPEATS;
        elapsed_ms = 0;
        sendTimeBroadcast(TRUE);

        /* Start the tick timer */
        TimerDelete(tick_timer_tid);
        tick_timer_tid = TimerCreate(TICK_INTERVAL_MS * MILLISECOND,
                                     TRUE, tickTimerHandler);
        return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadSensorDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads time model data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void ReadTimeModelDataFromNVM(uint16 offset)
{
    Nvm_Read((uint16*)(&p_time_model_hdlr_data->time_model.interval), 
             sizeof(uint16),
             offset);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteTimeModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes time model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void WriteTimeModelDataOntoNVM(uint16 offset)
{
    Nvm_Write((uint16*)(&p_time_model_hdlr_data->time_model.interval), 
              sizeof(uint16),
              offset);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      TimeModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Time model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void TimeModelHandlerInit(uint8 nw_id,
                                 uint16 model_groups[],
                                 CsrUint16 num_groups)
{
    /* Initialize Time Model */
    TimeModelInit(nw_id, 
                  model_groups,
                  num_groups,
                  timeModelEventHandler);

    TimeModelClientInit(timeModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      TimeModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Time Model data on the global 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void TimeModelDataInit(TIME_HANDLER_DATA_T* time_handler_data)
{
    if(time_handler_data != NULL)
    {
        p_time_model_hdlr_data = time_handler_data;
    }

    p_time_model_hdlr_data->time_model.interval = 60;
    p_time_model_hdlr_data->timezone = 0;
    MemSet(&p_time_model_hdlr_data->currenttime, 0, 
            sizeof(p_time_model_hdlr_data->currenttime));

    last_updated_tstamp = 0;
    g_time_state = time_state_init;
    elapsed_ms = 0;
    broadcast_repeats = 0;

    TimerDelete(tick_timer_tid);
    tick_timer_tid = TIMER_INVALID;
}

#endif /* ENABLE_TIME_MODEL */

