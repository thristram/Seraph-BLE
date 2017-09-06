/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      time_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __TIME_MODEL_HANDLER_H__
#define __TIME_MODEL_HANDLER_H__

#include "time_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* Time model state data */
    CSRMESH_TIME_STATE_T          time_model;
    CsrUint16                     currenttime[3]; 
    CsrInt8                       timezone; 
}TIME_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the time model handler */
extern void TimeModelHandlerInit(CsrUint8 nw_id,
                                 uint16 model_groups[],
                                 CsrUint16 num_groups);

/* The function initialises the time model data in the handler */
extern void TimeModelDataInit(TIME_HANDLER_DATA_T* time_handler_data);

/* The function reads the time model data from NVM */
extern void ReadTimeModelDataFromNVM(uint16 offset);

/* The function writes the time model data onto NVM */
extern void WriteTimeModelDataOntoNVM(uint16 offset);

/* The function can be used to set the UTC */
extern bool TimeModelSetUTC(uint8 utc_time[], CsrInt8 timezone);

/* The function can be used to get the UTC from the time model */
extern bool TimeModelGetUTC(uint8 current_time[], CsrInt8 *timezone);

#endif /* __TIME_MODEL_HANDLER_H__ */
