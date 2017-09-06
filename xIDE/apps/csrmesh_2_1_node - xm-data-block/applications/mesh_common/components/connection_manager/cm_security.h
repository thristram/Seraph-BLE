/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_security.h
 *
 *  DESCRIPTION
 *      Header file for the connection manager security
 *
 *
 ******************************************************************************/

#ifndef __CM_SECURITY_H__
#define __CM_SECURITY_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <bluetooth.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_hal.h"
#include "cm_types.h"

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/*! \brief Bonded device information type */
typedef struct
{
    /* IMPORTANT NOTE:
     * Adding any member variable affects the size of the CM_CONN_INFO_T.
     * Thus it is required to update the size of the CM_SIZEOF_CONN_INFO
     * in cm_types.h. Otherwise system panic while accessing the invalid 
     * pointer
     */

    bool                                        bonded;                     /*!< \brief  Boolean flag */

    TYPED_BD_ADDR_T                             bd_addr;                    /*!< \brief  Bluetooth Address */

    cm_peer_con_role                            peer_con_role;              /*!< \brief  GAP role */

    h_sm_keyset_t                               keys;                       /*!< \brief  SM Key Set */

} CM_BONDED_DEVICE_INFO_T;


/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* Initialises the CM security manager entity  */
extern void CMSecurityInit(CM_INIT_PARAMS_T *cm_init_params);

/* Handles the firmware events related to the security role */
extern void CMSecurityHandleProcessLMEvent(h_msg_t *msg); 

/* Gets the bonded device information */
extern bool CMGetBondedDevice(bond_handle_id bond_id,
                              CM_BONDED_DEVICE_INFO_T *device);

/* Gets the maximum number of bonded devices */
extern uint16 CMGetMaxBondedDevices(void);

/* Deletes the bonding information  */
extern bool ClearBondingInfo(bond_handle_id bond_id);

#endif /* __CM_SECURITY_H__ */
