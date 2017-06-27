/*! \file  battery.h
 *
 *  \brief Read the battery voltage
 *
 *  Public interface of the code that monitors the battery level and reports
 *  it up to the higher-layer (profile) software.
 *
 * Copyright (c) 2010 - 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __BATTERY_H__
#define __BATTERY_H__

/*! \addtogroup BAT
 * @{
 */
/*============================================================================*
Public Definitions
*============================================================================*/

/*============================================================================*
Public Data
*============================================================================*/

/*============================================================================*
Public Function Implementations
*============================================================================*/

/*!
 * \brief    Allows the application to query the battery voltage.
 * \returns  The current battery voltage in mV.
 */
extern uint16 BatteryReadVoltage(void);

/*!
 * \brief    Allows the application to query the Low Battery Threshold CSKey.
 * \returns  The value of the CS Key "Low Battery Threshold".
 */
extern uint16 BatteryReadLowThreshold(void);

/*! @} */
#endif /* __BATTERY_H__ */

