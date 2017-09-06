/******************************************************************************
 *  Copyright 2012 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gap_service.c
 *
 *  DESCRIPTION
 *      This file defines routines for using GAP service
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <gatt.h>
#include <gatt_prim.h>
#include <mem.h>
#include <buf_utils.h>
#include <gatt_uuid.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "cm_types.h"
#include "cm_appearance.h"
#include "gap_service.h"
#include "app_gatt_db.h"
#include "nvm_access.h"
#include "user_config.h"

/*============================================================================*
 *  Private Data Types
 *============================================================================*/

/* Service data type */
typedef struct
{
    /* Name length in Bytes */
    uint16  length;

    /* NVM offset at which GAP service data is stored */
    uint16  nvm_offset;

} SERVICE_DATA_T;

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* The offset of data being stored in NVM for GAP service. This offset is 
 * added to GAP service offset to NVM region (see g_service_data.nvm_offset) 
 * to get the absolute offset at which this data is stored in NVM
 */
#define GAP_NVM_DEVICE_LENGTH_OFFSET        (0)

#define GAP_NVM_DEVICE_NAME_OFFSET          (1)

/* Number of words of NVM memory used by GAP service */
#define GAP_SERVICE_NVM_MEMORY_WORDS        (DEVICE_NAME_LENGTH + 1)

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function is used to initialise GAP service data */
static void serviceDataInit(void);

/* This function updates the device name and length in gap service */
static void updateDeviceName(uint16 length, uint8 *name);

/* This function is used to read data from the NVM */
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset);

/* This function is used to write Device Name Length and Device Name to NVM */
static void writeDeviceNameToNvm(void);

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
 *============================================================================*/

/* Service data */
static SERVICE_DATA_T g_service_data;

/* Service handler */
static CM_HANDLERS_T g_service_handler = 
{
    .pCallback = &handleConnMgrProcedureEvent
};

/* Service Information */
static CM_SERVER_INFO_T g_service_info;

/* Device name - Extra two octets for storing AD Type and a Null ('\0') */ 
static uint8 g_device_name[DEVICE_NAME_LENGTH + 2];

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME 
 *      serviceDataInit
 *
 *  DESCRIPTION
 *      This function is used to initialise service data
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void serviceDataInit(void)
{
    uint8 *p_name = (g_device_name + 1);
    g_service_data.length = StrLen(DEVICE_NAME);

    g_device_name[0] = AD_TYPE_LOCAL_NAME_COMPLETE;
    MemCopy(p_name, DEVICE_NAME, g_service_data.length);
    p_name[g_service_data.length] = '\0';
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      updateDeviceName
 *
 *  DESCRIPTION
 *      This function updates the device name and length
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void updateDeviceName(uint16 length, uint8 *name)
{
    uint8 *p_name = (g_device_name + 1);

    /* Update Device Name length to the maximum of DEVICE_NAME_MAX_LENGTH  */
    if(length < DEVICE_NAME_LENGTH)
        g_service_data.length = length;
    else
        g_service_data.length = DEVICE_NAME_LENGTH;

    MemCopy(p_name, name, g_service_data.length);

    /* Null terminate the device name string */
    p_name[g_service_data.length] = '\0';

    /* Write the Name to the NVM */
    writeDeviceNameToNvm();
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      readDataFromNVM
 *
 *  DESCRIPTION
 *      This function is used to read data from the NVM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void readDataFromNVM(bool nvm_start_fresh, uint16 *nvm_offset)
{
    g_service_data.nvm_offset = *nvm_offset;
    
    if(!nvm_start_fresh)
    {
        /* Read Device Length */
        Nvm_Read(&g_service_data.length, sizeof(g_service_data.length), 
                 g_service_data.nvm_offset + 
                 GAP_NVM_DEVICE_LENGTH_OFFSET);

        /* Read Device Name 
         * Typecast of uint8 to uint16 or vice-versa shall not have any side 
         * affects as both types (uint8 and uint16) take one word memory on XAP
         */
        Nvm_Read((uint16*)(g_device_name+1), g_service_data.length, 
                 g_service_data.nvm_offset + 
                 GAP_NVM_DEVICE_NAME_OFFSET);

        /* Add NULL character to terminate the device name string */
        g_device_name[1 + g_service_data.length] = '\0';
    }
    else
    {
        /* Write the Name to the NVM */
        writeDeviceNameToNvm();
    }

    /* Increase NVM offset for maximum device name length */
    *nvm_offset += GAP_SERVICE_NVM_MEMORY_WORDS;

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeDeviceNameToNvm
 *
 *  DESCRIPTION
 *      This function is used to write GAP Device Name Length and Device Name 
 *      to NVM 
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void writeDeviceNameToNvm(void)
{
    /* Write device name length to NVM */
    Nvm_Write(&g_service_data.length, sizeof(g_service_data.length), 
              g_service_data.nvm_offset + 
              GAP_NVM_DEVICE_LENGTH_OFFSET);

    /* Write device name to NVM 
     * Typecast of uint8 to uint16 or vice-versa shall not have any side 
     * affects as both types (uint8 and uint16) take one word memory on XAP
     */
    Nvm_Write((uint16*)(g_device_name+1), g_service_data.length, 
              g_service_data.nvm_offset + 
              GAP_NVM_DEVICE_NAME_OFFSET); 

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
    uint16 length = 0;
    uint8  *p_value = NULL;
    sys_status rc = sys_status_success;

    switch(p_event_data->handle)
    {
        case HANDLE_DEVICE_NAME:
        {
            /* Validate offset against length, it should be less than 
             * device name length
             */
            if(p_event_data->offset < g_service_data.length)
            {
                length = g_service_data.length - p_event_data->offset;
                p_value = ((g_device_name + 1) + p_event_data->offset);
            }
            else
            {
                rc = gatt_status_invalid_offset;
            }
        }
        break;

        default:
            /* Let firmware handle the read request. */
            rc = gatt_status_irq_proceed;
        break;
    }

    cm_access_rsp.device_id         = p_event_data->device_id;
    cm_access_rsp.handle            = p_event_data->handle;
    cm_access_rsp.rc                = rc;
    cm_access_rsp.size_value        = length;
    cm_access_rsp.value             = p_value;

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
    sys_status rc = sys_status_success;

    switch(p_event_data->handle)
    {        
        case HANDLE_DEVICE_NAME:
            /* Update device name */
            updateDeviceName(p_event_data->length,p_event_data->data);
        break;

        default:
            /* No more IRQ characteristics */
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
static void handleConnMgrProcedureEvent (
                                     cm_event event_type,
                                     CM_EVENT_T *p_event_data)
{
    switch(event_type)
    {
        case CM_READ_ACCESS:
            handleAccessRead((CM_READ_ACCESS_T *) p_event_data);
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
 *      GapGetNameAndLength
 *
 *  DESCRIPTION
 *      This function is used to get the reference to the 'g_device_name' array, 
 *      which contains AD Type and device name. This function also returns the 
 *      AD Type and device name length.
 *
 *  RETURNS
 *      Pointer to device name array.
 *
 *---------------------------------------------------------------------------*/
extern uint8 *GapGetNameAndLength(uint16 *p_name_length)
{
    /* Device Name and Length include the AD Type field */
    *p_name_length = StrLen((char*)g_device_name);

    return (g_device_name);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GapInitServerService
 *
 *  DESCRIPTION
 *      This function initialises the GAP service
 *
 *  RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/
extern void GapInitServerService(bool nvm_start_fresh, uint16 *nvm_offset)
{
    /* Initialise the service data */
    serviceDataInit();
    
    /* Read the data from the NVM */
    readDataFromNVM(nvm_start_fresh, nvm_offset);

    /* Assign the service handler */
    g_service_info.server_handler = g_service_handler;
    g_service_info.start_handle = HANDLE_GAP_SERVICE;
    g_service_info.end_handle = HANDLE_GAP_SERVICE_END;

    /* Register the service with the CM */
    CMServerInitRegisterHandler(&g_service_info);
}
