/*! \file sleep.h
 *  \brief Control the CSR100x/CSR101x sleep states
 *
 * This file contains the implementation of sleep modes (Awake, Shallow Sleep,
 * and Deep Sleep) and sleep states (Hibernate and Dormant).
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd 2009-2011.
 */

#ifndef __SLEEP_H__
#define __SLEEP_H__

#include "time.h"

/*! \addtogroup PWRMGMT
 * @{
 */
/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief The state the chip woke up from.
 *
 * If the application requests Hibernate or Dormant, and the power is
 * subsequently removed, the sleep state recorded on the next wake event will
 * be dependent on how long power was lost for (i.e. whether or not the data in
 * the persistent memory was still valid).
 */
typedef enum
{
    /*! The device powered up after a long time without power */
    sleep_state_cold_powerup,

    /*! The device powered up after a short time without power (less than ~1
     * minute, based on how long data remains valid in the persistent memory)
     */
    sleep_state_warm_powerup,

    /*! The device powered up after being placed into the Dormant state */
    sleep_state_dormant,

    /*! The device powered up after being placed into the Hibernate state */
    sleep_state_hibernate,

    /*! The device powered up after an application-triggered warm reset */
    sleep_state_warm_reset,

} sleep_state;


/*! \brief The power state used by the chip when the radio is idle.
 *
 * These are the values that can be written to the sleep_mode Configuration
 * Store key, and also requested by the application using the SleepModeChange()
 * API call.
 *
 * Three sleep modes are supported: Deep Sleep; Shallow Sleep; Always Awake
 *
 * Deep Sleep is the most useful, and ensures that the chip is using the 32kHz
 * clock when the application, firmware, and radio are idle. The firmware will
 * automatically handle the transition between the Deep Sleep and Awake states
 * subject to system events (e.g. radio activity, expiry of timers, external
 * interrupt sources, etc.)
 *
 * Shallow Sleep is useful in certain application that want to use the main
 * processor or the 8051 PIO Controller to perform high-speed or low-latency
 * I/O. In this mode, the chip remains running off the 16MHz clock at all times
 * therefore there is little latency when the chip needs to wake up. However
 * this mode will increase the overall current consumption quite considerably.
 * Whilst in Shallow Sleep the internal Power Supply will run in an optimised
 * state, in order to reduce its current consumption, at any time the radio is
 * not in use. As with Deep Sleep, in this state the firmware will manage the
 * power supply settings subject to system events.
 *
 * Always Awake is of limited use. The chip remains running off the 16MHz clock
 * and the internal power supply runs in its normal "awake" state where it is
 * capable of supplying full power to the radio, even if the radio is not in
 * use.
 *
 * There are two methods of controlling the sleep mode used by the firmware. The
 * first is to set the sleep_mode CS key. The value set in this key will be
 * used by the firmware from power-up until (and if) the application requests an
 * alternate mode by calling the SleepModeChange() API. If the application does
 * not need to change the selected sleep mode at runtime then it is sufficient
 * to just set the CS key.
 */
typedef enum
{
    /*! Always Awake */
    sleep_mode_never,

    /*! Deep Sleep */
    sleep_mode_deep,

    /*! Shallow Sleep */
    sleep_mode_shallow,

} sleep_mode;


/*! \brief The WAKE pin mode controls which edges the CSR100x/CSR101x will wake on (if
 * required).
 */
typedef enum
{
    /*! Disable the WAKE pin */
    wakepin_mode_disable,

    /*! Pulling the WAKE pin low keeps the CSR100x/CSR101x awake */
    wakepin_mode_low_level,

    /*! Pulling the WAKE pin high keeps the CSR100x/CSR101x awake */
    wakepin_mode_high_level
} wakepin_mode;


/*============================================================================*
Public Data
*============================================================================*/

/*============================================================================*
Public Function Implementations
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  SleepModeChange
 */
/*! \brief  Tell the firmware to use a particular sleep mode for all subsequent
 *          periods when it is able to sleep.
 *
 *  \param new_mode     The new sleep mode to use
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void SleepModeChange(sleep_mode new_mode);


/*----------------------------------------------------------------------------*
 *  SleepRequest
 */
/*! \brief  Request a transition to one of the sleep states Hibernate or Dormant,
 *          as specified by new_sleep_state.
 *
 *  Hibernate and Dormant states can be requested irrespective of the
 *  currently selected sleep mode (i.e. they are allowed even if the sleep mode
 *  is set to sleep_mode_never).
 *
 *  The wakeup condition is specified by the wake_active_high parameter.
 *  Set this to TRUE if the chip should wake when the WAKE pin is at logic
 *  level 1, or set to FALSE if the chip should wake at logic level 0.
 *  NOTE: The CSR100x/CSR101x does not have an internal pull-up or pull-down
 *  resistor on the WAKE pin therefore an external resistor should be used.
 *
 *  When requesting Hibernate, a non-zero duration must be provided. This
 *  will be added onto the current system time to calculate the next wakeup
 *  time. The device will automatically wake up after the time has passed,
 *  or sooner if there is an external wake event on the dedicated WAKE pin.
 *  hibernate_duration is expressed in microseconds. However, the minimum
 *  allowed duration is 2^20 microseconds (1.048576s). In practise most
 *  real-world applications will actually require a much longer duration in
 *  order to realise any additional power-savings over Deep Sleep mode. The
 *  firmware will raise a fault and panic if the duration is too small.
 *
 *  The requested hibernate_duration is a suggested minimum. The system will
 *  sleep for at least that time, unless there is an external WAKE event,
 *  but it will always be slightly longer until the chip has completely
 *  woken up, due to system startup time. The exact time from an application
 *  requesting Hibernate until it gets called again at wakeup cannot be
 *  guaranteed.
 *
 *  The hibernate_duration is ignored if the application requests Dormant.
 *  In this state the device will *only* wake up if there is an external
 *  event on the WAKE pin.
 *
 *  WARNING! This function will *NOT* return. The device will go to a
 *           low power state and will perform a full RAM reset when
 *           it wakes up again. Any information that you need to
 *           save must be stored in the persistent memory before
 *           requesting the low power state.
 *
 *  \param new_sleep_state      The requested sleep state (Hibernate or Dormant)
 *  \param wake_active_high     WAKE pin polarity (acitve high or active low)
 *  \param hibernate_duration   Time (in microseconds) to hibernate for
 *  \returns NEVER RETURNS!
 */
/*---------------------------------------------------------------------------*/
extern void SleepRequest(sleep_state new_sleep_state, bool wake_active_high,
                         time48 hibernate_duration);


/*----------------------------------------------------------------------------*
 *  SleepWakePinEnable
 */
/*! \brief Set the operating mode of the WAKE pin.
 *
 *  Allows the user application to configure the operating mode of the chip's
 *  WAKE pin.
 *
 *  \param mode     The new WAKE pin mode to use
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void SleepWakePinEnable(wakepin_mode mode);


/*----------------------------------------------------------------------------*
 *  SleepWakePinStatus
 */
/*! \brief Return the current state of the WAKE pin.
 *
 *  \returns TRUE if WAKE pin is high else FALSE.
 */
/*---------------------------------------------------------------------------*/
extern bool SleepWakePinStatus(void);


/*----------------------------------------------------------------------------*
 *  SleepWakeOnUartRX
 */
/*! \brief Wake from Deep Sleep if the UART sees incoming data. On enabling the
 *  UART, the default is to wake on incoming data. This function may only be
 *  called when the UART is enabled either via \ref DebugInit or
 *  \ref UartEnable.
 *
 *  \param enable   TRUE to wake from Deep Sleep on UART RX, FALSE to remain asleep
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void SleepWakeOnUartRX(bool enable);

/*! @} */
#endif /* __SLEEP_H__ */
