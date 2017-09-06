/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia.h
 *  \brief This file defines header file for GAIA protocol
 */

#ifndef __GAIA_H__
#define __GAIA_H__

/*! \addtogroup GAIA_Service
 * @{
 */

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_types.h"

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/

/*! \brief GAIA Commands  */
#define GAIA_API_VERSION_MAJOR                                          (2)
#define GAIA_API_VERSION_MINOR                                          (0)

/* based on MTU size of 23, minus GATT headers */
#define GAIA_GATT_MAX_PACKET_SIZE                                       20

#define GAIA_GATT_VID_SIZE                                              2
#define GAIA_GATT_COMMAND_ID_SIZE                                       2

#define GAIA_PROTOCOL_FLAG_CHECK                                        (0x01)

#define GAIA_VENDOR_CSR                                                 (0x000A)
#define GAIA_VENDOR_NONE                                                (0x7FFE)

#define GAIA_ACK_MASK                                                   (0x8000)
#define GAIA_COMMAND_ID_MASK                                            (0x7FFF)
#define GAIA_ACK_MASK_H                                                 (0x80)
#define GAIA_COMMAND_INTENT_GET                                         (0x0080)
#define GET_GAIA_ACK_GROUP_ID(_opcode)              ( GAIA_ACK_MASK & (_opcode) )
#define GET_GAIA_COMMAND_ID(_opcode)         ( GAIA_COMMAND_ID_MASK & (_opcode) )

#define GAIA_COMMAND_TYPE_MASK                                          (0x7F00)
#define GAIA_COMMAND_TYPE_CONFIGURATION                                 (0x0100)
#define GAIA_COMMAND_TYPE_CONTROL                                       (0x0200)
#define GAIA_COMMAND_TYPE_STATUS                                        (0x0300)
#define GAIA_COMMAND_TYPE_FEATURE                                       (0x0500)
#define GAIA_COMMAND_TYPE_DATA_TRANSFER                                 (0x0600)
#define GAIA_COMMAND_TYPE_DEBUG                                         (0x0700)
#define GAIA_COMMAND_TYPE_EXTENSION                                     (0x2000)
#define GAIA_COMMAND_TYPE_NOTIFICATION                                  (0x4000)
#define GET_GAIA_COMMAND_GROUP_ID(_opcode)  ( GAIA_COMMAND_TYPE_MASK & (_opcode) )

#define GAIA_COMMAND_RESERVED                                           (0x0000)

#define GAIA_COMMAND_SET_SUPPORTED_FEATURES                             (0x022C)
#define GAIA_COMMAND_GET_API_VERSION                                    (0x0300)
#define GAIA_COMMAND_GET_CURRENT_RSSI                                   (0x0301)
#define GAIA_COMMAND_GET_CURRENT_BATTERY_LEVEL                          (0x0302)
#define GAIA_COMMAND_GET_MODULE_ID                                      (0x0303)
#define GAIA_COMMAND_GET_APPLICATION_VERSION                            (0x0304)
#define GAIA_COMMAND_GET_PEER_ADDRESS                                   (0x030A)
#define GAIA_COMMAND_GET_DFU_STATUS                                     (0x0310)
#define GAIA_COMMAND_GET_HOST_FEATURE_INFORMATION                       (0x0320)

#define GAIA_COMMAND_DATA_TRANSFER_SETUP                                (0x0601)
#define GAIA_COMMAND_DATA_TRANSFER_CLOSE                                (0x0602)
#define GAIA_COMMAND_HOST_TO_DEVICE_DATA                                (0x0603)
#define GAIA_COMMAND_DEVICE_TO_HOST_DATA                                (0x0604)
#define GAIA_COMMAND_VM_UPGRADE_CONNECT                                 (0x0640)
#define GAIA_COMMAND_VM_UPGRADE_DISCONNECT                              (0x0641)
#define GAIA_COMMAND_VM_UPGRADE_CONTROL                                 (0x0642)
#define GAIA_COMMAND_VM_UPGRADE_DATA                                    (0x0643)

#define GAIA_COMMAND_REGISTER_NOTIFICATION                              (0x4001)
#define GAIA_COMMAND_GET_NOTIFICATION                                   (0x4081)
#define GAIA_COMMAND_CANCEL_NOTIFICATION                                (0x4002)
#define GAIA_EVENT_NOTIFICATION                                         (0x4003)

#define GAIA_COMMAND_NOTIFICATION_EVENT_OFFSET                          0
#define GAIA_COMMAND_NOTIFICATION_ACK_SIZE                              2
#define GAIA_COMMAND_NOTIFICATION_ACK_STATUS_OFFSET                     0
#define GAIA_COMMAND_NOTIFICATION_ACK_EVENT_OFFSET                      1

#define GAIA_EVENT_START                                                (0x00)
#define GAIA_EVENT_RSSI_LOW_THRESHOLD                                   (0x01)
#define GAIA_EVENT_RSSI_HIGH_THRESHOLD                                  (0x02)
#define GAIA_EVENT_BATTERY_LOW_THRESHOLD                                (0x03)
#define GAIA_EVENT_BATTERY_HIGH_THRESHOLD                               (0x04)
#define GAIA_EVENT_DEVICE_STATE_CHANGED                                 (0x05)
#define GAIA_EVENT_PIO_CHANGED                                          (0x06)
#define GAIA_EVENT_BATTERY_CHARGED                                      (0x08)
#define GAIA_EVENT_CHARGER_CONNECTION                                   (0x09)
#define GAIA_EVENT_CAPSENSE_UPDATE                                      (0x0A)
#define GAIA_EVENT_USER_ACTION                                          (0x0B)
#define GAIA_EVENT_SPEECH_RECOGNITION                                   (0x0C)
#define GAIA_EVENT_DFU_STATE                                            (0x10)
#define GAIA_EVENT_VMUP_PACKET                                          (0x12)
#define GAIA_EVENT_HOST_NOTIFICATION                                    (0x13)

#define GAIA_STATUS_SUCCESS                                             (0x00)
#define GAIA_STATUS_NOT_SUPPORTED                                       (0x01)
#define GAIA_STATUS_NOT_AUTHENTICATED                                   (0x02)
#define GAIA_STATUS_INSUFFICIENT_RESOURCES                              (0x03)
#define GAIA_STATUS_AUTHENTICATING                                      (0x04)
#define GAIA_STATUS_INVALID_PARAMETER                                   (0x05)
#define GAIA_STATUS_INCORRECT_STATE                                     (0x06)
#define GAIA_STATUS_IN_PROGRESS                                         (0x07)

#define GAIA_PFS_MODE_READ                                              (0x00)
#define GAIA_PFS_MODE_OVERWRITE                                         (0x02)

#define GAIA_CONN_MODE_CONN                                             (0x11)
#define GAIA_CONN_MODE_CONN_DISC                                        (0x12)

#define GAIA_POWER_STATE_OFF                                            0x0
#define GAIA_POWER_STATE_ON                                             0x1

#define GAIA_GATT_PACKET_HEADER_SIZE                                    4

/*============================================================================*
 *  Public Data Types
 *============================================================================*/
/*!
 * \brief GAIA event notifications that can be sent to the application.
 *
 * An event handler callback function must be registered before events will
 * be received. See \link GaiaRegisterCallback \endlink
 */
typedef enum
{
    gaia_event_upgrade_connect,       /*!< \brief Upgrade connect request. \link see GAIA_EVENT_UPGRADE_CONNECT_T \endlink */
    gaia_event_upgrade_disconnect,    /*!< \brief Upgrade disconnect request. \link see GAIA_EVENT_UPGRADE_DISCONNECT_T \endlink */
} gaia_event;

/*!
 * \brief Event data sent with \link gaia_event_upgrade_connect \endlink
 *
 * Sent at the start of upgrade procedure
 */
typedef struct
{
} GAIA_EVENT_UPGRADE_CONNECT_T;

/*!
 * \brief Event data sent with \link gaia_event_upgrade_disconnect \endlink
 *
 * Sent at the end of upgrade procedure
 */
typedef struct
{
} GAIA_EVENT_UPGRADE_DISCONNECT_T;

/*!
 * \brief Union of all possible GAIA event data types.
 *
 * Event specific data may be accessed via the corresponding member according
 * to the event type (see \link gaia_event \endlink ).
 */
typedef union
{
    GAIA_EVENT_UPGRADE_CONNECT_T     upgrade_connect;       /*!< Data for gaia_event_upgrade_connect */
    GAIA_EVENT_UPGRADE_DISCONNECT_T  upgrade_disconnect;    /*!< Data for gaia_event_upgrade_disconnect */
    
} GAIA_EVENT_T;

/*!
 * \brief Prototype for a GAIA event handler callback function.
 *
 * Called by the GAIA library to notify the application of various events, see
 * \link gaia_event \endlink for a list of all possible events.
 *
 * \param[in] event The type of event being notified.
 * \param[in,out] data Pointer to the event data associated with the event.
 *
 * \return sys_status - The application should return sys_status_success if the
                        event was handled, and sys_status_failure otherwise, in
                        which case any expected return data will be ignored.
 */
typedef sys_status (*gaia_event_handler)(gaia_event event, GAIA_EVENT_T *data);

/*! \brief GAIA library structure */
typedef struct
{
    /*! \brief Indicates that connected host supports GAIA */
    bool gaia_host_connected;

    /*! \brief Indicates a data transfer session is in progress.
     * Smart Watch supports only on session at a time
     */
    bool data_transfer_session_on;

    /*! \brief Unique Session ID */
    uint16 session_id;

    /*! \brief Flag to check if it is a ongoing data session */
    bool data_transfer_ongoing;
    
    /*! \brief Callback to the application event handler */
    gaia_event_handler callback;       

} GAIA_LIB_DATA_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------
 *  GAIAProcessCommand
 *----------------------------------------------------------------------------*/
/*! \brief Process a GAIA command
 *
 * This function processess GAIA command
 * \param[in] payload_len Payload length
 * \param[in] payload Pointer to payload data
 * \returns Nothing
 *
 */
extern void GAIAProcessCommand(device_handle_id device_id, uint16 payload_len, uint8 *payload);

/*----------------------------------------------------------------------------
 *  GaiaIsHostConnected
 *----------------------------------------------------------------------------*/
/*! \brief Checks if GAIA host is connected
 *
 *  This function checks if GAIA host is connected
 * \returns TRUE if conencted,otherwise FALSE
 *
 */
extern bool GaiaIsHostConnected(void);

/*----------------------------------------------------------------------------
 *  GaiaSendPacket
 *----------------------------------------------------------------------------*/
/*! \brief Prepares a GAIA packet with header and given payload
 *
 *  Prepares a GAIA packet with header and given payload and sends BLE
 *  notification to host
 * \param[in] command_id  Command ID
 * \param[in] size_payload Payload length
 * \param[in] payload Pointer to payload data
 * \returns \ref cm_status_code
 *
 */
extern void GaiaSendPacket( device_handle_id device_id,
                            uint16 command_id,
                            uint16 size_payload,
                            uint8 *payload);
/*----------------------------------------------------------------------------
 *  GaiaLibInit
 *----------------------------------------------------------------------------*/
/*! \brief Initializes GAIA library data
 *
 *  This function initializes GAIA library data
 * \returns Nothing
 *
 */
extern void GaiaLibInit(void);

/*! \brief Handle a connection event in the GAIA library
 *
 *  \returns Nothing
 *
 */
extern void GaiaLibHandleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data);

/*! \brief Register callback function to handle GAIA events
 *
 * See \link gaia_event_handler \endlink
 *
 * \param[in] callback Pointer to application function that will handle GAIA events
 *
 */
extern void GaiaRegisterCallback(gaia_event_handler callback);


/*!@} */
#endif /* __GAIA_H__ */
