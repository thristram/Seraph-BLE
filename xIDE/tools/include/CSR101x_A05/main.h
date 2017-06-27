/*! \file main.h
 *  \brief Functions relating to powering up the device.
 *
 * Copyright (c) 2010 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "types.h"
#include "bt_event_types.h"
#include "sys_events.h"
#include "sleep.h"

/*! \addtogroup APP
 * @{
 */
/*----------------------------------------------------------------------------*
 *  AppPowerOnReset
 */
/*! \brief Application function called just after a power-on reset etc.
 *
 *  This user application function is called just after a power-on reset
 *  (including after a firmware panic), after a wakeup from Hibernate or
 *  Dormant sleep states.
 *
 *  At the time this function is called, the last sleep state is not yet known.
 *
 *  NOTE: this function should only contain code to be executed after a
 *  power-on reset or panic.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void AppPowerOnReset(void);

/*----------------------------------------------------------------------------*
 *  AppInit
 */
/*! \brief Application function called after a power-on reset etc.
 *
 *  This user application function is called after a power-on reset
 *  (including after a firmware panic), after a wakeup from Hibernate or
 *  Dormant sleep states or after an HCI Reset has been requested.
 *
 *  The last sleep state is provided to the application in the parameter.
 *
 *  NOTE: In the case of a power-on reset, this function is called
 *  after AppPowerOnReset().
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void AppInit(sleep_state last_sleep_state);

/*----------------------------------------------------------------------------*
 *  AppProcessSystemEvent
 */
/*! \brief Application function called on system event.
 *
 *  This user application function is called whenever a system event, such
 *  as a battery low notification, is received by the system. If the 'data'
 *  parameter is not NULL it points to a structure containing information
 *  relevant to the wakeup event (see individual events in the #sys_event_id
 *  definition for more information).
 *
 *  \warning This 'data' pointer is only valid for the duration of the call to
 *  AppProcessSystemEvent(). After the application returns from this function
 *  the data structure will become invalid. Therefore if the application needs
 *  to save any information for later processing it must copy it to a local
 *  variable.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void AppProcessSystemEvent(sys_event_id id, void *data);

/*! @} */
#endif /* __MAIN_H__ */

