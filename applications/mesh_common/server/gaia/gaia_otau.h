/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file   gaia_otau.h
 * \brief   GAIA OTAu internal API, used by the GAIA library
 *
 */
#ifndef GAIA_OTAU_H_
#define GAIA_OTAU_H_


/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <store_update_msg.h>
#include <cm_types.h>

/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/*!
 * \brief Handle a GAIA OTAu VM Upgrade Control command
 *
 * \param[in] device_id device ID of the device which sent the command
 * \param[in] dataSize size of the payload data
 * \param[in] data pointer to the payload data
 * \return None
 */
extern void GaiaOtauHandleVmUpgradeControlCommand(  device_handle_id device_id,
                                                    uint16 payload_size,
                                                    uint8 *payload);

/*!
 * \brief Initialise the GAIA OTAu library
 *
 * \param[in] nvm_start_fresh Boolean variable to check if NVM has been not been written before
 * \param[out] nvm_offset Pointer to NVM offset
 * \returns Nothing
 */
extern void GaiaOtauInit(bool nvm_start_fresh, uint16 *nvm_offset);

/*!
 * \brief Notify the GAIA OTAu library when a connection changes
 *
 * \param[in] cm_event_data pointer to a connection event message
 * \returns Nothing
 */
extern void GaiaOtauHandleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data);

#endif /* GAIA_OTAU_H_ */
