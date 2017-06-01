/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      firmware_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __FIRMWARE_MODEL_HANDLER_H__
#define __FIRMWARE_MODEL_HANDLER_H__

#include "firmware_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/
/* Application Model Handler Data Structure */
typedef struct
{
    /* firmware model state data */
    CSRMESH_FIRMWARE_VERSION_T     fw_version;
}FIRMWARE_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the firmware model handler */
extern void FirmwareModelHandlerInit(CsrUint8 nw_id,
                                     CsrUint16 fw_model_groups[],
                                     CsrUint16 num_groups);

/* The function initialises the firmware model data in the handler */
extern void FirmwareModelDataInit(FIRMWARE_HANDLER_DATA_T* fw_handler_data);

#endif /* __FIRMWARE_MODEL_HANDLER_H__ */
