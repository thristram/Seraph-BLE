/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
 /*! \file cm_observer.h
 *  \brief Header file for connection manager observer
 *
 */

#if defined (OBSERVER)

#ifndef __CM_OBSERVER_H__
#define __CM_OBSERVER_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_hal.h"

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*! \addtogroup CM_Observer
 * @{
 */

/*----------------------------------------------------------------------------*
 *  CMObserverInit
 *----------------------------------------------------------------------------*/
/*! \brief Initialises the observer module
 *
 * This function initialises the observer module
 * \returns Nothing
 *
 */
extern void CMObserverInit(void);

/*----------------------------------------------------------------------------*
 *  CMObserverSetScanState
 *----------------------------------------------------------------------------*/
/*! \brief Sets the Observer scanning state
 *
 * This function sets the Observer scanning state
 * \param[in] new_state New scan state as specified by \ref cm_scan_state.
 * \returns Nothing
 *
 */
extern void CMObserverSetScanState(cm_scan_state new_state);

/*----------------------------------------------------------------------------*
 *  CMCentralHandleProcessLMEvent
 *----------------------------------------------------------------------------*/
/*! \brief Handles the firmware events related to the observer role
 *
 * This function handles the firmware events related to the observer role
 * \param[in] msg Pointer to the incoming event message
 * \returns Nothing
 *
 */
extern void CMObserverHandleProcessLMEvent(h_msg_t *msg);

/*!@} */

#endif /* __CM_OBSERVER_H__ */

#endif /* OBSERVER */
