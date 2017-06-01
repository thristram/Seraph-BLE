/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      action_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __ACTION_MODEL_HANDLER_H__
#define __ACTION_MODEL_HANDLER_H__

#include "action_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the action model handler */
extern void ActionModelHandlerInit(CsrUint8 nw_id,
                                   uint16 model_groups[],
                                   CsrUint16 num_groups);

/* The function initialises the action model data in the handler */
extern void ActionModelDataInit(void);

/* The function reads the action model data from NVM */
extern void ReadActionModelDataFromNVM(uint16 offset);

/* The function writes the action model data onto NVM */
extern void WriteActionModelDataOntoNVM(uint16 offset);

/* This function is called to sync the current time with the action model */
extern void ActionModelSyncCurrentTime(uint8 current_time[]);

#endif /* __ACTION_MODEL_HANDLER_H__ */
