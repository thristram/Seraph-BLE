/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_security.c
 *
 *  DESCRIPTION
 *      This file defines connection manager security functionality security
 *
 ******************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <bluetooth.h>
#include <gatt.h>
#include <security.h>
#include <mem.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_api.h"
#include "cm_private.h"
#include "cm_client.h"
#include "cm_server.h"
#include "cm_security.h"
#include "nvm_access.h"

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* NVM offset for bonded flag */
#define NVM_CM_OFFSET_BONDED_INFO(n)     (g_security_data.nvm_offset +      \
                                            (sizeof (CM_BONDED_DEVICE_INFO_T)  \
                                            * (n)))

/* Total NVM Words used by the CM Security manager */
#define NVM_CM_MAX_MEMORY_WORDS         (g_security_data.max_bonded_device  \
                                            * sizeof(CM_BONDED_DEVICE_INFO_T))

/*============================================================================*
 *  Private Data Type
 *============================================================================*/

/* Security manager's data */
typedef struct
{
    /* Bonded device list  */
    CM_BONDED_DEVICE_INFO_T *bonded_device;

    /* Total Devices bonded */
    uint16 max_bonded_device;

    /* Pointer to NVM offset for the CM Security manager */
    uint16 nvm_offset;

}g_security_data_T;

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

#ifndef THIN_CM4_MESH_NODE
/* Gets the free index from the bonded device list */
static bond_handle_id getFreeIndex(void);

/*  Stores the bonded device information */
static bond_handle_id storeBondedDeviceInfo(CM_BONDED_DEVICE_INFO_T *bond_info);
#endif /* THIN_CM4_MESH_NODE */
/*  Reads the bonding information from the NVM */
static void readBondingInfoFromNVM(bool nvm_start_fresh);

#ifndef THIN_CM4_MESH_NODE
/*  Handles the signal SM_KEYS_IND */
static void handleSignalSmKeysInd(h_sm_keys_ind_t *p_event_data);

/* Handles the signal SM_LONG_TERM_KEY_IND */
static void handleSignalSMLongTermKeyInd(h_sm_long_key_ind_t *p_event_data);

/*  Handles the signal SM_PAIRING_AUTH_IND */
static void handleSignalSmPairingAuthInd(h_sm_pairing_auth_ind_t *p_event_data);

/*  Handles the signal LM_EV_ENCRYPTION_CHANGE */
static void handleSignalLMEncryptionChange(
                    h_ls_encryption_change_ind_t *p_event_data);

/*  Handles the signal SM_DIV_APPROVE_IND */
static void handleSignalSmDivApproveInd(h_sm_div_approval_ind_t *p_event_data);

#endif /* THIN_CM4_MESH_NODE */
    
/*  Handles the signal SM_SIMPLE_PAIRING_COMPLETE_IND */
static void handleSignalSmSimplePairingCompleteInd(
                                 h_sm_pairing_complete_ind_t *p_event_data);


/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Security Manager Data */
static g_security_data_T g_security_data;

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      getFreeIndex
 *
 *  DESCRIPTION
 *       Gets the free index from the bonded device list
 *
 *  RETURNS
 *      bond_handle_id: Free Index
 *
 *---------------------------------------------------------------------------*/

static bond_handle_id getFreeIndex(void)
{
    bond_handle_id bond_index;

    for(bond_index = 0; bond_index < g_security_data.max_bonded_device; bond_index++)
    {
        if(g_security_data.bonded_device[bond_index].bonded == FALSE)
        {
            return bond_index;
        }
    }
    return g_security_data.max_bonded_device;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      storeBondedDeviceInfo
 *
 *  DESCRIPTION
 *       Stores the bonded device information
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static bond_handle_id storeBondedDeviceInfo(CM_BONDED_DEVICE_INFO_T *bond_info)
{
    bond_handle_id index;

    /* Clear the old bonding information*/
    index = CMGetBondId(&bond_info->bd_addr);

    if(index == CM_INVALID_BOND_ID)
    {
        /* get the free slot */
        index = getFreeIndex();
    }

    /* Store the new keys */
    MemCopy(&g_security_data.bonded_device[index], bond_info,
            sizeof(CM_BONDED_DEVICE_INFO_T));

    /* Write to NVM */
    Nvm_Write((uint16*)&g_security_data.bonded_device[index],
              sizeof(CM_BONDED_DEVICE_INFO_T),
              NVM_CM_OFFSET_BONDED_INFO(index));

    return index;
}
#endif /* THIN_CM4_MESH_NODE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      readBondingInfoFromNVM
 *
 *  DESCRIPTION
 *       Reads the bonding information from the NVM
 *
 *  RETURNS
 *      None
 *---------------------------------------------------------------------------*/

static void readBondingInfoFromNVM(bool nvm_start_fresh)
{
    uint16 index;
    for( index = 0; index < g_security_data.max_bonded_device; index++)
    {
        if(!nvm_start_fresh)
        {
            /* NVM is not fresh */

            /* Read from NVM */
            Nvm_Read((uint16*)&g_security_data.bonded_device[index],
                      sizeof(CM_BONDED_DEVICE_INFO_T),
                      NVM_CM_OFFSET_BONDED_INFO(index));
        }
        else
        {
            /* Reset the bonded device */
            MemSet(&g_security_data.bonded_device[index], 0,
                   sizeof(CM_BONDED_DEVICE_INFO_T));

            /* Write to NVM */
            Nvm_Write((uint16*)&g_security_data.bonded_device[index],
                      sizeof(CM_BONDED_DEVICE_INFO_T),
                      NVM_CM_OFFSET_BONDED_INFO(index));
           }
    }
}

#ifndef THIN_CM4_MESH_NODE

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSmKeysInd
 *
 *  DESCRIPTION
 *       Handles the signal SM_KEYS_IND and copies IRK
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalSmKeysInd(h_sm_keys_ind_t *p_event_data)
{
    device_handle_id device_id = CMGetDeviceId(HAL_SM_KEYS_IND,
                                               (void*)p_event_data);

    if(device_id == CM_INVALID_DEVICE_ID)
        return;

    /* Handling signal as per current state */
    switch(CMGetDevState(device_id))
    {
        case dev_state_connected:
        {
            CM_BONDED_DEVICE_INFO_T bond_info;

            bond_info.bonded = TRUE;
            CMGetBdAdressFromDeviceId(device_id, &bond_info.bd_addr);
            bond_info.peer_con_role = CMGetPeerDeviceRole(device_id);

            /* Store the new keys */
            MemCopy(&bond_info.keys, p_event_data->keys, sizeof(SM_KEYSET_T));

            /* store the bond information */
            storeBondedDeviceInfo(&bond_info);
        }
        break;

        default:
            /* Control should never come here */
            CMReportPanic(cm_panic_invalid_state);

        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSMLongTermKeyInd
 *
 *  DESCRIPTION
 *       Handles the signal SM_LONG_TERM_KEY_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalSMLongTermKeyInd(h_sm_long_key_ind_t *p_event_data)
{
    device_handle_id device_id = CMGetDeviceId(HAL_SM_LONG_TERM_KEY_IND,
                                               (void*)p_event_data);

    if(device_id == CM_INVALID_DEVICE_ID)
        return;

     /* Handling signal as per current state */
    switch(CMGetDevState(device_id))
    {
        case dev_state_connected:
        {
            CM_LTK_RESP_T ltk_rsp;
            ltk_rsp.cid = p_event_data->cid;
            uint16 ltk[8];

            /* Initialise the parameters */
            ltk_rsp.key_size = 0;
            ltk_rsp.ltk = NULL;

            /* Check if the device is bonded */
            bond_handle_id bond_id = CMGetBondIdFromDeviceId(device_id);
            if(bond_id != CM_INVALID_BOND_ID)
            {
                if(g_security_data.bonded_device[bond_id].keys.keys_present &
                   (1<<SM_KEY_TYPE_ENC_CENTRAL))
                {
                    MemCopy(ltk,
                            g_security_data.bonded_device[bond_id].keys.ltk,
                            CM_MAX_WORDS_LTK);
                    ltk_rsp.key_size = 16;
                    ltk_rsp.ltk = ltk;
                }
            }

#ifndef CSR101x_A05
            ltk_rsp.security_level =
                    g_security_data.bonded_device[bond_id].keys.security_level;
#endif /* !CSR101x_A05 */

            HALSendLongTermKeyAuthRsp(&ltk_rsp);
        }
        break;

        default:
            /* Control should never come here */
            CMReportPanic(cm_panic_invalid_state);

        break;
    }
}
#endif /* THIN_CM4_MESH_NODE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSmSimplePairingCompleteInd
 *
 *  DESCRIPTION
 *       Handles the signal SM_SIMPLE_PAIRING_COMPLETE_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalSmSimplePairingCompleteInd(
                                 h_sm_pairing_complete_ind_t *p_event_data)
{
    device_handle_id device_id = CMGetDeviceId(HAL_SM_PAIRING_COMPLETE_IND,
                                     (void*)p_event_data);

    if(device_id == CM_INVALID_DEVICE_ID)
        return;

    switch(CMGetDevState(device_id))
    {
        case dev_state_connected:
        {
            CM_BONDING_NOTIFY_T cm_bond_notify;

            if(p_event_data->status == sys_status_success)
            {
                /* Ignore this event if the device already bonded.
                 * This event could be arrived because of the
                 * encryption during reconnection.
                 */
                if(CMGetDeviceBondState(device_id) == cm_dev_bonded)
                    return;

                /* Update the device state to bonded */
                CMSetDeviceBondState(device_id, cm_dev_bonded);

                /* Get the bond id */
                cm_bond_notify.bond_id = CMGetBondIdFromDeviceId(device_id);

                /* Update the bond state */
                cm_bond_notify.result = cm_bond_res_success;
            }
            else
            {
                if(p_event_data->status == sm_status_repeated_attempts)
                {
                    cm_bond_notify.result = cm_bond_res_rep_attempt_failed;
                }
                else
                {
                    cm_bond_notify.result = cm_bond_res_failed;
                }

                /* Assign the invalid bond id */
                cm_bond_notify.bond_id = CM_INVALID_BOND_ID;
            }

            /* copy the device id */
            cm_bond_notify.device_id = device_id;

            /* Copy the device id */
            CMGetBdAdressFromDeviceId(device_id, &cm_bond_notify.bd_addr);

            /* Send the bonding status back to all registered services */

#if defined (SERVER)
            CMServerNotifyGenericEvent(CM_BONDING_NOTIFY,
                              (CM_EVENT_T *)&cm_bond_notify);
#endif /* SERVER */

#if defined (CLIENT)
            CMClientNotifyGenericEvent(CM_BONDING_NOTIFY,
                                  (CM_EVENT_T *)&cm_bond_notify);
#endif /* CLIENT */

            CMNotifyEventToApplication(CM_BONDING_NOTIFY,
                              (CM_EVENT_T *)&cm_bond_notify);

            if(p_event_data->status == sm_status_repeated_attempts)
            {
                /* Bonding failed, disconnect the link */
                CMSetDevState(device_id, dev_state_disconnecting);
            }
        }
        break;

        default:
            /* Firmware may send this signal after disconnection. So don't
             * panic but ignore this signal.
             */
        break;
    }
}

#ifndef THIN_CM4_MESH_NODE
/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSmDivApproveInd
 *
 *  DESCRIPTION
 *       Handles the signal SM_DIV_APPROVE_IND
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleSignalSmDivApproveInd(h_sm_div_approval_ind_t *p_event_data)
{
    device_handle_id device_id = CMGetDeviceId(HAL_SM_DIV_APPROVAL_IND,
                                     (void*)p_event_data);

    switch(CMGetDevState(device_id))
    {
        case dev_state_connected:
        {
            sm_div_verdict approve_div = SM_DIV_REVOKED;
            bond_handle_id bond_id = CMGetBondIdFromDeviceId(device_id);

            if(bond_id != CM_INVALID_BOND_ID)
            {
                if(g_security_data.bonded_device[bond_id].keys.div ==
                                                p_event_data->div)
                {
                   approve_div = SM_DIV_APPROVED;
                }
            }

            /* Send the diversifier approval response */
            HALSMDivApprovalRsp(p_event_data->cid, approve_div);
        }
        break;

        default:
            /* Control should never come here */
            CMReportPanic(cm_panic_invalid_state);
        break;
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSmPairingAuthInd
 *
 *  DESCRIPTION
 *      Handles the signal SM_PAIRING_AUTH_IND. This message will
 *      only be received when the peer device is initiating 'Just Works'
 *      pairing
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleSignalSmPairingAuthInd(h_sm_pairing_auth_ind_t *p_event_data)
{
    CM_BONDING_AUTH_IND_T   bonding_auth_ind;

    bonding_auth_ind.device_id = CMGetDeviceId(HAL_SM_PAIRING_AUTH_IND,
                                     (void*)p_event_data);

    HALParseSmPairingAuthInd(&bonding_auth_ind, p_event_data);

    CMNotifyEventToApplication(CM_BONDING_AUTH_IND,
                               (CM_EVENT_T *)&bonding_auth_ind);
}

#endif /* THIN_CM4_MESH_NODE */
/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalLMEncryptionChange
 *
 *  DESCRIPTION
 *       Handles the signal LM_EV_ENCRYPTION_CHANGE
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalLMEncryptionChange(
                    h_ls_encryption_change_ind_t *p_event_data)
{
    device_handle_id device_id = CMGetDeviceId(HAL_SM_ENCRYPTION_CHANGE_IND,
                                     (void*)p_event_data);

    /* Handling signal as per current state */

    switch(CMGetDevState(device_id))
    {
        case dev_state_connected:
        {
            CM_ENCRYPTION_NOTIFY_T enc_ind;
            enc_ind.device_id = device_id;
            enc_ind.enc_enable = p_event_data->enc_enable;

            if(p_event_data->status == sys_status_success)
            {
                enc_ind.status = TRUE;
            }
            else
            {
                enc_ind.status = FALSE;
            }

            /* Update the Device Encryption State */
            CMSetDeviceEncryptState(device_id, p_event_data->enc_enable);

            /* Send the Encryption Change Status to the application. */

#if defined (SERVER)
            CMServerNotifyGenericEvent(CM_ENCRYPTION_NOTIFY,
                                     (CM_EVENT_T *)&enc_ind);
#endif /* SERVER */

#if defined (CLIENT)
            CMClientNotifyGenericEvent(CM_ENCRYPTION_NOTIFY, (CM_EVENT_T *)&enc_ind);
#endif /* CLIENT */

            CMNotifyEventToApplication(CM_ENCRYPTION_NOTIFY,
                                      (CM_EVENT_T *)&enc_ind);
        }
        break;

        default:
            /* Control should never come here */
            CMReportPanic(cm_panic_invalid_state);

        break;
    }
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSmPasskeyInputInd
 *
 *  DESCRIPTION
 *       Handles the signal SM_KEYS_IND and copies IRK
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalSmPasskeyInputInd(h_sm_passkey_input_ind_t *p_event_data)
{
    device_handle_id device_id = CMGetDeviceId(HAL_SM_PASSKEY_INPUT_IND,
                                                   (void*)p_event_data);

        if(device_id == CM_INVALID_DEVICE_ID)
            return;

        /* Handling signal as per current state */
        switch(CMGetDevState(device_id))
        {
        case dev_state_connected:
        {
            CM_PASSKEY_NOTIFY_T passkey_ind;
            passkey_ind.device_id = device_id;
            CMNotifyEventToApplication(CM_PASSKEY_INPUT,
                                          (CM_EVENT_T *)&passkey_ind);
        }
        break;

        default:
            /* Control should never come here */
            CMReportPanic(cm_panic_invalid_state);

            break;
        }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalSmkeyRequestInd
 *
 *  DESCRIPTION
 *       Handles the signal SM_KEY_REQUEST_IND
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalSmkeyRequestInd(h_sm_key_request_ind_t *p_event_data)
{
    h_sm_keyset_t *keys = NULL;
    device_handle_id device_id = CMGetDeviceId(HAL_SM_KEY_REQUEST_IND,
                                               (void*)p_event_data);

    if(device_id == CM_INVALID_DEVICE_ID)
        return;

    if(CMGetDeviceBondState(device_id) == cm_dev_bonded)
    {
        bond_handle_id bond_id = CMGetBondIdFromDeviceId(device_id);
        keys = &g_security_data.bonded_device[bond_id].keys;
    }
    HALSMKeyRequestResponse(device_id, keys);
}
#endif /* THIN_CM4_MESH_NODE */

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSecurityInit
 *
 *  DESCRIPTION
 *       Initialises the CM security manager entity
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMSecurityInit(CM_INIT_PARAMS_T *cm_init_params)
{
    /* Initialise the Security Manager */
    HALSMInit(cm_init_params->diversifier);

    g_security_data.bonded_device =
            (CM_BONDED_DEVICE_INFO_T*)cm_init_params->bonded_devices;
    g_security_data.max_bonded_device = cm_init_params->max_bonded_devices;
    g_security_data.nvm_offset = *cm_init_params->nvm_offset;

    /* Read the Bonded device Info from NVM */
    readBondingInfoFromNVM(cm_init_params->nvm_start_fresh);

    /* increment the nvm offset */
    *cm_init_params->nvm_offset += NVM_CM_MAX_MEMORY_WORDS;
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSecuritySetConfiguration
 *
 *  DESCRIPTION
 *       Sets the security configurations
 *
 *  RETURNS
 *      sys_status
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code CMSecuritySetConfiguration(uint16 io_capability)
{
    return HALSecuritySetConfiguration(io_capability);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondedDevices
 *
 *  DESCRIPTION
 *       Fills the bonded devices address and its role to the given buffer
 *
 *  RETURNS
 *      bool: TRUE if successfull
 *
 *---------------------------------------------------------------------------*/
extern bool CMGetBondedDevices(CM_BONDED_DEVICE_T *bond_dev, uint16 *num_dev)
{
    uint16 index = 0;
    *num_dev = 0;

    if(bond_dev == NULL)
        return FALSE;

    for(index = 0; index < g_security_data.max_bonded_device; index++)
    {
        if(g_security_data.bonded_device[index].bonded == TRUE)
        {
            /* Bond ID */
            bond_dev[*num_dev].bond_id =  index;

            /* Bluetooth Address */
            bond_dev[*num_dev].bd_addr =
                    g_security_data.bonded_device[index].bd_addr;

            /* Peer connection role */
            bond_dev[*num_dev].role =
                    g_security_data.bonded_device[index].peer_con_role;

            /* Diversifier */
            bond_dev[*num_dev].div =
                    g_security_data.bonded_device[index].keys.div;

            if(g_security_data.bonded_device[index].keys.keys_present &
                    (1 << SM_KEY_TYPE_ID))
            {
                /* Identity Root Key */
                MemCopy(bond_dev[*num_dev].irk,
                        g_security_data.bonded_device[index].keys.irk,
                        CM_MAX_WORDS_IRK);
            }

            /* Long Term Key */
            MemCopy(bond_dev[*num_dev].ltk,
                    g_security_data.bonded_device[index].keys.ltk,
                    CM_MAX_WORDS_LTK);

            ++(*num_dev);
        }
    }

    return TRUE;
}
#endif /* THIN_CM4_MESH_NODE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMIsDeviceBonded
 *
 *  DESCRIPTION
 *       Checks the given device is bonded or not
 *
 *  RETURNS/MODIFIES
 *      bool - True if device is bonded
 *
 *----------------------------------------------------------------------------*/

extern bool CMIsDeviceBonded(TYPED_BD_ADDR_T *bdaddr)
{
    uint16 count;

    for(count = 0; count < g_security_data.max_bonded_device; count++)
    {
        if(!g_security_data.bonded_device[count].bonded)
            continue;

        if(HALMatchBluetoothAddress(bdaddr,
                           &g_security_data.bonded_device[count].bd_addr,
                           g_security_data.bonded_device[count].keys.irk))
        {
            return TRUE;
        }
    }
    return FALSE;
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondedDevice
 *
 *  DESCRIPTION
 *       Gets the bonded device information
 *
 *  RETURNS/MODIFIES
 *      bool - TRUE if the request processed
 *
 *----------------------------------------------------------------------------*/
extern bool CMGetBondedDevice(bond_handle_id bond_id,
                              CM_BONDED_DEVICE_INFO_T *device)
{
    if(!CMIsBondIdValid(bond_id))
        return FALSE;

    *device = g_security_data.bonded_device[bond_id];

    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondedDeviceFromBondId
 *
 *  DESCRIPTION
 *       Get the bonded device from the bond id
 *
 *  RETURNS/MODIFIES
 *      bool - TRUE if the request processed
 *
 *----------------------------------------------------------------------------*/
extern bool CMGetBondedDeviceFromBondId(bond_handle_id bond_id,
                              CM_BONDED_DEVICE_T *bond_dev)
{
    CM_BONDED_DEVICE_INFO_T device;

    if(!CMIsBondIdValid(bond_id))
        return FALSE;

    device = g_security_data.bonded_device[bond_id];

    /* Bond ID */
    bond_dev->bond_id =  bond_id;

    /* Bluetooth Address */
    bond_dev->bd_addr = device.bd_addr;

    /* Peer connection role */
    bond_dev->role = device.peer_con_role;

    /* Diversifier */
    bond_dev->div = device.keys.div;

    if(device.keys.keys_present & (1 << SM_KEY_TYPE_ID))
    {
        /* Identity Root Key */
        MemCopy(bond_dev->irk, device.keys.irk, CM_MAX_WORDS_IRK);
    }

    /* Long Term Key */
    MemCopy(bond_dev->ltk, device.keys.ltk, CM_MAX_WORDS_LTK);

    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMIsBondIdValid
 *
 *  DESCRIPTION
 *       Checks the given bond id valid or not
 *
 *  RETURNS/MODIFIES
 *      bool - True if bond is valid
 *
 *----------------------------------------------------------------------------*/

extern bool CMIsBondIdValid(bond_handle_id bond_id)
{
    return (bond_id < g_security_data.max_bonded_device);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondedDeviceRole
 *
 *  DESCRIPTION
 *       Gets the role of the given bonded device
 *
 *  RETURNS/MODIFIES
 *      cm_peer_con_role: Role of the peer device
 *
 *----------------------------------------------------------------------------*/

extern cm_peer_con_role CMGetBondedDeviceRole(TYPED_BD_ADDR_T *bd_addr)
{
    uint8 count = 0;

    cm_peer_con_role peer_con_role = con_role_invalid;

    for(count = 0; count < g_security_data.max_bonded_device; count++)
    {
        if(HALMatchBluetoothAddress(bd_addr,
                           &g_security_data.bonded_device[count].bd_addr,
                           g_security_data.bonded_device[count].keys.irk))
        {
            peer_con_role = g_security_data.bonded_device[count].peer_con_role;
        }
    }

    return peer_con_role;
}

#endif /* THIN_CM4_MESH_NODE */
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondId
 *
 *  DESCRIPTION
 *       Gets the bond id of the given bonded device
 *
 *  RETURNS/MODIFIES
 *    bond_handle_id: index in the bonded devices list
 *
 *----------------------------------------------------------------------------*/

extern bond_handle_id CMGetBondId(TYPED_BD_ADDR_T *bd_addr)
{
    bond_handle_id index;

    for(index = 0; index < g_security_data.max_bonded_device; index++)
    {
        if(g_security_data.bonded_device[index].bonded == TRUE)
        {
            if(HALMatchBluetoothAddress(bd_addr,
                            &g_security_data.bonded_device[index].bd_addr,
                            g_security_data.bonded_device[index].keys.irk))
            {
                return index;
            }
        }
    }

    return CM_INVALID_BOND_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetBondedBDAddr
 *
 *  DESCRIPTION
 *       Gets the Bluetooth address from the bond index.
 *
 *  RETURNS
 *      bool: TRUE if the device found
 *
 *---------------------------------------------------------------------------*/

extern bool CMGetBondedBDAddr(bond_handle_id bond_id, TYPED_BD_ADDR_T *bdaddr)
{
    if(bond_id < g_security_data.max_bonded_device)
    {
        /* valid bond id */
        *bdaddr = g_security_data.bonded_device[bond_id].bd_addr;
        return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetMaxBondedDevices
 *
 *  DESCRIPTION
 *       Gets the size of the bonded device list
 *
 *  RETURNS
 *      uint16: Maximum size of the bonded device list
 *
 *---------------------------------------------------------------------------*/

extern uint16 CMGetMaxBondedDevices(void)
{
    return g_security_data.max_bonded_device;
}


#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMStartEncryption
 *
 *  DESCRIPTION
 *       Starts encryption. If not bonded initiates the pairing first.
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/

extern cm_status_code CMStartEncryption(device_handle_id device_id)
{
    HALStartEncryption(device_id);

    return cm_status_success;
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      CMBondingAuthResponse
 *
 *  DESCRIPTION
 *       Sends bonding authorisation response
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void CMBondingAuthResponse(CM_AUTH_RESP_T *auth_resp)
{
    HALSendAuthRsp(auth_resp);
}
#endif /* THIN_CM4_MESH_NODE */
/*----------------------------------------------------------------------------*
 *  NAME
 *      ClearBondingInfo
 *
 *  DESCRIPTION
 *       Clears the bonding information of the given bonded device
 *
 *  RETURNS/MODIFIES
 *      bool: True if the procedure successful
 *
 *----------------------------------------------------------------------------*/

extern bool ClearBondingInfo(bond_handle_id bond_id)
{
    CM_BONDING_NOTIFY_T cm_bond_notify;

    if(bond_id == CM_INVALID_BOND_ID)
        return FALSE;

    CMGetBondedBDAddr(bond_id, &cm_bond_notify.bd_addr);
    cm_bond_notify.bond_id = bond_id ;
    cm_bond_notify.result = cm_unbond_res_success;

    /* Reset the bonded flag */
    g_security_data.bonded_device[bond_id].bonded = FALSE;

    /* Write to the NVM */
    Nvm_Write((uint16*)&g_security_data.bonded_device[bond_id],
              sizeof(CM_BONDED_DEVICE_INFO_T),
              NVM_CM_OFFSET_BONDED_INFO(bond_id));

#if defined (CLIENT)
    /* Send bond notify event to the client handlers */
    CMClientNotifyGenericEvent(CM_BONDING_NOTIFY, (CM_EVENT_T *)&cm_bond_notify);
#endif /* CLIENT */

#if defined (SERVER)
    /* Send bond notify event to the server handlers */
    CMServerNotifyGenericEvent(CM_BONDING_NOTIFY,(CM_EVENT_T *)&cm_bond_notify);
#endif /* SERVER */

    /* Send bond notify event to the application */
    CMNotifyEventToApplication(CM_BONDING_NOTIFY,
                               (CM_EVENT_T *)&cm_bond_notify);

    return TRUE;
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPassKeyInputResponse
 *
 *  DESCRIPTION
 *       Sends the passkey reponse. Passed to the HAL layer to handle based on
 *       the underlining hardware.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void CMPassKeyInputResponse(device_handle_id device_id,
                                   const uint32 *pass_key)
{
    HALSendPasskeyRsp(device_id, pass_key);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMStoreFactoryBond
 *
 *  DESCRIPTION
 *       Stores the factory bonding information
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern bond_handle_id CMStoreFactoryBond(CM_FACTORY_BOND_INFO_T
                               *factory_bond_info)
{
    CM_BONDED_DEVICE_INFO_T bond_info;
    h_sm_keyset_t keyset;

    /* Reset the bond_info and keyset structures*/
    MemSet(&bond_info, 0,sizeof(CM_BONDED_DEVICE_INFO_T));
    MemSet(&keyset,0,sizeof(SM_KEYSET_T));

    /* Copy Keyset */
    MemCopy(keyset.irk,factory_bond_info->irk,CM_MAX_WORDS_IRK);
    MemCopy(keyset.ltk,factory_bond_info->ltk,CM_MAX_WORDS_LTK);
#ifndef CSR101x_A05
    keyset.security_level = factory_bond_info->security_level;
#endif
    keyset.keys_present = factory_bond_info->keys_present;
    keyset.encryption_key_size = factory_bond_info->encryption_key_size;

    /* store the bond information */
    bond_info.bonded = TRUE;
    bond_info.bd_addr = factory_bond_info->bd_addr;
    bond_info.peer_con_role = factory_bond_info->role;
    MemCopy(&bond_info.keys,&keyset, sizeof(SM_KEYSET_T));

    bond_handle_id bond_id = storeBondedDeviceInfo(&bond_info);

    return bond_id;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMNotifyFactoryBond
 *
 *  DESCRIPTION
 *       Notify in-factory bonding status to all registered services
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void CMNotifyFactoryBond(device_handle_id device_id)
{
    CM_BONDING_NOTIFY_T cm_bond_notify;

    /* Get the bond id */
    cm_bond_notify.bond_id = CMGetBondIdFromDeviceId(device_id);

    /* Update the bond state */
    cm_bond_notify.result = cm_bond_res_success;

    /* copy the device id */
    cm_bond_notify.device_id = device_id;

    /* Copy the device id */
    CMGetBdAdressFromDeviceId(device_id, &cm_bond_notify.bd_addr);

    /* Send the bonding status back to all registered services */

#if defined (SERVER)
    CMServerNotifyGenericEvent(CM_BONDING_NOTIFY,
                              (CM_EVENT_T *)&cm_bond_notify);
#endif /* SERVER */

#if defined (CLIENT)
    CMClientNotifyGenericEvent(CM_BONDING_NOTIFY,
                                  (CM_EVENT_T *)&cm_bond_notify);
#endif /* CLIENT */
}
#endif /* THIN_CM4_MESH_NODE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSecurityHandleProcessLMEvent
 *
 *  DESCRIPTION
 *       Handles the firmware events related to the security role
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/

extern void CMSecurityHandleProcessLMEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
#ifndef THIN_CM4_MESH_NODE
        case HAL_SM_KEYS_IND:
        {
            /* Indication for the keys and associated security information
             * on a connection that has completed Short Term Key Generation
             * or Transport Specific Key Distribution
             */
            handleSignalSmKeysInd(
                (h_sm_keys_ind_t*)HALGetMsg(msg));
        }
        break;
        case HAL_SM_LONG_TERM_KEY_IND:
        {
            /* Indication for the keys and associated security information
             * on a connection that has completed Short Term Key Generation
             * or Transport Specific Key Distribution
             */
            handleSignalSMLongTermKeyInd(
                    (h_sm_long_key_ind_t*)HALGetMsg(msg));
        }
        break;
#endif /* THIN_CM4_MESH_NODE */
        case HAL_SM_PAIRING_COMPLETE_IND:
        {
            /* Indication for completion of Pairing procedure */
            handleSignalSmSimplePairingCompleteInd(
                (h_sm_pairing_complete_ind_t*)HALGetMsg(msg));
        }
        break;
		case HAL_SM_ENCRYPTION_CHANGE_IND:
        {
            h_ls_encryption_change_ind_t enc_change_ind;
            
            HALParseEncChangeInd(&enc_change_ind, msg);            
            
            /* Indication for encryption change event */
            handleSignalLMEncryptionChange(&enc_change_ind);
        }
        break;

#ifndef THIN_CM4_MESH_NODE
        case HAL_SM_DIV_APPROVAL_IND:
        {
            /* Indication for SM Diversifier approval requested by F/W when
             * the last bonded host exchange keys. Application may or may not
             * approve the diversifier depending upon whether the application
             * is still bonded to the same host
             */
            handleSignalSmDivApproveInd(
                (h_sm_div_approval_ind_t*)HALGetMsg(msg));
        }
        break;
        case HAL_SM_PAIRING_AUTH_IND:
        {
            /* Authorize or Reject the pairing request */
            handleSignalSmPairingAuthInd(
                (h_sm_pairing_auth_ind_t*)HALGetMsg(msg));
        }
        break;

        case HAL_SM_PASSKEY_INPUT_IND:
        {
            handleSignalSmPasskeyInputInd
                    ((h_sm_passkey_input_ind_t*)HALGetMsg(msg));
        }
        break;

        case HAL_SM_KEY_REQUEST_IND:
        {
            handleSignalSmkeyRequestInd
                    ((h_sm_key_request_ind_t*)HALGetMsg(msg));
        }
        break;
#endif /* THIN_CM4_MESH_NODE */
        default:
        break;
    }
}

