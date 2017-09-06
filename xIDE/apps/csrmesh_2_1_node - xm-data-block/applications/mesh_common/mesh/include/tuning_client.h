/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Tuning_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Tuning model 
 */
/******************************************************************************/

#ifndef __TUNING_CLIENT_H__
#define __TUNING_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Tuning_Client
 * @{
 */
#include "tuning_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * TuningModelClientInit
 */
/*! \brief 
 *      Initialises Tuning Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TuningModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * TuningProbe
 */
/*! \brief Tuning Probe: The Tuning Probe message is sent to discover neighbours. This messages is issued by devices wanting to determine their density metrics.n The message is sent in two forms. A short form omitting both ScanDutyCycle and BatteryState with a TTL=0. This allows immediate neighbours to perform various calculations and in turn provide their own PROBE messages. The long version is only provided with TTL>0. This cannot be used for immediate neighbour density determination, but can be used to determine the overall network density. The ability to identify if a node is a potential pinch-point in the Mesh network can be achieved through the comparison of immediate and average number of neighbours within the network. The usage of the PROBE message with TTL=0 or TTL>0 is a way to perform these computations.  It is worth noting that the periodicity of these two types of messages are different; messages with TTL>0 is much more infrequent than messages with TTL=0. Furthermore, it is wise not to use messages for TTL>0 and embedded values in the determination of the average values. The AverageNumNeighbour field is fixed point 6.2 format encoded. The ScanDutyCycle is expressing percentage for numbers from 1 to 100 and (x-100)/10 percentage for numbers from 101 to 255.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TUNING_PROBE_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TuningProbe(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TUNING_PROBE_T *p_params );

/*----------------------------------------------------------------------------*
 * TuningGetStats
 */
/*! \brief Getting Tuning Stats: These messages are aimed at collecting statistics from specific nodes. This message allows for the request of all information or for some of its parts. Responses are multi-parts, each identified with an index (combining a continuation flag - top bit). MissingReplyParts for the required field serves at determining the specific responses one would like to collect. If instead all the information is requested, setting this field to zero will inform the destination device to send all messages. Importantly, response (STATS_RESPONSE) messages will not necessarily come back in order, or all reach the requestor. It is essential to handle these cases in the treatment of the collected responses.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_TUNING_STATS 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TUNING_GET_STATS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TuningGetStats(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TUNING_GET_STATS_T *p_params );

/*----------------------------------------------------------------------------*
 * TuningSetConfig
 */
/*! \brief Setting (or reading) tuning config: Omitted or zero fields mean do not change. This message enforces the state of the recipient. The state is  defined by two goals, normal and high traffic, and their associated number of neighbour to decide which cases to follow. Goals are expressed with unit-less values ranging from 0 to 255. These goals relate to metrics calculated on the basis of density computed at the node and across the network. The expectation is for these goals to be maintained through modification of the receive duty cycle. The average of number of neighbours for high and normal traffic is expressed as a ratio, both numbers sharing the same denominator and each representing their respective numerators. The duty cycle encoding follows the same rules as per duty cycle encoding encountered in PROBE message. This message comes in two formats. A fully truncated form containing only the OpCode (thus of length 2) is used to indicate a request for information. This message should be answered by the appropriate ACK_CONFIG. Further interpretations of this message are: 1. Missing ACK field implies that a request for ACK_CONFIG is made. Thus, this is a special case of the fully truncated mode. However, the provided fields are meant to be used in the setting of goals. 2. Individual fields with zero value are meant NOT to be changed in the received element. Same as for missing fields in truncated messages. Furthermore, in order to improve testing, a combination of values for main and high goals are conventionally expected to be used for defining two behaviours: 1. Suspended: Tuning Probe messages (TTL=0) should be sent and statistics maintained, but the duty cycle should not be changed - thus goals will never be achieved. The encoding are: Main Goal = 0x00 and High Goal = 0xFE. 2. Disable: No Tuning Probe message should be sent and statistics should not be gathered - averaged values should decay. The encoding are: Main Goal = 0x00 and High Goal = 0xFF.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * When a response is received the application  callback function is called 
 * to notify the \ref CSRMESH_TUNING_ACK_CONFIG 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_TUNING_SET_CONFIG_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult TuningSetConfig(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_TUNING_SET_CONFIG_T *p_params );

/*!@} */
#endif /* __TUNING_CLIENT_H__ */

