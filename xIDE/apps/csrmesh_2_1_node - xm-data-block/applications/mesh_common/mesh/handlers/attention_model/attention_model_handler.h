/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      attention_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __ATTENTION_MODEL_HANDLER_H__
#define __ATTENTION_MODEL_HANDLER_H__

#include "attention_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* Application Model Handler Data Structure */
typedef struct
{
    /* Attention model state data */
    CSRMESH_ATTENTION_STATE_T          attn_model;
}ATTENTION_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the attention model handler */
extern void AttentionModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups);

/* The function initialises the attention model data in the handler */
extern void AttentionModelDataInit(ATTENTION_HANDLER_DATA_T* attn_handler_data);

#endif /* __ATTENTION_MODEL_HANDLER_H__ */
