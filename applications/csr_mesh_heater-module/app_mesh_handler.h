/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_mesh_handler.h
 *
 *
 ******************************************************************************/
#ifndef __APP_MESH_HANDLER_H__
#define __APP_MESH_HANDLER_H__

#include "core_mesh_handler.h"
#include "user_config.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*============================================================================*
 *  Public Data
 *============================================================================*/
extern uint8 mesh_ad_data[];

/* Place holders to store assigned model group IDs */
uint16 sensor_model_groups[MAX_MODEL_GROUPS];
uint16 attention_model_groups[MAX_MODEL_GROUPS];
uint16 data_model_groups[MAX_MODEL_GROUPS];
uint16 lot_model_groups[MAX_MODEL_GROUPS];

#ifdef ENABLE_DATA_MODEL
/* Device info string */
#define DEVICE_INFO_STRING               "CSRmesh 2.1 Heater\r\n" \
                                         "Supported Models:\r\n" \
                                         " Sensor Model\r\n" \
                                         " Attention Model\r\n" \
                                         " Battery Model\r\n" \
                                         " Data Model"
#endif
/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* The function Initializes core mesh stack and the supported models. */
extern void AppMeshInit(void);

/* This function handles the CSRmesh GROUP SET message to set Group IDs as of 
 * the supported models as assigned in the message
 */
extern bool HandleGroupSetMsg(CSR_MESH_GROUP_ID_RELATED_DATA_T msg);

/* This function should be called when the stack has removed the association.
 * The function removes the association information from the application 
 * structures and cleans up the model groups and the app data.
 */
extern void RemoveAssociation(void);

/* The function sets the desired scan duty cycle on the device */
extern void SetScanDutyCycle(uint8 scan_duty_cycle);

/* The function restores the LED light based on the state of the device */
extern void RestoreLightState(void);

/* This function is called to set and reset the high scan duty cycle mode  */
extern void EnableHighDutyScanMode(bool enable);

/* The function is called on association complete */
extern void AppHandleAssociationComplete(void);

/* The function is called on current time update in the time model */
extern void AppHandleCurrentTimeUpdate(uint8 current_time[]);

#ifdef NVM_TYPE_FLASH
/* This function writes the application data to NVM. This function should 
 * be called on getting nvm_status_needs_erase
 */
extern void WriteApplicationAndServiceDataToNVM(void);
#endif /* NVM_TYPE_FLASH */

#endif /* __APP_MESH_HANDLER_H__ */
