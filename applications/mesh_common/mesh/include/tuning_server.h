/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file tuning_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh Tuning model
 */
/******************************************************************************/

#ifndef __TUNING_SERVER_H__
#define __TUNING_SERVER_H__

#include "csr_mesh.h"
#include "csr_mesh_model_common.h"
#include "types.h"

/*! \addtogroup Tuning_Server
 * @{
 */

/*! \brief Tuning statistics of neighbouring devices */
typedef struct {
    uint16 id;                    /*!< \brief ID of audible station. 0 means the entry is not  */
    uint16 rate;                  /*!< \brief  time-weighted rate of hearing recently */
    int16  rssi;                  /*!< \brief  average RSSI, weighted with time. */ 
    uint16 count;                 /*!< \brief  count up how many heard this epoch */
    uint16 herProbePeriod;        /*!< \brief  Periodic sending period of the station */
    int16  herTxPower;            /*!< \brief  Tx power of the station */
    uint32 seq;                  
} tuningStats_t;

/*============================================================================*
Public Definitions
*============================================================================*/

#define MAX_CSR_TUNING_MODEL_TIMERS         (1)

/*============================================================================*
Public Function Definitions
*============================================================================*/

/*----------------------------------------------------------------------------*
 * TuningModelInit
 */
/*! \brief Model Initialisation
 *  
 *   Registers the model handler with the CSRmesh. Sets the CSRmesh to report
 *   num_groups as the maximum number of groups supported for the model
 *
 *  \param nw_id Identifier of the network to which the model has to be
                 registered.
 *  \param group_id_list Pointer to a uint16 array to hold assigned group_ids. 
 *                       This must be NULL if no groups are supported
 *  \param num_groups Size of the group_id_list. This must be 0 if no groups
 *                    are supported.
 *  \param app_callback Pointer to the application callback function. This
 *                    function will be called to notify all model specific messages
 *
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TuningModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);

/*----------------------------------------------------------------------------*
 * TuningModelStart
 */
/*! \brief Starts sending TUNING_PROBE messages and adjusts the scan duty cycle 
 *         based on the analysis of the received messages.
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
extern void TuningModelStart( uint16 probePeriod, uint16 reportPeriod );

/*----------------------------------------------------------------------------* 
 * TuningModelStop
 */
/*! \brief Stops auto-tuning of rx duty cycle. 
 *
 *   This function stops the auto-tuning of the scan duty cycle.
 *   The scan duty cycle will not be restored to the original value set by 
 *   the application. The application has to explicitly set the required duty cycle
 *   by calling the \ref CsrMeshSetAdvScanParam when the tuning model is stopped.
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
extern void TuningModelStop( void );

/*----------------------------------------------------------------------------* 
 * TuningReadStats
 */
/*! \brief Returns the tuning statistics of the neighbouring devices
 *
 *   Returns the tuning statistics of the neighbouring devices
 *  
 *  \param stats Pointer to receive the tuning statistics buffer pointer.
 *
 *  \returns Number of devices for which the statistics are collected.
 */
/*----------------------------------------------------------------------------*/
extern uint16 TuningReadStats( tuningStats_t **stats );

/*!@} */
#endif /* __TUNING_SERVER_H__ */

