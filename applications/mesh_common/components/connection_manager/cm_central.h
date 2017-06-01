/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_central.h
 *
 *  DESCRIPTION
 *      Header file for connection manager central.
 *
 *
 ******************************************************************************/
#if defined (CENTRAL)

#ifndef __CM_CENTRAL_H__
#define __CM_CENTRAL_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_hal.h"

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* Initialises the central module */
extern void CMCentralInit(void);

/*  Sets the Scanning state */
extern void CMSetScanState(cm_scan_state new_state);

/* Sets the Connecting state */
extern void CMSetConnState(cm_connect_state new_state);

/* Handles the firmware events related to the central role */
extern void CMCentralHandleProcessLMEvent(h_msg_t *msg);

#endif /* __CM_CENTRAL_H__ */

#endif /* CENTRAL */
