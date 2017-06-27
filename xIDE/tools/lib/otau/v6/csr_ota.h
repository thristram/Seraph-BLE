/*******************************************************************************
 *  Copyright (c) 2013 - 2016 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.6.1
 *
 *  FILE
 *      csr_ota.h
 *
 *  DESCRIPTION
 *      Header definitions for CSR Over-the-Air Update Application Service
 *      library
 *
 ******************************************************************************/

#ifndef _CSR_OTA_H
#define _CSR_OTA_H

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <bt_event_types.h> /* Type definitions for Bluetooth events */
#include <status.h>         /* System wide status codes */

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/

/* This error is returned when the Host tries to read the value of a CS key
 * that cannot be read.
 */
#define CSR_OTA_KEY_NOT_READ    0x80

/* This version number is returned when the Shared Data could not be read. */
#define CSR_OTA_UNKNOWN_VERSION 0xff

/*=============================================================================*
 *  Public Data Types
 *============================================================================*/

typedef enum {
    csr_ota_boot_loader   = 0x0,  /* Identifies the CSR OTA Update Bootloader
                                   * application
                                   */
    csr_device_app_1      = 0x1,  /* Identifies application 1 on this device */
    csr_device_golden_app = csr_device_app_1,
                                  /* Identifies the Golden Application if
                                   * support is configured in the CSR OTA Update
                                   * Bootloader
                                   */
    csr_device_app_2      = 0x2,  /* Identifies application 2 on this device */
    
    csr_app_unknown       = 0xff  /* Unknown application - used as a return code
                                   * only
                                   */
} csr_application_id;

/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      OtaWriteCurrentApp
 *
 *  DESCRIPTION
 *      Set the current application. 
 *
 *      This function is called when the Host writes to the OTA_CURRENT_APP
 *      characteristic.
 *
 *      Primarily, this function is used to switch the device from the current
 *      application into OTA Update mode.
 *      It is possible to switch directly from one application to another; in
 *      this case, it is not necessary to supply the Host device information and
 *      this can be set to NULL.
 *
 *  PARAMETERS
 *      set_current_app [in]    Requested application index
 *      is_bonded [in]          Whether the Host is bonded with this device
 *      host_address [in]       Bluetooth device address of the Host from which
 *                              the update will be received, or NULL when
 *                              switching directly to another application
 *      diversifier [in]        The diversifier agreed with the bonded Host
 *                              device. This will be 0 (zero) if the Host device
 *                              does not have a random-resolvable address, or
 *                              when switching directly to another application
 *      local_random_address [in]
 *                              If this device is using a random address, then
 *                              pass the address here. The same address will
 *                              then be used by the bootloader.
 *                              If this device is not using a random address,
 *                              then pass NULL.
 *      irk [in]                The IRK shared with the Host device from which
 *                              the update will be received. If this information
 *                              is not available, set all fields to 0 (zero).
 *      service_changed_config [in]
 *                              Indicates whether the Host device has requested
 *                              indications on the Service Changed
 *                              characteristic.
 *
 *  RETURNS
 *      Function status indicating whether the write request was processed.
 *
 *  NOTE
 *      This function claims the I2C bus for the duration of the call.
 *      Applications using the I2C bus may need to re-initialise the bus after
 *      calling this function.
 *
 *      This function writes to EEPROM. It is the caller's responsibility to
 *      ensure that write protection is disabled prior to calling this function,
 *      and restored (if required) when this function returns.
 *----------------------------------------------------------------------------*/
extern sys_status OtaWriteCurrentApp(
                                    csr_application_id  set_current_app,
                                    bool                is_bonded,
                                    TYPED_BD_ADDR_T    *host_address,
                                    uint16              diversifier,
                                    BD_ADDR_T          *local_random_address,
                                    uint16             *irk,
                                    bool                service_changed_config);

/*-----------------------------------------------------------------------------*
 *  NAME
 *      OtaReadCurrentApp
 *
 *  DESCRIPTION
 *      Read the current application.
 *
 *      This function is called when the Host device reads the OTA_CURRENT_APP
 *      characteristic. It allows the Host device to read the index number of
 *      the current application (which will be this application).
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      The index number of the current application.
 *
 *  NOTE
 *      This function claims the I2C bus for the duration of the call.
 *      Applications using the I2C bus may need to re-initialise the bus after
 *      calling this function.
 *----------------------------------------------------------------------------*/
extern csr_application_id OtaReadCurrentApp(void);

/*-----------------------------------------------------------------------------*
 *  NAME
 *      OtaReset
 *
 *  DESCRIPTION
 *      Reset the device.
 *
 *      Typically, this function is called after a call to OtaWriteCurrentApp(),
 *      after disconnection from the Host device is complete.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURNS
 *      This function does not return.
 *----------------------------------------------------------------------------*/
extern void OtaReset(void);

#endif /* _CSR_OTA_H */