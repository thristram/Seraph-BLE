/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      extension_model_handler.h
 *
 ******************************************************************************/
#ifndef __EXTENSION_MODEL_HANDLER_H__
#define __EXTENSION_MODEL_HANDLER_H__

#include <csr_types.h>

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the extension model handler */
extern void ExtensionModelHandlerInit(CsrUint8 nw_id,
                                      CsrUint16 model_groups[],
                                      CsrUint16 num_groups);

/* The function initialises the extension model data in the handler */
extern void ExtensionModelDataInit(void);

/* The function reads the extension model data from NVM */
extern void ReadExtensionModelDataFromNVM(CsrUint16 offset);

/* The function writes the extension model data onto NVM */
extern void WriteExtensionModelDataOntoNVM(CsrUint16 offset);

#endif /* __EXTENSION_MODEL_HANDLER_H__ */
