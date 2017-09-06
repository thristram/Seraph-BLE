/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      power_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __POWER_MODEL_HANDLER_H__
#define __POWER_MODEL_HANDLER_H__

#include "power_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* Power model state data */
    CSRMESH_POWER_STATE_T          power_model;
}POWER_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the power model handler */
extern void PowerModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups);

/* The function initialises the power model data in the handler */
extern void PowerModelDataInit(POWER_HANDLER_DATA_T* power_handler_data);

/* This function can be called to set the power state on the power model */
extern void PowerUpdatePowerState(csr_mesh_power_state_t pwr_state);

#endif /* __POWER_MODEL_HANDLER_H__ */
