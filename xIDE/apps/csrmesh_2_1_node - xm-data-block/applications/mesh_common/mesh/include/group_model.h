/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Group_model.h
 *
 *  \brief Defines CSRmesh Group Model specific data structures\n
 */
/******************************************************************************/

#ifndef __GROUP_MODEL_H__
#define __GROUP_MODEL_H__

#include "csr_mesh_model_common.h"

/*! \addtogroup Group_Model 
 *  \brief Group Model API
 *
 * <br>The Group Model configures the set of groups that a device transmits or processes. The groups are configured for a particular model. Each model can have one or more groups associated with; the groups are organized by an index value per model. For example, a device can support 4 group identifiers for one model, and 2 group identifiers for another model.<br><br> Some models have no specific operation codes. The support of these models (for example the switch model) implies the support of the group model.It is possible to set the group identifiers for all models in a device using the model identifier 0xFF. <h4>Group State</h4>The group model has the following states:<ul style='list-style:square;list-style-position: inside;'><li style='list-style-type:square;'>NumberOfGroups</li><li style='list-style-type:square;'>NumberOfInstances</li> <li style='list-style-type:square;'>GroupIDs</li></ul> <h5> NumberOfGroups</h5>NumberOfGroups is an array of 8-bit unsigned integers, with one entry per model. The value of each entry is the number of group identifiers that the device supports for a given model.<h5>NumberOfInstances</h5> NumberOfInstances is an array of arrays of 8-bit unsigned integers. The top level array holds an array of instances for each model. The second level array holds the NumberOfInstances values for each GroupID that a device exposes in the NumberOfGroups state value<h5>GroupIDs</h5>GroupIDs is an array of arrays of 16-bit unsigned integers. The top level array holds an array of instances for each model. The second level array holds the GroupID values for that model, sized at least as big as the NumberOfGroups state value for that model.
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief CSRmesh Group Model message types */

/*! \brief Getting Number of Group IDs: Upon receiving a GROUP_GET_NUMBER_OF_MODEL_GROUPS message, where the destination address is the DeviceID of this device, the device responds with a GROUP_NUMBER_OF_MODEL_GROUPS message with the number of Group IDs that the given model supports on this device. */
typedef struct
{
    CsrUint8 model; /*!< \brief Model number */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_GROUP_GET_NUMBER_OF_MODEL_GROUPIDS_T;

/*! \brief Get number of groups supported by the model */
typedef struct
{
    CsrUint8 model; /*!< \brief Model number */
    CsrUint8 numgroups; /*!< \brief Number of model groups */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_GROUP_NUMBER_OF_MODEL_GROUPIDS_T;

/*! \brief Setting Model Group ID: Upon receiving a GROUP_SET_MODEL_GROUPID message, where the destination address is the DeviceID of this device, the device saves the Instance and GroupID fields into the appropriate state value determined by the Model and GroupIndex fields. It then responds with a GROUP_MODEL_GROUPID message with the current state information held for the given model and the GroupIndex values. */
typedef struct
{
    CsrUint8 model; /*!< \brief Model number */
    CsrUint8 groupindex; /*!< \brief Index of the group */
    CsrUint8 instance; /*!< \brief Instance of the group */
    CsrUint16 groupid; /*!< \brief Group identifier */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_GROUP_SET_MODEL_GROUPID_T;

/*! \brief Getting Model Group ID: Upon receiving a GROUP_GET_MODEL_GROUPID message, where the destination address is the DeviceID of this device, the device responds with a GROUP_MODEL_GROUPID message with the current state information held for the given Model and GroupIndex values. */
typedef struct
{
    CsrUint8 model; /*!< \brief Model number */
    CsrUint8 groupindex; /*!< \brief Index of the group */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_GROUP_GET_MODEL_GROUPID_T;

/*! \brief GroupID of a model */
typedef struct
{
    CsrUint8 model; /*!< \brief Model number */
    CsrUint8 groupindex; /*!< \brief Index of the GroupID in this model */
    CsrUint8 instance; /*!< \brief Instance of the group */
    CsrUint16 groupid; /*!< \brief Group identifier */
    CsrUint8 tid; /*!< \brief Transaction identifier. The TID, or transaction identifier field is an 8-bit integer that identifies a given message with a known transaction from a source device. */
} CSRMESH_GROUP_MODEL_GROUPID_T;


/*!@} */
#endif /* __GROUP_MODEL_H__ */

