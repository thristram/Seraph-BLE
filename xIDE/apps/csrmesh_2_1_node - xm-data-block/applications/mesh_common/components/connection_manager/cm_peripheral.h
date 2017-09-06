/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_peripheral.h
 *
 *  DESCRIPTION
 *      Header file for the connection manager peripheral functionality.
 *
 *
 ******************************************************************************/
#if defined (PERIPHERAL)

#ifndef __CM_PERIPHERAL_H__
#define __CM_PERIPHERAL_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <bluetooth.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"
#include "cm_hal.h"

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* Initialises the peripheral module */
extern void CMPeripheralInit(void);

/* Sets the Advertising state */
extern void CMSetAdvState(cm_advert_state new_state);

/* Returns the address of the first bonded device
 * for directed advertisments.
 */
extern bool CMPeripheralSetDirectAdvAddr(TYPED_BD_ADDR_T *adv_addr);

/* Starts connection parameter update procedure */
extern cm_status_code CMPeripheralConnParamUpdate(device_handle_id device_id);

/* Removes pairing from peripheral device */
extern void CMPeripheralRemovePairing(bond_handle_id bond_id);

/* Handles the directed advertisments timeout */
extern void CMPeripheralDirectAdvTimeout(void);

/* Enables or disables the whitelist */
extern bool CMIsWhitelistEnabled(void);

/* Handles the firmware events related to the peripheral role */
extern void CMPeripheralHandleProcessLmEvent(h_msg_t *msg);

#endif /* __CM_PERIPHERAL_H__ */

#endif /* PERIPHERAL */

