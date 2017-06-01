 /******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *****************************************************************************/
/*! \file csr_sched.h
 *  \brief CSRmesh library LE bearer scheduling configuration and control functions
 *
 *   This file contains the functions to provide the application to
 *   configuration and control of LE Bearer.
 *
 */
 /******************************************************************************/

#ifndef __CSR_SCHED_H__
#define __CSR_SCHED_H__

#include "csr_sched_types.h"

/*! \addtogroup CSRmesh
 * @{
 */

#if (CSR_MESH_ON_CHIP == 1) 
/*-----------------------------------------------------------------------------*
 * "CSRmesh Bearer-Scheduling Wrapper APIs
 *-----------------------------------------------------------------------------*/

/* Workaround for memory corruption issue in FH */
#if defined(CSR102x) || defined(CSR102x_A05)
extern void CSRSchedDisableRadioEvent(void);

extern void CSRSchedEnableRadioEvent(void);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRSchedSetScanDutyCycle
 */
 /*! \brief  Scan duty cycle can be configured using this API.
 *
 *  Select a pair scan_window, scan_interval, which are a reasonably
 *  small multiple of 625us, matching the percentage requested.
 *
 *  \param scan_duty_cycle Range 1-1000. 1 corresponds to 0.1% and 1000 corresponds to 100%
 *  \param new_scan_slot Range 4 to 16384.
 * 
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRSchedResult CSRSchedSetScanDutyCycle(CsrUint16 scan_duty_cycle, CsrUint16 new_scan_slot);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSRSchedGetScanDutyCycle
 */
/*! \brief  Current scan duty cycle can be fetched using this API.
 *
 *  Get scan duty cycle from scan window and scan interval.
 *
 * \returns CsrUint16.
 */
/*----------------------------------------------------------------------------*/
extern CsrUint16 CSRSchedGetScanDutyCycle(void);

/*----------------------------------------------------------------------------*
 * CSRSchedSetConfigParams
 */
/*! \brief  Configure Mesh and Generic LE parameters
 *
 *  Applicaton uses this API to Configure MESH and Generic LE parameters
 *
 *  \param le_params  This structure contains and Mesh or Non-mesh 
 *  scheduling parameters.
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedSetConfigParams(CSR_SCHED_LE_PARAMS_T *le_params);

/*----------------------------------------------------------------------------*
 * CSRSchedSendUserAdv
 */
/*! \brief Transmit Non-Connectable/Connectable Application data. 
 *
 * Application Uses this API to transmit user
 * advertising data(CONNECTABLE/NON-CONNECTABLE)
 *
 *  \param le_adv_data  contains advertising paramters advertising data and 
 *                      scan response data.
 *
 *  \param callBack  callback function registered by the application.
 *  Scheduler uses this callback to notify the application regarding the 
 *  successful transmission of user adv packet.
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedSendUserAdv(CSR_SCHED_ADV_DATA_T *le_adv_data, 
                                   CSR_SCHED_USER_ADV_NOTIFY_CB_T callBack);

/*----------------------------------------------------------------------------*
 * CSRSchedGetConfigParams
 */
/*! \brief Get scheduler configuration parameters
 *
 *  Application uses this API to Get the current configuration .
 *
 *  \param le_params  This structure contains Generic LE and Mesh LE
 *  parameters.
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedGetConfigParams (CSR_SCHED_LE_PARAMS_T *le_params);

/*----------------------------------------------------------------------------*
 * CSRSchedRegisterPriorityMsgCb
 */
/*! \brief Register callback function to handle radio busy error for 
 *   priority message. The callback will be called when the radio is available
 *   for transmission.
 *
 *  \param cb_ptr  Function pointer for callback function
 *
 *  \returns none
 */
/*---------------------------------------------------------------------------*/
void CSRSchedRegisterPriorityMsgCb(CSR_SCHED_PRIORITY_MSG_CB_T cb_ptr);

/*----------------------------------------------------------------------------*
 * CSRSchedSendPriorityMsg
 */
/*! \brief This function sends a CSRmesh message with priority.
 *
 *   This function sends the message with priority. The message bypasses the 
 *   transmit queue and will be advertised immediately.
 *   In case the radio busy in a connection event error code CSR_SCHED_RESULT_RADIO_BUSY 
 *   is returned.
 *
 *  \param mesh_packet_type 
 *  \param payload CSRmesh message. The message will be sent as a mesh advert.
 *
 *  \param length Message length
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedSendPriorityMsg(CsrUint8 mesh_packet_type, 
                                           CsrUint8 *payload, CsrUint8 length);

/*----------------------------------------------------------------------------*
 * CSRSchedHandleIncomingData
 */
/*! \brief Forward Mesh messages to the scheduler
 *
 *  This API should get called during the reception of incoming mesh message
 *
 *  \param data_event Specifies the bearer type over which the mesh data is received.
 *
 *  \param data  Mesh data received.
 *
 *  \param length  Length of the mesh data.
 *
 *  \param rssi  Received signal strength.
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedHandleIncomingData(CSR_SCHED_INCOMING_DATA_EVENT_T data_event, 
                                         CsrUint8* data, 
                                         CsrUint8 length, 
                                         CsrInt8 rssi);

/*----------------------------------------------------------------------------*
 * CSRSchedEnableListening
 */
/*! \brief Starts or stops LE scan operation
 *
 *  Application uses this api to start/stop LE scan operation.
 *
 *  \param enable TRUE / FALSE
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedEnableListening(CsrBool enable);

/*----------------------------------------------------------------------------*
 * IsCSRSchedRunning
 */
/*! \brief Indicates about the scheduler state
 *
 *  Application uses this api to check whether scheduler is ready for sending
 *  advtertisements.
 *
 *  \returns CsrBool.
 */
/*---------------------------------------------------------------------------*/
CsrBool IsCSRSchedRunning(void);

/*----------------------------------------------------------------------------*
 * CSRSchedStart
 */
/*! \brief Start scheduling of LE scan and advertisement
 *
 *  This API starts scheduling of LE scan and advertisement 
 *
 *  \returns CSRSchedResult. Refer to \ref CSRSchedResult.
 */
/*---------------------------------------------------------------------------*/
CSRSchedResult CSRSchedStart(void);

/*----------------------------------------------------------------------------*
 * CSRSchedNotifyGattEvent
 */
/*! \brief Initialize CSRmesh core mesh stack
 *
 *  Application uses this API to notify the scheduler regarding 
 *  the GATT connection state event
 *
 *  \param gatt_event_type  GATT event type 
 *
 *  \param gatt_event_data  GATT event data
 *
 *  \param call_back  callback function to notify mesh data to client
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void CSRSchedNotifyGattEvent(CSR_SCHED_GATT_EVENT_T gatt_event_type,
                             CSR_SCHED_GATT_EVENT_DATA_T *gatt_event_data,
                             CSR_SCHED_NOTIFY_GATT_CB_T call_back);

/*----------------------------------------------------------------------------*
 * CsrSchedSetTxPower
 */
/*! \brief Sets the Transmit Power for the Device.
 *
 *  Sets the Transmit power for the device. This internally uses
 *  LsSetTransmitPowerLevel API without any extra mappings.
 *
 *  \param level The level of the transmit power to be used. It is not a level 
 *               in dBm, it's an index value into the transmit power table.<br>
 *               Refer to LsSetTransmitPowerLevel in
 *               CSR &mu;Energy&reg; Firmware Library Documentation. An example
 *               mapping function from dBm to level is shown below for
 *               CSR101x/CSR102x.
 *
 *  \returns Nothing
 *
 *  \code
 * #if defined(CSR101x) || defined(CSR101x_A05)
 *
 * // Tx Power level mapping.
 * // 0 :-18 dBm 1 :-14 dBm 2 :-10 dBm 3 :-06 dBm
 * // 4 :-02 dBm 5 :+02 dBm 6 :+06 dBm 7 :+08 dBm
 *
 * #define CSR_MESH_MIN_TX_POWER_LEVEL   (-18)
 * #define CSR_MESH_MAX_TX_POWER_LEVEL   (8)
 * #define CSR_MESH_TX_POWER_LEVEL_STEP  (4)
 *
 * #else
 *
 * // Tx Power level mapping.
 * // #define TX_PA_POWER_DBM_0       (-60)
 * // #define TX_PA_POWER_DBM_1       (-30)
 * // #define TX_PA_POWER_DBM_2       (-22)
 * // #define TX_PA_POWER_DBM_3       (-16)
 * // #define TX_PA_POWER_DBM_4       (-13)
 * // #define TX_PA_POWER_DBM_5       (-10)
 * // #define TX_PA_POWER_DBM_6       (-6)
 * // #define TX_PA_POWER_DBM_7       (-4)
 * // #define TX_PA_POWER_DBM_8       (-3)
 * // #define TX_PA_POWER_DBM_9       (-2)
 * // #define TX_PA_POWER_DBM_10      (-1)
 * // #define TX_PA_POWER_DBM_11      (0)
 * // #define TX_PA_POWER_DBM_12      (1)
 * // #define TX_PA_POWER_DBM_13      (2)
 * // #define TX_PA_POWER_DBM_14      (3)
 * // #define TX_PA_POWER_DBM_15      (4)
 *
 * static const CsrInt8 fh_tx_pwr_map[] = { -60, -30, -22, -16, -13, -10, -6, -4, -3, -2,  -1, 0, 1, 2, 3, 4 };
 *
 * #define CSR_MESH_MIN_TX_POWER_LEVEL   (-60)
 * #define CSR_MESH_MAX_TX_POWER_LEVEL   (4)
 * #endif
 *
 * //----------------------------------------------------------------------------
 * //  CsrSchedConvertTxPower
 * //
 * //  DESCRIPTION
 * //      Sets the Transmit Power to the nearest value possible.
 * //
 * //  PARAMETERS
 * //      power Transmit power level in dBm.
 * //
 * //  RETURNS/MODIFIES
 * //      Returns level as per the platform.
 * //----------------------------------------------------------------------------
 * extern CsrUint8 CsrSchedConvertTxPower(CsrInt8 power)
 * {
 *     CsrUint8 level = 0;
 *
 *     if (power & 0x80)
 *     {
 *           power |= 0xFF00;
 *     }
 *
 *     // Map Power level in dBm to index
 *     if (power <= CSR_MESH_MIN_TX_POWER_LEVEL)
 *     {
 *           level = LS_MIN_TRANSMIT_POWER_LEVEL;
 *     }
 *     else if (power >= CSR_MESH_MAX_TX_POWER_LEVEL)
 *     {
 *           level = LS_MAX_TRANSMIT_POWER_LEVEL;
 *     }
 *     else
 *     {
 * #if defined(CSR102x) || defined(CSR102x_A05)
 *            for (level = 0; level < (sizeof(fh_tx_pwr_map) - 1); level++)
 *            {
 *                 if ((power >= fh_tx_pwr_map[level]) && (power < fh_tx_pwr_map[level + 1]))
 *                 {
 *                       break;
 *                 }
 *            }
 * #else
 *            level = (power - CSR_MESH_MIN_TX_POWER_LEVEL) / CSR_MESH_TX_POWER_LEVEL_STEP;
 * #endif
 *     }
 *
 *     return level;
 * }
 *
 *  \endcode
 *
 */
/*---------------------------------------------------------------------------*/
extern void CsrSchedSetTxPower(CsrUint8 level);

#endif /* (CSR_MESH_ON_CHIP == 1)  */

/*!@} */

#endif /* __CSR_SCHED_H__*/

