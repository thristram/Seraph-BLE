/*! \file config_store.h
 *  \brief Interface to the Configuration Store (CS)
 *
 * Copyright (c) 2009 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __CONFIG_STORE_H__
#define __CONFIG_STORE_H__

#include "types.h"
#include "bluetooth.h"

/*! \addtogroup CS
 *
 * \brief The Configuration Store contains read-only configuration values that
 * are set during device production. Many of the values are used for
 * configuration of the hardware or firmware library, although some keys are
 * set aside for generic application use. This module provides access to those
 * application keys.
 *
 * @{
 */
/*----------------------------------------------------------------------------*
 *      CSReadBdaddr
 */
/*! \brief  Read the device's Bluetooth address
 *
 * Unpacks the device's Bluetooth address from the Configuration Store
 * into the buffer passed, in a format suitable for the rest of
 * the firmware.
 *
 * \param  bdaddr  A pointer to the storage buffer supplied by the application
 *
 * \returns  TRUE for success, FALSE if some error
 */
/*---------------------------------------------------------------------------*/
bool CSReadBdaddr(BD_ADDR_T *bdaddr);

/*----------------------------------------------------------------------------*
 *      CSReadTxPower
 */
/*! \brief  Read the TX Power setting.
 *
 * Return the value stored in the TX Power CS key. This value is an integer, which
 * corresponds to fixed steps in the transmit power level used by the device.
 * It DOES NOT directly define a power level in dBm, as this will be board/design-specific
 * Therefore if an application needs to determine an absolute power level in dBm
 * the customer will need to perform additional calibration of the final product
 * to determine how each step maps to an output power.
 *
 * \returns  The transmit power level
 */
/*---------------------------------------------------------------------------*/
uint16 CSReadTxPower(void);

/*----------------------------------------------------------------------------*
 *      CSReadUserKey
 */
/*! \brief  Read a key from the user key set.
 *
 * Indexes into the array of user keys and returns the appropriate
 * value.  Raises a fault if the index is out of range.
 *
 * \param  key_index Index into the array of user keys
 *
 * \returns  Value stored in the Configuration Store User Keys
 */
/*---------------------------------------------------------------------------*/
uint16 CSReadUserKey(uint16 key_index);

/*! @} */
#endif /* __CONFIG_STORE_H__ */
