/*! \file timer.h
 *  \brief The chip's timers
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd 2009-2011.
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include "types.h"
#include "time.h"

/*! \addtogroup TIMER
 * @{
 */

/*! \brief Invalid Timer ID. E.g. this value is returned if a timer cannot be
 * created
 */
#define TIMER_INVALID           0

/*! \brief Size of structure for a single timer. See #TimerInit for details on
 * how to use this value.
 *
*/
#define SIZEOF_APP_TIMER        6

/*! \brief Opaque type used to reference a timer
 *
*/
typedef uint16 timer_id;

/*! \brief Timer expiry call back function type
 *
*/
typedef void( *timer_callback_arg )( timer_id const );

/*----------------------------------------------------------------------------*
 *  TimerInit
 */
/*! \brief Initialise the application timers.
 *
 * This function is used by the application to set up an array of timers. The
 * Firmware library uses this array to manage timers on behalf of the
 * application.
 *
 * \warning It is the responsibility of the application to ensure that the size
 * of the array pointed to by timer_array is big enough to hold all the timer
 * data required by the firmware. This size is #SIZEOF_APP_TIMER * max_timers.
 *
 * A typical use of this function in an application would be as follows:
 *
 * \verbatim

#define MAX_APP_TIMERS       2
     ...
static uint16 app_timers[ SIZEOF_APP_TIMER * MAX_APP_TIMERS ];
     ...
void AppInit(...)
{
    // Initialise application timers as soon as possible
    TimerInit(MAX_APP_TIMERS, (void*)app_timers);
    ...
    // The init function can start a timer now if it desires
    TimerCreate( ... );

\endverbatim
 *
 * The application is not allowed to have more timers running concurrently
 * than were originally provided. If no timers are left then TimerCreate will
 * return TIMER_INVALID.
 *
 * TimerInit() can only be called once after power-up. It is not possible to
 * re-initialise the application timers without performing a device reset.
 *
 * \warning The application MUST not modify the contents of the array at any
 * time after calling TimerInit().
 *
 *  \param max_timers Maximum number of application timers available.
 *  \param timer_array Fixed-length array to hold timer information.
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void TimerInit(uint16 max_timers, void* timer_array);


/*----------------------------------------------------------------------------*
 *  TimerCreate
 */
/*! \brief Insert an application timer into the timer queue.
 *
 * The timeout period is measured in microseconds. time.h defines a number of
 * constants for #MILLISECOND, #SECOND and #MINUTE, e.g. allowing 10*SECOND to
 * be used when starting a timer. Note that although the timeout value is a
 * 32-bit number the maximum timeout period is actually (2^31)-1 microseconds
 * (not (2^32)-1 microseconds) to enable safe 'roll over' handling.
 * (2^31)-1 microseconds corresponds to approximately 35 minutes 47 seconds.
 *
 * When the timer expires, the firmware will call the application timer handler
 * function. Prior to calling the handler, the firmware will have "cleaned up"
 * the timer structure, therefore the application does not need to manually
 * delete the timer. If the application needs to restart the timer (for the
 * same or a different duration) it can simply call TimerCreate() again.
 *
 *  \param time The number of microseconds the timer should run for.
 *  \param relative True => time is offset from "now".
 *  \param handler Pointer to the expiry callback function.
 *
 *  \return Timer reference, or TIMER_INVALID if the timer could not be started.
 */
/*---------------------------------------------------------------------------*/
extern timer_id TimerCreate(uint32 const time,
                            bool const relative,
                            timer_callback_arg handler);


/*----------------------------------------------------------------------------*
 *  TimerDelete
 */
/*! \brief Delete a timer from the queue.
 *
 *  \param tid Timer reference.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void TimerDelete(timer_id const tid);


/*----------------------------------------------------------------------------*
 *  AppBackgroundTick
 */
/*! \brief Enable or disable the Application Background Tick event.
 *
 *  The Background Tick event is generated via the firmware internal watchdog
 *  timer (although at a lower priority). Therefore one event will be sent to
 *  the application for each watchdog tick. The tick rate is defined by the
 *  Configuration Store "Watchdog period" key.
 *
 *  One advantage of using the Background Tick rather than running a separate
 *  application timer is that the number of times the chip has to wake from
 *  Deep Sleep can be minimised (as the firmware watchdog processing and
 *  application Background Tick processing will occur during the same wakeup
 *  period). If the application does not require to perform "background"
 *  processing as often as the watchdog period, it could use a simple counter
 *  to skip some Background Tick events.
 *
 *  The Background Tick is presented to the application as a regular event
 *  (SYS_BACKGROUND_TICK_IND) to the AppProcessLmEvent() event handler.
 *
 *  By default when the system powers up the Background Tick is disabled. The
 *  application can enable or disable the tick whenever it needs it.
 *
 *  \param enable TRUE to enable the tick event; FALSE to disable the tick event
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void AppBackgroundTick(bool enable);


/*! @} */
#endif /* __TIMER_H__ */
