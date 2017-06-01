/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      actuator_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __ACTUATOR_MODEL_HANDLER_H__
#define __ACTUATOR_MODEL_HANDLER_H__

#include "actuator_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* actuator model state data */
    SENSOR_FORMAT_TEMPERATURE_T     *current_air_temp;
    SENSOR_FORMAT_TEMPERATURE_T     *current_desired_air_temp;
}ACTUATOR_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the actuator model handler */
extern void ActuatorModelHandlerInit(CsrUint8 nw_id,
                                     CsrUint16 actuator_model_groups[],
                                     CsrUint16 num_groups);

/* The function initialises the actuator model data in the handler */
extern void ActuatorModelDataInit(
                            ACTUATOR_HANDLER_DATA_T* actuator_handler_data);

#endif /* __ACTUATOR_MODEL_HANDLER_H__ */
