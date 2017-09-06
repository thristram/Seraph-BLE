/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      core_mesh_handler.h
 *
 *
 ******************************************************************************/
#ifndef __CORE_MESH_HANDLER_H__
#define __CORE_MESH_HANDLER_H__

#include <csr_types.h>
#include <csr_mesh_types.h>

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* CSRmesh device association state */
typedef enum
{
    /* Application Initial State */
    app_state_not_associated = 0,

    /* Application state association started */
    app_state_association_started,

    /* Application state associated */
    app_state_associated,
} app_association_state;

/* Mesh Handler Data Structure */
typedef struct
{
    CSR_MESH_DEVICE_APPEARANCE_T   *appearance;
    CSR_MESH_VID_PID_VERSION_T     *vid_pid_info;
    app_association_state           assoc_state;
    CSR_MESH_TRANSMIT_STATE_T       bearer_tx_state;
    uint8                           ttl_value;
}MESH_HANDLER_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the core mesh handler */
extern void MeshHandlerInit(MESH_HANDLER_DATA_T* mesh_handler_data);

/* This function kick starts a timer to send periodic CSRmesh UUID Messages
 * and starts blinking Blue LED to visually indicate association ready status
 */
extern void InitiateAssociation(void);

/* This function updates the relay and promiscuous mode of the GATT and
 * and the LE Advert bearers
 */
extern void AppUpdateBearerState(CSR_MESH_TRANSMIT_STATE_T *p_bearer_state);

/* This function returns the association state. */
extern app_association_state AppGetAssociatedState(void);

/* This function returns the stored bearer state. */
extern CSR_MESH_TRANSMIT_STATE_T AppGetStoredBearerState(void);

/* This function returns the status of the GATT bearer */
extern bool IsGattBearerEnabled(void);

/* This function returns the TTL value stored in the application */
extern uint8 AppGetCurrentTTL(void);

/* This function sets the Tx power onto the firmware based on the power dbm */
extern void AppSetTxPower(CsrInt8 power);

/* This function initializes the sequence cache with the mesh stack */
extern void AppInitializeSeqCache(void);

/* This function clears the sequence cache with the mesh stack */
extern void AppClearSeqCache(void);

#endif /* __CORE_MESH_HANDLER_H__ */
