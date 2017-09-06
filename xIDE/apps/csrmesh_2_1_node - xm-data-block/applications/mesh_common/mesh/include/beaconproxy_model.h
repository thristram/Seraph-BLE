/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file BeaconProxy_model.h
 *
 *  \brief Defines CSRmesh BeaconProxy Model specific data structures\n
 */
/******************************************************************************/

#ifndef __BEACONPROXY_MODEL_H__
#define __BEACONPROXY_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup BeaconProxy_Model 
 *  \brief BeaconProxy Model API
 *
 * The messages in this model are used to manage a beacon proxy, controlling which nodes it is a proxy for, and allowing its currently stored messages to be interrogated and managed .A proxy can control groups of beacons, and can have group address assigned so that groups of beacons can be updated with one message. Groups can also be used in the conventional way, but in that case we'll lose the ability to know whether everyone has received the message.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/
/* Maximum managed beacon devices */
#define MAX_MANAGED_BEACON_DEVICES   10
#define MAX_MANAGED_BEACON_GROUPS    5

/*! \brief CSRmesh BeaconProxy Model message types */

/*! \brief Message can be sent to a beacon-proxy to add to the list of devices it manages. This request will be acknowledged using a Proxy Command Status Devices. Up to four device ID can be specified in this message – Group can be defined. This message also permits the flushing of pending messages for managed IDs. */
typedef struct
{
    CsrUint8 numdevices; /*!< \brief Bits 0..2 How many device are in this message.If bit 6 is set, the first device address is a GROUP address, to which the other device will be assigned. If bit 7 is set, queued messages for these devices will be cleared. */
    CsrUint8 deviceaddresses[8/size_in_bytes(CsrUint8)]; /*!< \brief Device addresses to add. The first can be a group address. */
    CsrUint8 deviceaddresses_len; /*!< \brief Length of the deviceaddresses field */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACONPROXY_ADD_T;

/*! \brief May be sent to a proxy to remove from the list of devices it manages. Any message queued for those devices will be cleared. This request will be acknowledged using a Proxy Command Status Devices. Groups can be used, implying that all its members will be removed from the proxy management. Specifying 0 in the Device Addresses will be interpreted as stopping all Proxy activities on the targeted proxy. */
typedef struct
{
    CsrUint8 numdevices; /*!< \brief Bits 0..2 How many device are in this message. Queued messages for these devices will be cleared. */
    CsrUint8 deviceaddresses[8/size_in_bytes(CsrUint8)]; /*!< \brief Device addresses to remove. These can be group addresses. Also, the special address '0' can be used to cease making this a proxy for all devices. */
    CsrUint8 deviceaddresses_len; /*!< \brief Length of the deviceaddresses field */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACONPROXY_REMOVE_T;

/*! \brief Generic acknowledgement - the transaction ID permits reconciliation with sender. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES_T;

/*! \brief Provides generic information on the internal states of a Proxy. Including number of managed nodes, groups, states of the various queues. */
typedef struct
{
    CsrUint16 nummanagednodes; /*!< \brief How many nodes this proxy is currently being a proxy for. If the proxy has been told to be a proxy for device address 0, this will be 0xFFFF */
    CsrUint8 nummanagedgroups; /*!< \brief How many groups this proxy is managing */
    CsrUint16 numqueuedtxmsgs; /*!< \brief How many messages are currently queued at this proxy for transmission to beacons */
    CsrUint16 numqueuedrxmsgs; /*!< \brief How many messages have been received from beacons at this proxy since its status was last queried */
    CsrUint16 maxqueuesize; /*!< \brief Current maximum queue size */
} CSRMESH_BEACONPROXY_PROXY_STATUS_T;

/*! \brief Fetch the current set of devices ID managed by a given proxy. This will be answered by one or more BEACON_PROXY_DEVICES messages. */
typedef struct
{
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_BEACONPROXY_GET_DEVICES_T;

/*! \brief Provide the list of Devices and Groups currently managed by a given Proxy. This will be a multiple parts message. */
typedef struct
{
    CsrUint8 packetnumber; /*!< \brief Packet number. Bit 7 will be set if this is the final packet. Following fields form the payload, and will be spread across multiple packets as  required. */
    CsrUint8 tid; /*!< \brief Transaction ID. The TID or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
    CsrUint8 numberofgroups; /*!< \brief Number of groups being managed. */
    CsrUint16 numberofdevices; /*!< \brief Number of device being managed. */
    CsrUint16 *groups; /*!< \brief Group IDs of managed groups. */
    CsrUint16 *devices; /*!< \brief Device IDs of managed groups. */
} CSRMESH_BEACONPROXY_DEVICES_T;


/*!@} */
#endif /* __BEACONPROXY_MODEL_H__ */

