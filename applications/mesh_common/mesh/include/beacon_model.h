/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Beacon_model.h
 *
 *  \brief Defines CSRmesh Beacon Model specific data structures\n
 */
/******************************************************************************/

#ifndef __BEACON_MODEL_H__
#define __BEACON_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Beacon_Model 
 *  \brief Beacon Model API
 *
 * This model allows managing beacons of various types (CSR retail beacons, Apple iBeacons, Google URI or Eddystone beacons,LTE direct beacons and so on). Beacons will normally be operating as beacons only, but will periodically turn into mesh devices for a duration of time.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/
#define MAX_BEACON_PAYLOAD_LENGTH                 31

/*! \brief CSRmesh Beacon Model message types */

/*! \brief May be sent to a beacon to set its status. More than one such command can be sent to set the intervals for different beacon types, which need not be the same. This message also allows for the wakeup intervals to be set if the beacon elects to only be on the mesh sporadically. The time is always with respect to the beacon’s internal clock and has no absolute meaning. This message will be answered through BEACON_STATUS. */
typedef struct
{
    CsrUint8 beacontype; /*!< \brief 0x00=CSR, 0x01=iBeacon, 0x02=Eddystone URL, 0x03=Eddystone UID, 0x04=LTE direct, 0x05=Lumicast. */
    CsrUint8 beaconinterval; /*!< \brief Time between beacon transmissions in 100ths of seconds (0..100). Setting this to 0 stops that beacon type transmitting. */
    CsrUint16 meshinterval; /*!< \brief How many minutes between mesh wakeups (1..N). */
    CsrUint8 meshtime; /*!< \brief How long node stays as a mesh node (10s of seconds). */
    CsrUint8 txpower; /*!< \brief -127..+127 dbM. */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACON_SET_STATUS_T;

/*! \brief Message to be sent to a beacon in order to recover its current status. This message fetch information pertinent to a particular type. The transaction ID will help matching the pertinent BEACON_STATUS message – a different transaction ID shall be used for a different type (although the BEACON_STATUS message has the type pertinent to the status and thus could be used in conjunction to the Transaction ID for disambiguation). */
typedef struct
{
    CsrUint8 beacontype; /*!< \brief 0x00=CSR, 0x01=iBeacon, 0x02=Eddystone URL, 0x03=Eddystone UID, 0x04=LTE direct, 0x05=Lumicast. */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACON_GET_BEACON_STATUS_T;

/*! \brief This message issue issued by a Beacon as response to BEACON_SET_STATUS or BEACON_GET_STATUS. Furthermore, a Beacon appearing on the mesh sporadically, will issue such message (with destination ID set to 0) as soon as it re-joins the mesh. In this case, one message per active beacon type should be issued. This message will provide the Payload ID currently in used for the associated type. */
typedef struct
{
    CsrUint8 beacontype; /*!< \brief 0x00=CSR, 0x01=iBeacon, 0x02=Eddystone URL, 0x03=Eddystone UID, 0x04=LTE direct, 0x05=Lumicast. */
    CsrUint8 beaconinterval; /*!< \brief Time between beacon transmissions in 100ths of seconds (0..100). Setting this to 0 stops that beacon type transmitting. */
    CsrUint16 meshinterval; /*!< \brief How many minutes between mesh wakeups (1..N). */
    CsrUint8 meshtime; /*!< \brief How long node stays as a mesh node (10s of seconds). */
    CsrUint8 txpower; /*!< \brief -127..+127 dbM. */
    CsrUint8 batterylevel; /*!< \brief 0..100 (percent). */
    CsrUint16 payloadid; /*!< \brief ID of latest received payload for this beacon type, as returned by beacon payload ack. */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACON_BEACON_STATUS_T;

/*! \brief Message allowing a node to fetch all supported types and associated information by a given Beacon. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACON_GET_TYPES_T;

/*! \brief Provides information on the set of beacon supported by the node. The battery level is reported as well as time since last message. */
typedef struct
{
    CsrUint16 beacontype; /*!< \brief 0x01=CSR, 0x02=iBeacon, 0x04=Eddystone URL, 0x08=Eddystone UID, 0x10=LTE direct, 0x20=Lumicast. */
    CsrUint8 batterylevel; /*!< \brief 0..100 (percent) */
    CsrUint16 timesincelastmessage; /*!< \brief Time in 10s of seconds since last message received. 0 means message received less than 10 seconds ago, 1 means 10..19 seconds and so on. The special value 0xFFFF means no messages have been received from this beacon since the proxy was last started. If this message is received direct from a beacon rather than from a proxy, this field will always be 0. */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACON_TYPES_T;

/*! \brief One or more of these packets may be sent to a beacon to set its payload. The content is either the raw advert data, or extra information (such as crypto cycle) which a beacon may require. The payload data is sent as a length and an offset, so the whole payload need not be sent if it has not changed. A beacon can support many beacon types - it can be sent as many different payloads as needed, one for each type. The first byte of the first payload packet contains the length and offset of the payload and the payload ID; this allows a beacon which already has the payload to send an immediate acknowledgement, saving traffic. This ID will be sent back in the 'Payload ACK' message if the beacon has received the whole payload. The payload may have to be split in several parts, in which case only the last part shall be acknowledged. Upon receiving a BEACON_SET_PAYLOAD, the beacon will update its corresponding beacon data – if data was previously available, it will be replaced by the provided payload, thus a beacon can only hold one payload per beacon type. */
typedef struct
{
    CsrUint8 packetframing; /*!< \brief Bits 0..3 defines packet number 1..16, Bits 4..7 gives total number of packets 1..16. */
    CsrUint16 payloadid; /*!< \brief ID of this payload (so beacon can know whether it needs to update). */
    CsrUint8 beacontype; /*!< \brief 0x00=CSR, 0x01=iBeacon, 0x02=Eddystone URL, 0x03=Eddystone UID, 0x04=LTE direct, 0x05=Lumicast. */
    CsrUint8 payloadlength; /*!< \brief Total length of payload data being sent. */
    CsrUint8 payloadoffset; /*!< \brief Offset into Payload. If this number is 0..0xBF then it is an offset into the raw advert data. If this number is 0xC0.0xFF then it is an offset into non-advertising data (e.g. crypto information). */
    CsrUint8 payload[MAX_BEACON_PAYLOAD_LENGTH]; /*!< \brief Payload data - up to 4 bytes total (segmented) */
    CsrUint8 payload_len; /*!< \brief Length of the payload field */
} CSRMESH_BEACON_SET_PAYLOAD_T;

/*! \brief Only one Acknowledgement message will occur for multiple BEACON_SET_PAYLOAD messages sharing the same transaction ID. Only when the last segment is received that such acknowledgement will be issued. Where missing payload messages exist, the list of their indices will be provided in the Ack field. */
typedef struct
{
    CsrUint8 payloadtype; /*!< \brief 0x00=CSR, 0x01=iBeacon, 0x02=Eddystone URL, 0x03=Eddystone UID, 0x04=LTE direct, 0x05=Lumicast. */
    CsrUint16 ackornack; /*!< \brief 0 means success, set bits indicate missing packet numbers 1..16. */
    CsrUint16 payloadid; /*!< \brief Payload ID which was received, or 0xFFFF if no final packet was received. */
} CSRMESH_BEACON_PAYLOAD_ACK_T;

/*! \brief Message allowing a node to retrieve the current payload held on a given beacon. This message shall be answered by one or more BEACON_SET_PAYLOAD messages. */
typedef struct
{
    CsrUint8 payloadtype; /*!< \brief 0x00=CSR, 0x01=iBeacon, 0x02=Eddystone URL, 0x03=Eddystone UID, 0x4=LTE direct, 0x5=Lumicast. */
} CSRMESH_BEACON_GET_PAYLOAD_T;


/*!@} */
#endif /* __BEACON_MODEL_H__ */

