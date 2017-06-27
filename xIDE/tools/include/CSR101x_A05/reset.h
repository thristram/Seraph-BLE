/*! \file reset.h
 *  \brief Chip/firmware reset functionality
 *
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
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

