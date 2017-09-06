/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      mesh_control_service.h
 *
 *  DESCRIPTION
 *      Header definitions for mesh control service
 *
 *****************************************************************************/

#ifndef __MESH_CONTROL_SERVICE_H__
#define __MESH_CONTROL_SERVICE_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

#define MESH_LONGEST_MSG_LEN                              (27)

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* This function is used to initialise Mesh Control Service data structure.*/
extern void MeshControlServiceDataInit(void);

/* This function notifies responses received on the mesh */
extern void MeshControlNotifyResponse(uint16 ucid, uint8 *mtl_msg, uint8 length);


extern void MeshControlInitServerService(bool nvm_start_fresh, uint16 *nvm_offset);

#endif /* __MESH_CONTROL_SERVICE_H__ */

