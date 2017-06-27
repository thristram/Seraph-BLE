/*! \file ls_app_if_event.h
 *  \brief  Link Supervisor interface to Applications
 *
 *  Defines the callback function to be defined by the application to receive LM Events.
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd 2010 - 2011
 */

#ifndef LS_APP_IF_EVENT_H_
#define LS_APP_IF_EVENT_H_


/*! \addtogroup APP
 * @{
 */

/*----------------------------------------------------------------------------*
 *  AppProcessLmEvent
 */
/*! \brief called whenever a LM-specific event is received by the system.
 *
 *
 *  \param event_code identifying which member of LM_EVENT_T union should
 *         be used to decode event_data
 *  \param event_data pointer to event datastructure
 *
 *  \returns Applications should invariably return TRUE.
 *  \note    A return of FALSE indicates that this event cannot be
 *           processed at this time and should stall the event queue.
 *           The Application will next be offered the same event when
 *           another event is added to the queue, which may be an
 *           indefinite time later. The event queue depth is relatively
 *           small so stalling the queue for an extended period may cause
 *           a fault condition to be raised, usually resulting in reboot.
 */
/*---------------------------------------------------------------------------*/
extern bool AppProcessLmEvent(lm_event_code event_code, LM_EVENT_T *event_data);

#if defined(ROM_BUILD) || defined(HAVE_ROM) 
extern bool (*AppProcessLmEventPtr)(lm_event_code event_code, LM_EVENT_T *event_data);

extern void installAppProcessLmEvent(bool (*ptr)(lm_event_code event_code, LM_EVENT_T *event_data));
#else /* defined(ROM_BUILD) || defined(HAVE_ROM)  */

/* If we aren't running with a ROM then call the function directly. */
#define AppProcessLmEventPtr(x, y) AppProcessLmEvent(x, y)
#endif


/*! @} */

/*! \addtogroup LSAPP
 * @{
 */

#endif /* LS_APP_IF_EVENT_H_ */
