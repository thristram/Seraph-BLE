/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      core_mesh_handler.c
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>
#include <mem.h>
#include <random.h>
#if defined (CSR101x_A05)
#include <config_store.h>
#include <ls_app_if.h>
#else
#include <ls_api.h>
#endif
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "iot_hw.h"
#include "appearance.h"
#include "user_config.h"
#include "nvm_access.h"
#include "app_debug.h"
#include "core_mesh_handler.h"
#include "csr_mesh_model_common.h"
#include "csr_mesh.h"
#include "app_mesh_handler.h"
#include "main_app.h"
#include "advertisement_handler.h"
#include "connection_handler.h"
#include "label.h"
#include "define.h"
#include "typedef.h"
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* Advertisement Timer for sending device identification */
#define DEVICE_ID_ADVERT_TIME     (5 * SECOND)

#define APP_SUPPORTED_BEARERS     (LE_BEARER_ACTIVE | GATT_SERVER_BEARER_ACTIVE)

#if defined(CSR101x) || defined(CSR101x_A05)
/* Tx Power level mapping.
* 0 :-18 dBm 1 :-14 dBm 2 :-10 dBm 3 :-06 dBm
* 4 :-02 dBm 5 :+02 dBm 6 :+06 dBm 7 :+08 dBm
*/
#define MIN_TX_POWER_LEVEL   (-18)
#define MAX_TX_POWER_LEVEL   (8)
#define TX_POWER_LEVEL_STEP  (4)
#else
/*
#define TX_PA_POWER_DBM_0       (-60)
#define TX_PA_POWER_DBM_1       (-30)
#define TX_PA_POWER_DBM_2       (-22)
#define TX_PA_POWER_DBM_3       (-16)
#define TX_PA_POWER_DBM_4       (-13)
#define TX_PA_POWER_DBM_5       (-10)
#define TX_PA_POWER_DBM_6       (-6)
#define TX_PA_POWER_DBM_7       (-4)
#define TX_PA_POWER_DBM_8       (-3)
#define TX_PA_POWER_DBM_9       (-2)
#define TX_PA_POWER_DBM_10      (-1)
#define TX_PA_POWER_DBM_11      (0)
#define TX_PA_POWER_DBM_12      (1)
#define TX_PA_POWER_DBM_13      (2)
#define TX_PA_POWER_DBM_14      (3)
#define TX_PA_POWER_DBM_15      (4)
*/
static const CsrInt8 fh_tx_pwr_map[] = { -60, -30, -22, -16, -13, -10, -6, -4, -3, -2,  -1, 0, 1, 2, 3, 4 };
#define MIN_TX_POWER_LEVEL   (-60)
#define MAX_TX_POWER_LEVEL   (4)
#endif


/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Association Attention timer id */
static timer_id assoc_attn_tid = TIMER_INVALID;

#if (SRC_SEQ_CACHE_SLOT_SIZE > 0)
CSR_MESH_SQN_LOOKUP_TABLE_T seqTable[SRC_SEQ_CACHE_SLOT_SIZE];

CSR_MESH_SEQ_CACHE_T        seqCache;
#endif

#ifdef ENABLE_DEVICE_UUID_ADVERTS
/* Device UUID advert timer id */
static timer_id dev_id_advert_tid = TIMER_INVALID;
#endif /* ENABLE_DEVICE_UUID_ADVERTS */

static MESH_HANDLER_DATA_T* p_mesh_hdlr_data;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      attnTimerHandler
 *
 *  DESCRIPTION
 *      This function handles Attention time-out.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void attnTimerHandler(timer_id tid)
{
    if (assoc_attn_tid == tid)
    {
        assoc_attn_tid = TIMER_INVALID;
        RestoreLightState();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      appProcessMeshEvent
 *
 *  DESCRIPTION
 *   The CSRmeshstack calls this call-back to notify asynchronous 
 *   events to applications.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void appProcessMeshEvent(CSR_MESH_APP_EVENT_DATA_T 
                                                            eventDataCallback)
{
    CSR_MESH_OPERATION_STATUS_T operation_status = eventDataCallback.status;
    CSR_MESH_EVENT_T             operation_event = eventDataCallback.event;

    /*Handling signal as per current state */
    switch(operation_status)
    {
        case CSR_MESH_OPERATION_SUCCESS:
        {
            switch(operation_event)
            {
                case CSR_MESH_ASSOC_STARTED_EVENT:
                {
                    p_mesh_hdlr_data->assoc_state 
                                                = app_state_association_started;

                    /* Delete the attention timeout timer if enabled */
                    if( assoc_attn_tid != TIMER_INVALID)
                    {
                        TimerDelete(assoc_attn_tid);
                        assoc_attn_tid = TIMER_INVALID;
                    }
                    Mesh_status = app_state_association_started;
                    /* Blink Light in Yellow to indicate association started */
                    IOTLightControlDeviceBlink(127, 127, 0, 32, 32);

                }
                break;
                case CSR_MESH_ASSOC_COMPLETE_EVENT:
                case CSR_MESH_SEND_ASSOC_COMPLETE_EVENT:
                {
                    p_mesh_hdlr_data->assoc_state = app_state_associated;

                    /* Initialize the source sequence cache */
                    AppInitializeSeqCache();

                    /* Write association state to NVM */
                    Nvm_Write((uint16 *)&p_mesh_hdlr_data->assoc_state,
                             sizeof(p_mesh_hdlr_data->assoc_state),
                             NVM_OFFSET_ASSOCIATION_STATE);

                    /* Disable promiscuous mode */
                    p_mesh_hdlr_data->bearer_tx_state.bearerPromiscuous = 0;

                    /* Save the state to NVM */
                    Nvm_Write((uint16 *)&p_mesh_hdlr_data->bearer_tx_state, 
                              sizeof(CSR_MESH_BEARER_STATE_DATA_T),
                              NVM_OFFSET_BEARER_STATE);

                    /* If the device is connected as a bridge, the stored 
                     * promiscuous settings would be assigned at the time of 
                     * disconnection.
                     */
                    
                        uint16 self_dev_id = 0;
                        CSR_MESH_APP_EVENT_DATA_T get_dev_id_data;
                        get_dev_id_data.appCallbackDataPtr = &self_dev_id;
                        CSRmeshGetDeviceID(CSR_MESH_DEFAULT_NETID, &get_dev_id_data);
                        if(self_dev_id >= 0x8000 && self_dev_id <= 0x9000)
                        Local_MESH_ID = self_dev_id;
                        Mesh_status = app_state_associated;
                        tm_100ms.tWriteFlashDelay.word = C_T_tWriteFlashDelay;
                        FlashWrite = ON;
                    if(GetConnectedDeviceId() == CM_INVALID_DEVICE_ID)
                    {
                        AppUpdateBearerState(&p_mesh_hdlr_data->bearer_tx_state);
                    }

                   /* Call this function in the application to indicate 
                    * association complete.
                    */
                    AppHandleAssociationComplete();
                }
                break;
                case CSR_MESH_CONFIG_RESET_DEVICE_EVENT:
                {
                    RemoveAssociation();
                }
                break;
                case CSR_MESH_BEARER_STATE_EVENT:
                {
                    CSR_MESH_BEARER_STATE_DATA_T bearer_state;
                    bearer_state = *((CSR_MESH_BEARER_STATE_DATA_T *)
                                        (eventDataCallback.appCallbackDataPtr));

                    /* The bearers supported by the application are only 
                     * considered here. Also the LE bearer should always be 
                     * enabled.
                     */
                    p_mesh_hdlr_data->bearer_tx_state.bearerRelayActive =
                        (bearer_state.bearerRelayActive & APP_SUPPORTED_BEARERS);
                    p_mesh_hdlr_data->bearer_tx_state.bearerEnabled =
                        (bearer_state.bearerEnabled & APP_SUPPORTED_BEARERS);
                    p_mesh_hdlr_data->bearer_tx_state.bearerPromiscuous = 
                        (bearer_state.bearerPromiscuous & APP_SUPPORTED_BEARERS);
                    p_mesh_hdlr_data->bearer_tx_state.bearerEnabled |= 
                        LE_BEARER_ACTIVE;

                    /* Save the state to NVM */
                    Nvm_Write((uint16 *)&p_mesh_hdlr_data->bearer_tx_state, 
                              sizeof(CSR_MESH_BEARER_STATE_DATA_T),
                              NVM_OFFSET_BEARER_STATE);

                    if(GetConnectedDeviceId() == CM_INVALID_DEVICE_ID)
                    {
                        /* Trigger connectable adverts. If the GATT bearer is
                         * enabled it will start advertising
                         */
                        GattTriggerConnectableAdverts(NULL);
                        AppUpdateBearerState(&p_mesh_hdlr_data->bearer_tx_state);
                    }
                }
                break;
                case CSR_MESH_CONFIG_SET_PARAMS_EVENT:
                {
                    CSR_MESH_CONFIG_BEARER_PARAM_T *config_param;

                    config_param = (CSR_MESH_CONFIG_BEARER_PARAM_T *)
                                        (eventDataCallback.appCallbackDataPtr);

                    p_mesh_hdlr_data->ttl_value = config_param->default_ttl;
                    AppSetTxPower(config_param->tx_power);

                    /* Save the ttl value onto NVM */
                    Nvm_Write((uint16 *)&p_mesh_hdlr_data->ttl_value, 
                              sizeof(p_mesh_hdlr_data->ttl_value),
                              NVM_OFFSET_TTL_VALUE);
                }
                break;

                case CSR_MESH_CONFIG_GET_PARAMS_EVENT:
                {
                    CSR_MESH_CONFIG_BEARER_PARAM_T bearer_param;

                    LsReadTransmitPowerLevel(&bearer_param.tx_power);
                    bearer_param.tx_power&= 0x00FF;
                    MemCopy(((CSR_MESH_CONFIG_BEARER_PARAM_T *)(eventDataCallback.appCallbackDataPtr)), 
                            &bearer_param, sizeof(CSR_MESH_CONFIG_BEARER_PARAM_T));
                    break;
                }

                case CSR_MESH_GROUP_SET_MODEL_GROUPID_EVENT:
                {
                    HandleGroupSetMsg(( *((CSR_MESH_GROUP_ID_RELATED_DATA_T *)
                        (eventDataCallback.appCallbackDataPtr))));
                }
                break;
                case CSR_MESH_INVALID_EVENT:
                break;
                
                case CSR_MESH_ASSOCIATION_ATTENTION_EVENT:
                {
                    CSR_MESH_ASSOCIATION_ATTENTION_DATA_T *attn_data;
        
                    attn_data = (CSR_MESH_ASSOCIATION_ATTENTION_DATA_T *)
                                      (eventDataCallback.appCallbackDataPtr);
        
                    /* Delete attention timer if it exists */
                    if (TIMER_INVALID != assoc_attn_tid)
                    {
                        TimerDelete(assoc_attn_tid);
                        assoc_attn_tid = TIMER_INVALID;
                    }
                    /* If attention Enabled */
                    if (attn_data->attract_attention)
                    {
                        /* Create attention duration timer if required */
                        if(attn_data->duration != 0xFFFF)
                        {
                            assoc_attn_tid = TimerCreate(
                                            attn_data->duration * MILLISECOND, 
                                            TRUE, attnTimerHandler);
                        }
                        /* Enable Green light blinking to attract attention */
                        IOTLightControlDeviceBlink(0, 127, 0, 16, 16);
                    }
                    else
                    {
                        if(p_mesh_hdlr_data->assoc_state 
                                                == app_state_not_associated)
                        {
                            /* Blink blue to indicate not associated status */
                            IOTLightControlDeviceBlink(0, 0, 127, 32, 32);
                        }
                        else
                        {
                            /* Restore Light State */
                            RestoreLightState();
                        }
                    }
                }
                break;
                default:
                break;
            }
        }
        break;
        case CSR_MESH_OPERATION_STACK_NOT_INITIALIZED:
        break;
        case CSR_MESH_OPERATION_NOT_PERMITTED:
        break;
        case CSR_MESH_OPERATION_MEMORY_FULL:
        break;
        case CSR_MESH_OPERATION_GENERIC_FAIL:
        {
            switch(operation_event)
            {
                case CSR_MESH_ASSOC_COMPLETE_EVENT:
                case CSR_MESH_SEND_ASSOC_COMPLETE_EVENT:
                {
                    DEBUG_STR("\r\n Association failed !!! \r\n");

                    /* Initiate association once again */
                    p_mesh_hdlr_data->assoc_state = app_state_not_associated;
                    InitiateAssociation();

                    /* Write association state to NVM */
                    Nvm_Write((uint16 *)&p_mesh_hdlr_data->assoc_state,
                             sizeof(p_mesh_hdlr_data->assoc_state),
                             NVM_OFFSET_ASSOCIATION_STATE);
                }
                break;
                default:
                break;
            }
        }
        break;
        case CSR_MESH_OPERATION_REQUEST_FOR_INFO:
        {
            switch(operation_event)
            {
                case CSR_MESH_GET_VID_PID_VERSTION_EVENT:
                {
                    MemCopy(((CSR_MESH_VID_PID_VERSION_T *)
                            (eventDataCallback.appCallbackDataPtr)), 
                            p_mesh_hdlr_data->vid_pid_info,
                            sizeof(CSR_MESH_VID_PID_VERSION_T));
                }
                break;
                case CSR_MESH_GET_DEVICE_APPEARANCE_EVENT:
                {
                    MemCopy(((CSR_MESH_DEVICE_APPEARANCE_T *)
                                    (eventDataCallback.appCallbackDataPtr)), 
                                    p_mesh_hdlr_data->appearance,
                                    sizeof(CSR_MESH_DEVICE_APPEARANCE_T));
                }
                break;

                default:
                break;
            }
        }
        break;

        default:
        break;
    }
}

 #ifdef ENABLE_DEVICE_UUID_ADVERTS
/*-----------------------------------------------------------------------------*
 *  NAME
 *      deviceIdAdvertTimeoutHandler
 *
 *  DESCRIPTION
 *      This function handles the Device ID advertise timer event.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void deviceIdAdvertTimeoutHandler(timer_id tid)
{

    if(tid == dev_id_advert_tid)
    {
        if(p_mesh_hdlr_data->assoc_state == app_state_not_associated)
          {
            uint32 random_delay = ((uint32)(Random16() & 0x1FF))*(MILLISECOND);
          /* Generate a random delay between 0 to 511 ms */
            //DEBUG_STR("CSRMeshAssociateToANetwork  API is called---> \r\n");
            Mesh_status = app_state_not_associated;
            if(Local_MESH_ID != NULL)
            {
                 Local_MESH_ID = CLEAR;  
                 tm_100ms.tWriteFlashDelay.word = C_T_tWriteFlashDelay;
                 FlashWrite = ON;
            }
            CSRmeshAssociateToANetwork(p_mesh_hdlr_data->appearance , 10);
            /*add by cdy 2017/2/4  */  
            if(tm_1s.tAdvUUID3Min.fov == ON)
            {
                 TimerDelete(dev_id_advert_tid);  
                 dev_id_advert_tid = TIMER_INVALID;
            }
            else
            {
                 dev_id_advert_tid = TimerCreate(
                                     (DEVICE_ID_ADVERT_TIME + random_delay),
                                      TRUE,
                                      deviceIdAdvertTimeoutHandler);      
            }

        }
        else
        {
            TimerDelete(dev_id_advert_tid);
            dev_id_advert_tid = TIMER_INVALID;
        }
    }
}
#endif /* ENABLE_DEVICE_UUID_ADVERTS */

/*============================================================================*
 *  Public Function Implemtations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      InitiateAssociation
 *
 *  DESCRIPTION
 *      This function kick starts a timer to send periodic CSRmesh UUID Messages
 *      and starts blinking Blue LED to visually indicate association ready 
 *      status
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void InitiateAssociation(void)
{
    /* Blink light to indicate that it is not associated */
    IOTLightControlDeviceBlink(0, 0, 127, 32, 32);
    
#ifdef ENABLE_DEVICE_UUID_ADVERTS
    tm_1s.tAdvUUID3Min.word = C_T_tAdvUUID3Min;/*add by cdy 2017/2/4  */  
    CSRmeshAssociateToANetwork(p_mesh_hdlr_data->appearance , 10);
    /* Restart the timer to send Device ID messages periodically to get
     * associated to a network*/
     
    TimerDelete(dev_id_advert_tid);
    dev_id_advert_tid = TIMER_INVALID;
    dev_id_advert_tid = TimerCreate(DEVICE_ID_ADVERT_TIME,
                                    TRUE,
                                    deviceIdAdvertTimeoutHandler);
                                    
#endif /* ENABLE_DEVICE_UUID_ADVERTS */
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppUpdateBearerState
 *
 *  DESCRIPTION
 *      This function updates the relay and promiscuous mode of the GATT and
 *      and the LE Advert bearers
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppUpdateBearerState(CSR_MESH_TRANSMIT_STATE_T *p_bearer_state)
{
    CSR_MESH_APP_EVENT_DATA_T ret_evt_data;
    CSR_MESH_TRANSMIT_STATE_T ret_bearer_state;

    /* The MASP relay enable is always set to TRUE. Also the relay for the 
     * default NETID is also enabled by default.
     */
    p_bearer_state->maspRelayEnable = TRUE;
    p_bearer_state->relay.enable = TRUE;
    p_bearer_state->relay.netId = CSR_MESH_DEFAULT_NETID;

    /* Update the bearer state here */
    ret_evt_data.appCallbackDataPtr = &ret_bearer_state;
    CSRmeshSetTransmitState(p_bearer_state, &ret_evt_data);
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppGetAssociatedState
 *
 *  DESCRIPTION
 *      This function returns the association state.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern app_association_state AppGetAssociatedState(void)
{
    return p_mesh_hdlr_data->assoc_state;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      IsGattBearerEnabled
 *
 *  DESCRIPTION
 *      This function returns the status of the GATT bearer.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern bool IsGattBearerEnabled(void)
{
    return p_mesh_hdlr_data->bearer_tx_state.bearerEnabled &
                                                     GATT_SERVER_BEARER_ACTIVE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppGetStoredBearerState
 *
 *  DESCRIPTION
 *      This function returns the stored bearer state.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern CSR_MESH_TRANSMIT_STATE_T AppGetStoredBearerState(void)
{
    return p_mesh_hdlr_data->bearer_tx_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      MeshHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Association handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void MeshHandlerInit(MESH_HANDLER_DATA_T* mesh_handler_data)
{
    CSRmeshRegisterAppCallback(appProcessMeshEvent);
    p_mesh_hdlr_data = mesh_handler_data;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppGetCurrentTTL
 *
 *  DESCRIPTION
 *      This function is called to retrieve the current TTL value in the 
 *      application.
 *
 *  RETURNS
 *      TTL value stored in the application.
 *
 *---------------------------------------------------------------------------*/
extern uint8 AppGetCurrentTTL(void)
{
    return p_mesh_hdlr_data->ttl_value;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppSetTxPower
 *
 *  DESCRIPTION
 *      Sets the Transmit Power to the nearest value possible.
 *
 *  PARAMETERS
 *      power Transmit power level in dBm.
 *
 *  RETURNS/MODIFIES
 *      Set power level in dBm
 *----------------------------------------------------------------------------*/
extern void AppSetTxPower(CsrInt8 power)
{
    CsrUint8 level = 0;

    if (power & 0x80)
    {
        power |= 0xFF00;
    }
    /* Map Power level in dBm to index */
    if (power <= MIN_TX_POWER_LEVEL)
    {
        level = LS_MIN_TRANSMIT_POWER_LEVEL;
    }
    else if (power >= MAX_TX_POWER_LEVEL)
    {
        level = LS_MAX_TRANSMIT_POWER_LEVEL;
    }
    else
    {
#if defined(CSR102x) || defined(CSR102x_A05)
        for (level = 0; level < (sizeof(fh_tx_pwr_map) - 1); level++)
        {
              if ((power >= fh_tx_pwr_map[level]) && (power < fh_tx_pwr_map[level + 1]))
              {
                    break;
              }
        }
#else
        level = (power - MIN_TX_POWER_LEVEL) / 
TX_POWER_LEVEL_STEP;
#endif
    }
    CsrSchedSetTxPower(level);
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppInitializeSeqCache
 *
 *  DESCRIPTION
 *      This function initialises the sequence cahce.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppInitializeSeqCache(void)
{
#if (SRC_SEQ_CACHE_SLOT_SIZE > 0)
    MemSet(seqTable, 0x00, SRC_SEQ_CACHE_SLOT_SIZE * sizeof(CSR_MESH_SQN_LOOKUP_TABLE_T));

    seqCache.cached_dev_count = SRC_SEQ_CACHE_SLOT_SIZE;
    seqCache.seq_deviation    = 0x20;
    seqCache.seq_lookup_table = seqTable;
    CSRmeshSetSrcSequenceCache(CSR_MESH_DEFAULT_NETID, &seqCache);
#endif
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppClearSeqCache
 *
 *  DESCRIPTION
 *      This function clears the sequence cahce.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppClearSeqCache(void)
{
#if (SRC_SEQ_CACHE_SLOT_SIZE > 0)
    /* Clear the source sequence cache */
    MemSet(seqTable, 0x00, SRC_SEQ_CACHE_SLOT_SIZE * sizeof(CSR_MESH_SQN_LOOKUP_TABLE_T));
    CSRmeshSetSrcSequenceCache(CSR_MESH_DEFAULT_NETID, NULL);
#endif
}

