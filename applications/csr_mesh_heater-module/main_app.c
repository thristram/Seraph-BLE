/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      main_app.c
 *
 *  DESCRIPTION
 *      This file implements the CSR Mesh application.
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#ifdef CSR101x_A05
#include <ls_app_if.h>
#include <config_store.h>
#include <csr_ota.h>
#else
#include <uart.h>           /* Functions to interface with the UART */
#include <uart_sdk.h>       /* Enums to interface with the UART */
#include <configstore_api.h>
#include <configstore_id.h> 
#include <mem.h>
#endif /* !CSR101x_A05 */
#include <timer.h>
#include <uart.h>
#include <pio.h>
#include <nvm.h>
#include <debug.h>
#include <panic.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "data_model_handler.h"
#include "app_debug.h"
#include "iot_hw.h"
#include "battery_hw.h"
#include "nvm_access.h"
#include "main_app.h"
#include "gap_service.h"
#include "gatt_service.h"
#include "mesh_control_service.h"
#include "advertisement_handler.h"
#include "connection_handler.h"
#include "mesh_control_service.h"
#include "conn_param_update.h"
#if !defined(CSR101x_A05)
#include "gaia_service.h"
#ifdef GAIA_OTAU_SUPPORT
#include "gaia_otau_api.h"
#include "app_otau_handler.h"
#endif /* GAIA_OTAU_SUPPORT  */
#else
#include "csr_ota_service.h"
#endif /* (!CSR101x_A05) */

#include "label.h"
#include "define.h"
#include "app_mesh_handler.h"
#include "iot_hw.h"
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#ifndef CSR101x_A05
/* Standard setup of CSR102x boards */
#define UART_PIO_TX                    (8)
#define UART_PIO_RX                    (9)
#define UART_PIO_RTS                   (PIO_NONE)
#define UART_PIO_CTS                   (PIO_NONE)
#define USER_KEY1                      (69)
#else
/* The User key index where the application config flags are stored */
#define CSKEY_INDEX_USER_FLAGS         (0)
#endif /* !CSR101x_A05 */


/*add by cdy 2016-12-30*/
timer_id t_250us_id;
timer_id t_10ms_id;
uint8 TIME10MS = 0;
#define TIME1MS (1*MILLISECOND)

/*extern void startStream(uint16 dest_id);*/
void time250ushandle(timer_id tid);
void time1mshandle(timer_id tid);
/*============================================================================*
 *  Private Data
 *===========================================================================*/
/* Declare space for application timers. */
static uint16 app_timers[SIZEOF_APP_TIMER * MAX_APP_TIMERS];

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
/* UART Receive callback */
/*
#if defined(CSR101x_A05) && defined(DEBUG_ENABLE)
static uint16 UartDataRxCallback ( void* p_data, uint16 data_count,
                                   uint16* p_num_additional_words );
#endif *//* defined(CSR101x_A05) && defined(DEBUG_ENABLE) */

#ifndef CSR101x_A05

/* Cached Value of UUID. */
uint16 cached_uuid[UUID_LENGTH_WORDS];

/* Cached Value of Authorization Code. */
#ifdef USE_AUTHORISATION_CODE
uint16 cached_auth_code[AUTH_CODE_LENGTH_WORDS];
#endif /* USE_AUTHORISATION_CODE */

#endif /* !CSR101x_A05 */

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
/*-----------------------------------------------------------------------------*
 *  NAME
 *      UartDataRxCallback
 *
 *  DESCRIPTION
 *      This callback is issued when data is received over UART. Application
 *      may ignore the data, if not required. For more information refer to
 *      the API documentation for the type "uart_data_out_fn"
 *
 *  RETURNS
 *      The number of words processed, return data_count if all of the received
 *      data had been processed (or if application don't care about the data)
 *
 *----------------------------------------------------------------------------*/
/*
#if defined(CSR101x_A05) && defined(DEBUG_ENABLE)
static uint16 UartDataRxCallback ( void* p_data, uint16 data_count,
        uint16* p_num_additional_words )
{
    
    *p_num_additional_words = 1;

    return data_count;
}
#endif *//* defined(CSR101x_A05) && defined(DEBUG_ENABLE) */

#ifndef CSR101x_A05

/*----------------------------------------------------------------------------*
 *  NAME
 *      configStoreProcessEvent
 *
 *  DESCRIPTION
 *      This function handles the config store messages
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/



static void configStoreProcessEvent(msg_t *msg)
{
    configstore_msg_t *cs_msg = (configstore_msg_t*)msg;
    switch(msg->header.id)
    {
        case CONFIG_STORE_READ_KEY_CFM:
        {
            configstore_read_key_cfm_t *read_key_cfm =
                    (configstore_read_key_cfm_t*)&cs_msg->body;

            if (STATUS_SUCCESS == read_key_cfm->status)
            {
                if ((USER_KEY1 == read_key_cfm->id) && (0 != read_key_cfm->value_len))
                {
                    /* Copy flags. */
                    g_cskey_flags = read_key_cfm->value[0];

                    /* Copy UUID if Random UUID is not used. */
                    if (0 == (g_cskey_flags & CSKEY_RANDOM_UUID_ENABLE_BIT))
                    {
                        if (read_key_cfm->value_len >= (UUID_LENGTH_WORDS + 1))
                        {
                            MemCopy(cached_uuid, &read_key_cfm->value[1],
                                    UUID_LENGTH_WORDS);
                        }

                        /* Copy Authorization Code if enabled. */
#ifdef USE_AUTHORISATION_CODE
                        if (read_key_cfm->value_len >= (UUID_LENGTH_WORDS + AUTH_CODE_LENGTH_WORDS + 1))
                        {
                            MemCopy(cached_auth_code, &read_key_cfm->value[1 + UUID_LENGTH_WORDS],
                                    AUTH_CODE_LENGTH_WORDS);
                        }
#endif /* USE_AUTHORISATION_CODE */
                    }
                    /* Initialize the nvm offset to max words taken by application. 
                     * The CM is stores its data after this.
                     */
                    g_app_nvm_offset = NVM_OFFSET_SANITY_WORD;
                    
                    /* Initialise the NVM. AppNvmReady is called when the NVM
                     * initialisation complete.
                     */
                    Nvm_Init(NVM_ID,NVM_SANITY_MAGIC, &g_app_nvm_offset);
                }
                else if(CS_ID_APP_STORE_ID == read_key_cfm->id)
                {
#ifdef GAIA_OTAU_SUPPORT
                    GaiaOtauConfigStoreMsg(msg);
#endif /* GAIA_OTAU_SUPPORT */
                }
            }
        }
        break;
        default:
        break;
    }
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      readUserKeys
 *
 *  DESCRIPTION
 *      This function reads the user keys from config file
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void readUserKeys(void)
{
#ifndef CSR101x_A05
    /* Read payload filler from User key index1 */
    ConfigStoreReadKey(USER_KEY1 ,STORE_ID_UNUSED);
#else
    g_cskey_flags = CSReadUserKey(CSKEY_INDEX_USER_FLAGS);

    /* Initialize the nvm offset to max words taken by application. The CM is
     * stores its data after this.
     */
    g_app_nvm_offset = NVM_OFFSET_SANITY_WORD;

    /* Initialise the NVM. AppNvmReady is called when the NVM initialisation
     * complete.
     */
    Nvm_Init(NVM_SANITY_MAGIC, &g_app_nvm_offset);
#endif
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      InitialiseAppSuppServices
 *
 *  DESCRIPTION
 *      This function initialises the application supported services.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void InitAppSupportedServices(void)
{
#ifndef CSR101x_A05
    g_app_nvm_offset = NVM_OFFSET_GAIA_OTA_SERVICE;
    /* Initialize the Gaia OTA service. */
    GAIAInitServerService(g_app_nvm_fresh, &g_app_nvm_offset);
#ifdef GAIA_OTAU_SUPPORT
    AppGaiaOtauInit();
#endif /* GAIA_OTAU_SUPPORT */
    g_app_nvm_offset = NVM_OFFSET_MESH_APP_SERVICES;
#else
#ifdef OTAU_BOOTLOADER
    /* Initialize the OTA service. */
    OtaInitServerService(g_app_nvm_fresh, &g_app_nvm_offset);
#endif
#endif
    
    /* Initialize the GATT service. */
    GattExtInitServerService(TRUE, &g_app_nvm_offset, TRUE);

    /* Initialize the GAP service. */
    GapInitServerService(TRUE, &g_app_nvm_offset);

    /* Initialize the Mesh control service */
    MeshControlInitServerService(TRUE, &g_app_nvm_offset);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppDataInit
 *
 *  DESCRIPTION
 *      This function is called to initialise CSRmesh application
 *      data structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AppDataInit(void)
{
    /* Stop the connection parameter update if its in progress */
    StopConnParamsUpdate();

    /* Initialises the GATT Data */
    InitialiseGattData();
     
    /* Initialize the Mesh Control Service Data Structure */
    MeshControlServiceDataInit();
}
void time250ushandle(timer_id tid)
{
       timer250us++;
       if(timer250us >= 4)
       {
          timer250us = 0;
          timer_RBINT();
          
       }
       TimerCreate(TIME250US, TRUE, time250ushandle);
}

void time1mshandle(timer_id tid)
{
     TIME10MS++;
     if(tm_100ms.tmeshfinishdataWait100ms.fov == ON || tm_1s.tRxMeshTimeOut2s.fov == ON)
     {
          tm_100ms.tmeshfinishdataWait100ms.word = CLEAR;
          tm_1s.tRxMeshTimeOut2s.word = CLEAR;
          f_Block_Buffer_Empty = ON;
     }
     processuartdata();
     if(TIME10MS == 10)
     {
          TIME10MS = CLEAR;
          if(f_Mesh_Tx_Ready == ON && (f_Mesh_First_Send == OFF ||tm_100ms.tmfTxdataWait100ms.fov == ON || tm_1s.tMeshTimeOut2s.fov == ON))
          {
               f_Mesh_Tx_Ready = OFF;
               f_Mesh_First_Send = ON;
               f_meshrxdataOK = OFF;/*前一包数据发送完成标志*/
               tm_100ms.tmfTxdataWait100ms.word = CLEAR;
               tm_1s.tMeshTimeOut2s.word = CLEAR;
               StartBlockSendData(TX_MESH_ID);
               /*startStream(TX_MESH_ID);*/
          }
          flash_run();
     }
     
     TimerCreate(TIME1MS, TRUE, time1mshandle);
}
void srf_init(void)
{
     f_Mesh_First_Send = CLEAR;
     g_trigger_write_callback = FALSE;
     tm_1ms.tUartWait10ms.word = C_T_tUartWait10ms;
     tm_1s.tPanic2Min.word = C_T_tPanic2Min;
     f_Block_Buffer_Empty = ON;
     FlashRead = ON;
     tm_1s.tHeartPack5s.word = 0x4000+3;
     tm_1s.tPoweronWait3s.word = 0x4000+3;
     
}
void Reset_BLE_Module(void)
{
     f_Ble_Reset = OFF;
     tm_1s.tBleReset3Min.word = C_T_tBleReset3Min;
     RemoveAssociation();     
     Panic(1);    
}
/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppInit
 *
 *  DESCRIPTION
 *      This user application function is called after a power-on reset
 *      (including after a firmware panic), after a wakeup from Hibernate or
 *      Dormant sleep states, or after an HCI Reset has been requested.
 *
 *      The last sleep state is provided to the application in the parameter.
 *
 *      NOTE: In the case of a power-on reset, this function is called
 *      after app_power_on_reset().
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
void AppInit(sleep_state last_sleep_state)
{
    /* Initialise the application timers */
    /*add by cdy 2016-12-30*/
    srf_init();
    /*PioSetMode_Bottun();*/
    /*****************************************/
    TimerInit(MAX_APP_TIMERS, (void*)app_timers);
    /*add by cdy 2016-12-30*/
    t_250us_id = TimerCreate(TIME250US, TRUE, time250ushandle);
    t_10ms_id = TimerCreate(TIME1MS, TRUE, time1mshandle);
    
    /*****************************************/
#ifdef DEBUG_ENABLE
    /*DebugInit(0, NULL, NULL);*/
    DebugInit(1, NULL, NULL);
    UartConfig(0x0014,7);
    UartEnable(TRUE);
    UartRead(1, 0);
    DebugWriteString("\r\n start \r\n");
#endif /* DEBUG_ENABLE */
#ifdef UART_BOARD_ENABLE    
	InitUart();
#endif
    /* Initialise Light and turn it off. */
    IOTLightControlDeviceInit();
    IOTLightControlDevicePower(FALSE);
    /* Read the user keys */
    readUserKeys();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppNvmReady
 *
 *  DESCRIPTION
 *      This function is called when the NVM is initialised
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AppNvmReady(bool nvm_fresh, uint16 nvm_offset)
{
    /* Save the NVM data */
    g_app_nvm_fresh = nvm_fresh;
#ifdef CSR101x_A05
    g_app_nvm_offset = NVM_MAX_APP_MEMORY_WORDS;
#else
    g_app_nvm_offset = NVM_OFFSET_CM_INITIALISATION;
#endif

    /* Initilize CM */
    AppCMInit(g_app_nvm_fresh, &g_app_nvm_offset);
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppProcesSystemEvent
 *
 *  DESCRIPTION
 *      This user application function is called whenever a system event, such
 *      as a battery low notification, is received by the system.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void AppProcessSystemEvent(sys_event_id id, void *data)
{
    switch (id)
    {
        case sys_event_pio_changed:
        {
              /*HandlePIOChangedEvent((pio_changed_data*)data);*/
        }
        break;

        case sys_event_battery_low:
        {
            /* Battery low event received - Broadcast the battery state to the
             * network.
             */
            SendLowBatteryIndication();
        }
        break;

        default:
        break;
    }
}

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      AppProcessEvent
 *
 *  DESCRIPTION
 *      Handles the system events
 *
 *  RETURNS
 *     status_t: STATUS_SUCCESS if successful
 *
 *----------------------------------------------------------------------------*/

status_t AppProcessEvent(msg_t *msg)
{
    switch(GET_GROUP_ID(msg->header.id))
    {
        case SM_GROUP_ID:
        case LS_GROUP_ID:
        case GATT_GROUP_ID:
        {
            /* CM Handles the SM, LS and GATT Messages */
            CMProcessMsg(msg);
        }
        break;

        case USER_STORE_GROUP_ID:
        {
            NvmProcessEvent(msg);
        }
        break;

        case CONFIG_STORE_GROUP_ID:
        {
            configStoreProcessEvent(msg);
        }
        break;
        
#ifdef GAIA_OTAU_SUPPORT
        case STORE_UPDATE_GROUP_ID:
        GaiaOtauHandleStoreUpdateMsg(GetConnectedDeviceId(), (store_update_msg_t*)msg );
        break;
#endif

        default:
        break;
    }    
    return STATUS_SUCCESS;
}

#else
/*----------------------------------------------------------------------------*
 *  NAME
 *      AppProcessLmEvent
 *
 *  DESCRIPTION
 *      This user application function is called whenever a LM-specific event is
 *      received by the system.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/

extern bool AppProcessLmEvent(lm_event_code event_code,
                              LM_EVENT_T *p_event_data)
{
    /* CM Handles the SM, LS and GATT Messages */
    CMProcessMsg(event_code, p_event_data);
    return TRUE;
}

/*----------------------------------------------------------------------------*
 * NAME
 *   AppPowerOnReset
 *
 * DESCRIPTION
 *   This user application function is called just after a power-on reset
 *   (including after a firmware panic), or after a wakeup from Hibernate or
 *   Dormant sleep states.
 *
 *   At the time this function is called, the last sleep state is not yet
 *   known.
 *
 *   NOTE: this function should only contain code to be executed after a
 *   power-on reset or panic. Code that should also be executed after an
 *   HCI_RESET should instead be placed in the AppInit() function.
 *
 * RETURNS
 *   Nothing
 *
 *----------------------------------------------------------------------------*/
void AppPowerOnReset(void)
{
     /* Code that is only executed after a power-on reset or firmware panic 
      * should be implemented here.
      */
}
#endif
