/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file cm_appearance.h
 *  \brief Defines all the appearances
 */

#ifndef __CM_APPEARANCE_H__
#define __CM_APPEARANCE_H__

/*! \addtogroup CMTypes
 * @{
 */ 

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*! \brief Brackets should not be used around the value of a macro. The parser 
 *  which creates .c and .h files from .db file doesn't understand  brackets 
 *  and will raise syntax errors.
 */

/*! \brief For values, refer http://developer.bluetooth.org/gatt/characteristics
 *  /Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.
 *  appearance.xml
 */

/*! \brief Unknown */
#define UNKNOWN                                 0

/*! \brief Generic Phone */
#define GENERIC_PHONE                           64

/*! \brief Generic Computer */
#define GENERIC_COMPUTER                        128

/*! \brief  Generic Watch */
#define GENERIC_WATCH                           192
#define SPORTS_WATCH                            193

/*! \brief Generic Clock */
#define GENERIC_CLOCK                           256

/*! \brief Generic Display */
#define GENERIC_DISPLAY                         320

/*! \brief Generic Remote Control */
#define GENERIC_REMOTE_CONTROLLER               384

/*! \brief Generic Eye-glasses */
#define GENERIC_EYE_GLASSES                     448

/*! \brief Generic Tag */
#define GENERIC_TAG                             512

/*! \brief Generic Keyring */
#define GENERIC_KEYRING                         576

/*! \brief Generic Media Player */
#define GENERIC_MEDIA_PLAYER                    640

/*! \brief Generic Barcode Scanner */
#define GENERIC_BARCODE_SCANNER                 704

/*! \brief Generic Thermometer */
#define GENERIC_THERMOMETER                     768
#define EAR                                     769

/*! \brief Generic Heart rate Sensor */
#define GENERIC_HEART_RATE_SENSOR               832
#define HEART_RATE_BELT                         833

/*! \brief Generic Blood Pressure */
#define GENERIC_BLOOD_PRESSURE                  896
#define ARM                                     897
#define WRIST                                   898

/*! \brief Human Interface Device (HID) */
#define HUMAN_INTERFACE_DEVICE                  960
#define HID_KEYBOARD                            961
#define HID_MOUSE                               962
#define HID_JOYSTICK                            963
#define HID_GAMEPAD                             964
#define HID_DIGITIZER_TABLET                    965
#define HID_CARD_READER                         966
#define HID_DIGITAL_PEN                         967
#define HID_BARCODE_SCANNER                     968

/*! \brief Generic Glucose Meter */
#define GENERIC_GLUCOSE_METER                   1024

/*! \brief Generic: Running Walking Sensor */
#define GENERIC_RUNNING_WALKING_SENSOR          1088
#define IN_SHOE                                 1089
#define ON_SHOE                                 1090
#define ON_HIP                                  1091

/*! \brief Generic: Cycling */
#define GENERIC_CYCLING                         1152
#define CYCLING_COMPUTER                        1153
#define SPEED_SENSOR                            1154
#define CADENCE_SENSOR                          1155
#define POWER_SENSOR                            1156
#define SPEED_AND_CADENCE_SENSOR                1157

/*! \brief Generic: Pulse Oximeter */
#define GENERIC_PULSE_OXIMETER                  3136
#define FINGERTIP                               3137
#define WRIST_WORN                              3138

/*! \brief Generic: Weight Scale */
#define GENERIC_WEIGHT_SCALE                    3200

/*! \brief Generic: Outdoor Sports Activity */
#define GENERIC_OUTDOOR_SPORTS_ACTIVITY         5184
#define LOCATION_DISPLAY_DEVICE                 5185
#define LOCATION_NAVIGATION_DISPLAY_DEVICE      5186
#define LOCATION_POD                            5187
#define LOCATION_AND_NAVIGATION_POD             5188

#endif /* __CM_APPEARANCE_H__ */
