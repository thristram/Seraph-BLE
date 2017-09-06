/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 * FILE
 *     appearance.h
 *
 *  DESCRIPTION
 *     This file defines macros for commonly used appearance values, which are 
 *     defined by BT SIG.
 *
 *****************************************************************************/

#ifndef __APPEARANCE_H__
#define __APPEARANCE_H__

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Brackets should not be used around the value of a macro. The parser 
 * which creates .c and .h files from .db file doesn't understand  brackets 
 * and will raise syntax errors.
 */

/* For values, refer http://developer.bluetooth.org/gatt/characteristics/Pages/
 * CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
 */

/* CSRmesh Light appearance value */
#define APPEARANCE_CSRMESH_LIGHT_VALUE          0x1060

/* CSRmesh Switch appearance value */
#define APPEARANCE_CSRMESH_SWITCH_VALUE         0x1061

/* CSRmesh Temperature Sensor appearance value*/
#define APPEARANCE_CSRMESH_TEMP_SENSOR_VALUE    0x1062

/* CSRmesh Heater appearance value*/
#define APPEARANCE_CSRMESH_HEATER_VALUE         0x1063

/* CSRmesh Heater appearance value */
#define APPEARANCE_CSRMESH_GATEWAY_VALUE        0x1064

/* CSRmesh Beacon appearance value */
#define APPEARANCE_CSRMESH_BEACON_VALUE         0x1065

/* CSRmesh Beacon proxy appearance value */
#define APPEARANCE_CSRMESH_BEACON_PROXY_VALUE   0x1066

/* CSRmesh Lumicast appearance value */
#define APPEARANCE_CSRMESH_LUMICAST_VALUE       0x1067

/* CSRmesh Bridge appearance value */
#define APPEARANCE_CSRMESH_BRIDGE_VALUE         0x1068

/* CSRmesh Devices Appearance Value - Unknown */
#define APPEARANCE_UNKNOWN_VALUE                0x0000

#endif /* __APPEARANCE_H__ */
