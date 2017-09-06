/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_otau_handler.h
 *
 *
 ******************************************************************************/
#ifndef __APP_OTAU_HANDLER_H__
#define __APP_OTAU_HANDLER_H__

#include "user_config.h"

/*============================================================================*
 *  Public Data Types
 *============================================================================*/
typedef enum {
   csr101x = 0,
   csr102x
} upgrade_header_device_variant_t;

typedef enum {
   firmware_upgrade = 0
} upgrade_header_upgrade_type_t;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* This function is uswed to register GAIA Otau callbacks */
extern void AppGaiaOtauInit(void);
extern void AppGaiaOtauDataInit(void);

#endif /* __APP_OTAU_HANDLER_H__ */
