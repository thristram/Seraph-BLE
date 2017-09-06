/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
 /*! \file gaia.c
 *  \brief  This file handles the GAIA message groups.
 *
 * API for sending GAIA-format notifications to the Host/Client
 *
 * This file handles the basic GAIA message groups:
 * + data transfer
 * + notifications
 * + status
 * + control
 *
 * Detailed handling is provided by other files on a per message type, or
 * protocol, basis.
 *
 */

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <mem.h>

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "gaia.h"
#include "gaia_service.h"
#include "byte_utils.h"

/*
 * Optional OTAu support
 */
#ifdef GAIA_OTAU_SUPPORT
#include "gaia_otau.h"
#endif  /* GAIA_OTAU_SUPPORT    */

/*
 * GAIA Get/Set Host feature information
 */
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
#include "gaia_service_event.h"
#endif /* GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT */

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/

/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* Handle GAIA data transfer command */
static bool gaiaHandleDataTransferCommand(  device_handle_id device_id,
                                            uint16 command_id,
                                            uint16 payload_length,
                                            uint8 *payload);

/* Handles GAIA notification command */
static bool gaiaHandleNotificationCommand(  device_handle_id device_id,
                                            uint16 command_id,
                                            uint16 payload_length,
                                            uint8 *payload);

/* handle the GAIA notification ACK from the host */
static void gaiaHandleNotificationCommandAck(   device_handle_id device_id,
                                                uint16 command_id,
                                                uint16 payload_length,
                                                uint8 *payload);

/* handle the GAIA Status Command ACK from the host */
static void gaiaHandleStatusCommandAck( device_handle_id device_id,
                                        uint16 command_id,
                                        uint16 payload_length,
                                        uint8 *payload);

/* handle a GAIA control command from the host */
static void gaiaHandleControlCommand(   device_handle_id device_id,
                                        uint16 command_id,
                                        uint16 payload_length,
                                        uint8 *payload);


/*============================================================================*
 *  Private Data
 *============================================================================*/
static GAIA_LIB_DATA_T g_gaia_lib_data;

/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/

#ifdef GAIA_OTAU_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      notifyApplication
 *
 *  DESCRIPTION
 *      Notify the application of GAIA events via the callback function.
 *
 *  PARAMETERS
 *      event [in]      Type of event to notify
 *      *data [in,out]  Pointer to any event-specific data that should be sent
 *
 *  RETURNS
 *      Return status of the callback
 *
 *---------------------------------------------------------------------------*/
static sys_status notifyApplication(gaia_event event, GAIA_EVENT_T *data)
{
    if (g_gaia_lib_data.callback != NULL)
    {
        return g_gaia_lib_data.callback(event, data);
    }

    return sys_status_failed;
}
#endif


/*-----------------------------------------------------------------------------
 * NAME
 *      gaiaHandleStatusCommandAck
 *
 * DESCRIPTION
 *      Handles Status command
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void gaiaHandleStatusCommandAck( device_handle_id device_id,
                                        uint16 command_id,
                                        uint16 payload_length,
                                        uint8 *payload)
{
    switch ( GET_GAIA_COMMAND_ID(command_id) )
    {
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
        case GAIA_COMMAND_GET_HOST_FEATURE_INFORMATION:
        {
            uint8 response;

            response = *(uint8 *)(payload);
            payload += 1;
            payload_length -= 1;
            if(response == GAIA_STATUS_SUCCESS)
            {
                uint16 feature;
                GetNBytesFromStream(&feature,0,
                                             payload,0 ,2);

                if(feature == gaia_time)
                {
                    GaiaEventParseNotification(payload_length, payload);
                    GAIAEventWaitSyncAck(FALSE);
                }
                else
                {
                    uint8 info_type = *(payload + 2);
                    if(info_type == sync_info)
                    {
                        GaiaEventParseSyncData(payload_length, payload);
                        GAIAEventWaitSyncAck(FALSE);
                    }
                    else if(info_type == more_info)
                    {
                        GAIAEventWaitMoreInfoAck(FALSE);
                    }
                }
            }
        }
            break;
#endif
        default:
            break;
    }
}

/*-----------------------------------------------------------------------------
 * NAME
 *      gaiaHandleControlCommand
 *
 * DESCRIPTION
 *      Handles Control command
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void gaiaHandleControlCommand(   device_handle_id device_id,
                                        uint16 command_id,
                                        uint16 payload_length,
                                        uint8 *payload)
{
    uint8 response;

    switch (command_id)
    {
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
        case GAIA_COMMAND_SET_SUPPORTED_FEATURES:
            g_gaia_lib_data.gaia_host_connected = TRUE;
            response = GAIA_STATUS_SUCCESS ;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            SetClientNotificationMask(BufReadUint16BE(&payload));
            break;
#endif
        default:
            response = GAIA_STATUS_NOT_SUPPORTED;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            break;
    }
}

/*-----------------------------------------------------------------------------
 * NAME
 *      gaiaHandleDataTransferCommand
 *
 * DESCRIPTION
 *      Handle a Data Transfer command or return FALSE if we can't
 *
 * RETURNS
 *      Returns TRUE if command is handled successfully
 *----------------------------------------------------------------------------*/

static bool gaiaHandleDataTransferCommand(  device_handle_id device_id,
                                            uint16 command_id,
                                            uint16 payload_length,
                                            uint8 *payload)
{
    uint8 response;

    switch (command_id)
    {
        case GAIA_COMMAND_DATA_TRANSFER_SETUP:
            if (g_gaia_lib_data.data_transfer_session_on)
            {
                response = GAIA_STATUS_INSUFFICIENT_RESOURCES;
            }
            else
            {
                /* It is not possible to reach here if a session is
                 * already on. So no need to check for duplicate session
                 * ID. Present protocol does not define valid range for
                 * session ID. So we cannot check that here */
                g_gaia_lib_data.session_id = BufReadUint16BE(&payload);
                payload_length -= 2;
                g_gaia_lib_data.data_transfer_session_on = TRUE;
                response = GAIA_STATUS_SUCCESS;
            }

            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            return TRUE;
            break;

        case GAIA_COMMAND_DATA_TRANSFER_CLOSE:
            if (g_gaia_lib_data.data_transfer_session_on)
            {
                if (BufReadUint16BE(&payload) == g_gaia_lib_data.session_id)
                {
                    response = GAIA_STATUS_SUCCESS;
                }
                else
                    response = GAIA_STATUS_INVALID_PARAMETER;
            }
            else
            {
                response = GAIA_STATUS_INCORRECT_STATE;
            }
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            /* Reset the session parameters in any case */
            g_gaia_lib_data.session_id = 0;
            g_gaia_lib_data.data_transfer_session_on = FALSE;
            g_gaia_lib_data.data_transfer_ongoing = FALSE;
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
            GaiaNotifyTransferState(g_gaia_lib_data.data_transfer_ongoing);
#endif
            return TRUE;
            break;

        case GAIA_COMMAND_HOST_TO_DEVICE_DATA:
            if (g_gaia_lib_data.data_transfer_session_on)
            {
                if (BufReadUint16BE(&payload) == g_gaia_lib_data.session_id)
                {
                    if(!g_gaia_lib_data.data_transfer_ongoing)
                    {
                        g_gaia_lib_data.data_transfer_ongoing = TRUE;
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
                        GaiaNotifyTransferState(g_gaia_lib_data.data_transfer_ongoing);
#endif
                    }
                    payload_length -= 2;
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
                    GaiaEventParseTransferData(payload_length, payload);
#endif
                    response = GAIA_STATUS_SUCCESS;
                }
                else
                {
                    response = GAIA_STATUS_INVALID_PARAMETER;
                }
            }
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            return TRUE;
            break;

#ifdef GAIA_OTAU_SUPPORT
        /* GAIA OTAu specific */
        case GAIA_COMMAND_VM_UPGRADE_CONNECT:
            response = GAIA_STATUS_SUCCESS;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            notifyApplication(gaia_event_upgrade_connect, NULL);
            return TRUE;
            break;

        case GAIA_COMMAND_VM_UPGRADE_DISCONNECT:
            response = GAIA_STATUS_SUCCESS;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            notifyApplication(gaia_event_upgrade_disconnect, NULL);
            return TRUE;
            break;

        case GAIA_COMMAND_VM_UPGRADE_CONTROL:
            response = GAIA_STATUS_SUCCESS;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            GaiaOtauHandleVmUpgradeControlCommand(device_id, payload_length, payload);
            return TRUE;
            break;
#endif  /* GAIA_OTAU_SUPPORT */

        default:
            response = GAIA_STATUS_NOT_SUPPORTED;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
            break;
    }
    return FALSE;
}

/*-----------------------------------------------------------------------------
 * NAME
 *     gaiaHandleNotificationCommand
 *
 * DESCRIPTION
 *     Handle a debugging command or return FALSE if we can't
 *
 * RETURNS
 *      Returns TRUE if command is handled successfully
 *----------------------------------------------------------------------------*/

static bool gaiaHandleNotificationCommand(  device_handle_id device_id,
                                            uint16 command_id,
                                            uint16 payload_length,
                                            uint8 *payload)
{
    uint8 event_code;
    uint8 response[GAIA_COMMAND_NOTIFICATION_ACK_SIZE];
    event_code = payload[GAIA_COMMAND_NOTIFICATION_EVENT_OFFSET];

    switch (command_id)
    {
        case GAIA_COMMAND_REGISTER_NOTIFICATION:
            {
                response[GAIA_COMMAND_NOTIFICATION_ACK_EVENT_OFFSET] = event_code;

                switch (event_code )
                {
                    case GAIA_EVENT_VMUP_PACKET:
                        response[GAIA_COMMAND_NOTIFICATION_ACK_STATUS_OFFSET] = GAIA_STATUS_SUCCESS;
                        break;

                    default:
                        response[GAIA_COMMAND_NOTIFICATION_ACK_STATUS_OFFSET] = GAIA_STATUS_INVALID_PARAMETER;
                        break;
                }

                GaiaSendPacket( device_id,
                                command_id | GAIA_ACK_MASK,
                                GAIA_COMMAND_NOTIFICATION_ACK_SIZE,
                                response);
            }
            break;

        case GAIA_EVENT_NOTIFICATION:
            if(event_code == GAIA_EVENT_HOST_NOTIFICATION)
            {
                response[0] = GAIA_STATUS_SUCCESS;

                /* copy the event type */
                response[1] = event_code;

                GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 2, response);
                
#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
                GaiaEventParseNotification(payload_length, payload);
#endif                
            }
            else
            {
                response[0] = GAIA_STATUS_NOT_SUPPORTED;

                /* return the event code. We don't bother as we support only
                 * one event
                 */
                response[1] = event_code;

                GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 2, response);
            }

            return TRUE;
            break;

        default:
            response[0] = GAIA_STATUS_NOT_SUPPORTED;
            GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, response);
            break;
    }

    return FALSE;
}

/*-----------------------------------------------------------------------------
 * NAME
 *     gaiaHandleNotificationCommandAck
 *
 * DESCRIPTION
 *     Handle a debugging ack command or return FALSE if we can't
 *
 * RETURNS
 *      None
 *----------------------------------------------------------------------------*/

static void gaiaHandleNotificationCommandAck(   device_handle_id device_id,
                                                uint16 command_id,
                                                uint16 payload_length,
                                                uint8 *payload)
{
    uint8 event_code,command_status;
    switch ( GET_GAIA_COMMAND_ID(command_id) )
    {
        case GAIA_COMMAND_REGISTER_NOTIFICATION:
            command_status = *(uint8 *)(payload);
            (uint8 *)(payload)++;
            event_code = *(uint8 *)(payload);
            (uint8 *)(payload)++;
            break;

        default:
            break;
    }
}

/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaLibInit
 *
 *  DESCRIPTION
 *      Initializes GAIA data
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void GaiaLibInit(void)
{
     g_gaia_lib_data.gaia_host_connected = FALSE;
     g_gaia_lib_data.data_transfer_session_on = FALSE;
     g_gaia_lib_data.session_id = 0;
     g_gaia_lib_data.data_transfer_ongoing = FALSE;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaLibHandleConnNotify
 *
 *  DESCRIPTION
 *      Handle any initialisation required for a connection event
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void GaiaLibHandleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data)
{
    GaiaLibInit();
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      GAIAProcessCommand
 *
 *  DESCRIPTION
 *      Process a GAIA command
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void GAIAProcessCommand(device_handle_id device_id, uint16 payload_len, uint8 *payload)
{
    uint16 command_id = GAIA_COMMAND_RESERVED;
    uint8 response;

    if (payload_len >= GAIA_GATT_VID_SIZE)
    {
        /* First two bytes contain vendor id in big-endian format */
        if (BufReadUint16BE(&payload) != GAIA_VENDOR_CSR)
            return;
        payload_len -= GAIA_GATT_VID_SIZE;
    }

    if (payload_len >= GAIA_GATT_COMMAND_ID_SIZE)
    {
        /* next two bytes contain commandId in big-endian format */
        command_id = BufReadUint16BE(&payload);
        payload_len -= GAIA_GATT_COMMAND_ID_SIZE;
    }

    if(GET_GAIA_ACK_GROUP_ID(command_id))
    {
        switch (GET_GAIA_COMMAND_GROUP_ID(command_id))
        {

            case GAIA_COMMAND_TYPE_STATUS:
                gaiaHandleStatusCommandAck(device_id, command_id, payload_len, payload);
                break;

            case GAIA_COMMAND_TYPE_NOTIFICATION:
                gaiaHandleNotificationCommandAck(device_id, command_id, payload_len, payload);
                break;

            default:
                break;
        }
    }
    else
    {
        switch (GET_GAIA_COMMAND_GROUP_ID(command_id))
        {
            case GAIA_COMMAND_TYPE_CONTROL:
                gaiaHandleControlCommand(device_id, command_id, payload_len, payload);
                break;

            case GAIA_COMMAND_TYPE_DATA_TRANSFER:
                gaiaHandleDataTransferCommand(device_id, command_id, payload_len, payload);
                break;

            case GAIA_COMMAND_TYPE_NOTIFICATION:
                gaiaHandleNotificationCommand(device_id, command_id, payload_len, payload);
                break;

            default:
                response = GAIA_STATUS_NOT_SUPPORTED;
                GaiaSendPacket(device_id, command_id | GAIA_ACK_MASK, 1, &response);
                break;
        }
    }
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GaiaIsHostConnected
 *
 *  DESCRIPTION
 *      Checks if GAIA host is connected
 *
 *  RETURNS:
 *      TRUE if host is connected
 *----------------------------------------------------------------------------*/
extern bool GaiaIsHostConnected(void)
{
    return g_gaia_lib_data.gaia_host_connected;
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      GaiaSendPacket
 *
 *  DESCRIPTION
 *      Prepares a GAIA packet with header and given payload and sends
 *      BLE notification to host.
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void GaiaSendPacket( device_handle_id device_id,
                            uint16 command_id,
                            uint16 size_payload,
                            uint8 *payload)
{
    uint8 data[GAIA_GATT_MAX_PACKET_SIZE];
    uint8 *p = data;
    uint16 size = size_payload;

    /* Packet Structure
         0 bytes  1        2        3        4          5    len+4
         +--------+--------+--------+--------+ +--------+--------+
         |    VENDOR ID    |   COMMAND ID    | | PAYLOAD   ...   |
         +--------+--------+--------+--------+ +--------+--------+
     */

    *p++ = HIGH(GAIA_VENDOR_CSR);
    *p++ = LOW(GAIA_VENDOR_CSR);
    *p++ = HIGH(command_id);
    *p++ = LOW(command_id);

    /*  Copy in the payload  */
    while (size--)
    {
        *p++ = *payload++;
    }

    GAIASendNotification(   device_id,
                            GAIA_GATT_PACKET_HEADER_SIZE + size_payload,
                            data);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GaiaRegisterCallback
 *
 *  DESCRIPTION
 *      Set the callback so the GAIA library can inform the application
 *      of the progress and warn about special events.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GaiaRegisterCallback(gaia_event_handler callback)
{
    g_gaia_lib_data.callback = callback;
}