/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_mesh_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __APP_MESH_MODEL_HANDLER_H__
#define __APP_MESH_MODEL_HANDLER_H__

#include "sensor_types.h"
#include "sensor_model_handler.h"
/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Current Air temperature sensor Index */
#define CURRENT_AIR_TEMP_IDX             (0)

/* Desired Air temperature sensor Index */
#define DESIRED_AIR_TEMP_IDX             (1)

/* Heater Status */
typedef enum
{
    heater_off = 0,
    heater_on = 1
}heater_status_t;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* Initialize supported model data */
extern void InitializeSupportedModelData(void);

/* This function reads sensor state data from NVM into state variable.*/
extern void ReadSensorDataFromNVM(uint16 idx);

/* This function writes sensor state data from state variable into NVM.*/
extern void WriteSensorDataToNVM(uint16 idx);

/* This function initialises the sensor data */
extern void InitialiseSensorData(void);

/* This function initialises the heater */
extern void InitialiseHeater(void);

/* This function returns the status of the heater */
extern heater_status_t GetHeaterStatus(void);

/* This function configures the heater */
extern void ConfigureHeater(bool old_config);

/* The function returns the config status of the Heater */
extern bool IsHeaterConfigured(void);

/* This function is called from the sensor model handler to the app */
extern void AppSensorModelHandler(SENSOR_APP_DATA_T sensor_app_data);

/* The function returns the sensor state of the requested sensor */
extern uint8 GetSensorState(sensor_type_t type);

#endif /* __APP_MESH_MODEL_HANDLER_H__ */
