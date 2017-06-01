/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      largeobjecttransfer_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __LARGEOBJECTTRANSFER_MODEL_HANDLER_H__
#define __LARGEOBJECTTRANSFER_MODEL_HANDLER_H__

#include "largeobjecttransfer_model.h"
/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the LOT model handler */
extern void LotModelHandlerInit(CsrUint8 nw_id,
                                  uint16 lot_model_groups[],
                                  CsrUint16 num_groups);


#endif /* __LARGEOBJECTTRANSFER_MODEL_HANDLER_H__ */
