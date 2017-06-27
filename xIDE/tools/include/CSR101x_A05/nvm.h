/*! \file nvm.h
 *
 *  \brief Application services for the non-volatile storage area within the
 *  CSR1000 boot device.
 *
 * Copyright (c) 2010 - 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
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
 *  Refer to the description of CS Key nvm_start_address and \refer NvmSetI2cEepromDeviceAddress
 *  usage in case EEPROM is greater than 0.5 Mbits (64 kilobytes).
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
 *  The CS key "SPI flash block size" should be set to the size in *bytes* of a
 *  single erasable block within the SPI Flash boot device. This figure can be found
 *  in the data-sheet for the selected device. If nvm_num_spi_blocks is 2 then it's
 *  mandatory to set this CS key equal to 'flash erase sector size'.
 *
 *  \note Scenarios:
 *  1. spi_flash_block_size is smaller than the actual erase block size and nvm_num_spi_blocks is 1
 *  \n => Some of the storage in the flash block is unused(and thus wasted).
 *  2. spi_flash_block_size is smaller than the actual erase block size and nvm_num_spi_blocks is 2
 *  \n => Erase operations will span multiple NVM blocks and data corruption will occur.
 *  3. spi_flash_block_size is larger than the actual erase block size,
 *  \n =>An NVM erase will only erase part of the block, and subsequent writes to the unerased
 *       area will fail. This scenario occurs whether nvm_num_spi_blocks is 1 or 2.
 *
 *  The CS key "NVM num SPI blocks" specifies the number of consecutive erasable
 *  SPI Flash memory blocks that are available for the NVM.
 *  \n If two blocks are available, then when the first block becomes full, the
 *  SPI Flash NVM manager will be able to automatically copy the information
 *  into the second (spare) block before the first block is erased. This will
 *  reduce the chance of data corruption if the power fails during a write
 *  operation.
 *  \n If only one block is available, then when it becomes full the
 *  application will need to erase the block (and optionally copy existing
 *  data back into it) before new information can be stored. This introduces
 *  a small window in which power loss could result in loss of data.
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

/*----------------------------------------------------------------------------*
 *  NvmSetI2cEepromDeviceAddress
 */
/*! \brief  Set the EEPROM device address during I2C operation.
 *
 *  This sets EEPROM device address excluding the Read/Write bit. Firmware will
 *  left shift this value by 1 and set the R/W bit according to the requested
 *  operation. The default device address is 0x50.
 *
 *  \param address  Device address
 */
/*----------------------------------------------------------------------------*/
extern void NvmSetI2cEepromDeviceAddress(uint16 address);

/*! @} */
#endif /* __NVM_H__ */

