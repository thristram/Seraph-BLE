/******************************************************************************
 *  Copyright 2012 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gap_service.h
 *
 *  DESCRIPTION
 *      Header definitions for GAP service
 *
 *****************************************************************************/

#ifndef __GAP_SERVICE_H__
#define __GAP_SERVICE_H__

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

/* This function initialises the GAP service */
extern void GapInitServerService(bool nvm_start_fresh, uint16 *nvm_offset);

/* This function is used to get the reference to the name 
 * array, which contains AD Type and device name
 */
extern uint8 *GapGetNameAndLength(uint16 *p_name_length);

#endif /* __GAP_SERVICE_H__ */
