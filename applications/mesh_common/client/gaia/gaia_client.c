/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 * FILE
 *    gaia_client.c
 *
 *  DESCRIPTION
 *    This file implements the GAIA protocol for a client connection
 *
 ******************************************************************************/

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <mem.h>
#include <battery.h>

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "gaia_otau_client_private.h"
#include "gaia_client_service.h"
#include "gaia_client.h"
#include "gaia.h"
#include "byte_utils.h"

/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------
 * NAME
 *     gaiaHandleNotificationCommand
 *
 * DESCRIPTION
 *     This function handles notification command
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

static void gaiaHandleNotificationCommand(uint16 command_id,
                                          uint16 payload_length, uint8 *payload)
{
    uint8 event_code;
    
    event_code = *(uint8 *)(payload);
    (uint8 *)(payload)++;
    
    switch (command_id)
    {
        case GAIA_EVENT_NOTIFICATION:
        {          
            if(event_code == GAIA_EVENT_VMUP_PACKET)
            {
                    HandleResponseNotification(payload_length-1,payload);
            }
            else
            {
                SetAcknowledgement(GAIA_STATUS_NOT_SUPPORTED);
            }
        }
        break;
        default:
        {
            SetAcknowledgement(GAIA_STATUS_NOT_SUPPORTED);
        }
        break;

    }
}

/*-----------------------------------------------------------------------------
 * NAME
 *      gaiaHandleDataTransferCommandAck
 *
 * DESCRIPTION
 *      Handle a Data Transfer command Acknowledgement
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

static void gaiaHandleDataTransferCommandAck(uint16 command_id,
                                          uint16 payload_length,
                                          uint8 *payload)
{
     switch (command_id)
     {
		case (GAIA_ACK_MASK | GAIA_COMMAND_VM_UPGRADE_CONNECT):
        {
            HandleUpgradeConnectAck(payload_length, payload);
    	}
		break;
		case (GAIA_ACK_MASK | GAIA_COMMAND_VM_UPGRADE_DISCONNECT):
		{
			HandleUpgradeDisconnectAck(payload_length, payload);
	    }
		break;
		case (GAIA_ACK_MASK | GAIA_COMMAND_VM_UPGRADE_CONTROL):
		{
            HandleUpgradeControlRequestAck(payload_length, payload);  
	    }
		break;
    }
}
/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      GAIAClientProcessCommand
 *
 *  DESCRIPTION
 *      Process a GAIA command
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void GAIAClientProcessCommand(uint16 payload_len, uint8 *payload)
{
    uint16 command_id = 0;

    if (payload_len >= 2)
    {
        /* First two bytes contain vendor id in big-endian format */
        if (BufReadUint16BE(&payload) != GAIA_VENDOR_CSR)
            return;
        payload_len -= 2;
    }

    if (payload_len >= 2)
    {
        /* next two bytes contain commandId in big-endian format */
        command_id = BufReadUint16BE(&payload);
        payload_len -= 2;
    }

    if(!(HIGH(command_id) & GAIA_ACK_MASK_H))
    {
        switch (command_id & GAIA_COMMAND_TYPE_MASK)
        {
            case GAIA_COMMAND_TYPE_NOTIFICATION:
            gaiaHandleNotificationCommand(command_id, payload_len, payload);
            break;

            default:
            {
                SetAcknowledgement(GAIA_STATUS_NOT_SUPPORTED);
            }
            break;
        }
    }
    else
    {
        switch (command_id & GAIA_COMMAND_TYPE_MASK)
        {
            
            case GAIA_COMMAND_TYPE_DATA_TRANSFER:
            gaiaHandleDataTransferCommandAck(command_id, payload_len, payload);
            break;

            default:
            {
            }
            break;
        }
    }
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GaiaClientSendCommandPacket
 *
 *  DESCRIPTION
 *      Prepares a upgrade control GAIA packet with header and given payload and 
 *      sends BLE notification to host.
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void GaiaClientSendCommandPacket(uint16 size_payload,
        uint8 *payload)
{
    uint8 data[20];
    uint8 *p = data;
    uint16 size = size_payload + 4;

    *p++ = HIGH(GAIA_VENDOR_CSR);
    *p++ = LOW(GAIA_VENDOR_CSR);    
    *p++ = HIGH(GAIA_COMMAND_VM_UPGRADE_CONTROL);
    *p++ = LOW(GAIA_COMMAND_VM_UPGRADE_CONTROL);

    /*  Copy in the payload  */
    while (size_payload--)
        *p++ = *payload++;
    GaiaWriteCommandEndpoint(size,data);
            
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GaiaClientSendDisconnectPacket
 *
 *  DESCRIPTION
 *      Prepares a GAIA upgrade disconnect packet with header and given payload 
 *      and sends BLE notification to host.
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void GaiaClientSendDisconnectPacket(void)
{
    uint8 data[4];
    uint8 *p = data;
    uint16 size = 4;

    *p++ = HIGH(GAIA_VENDOR_CSR);
    *p++ = LOW(GAIA_VENDOR_CSR);    
    *p++ = HIGH(GAIA_COMMAND_VM_UPGRADE_DISCONNECT);
    *p++ = LOW(GAIA_COMMAND_VM_UPGRADE_DISCONNECT);

    GaiaWriteCommandEndpoint(size,data);
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GaiaClientSendConnectPacket
 *
 *  DESCRIPTION
 *      Prepares a GAIA upgrade connect packet with header and given payload and 
 *      sends BLE notification to host.
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void GaiaClientSendConnectPacket(void)
{
    uint8 data[4];
    uint8 *p = data;
    uint16 size = 4;

    *p++ = HIGH(GAIA_VENDOR_CSR);
    *p++ = LOW(GAIA_VENDOR_CSR);    
    *p++ = HIGH(GAIA_COMMAND_VM_UPGRADE_CONNECT);
    *p++ = LOW(GAIA_COMMAND_VM_UPGRADE_CONNECT);

    GaiaWriteCommandEndpoint(size,data);
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      SetAcknowledgement
 *
 *  DESCRIPTION
 *      Sends acknowledgement of received GAIA upgrade control packets
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void SetAcknowledgement(uint8 status)
{
    uint8 data[5];
    uint8 *p = data;
    uint16 size = 5;

    *p++ = HIGH(GAIA_VENDOR_CSR);
    *p++ = LOW(GAIA_VENDOR_CSR);    
    *p++ = HIGH(GAIA_ACK_MASK | GAIA_COMMAND_VM_UPGRADE_CONTROL);
    *p++ = LOW(GAIA_ACK_MASK | GAIA_COMMAND_VM_UPGRADE_CONTROL);
    *p++ = status;

    GaiaWriteCommandEndpoint(size,data);
    
    if(status != GAIA_STATUS_SUCCESS)
        GaiaClientSendDisconnectPacket();
}
#endif