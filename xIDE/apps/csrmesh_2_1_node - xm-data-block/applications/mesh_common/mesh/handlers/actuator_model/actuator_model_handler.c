/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      actuator_model_handler.c
 *
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <mem.h>
#include <buf_utils.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "app_util.h"
#include "main_app.h"
#include "actuator_server.h"
#include "actuator_model_handler.h"
#include "app_mesh_model_handler.h"

#ifdef ENABLE_ACTUATOR_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

typedef struct
{
    CSRMESH_ACTUATOR_TYPES_T          actuator_types;
    CSRMESH_ACTUATOR_VALUE_ACK_T      actuator_value;
} MODEL_RSP_DATA_T;


/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Model Response Common Data */
static MODEL_RSP_DATA_T                         g_model_rsp_data;

/* Pointer to actuator handler data */
static ACTUATOR_HANDLER_DATA_T*                 p_actuator_hdlr_data;

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
 *      actuatorModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Actuator Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult actuatorModelEventHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                               CSRMESH_EVENT_DATA_T* data,
                                               CsrUint16 length, 
                                               void **state_data)
{
    switch(event_code)
    {
        /* Actuator Messages handling */
        case CSRMESH_ACTUATOR_GET_TYPES:
        {
            CSRMESH_ACTUATOR_GET_TYPES_T *p_event = 
                                    (CSRMESH_ACTUATOR_GET_TYPES_T *)data->data;
            MemSet(&g_model_rsp_data.actuator_types, 
                   0x0000,
                   sizeof(g_model_rsp_data.actuator_types));

            if(p_event->firsttype <= sensor_type_internal_air_temperature)
            {
                g_model_rsp_data.actuator_types.types[0] = 
                                        sensor_type_internal_air_temperature;
                g_model_rsp_data.actuator_types.types[1] = 
                                        sensor_type_desired_air_temperature;
                g_model_rsp_data.actuator_types.types_len = 2;
            }
            else if(p_event->firsttype <= sensor_type_desired_air_temperature)
            {
                g_model_rsp_data.actuator_types.types[0] = 
                                        sensor_type_desired_air_temperature;
                g_model_rsp_data.actuator_types.types_len = 1;
            }
            else
            {
                g_model_rsp_data.actuator_types.types[0] = sensor_type_invalid;
                g_model_rsp_data.actuator_types.types_len = 0;
            }

            g_model_rsp_data.actuator_types.tid = p_event->tid;
            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&g_model_rsp_data.actuator_types;
            }
        }
        break;


        case CSRMESH_ACTUATOR_SET_VALUE_NO_ACK:
        case CSRMESH_ACTUATOR_SET_VALUE:
        {
            CSRMESH_ACTUATOR_SET_VALUE_T *p_event = 
                                    (CSRMESH_ACTUATOR_SET_VALUE_T *)data->data;

            if(p_event->type == sensor_type_desired_air_temperature)
            {
                /* Length of the value of this type is 16 bits */
                *p_actuator_hdlr_data->current_desired_air_temp =
                                p_event->value[0] + (p_event->value[1] << 8);
                HandleDesiredTempChange();
            }
        }
        /* Fall through */
        case CSRMESH_ACTUATOR_GET_VALUE_ACK:
        {
            CSRMESH_ACTUATOR_GET_VALUE_ACK_T *p_event = 
                                 (CSRMESH_ACTUATOR_GET_VALUE_ACK_T *)data->data;

            if(event_code != CSRMESH_ACTUATOR_SET_VALUE_NO_ACK &&
               state_data != NULL )
            {
                CsrUint8 *p_temp = g_model_rsp_data.actuator_value.value;
                *state_data = (void *)&g_model_rsp_data.actuator_value;

                if(p_event->type == sensor_type_desired_air_temperature)
                {
                    g_model_rsp_data.actuator_value.type = p_event->type;
                    BufWriteUint16(&p_temp,
                            *p_actuator_hdlr_data->current_desired_air_temp);
                    g_model_rsp_data.actuator_value.value_len = 2;
                    g_model_rsp_data.actuator_value.tid = p_event->tid;
                }
                else if(p_event->type == sensor_type_internal_air_temperature)
                {
                    g_model_rsp_data.actuator_value.type = p_event->type;
                    BufWriteUint16(&p_temp,
                                   *p_actuator_hdlr_data->current_air_temp);
                    g_model_rsp_data.actuator_value.value_len = 2;
                    g_model_rsp_data.actuator_value.tid = p_event->tid;
                }
                else
                {
                    *state_data = NULL;
                }
                /* Send response data to model */

            }
            else
            {
                *state_data = NULL;
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
 *      ActuatorModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Actuator model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ActuatorModelHandlerInit(CsrUint8 nw_id,
                                     CsrUint16 actuator_model_grps[],
                                     CsrUint16 num_groups)
{
    /* Initialize Actuator Model */
    ActuatorModelInit(nw_id, 
                      actuator_model_grps,
                      num_groups,
                      actuatorModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ActuatorModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Actuator Model data 
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ActuatorModelDataInit(ACTUATOR_HANDLER_DATA_T* act_handler_data)
{
    if(act_handler_data != NULL)
    {
        p_actuator_hdlr_data = act_handler_data;
    }
}

#endif /* ENABLE_ACTUATOR_MODEL */

