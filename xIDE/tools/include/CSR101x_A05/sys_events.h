/*! \file sys_events.h
 *  \brief  System Event definitions and declarations
 *
 *  This file contains the types and definitions for CSR1000 system
 *  events.
 *
 * Copyright (c) 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __SYS_EVENTS_H__
#define __SYS_EVENTS_H__

/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief System event codes */
typedef enum
{
    /*! The system was woken by an edge on the WAKE pin. See #wakeup_data for
     * associated data.
     */
    sys_event_wakeup,

    /*! The system battery voltage has moved above or below the monitoring
     *  threshold. See #battery_low_data for associated data.
     */
    sys_event_battery_low,

    /*! One or more PIOs specified by PioSetEventMask() have changed
     * input level. See #pio_changed_data for associated data.
     */
    sys_event_pio_changed,

    /*! An event was received from the 8051 PIO Controller.
     * See #pio_ctrlr_data for associated data.
     */
    sys_event_pio_ctrlr

} sys_event_id;

/*! \brief The data associated with a #sys_event_wakeup event. */
typedef struct
{
    /*! TRUE if the WAKE pin has logic 1 level at the time it was sampled after
     * waking up. Note that the WAKE pin is not debounced, therefore this value
     * may not be accurate if WAKE is connected to a mechanical switch. This is
     * equivalent to the value returned by SleepWakePinStatus().
     */
    bool wake_asserted;

    /*! TRUE if the WAKE pin was configured to WAKE on a low-to-high edge, FALSE
     * if waking on a high-to-low edge.
     */
    bool wake_on_high;

} wakeup_data;

/*! \brief The data associated with a #sys_event_battery_low event. */
typedef struct
{
    /*! TRUE if the voltage has dropped below the threshold, FALSE is it has
     * risen above the threshold.
     */
    bool    is_below_threshold;

    /*! The current voltage as read from the battery, in millivolts (mV) */
    uint16  current_voltage;

    /*! The threshold voltage (mV) against which the battery voltage is compared.
     * This value is taken from the Configuration Store.
     */
    uint16 threshold_voltage;
} battery_low_data;

/*! \brief The data associated with a #sys_event_pio_changed event. */
typedef struct
{
    /*! The state of the PIOs at the time the event was processed */
    uint32 pio_state;

    /*! The PIO event(s) that caused the event to be sent. One or more of these
     * bits may be set depending on how rapidly PIOs are changing. A bit is '1'
     * if the corresponding PIO changed state.
     */
    uint32 pio_cause;

} pio_changed_data;

/*! \brief The data associated with a #sys_event_pio_ctrlr event. */
typedef struct
{
    /*! Pointer to the PIO Controller data word */
    uint16* pio_ctrlr_data_word;

} pio_ctrlr_data;

#endif /* __SYS_EVENTS_H__ */
