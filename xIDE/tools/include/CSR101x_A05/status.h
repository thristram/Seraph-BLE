/*! \file status.h
 *  \brief  CSR1000 System-wide status codes
 *
 *  Consisting of standard HCI codes (those in the range 0x00 - 0x3F) and
 *  extended codes (0x0100 upwards). Extended codes are grouped according to
 *  functional area, so for example all I2C status codes are in one group and
 *  all
 *
 * Copyright (c) 2011 - 2016 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __STATUS_H__
#define __STATUS_H__

#include "types.h"
#include "hci_error_codes.h"
#include "status_deprecated.h"

/*!  \addtogroup STAT
 * @{
 */

/*! \name Status Groups
 *
 * Within the complete set of status codes defined across the system, there are
 * groups of codes corresponding to specific functional areas. For example, all
 * I2C status codes are grouped together and given a common prefix. These
 * constants define the "base" code for each group of codes. These base codes
 * are then used in the definition of the \ref status enumerated type.
 *
 * @{
 */
#define STATUS_GROUP_NVM        0x0100      /*!< NVM Store Status Codes */
#define STATUS_GROUP_I2C        0x0200      /*!< I2C Status Codes (incl. I2C EEPROM) */
#define STATUS_GROUP_SPI        0x0300      /*!< SPI Status Codes (incl. SPI Flash) */
#define STATUS_GROUP_SKM        0x0400      /*!< Security Manager Status Codes */

#define STATUS_GROUP_L2CAP      0x0800      /*!< L2CAP Status Codes */
#define STATUS_GROUP_LS         0x0900      /*!< Link Supervisor Status Codes */
#define STATUS_GROUP_GATT       0x0A00      /*!< GATT Status Codes */
#define STATUS_GROUP_SM         0x0B00      /*!< Security Manager Status Codes */
/*! @} */


/*! \brief HCI and extended system-wide status codes
 *
 *  Please refer to the BlueTooth specifications V4.0,
 *  volume 2, part D for details of the HCI error codes
 *  in the range 0x00 - 0x3F.
 *
 *  The extended error codes (those above 0x0100) are documented here.
 */
typedef enum
#ifdef _WIN32
: uint16
#endif
{
    sys_status_success = 0x0000,            /*!< Generic "success" status code */

    /* HCI: TBD */

    /*! \name NVM Status Codes
     * @{
     */
    nvm_status_empty = STATUS_GROUP_NVM,    /*!< NVM is initialised but NVM Store is currently empty/unused */
    nvm_status_needs_erase,                 /*!< NVM Store only has one block and is full (SPI Flash only) */
    nvm_status_invalid_configuration,       /*!< NVM configuration in CS keys is invalid / not supported */
    nvm_status_not_initialised,             /*!< NVM Store is not initialised */
    nvm_status_invalid_offset,              /*!< Offset parameter is invalid or length runs past end of NVM Store */
    nvm_status_invalid_buffer,              /*!< Buffer address parameter is invalid / NULL */
    /*! @} */

    /*! \name I2C Status Codes
     *
     *  The first four status codes indicate successful completion of a request,
     *  or that the request is proceeding, or that the request could not be
     *  started due to some other part of the system using the I2C device driver
     *  or hardware.
     *
     *  The remaining status codes (starting i2c_status_fail_) indicate an
     *  unexpected failure condition, typically detected by the hardware. For
     *  example this could be caused by an attempt to communicate with an I2C
     *  device that is not powered up.
     * @{
     */
    i2c_status_waiting = STATUS_GROUP_I2C,  /*!< Transaction is continuing */
    i2c_status_firmware_busy,               /*!< Firmware is busy with another request */
    i2c_status_hardware_busy,               /*!< Hardware is busy with another request */
    i2c_status_controller_disabled,         /*!< The controller is currently disabled (see \ref I2cEnable) */
    i2c_status_fail,                        /*!< General failure */
    i2c_status_fail_nacked,                 /*!< NACK received from I2C slave */
    i2c_status_fail_bus_busy,               /*!< I2C bus was busy (external I2C bus master?) */
    i2c_status_fail_arb_lost,               /*!< Bus arbitration lost during transaction */
    i2c_status_fail_timeout,                /*!< Transaction failed to complete within timeout period */
    i2c_status_fail_inactive,               /*!< No active transaction to complete */
    i2c_status_fail_unknown,                /*!< Unknown error */
    i2c_status_fail_write_poll_timeout,     /*!< Timeout while polling the EEPROM for completion of a write cycle */
    /*! @} */

    /*! \name SPI Status Codes
     * @{
     */
    spi_status_waiting = STATUS_GROUP_SPI,  /*!< Transaction is continuing */
    spi_status_hardware_busy,               /*!< Hardware is busy with another request */
    spi_status_erase_pending,               /*!< Erase operation has started but not completed */
    spi_status_fail,                        /*!< General failure */
    spi_status_fail_timeout,                /*!< Transaction failed to complete within timeout period */
    spi_status_page_overflow,               /*!< A write was requested with more data than fits in one page */
    /*! @} */

    /*! \name L2CAP Status Codes
     * @{
     */
    l2cap_status_invalid_conn_state = STATUS_GROUP_L2CAP,
                                            /*!< Invalid connection state */
    l2cap_status_conn_disallowed,           /*!< Connection disallowed */
    l2cap_status_conn_not_ongoing,          /*!< Connection not ongoing for L2CAP client (ATT/SMP) */
    l2cap_status_buffer_full,               /*!< Buffer full; cannot process data */
    /*! @} */

    /*! \name Link Supervisor Status Codes
     * @{
     */
    ls_status_limited_advertising_timeout = STATUS_GROUP_LS,
                                            /*!< Limited advertising time out */

    /*! \name Generic Attribute Profile
     * @{
     */
    gatt_status_invalid_handle = STATUS_GROUP_GATT+1,
                                            /*!< The attribute handle given was not valid */
    /*! \cond This is for firmware use only. Firmware uses this for bounds check. */
    /* ATT error code can't go less than gatt_status_invalid_handle, so hardcode gatt_status_valid_att_err_first */
    gatt_status_valid_att_err_first         = gatt_status_invalid_handle,
    /*! \endcond */
    gatt_status_read_not_permitted,         /*!< The attribute cannot be read */
    gatt_status_write_not_permitted,        /*!< The attribute cannot be written */
    gatt_status_invalid_pdu,                /*!< The attribute PDU was invalid */
    gatt_status_insufficient_authentication,/*!< The attribute requires an authentication before it can be read or written */
    gatt_status_request_not_supported,      /*!< Target device doesn't support request */
    gatt_status_invalid_offset,             /*!< Offset specified was past the end of the long attribute */
    gatt_status_insufficient_authorization, /*!< The attribute requires authorization before it can be read or written */
    gatt_status_prepare_queue_full,         /*!< Too many prepare writes have been queued */
    gatt_status_attr_not_found,             /*!< No attribute found within the given attribute handle range. */
    gatt_status_not_long,                   /*!< This attribute cannot be read or written using the Read Blob Request
                                             *   or Write Blob Requests. */
    gatt_status_insufficient_encr_key_size, /*!< The Encryption Key Size used for encrypting this link is insufficient. */
    gatt_status_invalid_length,             /*!< The attribute value length is invalid for the operation. */
    gatt_status_unlikely_error,             /*!< The attribute request that was requested has encountered an error
                                             *   that was very unlikely, and therefore could not be completed as
                                             *   requested. */
    gatt_status_insufficient_encryption,    /*!< The attribute requires encryption before it can be read or written */
    gatt_status_unsupported_group_type,     /*!< The attribute type is not a supported grouping attribute as defined by a
                                             *   higher layer specification. */
    gatt_status_insufficient_resources,     /*!< Insufficient Resources to complete the request. */
    /*! \cond This is for firmware use only. Firmware uses this for bounds check. */
    gatt_status_valid_att_err_last = gatt_status_insufficient_resources,
    /*! \endcond */

    gatt_status_device_not_found = STATUS_GROUP_GATT+0x33,
                                            /*!< Error to indicate that request to LS can not be completed because 
                                             *   the device entity is not found */
    gatt_status_sign_failed,                /*!< Attribute signing failed. */
    gatt_status_busy,                       /*!< Operation can't be done now. */
    gatt_status_timeout,                    /*!< Current operation timed out. */
    gatt_status_invalid_mtu,                /*!< Invalid MTU */
    gatt_status_invalid_uuid,               /*!< Invalid UUID type */
    gatt_status_success_more,               /*!< Operation was successful, and more responses will follow */
    gatt_status_success_sent,               /*!< Indication sent, awaiting confirmation from the client */
    gatt_status_invalid_cid,                /*!< Invalid connection identifier */
    gatt_status_invalid_db,                 /*!< Attribute database is invalid */
    gatt_status_db_full,                    /*!< Attribute server database is full */
    gatt_status_invalid_permissions = STATUS_GROUP_GATT+0x3F,
                                            /*!< Attribute permissions are not valid */

    gatt_status_invalid_operation,          /*!< Operation requested by HL is not valid */
    gatt_status_invalid_param_value,        /*!< Invalid parameter value passed */
    gatt_status_data_validation_failed,     /*!< Data validation failed during Reliable Writes procedure */
    gatt_status_irq_proceed,                /*!< The application has authorised the Read or Write access
                                             *   which should now proceed through the database */

    gatt_status_app_mask = STATUS_GROUP_GATT+0x80,
                                            /*!< Start of Application error codes that
                                             *   may be defined by a higher layer specification.
                                             *   This value should be ORed with application status
                                             *   codes in the range 0x80 - 0x9F to ensure they are
                                             *   sent over the air to the peer device. */

    gatt_status_app_first_code = gatt_status_app_mask,
                                            /*!< First valid GATT Application Error code */
    gatt_status_app_last_code  = STATUS_GROUP_GATT+0x9F,
                                            /*!< Last valid GATT Application Error code */

    /*! \cond This is for firmware use only. Firmware uses this for bounds check. */
    /* This is a bit messy because newer profile and service error codes will be less than 
     * gatt_status_valid_prof_err_first. Change gatt_status_valid_prof_err_first in case 
     * more profile or service error codes get added. */
    gatt_status_valid_prof_err_first        = STATUS_GROUP_GATT + 0xFD,
    /*! \endcond */
    gatt_status_desc_improper_config        = STATUS_GROUP_GATT + 0xFD,
                                            /*!< Client Characteristic Configuration descriptor is not
                                            * configured according to the requirements of the profile or service*/
    gatt_status_proc_in_progress,
                                            /*!< Profile or service request cannot be serviced because 
                                            * an operation that has been previously triggered is still in progress*/
    gatt_status_att_val_oor,
                                            /*!< attribute value is out of range as defined by a profile or service specification*/
    /*! \cond This is for firmware use only. Firmware uses this for bounds check. */
    /* Since GATT error codes can't go beyond this, hardcode gatt_status_valid_prof_err_last */
    gatt_status_valid_prof_err_last         = gatt_status_att_val_oor,
    /*! \endcond */


    /*! \name Security Manager Status Codes
     * @{
     */
    sm_status_reserved = STATUS_GROUP_SM,   /*!< Reserved value */
    sm_status_passkey_entry_failed,         /*!< Passkey input cancelled */
    sm_status_oob_not_available,            /*!< Peer has no OOB data */
    sm_status_authentication_requirements,  /*!< Unauthenticated pairing is not acceptable */
    sm_status_confirm_value_failed,         /*!< Passkey input wrong */
    sm_status_pairing_not_supported,        /*!< Peer is currently unable to perform pairing */
    sm_status_encryption_key_size,          /*!< The negotiated encryption strength is not acceptable */
    sm_status_command_not_supported,        /*!< Peer does not support this operation */
    sm_status_unspecified_reason,           /*!< Something else went wrong */
    sm_status_repeated_attempts,            /*!< Peer is experiencing excessive failed pairings; please wait */
    sm_status_invalid_parameters,           /*!< Incorrect or Invalid arguments have been supplied */

    sm_status_last_standardised = STATUS_GROUP_SM+0x7F,
                                            /*!< Subsequent SM status definitions are internal and not standardised */
    sm_status_timeout,                      /*!< Peer did not respond */

    sys_status_invalid = 0xFFFF             /*!< Generic "invalid" status code */

    /*! @} */


} sys_status;

/*! @} */


#endif  /* __STATUS_H__ */
