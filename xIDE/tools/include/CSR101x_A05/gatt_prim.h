/*! \file gatt_prim.h
 *
 *  \brief Generic Attribute Profile application interface
 *
 * Copyright (c) 2010 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __GATT_PRIM_H__
#define __GATT_PRIM_H__

/*============================================================================*
ANSI C & System-wide Header Files
*============================================================================*/
#include "types.h"
#include "bluetooth.h"


/*============================================================================*
Interface Header Files
*============================================================================*/
#include "att_prim.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/* Convert ATT result to GATT */
#define GATT_STATUS_FROM_ATT_RESULT(_r) ((_r)?(STATUS_GROUP_GATT+((_r)&0x3f)):sys_status_success)

/*
 * Flags for Write Request
 */
/*! \addtogroup GATTfwr
 *
 *  \brief Aligned to the flags defined by ATT 
 *
 *  @{ */

/*! Send Write Request to the server */
#define GATT_WRITE_REQUEST       ATT_WRITE_REQUEST
/*! Send Write Command to the server. */
#define GATT_WRITE_COMMAND       ATT_WRITE_COMMAND
/*! Send Signed Write to the server. Only Write Command can be signed. */
#define GATT_WRITE_SIGNED        ATT_WRITE_SIGNED
/*! @} */

/*! GATT process type */
typedef enum gatt_proc_tag
{
    GATT_NO_PROC = 0,/* No procedure on-going */
    GATT_EXCHANGE_MTU_PROC,
    GATT_DISC_ALL_PRI_SERVICES_PROC,
    GATT_DISC_PRI_SERVICE_BY_UUID_PROC,
    GATT_FIND_INCLUDED_SERVICES_PROC,
    GATT_DISC_SERVICE_CHAR_PROC,
    GATT_DISC_ALL_CHAR_DESCRIPTORS_PROC,
    GATT_READ_CHAR_VAL_PROC,
    GATT_READ_FIRST_CHAR_USING_UUID_PROC,
    GATT_READ_NEXT_CHAR_USING_UUID_PROC,
    GATT_READ_LONG_CHAR_VAL_PROC,
    GATT_READ_MULTI_CHAR_VAL_PROC,
    GATT_WRITE_CHAR_VAL_PROC,
    GATT_WRITE_LONG_CHAR_VAL_PROC,
    GATT_CHAR_VAL_NOTI_PROC,
    GATT_CHAR_VAL_IND_PROC,
    GATT_ATT_PREPARE_WRITE_PROC,
    GATT_ATT_EXECUTE_WRITE_PROC
} GATT_PROC_T;



#endif /* __GATT_PRIM_H__ */
