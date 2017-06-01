/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_server.h
 *
 *  DESCRIPTION
 *      Header file for connection manager server role.
 *
 *
 ******************************************************************************/
#if defined (SERVER)

#ifndef __CM_SERVER_H__
#define __CM_SERVER_H__

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
/* Initialises the GATT server entity  */
extern bool CMServerInit(CM_INIT_PARAMS_T *cm_init_params);

/* Gets the server handler */
extern CM_HANDLERS_T *CMServerGetHandler(uint16 handle);

/* Sends the connection manager event onto all the registered servers */
extern void CMServerNotifyGenericEvent(cm_event event_type,
                              CM_EVENT_T *server_event);

/* Sends the server access event to the right service */
extern void CMServerNotifyGattEvent(cm_event event_type,
                              CM_EVENT_T *server_event,
                              CM_HANDLERS_T *server_handler);

/* Handles the firmware events related to the server role */
extern void CMServerHandleProcessLMEvent(h_msg_t *msg);

#endif /* SERVER */

#endif /* __CM_SERVER_H__ */
