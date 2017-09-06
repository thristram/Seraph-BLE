/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      diagnostic_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __DIAGNOSTIC_MODEL_HANDLER_H__
#define __DIAGNOSTICBEACON_MODEL_HANDLER_H__

#include <timer.h>
#include <bluetooth.h>
#include "diagnostic_server.h"
#include "csr_mesh_model_common.h"
/*============================================================================*
 *  Public Definitions
 *============================================================================*/
/* Application Model Handler Data Structure */
typedef struct
{
    CsrUint16                   diagnostic_msg[8];
}DIAGNOSTICS_HANDLER_DATA_T;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

extern void DiagnosticModelHandlerInit(CsrUint8 nw_id,
                                       CsrUint16 model_groups[],
                                       CsrUint16 num_groups);

extern void DiagnosticModelDataInit(
                    DIAGNOSTICS_HANDLER_DATA_T* diagnostic_handler_data);

#endif /* __DIAGNOSTIC_MODEL_HANDLER_H__ */
