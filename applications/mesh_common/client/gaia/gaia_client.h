/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia_client.h
 *  \brief This file defines header file for GAIA protocol
 *
 */

#ifndef __GAIA_CLIENT_H__
#define __GAIA_CLIENT_H__

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*! \addtogroup GAIA_Service
 * @{
 */

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/


/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------
 *  GAIAClientProcessCommand
 *----------------------------------------------------------------------------*/
/*! \brief Process a GAIA command
 *
 * This function processess GAIA command
 * \param[in] payload_len Payload length
 * \param[in] payload Pointer to payload data
 * \returns Nothing
 *
 */
extern void GAIAClientProcessCommand(uint16 payload_len, uint8 *payload);

/*----------------------------------------------------------------------------
 *  GaiaClientSendCommandPacket
 *----------------------------------------------------------------------------*/
/*! \brief Prepares a GAIA packet with header and given payload
 *
 *  Prepares a GAIA packet with header and given payload and sends BLE
 *  notification to host
 * \param[in] size_payload Payload length
 * \param[in] payload Pointer to payload data
 * \returns Nothing
 *
 */
extern void GaiaClientSendCommandPacket(uint16 size_payload, uint8 *payload);

/*----------------------------------------------------------------------------
 *  GaiaClientSendDisconnectPacket
 *----------------------------------------------------------------------------*/
/*! \brief Sends VM Disconnect
 *
 *  Disconnects the Link
 * \returns Nothing
 *
 */
extern void GaiaClientSendDisconnectPacket(void);

/*----------------------------------------------------------------------------
 *  GaiaClientSendConnectPacket
 *----------------------------------------------------------------------------*/
/*! \brief Sends VM Connect
 *
 *  Connects the link
 * \returns Nothing
 *
 */
extern void GaiaClientSendConnectPacket(void);

/*----------------------------------------------------------------------------
 *  SetAcknowledgement
 *----------------------------------------------------------------------------*/
/*! \brief Sends Acknowledgement
 *
 * \param[in] status status of operation
 * \returns Nothing
 *
 */
extern void SetAcknowledgement(uint8 status);


#endif
#endif /* __GAIA_H__ */
