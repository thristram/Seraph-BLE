/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia_service.h
 *  \brief This file defines GAIA service implementation
 *
 */
#ifndef __GAIA_SERVICE_H__
#define __GAIA_SERVICE_H__


/*! \addtogroup GAIA_Service
 * @{
 */

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/

/*! \brief Enum defining notification control */
typedef enum
{
    /*! \brief Disabled */
    gaia_notif_disabled = 0,

    /*! \brief Enabled */
    gaia_notif_enabled = 1,
} gaia_notification_control;

/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------
 *  GAIAInitServerService
 *----------------------------------------------------------------------------*/
/*! \brief Initialises the GAIA service
 *
 * This function initialises the GAIA service
 * \param[in] nvm_start_fresh Boolean variable to check if NVM has been not been written before
 * \param[out] nvm_offset Pointer to NVM offset
 * \returns Nothing
 *
 */
extern void GAIAInitServerService(bool nvm_start_fresh, uint16 *nvm_offset);

/*----------------------------------------------------------------------------
 *  GAIASendNotification
 *----------------------------------------------------------------------------*/
/*! \brief Sends notification to the host
 *
 *  This function sends notification to the host
 * \param[in] device_id id of the connected device
 * \param[in] packet_length Payload length
 * \param[in] packet Pointer to payload data
 * \returns \ref cm_status_code
 *
 */
extern cm_status_code GAIASendNotification( device_handle_id device_id,
                                            uint16 packet_length,
                                            uint8 *packet);

/*!@} */
#endif /* __GAIA_SERVICE_H__ */
