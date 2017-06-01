/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 * FILE
 *      user_config.h
 *
 * DESCRIPTION
 *      This file contains definitions which will enable customization of the
 *      application.
 *
 ******************************************************************************/

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/
/* Application version */
#define APP_MAJOR_VERSION       (2)
#define APP_MINOR_VERSION       (1)
#define APP_NEW_VERSION         (0)

/* Application NVM version. This version is used to keep the compatibility of
 * NVM contents with the application version. This value needs to be modified
 * only if the new version of the application has a different NVM structure
 * than the previous version (such as number of groups supported) that can
 * shift the offsets of the currently stored parameters.
 * If the application NVM version has changed, it could still read the values
 * from the old Offsets and store into new offsets.
 * This application currently erases all the NVM values if the NVM version has
 * changed.
 */
#define APP_NVM_VERSION         (1)

#define IMAGE_TYPE              (3)

#define CSR_MESH_HEATER_PID     (0x1063)

/* Vendor ID for CSR */
#define APP_VENDOR_ID           (0x0A12)

/* Product ID. */
#define APP_PRODUCT_ID          (CSR_MESH_HEATER_PID)

/* Number of model groups supported 
 * The switch application uses the Group ID assigned to the switch model as
 * the destination address for sending light control messages.
 * Since we only have one set of buttons(brightness control) and a switch(power 
 * control) on the IOT lighting board, to control destination devices, we
 * support only one group for all the supported models.
 */
#define MAX_MODEL_GROUPS        (4)

/* Version Number. */
#define APP_VERSION             (((uint32)(APP_MAJOR_VERSION & 0xFF) << 16) | \
                                 ((uint32)(APP_MINOR_VERSION & 0xFF)))

/* Default TTL value used in app */
#define DEFAULT_TTL_VALUE        (0x32)

/* Enable application debug logging on UART */
/*#define DEBUG_ENABLE*/
#define UART_BOARD_ENABLE
/* Enable Static Random Address for bridge connectable advertisements */
#define USE_STATIC_RANDOM_ADDRESS

/* Enable the this definition to use an authorisation code for association 
#define USE_AUTHORISATION_CODE */

/* Enable Device UUID Advertisements */
#define ENABLE_DEVICE_UUID_ADVERTS 

/* Enable Sensor model support 
#define ENABLE_SENSOR_MODEL*/

/* Enable Attention model support 
#define ENABLE_ATTENTION_MODEL*/

/* Enable Battery model support 
#define ENABLE_BATTERY_MODEL*/

/* Duty Cycle support Parameters */
/* Enable duty cycle change support */
#define ENABLE_DUTY_CYCLE_CHANGE_SUPPORT

/* Default scan duty cycle in 0.1% with values ranging from 1-1000 */
#define DEFAULT_SCAN_DUTY_CYCLE        (20)

/* scan duty cycle in 0.1% when device set to active scan mode. The device  
 * is present in this mode before association and on attention, data stream in 
 * progress or during when watchdog is enabled.
 */
#define HIGH_SCAN_DUTY_CYCLE           (1000)

/* Msg Retransmission parameters */
/* Maximum time the message should be retransmitted */
#define MAX_RETRANSMISSION_TIME        (7500 * MILLISECOND)

/* Message Retransmit timer in milliseconds */
#define RETRANSMIT_INTERVAL            (500 * MILLISECOND)

/* Enable the Acknowledge mode */
/* #define ENABLE_ACK_MODE */

/* The below models are not enabled by default with constraint of space on the
 * CSR101x platform
 */

/* Enable Data model support */
#define ENABLE_DATA_MODEL


#ifndef CSR101x_A05
/* Battery threshold voltage */
#define BATTERY_THRESHOLD_VOLTAGE      (0x834)

/* Enable Ping model support */
#define ENABLE_PING_MODEL

/* Enable LOT model support */
#define ENABLE_LOT_MODEL



/* Enable Time model support */
#define ENABLE_TIME_MODEL

/* Enable Action model support */
#define ENABLE_ACTION_MODEL 

#if defined(ENABLE_ACTION_MODEL)
/* Enable Time model support */
#define ENABLE_TIME_MODEL 

/* Maximum Actions supported. Each action takes 24 bytes of data to be stored.
 * On increasing number of actions the NVM space for the same need to be 
 * increased accordingly.
 */
#define MAX_ACTIONS_SUPPORTED          (6)
#endif /* ENABLE_ACTION_MODEL */
#else
/* This flag should be enabled for supporting OTA on CSR101x Devices */
/* #define OTAU_BOOTLOADER */
#endif /* CSR101x_A05 */

/* Device name and its length
 * Note: Do not increase device name length beyond (DEFAULT_ATT_MTU -3 = 20) 
 * octets as GAP service at the moment doesn't support handling of Prepare 
 * write and Execute write procedures.
 */

/* Macro for the device name */
#define DEVICE_NAME                                 "Heater 2.1"

#define APPEARANCE                                  UNKNOWN

/* Maximum Length of Device Name
 * Note: Do not increase device name length beyond (DEFAULT_ATT_MTU -3 = 20)
 * octets as GAP service at the moment doesn't support handling of Prepare
 * write and Execute write procedures.
 */
#define DEVICE_NAME_LENGTH                          (20)

/* Bonding Requirement */
#define GAP_MODE_BOND                               gap_mode_bond_no

/* Security Level Requirements */
#define GAP_MODE_SECURITY                   gap_mode_security_unauthenticate

/* Maximum active connections */
#define MAX_CONNECTIONS                             (1)

/* Maximum paired devices */
#define MAX_PAIRED_DEVICES                          (1)

/* Maximum number of server services supported */
#if defined(CSR101x_A05) && !defined(OTAU_BOOTLOADER)
#define MAX_SERVER_SERVICES                         (3)
#else
#define MAX_SERVER_SERVICES                         (4)
#endif

/* Initial diversifier */
#define DIVERSIFIER                                 (0)

#ifdef ENABLE_LOT_MODEL
#define LOT_INTEREST_ADVERT_COUNT                   (100)
#endif /* ENABLE_LOT_MODEL */

/* Sourse and Sequence cache slot size */
#define SRC_SEQ_CACHE_SLOT_SIZE                     (0)

#endif /* __USER_CONFIG_H__ */

