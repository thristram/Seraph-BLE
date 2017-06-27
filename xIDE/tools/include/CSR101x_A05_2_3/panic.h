/*! \file panic.h
 *  \brief Support for applications to panic due to unrecoverable
 *  errors.
 *
 *  This file contains the support for applications wishing to issue a
 *  panic because of some irretrievable error.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 */

#ifndef __PANIC_H__
#define __PANIC_H__

#include "types.h"
#include "fault.h"

/*! \addtogroup PNC
 * @{
 */
/*============================================================================*
Public Definitions
*============================================================================*/
/*! \brief Value indicating that application has not panicked. Firmware 
 * internally treats \ref PANIC_NONE as reset value of application panic code.
 * Applications are recommended to reserve 0 as no panic.
 */
#define PANIC_NONE          (0)

/*============================================================================*
Public Data
*============================================================================*/

/*============================================================================*
Public Function Implementations
*============================================================================*/

/*----------------------------------------------------------------------------*
 *  Panic
 */
/*! \brief  Raise a system panic, normally resetting the chip.
 *
 *  In normal operation, this function will not return since it resets
 *  the chip.  If the CS Key \c err_panic ("Cause faults to panic") is
 *  changed from its default setting to prevent \c
 *  FAULT_APPLICATION_PANIC from panicking the chip, this function
 *  will return.  This is not normally advisable.
 *
 *  \param  panic_code  An application-defined value.  This value is
 *      preserved across resets to assist debugging. Applications are strongly
 *      advised to refrain from using 0 as valid panic code, even though it is
 *      currently treated as valid code by firmware. See \ref PANIC_NONE.
 *
 *  \returns  Nothing.  Does not return under normal circumstances.
 */
/*---------------------------------------------------------------------------*/
extern void Panic(uint16 panic_code);

/*----------------------------------------------------------------------------*
 *  PanicReadAppPanic
 */
/*! \brief  Retrieve last application panic code.
 *
 *  This does not take care of validity of persistent store, so it could return
 *  \ref PANIC_NONE in case firmware determined that it needs to reset whole 
 *  persistent memory at boot up.
 *
 *  \param  pcode  pointer to location where panic code will be placed.
 *
 *  \returns  Status of read operation.
 */
/*---------------------------------------------------------------------------*/
extern sys_status PanicReadAppPanic(uint16 *pcode);

/*----------------------------------------------------------------------------*
 *  PanicClearAppPanic
 */
/*! \brief Clear application panic code; setting it to \ref PANIC_NONE.
 *
 *  \returns  Status of clear operation.
 */
/*---------------------------------------------------------------------------*/
extern sys_status PanicClearAppPanic(void);

/*! @} */
#endif /* __PANIC_H__ */
