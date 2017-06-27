/*! \file nvm.h
 *
 *  \brief Application services for the non-volatile storage area within the
 *  CSR1000 boot device.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd 2009-2011.
 */

#ifndef __NVM_H__
#define __NVM_H__

/*! \addtogroup NVM
 *
 *  \brief The NVM library functions provide access to a reserved block (the
 *  "NVM Store") within the CSR1000 boot device, to allow the application to
 *  read and write configuration settings, security information, etc. at
 *  runtime.
 *  
 *  The NVM Store contents are retained across power cycles. The library can
 *  access either an I2C EEPROM or a SPI Flash device - any device that the
 *  CSR1000 can boot from is supported.
 *
 *  The I2C manager does not include any write protection on the NVM Store
 *  (i.e. an application write cannot be considered an "atomic" operation)
 *  therefore the application should be aware that loss of power during a
 *  write cycle may result in corruption of the contents of the NVM Store
 *  (e.g. due to having only partially-written the new data).
 *
 *  The SPI NVM Manager uses multiple write regions and additional control
 *  information to offer better protection of the integrity of the NVM Store
 *  during updates.
 *
 * @{
 */


/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief The minimum size allowed for the NVM Store, in words. */
#define NVM_MINIMUM_SIZE            32

/*! \brief The default value of an erased location within the NVM Store.
 *
 *  This value may be device-dependent.
 */
#define NVM_DEFAULT_ERASED_WORD     0xFFFF


/*============================================================================*
Public Function Implementations
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  NvmConfigureI2cEeprom
 */
/*! \brief Configure the NVM manager to use an I2C EEPROM for the NVM Store.
 *
 *  CSR1000 supports both I2C EEPROM and SPI Flash boot devices, therefore the
 *  application must initialise the NVM manager with the appropriate type of
 *  device. This function is used to set up access to an I2C EEPROM device.
 *
 *  This function is typically called once, either as part of the application
 *  initialisation (in AppInit()) or just before the first read, write, or erase
 *  access to the NVM device.
 *
 *  The CS key "I2C EEPROM Initialisation Time" defines the time required by
 *  the I2C EEPROM device after it has powered on until it is ready to operate.
 *  This figure can typically be found in the datasheet for the selected device.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status NvmConfigureI2cEeprom(void);


/*----------------------------------------------------------------------------*
 *  NvmConfigureSpiFlash
 */
/*! \brief Configure the NVM manager to use a SPI Flash for the NVM Store.
 *
 *  CSR1000 supports both I2C EEPROM and SPI Flash boot devices, therefore the
 *  application must initialise the NVM manager with the appropriate type of
 *  device. This function is used to set up access to a SPI Flash device.
 *
 *  This function is typically called once, either as part of the application
 *  initialisation (in AppInit()) or just before the first read, write, or erase
 *  access to the NVM device.
 *
 *  The CS key "SPI Flash Initialisation Time" defines the time required by
 *  the SPI Flash device after it has powered on until it is ready to operate.
 *  This figure can typically be found in the datasheet for the selected device.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status NvmConfigureSpiFlash(void);


/*----------------------------------------------------------------------------*
 *  NvmDisable
 */
/*! \brief  Disable the NVM manager and power off the underlying storage device.
 *
 *  This function can be used to turn off the power to the NVM storage device.
 *  The NVM driver will retain the configuration state after powering off the
 *  device. This allows the application to subsequently call NvmRead(),
 *  NvmWrite(), and NvmErase() without having to call NvmConfigureI2cEeprom()
 *  or NvmConfigureSpiFlash() again.
 *
 *  \return Status of operation
*/
/*---------------------------------------------------------------------------*/
extern sys_status NvmDisable(void);


/*----------------------------------------------------------------------------*
 *  NvmRead
 */
/*! \brief  Read words from the NVM Store.
 *
 *  Read words starting at the word offset, and store them in the supplied
 *  buffer.
 *
 *  If the NVM device is currently disabled (via a previous call to NvmDisable())
 *  it will be automatically re-enabled before the Read operation is performed.
 *
 *  \param buffer  The buffer to read words into
 *  \param length  The number of words to read
 *  \param offset  The word offset within the NVM Store to read from
 *
 *  \return Status of operation
*/
/*---------------------------------------------------------------------------*/
extern sys_status NvmRead(uint16* buffer, uint16 length, uint16 offset);


/*----------------------------------------------------------------------------*
 *  NvmWrite
 */
/*! \brief  Write words to the NVM Store.
 *
 *  Write words from the supplied buffer into the NVM Store, starting at the
 *  given word offset
 *
 *  If the NVM device is currently disabled (via a previous call to NvmDisable())
 *  it will be automatically re-enabled before the Write operation is performed.
 *
 *  \param buffer  The buffer to write
 *  \param length  The number of words to write
 *  \param offset  The word offset within the NVM Store to write to
 *
 *  \return Status of operation
 */
/*----------------------------------------------------------------------------*/
extern sys_status NvmWrite(const uint16* buffer, uint16 length, uint16 offset);


/*----------------------------------------------------------------------------*
 *  NvmSize
 */
/*! \brief  Return the size in words of the NVM Store.
 *
 *  This function can be called prior to initialising the NVM device via
 *  NvmConfigureI2cEeprom() or NvmConfigureSpiFlash(). If it is called while
 *  the NVM device is disabled called, it will *not* re-enable the NVM device.
 *
 *  \param size_of_storage  Pointer to integer to size in
 *
 *  \return Status of operation
 */
/*----------------------------------------------------------------------------*/
extern sys_status NvmSize(uint16* size_of_storage);


/*----------------------------------------------------------------------------*
 *  NvmErase
 */
/*! \brief  Erase the contents of the NVM Store.
 *
 *  For SPI Flash devices, if the erase_all parameter is TRUE then all firmware
 *  control information is also erased. The erase_all parameter is not used on
 *  an I2C EEPROM.
 *
 *  If the NVM device is currently disabled (via a previous call to NvmDisable())
 *  it will be automatically re-enabled before the Erase operation is performed.
 *
 *  \param erase_all  SPI Flash only: erase all firmware control information as well
 *
 *  \return Status of operation
 */
/*----------------------------------------------------------------------------*/
extern sys_status NvmErase(bool erase_all);


/*! @} */
#endif /* __NVM_H__ */

