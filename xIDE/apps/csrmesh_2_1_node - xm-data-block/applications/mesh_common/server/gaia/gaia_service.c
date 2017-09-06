/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file service_gaia.c
 *  \brief This file is the Application/Public API for the GAIA service
 *
 * This file provides:
 * + entry point for Connection Manager messages for the GAIA GATT descriptors
 * + support for reliably sending Notifications to the Host/Client
 *
 */

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>
#include <gatt_prim.h>
#include <mem.h>
#include <buf_utils.h>

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_types.h"
#include "cm_api.h"
#include "app_gatt_db.h"
#include "gaia_service.h"
#include "nvm_access.h"
#include "gaia.h"
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

/* Macros for NVM access */
#define NVM_GAIA_CLIENT_CONFIG_OFFSET                       0
#define GAIA_SERVICE_NVM_MEMORY_WORDS                       1

#define GAIA_NOTIFICATION_ENABLED (g_gaia_data.gaia_client_config == gatt_client_config_notification)

#define DEFAULT_ATT_MTU                                     23  /* put this here for now */

/*=============================================================================*
 *  Private Data Types
 *============================================================================*/

typedef struct
{
    /* Reports Client Configuration */
    gatt_client_config  gaia_client_config;

    /* holds the last sent message in case it needs to be sent again */
    struct
    {
        device_handle_id device_id;
        uint16 packet_length;
        uint8 packet[DEFAULT_ATT_MTU];
    } GAIASendNotification_args;

    bool waiting_for_fw_buffer;

    /* NVM offset at which GAIA data is stored */
    uint16 nvm_offset;
} GAIA_SERVICE_DATA_T;

#ifndef NVM_DONT_PAD
#define GAIA_SERVICE_VERSION (0x0001)
#define GAIA_SERVICE_PADDED_LEN (NVM_PAD_ROUND_UP_TO_32(GAIA_SERVICE_NVM_MEMORY_WORDS))
#endif

/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function is used to read GAIA service specific data store in NVM */
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset);

/* This function handles Read operation on GAIA service attributes */
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data);

/* This function handles Write operation on GAIA service attributes */
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data);

/* This function handles the events from the connection manager */
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data);

/*=============================================================================*
 *  Private Data
 *============================================================================*/

static CM_HANDLERS_T g_gaia_server_handlers =
{
    .pCallback = &handleConnMgrProcedureEvent
};

static CM_SERVER_INFO_T g_gaia_service_info;

/* Service data */
static GAIA_SERVICE_DATA_T g_gaia_data;

/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/
/*-----------------------------------------------------------------------------
 *  NAME
 *      readDataFromNVM
 *
 *  DESCRIPTION
 *      This function is used to read GAIA service specific data store in NVM
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset )
{
#ifndef NVM_DONT_PAD
    uint16 next_nvm_offset = *nvm_offset + GAIA_SERVICE_PADDED_LEN;
    uint16 temp_nvm_offset = *nvm_offset;

    /* If the service cannot read or repair the data in NVM
       we treat it as a fresh start */
    if (!nvm_start_fresh && !Nvm_ValidateVersionedHeader(nvm_offset,
                                 GAIA_SERVICE_ID,
                                 GAIA_SERVICE_PADDED_LEN,
                                 GAIA_SERVICE_VERSION,
                                 NULL))
    {
        nvm_start_fresh = TRUE;
    }
    if (nvm_start_fresh)
    {
        *nvm_offset = temp_nvm_offset;
        Nvm_WriteVersionedHeader(nvm_offset,
                               GAIA_SERVICE_ID,
                               GAIA_SERVICE_PADDED_LEN,
                               GAIA_SERVICE_VERSION);
    }
#endif
    /* Save the NVM offset */
    g_gaia_data.nvm_offset = *nvm_offset;

    if(!nvm_start_fresh)
    {
        /* Read GAIA Client Configuration */
        Nvm_Read((uint16*)&g_gaia_data.gaia_client_config,
                 sizeof(g_gaia_data.gaia_client_config),
                 g_gaia_data.nvm_offset + NVM_GAIA_CLIENT_CONFIG_OFFSET);
    }
    else
    {
        g_gaia_data.gaia_client_config = gatt_client_config_none;
        Nvm_Write((uint16*)&g_gaia_data.gaia_client_config,
                   sizeof(g_gaia_data.gaia_client_config),
                   (g_gaia_data.nvm_offset + NVM_GAIA_CLIENT_CONFIG_OFFSET));
    }

    /* Increment the offset by the number of words of NVM memory required
     * by GAIA service
     */
    *nvm_offset += GAIA_SERVICE_NVM_MEMORY_WORDS;
#ifndef NVM_DONT_PAD
#ifdef DEBUG_BUILD
    /* Error if we try to write more data than we have padded space for */
    ASSERT(*nvm_offset < next_nvm_offset);
#endif
    *nvm_offset = next_nvm_offset;
#endif
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      handleAccessRead
 *
 *  DESCRIPTION
 *      This function handles Read operation on GAIA service attributes
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data)
{
    CM_ACCESS_RESPONSE_T cm_access_rsp;
    uint16 length = 0;
    uint8  *p_value = NULL;
    sys_status rc = sys_status_success;

    switch(p_event_data->handle)
    {
        case HANDLE_GAIA_RESPONSE_ENDPOINT:
            /*
             * polling is not supported as this library is only for devices which
             * support notifications
             */
            break;

        case HANDLE_GAIA_DATA_ENDPOINT:
            /* For now, this handle does not return any value */
            break;

        default:
            /* Let firmware handle the request */
            rc = gatt_status_irq_proceed;
            break;
    }

    cm_access_rsp.device_id = p_event_data->device_id;
    cm_access_rsp.handle = p_event_data->handle;
    cm_access_rsp.rc = rc;
    cm_access_rsp.size_value = length;
    cm_access_rsp.value = p_value;

    CMSendAccessRsp(&cm_access_rsp);
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      handleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles Write operation on GAIA service attributes
 *
 *  RETURNS:
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data)
{
    CM_ACCESS_RESPONSE_T cm_access_rsp;
    uint8 *p_value = p_event_data->data;
    sys_status rc = sys_status_success;
    uint16 client_config;

    switch(p_event_data->handle)
    {
        case HANDLE_GAIA_COMMAND_ENDPOINT:
            /* nothing to do here */
            break;

        case HANDLE_GAIA_DATA_ENDPOINT:
            /* nothing to do here */
            break;

        case HANDLE_GAIA_RESPONSE_CLIENT_CONFIG:
            /* The GAIA protocol follows big-endian format whereas in
             * uEnergy applications, little-endian format is followed for
             * client_config words.
             */
            client_config = BufReadUint16(&p_value);

            if( (client_config == gatt_client_config_notification) ||
                (client_config == gatt_client_config_none))
            {
                g_gaia_data.gaia_client_config = client_config;
                Nvm_Write(  (uint16*)&g_gaia_data.gaia_client_config,
                            sizeof(g_gaia_data.gaia_client_config),
                            (g_gaia_data.nvm_offset + NVM_GAIA_CLIENT_CONFIG_OFFSET));
            }
            else
            {
                rc = gatt_status_desc_improper_config;
            }
            break;

        default:
            /* Other characteristics in GAIA don't support the "write"
             * property */
            rc = gatt_status_write_not_permitted;
            break;
    }

    /* Send ACCESS RESPONSE */
    cm_access_rsp.device_id = p_event_data->device_id;
    cm_access_rsp.handle = p_event_data->handle;
    cm_access_rsp.rc = rc;
    cm_access_rsp.size_value = 0;
    cm_access_rsp.value = NULL;

    CMSendAccessRsp(&cm_access_rsp);

    if(p_event_data->handle == HANDLE_GAIA_COMMAND_ENDPOINT)
    {
        GAIAProcessCommand(p_event_data->device_id, p_event_data->length, p_event_data->data);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnNotify
 *
 *  DESCRIPTION
 *      This function handles the connection notifications for each sub part
 *      of GAIA.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleConnNotify(CM_CONNECTION_NOTIFY_T *cm_event_data)
{
    GaiaLibHandleConnNotify(cm_event_data);

#ifdef GAIA_OTAU_SUPPORT
    GaiaOtauHandleConnNotify(cm_event_data);
#endif
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleBondNotify
 *
 *  DESCRIPTION
 *      This function handles the bonding notification
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleBondNotify(CM_BONDING_NOTIFY_T *cm_event_data)
{
    if(cm_event_data->result != cm_bond_res_success)
    {
        g_gaia_data.gaia_client_config = gatt_client_config_none;

        Nvm_Write(  (uint16*)&g_gaia_data.gaia_client_config,
                    sizeof(g_gaia_data.gaia_client_config),
                    (g_gaia_data.nvm_offset + NVM_GAIA_CLIENT_CONFIG_OFFSET));
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalNumberOfCompletedPackets
 *
 *  DESCRIPTION
 *      Handles the CM_NUMBER_OF_COMPLETED_PKTS_IND message
 *      Used to resend Notifications if they fail to send
 *
 *  RETURNS
 *      none
 *
 *----------------------------------------------------------------------------*/
static void handleSignalNumberOfCompletedPackets(void)
{
    if (g_gaia_data.waiting_for_fw_buffer)
    {
        g_gaia_data.waiting_for_fw_buffer = FALSE;
        GAIASendNotification(   g_gaia_data.GAIASendNotification_args.device_id,
                                g_gaia_data.GAIASendNotification_args.packet_length,
                                g_gaia_data.GAIASendNotification_args.packet );
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnMgrProcedureEvent
 *
 *  DESCRIPTION
 *       This function handles the events from the connection manager.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleConnMgrProcedureEvent (cm_event event_type,
        CM_EVENT_T *p_event_data)
{
    switch(event_type)
    {
        case CM_READ_ACCESS:
            handleAccessRead((CM_READ_ACCESS_T *)p_event_data);
            break;

        case CM_WRITE_ACCESS:
            handleAccessWrite((CM_WRITE_ACCESS_T *)p_event_data);
            break;

        case CM_CONNECTION_NOTIFY:
            handleConnNotify((CM_CONNECTION_NOTIFY_T *)p_event_data);
            break;

        case CM_BONDING_NOTIFY:
            handleBondNotify((CM_BONDING_NOTIFY_T *)p_event_data);
            break;

        case CM_NUMBER_OF_COMPLETED_PKTS_IND:
            handleSignalNumberOfCompletedPackets();
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
 *      GAIAInitServerService
 *
 *  DESCRIPTION
 *      This function initialises the GAIA service.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void GAIAInitServerService(bool nvm_start_fresh, uint16 *nvm_offset)
{
    /* base GAIA library support */
    GaiaLibInit();

#ifdef GAIA_SET_GET_HOST_FEATURE_INFORMATION_SUPPORT
        /* GAIA support for the Get/Set Host Feature Information message type */
        GaiaMessageDataInit();
#endif

#ifdef GAIA_OTAU_SUPPORT
        /* GAIA support for the Over The Air Upgrade support */
        GaiaOtauInit(nvm_start_fresh, nvm_offset);
#endif

    readDataFromNVM(nvm_start_fresh, nvm_offset);

    g_gaia_service_info.server_handler = g_gaia_server_handlers;
    g_gaia_service_info.start_handle = HANDLE_GAIA_SERVICE;
    g_gaia_service_info.end_handle = HANDLE_GAIA_SERVICE_END;

    /* Register Server service */
    CMServerInitRegisterHandler(&g_gaia_service_info);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GAIASendNotification
 *
 *  DESCRIPTION
 *      Sends notification from GAIA service.
 *
 *  RETURNS
 *      cm_status_code
 *
 *----------------------------------------------------------------------------*/
extern cm_status_code GAIASendNotification( device_handle_id device_id,
                                            uint16 packet_length,
                                            uint8 *packet)
{
    CM_VALUE_NOTIFICATION_T notification;
    cm_status_code retval;

    /* Check if the GAIA state is not suspended */
    if(!GAIA_NOTIFICATION_ENABLED)
    {
        return cm_status_success;
    }

    /* Create notification */
    notification.device_id = device_id;
    notification.handle = HANDLE_GAIA_RESPONSE_ENDPOINT;
    notification.size_value = packet_length;
    notification.value = packet;

    /* Send the notification and check for send error. */
    retval = CMSendValueNotificationExt(&notification);

    if ( retval != sys_status_success )
    {
        /* make a copy of the arguments and resend this message later */
        g_gaia_data.GAIASendNotification_args.device_id = device_id;
        g_gaia_data.GAIASendNotification_args.packet_length = packet_length;
        MemCopy( g_gaia_data.GAIASendNotification_args.packet, packet, packet_length );
        g_gaia_data.waiting_for_fw_buffer = TRUE;
        CMSignalNumberOfCompletedPackets();
    }
    return retval;
}

