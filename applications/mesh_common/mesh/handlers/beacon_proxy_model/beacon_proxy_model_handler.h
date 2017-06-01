/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      beacon_proxy_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __BEACON_PROXY_MODEL_HANDLER_H__
#define __BEACON_PROXY_MODEL_HANDLER_H__

#include "beaconproxy_server.h"
#include "beacon_model.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*============================================================================*
 *  Public Types
 *============================================================================*/
/* Application Model Handler Data Structure */
typedef struct
{
    /* Beacon proxy model state data */
    CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES_T cmd_status;
    CSRMESH_BEACONPROXY_PROXY_STATUS_T proxy_status;
}BEACON_PROXY_HANDLER_DATA_T;

typedef struct
{
    CsrUint16 group_id[MAX_MANAGED_BEACON_GRPS];
    CsrUint16 dev_id[MAX_MANAGED_BEACON_DEVS];
    CsrUint16 dev_grp_bitmask[MAX_MANAGED_BEACON_DEVS];
}BEACON_PROXY_DEVICE_GROUP_LIST_T;
/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* The function initialises the beacon proxy model handler */
extern void BeaconProxyModelHandlerInit(CsrUint8 nw_id,
                                        CsrUint16 model_groups[],
                                        CsrUint16 num_groups);

/* The function initialises the beacon proxy model data in the handler */
extern void BeaconProxyModelDataInit(
                    BEACON_PROXY_HANDLER_DATA_T* beacon_proxy_handler_data);

/* The function reads the beacon proxy model data from NVM */
extern void WriteBeaconProxyModelDataOntoNVM(void);

/* The function writes the beacon proxy model data onto NVM */
extern void ReadBeaconProxyModelDataFromNVM(void);

/* This function is called to check whether the passed device is present in the 
 * beacon proxy database 
 */
extern bool CheckForDeviceInterest(CsrUint16 dev_id);

/* This function checks whether the passed device is a group or not.*/
extern bool IsAGroupDevice(CsrUint16 dev_id);

/* The function returns the groups the beacon belongs to */
extern CsrUint16 GetBeaconGroups(CsrUint16 dev_id, uint16 grps[]);

#endif /* __BEACON_PROXY_MODEL_HANDLER_H__ */
