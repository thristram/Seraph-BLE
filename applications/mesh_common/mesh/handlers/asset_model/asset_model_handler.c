/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      asset_model_handler.c
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
#include "asset_server.h"
#include "app_mesh_handler.h"
#include "asset_model_handler.h"
#include "asset_server.h"
#include "asset_client.h"
#include "main_app.h"

#ifdef ENABLE_ASSET_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* How many times to announce an asset */
#define DEFAULT_ANNOUNCE_COUNT                          (40)

/* Interval in milliseconds between announcements */
#define DEFAULT_ANNOUNCE_INTERVAL                       (50L)

/* Time between asset announcements seconds */
#define DEFAULT_BCAST_INTERVAL                          (15)

/* Default Tx Power */
#define DEFAULT_TX_POWER                                (-18)

/* TTL to be used for asset announce */
#define ASSET_ANNOUNCE_TTL                              (0)

/* The Max timer value supported by the device in seconds */
#define MAX_TIMER_VAL_SUPP_IN_SECONDS                   (2100)

/* Macros for NVM access */
#define NVM_OFFSET_ASSET_INTERVAL                       (0)
#define NVM_OFFSET_ASSET_SIDEEFFECTS                    (1)
#define NVM_OFFSET_ASSET_DEST_ID                        (2)
#define NVM_OFFSET_ASSET_ANNOUNCE_INTERVAL_AND_NO       (3)
#define NVM_OFFSET_ASSET_TX_POWER                       (4)

/* Application Model Handler Data Structure */
typedef struct
{
    /* Asset model state data */
    CSRMESH_ASSET_STATE_T         asset_state;
    CsrUint16                     time_wrap_cnt;
    CsrUint16                     time_remaining;
}ASSET_HANDLER_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/
 
/* Pointer to model handler Data */
static ASSET_HANDLER_DATA_T asset_model_hdlr_data;

/* Timer for sending asset broadcast messages */
static timer_id asset_bcast_timer_tid = TIMER_INVALID;

/* Broadcast repeats counter */
static uint16 asset_bcast_repeats;

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      assetBcastTimerHandler
 *
 *  DESCRIPTION
 *      This function handles the expiry of the Asset Bcast timer handler
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void assetBcastTimerHandler(timer_id tid)
{
    if (asset_bcast_timer_tid == tid)
    {
        uint16 time_interval;
        /* If the broadcast repeats have to be done  */
        if(asset_bcast_repeats > 0)
        {
            CSRMESH_ASSET_ANNOUNCE_T asset_announce;
            time_interval = asset_model_hdlr_data.asset_state.announceinterval;
            asset_bcast_repeats --;
            /* This can happen when the asset announce repeats can have value 1*/
            if(time_interval == 0)
            {
                time_interval = DEFAULT_ANNOUNCE_INTERVAL;
            }
            asset_announce.interval = asset_model_hdlr_data.asset_state.interval;
            asset_announce.sideeffects = asset_model_hdlr_data.asset_state.sideeffects;
            asset_announce.txpower = asset_model_hdlr_data.asset_state.txpower;

            AssetAnnounce(DEFAULT_NW_ID, 
                          asset_model_hdlr_data.asset_state.todestinationid,
                          ASSET_ANNOUNCE_TTL,
                          &asset_announce);

            asset_bcast_timer_tid = TimerCreate(time_interval * MILLISECOND,
                                                TRUE, assetBcastTimerHandler);
        }
        else if(asset_model_hdlr_data.time_wrap_cnt > 0)
        {
            if(asset_model_hdlr_data.time_wrap_cnt > 0)
            {
                asset_model_hdlr_data.time_wrap_cnt--;
                time_interval = MAX_TIMER_VAL_SUPP_IN_SECONDS;
            }
            else
            {
                time_interval = asset_model_hdlr_data.time_remaining;
                asset_model_hdlr_data.time_remaining = 0;
                asset_bcast_repeats = asset_model_hdlr_data.asset_state.numannounces;
            }
            asset_bcast_timer_tid = TimerCreate(time_interval * SECOND,
                                                TRUE, assetBcastTimerHandler);
        }
        else if(asset_model_hdlr_data.asset_state.interval > 0)
        {
            time_interval = asset_model_hdlr_data.asset_state.interval;

            /* Start the timer in parts if timer is more than 35 minutes */
            if(time_interval > MAX_TIMER_VAL_SUPP_IN_SECONDS)
            {
                asset_model_hdlr_data.time_wrap_cnt = 
                                         time_interval / MAX_TIMER_VAL_SUPP_IN_SECONDS;
                asset_model_hdlr_data.time_remaining = 
                                         time_interval % MAX_TIMER_VAL_SUPP_IN_SECONDS;
                /* Decrement the time wrap cnt as we are starting a timer here for 
                 * time wrap seconds.
                 */
                asset_model_hdlr_data.time_wrap_cnt --;
                time_interval = MAX_TIMER_VAL_SUPP_IN_SECONDS;
            }
            else
            {
                asset_model_hdlr_data.time_wrap_cnt = 0;
                asset_model_hdlr_data.time_remaining = 0;
                asset_bcast_repeats = asset_model_hdlr_data.asset_state.numannounces;
            }

            asset_bcast_timer_tid = TimerCreate(time_interval * SECOND,
                                                TRUE, assetBcastTimerHandler);
        }
    }

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      assetModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Asset Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult assetModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                           CSRMESH_EVENT_DATA_T* data,
                                           CsrUint16 length,
                                           void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_ASSET_SET_STATE:
        {
            CSRMESH_ASSET_SET_STATE_T *p_event = 
                                    (CSRMESH_ASSET_SET_STATE_T *)data->data;

            asset_model_hdlr_data.asset_state.interval = p_event->interval;
            asset_model_hdlr_data.asset_state.sideeffects = p_event->sideeffects;
            asset_model_hdlr_data.asset_state.todestinationid = p_event->todestinationid;
            asset_model_hdlr_data.asset_state.txpower = p_event->txpower;
            asset_model_hdlr_data.asset_state.numannounces = p_event->numannounces;
            asset_model_hdlr_data.asset_state.announceinterval = p_event->announceinterval;
            WriteAssetModelDataOntoNVM(NVM_OFFSET_ASSET_MODEL_DATA);

            /* Set the received Tx power through onto mesh stack. */
            AppSetTxPower(asset_model_hdlr_data.asset_state.txpower);

            /* Start the asset broadcast with the new values set */
            AssetStartBroadcast();

            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&asset_model_hdlr_data.asset_state;
            }
        }
        break;

        case CSRMESH_ASSET_GET_STATE:
        {
            CSRMESH_ASSET_GET_STATE_T *p_event = 
                                    (CSRMESH_ASSET_GET_STATE_T *)data->data;

            asset_model_hdlr_data.asset_state.tid = p_event->tid;
            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&asset_model_hdlr_data.asset_state;
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

/*----------------------------------------------------------------------------*
 *  NAME
 *      AssetStartBroadcast
 *
 *  DESCRIPTION
 *      This function starts the asset braodcast.
 *
 *  RETURNS
 *      None
 *
 *---------------------------------------------------------------------------*/
extern void AssetStartBroadcast(void)
{
    /* Start the Asset Broadcast */
    TimerDelete(asset_bcast_timer_tid);
    asset_bcast_timer_tid = TIMER_INVALID;
    if(asset_model_hdlr_data.asset_state.interval > 0)
    {
        asset_bcast_repeats = asset_model_hdlr_data.asset_state.numannounces;
        asset_bcast_timer_tid = TimerCreate(asset_model_hdlr_data.asset_state.announceinterval * MILLISECOND,
                                            TRUE, assetBcastTimerHandler);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadAssetModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads asset model data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void ReadAssetModelDataFromNVM(uint16 offset)
{
    uint16 temp;

    Nvm_Read((uint16 *)&asset_model_hdlr_data.asset_state.interval,
             sizeof(uint16),
             offset + NVM_OFFSET_ASSET_INTERVAL);

    Nvm_Read((uint16 *)&asset_model_hdlr_data.asset_state.sideeffects,
             sizeof(uint16),
             offset + NVM_OFFSET_ASSET_SIDEEFFECTS);

    Nvm_Read((uint16 *)&asset_model_hdlr_data.asset_state.todestinationid,
             sizeof(uint16),
             offset + NVM_OFFSET_ASSET_DEST_ID);

    /* Read action id and mcp packet len from NVM */
    Nvm_Read((uint16 *)&temp, sizeof(uint16), 
             offset + NVM_OFFSET_ASSET_ANNOUNCE_INTERVAL_AND_NO);

    asset_model_hdlr_data.asset_state.announceinterval = temp & 0xFF;
    temp >>= 8;
    asset_model_hdlr_data.asset_state.numannounces = temp & 0xFF;

    Nvm_Read((uint16 *)&asset_model_hdlr_data.asset_state.txpower,
             sizeof(uint16),
             offset + NVM_OFFSET_ASSET_TX_POWER);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteAssetModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes asset model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void WriteAssetModelDataOntoNVM(uint16 offset)
{
    uint16 temp=0;

    Nvm_Write((uint16*) (&asset_model_hdlr_data.asset_state.interval),
              sizeof(uint16),
              offset + NVM_OFFSET_ASSET_INTERVAL);

    Nvm_Write((uint16*) (&asset_model_hdlr_data.asset_state.sideeffects),
              sizeof(uint16),
              offset + NVM_OFFSET_ASSET_SIDEEFFECTS);

    Nvm_Write((uint16*) (&asset_model_hdlr_data.asset_state.todestinationid),
              sizeof(uint16),
              offset + NVM_OFFSET_ASSET_DEST_ID);

    /* Pack the num announce and announce interval onto word to reduce space.*/
    temp = 
        asset_model_hdlr_data.asset_state.numannounces << 8 |
        asset_model_hdlr_data.asset_state.announceinterval;

    Nvm_Write((uint16*) (&temp), 
              sizeof(uint16),
              offset + NVM_OFFSET_ASSET_ANNOUNCE_INTERVAL_AND_NO);

    Nvm_Write((uint16*) (&asset_model_hdlr_data.asset_state.txpower),
              sizeof(uint16),
              offset + NVM_OFFSET_ASSET_TX_POWER);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AssetModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Asset model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AssetModelHandlerInit(uint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups)
{
    /* Initialize Asset Model */
    AssetModelInit(nw_id, 
                   model_groups,
                   num_groups,
                   assetModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AssetModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Asset Model data on the global 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AssetModelDataInit(void)
{
    MemSet(&asset_model_hdlr_data, 0, sizeof(ASSET_HANDLER_DATA_T));

    asset_model_hdlr_data.asset_state.interval = DEFAULT_BCAST_INTERVAL;
    asset_model_hdlr_data.asset_state.txpower = DEFAULT_TX_POWER;
    asset_model_hdlr_data.asset_state.txpower &= 0x00FF;
    asset_model_hdlr_data.asset_state.numannounces = DEFAULT_ANNOUNCE_COUNT;
    asset_model_hdlr_data.asset_state.announceinterval = DEFAULT_ANNOUNCE_INTERVAL;
    asset_model_hdlr_data.asset_state.sideeffects = ASSET_SIDE_EFFECT_VALUE;
    asset_model_hdlr_data.asset_state.todestinationid = MESH_BROADCAST_ID;

    TimerDelete(asset_bcast_timer_tid);
    asset_bcast_timer_tid = TIMER_INVALID;
}

#endif /* ENABLE_ASSET_MODEL */

