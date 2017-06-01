/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Firmware_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Firmware model 
 */
/******************************************************************************/

#ifndef __FIRMWARE_CLIENT_H__
#define __FIRMWARE_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Firmware_Client
 * @{
 */
#include "firmware_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * FirmwareModelClientInit
 */
/*! \brief 
 *      Initialises Firmware Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult FirmwareModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * FirmwareGetVersion
 */
/*! \brief Get firmwre verison: Upon receiving a FIRMWARE_GET_VERSION the device reponds with a FIRMWARE_VERSION message containing the current firmware version
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_FIRMWARE_VERSION 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_FIRMWARE_GET_VERSION_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult FirmwareGetVersion(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_FIRMWARE_GET_VERSION_T *p_params );

/*----------------------------------------------------------------------------*
 * FirmwareUpdateRequired
 */
/*! \brief Requesting a firmware update. Upon receiving this message, the device moves to a state where it is ready for receiving a firmware update
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_FIRMWARE_UPDATE_ACKNOWLEDGED 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_FIRMWARE_UPDATE_REQUIRED_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult FirmwareUpdateRequired(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_FIRMWARE_UPDATE_REQUIRED_T *p_params );

/*!@} */
#endif /* __FIRMWARE_CLIENT_H__ */

