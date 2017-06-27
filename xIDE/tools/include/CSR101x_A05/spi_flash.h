/*! \file spi_flash.h
 *
 *  \brief Public header file for functions relating to SPI Flash memory
 *  access.
 *
 * Copyright (c) 2010 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

/*! \addtogroup SPI
 *
 * \brief The SPI Flash library functions provide access to read, write, and
 *  erase SPI Flash devices attached to the same SPI bus as the CSR1000 boot
 *  device. The CSR1000 acts as SPI Master using SPI Mode 0 operation.
 *
 * @{
 */

/*============================================================================*
Header Files
*============================================================================*/
#include "types.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/*! @{
 *  \brief Default PIO should be used for SPI Flash power or chip select signal.
 *
 *  This constant can be used when calling \ref SpiFlashInit() if the
 *  application wants to use the default PIO for the power or chip select
 *  signals. For example, a design with two SPI Flash devices may use the same
 *  PIO for the boot device and the secondary device (to save a PIO) and use
 *  discrete chip select signals for each device.
 */
#define SPI_FLASH_DEFAULT_PIO           0xFF
/*! @} */

/*! @{
 *  \brief PIO value indicating that the SPI Flash device driver should not
 *  manage the power for the SPI Flash device.
 *
 *  This constant can be used when calling \ref SpiFlashInit().
 */
#define SPI_FLASH_POWER_PIO_UNDEFINED   0xFF
/*! @} */

/*! \brief SPI Flash block erase sizes */
typedef enum
{
    spi_erase_4KB,                          /*!< Erase a 4KB block*/
    spi_erase_32KB,                         /*!< Erase a 32KB block*/
} spi_erase_size;


/*============================================================================*
Public Data
*============================================================================*/


/*============================================================================*
Public Function Implementations
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  SpiFlashInit
 */
/*! \brief Initialise the SPI Flash library.
 *
 *  Configure the NVM hardware for communication with a SPI Flash device. The
 *  SPI Flash must be on the same SPI bus as the boot flash (it is therefore
 *  not possible to use this interface to communicate with a SPI Flash device
 *  if the CSR1000 was booted from an I2C EEPROM).
 *
 *  The hardware has a fixed 8MHz SPI clock for all transactions.
 *
 *  \param ncs_pio  PIO (0-15) to use for SPI Flash Chip Select (active low).
 *  \param power_pio  PIO (0-15) to use for SPI Flash power.
 */
/*---------------------------------------------------------------------------*/
extern void SpiFlashInit(uint16 ncs_pio, uint16 power_pio);


/*----------------------------------------------------------------------------*
 *  SpiFlashDisable
 */
/*! \brief Disable & power-down the selected SPI Flash device.
 */
/*---------------------------------------------------------------------------*/
extern void SpiFlashDisable(void);


/*----------------------------------------------------------------------------*
 *  SpiFlashRead
 */
/*! \brief Read data from the selected SPI Flash device.
 *
 *  The data is read in units of bytes. The application should supply a pointer
 *  to a packed array (that is, a uint16*). This is to optimise the amount of
 *  RAM required to read data from a SPI Flash device. If an odd number of bytes
 *  are requested, the unused byte of the final packed word will be set to 0x00.
 *
 *  Due to fixed internal timeouts, it is recommended that no more than 4KB
 *  (4096 bytes) is read in a single transaction.
 *
 *  \param address  Address in SPI Flash memory to start reading the data from.
 *  \param length  The number of \b bytes to read.
 *  \param data  Pointer to storage for the data that is read (packed array)
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status SpiFlashRead(uint16 address, uint16 length, uint16* data);


/*----------------------------------------------------------------------------*
 *  SpiFlashWrite
 */
/*! \brief Write data to the selected SPI Flash device.
 *
 *  The data is written in units of bytes. The application should supply a
 *  pointer to a packed array (that is, a uint16*). This is to optimise the
 *  amount of RAM required to write data to a SPI Flash device.
 *
 *  The caller must ensure that no more than one full page (often 256 bytes,
 *  although dependent on selected device parameters) is written in a single
 *  transaction.
 *
 *  \param address  Address in SPI Flash memory to start writing the data to
 *  \param length   The number of \b bytes to write
 *  \param data     Pointer to the data to be written (packed array)
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status SpiFlashWrite(uint16 address, uint16 length, const uint16* data);


/*----------------------------------------------------------------------------*
 *  SpiFlashEraseBlock
 */
/*! \brief Erase a single block of the SPI Flash memory device.
 *
 *  The block is determined by the address passed in. Typically this will be
 *  the first address of the block, although devices may allow any address
 *  within the block to be used. The size of the block to be erased is
 *  determined by the size parameter. The driver currently supports erasing
 *  4KB or 32KB blocks.
 *
 *  The caller can specify whether or not the driver waits for the erase
 *  operation to finish before returning. With typical block erase times of up
 *  to a second, this allows the application to continue doing other processing
 *  while the erase operation completes. If the driver does not wait, then the
 *  caller MUST ensure that the erase has finished by later calling
 *  SpiFlashEraseComplete() before performing any other operations with the
 *  SPi Flash driver.
 *
 *  \param size     Size of block to erase
 *  \param address  Address corresponding to block to be erased
 *  \param wait     TRUE if the driver should wait for the erase to finish
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status SpiFlashEraseBlock(spi_erase_size size, uint16 address, bool wait);


/*----------------------------------------------------------------------------*
 *  SpiFlashEraseWaitComplete
 */
/*! \brief Waits for an erase operation to complete.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status SpiFlashEraseWaitComplete(void);


#endif /* __SPI_FLASH_H__ */
