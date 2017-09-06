/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      asset_model_handler.h
 *
 ******************************************************************************/
#ifndef __ASSET_MODEL_HANDLER_H__
#define __ASSET_MODEL_HANDLER_H__

#include "asset_model.h"

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the asset model handler */
extern void AssetModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups);

/* The function initialises the asset model data in the handler */
extern void AssetModelDataInit(void);

/* The function reads the asset model data from NVM */
extern void ReadAssetModelDataFromNVM(uint16 offset);

/* The function writes the asset model data onto NVM */
extern void WriteAssetModelDataOntoNVM(uint16 offset);

/* The function triggers the asset announce broadcast onto n/w */
extern void AssetStartBroadcast(void);

#endif /* __ASSET_MODEL_HANDLER_H__ */
