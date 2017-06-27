/*! \file  battery.h
 *
 *  \brief Read the battery voltage
 *
 *  Public interface of the code that monitors the battery level and reports
 *  it up to the higher-layer (profile) software.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
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

/*! @} */
#endif /* __BATTERY_H__ */

