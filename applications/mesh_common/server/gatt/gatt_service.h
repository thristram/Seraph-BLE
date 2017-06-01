/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gatt_service.h
 *
 *  DESCRIPTION
 *      Header definitions for Gatt service
 *
 *****************************************************************************/

#ifndef __GATT_SERVICE_H__
#define __GATT_SERVICE_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function initialises the gatt service */
extern void GattInitServerService(bool nvm_start_fresh, uint16 *nvm_offset);

/* This function initialises the gatt service in case temporary pairing is
 * required.
 */
extern void GattExtInitServerService(bool nvm_start_fresh, uint16 *nvm_offset,
                                     bool temporary_pairing);

/* This function allows other modules to read whether the bonded device
 * has requested indications on the Service Changed characteristic
 */
extern bool GattServiceChangedIndActive(void);

#if defined (CSR101x_A05)
/* This function should be called when the device is
 * switched into over-the-air update mode
 */
extern void GattOnOtaSwitch(void);
#endif /* CSR101x_A05 */

#endif /* __GATT_SERVICE_H__ */
