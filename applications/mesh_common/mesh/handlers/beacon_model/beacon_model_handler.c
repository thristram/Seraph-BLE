/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      beacon_model_handler.c
 *
 *  DESCRIPTION
 *      Implements handler function for beacon model messages
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <time.h>
#include <timer.h>
#include <mem.h>
#include <random.h>
#ifdef CSR101x
#include <ls_app_if.h>
#else
#include <ls_api.h>
#endif

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "iot_hw.h"
#include "csr_mesh.h"
#include "beacon_model_handler.h"
#include "app_mesh_handler.h"
#include "core_mesh_handler.h"
#include "main_app.h"
#include "nvm_access.h"
#include "battery_hw.h"
#include "app_util.h"
#include "beacon_client.h"

#if defined(APP_BEACON_MODE)
#include "app_mesh_beacon_handler.h"
#endif

#if defined(APP_LUMICAST_MODE)
#include "app_mesh_lumicast_handler.h"
#endif

#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
#include "beacon_proxy_model_handler.h"
#endif

#ifdef ENABLE_BEACON_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#define MESH_ON_TIME_UNIT                       (10 * SECOND)
#define MESH_INTERVAL_UNIT                      (1 * MINUTE)
#define MINUTE_IN_TENS_SECONDS                  (6)
#define MAX_TIMER_VAL_IN_TENS_SECONDS           (210)
#define BEACON_INDEX_INVALID                    (0xFF)
#define DEFAULT_PAYLOAD_ID                      (0)
#define RETRANSMIT_STATUS_MESH_TIME             (10)

/* Macros for NVM access */
#define NVM_OFFSET_BEACON_TYPE_INTERVAL         (0)
#define NVM_OFFSET_BEACON_MESH_INTVL            (1)
#define NVM_OFFSET_BEACON_MESH_TIME             (2)
#define NVM_OFFSET_BEACON_TX_POWER_LEN          (3)
#define NVM_OFFSET_BEACON_PAYLOAD_ID            (4)

#if defined(APP_BEACON_MODE) || defined(APP_LUMICAST_MODE) 
#define NVM_OFFSET_BEACON_PKT                   (5)
#endif

#ifdef APP_PROXY_MODE
#define NVM_OFFSET_BEACON_DEV_ID                (5)
#define NVM_OFFSET_BEACON_PAYLOAD_OFFSET        (6)
#define NVM_OFFSET_BEACON_PKT                   (7)
#endif

#define SET_STATUS_TID                          (0x20)
#define ZERO_TTL                                (0)

typedef struct
{
    CSRMESH_BEACON_BEACON_STATUS_T   beacon_status;
    CSRMESH_BEACON_TYPES_T           beacon_types;
} MODEL_RSP_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/
static BEACON_HANDLER_DATA_T*                   p_beacon_hdlr_data;
#ifdef APP_BEACON_MODE
static device_beacon_mode_t                     dev_mode;
static timer_id                                 device_mode_tid;
static timer_id                                 retransmit_status_tid;
static CsrUint8                                 time_transmit_count;
#endif
/* Model Response Common Data */
static MODEL_RSP_DATA_T                         g_model_rsp_data;

/* Default beacon payload for the supported beacon types */
#ifdef APP_BEACON_MODE
static uint8 g_default_ibeacon[IBEACON_PAYLOAD_SIZE] = 
    {0x12, 0x34, 0x56, 0x78, 0x12, 0x35, 0x12, 0x36, 0x12, 0x37,
     0xab, 0xcd, 0xef, 0x13, 0x57, 0x92, 0x00, 0x00, 0x00, 0x00, 0xc6};

static uint8 g_default_csr_beacon[CSR_RETAIL_BEACON_PAYLOAD_SIZE] = 
    {0x01, 0x01, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x03, 0x5d};

static uint8 g_default_eddystone_url[] = 
    {0x00/*Tx Power */, 0x02/*URL Prefix: https:// */, 'w','w','w','.','q','u',
    'a','l','c','o','m','m','.','c','o','m'};

static uint8 g_default_eddystone_uid[EDDYSTONE_PAYLOAD_SIZE] = 
    {0x00/*Tx Power */, 
     /*NID*/0x8b, 0x0c, 0xa7, 0x50, 0xe7, 0xa7, 0x4e, 0x14, 0xbd, 0x99, 
     /*BID*/0x09, 0x54, 0x77, 0xcb, 0x3e, 0x77,
     /*RFU*/0x00, 0x00};
#endif /* APP_BEACON_MODE */

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      verifyPayloadLength
 *
 *  DESCRIPTION
 *      The function is called to check for the length of the payload received
 *      for different types of beacons.
 *
 *  RETURNS
 *      TRUE if payload length is accepted and FALSE if not.
 *---------------------------------------------------------------------------*/
static bool verifyPayloadLength(CsrUint8 type, CsrUint8 payload_len)
{
    bool payload_accepted = TRUE;

    switch(type)
    {
        case csr_mesh_beacon_type_csr:
        {
            if(payload_len > CSR_RETAIL_BEACON_PAYLOAD_SIZE)
            {
                payload_accepted = FALSE;
            }
        }
        break;

        case csr_mesh_beacon_type_ibeacon:
        {
            if(payload_len > IBEACON_PAYLOAD_SIZE)
            {
                payload_accepted = FALSE;
            }
        }
        break;

        case csr_mesh_beacon_type_eddystone_uid:
        case csr_mesh_beacon_type_eddystone_url:
        {
            if(payload_len > EDDYSTONE_PAYLOAD_SIZE)
            {
                payload_accepted = FALSE;
            }
        }
        break;

        default:
        {
            if(payload_len > MAX_BEACON_PAYLOAD_SIZE)
            {
                payload_accepted = FALSE;
            }
        }
        break;

        break;
    }
    return payload_accepted;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      writeBeaconDataOnIndex
 *
 *  DESCRIPTION
 *      This function writes beacon data structure onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void writeBeaconDataOnIndex(uint8 index)
{
    uint16 index1, temp;
    uint16 nvm_beacon_index = GET_BEACON_NVM_OFFSET(index);

    Nvm_Write((uint16*) (&p_beacon_hdlr_data->mesh_time_idx), 
              sizeof(uint16),
              NVM_OFFSET_BEACON_MESH_TIME_IDX);

    /* Start storing the beacon information based on the index. Pack the beacon
     * type, beacon interval and tx power, payload length to reduce space.
     */
    temp = 
        p_beacon_hdlr_data->beacons[index].beaconinterval << 8 |
        p_beacon_hdlr_data->beacons[index].beacontype;

    Nvm_Write((uint16*) (&temp), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_TYPE_INTERVAL);

    /* Store the mesh time and the mesh interval received which is common
     * across all beacons
     */
    Nvm_Write((uint16*) (&p_beacon_hdlr_data->beacons[index].meshinterval), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_MESH_INTVL);


    Nvm_Write((uint16*) (&p_beacon_hdlr_data->beacons[index].meshtime), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_MESH_TIME);


    temp = 
        p_beacon_hdlr_data->beacons[index].payloadlength << 8 |
        p_beacon_hdlr_data->beacons[index].txpower;

    Nvm_Write((uint16*) (&temp), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_TX_POWER_LEN);

    Nvm_Write((uint16*) (&p_beacon_hdlr_data->beacons[index].payloadid), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_PAYLOAD_ID);

#ifdef APP_PROXY_MODE
    Nvm_Write((uint16*) (&p_beacon_hdlr_data->beacons[index].dev_id), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_DEV_ID);

    Nvm_Write((uint16*) (&p_beacon_hdlr_data->beacons[index].payload_offset), 
              sizeof(uint16),
              nvm_beacon_index + NVM_OFFSET_BEACON_PAYLOAD_OFFSET);
#endif

    /* Store the beacon data here */
    for(index1 = 0; 
        index1 <= p_beacon_hdlr_data->beacons[index].payloadlength;
        index1= index1 + 2)
    {
        temp = 
            p_beacon_hdlr_data->beacons[index].payload[index1 + 1] << 8 |
            p_beacon_hdlr_data->beacons[index].payload[index1];

        Nvm_Write((uint16*) (&temp), 
                  sizeof(uint16),
                  nvm_beacon_index + NVM_OFFSET_BEACON_PKT + (index1/2));
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      updateBeaconStatus
 *
 *  DESCRIPTION
 *      This function updates the beacon status for the beacon type requested
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void updateBeaconStatus(uint8 index)
{
    g_model_rsp_data.beacon_status.beacontype 
                    = p_beacon_hdlr_data->beacons[index].beacontype;

    g_model_rsp_data.beacon_status.meshinterval = 
                  p_beacon_hdlr_data->beacons[index].meshinterval;

    g_model_rsp_data.beacon_status.meshtime = 
                  p_beacon_hdlr_data->beacons[index].meshtime;

    g_model_rsp_data.beacon_status.beaconinterval = 
        p_beacon_hdlr_data->beacons[index].beaconinterval;

    g_model_rsp_data.beacon_status.payloadid = 
        p_beacon_hdlr_data->beacons[index].payloadid;

    g_model_rsp_data.beacon_status.txpower = 
        p_beacon_hdlr_data->beacons[index].txpower;

    g_model_rsp_data.beacon_status.batterylevel = ReadBatteryLevel();
}

#ifdef APP_PROXY_MODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      getBeaconIndexForProxy
 *
 *  DESCRIPTION
 *      This function returns the a valid index if beacon type and dev id 
 *      matches otherwise sends an invalid index.
 *
 *  RETURNS
 *      The index at which the beacon type is already stored or could be stored.
 *
 *---------------------------------------------------------------------------*/
static uint16 getBeaconIndexForProxy(uint8 beacon_type, uint16 dev_id)
{
    uint8 index;

    /* check whether the type and device id match in the array */
    for (index = 0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        if(p_beacon_hdlr_data->beacons[index].beacontype == beacon_type && 
           p_beacon_hdlr_data->beacons[index].dev_id == dev_id)
        {
            return index;
        }
    }
    return BEACON_INDEX_INVALID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getBeaconStoringIndexForProxy
 *
 *  DESCRIPTION
 *      This function returns the index where the beacon type needs to be stored
 *      for the specific device id.
 *
 *  RETURNS
 *      The index at which the beacon type is already stored or could be stored.
 *
 *---------------------------------------------------------------------------*/
static uint16 getBeaconStoringIndexForProxy(uint8 beacon_type, uint16 dev_id)
{
    /* check whether the type and device id match in the array */
    uint8 index = getBeaconIndexForProxy(beacon_type, dev_id);

    /* If the device is not present then look for an empty slot */
    if(index == BEACON_INDEX_INVALID)
    {
        /* check whether there is an empty slot in the array */
        for (index = 0; index < MAX_BEACONS_SUPPORTED; index++)
        {
            if(p_beacon_hdlr_data->beacons[index].beacontype == BEACON_TYPE_INVALID)
            {
                p_beacon_hdlr_data->beacons[index].beacontype = beacon_type;
                p_beacon_hdlr_data->beacons[index].dev_id = dev_id;
                return index;
            }
        }

        /* Choose a random slot to remove from the beacon array*/
        index = Random16()% MAX_BEACONS_SUPPORTED;
        MemSet(&p_beacon_hdlr_data->beacons[index], 0, sizeof(BEACON_INFO_T));
        p_beacon_hdlr_data->beacons[index].beacontype = beacon_type;
        p_beacon_hdlr_data->beacons[index].dev_id = dev_id;
    }
    return index;
}
#endif

#ifdef APP_BEACON_MODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      getBeaconStoringIndex
 *
 *  DESCRIPTION
 *      This function returns the index where the beacon type needs to be stored
 *
 *  RETURNS
 *      The index at which the beacon type is already stored or could be stored.
 *
 *---------------------------------------------------------------------------*/
static uint16 getBeaconStoringIndex(uint8 beacon_type)
{
    uint8 index;

    /* check whether the beacon type is already stored */
    for (index = 0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        if(p_beacon_hdlr_data->beacons[index].beacontype == beacon_type)
        {
            return index;
        }
    }
    return BEACON_INDEX_INVALID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      sendBeaconStatusForActiveBeacons
 *
 *  DESCRIPTION
 *      This function sends the beacon status for all the active beacon types.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void sendBeaconStatusForActiveBeacons(void)
{
    uint8 index;
    uint16 self_dev_id = 0;
    CSR_MESH_APP_EVENT_DATA_T get_dev_id_data;
    get_dev_id_data.appCallbackDataPtr = &self_dev_id;
    CSRmeshGetDeviceID(CSR_MESH_DEFAULT_NETID, &get_dev_id_data);

    /* check whether the beacon type is already stored */
    for (index = 0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        if(p_beacon_hdlr_data->beacons[index].beacontype != BEACON_TYPE_INVALID
           && p_beacon_hdlr_data->beacons[index].beaconinterval > 0)
        {
            updateBeaconStatus(index);
            BeaconBeaconStatus(DEFAULT_NW_ID, self_dev_id, MESH_BROADCAST_ID,
                               ZERO_TTL, &g_model_rsp_data.beacon_status);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      deviceModeChangeHandler
 *
 *  DESCRIPTION
 *      This function is a timer callback function to handle the timeout 
 *      of the current device mode to switch between application and timer
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void deviceModeChangeHandler(timer_id tid)
{
    device_beacon_mode_t new_mode;
    uint32 time_interval = 0;

    if(tid == device_mode_tid)
    {
        device_mode_tid = TIMER_INVALID;
        if(p_beacon_hdlr_data->time_wrap_cnt == 0 && 
                p_beacon_hdlr_data->time_remaining == 0)
        {
            /* Mode change expiry. Change the mode to beacon or mesh mode */
            new_mode = (dev_mode == device_mode_beacon)? device_mode_mesh:
                                                            device_mode_beacon;
            BeaconSetMode(new_mode);
        }
        else
        {
            if(p_beacon_hdlr_data->time_wrap_cnt > 0)
            {
                p_beacon_hdlr_data->time_wrap_cnt--;
                time_interval = MAX_TIMER_VAL_IN_TENS_SECONDS;
            }
            else
            {
                time_interval = p_beacon_hdlr_data->time_remaining;
                p_beacon_hdlr_data->time_remaining = 0;
            }

            device_mode_tid = TimerCreate(time_interval * 10 * SECOND,
                                          TRUE, 
                                          deviceModeChangeHandler);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      retransmitBeaconStatusHandler
 *
 *  DESCRIPTION
 *      This function is a timer callback function to handle the timeout 
 *      at which a new status message is sent onto the n/w from the beacon.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void retransmitBeaconStatusHandler(timer_id tid)
{
    uint32 time_interval = 0;

    if(tid == retransmit_status_tid)
    {
        retransmit_status_tid = TIMER_INVALID;
        time_transmit_count--;

        if(time_transmit_count > 0 && dev_mode == device_mode_mesh)
        {
            /* Interval at which the next status message needs to be sent */
            time_interval = 
                p_beacon_hdlr_data->beacons[p_beacon_hdlr_data->mesh_time_idx].meshtime;

            retransmit_status_tid = TimerCreate(time_interval * SECOND,
                                                TRUE, 
                                                retransmitBeaconStatusHandler);
            sendBeaconStatusForActiveBeacons();
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      checkForActiveBeacons
 *
 *  DESCRIPTION
 *      This function returns the active beacon status
 *
 *  RETURNS
 *      Returns TRUE if there is atleast one beacon whose interval is set to
 *      Non-Zero otherwise returns FALSE..
 *
 *---------------------------------------------------------------------------*/
static bool checkForActiveBeacons(void)
{
    uint8 index;

    /* check whether the beacon type is already stored */
    for (index = 0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        if(p_beacon_hdlr_data->beacons[index].beaconinterval > 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}
#endif /* APP_BEACON_MODE */

#if defined(APP_BEACON_MODE) || defined(APP_LUMICAST_MODE) 
/*----------------------------------------------------------------------------*
 *  NAME
 *      setDefaultPayload
 *
 *  DESCRIPTION
 *      This function sets the default paylod for the respective beacon type.
 *
 *  RETURNS
 *      Returns None
 *
 *---------------------------------------------------------------------------*/
static void setDefaultPayload(uint16 beacon_type, uint8 index)
{
    switch(beacon_type)
    {
        case csr_mesh_beacon_type_csr:
            p_beacon_hdlr_data->beacons[index].payloadlength 
                                        = CSR_RETAIL_BEACON_PAYLOAD_SIZE;
#ifdef APP_BEACON_MODE
            MemCopy(p_beacon_hdlr_data->beacons[index].payload,
                      &g_default_csr_beacon[0],
                      p_beacon_hdlr_data->beacons[index].payloadlength);
#endif /* APP_BEACON_MODE */
            break;

        case csr_mesh_beacon_type_ibeacon:
            p_beacon_hdlr_data->beacons[index].payloadlength 
                                        = IBEACON_PAYLOAD_SIZE;
#ifdef APP_BEACON_MODE
            MemCopy(p_beacon_hdlr_data->beacons[index].payload,
                      &g_default_ibeacon[0],
                      p_beacon_hdlr_data->beacons[index].payloadlength);
#endif /* APP_BEACON_MODE */
            break;

        case csr_mesh_beacon_type_eddystone_url:
            p_beacon_hdlr_data->beacons[index].payloadlength 
                                        = EDDYSTONE_PAYLOAD_SIZE;
#ifdef APP_BEACON_MODE
            MemCopy(p_beacon_hdlr_data->beacons[index].payload,
                      &g_default_eddystone_url[0],
                      p_beacon_hdlr_data->beacons[index].payloadlength);
#endif /* APP_BEACON_MODE */
            break;

        case csr_mesh_beacon_type_eddystone_uid:
            p_beacon_hdlr_data->beacons[index].payloadlength 
                                        = EDDYSTONE_PAYLOAD_SIZE;
#ifdef APP_BEACON_MODE
            MemCopy(p_beacon_hdlr_data->beacons[index].payload,
                      &g_default_eddystone_uid[0],
                      p_beacon_hdlr_data->beacons[index].payloadlength);
#endif /* APP_BEACON_MODE */
            break;
            
            case csr_mesh_beacon_type_lumicast:
             p_beacon_hdlr_data->beacons[index].payloadlength 
                                        = MAX_BEACON_PAYLOAD_SIZE;
            break;

        default:
            break;
    }
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      beaconModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Beacon Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult beaconModelEventHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                             CSRMESH_EVENT_DATA_T* data,
                                             CsrUint16 length,
                                             void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_BEACON_SET_STATUS:
        {
            CSRMESH_BEACON_SET_STATUS_T *p_event =
                                   (CSRMESH_BEACON_SET_STATUS_T *)(data->data);
            uint32 beacon_type_bitmask = 1 << p_event->beacontype;
            uint16 idx = 0;
#ifdef APP_BEACON_MODE
            bool old_beacon_status = checkForActiveBeacons();
#endif
#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
            if(!CheckForDeviceInterest(data->dst_id))
            break;
#endif
            MemSet(&g_model_rsp_data.beacon_status,
                   0x0000,
                   sizeof(g_model_rsp_data.beacon_status));

            /* Act on this message only if the beacon type is supported */
            if((beacon_type_bitmask & BEACON_TYPE_SUPPORTED) != 0)
            {

                /* Retrieve the beacon index where beacon needs to be stored.*/
#ifdef APP_BEACON_MODE
                idx = getBeaconStoringIndex(p_event->beacontype);
#endif
#ifdef APP_PROXY_MODE
                idx = getBeaconStoringIndexForProxy(p_event->beacontype, data->dst_id);
#endif
                if(idx != BEACON_INDEX_INVALID && 
                  ((p_event->meshinterval * MINUTE_IN_TENS_SECONDS) >= p_event->meshtime))
                {
                    /* Store mesh specific information on the globals */
                    p_beacon_hdlr_data->beacons[idx].meshinterval =
                                                        p_event->meshinterval;
                    p_beacon_hdlr_data->beacons[idx].meshtime = 
                                                        p_event->meshtime;

                    /* Store Beacon specific information on the globals */
                    p_beacon_hdlr_data->beacons[idx].txpower = 
                                                        p_event->txpower;

                    /* Store Beacon specific information on the globals */
                    p_beacon_hdlr_data->beacons[idx].beacontype = 
                                                        p_event->beacontype;
                    p_beacon_hdlr_data->beacons[idx].beaconinterval = 
                                                        p_event->beaconinterval;

                    p_beacon_hdlr_data->mesh_time_idx = idx;

#ifdef APP_BEACON_MODE
                    /* Set the received Tx power through onto mesh stack. */
                    AppSetTxPower(p_beacon_hdlr_data->beacons[idx].txpower);

                    /* If there are no active beacons move to mesh mode */
                    if((!checkForActiveBeacons()) && old_beacon_status)
                    {
                        TimerDelete(device_mode_tid);
                        device_mode_tid = TIMER_INVALID;
                        BeaconSetMode(device_mode_mesh);
                    }
                    /* Beacon interval is set so make sure you start the 
                     * timer for changing the beacon modes
                     */
                    else if((!old_beacon_status) && checkForActiveBeacons())
                    {
                        BeaconSetMode(device_mode_mesh);
                    }
#endif
                    /* Update the NVM with the payload received */
                    writeBeaconDataOnIndex(idx);
                }
                /* update the beacon status for the response message */
                updateBeaconStatus(idx);

                /* Return updated beacon state to the model */
                if (state_data != NULL)
                {
                    if(data->rx_ttl == ZERO_TTL)
                    {
                        data->rsp_ttl = ZERO_TTL;
                    }
                    *state_data = (void *)&g_model_rsp_data.beacon_status;
                }
            }
        }
        break;

        case CSRMESH_BEACON_GET_BEACON_STATUS:
        {
            CSRMESH_BEACON_GET_BEACON_STATUS_T *p_event =
                        (CSRMESH_BEACON_GET_BEACON_STATUS_T *)(data->data);

            uint32 beacon_type_bitmask = 1 << p_event->beacontype;
            MemSet(&g_model_rsp_data.beacon_status,
                   0x0000,
                   sizeof(g_model_rsp_data.beacon_status));
            uint8 idx = 0;

#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
            if(!CheckForDeviceInterest(data->dst_id))
            break;
#endif

            if((beacon_type_bitmask & BEACON_TYPE_SUPPORTED) != 0)
            {
                /* Retrieve the beacon index where beacon IS stored.*/
#ifdef APP_BEACON_MODE
                idx = getBeaconStoringIndex(p_event->beacontype);
#endif
#ifdef APP_PROXY_MODE
                idx = getBeaconIndexForProxy(p_event->beacontype, data->dst_id);
#endif
                if(idx != BEACON_INDEX_INVALID)
                {
                    /* update the beacon status based on the idx */
                    updateBeaconStatus(idx);

                    /* Return updated beacon state to the model */
                    if (state_data != NULL)
                    {
                        if(data->rx_ttl == ZERO_TTL)
                        {
                            data->rsp_ttl = ZERO_TTL;
                        }
                        *state_data = (void *)&g_model_rsp_data.beacon_status;
                    }
                }
                /* Do not send a response if the relavant beacon information
                 * is not present.
                 */
                else
                {
                    if (state_data != NULL)
                    {
                        *state_data = NULL;
                    }
                }
            }
        }
        break;

        case CSRMESH_BEACON_GET_TYPES:
        {

#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
            break;
#endif
            MemSet(&g_model_rsp_data.beacon_types,
                   0x0000,
                   sizeof(g_model_rsp_data.beacon_types));

            g_model_rsp_data.beacon_types.beacontype = BEACON_TYPE_SUPPORTED;
            g_model_rsp_data.beacon_types.batterylevel = ReadBatteryLevel();
            g_model_rsp_data.beacon_types.timesincelastmessage = 0;

            /* Return updated beacon state to the model */
            if (state_data != NULL)
            {
                if(data->rx_ttl == ZERO_TTL)
                {
                    data->rsp_ttl = ZERO_TTL;
                }
                *state_data = (void *)&g_model_rsp_data.beacon_types;
            }

        }
        break;

        case CSRMESH_BEACON_SET_PAYLOAD:
        {
            CSRMESH_BEACON_SET_PAYLOAD_T *p_event =
                                (CSRMESH_BEACON_SET_PAYLOAD_T *)(data->data);
            uint32 beacon_type_bitmask = 1 << p_event->beacontype;
            uint16 idx = 0;
#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
            if(!CheckForDeviceInterest(data->dst_id))
            break;
#endif
            if(((beacon_type_bitmask & BEACON_TYPE_SUPPORTED) != 0)
                &&(verifyPayloadLength(p_event->beacontype,(p_event->payloadoffset + p_event->payloadlength))))
            {
                /* Retrieve the beacon index where beacon needs to be stored.*/
#ifdef APP_BEACON_MODE
                idx = getBeaconStoringIndex(p_event->beacontype);
#endif
#ifdef APP_PROXY_MODE
                idx = getBeaconStoringIndexForProxy(p_event->beacontype, data->dst_id);
#endif

                /* If we have space to store and the length is valid and if 
                 * the payload id received is later version than we currently
                 * have then we store the beacon payload otherwise ignore.
                 */
                if(idx != BEACON_INDEX_INVALID)
                {
                    /* We have given a 512 window in case of payload id overflow */
                    if(p_event->payloadid > p_beacon_hdlr_data->beacons[idx].payloadid || 
                      (p_beacon_hdlr_data->beacons[idx].payloadid > 0xFF00 && p_event->payloadid < 0x00FF))
                    {
#ifdef APP_BEACON_MODE
                        bool old_beacon_status = checkForActiveBeacons();
                        MemCopy(&p_beacon_hdlr_data->beacons[idx].payload[p_event->payloadoffset],
                                &p_event->payload[0],
                                p_event->payloadlength);
#endif

#if defined(APP_LUMICAST_MODE) || defined(APP_PROXY_MODE)
                        p_beacon_hdlr_data->beacons[idx].payloadlength =  p_event->payloadlength;
                        MemCopy(&p_beacon_hdlr_data->beacons[idx].payload[0],
                                &p_event->payload[0],
                                p_event->payloadlength);
#endif
#ifdef APP_PROXY_MODE
                        p_beacon_hdlr_data->beacons[idx].payload_offset = p_event->payloadoffset;
#endif                        
                        p_beacon_hdlr_data->beacons[idx].payloadid = p_event->payloadid;

                        /* Update the NVM with the payload received */
                        writeBeaconDataOnIndex(idx);
#ifdef APP_LUMICAST_MODE
                        /* Send the received payload to lumicast library */
                        SetLumicastPayload(p_event->payload, p_event->payloadlength);
#endif

#ifdef APP_BEACON_MODE
                        /* Beacon payload is set so make sure you start the 
                         * timer for changing the beacon modes if beacons 
                         * were not active before.
                         */
                        if((!old_beacon_status) && checkForActiveBeacons())
                        {
                            BeaconSetMode(device_mode_mesh);
                        }
#endif
                    }
                    else 
                    {
                        uint8 ttl = AppGetCurrentTTL();
                        /* As payload is old, update and send beacon status */
                        updateBeaconStatus(idx);
                        if(data->rx_ttl == ZERO_TTL)
                        {
                            ttl = ZERO_TTL;
                        }

                        BeaconBeaconStatus(DEFAULT_NW_ID,
                                           data->dst_id,
                                           data->src_id,
                                           ttl,
                                           &g_model_rsp_data.beacon_status);
                    }
                }
            }
        }
        break;

        case CSRMESH_BEACON_GET_PAYLOAD:
        {
            CSRMESH_BEACON_GET_PAYLOAD_T *p_event =
                                (CSRMESH_BEACON_GET_PAYLOAD_T *)(data->data);
            CSRMESH_BEACON_SET_PAYLOAD_T p_params;
            uint32 payload_type_bitmask = 1 << p_event->payloadtype;
            uint16 idx = 0;
            MemSet(&p_params, 0, sizeof(CSRMESH_BEACON_SET_PAYLOAD_T));

#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
            if(!CheckForDeviceInterest(data->dst_id))
            break;
#endif

            /* check whether we support the specific beacon type and the payload
             * is valid, if so then send the set payload.
             */
            if((payload_type_bitmask & BEACON_TYPE_SUPPORTED) != 0)
            {
                /* Retrieve the beacon index where beacon needs to be stored.*/
#ifdef APP_BEACON_MODE
                idx = getBeaconStoringIndex(p_event->payloadtype);
#endif
#ifdef APP_PROXY_MODE
                idx = getBeaconIndexForProxy(p_event->payloadtype, data->dst_id);
#endif
                if(idx != BEACON_INDEX_INVALID)
                {
                    /* If we have the beacon type already stored and the payload
                     * id is much latest than the one present on the beacon then
                     * update the latest payload with the beacon.
                     */
                    if((p_beacon_hdlr_data->beacons[idx].beacontype == 
                                                        p_event->payloadtype)
#ifdef APP_PROXY_MODE
                    && (p_beacon_hdlr_data->beacons[idx].payloadid > 0)
#endif
                      )
                    {
                        uint8 ttl = AppGetCurrentTTL();
                        p_params.beacontype = p_event->payloadtype;
                        p_params.payloadid = p_beacon_hdlr_data->beacons[idx].payloadid;
                        p_params.payloadlength = p_beacon_hdlr_data->beacons[idx].payloadlength;
#ifdef APP_PROXY_MODE
                        p_params.payloadoffset = p_beacon_hdlr_data->beacons[idx].payload_offset;
#endif
                        MemCopy(&p_params.payload[0],
                                &p_beacon_hdlr_data->beacons[idx].payload[0],
                                p_beacon_hdlr_data->beacons[idx].payloadlength);

                        if(data->rx_ttl == ZERO_TTL)
                        {
                            ttl = ZERO_TTL;
                        }
                        BeaconSetPayload(DEFAULT_NW_ID, 
                                         data->dst_id,
                                         data->src_id,
                                         ttl,
                                         &p_params);
                    }
                }
            }
            /* As we are sending the set payload in the application, the library
             * should not send the set payload, hence state data is set to NULL.
             */
            if (state_data != NULL)
            {
                *state_data = NULL;
            }
        }
        break;

#if defined(APP_PROXY_MODE) && defined(ENABLE_BEACON_PROXY_MODEL) 
        case CSRMESH_BEACON_BEACON_STATUS:
        {
            CSRMESH_BEACON_BEACON_STATUS_T *p_event =
                                (CSRMESH_BEACON_BEACON_STATUS_T *)(data->data);
            CSRMESH_BEACON_SET_PAYLOAD_T p_params;
            uint32 beacon_type_bitmask = 1 << p_event->beacontype;
            uint16 idx = 0;
            MemSet(&p_params, 0, sizeof(CSRMESH_BEACON_SET_PAYLOAD_T));

            if(!CheckForDeviceInterest(data->src_id))
            break;

            /* check whether we support the specific beacon type and the status
             * beacon received with dest id as mesh broadcast id, this means
             * the device with the beacon is entering the mesh mode so update
             * the beacons if it has received a latest version.
             */
            if((beacon_type_bitmask & BEACON_TYPE_SUPPORTED) != 0 &&
                data->dst_id == MESH_BROADCAST_ID)
            {
                uint16 groups[MAX_MANAGED_BEACON_GRPS+1], grp_cnt=0, grp_idx=0;

                MemSet(&groups, 0, sizeof(groups));
                grp_cnt = GetBeaconGroups(data->src_id, groups);
                groups[grp_cnt++] = data->src_id;

                for(grp_idx = 0; grp_idx < grp_cnt; grp_idx++)
                {
                    idx = getBeaconIndexForProxy(p_event->beacontype, groups[grp_idx]);

                    if(idx != BEACON_INDEX_INVALID)
                    {
                        /* If we have the beacon type already stored and the payload
                         * id is much latest than the one present on the beacon then
                         * update the latest payload with the beacon.
                         */
                        CSRMESH_BEACON_SET_STATUS_T params;
                        if((p_beacon_hdlr_data->beacons[idx].payloadid > p_event->payloadid))
                        {
                            uint16 self_dev_id = 0;
                            CSR_MESH_APP_EVENT_DATA_T get_dev_id_data;
                            get_dev_id_data.appCallbackDataPtr = &self_dev_id;
                            CSRmeshGetDeviceID(CSR_MESH_DEFAULT_NETID, &get_dev_id_data);

                            p_params.beacontype = p_event->beacontype;
                            p_params.payloadid = p_beacon_hdlr_data->beacons[idx].payloadid;
                            p_params.payloadlength = p_beacon_hdlr_data->beacons[idx].payloadlength;
                            p_params.payloadoffset = p_beacon_hdlr_data->beacons[idx].payload_offset;

                            MemCopy(&p_params.payload[0],
                                    &p_beacon_hdlr_data->beacons[idx].payload[0],
                                    p_beacon_hdlr_data->beacons[idx].payloadlength);

                            BeaconSetPayload(DEFAULT_NW_ID,
                                             self_dev_id,
                                             data->src_id,
                                             ZERO_TTL,
                                             &p_params);
                        }

                        /* Send a set status message if the parameters in the 
                         * beacon have been modified from the last update
                         */
                        if((p_beacon_hdlr_data->beacons[idx].beaconinterval > 0 ||
                            p_beacon_hdlr_data->beacons[idx].meshinterval > 0 ||
                            p_beacon_hdlr_data->beacons[idx].meshtime > 0) &&
                           (p_beacon_hdlr_data->beacons[idx].beaconinterval != p_event->beaconinterval ||
                            p_beacon_hdlr_data->beacons[idx].meshinterval != p_event->meshinterval ||
                            p_beacon_hdlr_data->beacons[idx].meshtime != p_event->meshtime))
                        {
                            params.beacontype = p_beacon_hdlr_data->beacons[idx].beacontype;
                            params.beaconinterval = p_beacon_hdlr_data->beacons[idx].beaconinterval;
                            params.meshinterval = p_beacon_hdlr_data->beacons[idx].meshinterval;
                            params.meshtime = p_beacon_hdlr_data->beacons[idx].meshtime;
                            params.txpower = p_beacon_hdlr_data->beacons[idx].txpower;
                            params.tid = SET_STATUS_TID;

                            BeaconSetStatus(DEFAULT_NW_ID,
                                            data->src_id,
                                            ZERO_TTL,
                                            &params);
                        }
                        /* If the information stored is a group info, then we should not be deleting the 
                         * as some other devices might also be in the same group.
                         */
                        if(!IsAGroupDevice(groups[grp_idx]))
                        {
                            MemSet(&p_beacon_hdlr_data->beacons[idx], 0, sizeof(BEACON_INFO_T));
                            p_beacon_hdlr_data->beacons[idx].beacontype = BEACON_TYPE_INVALID;
                            /* Update the NVM as the beacon information is reset here */
                            writeBeaconDataOnIndex(idx);
                        }
                    }
                }
            }
        }
        break;
#endif

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
 *      ReadBeaconModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads beacon model data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void ReadBeaconModelDataFromNVM(void)
{
    uint8 index, index1;
    uint16 temp;

    Nvm_Read((uint16 *)&p_beacon_hdlr_data->mesh_time_idx,
             sizeof(uint16), 
             NVM_OFFSET_BEACON_MESH_TIME_IDX);

    /* Now read the individual beacon type information only if they are valid */
    for(index=0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        uint16 nvm_act_index = GET_BEACON_NVM_OFFSET(index);

        /* Read beacon type and beacon interval from NVM */
        Nvm_Read((uint16 *)&temp, sizeof(uint16), 
                 nvm_act_index + NVM_OFFSET_BEACON_TYPE_INTERVAL);

        p_beacon_hdlr_data->beacons[index].beacontype = temp & 0xFF;

        if(p_beacon_hdlr_data->beacons[index].beacontype != BEACON_TYPE_INVALID)
        {
            temp >>= 8;
            p_beacon_hdlr_data->beacons[index].beaconinterval = temp & 0xFF;

            Nvm_Read((uint16 *)&p_beacon_hdlr_data->beacons[index].meshinterval,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_BEACON_MESH_INTVL);

            Nvm_Read((uint16 *)&p_beacon_hdlr_data->beacons[index].meshtime,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_BEACON_MESH_TIME);

            Nvm_Read((uint16 *)&temp, sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_BEACON_TX_POWER_LEN);

            p_beacon_hdlr_data->beacons[index].txpower = temp & 0xFF;
            temp >>= 8;
            p_beacon_hdlr_data->beacons[index].payloadlength = temp & 0xFF;

            Nvm_Read((uint16 *)&p_beacon_hdlr_data->beacons[index].payloadid,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_BEACON_PAYLOAD_ID);

#ifdef APP_PROXY_MODE
            Nvm_Read((uint16 *)&p_beacon_hdlr_data->beacons[index].dev_id,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_BEACON_DEV_ID);
            Nvm_Read((uint16 *)&p_beacon_hdlr_data->beacons[index].payload_offset,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_BEACON_PAYLOAD_OFFSET);
#endif

            for(index1=0; 
                index1 < p_beacon_hdlr_data->beacons[index].payloadlength;
                index1 = index1+2)
            {
                /* Read beacon id and mcp packet len from NVM */
                Nvm_Read((uint16 *)&temp,
                        sizeof(uint16), 
                        nvm_act_index + NVM_OFFSET_BEACON_PKT + (index1/2));

                p_beacon_hdlr_data->beacons[index].payload[index1] = 
                                                                temp & 0xFF;
                temp >>=8;
                p_beacon_hdlr_data->beacons[index].payload[index1+1] = 
                                                                temp & 0xFF;
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteBeaconModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes beacon model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void WriteBeaconModelDataOntoNVM(void)
{
    uint8 index;
    for(index=0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        writeBeaconDataOnIndex(index);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BeaconModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Beacon model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BeaconModelHandlerInit(CsrUint8 nw_id,
                                   uint16 model_groups[],
                                   CsrUint16 num_groups)
{
    /* Initialize Beacon Model */
    BeaconModelInit(nw_id, 
                    model_groups,
                    num_groups,
                    beaconModelEventHandler);

#ifdef APP_PROXY_MODE
    BeaconModelClientInit(beaconModelEventHandler);
#endif

#ifdef APP_BEACON_MODE
    device_mode_tid = TIMER_INVALID;
    if( AppGetAssociatedState() == app_state_associated)
    {
        dev_mode = device_mode_beacon;
    }
    else
    {
        dev_mode = device_mode_mesh;
    }
    p_beacon_hdlr_data->time_remaining = 0;
    p_beacon_hdlr_data->time_wrap_cnt = 0;
    device_mode_tid = TimerCreate(100 * MILLISECOND, 
                                  TRUE, 
                                  deviceModeChangeHandler);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BeaconModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Beacon Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BeaconModelDataInit(BEACON_HANDLER_DATA_T* beacon_handler_data)
{
    uint8 index;
#if defined(APP_BEACON_MODE) || defined(APP_LUMICAST_MODE) 
    uint16 supported_type_mask = BEACON_TYPE_SUPPORTED;
    uint16 beacon_type = 0;
#endif
    
    if(beacon_handler_data != NULL)
    {
        p_beacon_hdlr_data = beacon_handler_data;
    }
#ifdef APP_BEACON_MODE
    device_mode_tid = TIMER_INVALID;
#endif
    MemSet(p_beacon_hdlr_data, 0, sizeof(BEACON_HANDLER_DATA_T));

    /* Set beacon types in the beacon info of each supported type */
    for(index=0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        p_beacon_hdlr_data->beacons[index].beacontype = BEACON_TYPE_INVALID;

#if defined(APP_BEACON_MODE) || defined(APP_LUMICAST_MODE) 
        while( beacon_type < 16)
        {
            if(supported_type_mask & (1 << beacon_type))
            {
                p_beacon_hdlr_data->beacons[index].beacontype = beacon_type;
                setDefaultPayload(beacon_type, index);
                beacon_type++;
                break;
            }
            beacon_type++;
        }
        p_beacon_hdlr_data->beacons[index].meshinterval = DEFAULT_MESH_INTERVAL;
        p_beacon_hdlr_data->beacons[index].meshtime = DEFAULT_MESH_ON_TIME;
#endif
    }
}

#ifdef APP_PROXY_MODE
/*-----------------------------------------------------------------------------*
 *  NAME
 *      GetQueuedTxMsgStats
 *
 *  DESCRIPTION
 *      This function can be called to retrieve the tx msg stats from the
 *      beacon model.
 *
 *  RETURNS/MODIFIES
 *      Tx msg stats
 *
 *----------------------------------------------------------------------------*/
extern CsrUint16 GetQueuedTxMsgStats(void)
{
    CsrUint16 idx;
    CsrUint16 tx_msgs = 0;
    
    for(idx = 0; idx < MAX_BEACONS_SUPPORTED; idx++)
    {
        if(p_beacon_hdlr_data->beacons[idx].payloadid > DEFAULT_PAYLOAD_ID && 
           p_beacon_hdlr_data->beacons[idx].beacontype != BEACON_TYPE_INVALID)
        {
            tx_msgs++;
        }
    }
    return tx_msgs;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      RemoveBeaconInfoOfDevice
 *
 *  DESCRIPTION
 *      This function removes the beacon information for the passed device id.
 *
 *  RETURNS
 *      None
 *
 *---------------------------------------------------------------------------*/
extern void RemoveBeaconInfoOfDevice(uint16 dev_id)
{
    uint8 index;

    /* check whether the type and device id match in the array */
    for (index = 0; index < MAX_BEACONS_SUPPORTED; index++)
    {
        if(p_beacon_hdlr_data->beacons[index].dev_id == dev_id)
        {
            MemSet(&p_beacon_hdlr_data->beacons[index], 0, sizeof(BEACON_INFO_T));
            p_beacon_hdlr_data->beacons[index].beacontype = BEACON_TYPE_INVALID;
            /* Update the NVM as the beacon information is reset here */
            writeBeaconDataOnIndex(index);
        }
    }
}
#endif

#ifdef APP_BEACON_MODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      BeaconSetMode
 *
 *  DESCRIPTION
 *      This function initialises the Beacon Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BeaconSetMode(device_beacon_mode_t mode)
{
    CsrUint32 time_interval;

    if(AppGetAssociatedState() == app_state_not_associated)
    {
        dev_mode = device_mode_mesh;
        CSRSchedEnableListening(TRUE);
        CSRmeshStart();
        InitiateAssociation();
    }
    else if (!checkForActiveBeacons())
    {
        dev_mode = device_mode_mesh;
        StartStopBeaconing(FALSE);
        CSRSchedEnableListening(TRUE);
        CSRmeshStart();
    }
    else
    {
        if(mode == device_mode_mesh)
        {
            dev_mode = device_mode_mesh;

            /* Mesh Time in tens of seconds */
            time_interval = 
                p_beacon_hdlr_data->beacons[p_beacon_hdlr_data->mesh_time_idx].meshtime;

            /* The set status is retransmitted 10 times within the mesh time 
             * at equal intervals of time.
             */
            time_transmit_count = RETRANSMIT_STATUS_MESH_TIME;

            StartStopBeaconing(FALSE);
            CSRSchedEnableListening(TRUE);
            CSRmeshStart();
            sendBeaconStatusForActiveBeacons();

            if(time_interval > 0)
            {
                retransmit_status_tid = TimerCreate(time_interval * SECOND,
                                                    TRUE, 
                                                    retransmitBeaconStatusHandler);
            }
        }
        else
        {
            /* Convert the time interval into 10 second intervals */
            time_interval = 
                p_beacon_hdlr_data->beacons[p_beacon_hdlr_data->mesh_time_idx].meshinterval * MINUTE_IN_TENS_SECONDS;

            /* subtract the mesh time as the mesh interval is the time between
             * the start of two mesh events.
             */
             time_interval -= p_beacon_hdlr_data->beacons[p_beacon_hdlr_data->mesh_time_idx].meshtime;

            if(time_interval > 0)
            {
                dev_mode = device_mode_beacon;
                CSRmeshStop();
                CSRSchedEnableListening(FALSE);
                StartStopBeaconing(TRUE);
            }
        }

        /* Start the timer in parts if timer is more than 30 minutes */
        if(time_interval > MAX_TIMER_VAL_IN_TENS_SECONDS)
        {
            p_beacon_hdlr_data->time_wrap_cnt = 
                                     time_interval / MAX_TIMER_VAL_IN_TENS_SECONDS;
            p_beacon_hdlr_data->time_remaining = 
                                     time_interval % MAX_TIMER_VAL_IN_TENS_SECONDS;
            /* Decrement the time wrap cnt as we are starting a timer here for 
             * time wrap seconds.
             */
            p_beacon_hdlr_data->time_wrap_cnt --;
            time_interval = MAX_TIMER_VAL_IN_TENS_SECONDS;
        }
        else
        {
            p_beacon_hdlr_data->time_wrap_cnt = 0;
            p_beacon_hdlr_data->time_remaining = 0;
        }

        if(time_interval > 0)
        {
            device_mode_tid = TimerCreate(time_interval * 10 * SECOND,
                                          TRUE, 
                                          deviceModeChangeHandler);
        }
    }
}
#endif /* APP_BEACON_MODE */
#endif /* ENABLE_BEACON_MODEL */

