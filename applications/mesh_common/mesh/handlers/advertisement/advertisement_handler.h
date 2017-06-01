/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      advertisement_handler.h
 *
 *  DESCRIPTION
 *      Header file for CSRmesh App's Advertisement related routines
 *
 ******************************************************************************/

#ifndef __ADVERTISEMENT_HANDLER_H__
#define __ADVERTISEMENT_HANDLER_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <time.h>
#include <gatt_prim.h>

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Extract low order byte of 16-bit UUID */
#define LE8_L(x)                                ((x) & 0xff)

/* Extract low order byte of 16-bit UUID */
#define LE8_H(x)                                (((x) >> 8) & 0xff)

/* Acceptable shortened device name length that can be sent in advertisement
 * data
*/
#define SHORTENED_DEV_NAME_LEN                  (8)

/* length of Tx Power prefixed with 'Tx Power' AD Type */
#define TX_POWER_VALUE_LENGTH                   (2)

/* length of 'device appearance' AD Type */
#define DEVICE_APPEARANCE_LENGTH                (2)

/* AD Type for Appearance */
#define AD_TYPE_APPEARANCE                      (0x19)

/* Connectable advert interval.
 * Adjust the interval to run on a gross timebase of 1250ms to allow CSRmesh
 * to complete atleast one scan interval between conn-ads.
 * The scan duty cycles set will be more accurate when the time spent in the 
 * conn-ad (5ms) and the random jitter variation are accounted for in the
 * advertising interval.
 */
#define GATT_ADVERT_GROSS_INTERVAL              (1245 * MILLISECOND)
#define GATT_ADVERT_RANDOM                      (10 * MILLISECOND)

#define ADVERT_INTERVAL                         (GATT_ADVERT_GROSS_INTERVAL \
                                                - (GATT_ADVERT_RANDOM/2))

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
#ifdef ENABLE_LOT_MODEL
/* This function triggers the connectable LOT advertisements */
extern void GattTriggerLOTAdverts(uint8* lot_interest_service_id);
#endif
                                                
/* This function triggers the connectable advertisements */
extern void GattTriggerConnectableAdverts(uint8* lot_interest_service_id);

/* This function is used to stop on-going advertisements */
extern void GattStopAdverts(void);

#ifdef USE_STATIC_RANDOM_ADDRESS
extern void GattGetRandomAddress(BD_ADDR_T* p_addr);
extern void GattSetRandomAddress(void);
#endif /* USE_STATIC_RANDOM_ADDRESS */

#endif /* __ADVERTISEMENT_HANDLER_H__ */
