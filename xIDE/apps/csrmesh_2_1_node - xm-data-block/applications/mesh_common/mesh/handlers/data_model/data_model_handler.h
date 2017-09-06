/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      data_model_handler.h
 *
 *  DESCRIPTION
 *      Header definitions for application data stream implementation
 *
 *****************************************************************************/

#ifndef __DATA_MODEL_HANDLER_H__
#define __DATA_MODEL_HANDLER_H__

/*============================================================================*
 *  CSRmesh Header Files
 *============================================================================*/
#include "data_model.h"
/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
extern void DataModelHandlerInit(CsrUint8 nw_id,
                                 uint16 model_groups[],
                                 CsrUint16 num_groups);

extern void startStream(uint16 dest_id);
#endif /* __DATA_MODEL_HANDLER_H__ */

