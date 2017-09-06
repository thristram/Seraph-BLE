/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gaia_client_service.h
 *
 *  DESCRIPTION
 *      Header file for the gaia client service
 *
 *
 ******************************************************************************/
#ifndef __GAIA_CLIENT_SERVICE_H__
#define __GAIA_CLIENT_SERVICE_H__

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_types.h"
#include "cm_api.h"


/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* This function intializes the Gaia Client Service */
extern void GaiaInitClientService(bool nvm_start_fresh, uint16 *nvm_offset);

/*  This function checks if the gaia client service is connected or not */
extern bool GaiaClientIsConnected(device_handle_id *device_id);

/* This function configures the Gaia Response Endpoint CCD */
extern bool GaiaConfigureResponseConfig(device_handle_id device_id);

/* This function writes the GAIA command endpoint */
extern bool GaiaWriteCommandEndpoint(uint16 len,
                                     uint8* value);

/* This function reads the GAIA response endpoint characteristic */
extern bool GaiaReadResponseEndpoint(device_handle_id device_id);

/* This function checks if the GAIA service handles are discovered */
extern bool GaiaClientIsServiceDiscovered(device_handle_id device_id);

/* This function configures the notifications for Gaia service */
extern void GaiaConfigureService(device_handle_id device_id);

/* This function disconnects the link */
extern void GaiaDeviceDisconnect(void);

#endif
#endif /* __GAIA_CLIENT_SERVICE_H__ */
