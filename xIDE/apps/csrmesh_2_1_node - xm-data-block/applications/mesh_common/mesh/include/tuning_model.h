/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Tuning_model.h
 *
 *  \brief Defines CSRmesh Tuning Model specific data structures\n
 */
/******************************************************************************/

#ifndef __TUNING_MODEL_H__
#define __TUNING_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Tuning_Model 
 *  \brief Tuning Model API
 *
 * <br>The Tuning Model is designed to allow for the local management of duty cycles through the determination of the local network density. The purpose of the Tuning model is to allow a device to detect that there are other similar devices nearby, and construct a metric for mesh density in the neighbourhood. The mesh density has to be a proxy for the local ability of the mesh to relay messages. The only information available is from the messages measured from the device itself. <br><br> A reasonable approach it to measure how many messages are received, and adjust the listening duty cycle up or down to make that metric match a goal either fixed for this deployment or commanded by a higher-level monitoring program (SET_GOAL).<br><br> Such a metric must not depend on normal product-oriented mesh traffic, because it is sporadic, unpredictable and time-dependent. In order to know what is measured, dedicated messages must be used with the following properties:<ul style='list-style:square;list-style-position: inside;'><li style='list-style-type:square;'>Messages are not forwarded through the mesh, thus measuring only immediately neighbours.</li><li style='list-style-type:square;'>Messages do not occur in real traffic, thus making the metric independent of other time varying activities.<li style='list-style-type:square;'>Messages arrive at a fixed rate, so measuring the same information before and after an adjustment.</li><li style='list-style-type:square;'>Messages are transmitted from all stations in the same manner, providing uniformity throughout the mesh.</li><li style='list-style-type:square;'>Messages identify distinct sources, so that we can count neighbours individually.</li></ul>  The TUNING_PROBE message is used to facilitate the metric computation, when sent with TTL = 0.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Tuning Model message types */

/*! \brief Tuning Probe: The Tuning Probe message is sent to discover neighbours. This messages is issued by devices wanting to determine their density metrics.n The message is sent in two forms. A short form omitting both ScanDutyCycle and BatteryState with a TTL=0. This allows immediate neighbours to perform various calculations and in turn provide their own PROBE messages. The long version is only provided with TTL>0. This cannot be used for immediate neighbour density determination, but can be used to determine the overall network density. The ability to identify if a node is a potential pinch-point in the Mesh network can be achieved through the comparison of immediate and average number of neighbours within the network. The usage of the PROBE message with TTL=0 or TTL>0 is a way to perform these computations.  It is worth noting that the periodicity of these two types of messages are different; messages with TTL>0 is much more infrequent than messages with TTL=0. Furthermore, it is wise not to use messages for TTL>0 and embedded values in the determination of the average values. The AverageNumNeighbour field is fixed point 6.2 format encoded. The ScanDutyCycle is expressing percentage for numbers from 1 to 100 and (x-100)/10 percentage for numbers from 101 to 255. */
typedef struct
{
    CsrUint16 transmissionrateandtxpower; /*!< \brief Transmission rate (seconds, low 10 bits), Tx power, dbM (high 6 bits) */
    CsrUint8 numaudibleneighbours; /*!< \brief Number of audible neighbours */
    CsrUint8 averagenumneighbours; /*!< \brief Average number of neighbours seen, 6.2 fixed point format */
    CsrUint8 scandutycycle; /*!< \brief Duty cycle 1..100% or 101..255 (x-100) per mille */
    CsrUint8 batterystate; /*!< \brief Battery state 0..100% */
} CSRMESH_TUNING_PROBE_T;

/*! \brief Getting Tuning Stats: These messages are aimed at collecting statistics from specific nodes. This message allows for the request of all information or for some of its parts. Responses are multi-parts, each identified with an index (combining a continuation flag - top bit). MissingReplyParts for the required field serves at determining the specific responses one would like to collect. If instead all the information is requested, setting this field to zero will inform the destination device to send all messages. Importantly, response (STATS_RESPONSE) messages will not necessarily come back in order, or all reach the requestor. It is essential to handle these cases in the treatment of the collected responses. */
typedef struct
{
    CsrUint32 missingreplyparts; /*!< \brief Bitset of missing reply parts (0 means send all) */
} CSRMESH_TUNING_GET_STATS_T;

/*! \brief Current Asset State: Response to the request. The PartNumber indicates the current index of the message, the top bit of this field is used to indicate that more messages are available after this part. For example, a response made up of three messages will have part numbers 0x80, 0x81 and 0x02. Each message has a maximum of  two neighbours. The combination of these responses and PROBE (TTL>0) are a means to establish an overall perspective of the entire Mesh Network. */
typedef struct
{
    CsrUint8 partnumber; /*!< \brief Part number (0..31). Bit 7 means more to come */
    CsrUint16 neighbourid1; /*!< \brief ID of first neighbour in this message */
    CsrUint8 neighbourrate1; /*!< \brief Average rate neighbour 1 is seen, 6.2 fixed point format, 0..63.75 */
    CsrInt8 neighbourrssi1; /*!< \brief Average RSSI of neighbour 1 */
    CsrUint16 neighbourid2; /*!< \brief ID of second neighbour in this message */
    CsrUint8 neighbourrate2; /*!< \brief Average rate neighbour 2 is seen, 6.2 fixed point format, 0..63.75 */
    CsrInt8 neighbourrssi2; /*!< \brief Average RSSI of neighbour 2 */
} CSRMESH_TUNING_STATS_T;

/*! \brief Current tuning config for a device: This message comes as a response to a SET_CONFIG. Encoding its various fields follow the same convention as the ones exposed in SET_CONFIG. */
typedef struct
{
    CsrUint8 numeratorhightrafficneighratio; /*!< \brief Default is 14. See tuning doc for explanation */
    CsrUint8 numeratornormaltrafficneighratio; /*!< \brief Default is 12. See tuning doc for explanation */
    CsrUint8 denominatortrafficneighratio; /*!< \brief Default is 10. See tuning doc for explanation */
    CsrUint8 normalgoalvalue; /*!< \brief Default is 60. See tuning doc for explanation */
    CsrUint8 highgoalvalue; /*!< \brief Default is 75. See tuning doc for explanation */
    CsrUint8 currentscandutycycle; /*!< \brief Duty cycle 1..100% or 101..255 (x-100) per mille */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TUNING_ACK_CONFIG_T;

/*! \brief Setting (or reading) tuning config: Omitted or zero fields mean do not change. This message enforces the state of the recipient. The state is  defined by two goals, normal and high traffic, and their associated number of neighbour to decide which cases to follow. Goals are expressed with unit-less values ranging from 0 to 255. These goals relate to metrics calculated on the basis of density computed at the node and across the network. The expectation is for these goals to be maintained through modification of the receive duty cycle. The average of number of neighbours for high and normal traffic is expressed as a ratio, both numbers sharing the same denominator and each representing their respective numerators. The duty cycle encoding follows the same rules as per duty cycle encoding encountered in PROBE message. This message comes in two formats. A fully truncated form containing only the OpCode (thus of length 2) is used to indicate a request for information. This message should be answered by the appropriate ACK_CONFIG. Further interpretations of this message are: 1. Missing ACK field implies that a request for ACK_CONFIG is made. Thus, this is a special case of the fully truncated mode. However, the provided fields are meant to be used in the setting of goals. 2. Individual fields with zero value are meant NOT to be changed in the received element. Same as for missing fields in truncated messages. Furthermore, in order to improve testing, a combination of values for main and high goals are conventionally expected to be used for defining two behaviours: 1. Suspended: Tuning Probe messages (TTL=0) should be sent and statistics maintained, but the duty cycle should not be changed - thus goals will never be achieved. The encoding are: Main Goal = 0x00 and High Goal = 0xFE. 2. Disable: No Tuning Probe message should be sent and statistics should not be gathered - averaged values should decay. The encoding are: Main Goal = 0x00 and High Goal = 0xFF. */
typedef struct
{
    CsrUint8 numeratorhightrafficneighratio; /*!< \brief Default is 14. See tuning doc for explanation */
    CsrUint8 numeratornormaltrafficneighratio; /*!< \brief Default is 12. See tuning doc for explanation */
    CsrUint8 denominatortrafficneighratio; /*!< \brief Default is 10. See tuning doc for explanation */
    CsrUint8 normalgoalvalue; /*!< \brief Default is 60. Set to 0 to disable/suspend tuning */
    CsrUint8 highgoalvalue; /*!< \brief Default is 75. Set to 254 to suspend, 255 to disable tuning */
    CsrUint8 currentscandutycycle; /*!< \brief Duty cycle 1..100% or 101..255 (x-100) per mille */
    CsrBool ackrequest; /*!< \brief Whether ack required */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TUNING_SET_CONFIG_T;


/*!@} */
#endif /* __TUNING_MODEL_H__ */

