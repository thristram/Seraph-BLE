/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      csr_ota_service.h
 *
 *  DESCRIPTION
 *      Header definitions for OTA service
 *
 *****************************************************************************/

#ifndef __OTA_SERVICE_H__
#define __OTA_SERVICE_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Extract low order byte of 16-bit UUID */
#define LE8_L(x)                       ((x) & 0xff)

/* Extract low order byte of 16-bit UUID */
#define LE8_H(x)                       (((x) >> 8) & 0xff)

/* Highest possible handle for ATT database. */
#define ATT_HIGHEST_POSSIBLE_HANDLE    (0xFFFF)

/* Extract 3rd byte (bits 16-23) of a uint24 variable */
#define THIRD_BYTE(x)                   \
                                       ((uint8)((((uint24)x) >> 16) & 0x0000ff))

/* Extract 2rd byte (bits 8-15) of a uint24 variable */
#define SECOND_BYTE(x)                  \
                                       ((uint8)((((uint24)x) >> 8) & 0x0000ff))

/* Extract least significant byte (bits 0-7) of a uint24 variable */
#define FIRST_BYTE(x)                  ((uint8)(((uint24)x) & 0x0000ff))

/* Convert a word-count to bytes */
#define WORDS_TO_BYTES(_w_)            (_w_ << 1)

/* Convert bytes to word-count*/
#define BYTES_TO_WORDS(_b_)            (((_b_) + 1) >> 1)

/* The Maximum Transmission Unit length supported by this device. */
#define ATT_MTU                        23

/* The maximum user data that can be carried in each radio packet.
 * MTU minus 3 bytes header
 */
#define MAX_DATA_LENGTH                (ATT_MTU-3)

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function initialises the OTA service */
extern void OtaInitServerService(bool nvm_start_fresh, uint16 *nvm_offset);

/* This function indicates whether the OTA module requires
 * the device to reset on client disconnection 
 */
extern bool OtaResetRequired(void);

#endif /* __OTA_SERVICE_H__ */
