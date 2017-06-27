/*! \file  thermometer.h
 *
 *  \brief Read the temperature sensor
 *
 *  Public interface of the code that reads the temperature sensor
 *  in degrees centigrade and reports  it up to the higher-layer 
 *  (profile) software.
 * 
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __THERMOMETER_H__
#define __THERMOMETER_H__

/*! \addtogroup THERM
 * @{
 */
/*============================================================================*
Public Function Implementations
*============================================================================*/

/*!
 * \brief    Allows the application to query the thermometer
 *
 * Note that this function does not trigger a temperature measurement,
 * it returns a cached value of the temperature that the firmware updates
 * every watchdog period, by default every 15 seconds.
 *
 * \returns  The cached temperature in degrees centigrade.
 */
extern int16 ThermometerReadTemperature(void);

/*! @} */
#endif /* __THERMOMETER_H__ */

