/*! \file fault.h
 *  \brief Application services for firmware fault. 
 *
 * Copyright (c) 2010 - 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __FAULT_H__
#define __FAULT_H__

/*! \addtogroup PNC
 *
 *  \brief Incorrect function from firmware should normally trigger a fault.
 *  Based on CSKey settings this can trigger a watchdog.
 *
 * @{
 */
/*----------------------------------------------------------------------------*
 *      PanicReadFwFault
 */
/*! \brief  Allows the user application to retrieve last FW fault ID. 
 *
 * FW fault ID is stored in persistent memoory. This API does not take care of 
 * persistent memory validity. This could return FAULT_NONE in case FW 
 * determines that whole persistent memory needs to be reset at boot up.
 * See fault.xml for more information on fault ID.
 *
 * \param  fid pointer to location where fault ID will be placed.
 *
 * \returns  Status of read operation.
 */
/*---------------------------------------------------------------------------*/
extern sys_status PanicReadFwFault(uint16 *fid);

/*----------------------------------------------------------------------------*
 *      PanicClearFwFault
 */
/*! \brief  Allows the user application to clear last FW fault ID.
 *
 * This does not affect the fault statistics maintained by firmware.
 *
 * \returns  Status of clear operation.
 */
/*---------------------------------------------------------------------------*/
extern sys_status PanicClearFwFault(void);

/*! @} */
#endif /* __FAULT_H__ */
