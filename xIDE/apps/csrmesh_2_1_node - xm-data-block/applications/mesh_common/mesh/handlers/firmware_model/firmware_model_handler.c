/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      firmware_model_handler.c
 *
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#ifdef CSR101x_A05
#include <csr_ota.h>
#include <timer.h>
#include "app_debug.h"
#include "advertisement_handler.h"
#endif

#include "user_config.h"
#include "firmware_model_handler.h"
#include "app_mesh_handler.h"
#include "firmware_server.h"

#ifdef ENABLE_FIRMWARE_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/*============================================================================*
 *  Private Data
 *============================================================================*/
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
/* OTA Reset Defer Duration */
#define OTA_RESET_DEFER_DURATION                  (500 * MILLISECOND)

static timer_id                                    ota_rst_tid;
#endif

/* Pointer to firmware handler data */
static FIRMWARE_HANDLER_DATA_T*                   p_fw_hdlr_data;
/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
/*-----------------------------------------------------------------------------*
 *  NAME
 *      issueOTAReset
 *
 *  DESCRIPTION
 *      This function issues an OTA Reset.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 
*----------------------------------------------------------------------------*/
static void issueOTAReset(timer_id tid)
{
    if (ota_rst_tid == tid)
    {
        ota_rst_tid = TIMER_INVALID;

        /* Issue OTA Reset. */
        OtaReset();
    }
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      firmwareModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Firmware Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult firmwareModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                               CSRMESH_EVENT_DATA_T* data,
                                               CsrUint16 length,
                                               void **state_data)
{

    switch(event_code)
    {
        case CSRMESH_FIRMWARE_GET_VERSION:
        {
            /* Send Firmware Version data to the model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_fw_hdlr_data->fw_version;
            }
        }
        break;

        case CSRMESH_FIRMWARE_UPDATE_REQUIRED:
        {
#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)
            if(ota_rst_tid == TIMER_INVALID)
            {
                BD_ADDR_T *pBDAddr = NULL;
#ifdef USE_STATIC_RANDOM_ADDRESS
                GattGetRandomAddress(pBDAddr); 
#endif /* USE_STATIC_RANDOM_ADDRESS */

                DEBUG_STR("\r\n FIRMWARE UPDATE IN PROGRESS \r\n");

                /* Write the value CSR_OTA_BOOT_LOADER to NVM so that
                 * it starts in OTA mode upon reset
                 */
                OtaWriteCurrentApp(csr_ota_boot_loader,
                                   FALSE,   /* is bonded */
                                   NULL,    /* Typed host BD Address */
                                   0,       /* Diversifier */
                                   pBDAddr, /* local_random_address */
                                   NULL,    /* irk */
                                   FALSE    /* service_changed_config */
                                  );

               /* Defer OTA Reset for half a second to ensure that,
                * acknowledgements are sent before reset.
                */
               ota_rst_tid = TimerCreate(OTA_RESET_DEFER_DURATION, TRUE,
                                         issueOTAReset);
            }
#endif
            /* Send Firmware Version data to the model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_fw_hdlr_data->fw_version;
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
 *      FirmwareModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the firmware model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void FirmwareModelHandlerInit(CsrUint8 nw_id,
                                     CsrUint16 fw_model_grps[],
                                     CsrUint16 num_groups)
{
    /* Initialize firmware Model */
    FirmwareModelInit(nw_id, 
                      fw_model_grps,
                      num_groups,
                      firmwareModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      FirmwareModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Firmware Model data.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void FirmwareModelDataInit(FIRMWARE_HANDLER_DATA_T* fw_handler_data)
{
    if(fw_handler_data != NULL)
    {
        p_fw_hdlr_data = fw_handler_data;
    }
}

#endif /* ENABLE_FIRMWARE_MODEL */

