/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_private.h
 *
 *  DESCRIPTION
 *      Header file for the connection manager private interfaces
 *
 *
 ******************************************************************************/

#ifndef __CM_PRIVATE_H__
#define __CM_PRIVATE_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <main.h>
#include <bluetooth.h>
#include <bt_event_types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_api.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Invalid hci handle indicating the handle is not used */
#define CM_INVALID_HCI_HANDLE               (0xFFFF)

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/* Structure containing device info */
typedef struct
{
    /* IMPORTANT NOTE:
     * Adding any member variable affects the size of the CM_CONN_INFO_T.
     * Thus it is required to update the size of the CM_SIZEOF_CONN_INFO
     * in cm_types.h. Otherwise system panic while accessing the invalid 
     * pointer
     */

    /* Boolean flag to keep track if the link is encrypted or not. */
    bool                                    encryption_enabled;

    /* Remote device connection parameters set */
    CM_DEV_CONN_PARAM_T                     conn_param;

    /* HCI connection handle for the connection. */
    hci_connection_handle_t                 hci_conn_handle;

    /* Connection identifier for the connection. */
    uint16                                  cid;

    /* TYPED_BD_ADDR_T of the host to which device is bonded.*/
    TYPED_BD_ADDR_T                         remote_bd_addr;

    /* Device state in the connection manager */
    cm_dev_state                            device_state;

    /* Peer device connected as central or peripheral */
    cm_peer_con_role                        peer_con_role;

    /* peer device bond state */
    cm_dev_bond_state                       bond_state;

}CM_CONN_INFO_T;

/* Connection manager main data structure which could be used by other
 * connection manager components such as conn manager client, server, central,
 * peripheral, security and applications
 */
typedef struct
{
    /* Application Handler */
    CM_HANDLERS_T                           *app_handlers;

    /* Connection information */
    CM_CONN_INFO_T                          *cm_conn_info;
    
    /* Maximum number of connections */
    uint16                                  max_connections;

    /*! \brief enable number of completed message to be forwarded to 
     *   all clients and servers and the applicaiton
     */
    bool                                    num_completed_packets_enabled;
}CM_MAIN_DATA_T;

/* Connection manager defined panic codes */
typedef enum
{
    /* Failure while setting the GAP mode */
    cm_panic_gap_set_mode,
            
    /* Failure while setting advertisement parameters */
    cm_panic_set_advert_params,

    /* Failure while setting advertisement data */
    cm_panic_set_advert_data,

    /* Failure while setting scan response data */
    cm_panic_set_scan_rsp_data,

    /* Failure while establishing connection */
    cm_panic_connection_est,

    /* Failure while registering GATT DB with firmware */
    cm_panic_db_registration,

    /* Failure while reading NVM */
    cm_panic_nvm_read,

    /* Failure while writing NVM */
    cm_panic_nvm_write,

    /* Failure while deleting device from whitelist */
    cm_panic_delete_whitelist,

    /* Failure while adding device to whitelist */
    cm_panic_add_whitelist,

    /* Failure while triggering connection parameter update procedure */
    cm_panic_con_param_update,

    /* Event received in an unexpected application state */
    cm_panic_invalid_state,

    /* Unexpected beep type */
    cm_panic_unexpected_beep_type,

    /* Not supported UUID*/
    cm_panic_uuid_not_supported,

    /* Panic code to specify that the cm_client_service  list size is more than
     * than the MAX_CLIENT_SERVICE_HANDLERS size.
     */
    cm_panic_client_service_size_exceeded,

    /* Panic code to specify that the cm_server_service list size is more than
     * than the MAX_SERVER_SERVICE_HANDLERS size.
     */
    cm_panic_server_service_size_exceeded,

    /* Failure as there are no device slots to add the new device onto the
     * connection manager device database.
     */
    cm_panic_device_db_add_error,

    /* Panic because of invalid device id */
    cm_panic_invalid_device_id,

    /* Failure while reading Tx Power Level */
    cm_panic_read_tx_pwr_level

}cm_panic_code;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* Initializes the CM Private */
extern void CMPrivateInit(CM_INIT_PARAMS_T *cm_init_params);

/* Gets the device id of the device */
extern device_handle_id CMGetDeviceId(uint16 event_id, void *event);

/* Checks the device id valid or not */
extern bool CMIsDeviceIdValid(device_handle_id device_id);

/* Sets the connection parameters of the device */
extern void CMSetConnParam(device_handle_id device_id, 
                              CM_DEV_CONN_PARAM_T *connection_param);

/* Sets the bonded flag for the device to true  */
extern void CMSetDeviceBondState(device_handle_id device_id, 
                                 cm_dev_bond_state bond_state);

/* Sets the encrypt state of the device to TRUE */
extern void CMSetDeviceEncryptState(device_handle_id device_id, bool enc_state);

/* Gets the device's encrypt state for the device */
extern bool CMGetDeviceEncryptState(device_handle_id device_id);

#if defined (CLIENT)
/* Sets the Ongoing read Handle for the device */
extern void CMSetOngoingReadHandle(device_handle_id device_id, uint16 handle);

/* Gets the Ongoing read Handle for the device */
extern uint16 CMGetOngoingReadHandle(device_handle_id device_id);

/* Sets the Ongoing write Handle for the device */
extern void CMSetOngoingWriteHandle(device_handle_id device_id, uint16 handle);

/* Gets the Ongoing write Handle for the device */
extern uint16 CMGetOngoingWriteHandle(device_handle_id device_id);
#endif /* CLIENT */

/* Raises the panic */
extern void CMReportPanic(cm_panic_code panic_code);

/* Sends the connection manager events to application */
extern void CMNotifyEventToApplication(cm_event event_type,
                                       CM_EVENT_T *p_event_data);

/* Resets the device connection parameters */
extern void CMResetDevConnParam(device_handle_id device_id);

/* Returns the connection role of the given device */
extern cm_peer_con_role CMGetDevConnType(device_handle_id device_id);

/* Returns pointer to the connection database. */
extern CM_MAIN_DATA_T* CMGetMainData(void);

/* Sets the device state in the connection manager */
extern void CMSetDevState(device_handle_id device_id, cm_dev_state state);

#endif /* __CM_PRIVATE_H__ */
