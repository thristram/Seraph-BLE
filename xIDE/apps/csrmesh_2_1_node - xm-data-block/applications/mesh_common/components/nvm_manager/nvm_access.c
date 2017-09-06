/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      nvm_access.c
 *
 *  DESCRIPTION
 *      This file defines routines which the application uses to access
 *      NVM.
 *
 *
 ******************************************************************************/
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <pio.h>
#include <panic.h>
#include <nvm.h>
#if defined (CSR101x_A05)
#include <i2c.h>
#endif /* CSR101x_A05 */

#if defined (CSR102x_A05)
#include <random.h>
#endif
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "nvm_access.h"

#define CSR_MESH_SEC_SANITY_MAGIC               (0x0060)
#define NVM_MESH_SECURE_ID                      (4)
#define USER_STORE_NVM_SIZE                     (1024)

/*============================================================================*
 *  Local Definitions
 *============================================================================*/
enum
{
    /* Failure while opening the NVM */
    panic_nvm_open = 101,

    /* Failure while creating the NVM */
    panic_nvm_create,

    /* Failure while reading NVM */
    panic_nvm_read,

    /* Failure while writing NVM */
    panic_nvm_write,
};

#ifndef CSR101x_A05
/* NVM states */
typedef enum
{
    NVM_STATE_UNINITIALISED,
    NVM_STATE_OPENING,
    NVM_STATE_CREATING,
    NVM_STATE_INITIALISED,
} nvm_state_t;
#endif

/* NVM offset for NVM sanity word */
#define NVM_OFFSET_SANITY_WORD                      (0)

/* NVM Size */
#define NVM_MAX_MEMORY_WORDS                        (1)

/*============================================================================*
 *  Local Data
 *============================================================================*/
static bool g_nvm_fresh = FALSE;
#ifndef CSR101x_A05
static nvm_state_t g_nvm_state = NVM_STATE_UNINITIALISED;
static uint16 g_nvm_sanity;
static uint16 g_nvm_offset;
static memory_type_t g_nvm_type = MEM_DEVICE_SMEM;
static store_id_t g_store_id;
#endif /* !CSR101x_A05 */

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
/* Raises the NVM panic */
static void reportPanic(uint16 code);

/* This function reads the sanity word from the NVM */
static void nvmReadSanity(uint16 nvm_sanity, uint16 *nvm_offset);

/* This function disables the NVM */
static void nvmDisable(void);

typedef enum {
    secureKey_init = 0,
    secureKey_sanity_read,
    secureKey_sanity_write,
    secureKey_key_read,
    secureKey_key_write,
    secureKey_key_available,
    secureKey_key_unavailable,
} secureKeyUpdateState_t;

typedef struct {
    uint32 sanity_word;
    uint16 secure_key[8];
    secureKeyUpdateState_t state;
} SecureKeyUpdateInfo_t;

SecureKeyUpdateInfo_t secureKeyInfo;

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      reportPanic
 *
 *  DESCRIPTION
 *      Raises the NVM panic
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void reportPanic(uint16 code)
{
    Panic(code);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      nvmReadSanity
 *
 *  DESCRIPTION
 *      This function reads the sanity word from the NVM
 *
 *  RETURNS
 *      boolean   TRUE if the NVM is fresh
 *
 *----------------------------------------------------------------------------*/
static void nvmReadSanity(uint16 nvm_sanity, uint16 *nvm_offset)
{
    /* Initialise the nvm sanity */
    uint16 sanity_word = 0xffff;

    /* Read NVM Sanity word */
    Nvm_Read(&sanity_word,
             sizeof(sanity_word),
             NVM_OFFSET_SANITY_WORD + (*nvm_offset));

    /* If a valid NVM sanity word is found */
    if(sanity_word != nvm_sanity)
    {
        sanity_word = nvm_sanity;

        /* Write NVM Sanity word to the NVM */
        Nvm_Write(&sanity_word,
                  sizeof(sanity_word),
                  NVM_OFFSET_SANITY_WORD + (*nvm_offset));

        /* NVM is fresh */
        g_nvm_fresh = TRUE;
    }

    /* increment the nvm offset */
    *nvm_offset += NVM_MAX_MEMORY_WORDS;

    /* Invoke the application callback */
    AppNvmReady(g_nvm_fresh, *nvm_offset);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      nvmDisable
 *
 *  DESCRIPTION
 *      This function is used to perform things necessary to save power on NVM
 *      once the read/write operations are done.
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void nvmDisable(void)
{
#if defined (CSR101x_A05)
    /* Disable the NVM */
    NvmDisable();

    /* Pull down the I2C lines */
    PioSetI2CPullMode(pio_i2c_pull_mode_strong_pull_down);
#endif /* CSR101x_A05 */

}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

#ifndef NVM_DONT_PAD
/*----------------------------------------------------------------------------
 *  Nvm_ValidateVersionedHeader
 *----------------------------------------------------------------------------*/
/*! \brief Verifies that the data block in NVM matches the expected identifier
 *
 * This function validates the Versioned NVM header in NVM against a set
 * of parameters. If the versioned header in NVM matches the ID and length
 * passed, but the version number does not match, then the migrate_handler
 * is called. The migrate_handler should attempt to update the data in NVM to
 * the new version.
 * \param[in,out] nvm_offset Pointer to NVM offset
 * \param[in] key_id System specific ID to check for
 * \param[in] data_len Length of data expected
 * \param[in] version Application specific version number for the data
 * \returns True if the requested versioned header was found; 
 *          nvm_offset will be updated to the next NVM offset after the header.
 *          False if the header was not found; nvm_offset is not changed.
 */
bool Nvm_ValidateVersionedHeader(uint16 *nvm_offset, 
                                 nvm_data_id_t key_id, uint16 data_len, uint16 version,
                                 migrate_handler_t migrate_handler)
{
    nvm_versioned_header_t nvh;

    Nvm_Read((uint16*)&nvh, sizeof(nvh), *nvm_offset);
    if (nvh.header.id != key_id)
    {
        /* nvm is corrupt; we need to write the NVM again */
        return FALSE;
    }
    if (nvh.header.len != data_len)
    {
        /* nvm is corrupt; we need to write the NVM again */
        return FALSE;
    }
    if (nvh.version != version)
    {
        if (migrate_handler!=NULL)
        {
            /* See if the data can be migrated in situ */
            if (migrate_handler(nvh.version,
                                (uint16)(*nvm_offset) + sizeof(nvh), 
                                nvh.header.len))
            {
                /* Burn the new header version */
                nvh.version = version;
                Nvm_Write((uint16*)&nvh, sizeof(nvh), *nvm_offset);
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    *nvm_offset += sizeof(nvh);
    return TRUE;
}

/*----------------------------------------------------------------------------
 *  Nvm_WriteVersionedHeader
 *----------------------------------------------------------------------------*/
/*! \brief Writes a versioned header to NVM
 *
 * This function writes meta data to the NVM. The data contains an ID
 * which should be used by the application to identify its data block.
 * 
 * \param[in,out] nvm_offset Offset into the NVM
 * \param[in] key_id Identifier for the block of data
 * \param[in] data_len Size of the block of data
 * \param[in] version Application specific version information about the block of data
 * \returns Nothing
 */
void Nvm_WriteVersionedHeader(  uint16 *nvm_offset, nvm_data_id_t key_id, 
                                uint16 data_len, uint16 version)
{
    nvm_versioned_header_t nvh;

    nvh.header.id = key_id;
    nvh.header.len = data_len;
    nvh.version = version;
        
    Nvm_Write((uint16*)&nvh, sizeof(nvh), *nvm_offset);
    *nvm_offset += sizeof(nvh);
}

#endif /* NVM_DONT_PAD */

#if defined (CSR101x_A05)
/*----------------------------------------------------------------------------*
 *  NAME
 *      Nvm_Init
 *
 *  DESCRIPTION
 *      This function initialises the NVM
 *
 *  RETURNS/MODIFIES
 *      bool: TRUE if NVM is fresh, FALSE otherwise
 *
 *---------------------------------------------------------------------------*/
extern void Nvm_Init(uint16 nvm_sanity, uint16 *nvm_offset)
{
#ifdef NVM_TYPE_EEPROM
    /* Configure the NVM manager to use I2C EEPROM for NVM store */
    NvmConfigureI2cEeprom();
#elif NVM_TYPE_FLASH
    /* Configure the NVM Manager to use SPI flash for NVM store. */
    NvmConfigureSpiFlash();
#endif /* NVM_TYPE_EEPROM */
    nvmReadSanity(nvm_sanity, nvm_offset);
}
#else
/*----------------------------------------------------------------------------*
 *  NAME
 *      Nvm_Init
 *
 *  DESCRIPTION
 *      This function initialises the NVM
 *
 *  RETURNS/MODIFIES
 *      bool: TRUE if NVM is fresh, FALSE otherwise
 *
 *---------------------------------------------------------------------------*/
extern void Nvm_Init(store_id_t id, uint16 nvm_sanity, uint16 *nvm_offset)
{
    g_nvm_sanity = nvm_sanity;
    g_nvm_offset = *nvm_offset;
	g_nvm_state = NVM_STATE_OPENING;
	g_store_id = id;
    secureKeyInfo.state = secureKey_init;
    secureKeyInfo.sanity_word = CSR_MESH_SEC_SANITY_MAGIC;
#ifdef SECURE_STORE_ENABLE    
    Userstore_Open(NVM_MESH_SECURE_ID);
#else
    Userstore_Open(g_store_id);
#endif
}
#endif /* CSR101x_A05 */

/*----------------------------------------------------------------------------*
 *  NAME
 *      Nvm_Read
 *
 *  DESCRIPTION
 *      Read words from the NVM Store after preparing the NVM to be readable.
 *
 *      Read words starting at the word offset and store them in the supplied
 *      buffer.
 *
 *      \param buffer  The buffer to read words into.
 *      \param length  The number of words to read.
 *      \param offset  The word offset within the NVM Store to read from.
 *
 *  RETURNS/MODIFIES
 *  Status of operation.
 *
 *---------------------------------------------------------------------------*/

extern void Nvm_Read(uint16* buffer, uint16 length, uint16 offset)
{
    sys_status result;

    /* NvmRead automatically enables the NVM before reading */
    result = NvmRead(buffer, length, offset);

    /* Disable NVM after reading/writing */
    nvmDisable();

    /* If NvmRead fails, report panic */
    if(sys_status_success != result)
    {
        reportPanic(panic_nvm_read);
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      Nvm_Write
 *
 *  DESCRIPTION
 *      Write words to the NVM store after powering it up.
 *
 *      Write words from the supplied buffer into the NVM Store, starting at the
 *      given word offset.
 *
 *      \param buffer  The buffer to write.
 *      \param length  The number of words to write.
 *      \param offset  The word offset within the NVM Store to write to.
 *
 *  RETURNS/MODIFIES
 *      Status of operation.
 *
 *----------------------------------------------------------------------------*/

extern void Nvm_Write(uint16* buffer, uint16 length, uint16 offset)
{
    sys_status result;

    /* NvmWrite automatically enables the NVM before writing */
    result = NvmWrite(buffer, length, offset);

    /* Disable NVM after reading/writing */
    nvmDisable();

    /* If NvmWrite fails, report panic */
    if(sys_status_success != result)
    {
        reportPanic(panic_nvm_write);
    }
}

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------*
 *  NAME
 *      Nvm_Read_Secure_Key
 *
 *  DESCRIPTION
 *      Read secure key.
 *
 *  RETURNS/MODIFIES
 *  Returns pointer to globle secure key pointer if it is updated properly,
 *  otherwise returns NULL
 *
 *---------------------------------------------------------------------------*/

extern uint16* Nvm_Read_Secure_Key(void)
{
    if(secureKeyInfo.state == secureKey_key_available)
        return secureKeyInfo.secure_key;
    else
        return NULL;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmProcessEvent
 *
 *  DESCRIPTION
 *      This function handles the user store messages
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void NvmProcessEvent(msg_t *msg)
{
    userstore_msg_t *us_msg = (userstore_msg_t*)msg;
    switch(msg->header.id)
    {
        case USER_STORE_OPEN_CFM:
        {
            userstore_open_cfm_t *open_cfm = (userstore_open_cfm_t*)&
                                             us_msg->body;

            if(g_nvm_state == NVM_STATE_OPENING && open_cfm->id == g_store_id)
            {
                if(open_cfm->status == STATUS_SUCCESS)
                {
                    /* Read the nvm sanity word */
                    g_nvm_state = NVM_STATE_INITIALISED;
                    nvmReadSanity(g_nvm_sanity, &g_nvm_offset);
                }
                else if(open_cfm->status == sys_status_no_store)
                {
                    g_nvm_state = NVM_STATE_CREATING;
                    if (g_nvm_type == MEM_DEVICE_SMEM)
                    {
                       Userstore_Create(g_store_id, USER_STORE_NVM_SIZE, 8192, MEM_DEVICE_SMEM);
                    }
					else
					{
					    Userstore_Create(g_store_id, 128, 128, MEM_DEVICE_MTP);
				    }
                }
                else
                {
                    /* User store open failed */
                    reportPanic(panic_nvm_open);
                }
            }
            else if(open_cfm->id == NVM_MESH_SECURE_ID && 
                (secureKeyInfo.state == secureKey_init))
            {                
                if(open_cfm->status == STATUS_SUCCESS)
                {
                    /* Read the nvm sanity word */
                    secureKeyInfo.state = secureKey_sanity_read;

                    Userstore_Read(	NVM_MESH_SECURE_ID,
                                    0x0000,
                                    2,
                                    (uint16 *)&secureKeyInfo.sanity_word);
                }
                else if(open_cfm->status == sys_status_no_store)
                {
                    /* 4words has to be reserved for header */
                    Userstore_Create(NVM_MESH_SECURE_ID, 18, 18, MEM_DEVICE_OTP); 
                    /* Userstore_Create(NVM_MESH_SECURE_ID, 512, 4096, MEM_DEVICE_SMEM); */
                }
                else
                {
                    /* User store open failed */
                    reportPanic(panic_nvm_open);
                }
            }
        }
        break;

        case USER_STORE_CREATE_CFM:
        {
            userstore_create_cfm_t *create_cfm = 
                    (userstore_create_cfm_t*)&us_msg->body;
            if(g_nvm_state == NVM_STATE_CREATING && create_cfm->id == g_store_id)
            {
                if(create_cfm->status == STATUS_SUCCESS)
                {
                    g_nvm_fresh = TRUE;
                    g_nvm_state = NVM_STATE_OPENING;
                    Userstore_Open(g_store_id);
                }
                else
                {
                    reportPanic(panic_nvm_create);
                }
            }
            else if(create_cfm->id == NVM_MESH_SECURE_ID)
            {            
                if(create_cfm->status == STATUS_SUCCESS)
                {
                    Userstore_Open(NVM_MESH_SECURE_ID);
                }
                else
                {
                    reportPanic(panic_nvm_create);
                }
            }
        }
        break; 

        case USER_STORE_READ_CFM:
        {
            userstore_read_cfm_t *read_cfm = 
                    (userstore_read_cfm_t*)&us_msg->body;

            if(read_cfm->status == STATUS_SUCCESS)
            {
                switch(secureKeyInfo.state)
                {
                    case secureKey_sanity_read:
                    {
                        if(*((read_cfm->data) + 1) != CSR_MESH_SEC_SANITY_MAGIC)
                        {                            
                            secureKeyInfo.state = secureKey_sanity_write;
                            secureKeyInfo.sanity_word = CSR_MESH_SEC_SANITY_MAGIC;
                            
                            Userstore_Write(NVM_MESH_SECURE_ID, 0x0000, 2, (uint16 *)&secureKeyInfo.sanity_word);
                        } 
                        else
                        {
                            secureKeyInfo.state = secureKey_key_read;

                            Userstore_Read(	NVM_MESH_SECURE_ID,
                                    0x0002,
                                    8,
                                    secureKeyInfo.secure_key);
                        }
                    }
                    break;

                    case secureKey_key_read:
                    {
                        secureKeyInfo.state = secureKey_key_available;

                        /* proceed with nvm initialize procedure */
                        Userstore_Open(g_store_id);
                    }
                    break;
                    
                    default:
                    break;
                }
            }
            else
            {
                secureKeyInfo.state = secureKey_key_unavailable;

                /* Abort the secure key update procedure, 
                   and proceed with nvm initialize procedure */
                Userstore_Open(g_store_id);
            }
        }
        break;

        case USER_STORE_WRITE_CFM:
        {
            userstore_write_cfm_t *write_cfm = 
                    (userstore_write_cfm_t*)&us_msg->body;

            if(write_cfm->status == STATUS_SUCCESS)
            {
                if(secureKeyInfo.state == secureKey_sanity_write)
                {                
                    secureKeyInfo.state = secureKey_key_write;
                    
                    uint16 i;

                    for(i=0; i<8; i++)
                    {
                        secureKeyInfo.secure_key[i] = Random16();
                    }

                    Userstore_Write(NVM_MESH_SECURE_ID, 0x0002, 8, secureKeyInfo.secure_key);
                }
                else if(secureKeyInfo.state == secureKey_key_write)
                {
                    secureKeyInfo.state = secureKey_key_available;

                    /* proceed with nvm initialize procedure */
                    Userstore_Open(g_store_id);
                }
            }
            else
            {
                secureKeyInfo.state = secureKey_key_unavailable;

                /* Abort the secure key update procedure, 
                   and proceed with nvm initialize procedure */
                Userstore_Open(g_store_id);
            }
        }
        break;

        default:
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      Nvm_SetMemType
 *
 *  DESCRIPTION
 *      The default memory type is SMEM. If the application needs different
 *      type of memory this function should be called before Nvm_Init() to
 *      set the memory type.
 *
 *      \param type  The memory device where the new user store will be used.
 *
 *  RETURNS/MODIFIES
 *      Status of operation.
 *
 *----------------------------------------------------------------------------*/

extern sys_status Nvm_SetMemType(memory_type_t type)
{
    sys_status result = sys_status_failed;

    switch(type)
    {
    case MEM_DEVICE_SMEM: /* Fall through */
    case MEM_DEVICE_MTP:
        g_nvm_type = type;
        result = sys_status_success;
        break;
    default:
        result = sys_status_bad_params;
    }

    return result;
}
#endif /* !CSR101x_A05 */
