/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      battery_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __BATTERY_MODEL_HANDLER_H__
#define __BATTERY_MODEL_HANDLER_H__

#include "battery_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* Battery model state data */
    CSRMESH_BATTERY_STATE_T        battery_model;
}BATTERY_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the battery model handler */
extern void BatteryModelHandlerInit(CsrUint8 nw_id,
                                    uint16 model_groups[],
                                    CsrUint16 num_groups);

/* The function initialises the battery model handler */
extern void BatteryModelDataInit(BATTERY_HANDLER_DATA_T* p_handler_data);

#endif /* __BATTERY_MODEL_HANDLER_H__ */
