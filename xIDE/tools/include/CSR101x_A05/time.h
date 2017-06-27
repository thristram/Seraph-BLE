/*! \file time.h
 *  \brief Application interface to System Time.
 *
 *  Access to 16-, 32- and 48-bit times and manipulations thereof.
 *
 * Copyright (c) 2009 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef _TIME_H_
#define _TIME_H_

#include "types.h"

/*! \addtogroup TIME
 * @{
 */

/*! \brief System time in microseconds, expressed as a 32-bit number.
 * Although this corresponds to 2^32 microseconds (~71.5 minutes) it
 * should be noted that this is not the maximum timer expiry value
 * that can be used. The maximum timer expiry value is (2^31)-1 microseconds
 * (approx. 35 minutes 47 seconds), this restriction enables correct handling of
 * time counter 'roll overs'.
 */
typedef	uint32	TIME;

/*! \brief System time in microseconds, expressed as a 48-bit number
 * (maximum duration ~8.9 years unsigned).
 */
typedef uint16 time48[3];

/*! \brief Time constant expressing one millisecond in system time units */
#define	MILLISECOND	((TIME)(1000))

/*! \brief Time constant expressing one second in system time units */
#define	SECOND		((TIME)(1000 * MILLISECOND))

/*! \brief Time constant expressing one minute in system time units */
#define	MINUTE		((TIME)(60 * SECOND))

/*! \brief Delay for one CPU cycle */
#define DELAY_ONE_CYCLE() __asm__("nop")
/*----------------------------------------------------------------------------*
 *  TimeGet32
 */
/*! \brief Read the current system time, 32 bits worth.
 *
 *  Returns the current value of the low 32 bits of the system's 48
 *  bit 1MHz clock. The resolution of the returned uint32 is one
 *  microsecond, so this value wraps after approximately 71 minutes.
 *
 *  This clock is the basis of all timed events in the chip's hardware,
 *  notably other functions declared in this file.
 *
 *  The function can be called from the machine's background or from
 *  interrupt routines.
 *
 *  \return The current system time.
 */
/*---------------------------------------------------------------------------*/
extern uint32 TimeGet32(void);

/*----------------------------------------------------------------------------*
 *  TimeGet16
 */
/*! \brief Read the current system time, 16 bits worth.
 *
 *  Returns the current value of the low 16 bits of the system's 48
 *  bit 1MHz clock. The resolution of the returned uint16 is one
 *  microsecond, so this value wraps after approximately 65ms.
 *
 *  NOTE: Due to the fast speed of the clock, using just the lowest 16 bits
 *  is typically only useful when making quick differential measurements, e.g
 *  to check execution time of task that is guaranteed to finish within 65ms.
 *
 *  The function can be called from the machine's background or from
 *  interrupt routines.
 *
 *  \return The current system time.
 *
 *  \note The time manipulation macros are not appropriate here.  But make
 *  sure you know what you are doing with wraps and int16/uint16 stuff.
 */
/*---------------------------------------------------------------------------*/
extern uint16 TimeGet16(void);

/*----------------------------------------------------------------------------*
 *  TimeAdd
 */
/*! \brief Add two 16- or 32-bit time values.
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return "t1 + t2".
 *
 *  \note Adding the numbers can overflow the range of a TIME, so the user must
 *  be cautious.
 */
/*---------------------------------------------------------------------------*/
#define	TimeAdd(t1, t2)	((t1) + (t2))

/*----------------------------------------------------------------------------*
 *  TimeSub
 */
/*! \brief Subtract two 16- or 32-bit time values
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return "t1 - t2".
 *
 *  \note Subtracting the numbers can provoke an underflow. This returns
 *  a signed number for correct use in comparisons.
 */
/*---------------------------------------------------------------------------*/
#define	TimeSub(t1, t2)	((int32) (t1) - (int32) (t2))

/*----------------------------------------------------------------------------*
 *  TimeCmpEQ
 */
/*! \brief Determine if two 16- or 32-bit time values are equal
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return TRUE if "t1" equals "t2", else FALSE.
 */
/*---------------------------------------------------------------------------*/
#define	TimeCmpEQ(t1, t2)		((t1) == (t2))

/*----------------------------------------------------------------------------*
 *  TimeCmpGT
 */
/*! \brief Determine if one 16- or 32-bit time value is greater than another
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return TRUE if "t1" is greater than "t2", else FALSE.
 *
 * 	\note Because time values wrap, "t1" and "t2" must differ by less than
 * 	\f$ 2^{31} \f$ microseconds, as the comparison uses signed arithmetic.
 */
/*---------------------------------------------------------------------------*/
#define TimeCmpGT(t1, t2) (TimeSub((t1), (t2)) > 0)

/*----------------------------------------------------------------------------*
 *  TimeCmpGE
 */
/*! \brief Determine if one 16- or 32-bit time value is greater than or
 * equal to another
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return TRUE if "t1" is greater than or equal to "t2", else FALSE.
 *
 * 	\note see TimeCmpGT().
 */
/*---------------------------------------------------------------------------*/
#define TimeCmpGE(t1, t2) (TimeSub((t1), (t2)) >= 0)

/*----------------------------------------------------------------------------*
 *  TimeCmpLT
 */
/*! \brief Determine if one 16- or 32-bit time value is less than another
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return TRUE if "t1" is less than "t2", else FALSE.
 *
 *  \note see TimeCmpGT().
 */
/*---------------------------------------------------------------------------*/
#define TimeCmpLT(t1, t2) (TimeSub((t1), (t2)) < 0)

/*----------------------------------------------------------------------------*
 *  TimeCmpLE
 */
/*! \brief Determine if one 16- or 32-bit time value is less than or
 * equal to another
 *
 *  \param t1 1st time value.
 *  \param t2 2nd time value.
 *
 *  \return TRUE if "t1" is less than or equal to "t2", else FALSE.
 *
 *  \note see TimeCmpGT().
 */
/*---------------------------------------------------------------------------*/
#define TimeCmpLE(t1, t2) (TimeSub((t1), (t2)) <= 0)

/*----------------------------------------------------------------------------*
 *  TimeGet48
 */
/*! \brief Read the current full 48-bit system time
 *
 *  Returns all 48 bits of the current value of the system's 1MHz
 *  clock.  The resolution of the returned 32 bit value is one
 *  microsecond, with the most significant word returned through the
 *  pointer passed as a parameter.  The clock therefore wraps after
 *  approximately 8.9 years.  That should be enough for most purposes.
 *
 *  Except as noted above, this function can be treated as per TimeGet32().
 *
 *  \param time_msw A pointer to store the most significant word of the current
 *  system time
 *
 *  \return Lower 32 bits to the current system time.
 */
/*---------------------------------------------------------------------------*/
extern uint32 TimeGet48(uint16 *time_msw);

/*----------------------------------------------------------------------------*
 *  TimeGet48WithOffset
 */
/*! \brief Read the current 48-bit time plus a 16-bit offset
 *
 *  As per TimeGet48(), but treats the time as an array of three
 *  uint16s rather than a uint16 MSW and uint32 LSW, and adds a uint16
 *  offset to the current time.
 *
 *  \param time48 A 3 word array to store the return value.
 *  \param offset Value to added to the current system time.
 *
 *  \return Nothing
 *
 *  \note Places the offset system time in the three words following the
 *  pointer parameter, which is *not* checked for validity.
 */
/*---------------------------------------------------------------------------*/
extern void TimeGet48WithOffset(uint16 *time48, uint16 offset);

/*----------------------------------------------------------------------------*
 *  TimeCopy48
 */
/*! \brief Copy one 48-bit time value to another
 *
 *  Copies the time in ts to td.
 *
 *  \param td A 3 word array containing the destination variable.
 *  \param ts A 3 word array containing the source variable.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
#define TimeCopy48(td, ts) \
    ((td)[0] = (ts)[0], (td)[1] = (ts)[1], (td)[2] = (ts)[2])

/*----------------------------------------------------------------------------*
 *  TimeIncrement48
 */
/*! \brief Add a 16-bit offset to a 48-bit time in-place.
 *
 *  The result is written back into the original parameter.
 *
 *  \param t A 3 word array containing the source/destination variable.
 *  \param increment The amount "t" is to increased.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void TimeIncrement48(time48 t, uint16 increment);

/*----------------------------------------------------------------------------*
 *  TimeSub48
 */
/*! \brief Subtract two 48-bit time values.
 *
 *  Sets t_result to t1-t2 (using 48-bit unsigned arithmetic).
 *
 *  \param t_result A 3 word array containing the result
 *  \param t1 A 3 word array containing the 1st time value.
 *  \param t2 A 3 word array containing the 2nd time value.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void TimeSub48(time48 t_result, time48 t1, time48 t2);

/*----------------------------------------------------------------------------*
 *  TimeCmp48LT
 */
/*! \brief Compare two 48-bit time values.
 *
 *  Check if the first time is behind the second time, using the usual
 *  "half the wrap period" definitions of 'behind' and 'ahead'.
 *
 *  \param t1 A 3 word array containing the 1st time value.
 *  \param t2 A 3 word array containing the 2nd time value.
 *
 *  \return TRUE if the first time is behind the second, otherwise FALSE.
 */
/*---------------------------------------------------------------------------*/
extern bool TimeCmp48LT(time48 t1, time48 t2);

/*----------------------------------------------------------------------------*
 *  TimeDelayUSec
 */
/*! \brief Delay for a given number of microseconds.
 *
 *  Delay for at least delay microseconds, and less than delay + 1
 *  microseconds (this arises because the code allows for the timer
 *  ticking just before it first measures it, so errs on the side of a
 *  longer delay rather than an unexpectedly short one).
 *
 *  \param delay The delay value in microseconds.
 *
 *  \return Nothing
 *
 *  \warning "delay" should be strictly less than 65535 microseconds.
 *  For safety, especially where the possibility exists of running
 *  at a slower clock speed, a few microseconds of margin are advised.
 */
/*---------------------------------------------------------------------------*/
extern void TimeDelayUSec(uint16 delay);

/*----------------------------------------------------------------------------*
 *  TimeWaitWithTimeout32
 */
/*! \brief Busy-wait for a condition on a timeout.
 *
 *  Busy wait for a condition to become true, with a timeout.
 *  This is a common pattern which people tend to get wrong: in a
 *  naive implementation, if we spend time processing an interrupt
 *  between checking the condition and checking the timeout, we
 *  can falsely decide that the condition is not met in time.
 *  This implementation checks the condition again after exiting
 *  the timing loop (so works on the assumption that the condition
 *  is stable once it has become TRUE).
 *
 *  \param cond The expression you're waiting for.
 *  \param delay The timeout in microseconds.
 *  \param result Set to TRUE if the condition was met in time, FALSE otherwise.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
#define TimeWaitWithTimeout32(cond, delay, result)              \
    do {                                                        \
        TIME wwt_end_time = TimeAdd(TimeGet32(), delay);        \
        while (!(cond) && TimeCmpLT(TimeGet32(), wwt_end_time)) \
            ;                                                   \
        result = (cond);                                        \
    } while (0)

/*----------------------------------------------------------------------------*
 *  TimeWaitWithTimeout16
 */
/*! \brief Busy-wait for a condition with a timeout.
 *
 *  Busy wait for a condition to become true, with a timeout.
 *  This is a common pattern which people tend to get wrong: in a
 *  naive implementation, if we spend time processing an interrupt
 *  between checking the condition and checking the timeout, we
 *  can falsely decide that the condition is not met in time.
 *  This implementation checks the condition again after exiting
 *  the timing loop (so works on the assumption that the condition
 *  is stable once it has become TRUE).
 *
 *  \param cond The expression you're waiting for.
 *  \param delay The timeout in microseconds.
 *  \param result Set to TRUE if the condition was met in time, FALSE otherwise.
 *
 *  \return Nothing
 *
 *  \note The maximum time you can wait for is strictly less than 65535
 *  microseconds.
 */
/*---------------------------------------------------------------------------*/
#define TimeWaitWithTimeout16(cond, delay, result)              \
    do {                                                        \
        uint16 wwt_start_time = TimeGet16();                    \
        while (!(cond) && ((TimeGet16() - wwt_start_time)       \
                <= (uint16)(delay)))                            \
            ;                                                   \
        result = (cond);                                        \
    } while (0)

/*----------------------------------------------------------------------------*
 *  TimeWaitWithAbsoluteTimeout32
 */
/*! \brief Busy-wait for a condition on a timeout.
 *
 *  Busy wait for a condition to become true, with a timeout.
 *  This is a common pattern which people tend to get wrong: in a
 *  naive implementation, if we spend time processing an interrupt
 *  between checking the condition and checking the timeout, we
 *  can falsely decide that the condition is not met in time.
 *  This implementation checks the condition again after exiting
 *  the timing loop (so works on the assumption that the condition
 *  is stable once it has become TRUE).
 *
 *  \param cond The expression you're waiting for.
 *  \param endtime The time at which to timeout.
 *  \param result Set to TRUE if the condition was met in time, FALSE otherwise.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
#define TimeWaitWithAbsoluteTimeout32(cond, endtime, result)    \
    do {                                                        \
        while (!(cond) && TimeCmpLT(TimeGet32(), endtime))      \
            ;                                                   \
        result = (cond);                                        \
    } while (0)


/*----------------------------------------------------------------------------*
 *  TimeWaitWithAbsoluteTimeout16
 */
/*! \brief Busy-wait for a condition with a timeout.
 *  Busy wait for a condition to become true, with a timeout.
 *  This is a common pattern which people tend to get wrong: in a
 *  naive implementation, if we spend time processing an interrupt
 *  between checking the condition and checking the timeout, we
 *  can falsely decide that the condition is not met in time.
 *  This implementation checks the condition again after exiting
 *  the timing loop (so works on the assumption that the condition
 *  is stable once it has become TRUE).
 *
 *  \param cond The expression you're waiting for.
 *  \param endtime The time at which to timeout.
 *  \param result Set to TRUE if the condition was met in time, FALSE otherwise.
 *
 *  \return Nothing
 *
 *  \note The maximum time you can wait for is strictly less than 32767
 *  microseconds. This is used by I2C and the max delay is 1ms.
 */
/*---------------------------------------------------------------------------*/
#define TimeWaitWithAbsoluteTimeout16(cond, endtime, result)    \
    do {                                                        \
        while (!(cond) && ((int16)(TimeGet16() - endtime) < 0)) \
            ;                                                   \
        result = (cond);                                        \
    } while (0)

/*! @} */
#endif /* _TIME_H_ */
