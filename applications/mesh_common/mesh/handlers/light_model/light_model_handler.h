/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      light_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __LIGHT_MODEL_HANDLER_H__
#define __LIGHT_MODEL_HANDLER_H__

#include "light_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* Light model state data */
    CSRMESH_LIGHT_STATE_T          light_model;
    uint8                          white_level;
}LIGHT_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the light model handler */
extern void LightModelHandlerInit(CsrUint8 nw_id,
                                  uint16 light_model_groups[],
                                  CsrUint16 num_groups);

/* The function initialises the light model data in the handler */
extern void LightModelDataInit(LIGHT_HANDLER_DATA_T* light_handler_data);

/* The function reads the light model data from NVM */
extern void ReadLightModelDataFromNVM(void);

/* The function writes the light model data onto NVM */
extern void WriteLightModelDataOntoNVM(void);

/* This function can be called to set the power state on the light model */
extern void LightUpdatePowerState(csr_mesh_power_state_t pwr_state);

#endif /* __LIGHT_MODEL_HANDLER_H__ */
