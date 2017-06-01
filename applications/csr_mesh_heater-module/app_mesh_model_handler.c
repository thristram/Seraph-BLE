/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_mesh_model_handler.c
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <main.h>
#ifdef CSR101x_A05
#include <ls_app_if.h>
#include <config_store.h>
#endif
#include <nvm.h>
#include <timer.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "nvm_access.h"
#include "app_debug.h"
#include "main_app.h"
#include "iot_hw.h"
#include "app_mesh_model_handler.h"
#include "sensor_client.h"
#include "sensor_server.h"
#include "sensor_model_handler.h"
#include "battery_model_handler.h"
#include "attention_model_handler.h"
#include "time_model_handler.h"
#include "action_model_handler.h"
#include "app_mesh_handler.h"
#include "app_util.h"
#include "gatt_uuid.h"
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

#ifdef ENABLE_ACK_MODE
typedef struct 
{
    uint16 dev_id;
    uint16 tid;
}DEVICE_INFO_T;

/* Maximum devices for which ackowledgements could be sent */
#define MAX_ACK_DEVICES                     (5)

/* Maximum retransmit count */
#define MAX_RETRANSMIT_COUNT                (MAX_RETRANSMISSION_TIME / \
                                             RETRANSMIT_INTERVAL)
#endif /* ENABLE_ACK_MODE */

typedef struct
{
    sensor_type_t type;
    uint16        *value;
    uint8         repeat_interval;
} SENSOR_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

#ifdef ENABLE_ATTENTION_MODEL
static ATTENTION_HANDLER_DATA_T         g_attention_handler_data;
#endif /* ENABLE_ATTENTION_MODEL */

#ifdef ENABLE_BATTERY_MODEL
static BATTERY_HANDLER_DATA_T           g_battery_handler_data;
#endif /* ENABLE_BATTERY_MODEL */

#ifdef ENABLE_SENSOR_MODEL
static SENSOR_HANDLER_DATA_T            g_sensor_handler_data;
#endif /* ENABLE_SENSOR_MODEL */

#ifdef ENABLE_TIME_MODEL
static TIME_HANDLER_DATA_T              g_time_handler_data;
#endif /* ENABLE_TIME_MODEL */

/* Sensor Model Data */
static SENSOR_DATA_T                    sensor_data[NUM_SENSORS_SUPPORTED];

/* Temperature Value in 1/32 kelvin units. */
static SENSOR_FORMAT_TEMPERATURE_T      current_air_temp;

/* Temperature Controller's Current Desired Air Temperature. */
static SENSOR_FORMAT_TEMPERATURE_T      current_desired_air_temp;

/* Read value transmit counter */
static uint16                           read_value_transmit_count = 0;

/* Read Value Timer ID. */
static timer_id                         read_val_tid = TIMER_INVALID;

/* Heater status */
static heater_status_t                  heater_status;

#ifdef ENABLE_ACK_MODE
/* Retransmit Timer ID. */
static timer_id                         retransmit_tid = TIMER_INVALID;

/* Write Value Msg Retransmit counter */
static uint16                           ack_retransmit_count = 0;

/* Array of structure holding the device id and the transaction id to be sent*/
static DEVICE_INFO_T                    sensor_dev_ack[MAX_ACK_DEVICES];
#endif /* ENABLE_ACK_MODE */

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static void startReadValueTimer(void);
static void readValTimerHandler(timer_id tid);
#ifdef ENABLE_SENSOR_MODEL
static void readCurrentTempFromGroup(void);
#endif
#ifdef ENABLE_ACK_MODE
static void sendValueAck(void);
static void retransmitIntervalTimerHandler(timer_id tid);
static void startRetransmitTimer(void);
static void addDeviceToSensorList(uint16 dev_id, uint8 tid);
static void resetDeviceList(void);
#endif /* ENABLE_ACK_MODE */

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
 
#ifdef ENABLE_ACK_MODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      sendValueAck
 *
 *  DESCRIPTION
 *      This function sends the sensor value acknowledgement message back to the
 *      device.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void sendValueAck(void)
{
    uint16 index;

    for(index = 0; index < MAX_ACK_DEVICES; index++)
    {
        if(sensor_dev_ack[index].dev_id != MESH_BROADCAST_ID)
        {
            /* Retransmitting the same message on every transmission interval 
             * can be configured to increase the possibility of the msg to reach 
             * to the scanning devices with more robustness as they are scanning 
             * at low duty cycles.
             */
            CSRMESH_SENSOR_VALUE_T value;
            value.type = sensor_type_internal_air_temperature;
            value.value[0] = WORD_LSB(current_air_temp);
            value.value[1] = WORD_MSB(current_air_temp);
            value.value_len = 2;
            value.type2 = sensor_type_desired_air_temperature;
            value.value2[0] = WORD_LSB(current_desired_air_temp);
            value.value2[1] = WORD_MSB(current_desired_air_temp);
            value.value2_len = 2;
            value.tid = sensor_dev_ack[index].tid;
            SensorValue(0, sensor_dev_ack[index].dev_id, AppGetCurrentTTL(),
                                                                        &value);
        }
    }
    DEBUG_STR(" Acknowledge DESIRED TEMP : ");
    PrintInDecimal(current_desired_air_temp/32);
    DEBUG_STR(" kelvin\r\n");

    DEBUG_STR(" Acknowledge AIR TEMP : ");
    PrintInDecimal(current_air_temp/32);
    DEBUG_STR(" kelvin\r\n");
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      retransmitIntervalTimerHandler
 *
 *  DESCRIPTION
 *      This function expires when the next message needs to be transmitted 
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void retransmitIntervalTimerHandler(timer_id tid)
{
    if (tid == retransmit_tid)
    {
        retransmit_tid = TIMER_INVALID;

        /* transmit the pending message to all the groups*/
        sendValueAck();

        ack_retransmit_count --;

        /* start a timer to send the broadcast ack data */
        startRetransmitTimer();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startRetransmitTimer
 *
 *  DESCRIPTION
 *      This function starts the broadcast timer for retransmission.
 *
 *  RETURNS
 *      None
 *
 *----------------------------------------------------------------------------*/
static void startRetransmitTimer(void)
{
    if(retransmit_tid == TIMER_INVALID && ack_retransmit_count > 0)
    {
        retransmit_tid = TimerCreate(RETRANSMIT_INTERVAL, TRUE,
                                     retransmitIntervalTimerHandler);
    }

    /* reset the device id and the tid in the device list as we have sent all
     * the ack to the devices
     */
    if(ack_retransmit_count == 0)
    {
        resetDeviceList();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      addDeviceToSensorList
 *
 *  DESCRIPTION
 *      This function adds the device id and the transaction id onto the 
 *      list.
 *
 *  RETURNS
 *      None
 *
 *----------------------------------------------------------------------------*/
static void addDeviceToSensorList(uint16 dev_id, uint8 tid)
{
    uint8 index;
    bool dev_found = FALSE;

    /* Check whether the device is already present in the list as we are 
     * sending the acknowledgements in which case we can refresh the latest
     * tid for that device.
     */
    for(index = 0; index < MAX_ACK_DEVICES; index++)
    {
        if(sensor_dev_ack[index].dev_id == dev_id)
        {
            dev_found = TRUE;
            sensor_dev_ack[index].tid = tid;
            break;
        }
    }
    /* If the device is not found then add the device onto the device list db */
    if(!dev_found)
    {
        for(index = 0; index < MAX_ACK_DEVICES; index++)
        {
            if(sensor_dev_ack[index].dev_id == MESH_BROADCAST_ID)
            {
                sensor_dev_ack[index].dev_id = dev_id;
                sensor_dev_ack[index].tid = tid;
                break;
            }
        }
        /* If the device list is full then just replace the oldest device id as
         * we would have sent the maximum acknowledgements to the first dev
         * added in the list.
         */
        if(index == MAX_ACK_DEVICES - 1)
        {
            sensor_dev_ack[0].dev_id = dev_id;
            sensor_dev_ack[0].tid = tid;
        }
    }
    ack_retransmit_count = MAX_RETRANSMIT_COUNT;

    /* start a timer to send the broadcast ack data */
    TimerDelete(retransmit_tid);
    retransmit_tid = TIMER_INVALID;
    startRetransmitTimer();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      resetDeviceList
 *
 *  DESCRIPTION
 *      The function resets the device id and the ack flag of complete db
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void resetDeviceList(void)
{
    uint16 idx;

    for(idx=0; idx < MAX_ACK_DEVICES; idx++)
    {
        sensor_dev_ack[idx].dev_id = MESH_BROADCAST_ID;
        sensor_dev_ack[idx].tid = 0;
    }
}
#endif /* ENABLE_ACK_MODE */

#ifdef ENABLE_SENSOR_MODEL
/*----------------------------------------------------------------------------*
 *  NAME
 *      readCurrentTempFromGroup
 *
 *  DESCRIPTION
 *      This function reads the current temperature from the supported group
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void readCurrentTempFromGroup(void)
{
    uint16 index;
    CSRMESH_SENSOR_READ_VALUE_T sensor_read;

    for(index = 0; index < MAX_MODEL_GROUPS; index++)
    {
        if(sensor_model_groups[index] != 0)
        {
            sensor_read.type = sensor_type_internal_air_temperature;
            sensor_read.type2 = sensor_type_desired_air_temperature;
            sensor_read.tid = 0;
            SensorReadValue(DEFAULT_NW_ID, sensor_model_groups[index],
                            AppGetCurrentTTL(), &sensor_read);
        }
    }
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      startReadValueTimer
 *
 *  DESCRIPTION
 *      This function starts the broadcast timer for retransmission.
 *
 *  RETURNS
 *      None
 *
 *----------------------------------------------------------------------------*/
static void startReadValueTimer(void)
{
    if(read_val_tid == TIMER_INVALID && read_value_transmit_count > 0)
    {
        read_val_tid = TimerCreate(RETRANSMIT_INTERVAL, TRUE,
                                   readValTimerHandler);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      readValTimerHandler
 *
 *  DESCRIPTION
 *      This function expires when the next message needs to be transmitted 
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void readValTimerHandler(timer_id tid)
{
    if (tid == read_val_tid)
    {
        read_val_tid = TIMER_INVALID;

#ifdef ENABLE_SENSOR_MODEL
        /* Read the internal and desired temp from the group */
        readCurrentTempFromGroup();
#endif
        read_value_transmit_count --;

        /* start a timer to read the temp from the group */
        startReadValueTimer();
    }
}


/*============================================================================*
 *  Public Function Implemtations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      InitializeSupportedModelData
 *
 *  DESCRIPTION
 *      This function initializes the mesh model data used by the application.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void InitializeSupportedModelData(void)
{
#ifdef ENABLE_SENSOR_MODEL
        /* Initialize the sensor model */
        SensorModelDataInit(&g_sensor_handler_data);
#endif /* ENABLE_SENSOR_MODEL */

#ifdef ENABLE_ATTENTION_MODEL
        /* Initialize the attention model */
        AttentionModelDataInit(&g_attention_handler_data);
#endif /* ENABLE_ATTENTION_MODEL */

#ifdef ENABLE_BATTERY_MODEL
        /* Initialize the battery model */
        BatteryModelDataInit(&g_battery_handler_data);
#endif /* ENABLE_BATTERY_MODEL */

#ifdef ENABLE_TIME_MODEL
        /* Initialize Time Model */
        TimeModelDataInit(&g_time_handler_data);
#endif /* ENABLE_TIME_MODEL */

#ifdef ENABLE_ACTION_MODEL
        /* Initialize Action Model */
        ActionModelDataInit();
#endif /* ENABLE_ACTION_MODEL */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadSensorDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads sensor state data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void ReadSensorDataFromNVM(uint16 idx)
{
    Nvm_Read((uint16*)(sensor_data[idx].value), 
             sizeof(uint16),
             (GET_SENSOR_NVM_OFFSET(idx)));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteSensorDataToNVM
 *
 *  DESCRIPTION
 *      This function writes sensor state data from state variable into NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void WriteSensorDataToNVM(uint16 idx)
{
    Nvm_Write((uint16*)(sensor_data[idx].value), 
              sizeof(uint16),
              (GET_SENSOR_NVM_OFFSET(idx)));
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      InitialiseSensorData
 *
 *  DESCRIPTION
 *      This function initialises supported sensor data.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void InitialiseSensorData(void)
{
    /* Initialise sensor data. */
    sensor_data[CURRENT_AIR_TEMP_IDX].type  = 
                                        sensor_type_internal_air_temperature;
    sensor_data[CURRENT_AIR_TEMP_IDX].value = 
                                        (uint16 *)&current_air_temp;
    sensor_data[DESIRED_AIR_TEMP_IDX].type  = 
                                        sensor_type_desired_air_temperature;
    sensor_data[DESIRED_AIR_TEMP_IDX].value = 
                                        (uint16 *)&current_desired_air_temp;
#ifdef ENABLE_SENSOR_MODEL
    g_sensor_handler_data.current_air_temp = 
                                         (uint16 *)&current_air_temp;
    g_sensor_handler_data.current_desired_air_temp = 
                                         (uint16 *)&current_desired_air_temp;
#endif
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      IsHeaterConfigured
 *
 *  DESCRIPTION
 *      This below function returns whether the heater is configured or not
 *
 *  RETURNS/MODIFIES
 *      TRUE if the heater has been grouped otherwise returns FALSE
 *
 *----------------------------------------------------------------------------*/
extern bool IsHeaterConfigured(void)
{
    uint16 index;

    for(index = 0; index < MAX_MODEL_GROUPS; index++)
    {
        if(sensor_model_groups[index] != 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      ConfigureHeater
 *
 *  DESCRIPTION
 *      The below function is called when the sensor group is modified.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void ConfigureHeater(bool old_config)
{
    /* If sensor was previously not grouped and has been grouped now, then the
     * sensor should move into low duty cycle 
     */
    if(!old_config && IsHeaterConfigured())
    {
        DEBUG_STR("Moving to Low Power Sniff Mode \r\n\r\n");
        EnableHighDutyScanMode(FALSE);
    }
    else if(old_config && !IsHeaterConfigured())
    {
        DEBUG_STR("Sensor Moving to active scan Mode \r\n\r\n");
        EnableHighDutyScanMode(TRUE);

#ifdef ENABLE_ACK_MODE
        /* Delete the retransmit timer */
        TimerDelete(retransmit_tid);
        retransmit_tid = TIMER_INVALID;
#endif
        /* Stop the reading of the temp */
        TimerDelete(read_val_tid);
        read_val_tid = TIMER_INVALID;
    }

    /* Grouping has been modified but sensor is still configured. Hence 
     * start temp read and update 
     */
    if(IsHeaterConfigured())
    {
        /* read the current temperature of the group */
        read_value_transmit_count = 60;

        /* start a timer to read the temp from the group */
        TimerDelete(read_val_tid);
        read_val_tid = TIMER_INVALID;
        startReadValueTimer();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      InitialiseHeater
 *
 *  DESCRIPTION
 *      This function is called to enable the reading of the temperature sensor.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void InitialiseHeater(void)
{
    if(IsHeaterConfigured())
    {
        EnableHighDutyScanMode(FALSE);
        DEBUG_STR("Heater Configured Moving to Low Power Mode \r\n\r\n");

        /* read the current temperature of the group */
        read_value_transmit_count = 60;

#ifdef ENABLE_ACK_MODE
        retransmit_tid = TIMER_INVALID;
#endif
        /* start a timer to read the temp from the group */
        read_val_tid = TIMER_INVALID;
        startReadValueTimer();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetSensorState
 *
 *  DESCRIPTION
 *      This function returns the state value of the specfic sensor requested.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern uint8 GetSensorState(sensor_type_t type)
{
    if(type == sensor_type_desired_air_temperature)
    {
        return sensor_data[DESIRED_AIR_TEMP_IDX].repeat_interval;
    }
    else if(type == sensor_type_internal_air_temperature)
    {
        return sensor_data[CURRENT_AIR_TEMP_IDX].repeat_interval;
    }
    else
    {
        return 0;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetHeaterStatus
 *
 *  DESCRIPTION
 *      This function returns the status of the heater.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern heater_status_t GetHeaterStatus(void)
{
    return heater_status;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppSensorModelHandler
 *
 *  DESCRIPTION
 *      This function is called from the sensor model handler to the application
 *      to indicate a message of interest has been received by the model. The
 *      application can handle the required sensor model messages and take 
 *      relavant actions on the same.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void AppSensorModelHandler(SENSOR_APP_DATA_T sensor_app_data)
{
    switch(sensor_app_data.event_code)
    {
        case CSRMESH_SENSOR_VALUE:
        case CSRMESH_SENSOR_WRITE_VALUE:
        case CSRMESH_SENSOR_WRITE_VALUE_NO_ACK:
        {
            /* We have received a new value from the group. Hence stop the 
             * read being done for the current and the desired temp.
             */
            TimerDelete(read_val_tid);
            read_val_tid = TIMER_INVALID;
            read_value_transmit_count = 0;

            if(((current_desired_air_temp != sensor_app_data.recvd_desired_temp)
                    || current_air_temp != sensor_app_data.recvd_curr_temp)
                && (sensor_app_data.recvd_desired_temp != 0 
                    && sensor_app_data.recvd_curr_temp != 0))
            {
                DEBUG_STR(" RECEIVED CURRENT TEMP : ");
                PrintInDecimal(sensor_app_data.recvd_curr_temp/32);
                DEBUG_STR(" kelvin\r\n");
                DEBUG_STR(" RECEIVED DESIRED TEMP : ");
                PrintInDecimal(sensor_app_data.recvd_desired_temp/32);
                DEBUG_STR(" kelvin\r\n");

                /* As these are new values Initialise them to the latest */
                current_desired_air_temp = sensor_app_data.recvd_desired_temp;
                current_air_temp = sensor_app_data.recvd_curr_temp;
            }

#ifdef ENABLE_ACK_MODE
                addDeviceToSensorList(sensor_app_data.src_id, 
                                      sensor_app_data.tid);
#endif /* ENABLE_ACK_MODE */

                /* Desired temperature needs to be updated in the NVM */
                WriteSensorDataToNVM(DESIRED_AIR_TEMP_IDX);

            if( current_desired_air_temp > current_air_temp )
            {
                if( heater_status == heater_off )
                {
                    /* Print only if the status changed */
                    DEBUG_STR("HEATER ON\r\n");
                    heater_status = heater_on;
                }
                /* Indicate the Heater ON status by glowing red LED */
                IOTLightControlDeviceSetColor(255,0,0);
            }
            else
            {
                if( heater_status == heater_on )
                {
                    /* Print if the status changed */
                    DEBUG_STR("HEATER OFF\r\n");
                    heater_status = heater_off;
                }
                /* Turn off the red LED to indicate Heating status */
                IOTLightControlDevicePower(FALSE);
            }
        }
        break;

        default:
        break;
    }
}
