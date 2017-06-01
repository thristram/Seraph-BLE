/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      sensor_model_handler.c
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
#include "sensor_server.h"
#include "sensor_model_handler.h"
#include "app_mesh_model_handler.h"
#include "app_mesh_handler.h"

#ifdef ENABLE_SENSOR_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

typedef struct
{
    CSRMESH_SENSOR_TYPES_T   sensor_types;
    CSRMESH_SENSOR_VALUE_T   sensor_value;
} MODEL_RSP_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Model Response Common Data */
static MODEL_RSP_DATA_T                         g_model_rsp_data;

/* Pointer to sensor handler data */
static SENSOR_HANDLER_DATA_T*                   p_sensor_hdlr_data;

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
 *      sensorModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Sensor Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult sensorModelEventHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                             CSRMESH_EVENT_DATA_T* data,
                                             CsrUint16 length, 
                                             void **state_data)
{
    /* Due to multiple retransmissions it sometimes happens that the device
     * receives its own message relayed by other devices. So check the source
     * ID and discard the message if it is our own message
     */
    uint16 self_dev_id = 0;
    CSR_MESH_APP_EVENT_DATA_T get_dev_id_data;
    bool event_to_app = FALSE;
    SENSOR_APP_DATA_T sensor_app_data;
    get_dev_id_data.appCallbackDataPtr = &self_dev_id;
    CSRmeshGetDeviceID(CSR_MESH_DEFAULT_NETID, &get_dev_id_data);

    if(self_dev_id == data->src_id)
    {
        *state_data = NULL;
        return CSR_MESH_RESULT_SUCCESS;
    }

    MemSet(&sensor_app_data,
           0x0000,
           sizeof(sensor_app_data));    
    sensor_app_data.event_code = event_code;
    switch(event_code)
    {
        /* Sensor Messages handling */
        case CSRMESH_SENSOR_GET_TYPES:
        {
            CSRMESH_SENSOR_GET_TYPES_T *p_event = 
                                    (CSRMESH_SENSOR_GET_TYPES_T *)data->data;
            MemSet(&g_model_rsp_data.sensor_types,
                   0x0000,
                   sizeof(g_model_rsp_data.sensor_types));

            if(p_event->firsttype <= sensor_type_internal_air_temperature)
            {
                g_model_rsp_data.sensor_types.types[0] = 
                                        sensor_type_internal_air_temperature;
                g_model_rsp_data.sensor_types.types[1] = 
                                        sensor_type_desired_air_temperature;
                g_model_rsp_data.sensor_types.types_len = 2;
            }
            else if(p_event->firsttype <= sensor_type_desired_air_temperature)
            {
                g_model_rsp_data.sensor_types.types[0] = 
                                        sensor_type_desired_air_temperature;
                g_model_rsp_data.sensor_types.types_len = 1;
            }
            else
            {
                g_model_rsp_data.sensor_types.types[0] = sensor_type_invalid;
                g_model_rsp_data.sensor_types.types_len = 0;
            }

            g_model_rsp_data.sensor_types.tid = p_event->tid;
            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&g_model_rsp_data.sensor_types;
            }
        }
        break;

        case CSRMESH_SENSOR_READ_VALUE:
        {
            CSRMESH_SENSOR_READ_VALUE_T *p_event = 
                                    (CSRMESH_SENSOR_READ_VALUE_T *)data->data;
            MemSet(&g_model_rsp_data.sensor_value,
                   0x0000,
                   sizeof(g_model_rsp_data.sensor_value));
            bool send_ack_msg = FALSE;

            if(p_event->type == sensor_type_internal_air_temperature)
            {
                g_model_rsp_data.sensor_value.type = 
                                        sensor_type_internal_air_temperature;
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value;
                BufWriteUint16(&p_temp, *p_sensor_hdlr_data->current_air_temp);
                g_model_rsp_data.sensor_value.value_len = 2;
                send_ack_msg = TRUE;
            }
            else if(p_event->type == sensor_type_desired_air_temperature)
            {
                g_model_rsp_data.sensor_value.type = 
                                        sensor_type_desired_air_temperature;
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value;
                BufWriteUint16(&p_temp, 
                                *p_sensor_hdlr_data->current_desired_air_temp);
                g_model_rsp_data.sensor_value.value_len = 2;
                send_ack_msg = TRUE;
            }

            if(p_event->type2 == sensor_type_desired_air_temperature)
            {
                g_model_rsp_data.sensor_value.type2 = 
                                        sensor_type_desired_air_temperature;
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value2;
                BufWriteUint16(&p_temp, 
                                *p_sensor_hdlr_data->current_desired_air_temp);
                g_model_rsp_data.sensor_value.value2_len = 2;
                send_ack_msg = TRUE;
            }
            else if(p_event->type2 == sensor_type_internal_air_temperature)
            {
                g_model_rsp_data.sensor_value.type2 = 
                                        sensor_type_internal_air_temperature;
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value2;
                BufWriteUint16(&p_temp, *p_sensor_hdlr_data->current_air_temp);
                g_model_rsp_data.sensor_value.value2_len = 2;
                send_ack_msg = TRUE;
            }

            if(send_ack_msg == TRUE)
            {
                /* Populate the structure to be sent to app */
                event_to_app = TRUE;
                sensor_app_data.type = p_event->type;
                sensor_app_data.type2 = p_event->type2;

                g_model_rsp_data.sensor_value.tid = p_event->tid;
                /* Send response data to model */
                if (state_data != NULL)
                {
                    *state_data = (void *)&g_model_rsp_data.sensor_value;
                }
            }
            else
            {
                *state_data = NULL;
            }
        }
        break;

        case CSRMESH_SENSOR_MISSING:
        {
            CSRMESH_SENSOR_MISSING_T *p_event = 
                                         (CSRMESH_SENSOR_MISSING_T *)data->data;
            uint8 index = 0;
            uint8* types;
            uint16 type;
            bool send_ack_msg = FALSE, first_val_inserted = FALSE;
            sensor_type_t type1,type2;

            MemSet(&g_model_rsp_data.sensor_value, 
                   0x0000,
                   sizeof(g_model_rsp_data.sensor_value));

            types =(uint8 *) p_event->types;
            for(index = 0; index < 8; index= index+2)
            {
                type = (uint16)BufReadUint16(&types);
                if(type == sensor_type_internal_air_temperature)
                {
                    type1 = sensor_type_internal_air_temperature;
                    if(first_val_inserted == FALSE)
                    {
                        g_model_rsp_data.sensor_value.type = 
                                        sensor_type_internal_air_temperature;
                        CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value;
                        BufWriteUint16(&p_temp,
                                       *p_sensor_hdlr_data->current_air_temp);
                        g_model_rsp_data.sensor_value.value_len = 2;
                    }
                    else
                    {
                        g_model_rsp_data.sensor_value.type2 = 
                                        sensor_type_internal_air_temperature;
                        CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value2;
                        BufWriteUint16(&p_temp,
                                       *p_sensor_hdlr_data->current_air_temp);
                        g_model_rsp_data.sensor_value.value2_len = 2;
                        break;
                    }
                    send_ack_msg = TRUE;
                }
                else if(type == sensor_type_desired_air_temperature)
                {
                    type2 = sensor_type_desired_air_temperature;
                    if(first_val_inserted == FALSE)
                    {
                        g_model_rsp_data.sensor_value.type = 
                                        sensor_type_desired_air_temperature;
                        CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value;
                        BufWriteUint16(&p_temp, 
                            *p_sensor_hdlr_data->current_desired_air_temp);
                        g_model_rsp_data.sensor_value.value_len = 2;
                    }
                    else
                    {
                        g_model_rsp_data.sensor_value.type2 = 
                                        sensor_type_desired_air_temperature;
                        CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value2;
                        BufWriteUint16(&p_temp, 
                            *p_sensor_hdlr_data->current_desired_air_temp);
                        g_model_rsp_data.sensor_value.value2_len = 2;
                        break;
                    }
                    send_ack_msg = TRUE;
                }
            }
            if(send_ack_msg == TRUE)
            {
                /* Populate the structure to be sent to app */
                event_to_app = TRUE;
                sensor_app_data.type = type1;
                sensor_app_data.type2 = type2;

                /* Send response data to model */
                if (state_data != NULL)
                {
                    *state_data = (void *)&g_model_rsp_data.sensor_value;
                }
            }
            else
            {
                *state_data = NULL;
            }
        }
        break;

        case CSRMESH_SENSOR_SET_STATE:
        {
            CSRMESH_SENSOR_SET_STATE_T *p_event = 
                                    (CSRMESH_SENSOR_SET_STATE_T *)data->data;
            bool send_ack_msg = FALSE;
            uint8 repeatinterval; 


            /* repeat interval of desired or current temp has changed */
            if(p_event->type == sensor_type_desired_air_temperature)
            {
                repeatinterval = p_event->repeatinterval;
                send_ack_msg = TRUE;
            }
            else if(p_event->type == sensor_type_internal_air_temperature)
            {
                repeatinterval = p_event->repeatinterval;
                send_ack_msg = TRUE;
            }

            if(send_ack_msg == TRUE)
            {
                p_sensor_hdlr_data->sensor_model.type = p_event->type;
                p_sensor_hdlr_data->sensor_model.repeatinterval = 
                                                    p_event->repeatinterval;
                p_sensor_hdlr_data->sensor_model.tid = p_event->tid;
 
                /* Send response data to model */
                if (state_data != NULL)
                {
                    *state_data = (void *)&p_sensor_hdlr_data->sensor_model;
                }

                /* Populate the structure to be sent to app */
                event_to_app = TRUE;
                sensor_app_data.type = p_event->type;
                sensor_app_data.repeatinterval = p_event->repeatinterval;
            }
            else
            {
                *state_data = NULL;
            }
        }
        break;


        case CSRMESH_SENSOR_GET_STATE:
        {
            CSRMESH_SENSOR_GET_STATE_T *p_event = 
                                    (CSRMESH_SENSOR_GET_STATE_T *)data->data;
            bool send_ack_msg = FALSE;

            if(p_event->type == sensor_type_desired_air_temperature)
            {
                p_sensor_hdlr_data->sensor_model.repeatinterval = 
                                            GetSensorState(p_event->type);
                send_ack_msg = TRUE;
            }
            else if(p_event->type == sensor_type_internal_air_temperature)
            {
                p_sensor_hdlr_data->sensor_model.repeatinterval = 
                                            GetSensorState(p_event->type);
                send_ack_msg = TRUE;
            }

            if(send_ack_msg == TRUE)
            {
                /* Populate the structure to be sent to app */
                event_to_app = TRUE;
                sensor_app_data.type = p_event->type;

                p_sensor_hdlr_data->sensor_model.type = p_event->type;
                p_sensor_hdlr_data->sensor_model.tid = p_event->tid;
 
                /* Send response data to model */
                if (state_data != NULL)
                {
                    *state_data = (void *)&p_sensor_hdlr_data->sensor_model;
                }
            }
            else
            {
                *state_data = NULL;
            }
        }
        break;

        case CSRMESH_SENSOR_VALUE:
        case CSRMESH_SENSOR_WRITE_VALUE:
        case CSRMESH_SENSOR_WRITE_VALUE_NO_ACK:
        {
            CSRMESH_SENSOR_WRITE_VALUE_T *p_event = 
                                    (CSRMESH_SENSOR_WRITE_VALUE_T *)data->data;

            SENSOR_FORMAT_TEMPERATURE_T recvd_desired_temp = 0;
            SENSOR_FORMAT_TEMPERATURE_T recvd_air_temp = 0;
            uint8 *value, *value2;
            bool send_ack_msg = FALSE;
            MemSet(&g_model_rsp_data.sensor_value,
                   0x0000,
                   sizeof(g_model_rsp_data.sensor_value));

            value = p_event->value;
            if(p_event->type == sensor_type_internal_air_temperature)
            {
                recvd_air_temp = (uint16)BufReadUint16(&value);
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value;
                BufWriteUint16(&p_temp, recvd_air_temp);
                g_model_rsp_data.sensor_value.value_len = 2;
                send_ack_msg = TRUE;
            }
            else if(p_event->type == sensor_type_desired_air_temperature) 
            {
                recvd_desired_temp = (uint16)BufReadUint16(&value);
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value;
                BufWriteUint16(&p_temp, recvd_desired_temp);
                g_model_rsp_data.sensor_value.value_len = 2;
                send_ack_msg = TRUE;
            }

            value2 = p_event->value2;
            if(p_event->type2 == sensor_type_desired_air_temperature) 
            {
                recvd_desired_temp = (uint16)BufReadUint16(&value2);
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value2;
                BufWriteUint16(&p_temp, recvd_desired_temp);
                g_model_rsp_data.sensor_value.value2_len = 2;
                send_ack_msg = TRUE;
            }
            else if(p_event->type2 == sensor_type_internal_air_temperature)
            {
                recvd_air_temp = (uint16)BufReadUint16(&value2);
                CsrUint8 *p_temp = g_model_rsp_data.sensor_value.value2;
                BufWriteUint16(&p_temp, recvd_air_temp);
                g_model_rsp_data.sensor_value.value2_len = 2;
                send_ack_msg = TRUE;
            }

            if(send_ack_msg == TRUE)
            {
                /* Populate the structure to be sent to app */
                event_to_app = TRUE;
                sensor_app_data.type = p_event->type;
                sensor_app_data.type2 = p_event->type2;
                sensor_app_data.recvd_curr_temp = recvd_air_temp;
                sensor_app_data.recvd_desired_temp = recvd_desired_temp;
                sensor_app_data.src_id = data->src_id;
                sensor_app_data.tid = p_event->tid;
            }

            if(event_code == CSRMESH_SENSOR_WRITE_VALUE)
            {
                if(send_ack_msg == TRUE)
                {
                     g_model_rsp_data.sensor_value.type = p_event->type;
                     g_model_rsp_data.sensor_value.type2 = p_event->type2;
                     g_model_rsp_data.sensor_value.tid = p_event->tid;
                    /* Send response data to model */
                    if (state_data != NULL)
                    {
                        *state_data = (void *)&g_model_rsp_data.sensor_value;
                    }
                }
                else
                {
                    *state_data = NULL;
                }
            }
        }
        break;

        default:
        break;
    }

    if(event_to_app == TRUE)
    {
        AppSensorModelHandler(sensor_app_data);
    }

    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      SensorModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the sensor model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void SensorModelHandlerInit(CsrUint8 nw_id,
                                   uint16 sensor_model_grps[],
                                   CsrUint16 num_groups)
{
    /* Initialize sensor Model */
    SensorModelInit(nw_id, 
                    sensor_model_grps,
                    num_groups,
                    sensorModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      SensorModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Sensor Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void SensorModelDataInit(SENSOR_HANDLER_DATA_T* sensor_handler_data)
{
    if(sensor_handler_data != NULL)
    {
        p_sensor_hdlr_data = sensor_handler_data;
    }
}

#endif /* ENABLE_SENSOR_MODEL */

