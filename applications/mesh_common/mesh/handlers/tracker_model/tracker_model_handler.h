/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      tracker_model_handler.h
 *
 ******************************************************************************/
#ifndef __TRACKER_MODEL_HANDLER_H__
#define __TRACKER_MODEL_HANDLER_H__

#include "tracker_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the tracker model handler */
extern void TrackerModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups);

/* The function initialises the tracker model data in the handler */
extern void TrackerModelDataInit(void);

/* The function reads the tracker model data from NVM */
extern void ReadTrackerModelDataFromNVM(uint16 offset);

/* The function writes the tracker model data onto NVM */
extern void WriteTrackerModelDataOntoNVM(uint16 offset);

#endif /* __TRACKER_MODEL_HANDLER_H__ */
