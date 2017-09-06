/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      tracker_model_handler.c
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
#include "tracker_server.h"
#include "app_mesh_handler.h"
#include "tracker_model_handler.h"
#include "tracker_client.h"
#include "asset_client.h"
#include "main_app.h"

#ifdef ENABLE_TRACKER_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* Macros for NVM access */
#define NVM_OFFSET_TRACKER_ZONE_THRESHOLDS                (0)
#define NVM_OFFSET_TRACKER_DELETE_INTERVAL                (3)
#define NVM_OFFSET_TRACKER_REPORT_DEST_ID                 (5)
#define NVM_OFFSET_TRACKER_DELAY_OFFSET                   (6)
#define NVM_OFFSET_TRACKER_DELAY_FACTOR                   (7)

/* Default values */
#define DEFAULT_ZONE0_THRESHOLD                           (-60)
#define DEFAULT_ZONE1_THRESHOLD                           (-83)
#define DEFAULT_ZONE2_THRESHOLD                           (-100)
#define TRACKER_MAX_ZONES                                 (3)
#define TRACKER_DEFAULT_ZONE                              (2)

// Limits on RSSI
#define ASSET_MAX_RSSI                                    (-30)
#define ASSET_MIN_RSSI                                    (-120)

// Put this in the zone to mark pending assets for delete
#define MARK_FOR_DELETE                                   (15)

// Factors used to compute report delay in ms = (OFFSET - RSSI) * FACTOR
// remember rssi is negative - this gives roughly 3 to 4.5 second delay
#define DEFAULT_DELAY_OFFSET                              (60)
#define DEFAULT_DELAY_FACTOR                              (30)

// Time before we delete asset from cache (10 minutes)
#define DEFAULT_DELETE_INTERVAL                           (600)

typedef struct
{
    uint16   dev_id;            /* Dev ID of asset.*/
    uint8    proximity;         /* Asset proximity zone.*/
    int8     rssi;              /* Asset RSSI.*/
    uint32   timeLastHeard;     /* Timestamp in seconds when heard.*/
    uint32   deleteCount;       /* Time before removed from cache,or time before report for new assets */
    uint8    count;             /* How many times heard (used for averaging RSSI) */
    uint16   effects;           /* Asset side effects mask */
} ASSET_INFO_T;

/* Application Model Handler Data Structure */
typedef struct
{
    ASSET_INFO_T                trackerCache [TRACKER_MAX_CACHED_ASSETS];
    ASSET_INFO_T                pendingCache [TRACKER_MAX_PENDING_ASSETS];
    int8                        zoneThresholds[TRACKER_MAX_ZONES];
    uint32                      assetDeleteInterval; // in seconds
    uint16                      reportDest;     // Dest addr for reports 
    uint16                      delayOffset;
    uint16                      delayFactor;
    uint32                      secTimerCount;
    uint32                      delayFactorTimerCount;
} TRACKER_HANDLER_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/
 
/* Pointer to model handler Data */
static TRACKER_HANDLER_DATA_T tracker_hdlr_data;

/* 1 Second Timer for sending tracker reports, deleting assets from asset cache */
static timer_id asset_cache_timer_tid = TIMER_INVALID;

/* Millisecond Timer for deleting assets from pending cache */
static timer_id pending_cache_timer_tid = TIMER_INVALID;

/* Tracker found to be sent in response to tracker find */
static CSRMESH_TRACKER_FOUND_T tracker_found;

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      trackerFindAssetInCache
 *
 *  DESCRIPTION
 *      The function checks whether the device id of the passed asset is 
 *      present in the asset cache and if present it returns the index
 *      of the cached asset.
 *
 *  RETURNS
 *      Index of the asset in the passed cache
 *
 *---------------------------------------------------------------------------*/
static uint16 trackerFindAssetInCache(uint16 device_id, ASSET_INFO_T asset_cache[],
                                      uint16 asset_count)
{
    uint16 index;

    for(index = 0; index < asset_count; index++)
    {
        if(asset_cache[index].dev_id == device_id)
        {
            return index;
        }
    }
    return 0xFFFF;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      trackerFindFreeSlotInCache
 *
 *  DESCRIPTION
 *      The function finds a free slot in the cache and returns the index for
 *      the same
 *
 *  RETURNS
 *      Index of the first free slot to store the asset in the passed cache
 *
 *---------------------------------------------------------------------------*/
static uint16 trackerFindFreeSlotInCache(ASSET_INFO_T asset_cache[],uint16 asset_count)
{
    uint16 index;
    uint16 oldest_index=0, max_age_seconds=0, age_seconds;

    for(index = 0; index < asset_count; index++)
    {
        if(asset_cache[index].dev_id == 0x00)
        {
            return index;
        }
    }

    for(index = 0; index < asset_count; index++)
    {
        age_seconds = (tracker_hdlr_data.secTimerCount > asset_cache[index].timeLastHeard) ?
                        (tracker_hdlr_data.secTimerCount - asset_cache[index].timeLastHeard) :
                        ((0xFFFF - asset_cache[index].timeLastHeard) + tracker_hdlr_data.secTimerCount);

        if(age_seconds > max_age_seconds)
        {
            max_age_seconds = age_seconds;
            oldest_index = index;
        }
    }
    return oldest_index;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      trackerClearCache
 *
 *  DESCRIPTION
 *      The function clears the complete cache
 *
 *  RETURNS
 *      None
 *
 *---------------------------------------------------------------------------*/
static void trackerClearCache(ASSET_INFO_T asset_cache[],uint16 asset_count)
{
    uint16 index;

    for(index = 0; index < asset_count; index++)
    {
        MemSet(&asset_cache[index], 0, sizeof(ASSET_INFO_T));
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      trackerIsCacheEmpty
 *
 *  DESCRIPTION
 *      The function returns whether the cahce is empty or not
 *
 *  RETURNS
 *      Returns TRUE if the cache is empty otherwise returns FALSE
 *
 *---------------------------------------------------------------------------*/
static bool trackerIsCacheEmpty(ASSET_INFO_T asset_cache[],uint16 asset_count)
{
    uint16 index;

    for(index = 0; index < asset_count; index++)
    {
        if(asset_cache[index].dev_id != 0x00)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      convertRssiToZone
 *
 *  DESCRIPTION
 *      This function converts the RSSI into zone
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static uint16 convertRssiToZone(int8 rssi)
{
    uint16 index;

    for (index=0; index < TRACKER_MAX_ZONES ; ++index) 
    {
        if (rssi >= tracker_hdlr_data.zoneThresholds[index])
        {
            return index;
        }
    }
    return TRACKER_DEFAULT_ZONE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      pendingCacheTimerHandler
 *
 *  DESCRIPTION
 *      This function handles the expiry of the asset cache timer 
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void pendingCacheTimerHandler(timer_id tid)
{
    if (pending_cache_timer_tid == tid)
    {
        uint16 index;
        tracker_hdlr_data.delayFactorTimerCount++;
        pending_cache_timer_tid = TIMER_INVALID;

        for(index=0; index < TRACKER_MAX_PENDING_ASSETS; index++)
        {
            if(tracker_hdlr_data.pendingCache[index].deleteCount == tracker_hdlr_data.delayFactorTimerCount)
            {
                ASSET_INFO_T *pending_asset = &tracker_hdlr_data.pendingCache[index];
                uint16 cached_index;

                if (pending_asset->proximity == MARK_FOR_DELETE) 
                {
                    /* This asset was heard more strongly elsewhere remove the pending entry */
                    MemSet(&tracker_hdlr_data.pendingCache[index], 0, sizeof(ASSET_INFO_T));
                }
                else
                {
                    /* Check whether the asset is already present in the asset cache */
                    cached_index = 
                        trackerFindAssetInCache(pending_asset->dev_id, tracker_hdlr_data.trackerCache, TRACKER_MAX_CACHED_ASSETS);

                    /* Asset not present in tracker cache, hence add it onto tracker cache */
                    if(cached_index == 0xFFFF)
                    {
                        cached_index = trackerFindFreeSlotInCache(tracker_hdlr_data.trackerCache,  TRACKER_MAX_CACHED_ASSETS);
                    }

                    /* Update the asset information in tracker cache and remove from the pending entry */
                    if(cached_index != 0xFFFF)
                    {
                        ASSET_INFO_T *cached_asset = &tracker_hdlr_data.trackerCache[cached_index];
                        CSRMESH_TRACKER_REPORT_T report;
                        
                        report.assetdeviceid = cached_asset->dev_id = pending_asset->dev_id;
                        report.sideeffects = cached_asset->effects = pending_asset->effects;
                        report.rssi = cached_asset->rssi = pending_asset->rssi;
                        report.zone = cached_asset->proximity = convertRssiToZone(pending_asset->rssi);
                        report.ageseconds = (tracker_hdlr_data.secTimerCount >= cached_asset->timeLastHeard) ? (tracker_hdlr_data.secTimerCount - cached_asset->timeLastHeard) : 
                                            ((0xFFFF - cached_asset->timeLastHeard) + tracker_hdlr_data.secTimerCount);

                        /* Send tracker report */
                        TrackerReport(DEFAULT_NW_ID,
                                      tracker_hdlr_data.reportDest,
                                      AppGetCurrentTTL(),
                                      &report);

                        /* Store the delete interval for this asset deletion */
                        cached_asset->deleteCount = tracker_hdlr_data.secTimerCount + tracker_hdlr_data.assetDeleteInterval;
                    }
                    /* Removing the asset from the pending entry */
                    MemSet(&tracker_hdlr_data.pendingCache[index], 0, sizeof(ASSET_INFO_T));
                }
            }
        }

        /* If there are any pending assets then re-start the timer */
        if(!trackerIsCacheEmpty(tracker_hdlr_data.pendingCache, TRACKER_MAX_PENDING_ASSETS))
        {
            pending_cache_timer_tid = TimerCreate(tracker_hdlr_data.delayFactor * MILLISECOND,
                                                  TRUE, pendingCacheTimerHandler);
        }
    }
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      assetCacheTimerHandler
 *
 *  DESCRIPTION
 *      This function handles the expiry of the asset cache timer 
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void assetCacheTimerHandler(timer_id tid)
{
    if (asset_cache_timer_tid == tid)
    {
        uint16 index;
        tracker_hdlr_data.secTimerCount++;
        asset_cache_timer_tid = TIMER_INVALID;

        for(index=0; index < TRACKER_MAX_CACHED_ASSETS; index++)
        {
            if(tracker_hdlr_data.trackerCache[index].deleteCount == tracker_hdlr_data.secTimerCount)
            {
                /* Remove the asset from tracker cache as the delete interval 
                 * has expired for the asset.
                 */
                MemSet(&tracker_hdlr_data.trackerCache[index], 0, sizeof(ASSET_INFO_T));
            }
        }

        /* If there are any pending or cached assets then re-start the timer */
        if((!trackerIsCacheEmpty(tracker_hdlr_data.pendingCache, TRACKER_MAX_PENDING_ASSETS)) ||
          (!trackerIsCacheEmpty(tracker_hdlr_data.trackerCache, TRACKER_MAX_CACHED_ASSETS)))
        {
            asset_cache_timer_tid = TimerCreate(SECOND, TRUE, assetCacheTimerHandler);
        }
        else
        {
            tracker_hdlr_data.secTimerCount = 0;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      trackerModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Tracker Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult trackerModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                              CSRMESH_EVENT_DATA_T* data,
                                              CsrUint16 length,
                                              void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_ASSET_ANNOUNCE:
        {
            CSRMESH_ASSET_ANNOUNCE_T *p_event = 
                                    (CSRMESH_ASSET_ANNOUNCE_T *)data->data;
            uint16 asset_index;
            int8 rssi = data->rx_rssi;

            /* Check whether the device is present in the pending cache */
            asset_index = 
                trackerFindAssetInCache(data->src_id, tracker_hdlr_data.pendingCache, TRACKER_MAX_PENDING_ASSETS);

            if (rssi < ASSET_MIN_RSSI) rssi = ASSET_MIN_RSSI;
            if (rssi > ASSET_MAX_RSSI) rssi = ASSET_MAX_RSSI;

            /* The asset is already present in the pending cache */
            if(asset_index != 0xFFFF)
            {
                ASSET_INFO_T *asset = &tracker_hdlr_data.pendingCache[asset_index];
                /* If caching with rolling avg is enabled calculate rolling 
                 * avg otherwise store the latest rssi
                 */
#ifdef TRACKER_CACHE_RSSI_ROLLING_AVG
                int32 newRssi;
                newRssi = ((int32)rssi + ((int32)asset->rssi * (int32)asset->count)) / (int32)(asset->count + 1);
                asset->count++;
                asset->rssi = (int8)newRssi;
#else
                asset->rssi = rssi;
#endif
                asset->timeLastHeard = tracker_hdlr_data.secTimerCount;
            }
            /* The asset is not present in the pending cache so add to pending cache */
            else
            {
                asset_index = trackerFindFreeSlotInCache(tracker_hdlr_data.pendingCache,  TRACKER_MAX_PENDING_ASSETS);
                if(asset_index != 0xFFFF)
                {
                    ASSET_INFO_T *asset = &tracker_hdlr_data.pendingCache[asset_index];
                    
                    asset->dev_id = data->src_id;
                    asset->effects = p_event->sideeffects;
                    asset->rssi = rssi;
                    asset->count = 1;

                    /* If the timer does not exist create a new timer */
                    if(pending_cache_timer_tid == TIMER_INVALID)
                    {
                        pending_cache_timer_tid = TimerCreate(tracker_hdlr_data.delayFactor * MILLISECOND,
                                                              TRUE,
                                                              pendingCacheTimerHandler);
                        tracker_hdlr_data.delayFactorTimerCount = 0;
                    }

                    /* If the timer does not exist create a new timer */
                    if(asset_cache_timer_tid == TIMER_INVALID)
                    {
                        asset_cache_timer_tid = TimerCreate(SECOND, TRUE,
                                                            assetCacheTimerHandler);
                        tracker_hdlr_data.secTimerCount = 0;
                    }

                    asset->timeLastHeard = tracker_hdlr_data.secTimerCount;

                    /* Timer count is calculated by taking the absolute rssi value and adding the delay offset */
                    asset->deleteCount = tracker_hdlr_data.delayFactorTimerCount + (tracker_hdlr_data.delayOffset - asset->rssi);
                }
            }

            asset_index = 
                trackerFindAssetInCache(data->src_id, tracker_hdlr_data.trackerCache, TRACKER_MAX_CACHED_ASSETS);

            /* Check whether the asset is present in the tracker cache */
            if(asset_index != 0xFFFF)
            {
                ASSET_INFO_T *asset = &tracker_hdlr_data.trackerCache[asset_index];
                asset->timeLastHeard = tracker_hdlr_data.secTimerCount;
                asset->rssi = rssi;
                asset->effects = p_event->sideeffects;
                asset->deleteCount = tracker_hdlr_data.secTimerCount + tracker_hdlr_data.assetDeleteInterval;
            }
        }
        break;

        case CSRMESH_TRACKER_FIND:
        {
            CSRMESH_TRACKER_FIND_T *p_event = 
                                    (CSRMESH_TRACKER_FIND_T *)data->data;

            /* Find the asset in the tracker cache */
            uint16 asset_index = 
                trackerFindAssetInCache(p_event->assetdeviceid, tracker_hdlr_data.trackerCache, TRACKER_MAX_CACHED_ASSETS);

            /* If the device is found in the tracker cache, send the relavant info in tracker found. */
            if(asset_index != 0xFFFF)
            {
                ASSET_INFO_T *asset = &tracker_hdlr_data.trackerCache[asset_index];
                MemSet(&tracker_found, 0, sizeof(CSRMESH_TRACKER_FOUND_T));
                tracker_found.assetdeviceid = p_event->assetdeviceid;
                tracker_found.tid = p_event->tid;
                tracker_found.ageseconds = (tracker_hdlr_data.secTimerCount > asset->timeLastHeard) ? (tracker_hdlr_data.secTimerCount - asset->timeLastHeard) : 
                                           ((0xFFFF - asset->timeLastHeard) + tracker_hdlr_data.secTimerCount);
                tracker_found.rssi = asset->rssi;
                tracker_found.sideeffects = asset->effects;
                tracker_found.zone = asset->proximity;

                /* Send response data to model */
                if (state_data != NULL)
                {
                    *state_data = (void *)&tracker_found;
                }
            }
            else
            {
                if (state_data != NULL)
                {
                    *state_data = NULL;
                }
            }
        }
        break;

        case CSRMESH_TRACKER_REPORT:
        {
            CSRMESH_TRACKER_REPORT_T *p_event = 
                                    (CSRMESH_TRACKER_REPORT_T *)data->data;
            uint16 asset_index;
            int8 rssi = 0xFF00 | p_event->rssi;

            asset_index = 
                trackerFindAssetInCache(p_event->assetdeviceid, tracker_hdlr_data.pendingCache, TRACKER_MAX_PENDING_ASSETS);

            /* If the device is found in the pending cahche and the report contains a better RSSI value then mark for delete */
            if(asset_index != 0xFFFF)
            {
                if(rssi > tracker_hdlr_data.pendingCache[asset_index].rssi)
                {
                    tracker_hdlr_data.pendingCache[asset_index].proximity = MARK_FOR_DELETE;
                }
            }

            asset_index = 
                trackerFindAssetInCache(p_event->assetdeviceid, tracker_hdlr_data.trackerCache, TRACKER_MAX_CACHED_ASSETS);

            /* If the device is found in the tracker cache, and the report contains a better RSSI value then delete the asset. */
            if(asset_index != 0xFFFF)
            {
                if(rssi > tracker_hdlr_data.trackerCache[asset_index].rssi)
                {
                    MemSet(&tracker_hdlr_data.trackerCache[asset_index], 0, sizeof(ASSET_INFO_T));
                }
            }
        }
        break;

        case CSRMESH_TRACKER_CLEAR_CACHE:
        {
            /* Clear the pending and the tracker cache */
            trackerClearCache(tracker_hdlr_data.pendingCache, TRACKER_MAX_PENDING_ASSETS);
            trackerClearCache(tracker_hdlr_data.trackerCache, TRACKER_MAX_CACHED_ASSETS);
        }
        break;

        case CSRMESH_TRACKER_SET_PROXIMITY_CONFIG:
        {
            CSRMESH_TRACKER_SET_PROXIMITY_CONFIG_T *p_event = 
                                    (CSRMESH_TRACKER_SET_PROXIMITY_CONFIG_T *)data->data;

            tracker_hdlr_data.zoneThresholds[0] = p_event->zone0rssithreshold;
            tracker_hdlr_data.zoneThresholds[1] = p_event->zone1rssithreshold;
            tracker_hdlr_data.zoneThresholds[2] = p_event->zone2rssithreshold;
            tracker_hdlr_data.assetDeleteInterval = p_event->cachedeleteinterval;
            tracker_hdlr_data.delayFactor = p_event->delayfactor;
            tracker_hdlr_data.delayOffset = p_event->delayoffset;
            tracker_hdlr_data.reportDest = p_event->reportdest;
            WriteTrackerModelDataOntoNVM(NVM_OFFSET_TRACKER_MODEL_DATA);
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
 *      ReadTrackerModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads tracker model data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void ReadTrackerModelDataFromNVM(uint16 offset)
{
    uint16 index;

    for(index = 0; index < TRACKER_MAX_ZONES; index++)
    {
        Nvm_Read((uint16 *)&tracker_hdlr_data.zoneThresholds[index],
                 sizeof(uint16),
                 offset + NVM_OFFSET_TRACKER_ZONE_THRESHOLDS + index);
    }

    Nvm_Read((uint16 *)&tracker_hdlr_data.assetDeleteInterval,
             sizeof(uint32),
             offset + NVM_OFFSET_TRACKER_DELETE_INTERVAL);

    Nvm_Read((uint16 *)&tracker_hdlr_data.reportDest,
             sizeof(uint16),
             offset + NVM_OFFSET_TRACKER_REPORT_DEST_ID);

    Nvm_Read((uint16 *)&tracker_hdlr_data.delayOffset,
             sizeof(uint16),
             offset + NVM_OFFSET_TRACKER_DELAY_OFFSET);

    Nvm_Read((uint16 *)&tracker_hdlr_data.delayFactor,
             sizeof(uint16),
             offset + NVM_OFFSET_TRACKER_DELAY_FACTOR);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteTrackerModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes tracker model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void WriteTrackerModelDataOntoNVM(uint16 offset)
{
    uint16 index;

    for(index = 0; index < TRACKER_MAX_ZONES; index++)
    {
        Nvm_Write((uint16 *)&tracker_hdlr_data.zoneThresholds[index],
                  sizeof(uint16),
                  offset + NVM_OFFSET_TRACKER_ZONE_THRESHOLDS + index);
    }

    Nvm_Write((uint16*) (&tracker_hdlr_data.assetDeleteInterval),
              sizeof(uint32),
              offset + NVM_OFFSET_TRACKER_DELETE_INTERVAL);

    Nvm_Write((uint16*) (&tracker_hdlr_data.reportDest),
              sizeof(uint16),
              offset + NVM_OFFSET_TRACKER_REPORT_DEST_ID);

    Nvm_Write((uint16*) (&tracker_hdlr_data.delayOffset),
              sizeof(uint16),
              offset + NVM_OFFSET_TRACKER_DELAY_OFFSET);

    Nvm_Write((uint16*) (&tracker_hdlr_data.delayFactor),
              sizeof(uint16),
              offset + NVM_OFFSET_TRACKER_DELAY_FACTOR);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      TrackerModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Tracker model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void TrackerModelHandlerInit(uint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups)
{
    /* Initialize Tracker Model */
    TrackerModelInit(nw_id, 
                     model_groups,
                     num_groups,
                     trackerModelEventHandler);

    /* Initialize Tracker Client */
    TrackerModelClientInit(trackerModelEventHandler);

    /* Initialize Asset Client */
    AssetModelClientInit(trackerModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      TrackerModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Tracker Model data on the global 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void TrackerModelDataInit(void)
{
    MemSet(&tracker_hdlr_data, 0, sizeof(TRACKER_HANDLER_DATA_T));

    tracker_hdlr_data.zoneThresholds[0] = DEFAULT_ZONE0_THRESHOLD;
    tracker_hdlr_data.zoneThresholds[1] = DEFAULT_ZONE1_THRESHOLD;
    tracker_hdlr_data.zoneThresholds[2] = DEFAULT_ZONE2_THRESHOLD;

    tracker_hdlr_data.assetDeleteInterval = DEFAULT_DELETE_INTERVAL;
    tracker_hdlr_data.reportDest = MESH_BROADCAST_ID;
    tracker_hdlr_data.delayOffset = DEFAULT_DELAY_OFFSET;
    tracker_hdlr_data.delayFactor = DEFAULT_DELAY_FACTOR;

    TimerDelete(asset_cache_timer_tid);
    asset_cache_timer_tid = TIMER_INVALID;

    TimerDelete(pending_cache_timer_tid);
    pending_cache_timer_tid = TIMER_INVALID;
}

#endif /* ENABLE_TRACKER_MODEL */

