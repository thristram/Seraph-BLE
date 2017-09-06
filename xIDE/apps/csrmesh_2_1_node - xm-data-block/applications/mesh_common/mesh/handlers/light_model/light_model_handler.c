/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      light_model_handler.c
 *
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
#include "main_app.h"
#include "light_server.h"
#include "light_model_handler.h"
#include "app_hw.h"
#include "core_mesh_handler.h"

#ifdef ENABLE_POWER_MODEL
#include "power_model_handler.h"
#endif

#ifdef ENABLE_LIGHT_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* NVM Data Write defer Duration */
#define NVM_WRITE_DEFER_DURATION       (5 * SECOND)

#define NUM_TRANSITION_STEPS           (100)

/* supported transition states */
typedef enum
{
    state_idle,
    state_color_change_attacking,
    state_level_change_attacking,
    state_white_level_change_attacking,
    state_sustaining,
    state_temp_change_attacking,
    state_decaying
}transition_sd_state;

/* Transition data stored for transition across different states */
typedef struct
{
    CsrUint32                   delta_duration;
    CsrUint32                   delta_sustain_duration;
    CsrUint32                   delta_decay_duration;

    CsrInt16                    stored_level;
    CsrInt16                    stored_red;
    CsrInt16                    stored_green;
    CsrInt16                    stored_blue;
    CsrInt16                    stored_temp;
    CsrInt16                    stored_white_level;

    CsrInt16                    delta_level;
    CsrInt16                    delta_white_level;
    CsrInt16                    delta_red;
    CsrInt16                    delta_green;
    CsrInt16                    delta_blue;
    CsrInt32                    delta_temp;

    transition_sd_state         transition_state;
    CsrUint8                    transition_count;
    timer_id                    transition_tid;
    CsrUint16                   dest_id;
}TRANSITION_DATA_T;

typedef struct
{
    CSRMESH_LIGHT_WHITE_T       light_white;
} MODEL_RSP_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Transition data */
static TRANSITION_DATA_T                        g_trans_data;

/* Model Response Common Data */
static MODEL_RSP_DATA_T                         g_model_rsp_data;

/* Pointer to light handler data */
static LIGHT_HANDLER_DATA_T*                    p_light_hdlr_data;

/* Light NVM Timer Id */
static timer_id                                 light_nvm_tid;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* Power model Set state message handler */
static void lightDataNVMWriteTimerHandler(timer_id tid);
static uint32 getDurationForState(void);

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      lightDataNVMWriteTimerHandler
 *
 *  DESCRIPTION
 *      This function handles NVM Write Timer time-out.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void lightDataNVMWriteTimerHandler(timer_id tid)
{
    if (tid == light_nvm_tid)
    {
        light_nvm_tid = TIMER_INVALID;
        WriteLightModelDataOntoNVM();
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      initialiseTransitionData
 *
 *  DESCRIPTION
 *      This function initialises the transition data
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void initialiseTransitionData(void)
{
    g_trans_data.transition_state = state_idle;
    g_trans_data.transition_count = 0;

    if(g_trans_data.transition_tid != TIMER_INVALID)
    {
        TimerDelete(g_trans_data.transition_tid);
    }
    g_trans_data.transition_tid = TIMER_INVALID;
    g_trans_data.delta_red = 0;
    g_trans_data.delta_green = 0;
    g_trans_data.delta_blue = 0;
    g_trans_data.delta_level = 0;

    g_trans_data.stored_red = 0;
    g_trans_data.stored_green = 0;
    g_trans_data.stored_blue = 0;
    g_trans_data.stored_level = 0;

    g_trans_data.delta_duration = 0;
    g_trans_data.delta_sustain_duration = 0;
    g_trans_data.delta_decay_duration = 0;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      getDurationForState
 *
 *  DESCRIPTION
 *      This function returns the duration based on the transition state
 *
 *  RETURNS/MODIFIES
 *      The delta duration for the corresponding transition state
 *
 *----------------------------------------------------------------------------*/
static uint32 getDurationForState(void)
{
    uint32 duration = 0;

    switch(g_trans_data.transition_state)
    {
        case state_color_change_attacking:
        case state_level_change_attacking:
        case state_temp_change_attacking:
        case state_white_level_change_attacking:
            duration = g_trans_data.delta_duration;
        break;

        case state_sustaining:
            duration = g_trans_data.delta_sustain_duration;
        break;

        case state_decaying:
            duration = g_trans_data.delta_decay_duration;
        break;

        case state_idle:
        default:
        break;
    }
    return duration;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      transitionTimerHandler
 *
 *  DESCRIPTION
 *      This function handles the timer expiry for transition.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void transitionTimerHandler(timer_id tid)
{
    if (tid == g_trans_data.transition_tid)
    {
        g_trans_data.transition_tid = TIMER_INVALID;
        g_trans_data.transition_count ++;
        bool start_nvm_timer = FALSE;

        /* If the count is less than max transition steps then modify and set 
         * the model values based on the appropriate transition state and 
         * restart the transition timer again.
         */
        if(g_trans_data.transition_count <= NUM_TRANSITION_STEPS)
        {
            switch(g_trans_data.transition_state)
            {
                case state_temp_change_attacking:
                {
                    p_light_hdlr_data->light_model.colortemperature = 
                        g_trans_data.stored_temp + (uint16)((g_trans_data.delta_temp * 
                         g_trans_data.transition_count) /NUM_TRANSITION_STEPS);
#ifdef COLOUR_TEMP_ENABLED
                    LightHardwareGetRGBFromColorTemp(
                                   p_light_hdlr_data->light_model.colortemperature,
                                   &p_light_hdlr_data->light_model.red,
                                   &p_light_hdlr_data->light_model.blue,
                                   &p_light_hdlr_data->light_model.green);
#endif
                    /* Set the light level in the updated RGB setting */
                    LightHardwareSetLevel(p_light_hdlr_data->light_model.red,
                                          p_light_hdlr_data->light_model.green,
                                          p_light_hdlr_data->light_model.blue,
                                          p_light_hdlr_data->light_model.level);
                    start_nvm_timer = TRUE;
                }
                break;
                case state_color_change_attacking:
                {
                    p_light_hdlr_data->light_model.red = 
                        (CsrUint8)(g_trans_data.stored_red +
                        (CsrInt16)(g_trans_data.delta_red * 
                        g_trans_data.transition_count) /NUM_TRANSITION_STEPS);

                    p_light_hdlr_data->light_model.green = 
                        (CsrUint8)(g_trans_data.stored_green +
                        (CsrInt16)(g_trans_data.delta_green * 
                        g_trans_data.transition_count) /NUM_TRANSITION_STEPS);

                    p_light_hdlr_data->light_model.blue = 
                        (CsrUint8)(g_trans_data.stored_blue +
                        (CsrInt16)(g_trans_data.delta_blue * 
                        g_trans_data.transition_count) /NUM_TRANSITION_STEPS);
                }
                /* Fall-Through */
                case state_level_change_attacking:
                case state_decaying:
                {
                    p_light_hdlr_data->light_model.level = 
                        (CsrUint8)(g_trans_data.stored_level +
                        (CsrInt16)(g_trans_data.delta_level * 
                        g_trans_data.transition_count) /NUM_TRANSITION_STEPS);

                    /* Set the light level in the latest RGB setting */
                    LightHardwareSetLevel(p_light_hdlr_data->light_model.red,
                                          p_light_hdlr_data->light_model.green,
                                          p_light_hdlr_data->light_model.blue,
                                          p_light_hdlr_data->light_model.level);

                    start_nvm_timer = TRUE;
                }
                break;

                case state_white_level_change_attacking:
                {
                    p_light_hdlr_data->white_level = 
                        (CsrUint8)(g_trans_data.stored_white_level +
                        (CsrInt16)(g_trans_data.delta_white_level * 
                        g_trans_data.transition_count) /NUM_TRANSITION_STEPS);
                    start_nvm_timer = TRUE;
                }
                break;

                case state_sustaining:
                case state_idle:
                default:
                break;
            }

            /* Restart the timer based on the transition state */
            g_trans_data.transition_tid =
                                        TimerCreate(getDurationForState(),
                                                    TRUE,
                                                    transitionTimerHandler);
            if(start_nvm_timer == TRUE)
            {
                /* Delete existing timer */
                if (TIMER_INVALID != light_nvm_tid)
                {
                    TimerDelete(light_nvm_tid);
                }

                /* Restart the timer */
                light_nvm_tid = 
                                TimerCreate(NVM_WRITE_DEFER_DURATION,
                                            TRUE,
                                            lightDataNVMWriteTimerHandler);
            }
        }
        /* If the transition count has reached the maximum steps then start a 
         * new transition based on the state or move the state to idle if all 
         * the transitions are complete.
         */
        else
        {
            /* Send a light state with no ack message on a transition complete
             * when there is a change of model attributes and reset the 
             * transition count.
             */
            g_trans_data.transition_count = 0;
            if(g_trans_data.transition_state 
                                        == state_white_level_change_attacking)
            {
                g_model_rsp_data.light_white.level = p_light_hdlr_data->white_level;

                LightWhite(DEFAULT_NW_ID,
                           g_trans_data.dest_id,
                           AppGetCurrentTTL(),
                           &g_model_rsp_data.light_white, 
                           FALSE);
            }
            else if(g_trans_data.transition_state != state_sustaining)
            {
                LightState(DEFAULT_NW_ID,
                           g_trans_data.dest_id,
                           AppGetCurrentTTL(),
                           &p_light_hdlr_data->light_model,
                           FALSE);
            }
            /* If the Level change transtition is complete then move to either
             * sustaining state or decaying state based on the delta duration
             * values.
             */
            if(g_trans_data.transition_state == state_level_change_attacking)
            {
                if(g_trans_data.delta_sustain_duration != 0)
                {
                    g_trans_data.transition_state = state_sustaining;
                }
                else if(g_trans_data.delta_decay_duration != 0)
                {
                    g_trans_data.transition_state = state_decaying;

                    g_trans_data.delta_level = 
                                (CsrInt16)(-p_light_hdlr_data->light_model.level);
                    g_trans_data.stored_level = 
                                            p_light_hdlr_data->light_model.level;
                }
                else
                {
                    g_trans_data.transition_state = state_idle;
                }
            }
            /* If the sustaining transtition is complete then move to decaying
             * state if the delta decay duration is not zero otherwise move to
             * idle state.
             */
            else if(g_trans_data.transition_state == state_sustaining)
            {
                if(g_trans_data.delta_decay_duration != 0)
                {
                    g_trans_data.transition_state = state_decaying;

                    g_trans_data.delta_level = 
                                (CsrInt16)(-p_light_hdlr_data->light_model.level);
                    g_trans_data.stored_level = 
                                            p_light_hdlr_data->light_model.level;
                }
                else
                {
                    g_trans_data.transition_state = state_idle;
                }
            }
            /* In all the other transition cases move back to idle state */
            else
            {
                g_trans_data.transition_state = state_idle;
            }

            /* start a new transition timer if the state is not assigned to 
             * idle for the next transition.
             */
            if(g_trans_data.transition_state != state_idle)
            {
                /* Restart the timer */
                g_trans_data.transition_tid =
                                        TimerCreate(getDurationForState(),
                                                    TRUE,
                                                    transitionTimerHandler);
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      lightModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Light Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult lightModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                            CSRMESH_EVENT_DATA_T* data,
                                            CsrUint16 length,
                                            void **state_data)
{
    bool start_nvm_timer = FALSE;

    switch(event_code)
    {
        /* handling of Light Model Event */
        case CSRMESH_LIGHT_SET_LEVEL_NO_ACK:
        case CSRMESH_LIGHT_SET_LEVEL:
        {
            CSRMESH_LIGHT_SET_LEVEL_T *p_data = 
            (CSRMESH_LIGHT_SET_LEVEL_T *)(((CSRMESH_EVENT_DATA_T *)data)->data);

            /* Initialise the transition data and delete the timer */
            initialiseTransitionData();

            p_light_hdlr_data->light_model.level = p_data->level;
            p_light_hdlr_data->light_model.power = csr_mesh_power_state_on;

#ifdef ENABLE_POWER_MODEL
            PowerUpdatePowerState(csr_mesh_power_state_on);
#endif
            start_nvm_timer = TRUE;

            /* Set the light level */
            LightHardwareSetLevel(p_light_hdlr_data->light_model.red,
                                  p_light_hdlr_data->light_model.green,
                                  p_light_hdlr_data->light_model.blue,
                                  p_data->level);

            /* Send Light State Information to Model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_light_hdlr_data->light_model;
            }

        }
        break;

        case CSRMESH_LIGHT_SET_POWER_LEVEL_NO_ACK:
        case CSRMESH_LIGHT_SET_POWER_LEVEL:
        {
            CSRMESH_LIGHT_SET_POWER_LEVEL_T *p_data = 
                                (CSRMESH_LIGHT_SET_POWER_LEVEL_T *)
                                    (((CSRMESH_EVENT_DATA_T *)data)->data);
            uint32 duration = 0;

            /* Initialise the transition data and delete the timer.*/
            initialiseTransitionData();
            g_trans_data.dest_id = data->src_id;

            p_light_hdlr_data->light_model.power = p_data->power;
#ifdef ENABLE_POWER_MODEL
            PowerUpdatePowerState(p_light_hdlr_data->light_model.power);
#endif
            if(p_data->power == csr_mesh_power_state_on ||
               p_data->power == csr_mesh_power_state_onfromstandby)
            {
                /* convert the duration received into delta duration based on 
                 * the number of transition steps.
                 */
                g_trans_data.delta_duration =
                  (uint32)(p_data->levelduration*(SECOND/NUM_TRANSITION_STEPS));

                g_trans_data.delta_sustain_duration = 
                   (uint32)(p_data->sustain * (SECOND / NUM_TRANSITION_STEPS));

                g_trans_data.delta_decay_duration = 
                   (uint32)(p_data->decay * (SECOND / NUM_TRANSITION_STEPS));

                /* If the level duration is zero then update the level and move
                 * to sustain or decay states based on the sustain or decay 
                 * duration received.
                 */
                if(p_data->levelduration == 0)
                {
                    p_light_hdlr_data->light_model.level = p_data->level;
                    start_nvm_timer = TRUE;

                    /* Set the light level */
                    LightHardwareSetLevel(p_light_hdlr_data->light_model.red,
                                          p_light_hdlr_data->light_model.green,
                                          p_light_hdlr_data->light_model.blue,
                                          p_data->level);

                    /* If the sustain duration is non-zero move onto sustain .
                     * state. Even though we do not explicitely do any operation
                     * in the sustain state we need to split the sustain 
                     * duration onto delta timers as max timer supported is 
                     * around 32 min and the sustain duration could be higher.
                     */
                    if(p_data->sustain != 0)
                    {
                        g_trans_data.transition_state = state_sustaining;
                        duration = g_trans_data.delta_sustain_duration;
                    }
                    /* If neither the level duration or the sustain duration is 
                     * defined and decay duration is assigned then move onto 
                     * decay state.
                     */
                    else if(p_data->decay != 0)
                    {
                        g_trans_data.transition_state = state_decaying;
                        duration = g_trans_data.delta_decay_duration;
                        g_trans_data.delta_level = 
                            (CsrInt16)(-p_light_hdlr_data->light_model.level);
                        g_trans_data.stored_level = 
                                          p_light_hdlr_data->light_model.level;
                    }
                }
                /* If the level duration is defined then move to level_change 
                 * state.
                 */
                else
                {
                    g_trans_data.transition_state=state_level_change_attacking;
                    duration = g_trans_data.delta_duration;

                    g_trans_data.delta_level = (CsrInt16)(p_data->level - 
                                            p_light_hdlr_data->light_model.level);

                    g_trans_data.stored_level=p_light_hdlr_data->light_model.level;
               }

                /* Start the transition timer if the state is assigned to 
                 * non-idle.
                 */
                if(g_trans_data.transition_state != state_idle)
                {
                    /* start a timer and then start a counter to change the 
                     * color in NUM_TRANSITION_STEPS steps.
                     */
                    g_trans_data.transition_tid = 
                            TimerCreate(duration, TRUE, transitionTimerHandler);
                }
            }
            else if(p_data->power == csr_mesh_power_state_off ||
                    p_data->power == csr_mesh_power_state_standby)
            {
                LightHardwarePowerControl(FALSE);
            }

            /* Send Light State Information to Model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_light_hdlr_data->light_model;
            }
        }
        break;


        case CSRMESH_LIGHT_SET_RGB_NO_ACK:
        case CSRMESH_LIGHT_SET_RGB:
        {
            CSRMESH_LIGHT_SET_RGB_T *p_data = 
                (CSRMESH_LIGHT_SET_RGB_T *)(data->data);

            p_light_hdlr_data->light_model.power = csr_mesh_power_state_on;
#ifdef ENABLE_POWER_MODEL
            PowerUpdatePowerState(csr_mesh_power_state_on);
#endif
            /* Initialise the transition data and delete the timer */
            initialiseTransitionData();
            g_trans_data.dest_id = data->src_id;

            /* If the color duration is zero then update the RGB values. If the
             * duration is non-zero then move to color_change state and start 
             * the RGB and level transition to the desired values.
             */
            if(p_data->colorduration == 0)
            {
                p_light_hdlr_data->light_model.level = p_data->level;
                /* Update State of RGB in application */
                p_light_hdlr_data->light_model.red   = p_data->red;
                p_light_hdlr_data->light_model.green = p_data->green;
                p_light_hdlr_data->light_model.blue  = p_data->blue;
                start_nvm_timer = TRUE;

                /* Set the light level in the latest RGB setting */
                LightHardwareSetLevel(p_light_hdlr_data->light_model.red, 
                                      p_light_hdlr_data->light_model.green,
                                      p_light_hdlr_data->light_model.blue,
                                      p_light_hdlr_data->light_model.level);
            }
            else
            {
                g_trans_data.transition_state = state_color_change_attacking;

                /* Initialize the delta values by subtracting the received RGB
                 * and the level values to the ones present currently.
                 */
                g_trans_data.delta_level = (CsrInt16)(p_data->level - 
                                            p_light_hdlr_data->light_model.level);
                g_trans_data.delta_red   = (CsrInt16)(p_data->red - 
                                            p_light_hdlr_data->light_model.red);
                g_trans_data.delta_green = (CsrInt16)(p_data->green - 
                                            p_light_hdlr_data->light_model.green);
                g_trans_data.delta_blue = (CsrInt16)(p_data->blue - 
                                            p_light_hdlr_data->light_model.blue);

                /* store the current values as they are required for calculating
                 * during the transition process.
                 */
                g_trans_data.stored_level = p_light_hdlr_data->light_model.level;
                g_trans_data.stored_red = p_light_hdlr_data->light_model.red;
                g_trans_data.stored_green = p_light_hdlr_data->light_model.green;
                g_trans_data.stored_blue = p_light_hdlr_data->light_model.blue;


                g_trans_data.delta_duration =
                   (p_data->colorduration * (SECOND / NUM_TRANSITION_STEPS));

                /* start a timer and then start a counter to change the color
                 * in NUM_TRANSITION_STEPS steps.
                 */
                g_trans_data.transition_tid = 
                                TimerCreate(g_trans_data.delta_duration,
                                            TRUE,
                                            transitionTimerHandler);
            }

            /* Send Light State Information to Model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_light_hdlr_data->light_model;
            }
        }
        break;

        case CSRMESH_LIGHT_SET_COLOR_TEMP:
        {
#ifdef COLOUR_TEMP_ENABLED
            CSRMESH_LIGHT_SET_COLOR_TEMP_T *p_data = 
                            (CSRMESH_LIGHT_SET_COLOR_TEMP_T *)(data->data);

            p_light_hdlr_data->light_model.power = csr_mesh_power_state_on;
#ifdef ENABLE_POWER_MODEL
            PowerUpdatePowerState(csr_mesh_power_state_on);
#endif
            /* Initialise the transition data and delete the timer */
            initialiseTransitionData();
            g_trans_data.dest_id = data->src_id;

            if(p_data->tempduration == 0)
            {
                p_light_hdlr_data->light_model.colortemperature = 
                                                    p_data->colortemperature;
                /* Set Colour temperature of light */
                LightHardwareSetColorTemp(
                                p_light_hdlr_data->light_model.colortemperature);
                start_nvm_timer = TRUE;
            }
            else
            {
                g_trans_data.transition_state = state_temp_change_attacking;
                /* Store current color temperature */
                g_trans_data.stored_temp = 
                                   p_light_hdlr_data->light_model.colortemperature;
                g_trans_data.delta_duration = 
                       (p_data->tempduration * (SECOND / NUM_TRANSITION_STEPS));

                g_trans_data.delta_temp = (int32)p_data->colortemperature -
                            (int32)p_light_hdlr_data->light_model.colortemperature;

                /* start a timer and then start a counter to change the color
                 * in NUM_TRANSITION_STEPS steps.
                 */
                g_trans_data.transition_tid = 
                                    TimerCreate(g_trans_data.delta_duration,
                                                TRUE,
                                                transitionTimerHandler);
            }
            /* Send Light State Information to Model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_light_hdlr_data->light_model;
            }
            
#endif /* COLOUR_TEMP_ENABLED */

        }
        break;
        
        case CSRMESH_LIGHT_GET_STATE:
        {
            /* Send Light State Information to Model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_light_hdlr_data->light_model;
            }
        }
        break;

        case CSRMESH_LIGHT_SET_WHITE_NO_ACK:
        case CSRMESH_LIGHT_SET_WHITE:
        {
            CSRMESH_LIGHT_SET_WHITE_T *p_data = 
            (CSRMESH_LIGHT_SET_WHITE_T *)(((CSRMESH_EVENT_DATA_T *)data)->data);

            if(p_data->duration != 0)
            {
                /* Initialise the transition data and delete the timer.*/
                initialiseTransitionData();
                g_trans_data.dest_id = data->src_id;

                g_trans_data.transition_state = 
                                        state_white_level_change_attacking;

                g_trans_data.delta_white_level = (CsrInt16)(p_data->level - 
                                        p_light_hdlr_data->white_level);

                g_trans_data.stored_white_level = 
                                        p_light_hdlr_data->white_level;

                g_trans_data.delta_duration =
                        (p_data->duration * (SECOND / NUM_TRANSITION_STEPS));

                /* start a timer and then start a counter to change the color
                 * in NUM_TRANSITION_STEPS steps.
                 */
                g_trans_data.transition_tid = 
                                TimerCreate(g_trans_data.delta_duration,
                                            TRUE,
                                            transitionTimerHandler);
            }
            else
            {
                /* Store the received white level in the model rsp struct */
                p_light_hdlr_data->white_level = p_data->level;

                start_nvm_timer = TRUE;
            }

            g_model_rsp_data.light_white.level = p_light_hdlr_data->white_level;
            /* The white level can be used to control a seperate white LED. */

            /* Send Light white level back for sending the ack */
            if (state_data != NULL)
            {
                *state_data = (void *)&g_model_rsp_data.light_white;
            }
        }
        break;


        case CSRMESH_LIGHT_GET_WHITE:
        {
            g_model_rsp_data.light_white.level = p_light_hdlr_data->white_level;

            /* Send Light white level back for sending the ack */
            if (state_data != NULL)
            {
                *state_data = (void *)&g_model_rsp_data.light_white;
            }
        }
        break;


        default:
        break;
    }

    /* Start NVM timer if required */
    if (TRUE == start_nvm_timer)
    {
        /* Delete existing timer */
        if (TIMER_INVALID != light_nvm_tid)
        {
            TimerDelete(light_nvm_tid);
        }

        /* Restart the timer */
        light_nvm_tid = TimerCreate(NVM_WRITE_DEFER_DURATION,
                                              TRUE,
                                              lightDataNVMWriteTimerHandler);
    }

    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the light model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void LightModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups)
{
    /* Initialize Light Model */
    LightModelInit(nw_id, 
                   model_groups,
                   num_groups,
                   lightModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Light Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void LightModelDataInit(LIGHT_HANDLER_DATA_T* light_handler_data)
{
    if(light_handler_data != NULL)
    {
        p_light_hdlr_data = light_handler_data;
    }

    p_light_hdlr_data->light_model.red   = 0xFF;
    p_light_hdlr_data->light_model.green = 0xFF;
    p_light_hdlr_data->light_model.blue  = 0xFF;
    p_light_hdlr_data->light_model.level = 0xFF;
    p_light_hdlr_data->white_level = 0xFF;
    p_light_hdlr_data->light_model.power = csr_mesh_power_state_off;

    light_nvm_tid = TIMER_INVALID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadLightModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads the Light Model data from the NVM and stores onto
 *      the global Light Model Structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ReadLightModelDataFromNVM(void)
{
    uint32 rd_data = 0;
    uint16 rd_lvl_data = 0;

    /* Read RGB and Power Data from NVM */
    Nvm_Read((uint16 *)&rd_data, sizeof(uint32), NVM_RGB_DATA_OFFSET);

    /* Unpack data in to the global variables */
    p_light_hdlr_data->light_model.red   = rd_data & 0xFF;
    rd_data >>= 8;
    p_light_hdlr_data->light_model.green = rd_data & 0xFF;
    rd_data >>= 8;
    p_light_hdlr_data->light_model.blue  = rd_data & 0xFF;
    rd_data >>= 8;
    p_light_hdlr_data->light_model.power = rd_data & 0xFF;

    /* Read level and white level Data from NVM */
    Nvm_Read((uint16 *)&rd_lvl_data, sizeof(uint16),
                      NVM_RGB_DATA_OFFSET + LEVEL_DATA_OFFSET);

    /* Keep the level at highest as default */
    p_light_hdlr_data->white_level = rd_lvl_data & 0xFF;
    rd_lvl_data >>= 8;
    p_light_hdlr_data->light_model.level = rd_lvl_data & 0xFF;

#ifdef ENABLE_POWER_MODEL
    PowerUpdatePowerState(p_light_hdlr_data->light_model.power);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteLightModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function writes the Light Model data from the NVM and stores onto
 *      the global Light Model Structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void WriteLightModelDataOntoNVM(void)
{
    uint32 rd_data = 0;
    uint32 wr_data = 0;
    uint16 rd_lvl_data = 0;
    uint16 wr_lvl_data = 0;

    /* Read RGB and Power Data from NVM */
    Nvm_Read((uint16 *)&rd_data, sizeof(uint32), NVM_RGB_DATA_OFFSET);

    /* Pack Data for writing to NVM */
    wr_data = ((uint32) p_light_hdlr_data->light_model.power << 24) |
              ((uint32) p_light_hdlr_data->light_model.blue  << 16) |
              ((uint32) p_light_hdlr_data->light_model.green <<  8) |
              p_light_hdlr_data->light_model.red;

    /* If data on NVM is not equal to current state, write current state
     * to NVM.
     */
    if (rd_data != wr_data)
    {
        Nvm_Write((uint16 *)&wr_data, sizeof(uint32),NVM_RGB_DATA_OFFSET);
    }

    /* Read level and white level Data from NVM */
    Nvm_Read((uint16 *)&rd_lvl_data, sizeof(uint16),
                                 NVM_RGB_DATA_OFFSET + LEVEL_DATA_OFFSET);

    /* Pack Data for writing to NVM */
    wr_lvl_data = ((uint16) p_light_hdlr_data->light_model.level <<  8) |
                  p_light_hdlr_data->white_level;

    /* If data on NVM is not equal to current state, write current state
     * to NVM.
     */
    if (rd_lvl_data != wr_lvl_data)
    {
        Nvm_Write((uint16 *)&wr_lvl_data, 
                  sizeof(uint16),
                  NVM_RGB_DATA_OFFSET + LEVEL_DATA_OFFSET);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightUpdatePowerState
 *
 *  DESCRIPTION
 *      This function can be called to update the Light power state. The 
 *      function also writes the state information onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void LightUpdatePowerState(csr_mesh_power_state_t pwr_state)
{
    p_light_hdlr_data->light_model.power = pwr_state;

    /* Delete existing timer */
    if (TIMER_INVALID != light_nvm_tid)
    {
        TimerDelete(light_nvm_tid);
    }

    /* Restart the timer */
    light_nvm_tid = TimerCreate(NVM_WRITE_DEFER_DURATION,
                                TRUE,
                                lightDataNVMWriteTimerHandler);
}
#endif /* ENABLE_LIGHT_MODEL */

