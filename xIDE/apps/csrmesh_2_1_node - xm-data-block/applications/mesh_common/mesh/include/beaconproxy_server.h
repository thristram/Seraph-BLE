/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file beaconproxy_server.h
 *
 *  \brief This file provides the prototypes of the server functions defined
 *         in the CSRmesh BeaconProxy model
 */
/******************************************************************************/

#ifndef __BEACONPROXY_SERVER_H__
#define __BEACONPROXY_SERVER_H__

/*! \addtogroup BeaconProxy_Server
 * @{
 */
#include "beaconproxy_model.h"

/*============================================================================*
    Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * BeaconProxyModelInit
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
extern CSRmeshResult BeaconProxyModelInit(CsrUint8 nw_id, CsrUint16 *group_id_list, CsrUint16 num_groups,
                                         CSRMESH_MODEL_CALLBACK_T app_callback);


/*----------------------------------------------------------------------------*
 * BeaconProxySetupBeaconList
 */
/*! \brief Sets up a list to manage added beacons
 *  
 *  \param beacon_list Pointer to buffer to hold the added beacon device IDs and
                       group IDs. The buffer should be large enough to store
                       max_devices + max_groups
 *  \param max_devices Maximum number of devices that the proxy can manage
 *  \param max_groups  Maximum groups of beacons the proxy can manage
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxySetupBeaconList(CsrUint16 *beacon_list,
                                                CsrUint8 max_groups,
                                                CsrUint8 max_devices);


/*----------------------------------------------------------------------------*
 * BeaconProxyCommandStatusDevices
 */
/*! \brief Generic acknowledgement - the transaction ID permits reconciliation with sender.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyCommandStatusDevices(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES_T *p_params);

/*----------------------------------------------------------------------------*
 * BeaconProxyProxyStatus
 */
/*! \brief Provides generic information on the internal states of a Proxy. Including number of managed nodes, groups, states of the various queues.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACONPROXY_PROXY_STATUS_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyProxyStatus(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACONPROXY_PROXY_STATUS_T *p_params);

/*----------------------------------------------------------------------------*
 * BeaconProxyDevices
 */
/*! \brief Provide the list of Devices and Groups currently managed by a given Proxy. This will be a multiple parts message.
 * 
 * This function packs the given parameters into a CSRmesh message and sends
 * it over the network. 
 * 
 *
 * \param nw_id Network identifier over which the message has to be sent.
 * \param dest_id 16-bit identifier of the destination device/group
 * \param ttl TTL value with which the message needs to be sent.
 * \param p_params Pointer to the message parameters of type
              \ref CSRMESH_BEACONPROXY_DEVICES_T
 *
 * \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult BeaconProxyDevices(
                                  CsrUint8 nw_id,
                                  CsrUint16 dest_id,
                                  CsrUint8 ttl,
                                  CSRMESH_BEACONPROXY_DEVICES_T *p_params);

/*!@} */
#endif /* __BEACONPROXY_SERVER_H__ */

