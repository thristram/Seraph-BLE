/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file conn_param_update.h
 *  \brief Header definitions for connection parameter update procedure
 *
 */

#ifndef __CONN_PARAM_UPDATE_H__
#define __CONN_PARAM_UPDATE_H__

/*! \addtogroup Peripheral
 * @{
 */

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_types.h"
#include "cm_api.h"

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/*! \brief Connection parameters */
typedef struct
{
 /*! \brief Variable to store the current connection interval being used. */
    uint16                          conn_interval;

 /*! \brief Variable to store the current slave latency. */
    uint16                          conn_latency;

 /*! \briefVariable to store the current connection timeout value. */
    uint16                          conn_timeout;

}CONN_PARAMS_T;

/*!
 * \brief Function to call when a connection parameter update request completes
 *
 * \param[in] params_applied TRUE if host accepted new params, FALSE otherwise
 * \return None
 */
typedef void (*conn_param_update_complete)(bool params_applied);

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------
 *  RequestConnParamsUpdate
 *----------------------------------------------------------------------------*/
/*! \brief Starts the Connection parameter update procedure
 *
 * This function will wait for the appropriate time to request a connection
 * parameter update and then send it. It will retry sending the first set
 * of parameters a number of times before falling back to the second set.
 * It may be called at any time during a connection, but may take some time
 * to complete.
 * \param[in] device_id Device handle
 * \param[in] new_params_1 Preferred connection parameters
 * \param[in] new_params_2 Apple compliant connection parameters
 * \returns Nothing
 *
 */
extern void RequestConnParamsUpdate(device_handle_id device_id,
                                    ble_con_params *new_params_1,
                                    ble_con_params *new_params_2);

/*----------------------------------------------------------------------------
 *  RequestConnParamsUpdateOnce
 *----------------------------------------------------------------------------*/
/*! \brief Requests a single update for each connection parameter set given
 *
 * This function requests a connection parameter update only once for each
 * set of connection parameters given. It will send the request as soon as
 * possible, so should not be called until well after a connection has
 * been established. It can be used when a change in connection parameters
 * is required midway through a connection, for example, to temporarily
 * reduce the connection interval before some data transfer. It will still
 * wait for any GATT activity to finish before sending the request, but
 * aims to complete as quickly as possible (hence only attempting each set
 * once).
 * \param[in] device_id Device handle
 * \param[in] new_params_1 Preferred connection parameters
 * \param[in] new_params_2 Apple compliant connection parameters
 * \returns Nothing
 *
 */
extern void RequestConnParamsUpdateOnce(device_handle_id device_id,
                                        ble_con_params *new_params_1,
                                        ble_con_params *new_params_2);

/*----------------------------------------------------------------------------
 *  StopConnParamsUpdate
 *----------------------------------------------------------------------------*/
/*! \brief Stops the connection parameter update procedure
 *
 * This function stops the connection parameter update procedure
 * \returns Nothing
 *
 */
extern void StopConnParamsUpdate(void);

/*----------------------------------------------------------------------------
 *  GetUpdatedConnParams
 *----------------------------------------------------------------------------*/
/*! \brief Gets the updated connection parameters
 *
 * This function gets the updated connection parameters
 * \param[out] conn_params Updated connection parameters
 * \returns TRUE if the parameter got updated, FALSE otherwise
 *
 */
extern bool GetUpdatedConnParams(CONN_PARAMS_T *conn_params);

/*----------------------------------------------------------------------------
 *  ConnectionParamUpdateEvent
 *----------------------------------------------------------------------------*/
/*! \brief Handles the events related to connection parameter update procedure
 *
 * This function handles the events related to connection
 * parameter update procedure
 * \param[in] event_type  Connection parameter update event type
 * \param[in] cm_event_data Pointer to connection parameter update event data
 * \returns Nothing
 *
 */
extern void ConnectionParamUpdateEvent(cm_event event_type,
                                       CM_EVENT_T *cm_event_data);

/*----------------------------------------------------------------------------
 *  ConnectionParamUpdateSetCallback
 *----------------------------------------------------------------------------*/
/*! \brief Set a function to call when a connection parameter update completes
 *
 * This function allows an application callback function to be set, which will
 * be called when a connection parameter update request completes. This could
 * either be because the host accepted the new parameters, or because the
 * maximum number of rejections was reached (and so the new parameters have not
 * been applied)
 * \param[in] update_complete Function to call
 * \returns Nothing
 *
 */
extern void ConnectionParamUpdateSetCallback(conn_param_update_complete function);

/*!@} */
#endif /* __CONN_PARAM_UPDATE_H__ */
