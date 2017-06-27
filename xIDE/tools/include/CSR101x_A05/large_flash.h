/******************************************************************************/
/*! \file large_flash.h
 *
 *  \brief This module provides access to large (> 512-kbit) SPI Flash devices.
 *      Currently the following devices are supported:
 *
 *      Adesto AT25DF011 (1-Mbit)
 *      Macronix MX25L4006E (4-Mbit)
 *
 *      Two PIOs must be allocated to permit access to the device. See
 *      CS-324434: Interfacing Large Serial Flash and EEPROM Application Note
 *      for more details.
 *
 * Copyright (c) 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */
/******************************************************************************/

#ifndef __LARGE_FLASH_H__
#define __LARGE_FLASH_H__

#include "types.h"

/*! \addtogroup NVM
 *
 * @{
 */

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/*! \brief Range of block sizes that may be erased.
 *         Not all sizes are supported on all devices:
 *         AT25DF011:  Supports all sizes
 *         MX25L4006E: large_spi_erase_4KB supported
 *                     large_spi_erase_32KB erases 64KB blocks
 *                     large_spi_erase_256B not supported
 */
typedef enum
{
    large_spi_erase_4KB,        /*!< 4KB block */
    large_spi_erase_32KB,       /*!< 32KB block (64KB on MX25L4006E) */
    large_spi_erase_256B        /*!< 256B page (not supported on MX25L4006E) */
} large_spi_flash_erase_size;

/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashInit
 */
/*! \brief  Configure the Large SPI Flash library.
 *
 *  \param  mosi  [in] PIO to use for SPI MOSI line, or
 *                     SPI_FLASH_DEFAULT_PIO for the default
 *  \param  miso  [in] PIO to use for SPI MISO line
 *  \param  clk   [in] PIO to use for SPI clock line
 *  \param  ncs   [in] PIO to use for SPI slave select line, or
 *                     SPI_FLASH_DEFAULT_PIO for the default
 *  \param  pow   [in] PIO to use for NVM power control, or
 *                     SPI_FLASH_DEFAULT_PIO for the default
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void LargeSpiFlashInit(uint16 mosi, uint16 miso, uint16 clk, uint16 ncs,
                              uint16 pow);

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashEnable
 */
/*! \brief  Enable the PIOs for the Large SPI Flash library.
 *          Assumes memory is already powered up and NVM controller is not busy.
 *          Disable the NVM controller and set PIOs.
 *
 *  PARAMETERS
 *      None
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void LargeSpiFlashEnable(void);

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashDisable
 */
/*! \brief  Disable the Large SPI Flash library.
 *          Assumes the NVM controller is already initialised.
 *          Enables the NVM controller and restore PIOs.
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void LargeSpiFlashDisable(void);

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashRead
 */
/*! \brief  Read data from the Large SPI Flash device.
 *
 *  \param  address  [in] 32-bit address to start read from
 *  \param  length   [in] Number of octets to read
 *  \param  data     [in] Data buffer to hold data read
 *
 *  \return spi_status_hardware_busy: Another operation in progress
 *          spi_status_fail_timeout:  Read operation timed out
 *          sys_status_success:       Success
 */
/*---------------------------------------------------------------------------*/
extern sys_status LargeSpiFlashRead(uint32 address, uint16 length, uint16 *data);

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashWrite
 */
/*! \brief  Write data to the Large SPI Flash device.
 *
 *  \param  address [in] 24-bit address to write to
 *  \param  length  [in] Number of octets to write (maximum LSF_PAGE_SIZE)
 *  \param  data    [in] Data buffer to write to Large SPI Flash device
 *
 *  \return spi_status_hardware_busy: Another operation in progress
 *          spi_status_page_overflow: Too much data to write in one operation
 *          spi_status_fail_timeout:  Write operation timed out
 *          sys_status_success:       Success
 */
/*---------------------------------------------------------------------------*/
extern sys_status LargeSpiFlashWrite(uint32 address, uint16 length, const uint16 *data);

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashEraseBlock
 */
/*! \brief  Erase a single block of the Large SPI Flash device.
 *
 *  \param  size    [in] Size of block to erase.
 *                       Refer \ref large_spi_flash_erase_size
 *                       Note: Not all Flash devices accept all sizes.
 *  \param  address [in] 32-bit address in block to erase (typically set to the
 *                       start of the block, though some Flash devices accept
 *                       any address in the block).
 *  \param  wait    [in] TRUE:  Wait for the erase operation to complete
 *                       FALSE: Return as soon as the erase command has been issued
 *
 *  \return spi_status_hardware_busy: Another operation in progress
 *          spi_status_fail:    Unknown block size requested
 *          spi_status_fail_timeout:  Erase operation timed out
 *          sys_status_success:       Success
 */
/*---------------------------------------------------------------------------*/
extern sys_status LargeSpiFlashEraseBlock(large_spi_flash_erase_size size,
                                          uint32 address, bool wait);

/*----------------------------------------------------------------------------*
 *  LargeSpiFlashEraseWaitComplete
 */
/*! \brief  Wait for an erase operation to complete.
 *
 *  \return spi_status_fail_timeout:  Erase operation timed out
 *          sys_status_success:       Success
 */
/*---------------------------------------------------------------------------*/
extern sys_status LargeSpiFlashEraseWaitComplete(void);

/*! @} */
#endif /* __LARGE_FLASH_H__ */

