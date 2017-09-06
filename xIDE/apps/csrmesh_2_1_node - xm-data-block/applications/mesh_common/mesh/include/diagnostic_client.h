/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Diagnostic_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Diagnostic model 
 */
/******************************************************************************/

#ifndef __DIAGNOSTIC_CLIENT_H__
#define __DIAGNOSTIC_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Diagnostic_Client
 * @{
 */
#include "diagnostic_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * DiagnosticModelClientInit
 */
/*! \brief 
 *      Initialises Diagnostic Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult DiagnosticModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * DiagnosticState
 */
/*! \brief When received this message is interpreted as to reconfigure the set of information collected. Statistics gathering can be turned on/off ? in the off mode no measurement of messages count and RSSI measurements will be made. RSSI binning can be stored, such that collection ALL messages? RSSI (MASP/MCP, irrespective of encoding) are split between a given number of bin, each of equal dimensions. Masking of individual broadcast channel can be specified, resulting in the collection of information specifically on the selected channels. A REST bit is also available. When present all the accumulated data will be cleared and all counters restarted. Note that it is possible to change various configurations without the RESET flag, this will result in the continuation of accumulation and therefore incoherent statistics.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_DIAGNOSTIC_STATE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult DiagnosticState(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_DIAGNOSTIC_STATE_T *p_params );

/*----------------------------------------------------------------------------*
 * DiagnosticGetStats
 */
/*! \brief 
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_DIAGNOSTIC_STATS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_DIAGNOSTIC_GET_STATS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult DiagnosticGetStats(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_DIAGNOSTIC_GET_STATS_T *p_params );

/*!@} */
#endif /* __DIAGNOSTIC_CLIENT_H__ */

