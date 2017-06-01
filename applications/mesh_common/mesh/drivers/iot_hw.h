/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      iot_hw.h
 *
 *  DESCRIPTION
 *      This file defines all the function which interact with IOT hardware
 *      to control power, intensity and colour of RGB LEDs.
 *
 *  NOTES
 *
 ******************************************************************************/
#ifndef __IOT_HW_H__
#define __IOT_HW_H__
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <pio.h>            /* Programmable I/O configuration and control */

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* All the LED Blinking and Buzzer code has been put under these compiler flags
 * Disable these flags at the time of current consumption measurement 
 */

#define PIO_BIT_MASK(pio)       (0x01 << (pio))

/* PIO direction */
#define PIO_DIRECTION_INPUT     (FALSE)
#define PIO_DIRECTION_OUTPUT    (TRUE)

/* PIO state */
#define PIO_STATE_HIGH          (TRUE)
#define PIO_STATE_LOW           (FALSE)

#ifdef CSR101x_A05
#define LED_PWM_RED             (0)
#define LED_PWM_GREEN           (1)
#define LED_PWM_BLUE            (2)
/*
#ifdef NVM_TYPE_FLASH
#define LED_PIO_RED             (5)
#define LED_PIO_GREEN           (6)
#define LED_PIO_BLUE            (7)
#else
  */
#define LED_PIO_RED             (10)
#define LED_PIO_GREEN           (10)
#define LED_PIO_BLUE            (10)
/*#endif*/

#define SW2_PIO                 (1)
#define SW3_PIO                 (0)
#define SW4_PIO                 (4)

#define SW2_MASK                PIO_BIT_MASK(SW2_PIO)
#define SW3_MASK                PIO_BIT_MASK(SW3_PIO)
#define SW4_MASK                PIO_BIT_MASK(SW4_PIO)
#else /* CSR101x_A05 */
#define LED_PIO_RED             (6)
#define LED_PIO_GREEN           (5)
#define LED_PIO_BLUE            (4)

#define SW2_PIO                 (24)
#define SW3_PIO                 (23)
#define SW4_PIO                 (22)
#endif /* CSR101x_A05 */

/* Bit-mask of all the Switch PIOs used by the board. */
#define BUTTONS_BIT_MASK        (SW2_MASK | SW3_MASK | SW4_MASK)

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* This function initialises the Red, Green and Blue LED lines. */
extern void IOTLightControlDeviceInit(void);

/* This function sets the brightness level for white light. */
extern void IOTLightControlDeviceSetLevel(uint8 level);

/* This function sets the colour as per RGB values. */
extern void IOTLightControlDeviceSetColor(uint8 red, uint8 green, uint8 blue);

/* This function sets the Power State of Light. */
extern void IOTLightControlDevicePower(bool power_on);

/* This function Initializes Switch GPIO */
extern void IOTSwitchInit(void);

#ifndef CSR101x_A05
/* This function gets the IOT switch mask */
extern bool GetIOTSwitchMask(uint8 pio, pio_mask_t *switch_mask,
                             uint16 *switch_index);
#endif

/* This function sets colour and blink time for LEDs. */
extern void IOTLightControlDeviceBlink(uint8 red, uint8 green, uint8 blue,
                                       uint8 on_time, uint8 off_time);
#endif /*__IOT_HW_H__*/
