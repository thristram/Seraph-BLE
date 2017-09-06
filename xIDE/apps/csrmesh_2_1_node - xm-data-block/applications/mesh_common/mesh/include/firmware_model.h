/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Firmware_model.h
 *
 *  \brief Defines CSRmesh Firmware Model specific data structures\n
 */
/******************************************************************************/

#ifndef __FIRMWARE_MODEL_H__
#define __FIRMWARE_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Firmware_Model 
 *  \brief Firmware Model API
 *
 * The Firmware Model allows a device to expose it's current firmware version revision and to accept new firmware downloads. There is no update method defined by this specification, this is left as an implementation detail for the firmware developers.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Firmware Model message types */

/*! \brief Get firmwre verison: Upon receiving a FIRMWARE_GET_VERSION the device reponds with a FIRMWARE_VERSION message containing the current firmware version */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_FIRMWARE_GET_VERSION_T;

/*! \brief Firmware version information */
typedef struct
{
    CsrUint16 majorversion; /*!< \brief Firmware Major Version */
    CsrUint16 minorversion; /*!< \brief Firmware Minor Version */
    CsrUint8 reserved[4/size_in_bytes(CsrUint8)]; /*!< \brief Not used */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_FIRMWARE_VERSION_T;

/*! \brief Requesting a firmware update. Upon receiving this message, the device moves to a state where it is ready for receiving a firmware update */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_FIRMWARE_UPDATE_REQUIRED_T;

/*! \brief Acknowledgement message to the firmware update request */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_FIRMWARE_UPDATE_ACKNOWLEDGED_T;


/*!@} */
#endif /* __FIRMWARE_MODEL_H__ */

