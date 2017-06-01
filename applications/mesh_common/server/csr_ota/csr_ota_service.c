/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      csr_ota_service.c
 *
 *  DESCRIPTION
 *      This file defines routines for using ota service
 *
 ****************************************************************************/
#include "user_config.h"

#if defined(CSR101x_A05) && defined(OTAU_BOOTLOADER)

/*============================================================================*
 *  SDK Header Files
 *===========================================================================*/
#include <gatt.h>
#include <gatt_uuid.h>
#include <buf_utils.h>
#include <nvm.h>
#include <ble_hci_test.h>
#include <csr_ota.h>
#include <memory.h>
#include <gap_app_if.h>
#include <mem.h>

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

#include "cm_types.h"
#include "cm_api.h"
#include "csr_ota_service.h"
#include "gatt_service.h"
#include "app_gatt_db.h"
#include "main_app.h"

#ifdef ENABLE_MESH_OTA
#include "connection_handler.h"
#include "advertisement_handler.h"
#endif
/*============================================================================*
 *   Private Function Prototypes
 *===========================================================================*/

/* Read a section of the CS block */
static sys_status readCsBlock(uint16 offset, uint8 length, uint8 *value);

/* This function handles read operation */
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data);

/* This function handles write operation */
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data);

/* This function handles the events from the connection manager */
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data);

/*============================================================================*
 *  Private Data
 *===========================================================================*/

/* Service handler */
static CM_HANDLERS_T g_server_handler =
{
    .pCallback = &handleConnMgrProcedureEvent
};

/* Service Information */
static CM_SERVER_INFO_T g_service_info;

/* The current value of the DATA TRANSFER characteristic */
static uint8 data_transfer_memory[MAX_DATA_LENGTH] = {0};

/* The number of bytes of valid data in data_transfer_memory */
static uint8 data_transfer_data_length = 0;

/* The current configuration of the DATA TRANSFER characteristic */
static uint8 data_transfer_configuration[2] = {gatt_client_config_none, 0};

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      readCsBlock
 *
 *  DESCRIPTION
 *      Read a section of the CS block.
 *
 *      This function is called when the Host requests the contents of a CS
 *      block, by writing to the OTA_READ_CS_BLOCK handle. This function is
 *      supported only if the application supports the OTA_READ_CS_BLOCK
 *      characteristic.
 *
 *  PARAMETERS
 *      offset [in]             Offset into the CS block to read from, in words.
 *                              This is the value written by the Host to the
 *                              Characteristic.
 *      length [in]             Number of octets to read.
 *      value [out]             Buffer to contain block contents
 *
 *  RETURNS
 *      sys_status_success: The read was successful and "value" contains valid
 *                          information.
 *      CSR_OTA_KEY_NOT_READ: The read was unsuccessful and "value" does not
 *                          contain valid information.
 *
 *----------------------------------------------------------------------------*/
static sys_status readCsBlock(uint16 offset, uint8 length, uint8 *value)
{
    /* Check the length is within the packet size and that the read does not
     * overflow the CS block.
     */
    if ((length > MAX_DATA_LENGTH) ||
        (offset + BYTES_TO_WORDS(length) > CSTORE_SIZE))
        return CSR_OTA_KEY_NOT_READ;

    MemCopyUnPack(value, (uint16 *)(DATA_CSTORE_START + offset), length);

    return sys_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleAccessRead
 *
 *  DESCRIPTION
 *      This function handles read operation
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleAccessRead(CM_READ_ACCESS_T *p_event_data)
{
    CM_ACCESS_RESPONSE_T cm_access_rsp;
    csr_application_id current_app;
    uint8 *p_value = NULL;
    uint8 data_length = 0;
    sys_status rc = sys_status_success;

    switch(p_event_data->handle)
    {
        case HANDLE_CSR_OTA_CURRENT_APP:
        {
            /* Read the index of the current application */
            current_app = OtaReadCurrentApp();

            p_value = (uint8*)&current_app;
            data_length = 1;
        }
        break;

        case HANDLE_CSR_OTA_DATA_TRANSFER:
        {
            /* Read the value of the data transfer characteristic */
            p_value = (uint8*)data_transfer_memory;
            data_length = data_transfer_data_length;
        }
        break;

        case HANDLE_CSR_OTA_DATA_TRANSFER_CLIENT_CONFIG:
        {
            /* Read the value of the Data Transfer Client Characteristic
             * Configuration Descriptor
             */
            p_value = (uint8 *)data_transfer_configuration;
            data_length = 2;
        }
        break;

        case HANDLE_CSR_OTA_VERSION:
        {
            /* Let the firmware handle reads on this service.
             * Send response indication
             */
            rc = gatt_status_irq_proceed;
        }
        break;

        default:
            /* No more IRQ characteristics */
            rc = gatt_status_read_not_permitted;
        break;

    }

    cm_access_rsp.device_id     = p_event_data->device_id;
    cm_access_rsp.handle        = p_event_data->handle;
    cm_access_rsp.rc            = rc;
    cm_access_rsp.size_value    = data_length;
    cm_access_rsp.value         = p_value;

    /* Send the access response */
    CMSendAccessRsp(&cm_access_rsp);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      handleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles write operation
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleAccessWrite(CM_WRITE_ACCESS_T *p_event_data)
{
    CM_ACCESS_RESPONSE_T cm_access_rsp;
    uint16 client_config;
    BD_ADDR_T *p_bd_addr;
    TYPED_BD_ADDR_T con_bd_addr;
    sys_status rc = sys_status_success;
#if defined(USE_STATIC_RANDOM_ADDRESS) || defined(USE_RESOLVABLE_RANDOM_ADDRESS)
    BD_ADDR_T bd_addr;
    p_bd_addr = &bd_addr;
    #ifdef ENABLE_MESH_OTA
        GattGetRandomAddress(p_bd_addr); 
    #else   
        (void)GapGetRandomAddress(p_bd_addr);
    #endif
#else
   p_bd_addr = NULL;
#endif /* USE_STATIC_RANDOM_ADDRESS || USE_RESOLVABLE_RANDOM_ADDRESS */

    switch(p_event_data->handle)
    {
        /* When the OTAU Host writes to this characteristic, it causes the
         * device to reboot to the OTAU boot-loader or, (if supported), the
         * specified application
         */
        case HANDLE_CSR_OTA_CURRENT_APP:
        {
            /* Set the index of the current application */
            const uint8 app_id = p_event_data->data[0]; /* New app index */

            /* Get the connected device address */
            CMGetBdAdressFromDeviceId(p_event_data->device_id, &con_bd_addr);
            
#ifdef ENABLE_MESH_OTA
            rc = OtaWriteCurrentApp(app_id,
                                  GetDevicePairingStatus(),
                                  &con_bd_addr,
                                  0,
                                  p_bd_addr,NULL,
                                  GattServiceChangedIndActive());
#else
            bond_handle_id bond_id;
            bool bonded = FALSE;
            CM_BONDED_DEVICE_T bond_dev;

            /* Get the bond id from the device id */
            bond_id = CMGetBondIdFromDeviceId(p_event_data->device_id);
            if(bond_id != CM_INVALID_BOND_ID)
            {
                bonded = TRUE;

                /* Get the Bonded Device Info */
                CMGetBondedDeviceFromBondId(bond_id, &bond_dev);
            }

            rc = OtaWriteCurrentApp(app_id,
                                    bonded,
                                    &con_bd_addr,
                                    bond_dev.div,
                                    p_bd_addr,bond_dev.irk,
                                    GattServiceChangedIndActive());
#endif
            if (rc != sys_status_success)
            {
                /* Sanitise the result. If OtaWriteCurrentApp fails it will
                 * be because one or more of the supplied parameters was
                 * invalid
                 */
                rc = gatt_status_invalid_param_value;
            }
        }
        break;

        /* The OTAU Host uses this characteristic to request a specific
         * configuration key from the application. The value of the
         * configuration key is returned through a Data Transfer characteristic
         */
        case HANDLE_CSR_OTA_READ_CS_BLOCK:
        {
            /* Set the offset and length of a block of CS to read and
             * validate input(expecting uint16[2])
             */
            if (p_event_data->length == WORDS_TO_BYTES(sizeof(uint16[2])))
            {
                const uint16 offset = BufReadUint16(&p_event_data->data);

                data_transfer_data_length =
                        (uint8)BufReadUint16(&p_event_data->data);

                rc = readCsBlock(offset,
                                 data_transfer_data_length,
                                 data_transfer_memory);
            }
            else
            {
                rc = gatt_status_invalid_length;
            }
        }
        break;

        /* OTAU Host subscribes for notifications on this characteristic
         * before requesting the configuration keys from the application.
         */
        case HANDLE_CSR_OTA_DATA_TRANSFER_CLIENT_CONFIG:
        {
            /* Modify the Data Transfer Client Characteristic Configuration
             * Descriptor
             */
            client_config = BufReadUint16(&(p_event_data->data));

            if((client_config == gatt_client_config_notification) ||
               (client_config == gatt_client_config_none))
            {
                data_transfer_configuration[0] = client_config;
                rc = sys_status_success;
            }
            else
            {
                /* Return error as only notifications are supported */
                rc = gatt_status_desc_improper_config;
            }
        }
        break;

        default:
            rc = gatt_status_write_not_permitted;
        break;
    }

    cm_access_rsp.device_id         = p_event_data->device_id;
    cm_access_rsp.handle            = p_event_data->handle;
    cm_access_rsp.rc                = rc;
    cm_access_rsp.size_value        = 0;
    cm_access_rsp.value             = NULL;

    /* Send the access response */
    CMSendAccessRsp(&cm_access_rsp);

    /* Perform now any follow-up actions */
    if (rc == sys_status_success)
    {
        switch(p_event_data->handle)
        {
            case HANDLE_CSR_OTA_READ_CS_BLOCK:
            {
                /* If this write action was to trigger a CS block read, and
                 * the notifications have been enabled, send the result now.
                 */
                if(data_transfer_configuration[0] ==
                   gatt_client_config_notification)
                {
                    CM_VALUE_NOTIFICATION_T cm_value_notify;

                    cm_value_notify.device_id = p_event_data->device_id;
                    cm_value_notify.handle = HANDLE_CSR_OTA_DATA_TRANSFER;
                    cm_value_notify.size_value = data_transfer_data_length;
                    cm_value_notify.value = data_transfer_memory;

                    /*  Send a notification */
                    CMSendValueNotification(&cm_value_notify);
                }
            }
            break;

            case HANDLE_CSR_OTA_CURRENT_APP:
            {
                /* Request Application Reset */
                AppOtaReset();
            }
            break;

            default:
                /* No follow up action necessary */
            break;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnMgrProcedureEvent
 *
 *  DESCRIPTION
 *       This function handles the events from the connection manager
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
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

        default:
        break;

    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      OtaInitServerService
 *
 *  DESCRIPTION
 *      This function initialises the OTA service
 *
 *  RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/
extern void OtaInitServerService(bool nvm_start_fresh, uint16 *nvm_offset)
{
    /* Assign the service handler */
    g_service_info.server_handler = g_server_handler;
    g_service_info.start_handle = HANDLE_CSR_OTA_SERVICE;
    g_service_info.end_handle = HANDLE_CSR_OTA_SERVICE_END;

    /* Register the service with the CM */
    CMServerInitRegisterHandler(&g_service_info);
}
#endif /* CSR101x_A05 */