/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Diagnostic_model.h
 *
 *  \brief Defines CSRmesh Diagnostic Model specific data structures\n
 */
/******************************************************************************/

#ifndef __DIAGNOSTIC_MODEL_H__
#define __DIAGNOSTIC_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Diagnostic_Model 
 *  \brief Diagnostic Model API
 *
 * This model permits the collection of local diagnostics for the purpose of accumulation of this information for further global data analysis. The diagnostic model is only about reporting information, how this information is gathered is beyond its scope. This model is limited to handling one single Network Key.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Diagnostic Model message types */

/*! \brief When received this message is interpreted as to reconfigure the set of information collected. Statistics gathering can be turned on/off ? in the off mode no measurement of messages count and RSSI measurements will be made. RSSI binning can be stored, such that collection ALL messages? RSSI (MASP/MCP, irrespective of encoding) are split between a given number of bin, each of equal dimensions. Masking of individual broadcast channel can be specified, resulting in the collection of information specifically on the selected channels. A REST bit is also available. When present all the accumulated data will be cleared and all counters restarted. Note that it is possible to change various configurations without the RESET flag, this will result in the continuation of accumulation and therefore incoherent statistics. */
typedef struct
{
    CsrUint16 flag; /*!< \brief  */
    CsrUint8 mode; /*!< \brief  */
    CsrInt8 rssi; /*!< \brief  */
    CsrInt8 rssi_bin; /*!< \brief  */
} CSRMESH_DIAGNOSTIC_STATE_T;

/*! \brief  */
typedef struct
{
    CsrUint16 flag; /*!< \brief  */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_DIAGNOSTIC_GET_STATS_T;

/*! \brief  */
typedef struct
{
    CsrUint8 type; /*!< \brief  */
    CsrUint8 data[7/size_in_bytes(CsrUint8)]; /*!< \brief  */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_DIAGNOSTIC_STATS_T;


/*!@} */
#endif /* __DIAGNOSTIC_MODEL_H__ */

