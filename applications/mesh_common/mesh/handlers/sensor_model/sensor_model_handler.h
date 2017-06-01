/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      sensor_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __SENSOR_MODEL_HANDLER_H__
#define __SENSOR_MODEL_HANDLER_H__

#include "sensor_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* sensor model state data */
    CSRMESH_SENSOR_STATE_T          sensor_model;
    SENSOR_FORMAT_TEMPERATURE_T     *current_air_temp;
    SENSOR_FORMAT_TEMPERATURE_T     *current_desired_air_temp;
}SENSOR_HANDLER_DATA_T;

/* Data structure defined to send the information from the Sensor model handler
 * to the application 
 */
typedef struct
{
    CSRMESH_MODEL_EVENT_T           event_code;
    sensor_type_t                   type;
    sensor_type_t                   type2;
    SENSOR_FORMAT_TEMPERATURE_T     recvd_curr_temp;
    SENSOR_FORMAT_TEMPERATURE_T     recvd_desired_temp;
    CsrUint8                        repeatinterval;
    CsrUint16                       src_id;
    CsrUint8                        tid;
}SENSOR_APP_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the sensor model handler */
extern void SensorModelHandlerInit(CsrUint8 nw_id,
                                  uint16 sensor_model_groups[],
                                  CsrUint16 num_groups);

/* The function initialises the sensor model data in the handler */
extern void SensorModelDataInit(SENSOR_HANDLER_DATA_T* sensor_handler_data);

#endif /* __SENSOR_MODEL_HANDLER_H__ */
