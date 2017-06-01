/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      beacon_model_handler.h
 *
 *
 ******************************************************************************/
#ifndef __BEACON_MODEL_HANDLER_H__
#define __BEACON_MODEL_HANDLER_H__

#include <timer.h>
#include <bluetooth.h>
#include "beacon_server.h"
#include "csr_mesh_model_common.h"
/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*============================================================================*
 *  Public Data
 *============================================================================*/
typedef struct
{
    CsrUint16                   meshinterval;
    CsrUint8                    meshtime;
    CsrUint8                    beacontype;
    CsrUint8                    beaconinterval;
    CsrInt8                     txpower;
    CsrUint8                    payloadlength;
    CsrUint16                   payloadid;
    CsrUint8                    payload[MAX_BEACON_PAYLOAD_SIZE];
#ifdef APP_BEACON_MODE
    timer_id                    beacon_tid;
#endif
#ifdef APP_PROXY_MODE
    CsrUint16                   dev_id;
    CsrUint8                    payload_offset;
#endif
}BEACON_INFO_T;

/* Application Model Handler Data Structure */
typedef struct
{
    BEACON_INFO_T               beacons[MAX_BEACONS_SUPPORTED];
    CsrUint8                    mesh_time_idx;
#ifdef APP_BEACON_MODE
    BD_ADDR_T                   bd_addr;
    CsrUint16                   time_wrap_cnt;
    CsrUint16                   time_remaining;
#endif
}BEACON_HANDLER_DATA_T;

/* The different modes application is in */
typedef enum 
{
    device_mode_beacon = 0,
    device_mode_mesh = 1
}device_beacon_mode_t;

/* The payload size available to update based for specific beacon types */

/* CSR : 11 (31 - 3(FLAGS) -4(UUID) - 3(Tx Power) - 6(Name) - 4(MANUF))*/
#define CSR_RETAIL_BEACON_PAYLOAD_SIZE          (11)
/* IBEACON : 21 (30 - 3(FLAGS) -6(MANUF + COMPANY CODE))*/
#define IBEACON_PAYLOAD_SIZE                    (21)
/* EDDYSTONE :19 (31 - 3(FLAGS) -4(SERV UUID) - 4(DATA UUID) -1(EDDY BEACON TYPE))*/
#define EDDYSTONE_PAYLOAD_SIZE                  (19)

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function initialises the beacon model handler */
extern void BeaconModelHandlerInit(CsrUint8 nw_id,
                                   CsrUint16 model_groups[],
                                   CsrUint16 num_groups);

/* The function initialises the beacon model data in the handler */
extern void BeaconModelDataInit(BEACON_HANDLER_DATA_T* power_handler_data);

/* The function reads the beacon model data from NVM */
extern void WriteBeaconModelDataOntoNVM(void);

/* The function writes the beacon model data onto NVM */
extern void ReadBeaconModelDataFromNVM(void);

#ifdef APP_PROXY_MODE
/* This function is called to retrieve the Tx Msg Stats from the 
 * beacon model handler.
 */
extern CsrUint16 GetQueuedTxMsgStats(void);

/* This function removes the stored beacon information for the specific device */
extern void RemoveBeaconInfoOfDevice(uint16 dev_id);
#endif

#ifdef APP_BEACON_MODE
/* This function is called to set the device onto mesh or beacon mode */
extern void BeaconSetMode(device_beacon_mode_t mode);
#endif

#endif /* __BEACON_MODEL_HANDLER_H__ */
