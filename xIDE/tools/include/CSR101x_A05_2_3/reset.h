/*! \file reset.h
 *  \brief Chip/firmware reset functionality
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd 2009-2011.
 */

#ifndef __RESET_H__
#define __RESET_H__

#include "types.h"

/*! \addtogroup RST
 * @{
 */


/*============================================================================*
Public Definitions
*============================================================================*/


/*============================================================================*
Public Function Implementations
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  WarmReset
 */
/*! \brief  Trigger a warm reset of the CSR1000, reloading code from the boot
 *  device and resetting the hardware.
 *
 *  \returns [Never returns].
 */
/*---------------------------------------------------------------------------*/
extern void WarmReset(void);


/*! @} */

#endif /* __RESET_H__ */

