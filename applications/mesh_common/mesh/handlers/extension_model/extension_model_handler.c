/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      extension_model_handler.c
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <mem.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "extension_model_handler.h"
#include "app_mesh_handler.h"
#include "extension_server.h"
#include "extension_client.h"
#include "main_app.h"
#include "nvm_access.h"
#include "app_util.h"
#include "core_mesh_handler.h"

#ifdef ENABLE_EXTENSION_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
/* Macros for NVM access */
#define NVM_OFFSET_EXTENSION_OPCODE               (0)
#define NVM_OFFSET_EXTENSION_RANGE                (1)

#define MANUF_HASH_STRING_LENGTH                  (6)

#define COPY_OPCODE_2BYTE(buf, opcode) buf[0] = (opcode >> 8) & 0xFF; \
                                       buf[1] = (opcode & 0xFF)

#define WRITE_CsrUint16(p, v, offset, len) p[offset++] = v & 0x00FF; \
                                           p[offset++] = (v >> 8) & 0xFF

typedef union
{
    CSRMESH_EXTENSION_CONFLICT_T                   ext_conflict;
} EXTENSION_MODEL_RSP_DATA_T;

typedef struct
{
    CsrUint16 opcode;
    CsrUint16 range;
    CsrUint8 manuf_hash_string[MANUF_HASH_STRING_LENGTH];
}EXT_MODEL_PROPOSED_OPCODE_T;

/* Application Model Handler Data Structure */
typedef struct
{
    /* extension model opcode data to be stored */
    EXT_MODEL_PROPOSED_OPCODE_T prop_opcode[MAX_EXT_OPCODE_RANGE_SUPPORTED];
}EXTENSION_HANDLER_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Pointer to extension handler data */
static EXTENSION_HANDLER_DATA_T                    ext_hdlr_data;

/* Model Response Common Data */
static EXTENSION_MODEL_RSP_DATA_T                  ext_model_rsp_data;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      verifyConflictingOpcodes
 *
 *  DESCRIPTION
 *      The function checks whether the proposed opcodes conflict with the ones
 *      supported in the device or not.
 *
 *  RETURNS
 *      TRUE if the opcodes conflict otherwise returns FALSE
 *
 *---------------------------------------------------------------------------*/
static bool verifyConflictingOpcodes(CsrUint16 opcode_start,
                                     CsrUint16 opcode_end)
{
    bool is_conflict = FALSE;
    CsrUint16 op_strt_msb = opcode_start & 0xFF00;
    CsrUint16 op_end_msb = opcode_end & 0xFF00;
    CsrUint16 index;

    /* If the proposed opcode received is in the range of model opcodes defined
     * in mesh then make the opcodes as conflicting.
     */
    is_conflict = ExtensionVerifyOpcodeConflictWithMesh(opcode_start, opcode_end);

    if(!is_conflict)
    {
        /* Check whether the same opcode might have already assigned onto an 
         * a range already set by the user.
         */
        for(index = 0; index < MAX_EXT_OPCODE_RANGE_SUPPORTED; index++)
        {
            if(((ext_hdlr_data.prop_opcode[index].opcode & 0xFF00)==op_strt_msb)
            || ((ext_hdlr_data.prop_opcode[index].opcode & 0xFF00)==op_end_msb))
            {
                is_conflict = TRUE;
            }
        }
    }
    return is_conflict;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeExtensionDataOnIndex
 *
 *  DESCRIPTION
 *      This function writes action data structure onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void writeExtensionDataOnIndex(uint8 index)
{
    CsrUint16 nvm_ext_index = GET_EXTENSION_NVM_OFFSET(index);

    Nvm_Write((CsrUint16*) (&ext_hdlr_data.prop_opcode[index].opcode),
              sizeof(CsrUint16),
              nvm_ext_index + NVM_OFFSET_EXTENSION_OPCODE);

    Nvm_Write((CsrUint16*) (&ext_hdlr_data.prop_opcode[index].range),
              sizeof(CsrUint16),
              nvm_ext_index + NVM_OFFSET_EXTENSION_RANGE);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      extendedMsgHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Extension opcode messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void extendedMsgHandler(CSRMESH_MODEL_EVENT_T event_code,
                               CSRMESH_EVENT_DATA_T* data,
                               CsrUint16 length,
                               void **state_data)
{
    /* Handling the proposed opcode range of first manufacturer hash string */
    if(ext_hdlr_data.prop_opcode[0].opcode != 0 && 
       ext_hdlr_data.prop_opcode[0].range != 0)
    {
        /* User Defined opcode is received, hence process the data received */
        if(event_code == ext_hdlr_data.prop_opcode[0].opcode)
        {
            /* For this example we form a packet and send the firmware version
             * back to the sender
             */
            uint8 cmd[11], offset;

            /* Form the command packet */
            COPY_OPCODE_2BYTE(cmd, (ext_hdlr_data.prop_opcode[0].opcode + 1));
            offset = 2;

            WRITE_CsrUint16(cmd, APP_MAJOR_VERSION, offset, 2);
            WRITE_CsrUint16(cmd, APP_MINOR_VERSION, offset, 2);

            ExtensionSendMessage(DEFAULT_NW_ID, data->src_id,
                                 AppGetCurrentTTL(), cmd, offset);
        }
    }

    /* Handling the proposed opcode range of second manufacturer hash string */
    if(ext_hdlr_data.prop_opcode[1].opcode != 0 && 
            ext_hdlr_data.prop_opcode[1].range != 0)
    {
        /* User Defined opcode is received, hence process the data received */
        if(event_code == ext_hdlr_data.prop_opcode[1].opcode)
        {
            /* For this example we form a packet and send the firmware version
             * back to the sender
             */
            uint8 cmd[11], offset;

            /* Form the command packet */
            COPY_OPCODE_2BYTE(cmd, (ext_hdlr_data.prop_opcode[1].opcode + 1));
            offset = 2;

            WRITE_CsrUint16(cmd, APP_MAJOR_VERSION, offset, 2);
            WRITE_CsrUint16(cmd, APP_MINOR_VERSION, offset, 2);

            ExtensionSendMessage(DEFAULT_NW_ID, data->src_id,
                                 AppGetCurrentTTL(), cmd, offset);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      extensionModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Extension Model messages
 *
 *  RETURNS
 *      CSRmeshResult : The status of the message handled.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult extensionModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                                CSRMESH_EVENT_DATA_T* data,
                                                CsrUint16 length,
                                                void **state_data)
{
    MemSet(&ext_model_rsp_data,
           0x0000,
           sizeof(ext_model_rsp_data));

    switch(event_code)
    {
        /* Do not add any other message handling in the below switch case */
        case CSRMESH_EXTENSION_REQUEST:
        {
            CSRMESH_EXTENSION_REQUEST_T *p_event = 
                                    (CSRMESH_EXTENSION_REQUEST_T *)data->data;
            bool is_conflict = FALSE, send_conf_rsp = FALSE;
            CsrUint16 index, opcode_idx = 0xFF;

            /* Check whether the received hash is of any interest */
            for(index = 0; index < MAX_EXT_OPCODE_RANGE_SUPPORTED; index++)
            {
                if(!MemCmp(&ext_hdlr_data.prop_opcode[index]
                                                    .manuf_hash_string,
                       p_event->extensionhash, MANUF_HASH_STRING_LENGTH))
                {
                    opcode_idx = index;
                    break;
                }
            }

            /* Check whether the received opcodes are of conflict and if so 
             * send an extension conflict message onto n/w.
             */
            is_conflict = verifyConflictingOpcodes(p_event->proposedopcode,
                                p_event->proposedopcode + p_event->range);

            /* The hash string has matched, hence this is for our device */
            if(opcode_idx != 0xFF)
            {
                /* If not conflicting check whether there is an already opcode
                 * set for this string and if so send the old proposed opcode
                 * here otherwise add it to the db.
                 */
                if(!is_conflict)
                {
                    if(ext_hdlr_data.prop_opcode[opcode_idx].opcode != 0)
                    {
                        /* Add opcode range and proposed opcode value here */
                        ext_model_rsp_data.ext_conflict.proposedopcode = 
                            ext_hdlr_data.prop_opcode[opcode_idx].opcode ;
                        ext_model_rsp_data.ext_conflict.reason = 0x80;
                        ext_model_rsp_data.ext_conflict.reason |= 
                          ext_hdlr_data.prop_opcode[opcode_idx].range;
                        data->src_id =0;
                        send_conf_rsp = TRUE;
                    }
                    else
                    {
                        /* Its a new valid opcode received, hence add it to the 
                         * proposed opcode database.
                         */
                        ext_hdlr_data.prop_opcode[opcode_idx].opcode = 
                                                        p_event->proposedopcode;
                        ext_hdlr_data.prop_opcode[opcode_idx].range = 
                                                        p_event->range;
                        writeExtensionDataOnIndex(opcode_idx);
                    }
                }
                else
                {

                    if(ext_hdlr_data.prop_opcode[opcode_idx].opcode != 0)
                    {
                        /* Send a conflict only if the proposed opcode or range.
                         * is different.
                         */
                        if((ext_hdlr_data.prop_opcode[opcode_idx].opcode
                            != p_event->proposedopcode) ||
                            (ext_hdlr_data.prop_opcode[opcode_idx].range
                            != p_event->range))
                        {
                            /* Add opcode range and proposed opcode value */
                            ext_model_rsp_data.ext_conflict.proposedopcode = 
                                ext_hdlr_data.prop_opcode[opcode_idx].opcode ;
                            ext_model_rsp_data.ext_conflict.reason = 0x80;
                            ext_model_rsp_data.ext_conflict.reason |= 
                              ext_hdlr_data.prop_opcode[opcode_idx].range;
                            data->src_id =0;
                            send_conf_rsp = TRUE;
                        }
                    }
                    else
                    {
                        /* There is a conflict, hence send this message with 
                         * reason code as conflict
                         */
                        ext_model_rsp_data.ext_conflict.proposedopcode = 
                                                        p_event->proposedopcode;
                        ext_model_rsp_data.ext_conflict.reason = 0x01;
                        data->src_id =0;
                        data->allow_relay = FALSE;
                        send_conf_rsp = TRUE;
                    }
                }
            }
            else
            {
                /* There is a conflict and we do not have the hash string 
                 * supported, hence send the conflict with reason 01.
                 */
                ext_model_rsp_data.ext_conflict.proposedopcode = 
                                                p_event->proposedopcode;
                ext_model_rsp_data.ext_conflict.reason = 0x01;
                data->src_id =0;
                data->allow_relay = FALSE;
                send_conf_rsp = TRUE;
            }

            if (state_data != NULL && send_conf_rsp)
            {
                MemCopy(&ext_model_rsp_data.ext_conflict.extensionhash,
                        p_event->extensionhash,
                        MANUF_HASH_STRING_LENGTH);

                *state_data = (void *)&ext_model_rsp_data.ext_conflict;
            }
        }
        break;

         default:
            /* Handle the extended opcode messages here */
            extendedMsgHandler(event_code, data, length,state_data);
        break;
    }
    
    return CSR_MESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      extensionClientEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Extension Client messages
 *
 *  RETURNS
 *      CSRmeshResult : The status of the message handled.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult extensionClientEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                                CSRMESH_EVENT_DATA_T* data,
                                                CsrUint16 length,
                                                void **state_data)
{
    switch(event_code)
    {
       case CSRMESH_EXTENSION_CONFLICT:
       {
            CSRMESH_EXTENSION_CONFLICT_T *p_event = 
                                    (CSRMESH_EXTENSION_CONFLICT_T *)data->data;
            CsrUint16 index, opcode_idx = 0xFF;

            /* Check whether the received hash is of any interest */
            for(index = 0; index < MAX_EXT_OPCODE_RANGE_SUPPORTED; index++)
            {
                if(!MemCmp(&ext_hdlr_data.prop_opcode[index]
                                                    .manuf_hash_string,
                       p_event->extensionhash, MANUF_HASH_STRING_LENGTH))
                {
                    opcode_idx = index;
                    break;
                }
            }

            /* The hash string has matched, hence this is for our device */
            if(opcode_idx != 0xFF)
            {
                /* It looks like there is a conflict in the storerd proposed
                 * opcode range, so delete the same.
                 */
                if(ext_hdlr_data.prop_opcode[opcode_idx].opcode != 0)
                {
                    if(p_event->reason == 0x01)
                    {
                        /* Add opcode range and proposed opcode value here */
                        ext_hdlr_data.prop_opcode[opcode_idx].opcode = 0;
                        ext_hdlr_data.prop_opcode[opcode_idx].range = 0;
                        writeExtensionDataOnIndex(opcode_idx);
                    }
                    else if((p_event->reason & 0xF0) == 0x80)
                    {
                        /* Modify the opcode range and proposed opcode value */
                        ext_hdlr_data.prop_opcode[opcode_idx].opcode = 
                                                    p_event->proposedopcode;
                        ext_hdlr_data.prop_opcode[opcode_idx].range = 
                                                    p_event->reason & 0x0F;
                        writeExtensionDataOnIndex(opcode_idx);
                    }
                }
            }
        }
        break;
        
        default:
        break;
    }  
    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadExtensionModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads action model data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void ReadExtensionModelDataFromNVM(CsrUint16 offset)
{
    uint8 index;

    for(index=0; index < MAX_EXT_OPCODE_RANGE_SUPPORTED; index++)
    {
        CsrUint16 nvm_ext_index = GET_EXTENSION_NVM_OFFSET(index);

        Nvm_Read(
            (CsrUint16 *)&ext_hdlr_data.prop_opcode[index].opcode,
                  sizeof(CsrUint16),
                  nvm_ext_index + NVM_OFFSET_EXTENSION_OPCODE);

        Nvm_Read(
            (CsrUint16 *)&ext_hdlr_data.prop_opcode[index].range,
                 sizeof(CsrUint16), 
                 nvm_ext_index + NVM_OFFSET_EXTENSION_RANGE);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteExtensionModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes action model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void WriteExtensionModelDataOntoNVM(CsrUint16 offset)
{
    uint8 index;
    for(index=0; index < MAX_EXT_OPCODE_RANGE_SUPPORTED; index++)
    {
        writeExtensionDataOnIndex(index);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ExtensionModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the extension model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ExtensionModelHandlerInit(CsrUint8 nw_id,
                                     CsrUint16 model_grps[],
                                     CsrUint16 num_groups)
{
    /* Initialize extension Model */
    ExtensionModelInit(nw_id, 
                       model_grps,
                       num_groups,
                       extensionModelEventHandler);

    /* Initialize extension Client to receive the conflict messages */
    ExtensionModelClientInit(extensionClientEventHandler);

    /* Initialize the extension opcode range structure with the server */
    ExtensionServerSetupOpcodeList((uint16*) ext_hdlr_data.prop_opcode, 
                                    MAX_EXT_OPCODE_RANGE_SUPPORTED);

    /* Initialize the extension opcode range structure with the client */
    ExtensionClientSetupOpcodeList((uint16*) ext_hdlr_data.prop_opcode, 
                                    MAX_EXT_OPCODE_RANGE_SUPPORTED);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ExtensionModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Extension Model data.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ExtensionModelDataInit(void)
{
    MemSet(&ext_hdlr_data, 0, sizeof(ext_hdlr_data));

    /* Hash calculated for the manufacturer string value Qualcomm-Inc in 
     * little endian format 
     */
    ext_hdlr_data.prop_opcode[0].manuf_hash_string[5] = 0xD0;
    ext_hdlr_data.prop_opcode[0].manuf_hash_string[4] = 0x2E;
    ext_hdlr_data.prop_opcode[0].manuf_hash_string[3] = 0xBA;
    ext_hdlr_data.prop_opcode[0].manuf_hash_string[2] = 0xD3;
    ext_hdlr_data.prop_opcode[0].manuf_hash_string[1] = 0xA8;
    ext_hdlr_data.prop_opcode[0].manuf_hash_string[0] = 0x16;

    /* Hash calculated for the manufacturer string value Qualcomm-CSR in 
     * little endian format. 
     */
    ext_hdlr_data.prop_opcode[1].manuf_hash_string[5] = 0x3C;
    ext_hdlr_data.prop_opcode[1].manuf_hash_string[4] = 0xE5;
    ext_hdlr_data.prop_opcode[1].manuf_hash_string[3] = 0x08;
    ext_hdlr_data.prop_opcode[1].manuf_hash_string[2] = 0x0D;
    ext_hdlr_data.prop_opcode[1].manuf_hash_string[1] = 0xA5;
    ext_hdlr_data.prop_opcode[1].manuf_hash_string[0] = 0x40;
}

#endif /* ENABLE_EXTENSION_MODEL */

