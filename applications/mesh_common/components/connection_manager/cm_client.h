/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_client.h
 *
 *  DESCRIPTION
 *      Header file for connection manager client
 *
 ******************************************************************************/
#if defined (CLIENT)

#ifndef __CM_CLIENT_H__
#define __CM_CLIENT_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_types.h"
#include "cm_hal.h"

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* Initialises the CM client entity  */
extern void CMClientInit(CM_INIT_PARAMS_T *cm_init_params);

/* Sends the generic event to all the registered service clients */
extern void CMClientNotifyGenericEvent(cm_event event_type,
                                  CM_EVENT_T *client_event);

/* Sends the gatt event to the right client handler */
extern void CMClientNotifyGattEvent(cm_event event_type,
                                CM_EVENT_T *client_event,
                                CM_HANDLERS_T *client_handler);

/* Handles the firmware events related to the client role */
extern void CMClientHandleProcessLMEvent(h_msg_t *msg);

#endif /* __CM_CLIENT_H__ */

#endif /* CLIENT */

