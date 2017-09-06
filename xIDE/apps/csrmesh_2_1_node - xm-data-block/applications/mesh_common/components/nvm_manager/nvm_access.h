/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 */
 /*! \file nvm_access.h
 *  \brief Header definitions for NVM usage
 */

#ifndef __NVM_ACCESS_H__
#define __NVM_ACCESS_H__

/*! \addtogroup NVM_Access
 * @{
 */

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <status.h>
#ifndef CSR101x_A05
#include <userstore.h>
#include <userstore_msg.h>
#endif /* !CSR101x_A05 */


/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*
 * In order to future proof applications that can be updated over the air,
 * the NVM subsystem provides a "versioned key" interface.
 * Each application can maintain a block of data in NVM, identified by a
 * unique identifier. The block of data must be initialsed with a specific
 * length. In order to allow an application to modify its data structures
 * in future, the data block length should be created with some padding.
 * By default the services provided in this SDK have rounded up their
 * NVM requirements to 16 or 32. In effect, the NVM users are reserving
 * space for future requirements.
 * The Versioned Key system requires a version number to be written along
 * with the Key ID and Length information.
 * The applications can Write a new Versioned Key block and also validate
 * a Versioned Key block at run time. If the block in NVM matches the Key ID
 * and Length, but not the version, a callback function is called which
 * allows the application to migrate the stored data to a new data format.
 * Since the header containing the version information is not written until
 * after the data has been written, it is strongly recommended that the
 * application does not change the meaning of data structure contents between
 * application versions except by appending new structure members. In this
 * way, if there is a power failure between writing the new data and writing
 * the header, upon system reboot the NVM system will call the migrate function
 * a second time. This is benign, since the data meaning will not have changed.
 * The Key Identifiers are specified with the enum nvm_data_id.
 * Applications should use the USER_DATA identifiers, since Qualcomm may update
 * the identifiers between 0x8000 and 0xc000 between SDK versions without warning.
 */

/*! \brief NVM key identifiers */
typedef enum  {
    NVM_ACCESS_KEY_INVALID = 0,
    USER_DATA_KEY_0_ID = 0x0001,
    USER_DATA_KEY_1_ID,
    USER_DATA_KEY_2_ID,
    USER_DATA_KEY_3_ID,
    USER_DATA_KEY_4_ID,
    USER_DATA_KEY_5_ID,
    USER_DATA_KEY_6_ID,
    USER_DATA_KEY_7_ID,
    HID_SERVICE_ID = 0x8001,
    HID_BOOT_SERVICE_ID,
    GAP_SERVICE_ID,
    GATT_SERVICE_ID,
    BATTERY_SERVICE_ID,
    DEV_INFO_SERVICE_ID,
    CM_SECURITY_MGR_ID,
    BMS_SERVICE_ID,
    SCAN_PARAM_SERVICE_ID,
    IMM_ALERT_SERVICE_ID,
    CSR_AUDIO_SERVICE_ID,
    IR_SERVICE_ID,
    IRCDFS_SERVICE_ID,
    A4WP_SERVICE_ID,
    BEACON_SERVICE_ID,
    EDDYSTONE_SERVICE_ID,
    BP_SERVICE_ID,
    CSC_SERVICE_ID,
    RSC_SERVICE_ID,
    CTX_SERVICE_ID,
    ENV_SENSOR_SERVICE_ID,
    GAIA_SERVICE_ID,
    GLUCOSE_SENSOR_SERVICE_ID,
    HT_SERVICE_ID,
    HR_SENSOR_SERVICE_ID,
    LINK_LOSS_SERVICE_ID,
    MESH_CONTROL_SERVICE_ID,
    PAIRING_DELETION_SERVICE_ID,
    SPECIAL_AUTH_SERVICE_ID,
    TPS_SERVICE_ID,
    TRUNK_CONTROL_SERVICE_ID,
    VEHICLE_INFO_SERVICE_ID,
    BODY_COMPOSITION_SERVICE_ID,
    WEIGHT_SCALE_SERVICE_ID,
    USER_DATA_SERVICE_ID,
    SERIAL_SERVICE_ID,
    TX_POWER_SERVICE_ID,
    ALERT_NOTIFICATION_CLIENT_SERVICE_ID,
    ANCS_CLIENT_SERVICE_ID,
    BATTERY_CLIENT_SERVICE_ID,
    DEV_INFO_CLIENT_SERVICE_ID,
    GATT_CLIENT_SERVICE_ID,
    HR_CLIENT_SERVICE_ID,
    LOCK_UNLOCK_CLIENT_SERVICE_ID,
    NDC_CLIENT_SERVICE_ID,
    PDS_CLIENT_SERVICE_ID,
    PAS_CLIENT_SERVICE_ID,
    RTU_CLIENT_SERVICE_ID,
    RSC_CLIENT_SERVICE_ID,
    SERIAL_CLIENT_SERVICE_ID,
    SAS_CLIENT_SERVICE_ID,
    TCS_CLIENT_SERVICE_ID,
    DVIS_CLIENT_SERVICE_ID,
    CTP_CLIENT_SERVICE_ID,
    CURRENT_TIME_CLIENT_SERVICE_ID,
    IMM_ALERT_CLIENT_SERVICE_ID,
    GAIA_OTAU_SERVICE_ID,
}nvm_data_id_t;


/*! \brief NVM key descriptor */
typedef struct {
    nvm_data_id_t id;  /*! \brief NVM Key identifier */
    uint16 len; /*! \brief NVM Key length */
} nvm_cs_header_t;

/*! \brief Versioned header information */
typedef struct {
    nvm_cs_header_t header;
    uint16 version;  /*! \brief Application specific version number */
} nvm_versioned_header_t;

/*! \brief Macros to round a number up to the next 16 or 32
 *         after including the Versioned Header size */
#define NVM_PAD_ROUND_UP_TO_16(x) ((sizeof(nvm_versioned_header_t) + x + 15) & ~0x0f)
#define NVM_PAD_ROUND_UP_TO_32(x) ((sizeof(nvm_versioned_header_t) + x + 31) & ~0x1f)


/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
#ifndef NVM_DONT_PAD

/*! \brief Function to convert data in the NVM to a newer version
 *
 * Function that can be called during the NVM version validation process,
 * initiated by calling Nvm_ValidateVersionedHeader().
 * The function should return FALSE if the NVM data is totally incompatible
 * with the new function.
 * If it is possible to migrate the data between the versions then
 * the application should use a Nvm_Read() and Nvm_Write() to reformat the data
 * to meet the new version. The application should not write more than
 * nvm_data_length words into NVM from nvm_app_data_offset.
 * \param[in] version_in_nvm Version of data recorded in the NVM block
 * \param[in] nvm_app_data_offset Offset in NVM of the application data
 * \param[in] nvm_data_length Length of managed NVM data block, including the
 *            NVM versioned key header
 * \returns True is the data was updated and written to NVM. The version
 *          information will be updated if the function returns True.
 */
typedef bool (*migrate_handler_t)(uint16 version_in_nvm,
                               uint16 nvm_app_data_offset,
                               uint16 nvm_data_length);

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
 * \param[in] migrate_handler If not NULL, a function which is called to
 *            modify the NVM data a new application specific format.
 * \returns True if the requested versioned header was found;
 *          nvm_offset will be updated to the next NVM offset
 */
bool Nvm_ValidateVersionedHeader(uint16 *nvm_offset,
                                 nvm_data_id_t key_id, uint16 data_len, uint16 version,
                                 migrate_handler_t migrate_handler);

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
                                uint16 data_len, uint16 version);
#endif

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------
 *  Nvm_Init
 *----------------------------------------------------------------------------*/
/*! \brief Initialises the NVM
 *
 * This function initialises the NVM
 * \param[in] id Store ID
 * \param[in] nvm_sanity NVM sanity word
 * \param[in] nvm_offset Pointer to NVM offset
 * \returns Nothing
 *
 */
extern void Nvm_Init(store_id_t id, uint16 nvm_sanity, uint16 *nvm_offset);
#else
/*----------------------------------------------------------------------------
 *  Nvm_Init
 *----------------------------------------------------------------------------*/
/*! \brief Initialises the NVM
 *
 * This function initialises the NVM
 * \param[in] nvm_sanity NVM sanity word
 * \param[in] nvm_offset Pointer to NVM offset
 * \returns Nothing
 *
 */
extern void Nvm_Init(uint16 nvm_sanity, uint16 *nvm_offset);
#endif /* !CSR101x_A05 */

/*----------------------------------------------------------------------------
 *  AppNvmReady
 *----------------------------------------------------------------------------*/
/*! \brief Called when the NVM initialisation is done
 *
 * This function is called when the NVM is initialised
 * \param[in] nvm_fresh Boolean variable to check if NVM has been not been written before
 * \param[out] nvm_offset Current NVM offset
 * \returns Nothing
 *
 */
extern void AppNvmReady(bool nvm_fresh, uint16 nvm_offset);

/*----------------------------------------------------------------------------
 *  Nvm_Read
 *----------------------------------------------------------------------------*/
/*! \brief Read words from the NVM store after preparing the NVM to be readable
 *
 * This function reads words from the NVM store after preparing the NVM to be readable
 * \param[out] buffer The buffer to read words into
 * \param[in] length The number of words to read
 * \param[in] offset The word offset within the NVM Store to read from
 * \returns Nothing
 *
 */
extern void Nvm_Read(uint16* buffer, uint16 length, uint16 offset);

/*----------------------------------------------------------------------------
 *  Nvm_Write
 *----------------------------------------------------------------------------*/
/*! \brief Write words to the NVM store after preparing the NVM to be writable
 *
 * This function writes words to the NVM store after preparing the NVM to be writable
 * \param[in] buffer The buffer to write
 * \param[in] length The number of words to write
 * \param[in] offset The word offset within the NVM Store to write to
 * \returns Nothing
 *
 */
extern void Nvm_Write(uint16* buffer, uint16 length, uint16 offset);

extern void Nvm_Disable(void);

extern uint16* Nvm_Read_Secure_Key(void);

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------
 *  NvmProcessEvent
 *----------------------------------------------------------------------------*/
/*! \brief Handles the user store messages
 *
 * This function handles the user store messages
 * \param[in] msg Pointer to incoming message event
 * \returns Nothing
 *
 */
extern void NvmProcessEvent(msg_t *msg);

/*----------------------------------------------------------------------------
 *  Nvm_SetMemType
 *----------------------------------------------------------------------------*/
/*! \brief Sets the memory device type for the user store
 *
 * This function sets the memory device type for the user store
 * \param[in] memory_type_t Memory device type to set
 * \returns Status
 *
 */
extern sys_status Nvm_SetMemType(memory_type_t type);
#endif /* !CSR101x_A05 */

/*!@} */

#endif /* __NVM_ACCESS_H__ */
