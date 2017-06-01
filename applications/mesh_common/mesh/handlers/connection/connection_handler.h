/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      connection_handler.h
 *
 *
 ******************************************************************************/
#ifndef __CONNECTION_HANDLER_H__
#define __CONNECTION_HANDLER_H__

#include "cm_types.h"
/*============================================================================*
 *  Public Definitions
 *============================================================================*/

typedef enum
{
    /* Enters when the application is not connected to remote host */
    app_state_idle = 0,

    /* Enters when slow Undirected advertisements are configured */
    app_state_advertising,

    /* Enters when connection is established with the host */
    app_state_connected,

    /* Enters when disconnect is initiated by the application */
    app_state_disconnecting
} app_state;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function initialises the connection manager. */
extern void AppCMInit(bool nvm_fresh, uint16* nvm_offset);

/* This function returns the CM device id of the connected device */
extern uint16 GetConnectedDeviceId(void);

/* This function is used to set the state of the application */
extern void AppSetState(app_state new_state);

/* This function initialises the GATT Data */
extern void InitialiseGattData(void);

/* This function triggers the connection parameter update procedure */
extern void AppRequestConnParamUpdate(device_handle_id device_id);

/* This function returns the pairing status of the connected device */
extern bool GetDevicePairingStatus(void);

/* This function sets the value of otau_in_progress flag */
#ifdef GAIA_OTAU_SUPPORT
extern void GaiaOtauUpgradedApplication(bool upgraded_application);
extern bool IsGaiaOtauUpgradedApplication(void);
extern uint16 GetConnInterval(void);
#endif

#ifdef GAIA_OTAU_RELAY_SUPPORT
extern void AppConfigureNextService(uint16 device_id);
extern void AppSendConnectRequest(TYPED_BD_ADDR_T conn_addr);
extern TYPED_BD_ADDR_T AppGetLastConnectedAddress(void);
extern void SendConnectRequest(void);
#endif

#if defined(GAIA_OTAU_SUPPORT) || defined(GAIA_OTAU_RELAY_SUPPORT)
extern void GaiaOtauInProgress(bool in_progress);
extern bool IsGaiaOtauInProgress(void);
#endif

#ifdef CSR101x_A05
/* This function handles the OTA reset request */
extern void AppOtaReset(void);
#endif

#endif /* __CONNECTION_HANDLER_H__ */
