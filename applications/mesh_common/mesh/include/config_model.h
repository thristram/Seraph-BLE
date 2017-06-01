/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Config_model.h
 *
 *  \brief Defines CSRmesh Config Model specific data structures\n
 */
/******************************************************************************/

#ifndef __CONFIG_MODEL_H__
#define __CONFIG_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Config_Model 
 *  \brief Config Model API
 *
 * <br>The Configuration Model configures the device within MCP. It sets the ability to discover devices within the CSRmesh network, configure the Device ID for a given device, and discover device information such as the set of models a device supports or a device UUID. It also sets the communication parameters for this device including the receive and transmit duty cycles and time-to-live values for transmitted packets. <h6>Note:</h6>Many devices implement the configuration model because it provides the primary method of inspecting a device and configuring the device. It has been considered that this model should be made mandatory, however at this time it is not a mandatory model as some devices may not need the configuration model; a device that only transmits asset broadcast messages using the asset model may not need the configuration model to work.<h4>Configuration State</h4>The configuration model has the following states:<ul style='list-style:square;list-style-position: inside;'><li style='list-style-type:square;'>DeviceUUID</li> <li style='list-style-type:square;'>SequenceNumber</li> <li style='list-style-type:square;'>DeviceID</li><li style='list-style-type:square;'> ModelsSupported</li><li style='list-style-type:square;'> TransmitInterval</li><li style='list-style-type:square;'>TransmitDuration<li style='list-style-type:square;'>ReceiverDutyCycle</li><li style='list-style-type:square;'>TransmitPower </li><li style='list-style-type:square;'>DefaultTimeToLive </li><li style='list-style-type:square;'>VendorIdentifier<li style='list-style-type:square;'>ProductIdentifier</li><li style='list-style-type:square;'>VersionNumber </li><li style='list-style-type:square;'>Appearance  </li><li style='list-style-type:square;'>LastETag </li><li style='list-style-type:square;'>Conformance Signature </li><li style='list-style-type:square;'>Stack version </li></ul><h5> DeviceUUID</h5>DeviceUUID is the 128-bit UUID, the unique identifier for the device. This value is fixed and cannot be changed. DeviceUUID is used 
erate DeviceHash in some configuration messages.<h5>SequenceNumber</h5>SequenceNumber is a 24-bit unsigned integer.<h5>DeviceID</h5>DeviceID is a 16-bit value. This value is used as the source address of the device when sending MCP messages and as the destination address when other devices send packets to this device. <h5> ModelsSupported </h5>ModelsSupported is a 128-bit bit field that has a single bit allocated for each model. If a bit is set, then the server behaviour of the set model should be supported, otherwise the server behaviour of the model is not supported.<h6>Note:</h6>It is not possible to determine if a device supports the client behaviour of a model, as this would be typically be determined by the functionality that the device exposes to the user.<h5>TransmitInterval</h5>The TransmitInterval is a 16-bit unsigned integer in milliseconds. This value determines how often a device retransmits a CSRmesh message.<h5>TransmitDuration</h5>TransmitDuration is a 16-bit unsigned integer in milliseconds. This value determines how long a device retransmits a CSRmesh message.<h5> ReceiverDutyCycle</h5>The ReceiverDutyCycle is an 8-bit unsigned integer that is 1/255th of a second. This value determines how frequently a device listens for CSRmesh messages from other devices.<h6>Note:</h6>The larger this value, the more reliably this device will receive CSRmesh messages, but the device will consume more energy. If this is value is zero, then the device will not listen for messages.<h5> TransmitPower</h5>TransmitPower is an 8-bit signed integer in dBm. This value determines how much energy is used when transmitting CSRmesh messages.<h6>Note:</h6>The larger this value, the more energy this device will consume but the greater the ranges in which this device will be able to transmit messages.<h5>DefaultTimeToLive</h5>DefaultTimeToLive is an 8-bit unsigned integer. <h5>VendorIdentifier</h5>VendorIdentifier is a 16-bit unsigned integer. This shall use the same enumerations as the existing Bluetooth SIG Company I
ication assigned numbers.<h5>ProductIdentifier</h5>The ProductIdentifier is a 16-bit unsigned integer allocated by the Vendor.<h5>VersionNumber</h5>VersionNumber is a 32-bit unsigned integer allocated per product by the vendor. Each new version of the device shall have a different VersionNumber. A new version is defined as any change to the software, hardware or firmware of a device, even if this change has no material impact to the behaviour of the device.<h5>Appearance</h5>Appearance is a 24-bit unsigned integer using the same the Bluetooth SIG GAP Appearance characteristic values.<h5>LastETag</h5>LastETag is a 64-bit unsigned integer that should be set to a different value each time the device has a change in its configuration. This value is a read only value, and can be used to quickly determine if a device has changed its configuration without another device reading the full configuration. A configuration change is defined as any change to the device such that the device behaviour changes - for example, changing Groups or the interval for sending Sensor broadcast data.<h4>Config Background Behaviour</h4>If a device does not have a DeviceID, that is, the DeviceID is 0x0000, then it may periodically transmit a CONFIG_DEVICE_IDENTIFIER message to the destination address of 0x0000 with its DeviceHash.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Config Model message types */

/*! \brief Upon receiving a CONFIG_LAST_SEQUENCE_NUMBER message from a trusted device, the local device updates the SequenceNumber to at least one higher than the LastSequenceNumber in the message. Note: A trusted device means a device that is not only on the same CSRmesh network, having the same network key, but also interacted with in the past. This message is most useful to check if a device has been reset, for example when the batteries of the device are changed, but it does not remember its last sequence number in non-volatile memory. */
typedef struct
{
    CsrUint24 lastsequencenumber; /*!< \brief The last sequence number seen by the source device from the destination device. */
} CSRMESH_CONFIG_LAST_SEQUENCE_NUMBER_T;

/*! \brief Upon receiving a CONFIG_RESET_DEVICE message from a trusted device directed at only this device, the local device sets the DeviceID to zero, and forgets all network keys, associated NetworkIVs and other configuration information. The device may act as if it is not associated and use MASP to re-associate with a network. Note: If the CONFIG_RESET_DEVICE message is received on any other destination address than the DeviceID of the local device, it is ignored. This is typically used when selling a device, to remove the device from the network of the seller so that the purchaser can associate the device with their network. */
typedef struct
{
    CsrUint8 signature[8/size_in_bytes(CsrUint8)]; /*!< \brief 8 lowest bytes of HMAC(DHM_KEY,DeviceHash). */
} CSRMESH_CONFIG_RESET_DEVICE_T;

/*! \brief When the device with a DeviceID of 0x0000 receives a CONFIG_SET_DEVICE_IDENTIFIER message and the DeviceHash of the message matches the DeviceHash of this device, the DeviceID of this device is set to the DeviceID field of this message. Then the device responds with the DEVICE_CONFIG_IDENTIFIER message using the new DeviceID as the source address. Note: This function is not necessary in normal operation of a CSRmesh network as DeviceID is distributed as part of the MASP protocol in the MASP_ID_DISTRIBUTION message. */
typedef struct
{
    CsrUint8 devicehash[8/size_in_bytes(CsrUint8)]; /*!< \brief The DeviceHash for the device that will have a new DeviceID. */
    CsrUint16 deviceid; /*!< \brief The new DeviceID for the device identified by DeviceHash. */
} CSRMESH_CONFIG_SET_DEVICE_IDENTIFIER_T;

/*! \brief Upon receiving a CONFIG_SET_PARAMETERS message, where the destination address is the DeviceID of this device, the device saves the TxInterval, TxDuration, RxDutyCycle, TxPower and TTL fields into the TransmitInterval, TransmitDuration, ReceiverDutyCycle, TransmitPower and DefaultTimeToLive state respectively. Then the device responds with a CONFIG_PARAMETERS message with the current configuration model state information. */
typedef struct
{
    CsrUint16 txinterval; /*!< \brief The transmit interval for this device. The TxInterval field is a 16-bit unsigned integer in milliseconds that determines the interval between transmitting messages. */
    CsrUint16 txduration; /*!< \brief How long a single transmission should occur for. The TxDuration, or transmit duration field is a 16-bit unsigned integer in milliseconds that determines the duration of transmission for a single message. */
    CsrUint8 rxdutycycle; /*!< \brief How much time this device listens for messages. The RxDutyCycle, or receiver duty cycle field is an 8-bit unsigned integer in 1/255ths of a second that determines how often the device listens for CSRmesh messages. Note: The value 0x00 implies that the device does not listen for messages. The value 0xFF implies that the device continuously listens for messages. */
    CsrInt8 txpower; /*!< \brief How loud the device transmits messages. The TxPower, or transmit power field is an 8-bit signed integer in decibels that determines the radio transmit power for a device. */
    CsrUint8 timetolive; /*!< \brief The initial default time to live for messages. The TimeToLive field is an 8-bit unsigned integer that determines the default value for the TTL packet field in an MTL packet. */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_SET_PARAMETERS_T;

/*! \brief Upon receiving a CONFIG_GET_PARAMETERS message, where the destination address is the DeviceID of this device, the device will respond with a CONFIG_PARAMETERS message with the current config model state information. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_GET_PARAMETERS_T;

/*! \brief Configuration parameters */
typedef struct
{
    CsrUint16 txinterval; /*!< \brief The transmit interval for this device. The TxInterval field is a 16-bit unsigned integer in milliseconds that determines the interval between transmitting messages. */
    CsrUint16 txduration; /*!< \brief How long a single transmission should occur for. The TxDuration, or transmit duration field is a 16-bit unsigned integer in milliseconds that determines the duration of transmission for a single message. */
    CsrUint8 rxdutycycle; /*!< \brief How much time this device listens for messages. The RxDutyCycle, or receiver duty cycle field is an 8-bit unsigned integer in 1/255ths of a second that determines how often the device listens for CSRmesh messages. Note: The value 0x00 implies that the device does not listen for messages. The value 0xFF implies that the device continuously listens for messages. */
    CsrInt8 txpower; /*!< \brief How loud should this device transmit messages. The TxPower, or transmit power field is an 8-bit signed integer in decibels that determines the radio transmit power for a device. */
    CsrUint8 ttl; /*!< \brief The initial default time to live for messages. The TimeToLive field is an 8-bit unsigned integer that determines the default value for the TTL packet field in an MTL packet. */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_PARAMETERS_T;

/*! \brief Upon receiving a CONFIG_DISCOVER_DEVICE message directed at the 0x0000 group identifier or to DeviceID of this device, the device responds with a CONFIG_DEVICE_IDENTIFIER message. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_DISCOVER_DEVICE_T;

/*! \brief Device identifier */
typedef struct
{
    CsrUint8 devicehash[8/size_in_bytes(CsrUint8)]; /*!< \brief Hash of the DeviceUUID. The DeviceHash field is a 64-bit hash of the DeviceUUID that is used to identify this device. */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_DEVICE_IDENTIFIER_T;

/*! \brief Upon receiving a CONFIG_GET_INFO message, directed at the DeviceID of this device, the device responds with a CONFIG_INFO message. The Info field of the CONFIG_GET_INFO message determines the information to be included in the CONFIG_INFO message. The following information values are defined: DeviceUUIDLow (0x00) contains the least significant eight octets of the DeviceUUID state value. DeviceUUIDHigh (0x01) contains the most significant eight octets of the DeviceUUID state value. ModelsLow (0x02) contains the least significant eight octets of the ModelsSupported state value. ModelsHigh (0x03) contains the most significant eight octets of the ModelsSupported state value. */
typedef struct
{
    CsrUint8 info; /*!< \brief The type of information being requested. The Info field is an 8-bit unsigned integer that enumerates the information being requested. The following values are defined: 0x00 = UUID Low, 0x01 UUID High, 0x02 = Model Low, 0x03 = Model High, 0x04 = VID_PID_Version, 0x05 = Appearance, 0x06 = LastETag, 0x07 = Conformance, 0x08 = Stack Version. */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_GET_INFO_T;

/*! \brief Current device information */
typedef struct
{
    CsrUint8 info; /*!< \brief The type of information. The Info field is an 8-bit unsigned integer that enumerates the information being requested. The following values are defined:0x00 = UUID Low, 0x01 = UUID High, 0x02 = Model Low, 0x03 = Model High. */
    CsrUint8 information[8/size_in_bytes(CsrUint8)]; /*!< \brief Information requested. The Information field is a 64-bit value that contains information determined by the Info field. If the Info field is 0x00, the Information field contains the least significant 64-bits of the DeviceUUID. If the Info field is 0x01, the Information field contains the most significant 64-bits of the DeviceUUID. If the Info field is 0x02, the Information field contains the least significant 64-bits of the ModelsSupported bit-field. If the Info field is 0x03, the Information field contains the most significant 64-bits of the ModelsSupported bit-field. */
    CsrUint8 information_len; /*!< \brief Length of the information field */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_INFO_T;

/*! \brief  */
typedef struct
{
    CsrUint8 txqueuesize; /*!< \brief Max number of messages in transmit queue. */
    CsrUint8 relayrepeatcount; /*!< \brief Number of times to transmit a relayed message. */
    CsrUint8 devicerepeatcount; /*!< \brief Number of times to transmit a message from this device. */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_SET_MESSAGE_PARAMS_T;

/*! \brief  */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_GET_MESSAGE_PARAMS_T;

/*! \brief  */
typedef struct
{
    CsrUint8 txqueuesize; /*!< \brief Max number of messages in transmit queue. */
    CsrUint8 relayrepeatcount; /*!< \brief Number of times to transmit a relayed message. */
    CsrUint8 devicerepeatcount; /*!< \brief Number of times to transmit a message from this device. */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_CONFIG_MESSAGE_PARAMS_T;


/*!@} */
#endif /* __CONFIG_MODEL_H__ */

