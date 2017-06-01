/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      attention_model_handler.c
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "iot_hw.h"
#include "user_config.h"
#include "attention_server.h"
#include "app_mesh_handler.h"
#include "attention_model_handler.h"

#ifdef ENABLE_ATTENTION_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/*============================================================================*
 *  Private Data
 *============================================================================*/
 
/* Attention timer id */
static timer_id attn_tid = TIMER_INVALID;

/* Pointer to model handler Data */
static ATTENTION_HANDLER_DATA_T* p_attn_model_hdlr_data;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
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
    if (attn_tid == tid)
    {
        attn_tid = TIMER_INVALID;
        RestoreLightState();

#ifdef ENABLE_DUTY_CYCLE_CHANGE_SUPPORT
        /* Set back the scan to low duty cycle only if the device has
         * already been grouped.
         */
        EnableHighDutyScanMode(FALSE);
#endif /* ENABLE_DUTY_CYCLE_CHANGE_SUPPORT */
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      attentionModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Attention Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult attentionModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                                CSRMESH_EVENT_DATA_T* data,
                                                CsrUint16 length,
                                                void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_ATTENTION_SET_STATE:
        {
            CSRMESH_ATTENTION_SET_STATE_T *p_event = 
                 (CSRMESH_ATTENTION_SET_STATE_T *)data->data;

            p_attn_model_hdlr_data->attn_model.attractattention =
                                                    p_event->attractattention;
            p_attn_model_hdlr_data->attn_model.duration = p_event->duration;
            p_attn_model_hdlr_data->attn_model.tid = p_event->tid;

            /* Delete attention timer if it exists */
            if (TIMER_INVALID != attn_tid)
            {
                TimerDelete(attn_tid);
                attn_tid = TIMER_INVALID;
            }

            /* If attention Enabled */
            if (p_event->attractattention)
            {
                /* Create attention duration timer if required */
                if (p_event->duration != 0xFFFF)
                {
                    attn_tid = 
                        TimerCreate((uint32)p_event->duration * MILLISECOND,
                                                    TRUE, attnTimerHandler);
                }

#ifdef ENABLE_DUTY_CYCLE_CHANGE_SUPPORT
                /* Change the Rx scan duty cycle on enabling attention */
                EnableHighDutyScanMode(TRUE);
#endif /* ENABLE_DUTY_CYCLE_CHANGE_SUPPORT */

                /* Enable Red light blinking to attract attention */
                IOTLightControlDeviceBlink(127, 0, 0, 32, 32);
            }
            else
            {
#ifdef ENABLE_DUTY_CYCLE_CHANGE_SUPPORT
                /* Set back the scan to low duty cycle only if the device has
                 * already been grouped.
                 */
                EnableHighDutyScanMode(FALSE);
#endif /* ENABLE_DUTY_CYCLE_CHANGE_SUPPORT */

                /* Restore Light State */
                RestoreLightState();
            }

            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_attn_model_hdlr_data->attn_model;
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
 *      AttentionModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Attention model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AttentionModelHandlerInit(CsrUint8 nw_id,
                                      uint16 model_groups[],
                                      CsrUint16 num_groups)
{
    /* Initialize Attention Model */
    AttentionModelInit(nw_id, 
                       model_groups,
                       num_groups,
                       attentionModelEventHandler);

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AttentionModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Attention Model data on the global 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void AttentionModelDataInit(ATTENTION_HANDLER_DATA_T* attn_handler_data)
{
    if(attn_handler_data != NULL)
    {
        p_attn_model_hdlr_data = attn_handler_data;
    }

    /* Delete attention timer if it exists */
    if (TIMER_INVALID != attn_tid)
    {
        TimerDelete(attn_tid);
        attn_tid = TIMER_INVALID;
    }
}
#endif /* ENABLE_ATTENTION_MODEL */


