/*! \file aio.h
 *  \brief Analogue I/O configuration and control functions.
 *
 *   This file contains the functions to provide the application with
 *   access to the AIOs.
 *
 *   NOTE: the AIOs do not drive while the CSR1000 chip is in Deep
 *   Sleep.  Applications wishing to use AIOs as outputs should
 *   disable Deep Sleep by calling #SleepModeChange() with a parameter
 *   of #sleep_mode_shallow or #sleep_mode_never.
 *
 *   Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 */

#ifndef __AIO_H__
#define __AIO_H__

#include "types.h"

/*! \addtogroup AIO
 * @{
 */
/*============================================================================*
Public Definitions
*============================================================================*/

/*! AIO parameter number */
typedef enum
{
    AIO0           = 0,
    AIO1           = 1,
    AIO2           = 2,
    NUMBER_OF_AIOS = 3
} aio_select;


/*============================================================================*
Public Function Implementations
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  AioDrive
 */
/*! \brief  The AIO is configured as an analogue output and driven to
 *          the specified voltage, in mV.
 *
 *  \param  aio  The AIO to set.
 *
 *  \param  level  The analogue level to drive the AIO output at, in mV,
 *      from very near VDD voltage to 0.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void AioDrive(aio_select aio, uint16 level);

/*----------------------------------------------------------------------------*
 *  AioRead
 */
/*! \brief  Read the level of an AIO, in mV.
 *          Values read will range from 0 to VDD.
 *
 *  \param  aio  The AIO to read.
 *
 *  \returns  The voltage in mV.
 */
/*---------------------------------------------------------------------------*/
extern uint16 AioRead(aio_select aio);

/*----------------------------------------------------------------------------*
 *  AioOff
 */
/*! \brief  Turns the specified AIO "off" as an output, i.e. stops it
 *      driving out.
 *
 *  \param  aio  The AIO to set.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void AioOff(aio_select aio);

/*----------------------------------------------------------------------------*
 *  AioSetDig
 */
/*! \brief  Set the specified AIO digital output level.
 *      The specified AIO is configured as a digital output.
 *
 *  \param  aio  The AIO to set.
 *
 *  \param  state  TRUE to set the AIO high, FALSE to set it low.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void AioSetDig(aio_select aio, bool state);

/*! @} */
#endif /* __AIO_H__ */
