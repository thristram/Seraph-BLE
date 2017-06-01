/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Tracker_model.h
 *
 *  \brief Defines CSRmesh Tracker Model specific data structures\n
 */
/******************************************************************************/

#ifndef __TRACKER_MODEL_H__
#define __TRACKER_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Tracker_Model 
 *  \brief Tracker Model API
 *
 * <br>The Tracker Model finds assets within a mesh network. When a device wishes to find an asset it sends a find message into the mesh network. Any device that has recently heard from an asset that is trying to be found responds with an appropriate message indicating when and how loud it heard this message. It is possible that a device that has only heard a message with a weak signal, delays its response or does not respond at all if it subsequently hears another device saying that it had heard the device at a stronger signal level more recently. This optimises the number of messages transmitted by devices in the network when an asset has been moving around the network. <br><br> A device that is listening for and recording asset presence messages do not need to save every presence message that it receives. It could only save a subset of messages based on various heuristics such as signal strength or number of devices or frequency of message receipt.<h4>Tracker Cache</h4> The tracker model has a tracker cache, with a list of the last seen asset broadcasts with associated data. Such associated data includes the received signal strength and the time when that asset broadcast was received. It is impossible to store all possible asset broadcast messages received on a device and therefore it is assumed that devices implementing the tracker model uses a least-recently-used algorithm to remove old and/or weak data. The actual algorithm used is an implementation issue.<h4>Tracker Server Behaviour</h4>When a tracker device hears an ASSET_ANNOUNCE it first listens for other TRACKER_REPORT reports for the same asset to determine if the cached information is either louder or newer than other devices. If the server does not receive any other TRACKER_REPORT message or the RSSI and Age fields of those messages are louder or younger than the information in its tracker cache, then it does not send a TRACKER_REPORT message for the asset. The timing of sending a TRACKER_REPORT message is a function of the age and RSSI valu
the tracker cache. The RSSI heard is converted to a delay in milliseconds using the formula: (DelayOffset - RSSI) * DelayFactor.<br><br> RSSI is a negative number usually between -40 and -100. DelayOffset is 60 by default and DelayFactor is 30. This gives a delay of 3 to 4.8 seconds before a REPORT is sent.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Tracker Model message types */

/*! \brief Finding an Asset:  Upon receiving a TRACKER_FIND message, the server checks its tracker cache to see if it has received an ASSET_ANNOUNCE message recently that has the same DeviceID. If it finds one, it will send a TRACKER_FOUND message with the cached information. */
typedef struct
{
    CsrUint16 assetdeviceid; /*!< \brief Device ID for this asset */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TRACKER_FIND_T;

/*! \brief Asset found */
typedef struct
{
    CsrUint16 assetdeviceid; /*!< \brief Device ID of this asset */
    CsrInt8 rssi; /*!< \brief RSSI of ASSET_ANNOUNCE */
    CsrUint8 zone; /*!< \brief Zone: 0=Immediate 1=Near 2=Distant */
    CsrUint16 ageseconds; /*!< \brief How long since last seen */
    CsrUint16 sideeffects; /*!< \brief Side effects for this asset */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_TRACKER_FOUND_T;

/*! \brief Asset report */
typedef struct
{
    CsrUint16 assetdeviceid; /*!< \brief Device ID of this asset */
    CsrInt8 rssi; /*!< \brief RSSI  of ASSET_ANNOUNCE */
    CsrUint8 zone; /*!< \brief Zone: 0=Immediate 1=Near 2=Distant */
    CsrUint16 ageseconds; /*!< \brief How long since last seen */
    CsrUint16 sideeffects; /*!< \brief Side effects for this asset */
} CSRMESH_TRACKER_REPORT_T;

/*! \brief Set tracker proximity config */
typedef struct
{
    CsrInt8 zone0rssithreshold; /*!< \brief Threshold in dBm for Zone 0  (signed). Default -60 */
    CsrInt8 zone1rssithreshold; /*!< \brief Threshold in dBm for Zone 1 (signed). Default -83 */
    CsrInt8 zone2rssithreshold; /*!< \brief Threshold in dBm for Zone 2 (signed). Default -100 */
    CsrUint16 cachedeleteinterval; /*!< \brief How long until cached entry is deleted (seconds). Default 600 */
    CsrUint8 delayoffset; /*!< \brief Offset value for RSSI to delay computation. Default 60 */
    CsrUint8 delayfactor; /*!< \brief Factor for RSSI to delay computation. Default 30 */
    CsrUint16 reportdest; /*!< \brief Destination ID to which asset reports will be sent. Default 0  */
} CSRMESH_TRACKER_SET_PROXIMITY_CONFIG_T;


/*!@} */
#endif /* __TRACKER_MODEL_H__ */

