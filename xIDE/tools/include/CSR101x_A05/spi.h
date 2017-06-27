/*! \file spi.h
 *
 *  \brief Public header file for functions relating to SPI data transactions.
 *
 * Copyright (c) 2010 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __SPI_H__
#define __SPI_H__

/*! \addtogroup SPI
 *
 * \brief The SPI library functions provide access to SPI devices attached to
 *  a group of PIOs. The CSR1000 acts as SPI Master using SPI Mode 3 operation.
 *
 * @{
 */


/*============================================================================*
Header Files
*============================================================================*/
#include "types.h"


/*============================================================================*
Public Data
*============================================================================*/
/*! @{
 *  \brief PIO selection to be used if the application wants to control the
 *  SPI chip select line(s) instead of the firmware.
 *
 *  This constant can be used when calling \ref SpiInit().
 */
#define SPI_NCS_PIO_UNDEFINED   0xff

/*============================================================================*
Public Function Implementations
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  SpiInit
 */
/*! \brief Initialise the SPI library.
 *
 *  Configure the PIOs required for SPI bus communication, with the CSR1xxx
 *  operating as SPI Master, using SPI Mode 3. All read and write operations
 *  assume 8-bit values. The Most Significant Bit (MSB) will be clocked out
 *  (for writes) or clocked in (for reads) first. The protocol is assumed to be
 *  half-duplex, i.e. when writing bytes, read data is ignored, and vice-versa.
 *
 *  SPI bus transactions are implemented using a software driver to control the
 *  selected PIO lines. Therefore the fundamental SPI clock rate is restricted
 *  by the speed of the processor. In practise we have found the clock rate to
 *  be approximately 470kHz. The clock rate is fixed.
 *
 *  After calling this function all subsequent Spi*() operations will use the
 *  selected PIOs for communicating with the SPI peripheral. If an application
 *  wishes to communicate with two independent SPI devices on the same bus it
 *  must call SpiInit() each time it wishes to switch to the other SPI device.
 *  Typically an implementation would have SPI Clock and SPI Data signals
 *  assigned to common PIOs while the SPI Chip Select signal for each SPI slave
 *  device would have a dedicated PIO. However this arrangement is not mandatory.
 *
 *  Calling this function will clear the delay configuration parameters
 *  (Read Register Delay and Write Termination Delay).
 *
 *  \note All 4 SPI PIO lines must be in the same bank (0-15 or 16-31)
 *
 *  \param mosi_pio PIO (0-31) to use for SPI Data Master Out/Slave In.
 *  \param miso_pio PIO (0-31) to use for SPI Data Master In/Slave Out.
 *  \param clk_pio  PIO (0-31) to use for SPI Clock.
 *  \param ncs_pio  PIO (0-31) to use for SPI Slave Chip Select (active low),
 *                  or \ref SPI_NCS_PIO_UNDEFINED to control chip select from the application.
 *
 *  \return TRUE if bus is initialised or FALSE if there was an error
 */
/*---------------------------------------------------------------------------*/
extern bool SpiInit(uint16 mosi_pio, uint16 miso_pio, uint16 clk_pio, uint16 ncs_pio);


/*----------------------------------------------------------------------------*
 *  SpiConfigReadRegisterDelay
 */
/*! \brief Set the Read Register Delay configuration parameter.
 *
 *  The Read Register Delay is the minimum period in microseconds that the SPI
 *  driver will wait between the rising edge of SCLK for the last bit of the
 *  register address byte to the falling edge of SCLK for the first bit of data
 *  read. The driver has internal delays that account for about 3.5us, so if a
 *  SPI slave needs more time than that to prepare a response, the app should
 *  set this delay to a non-zero value.
 *
 *  This value is only applied between writing the register address and reading
 *  the value(s) (including burst register reads). Burst Reads do no insert any
 *  additional delays between reading of individual register values.
 *
 *  The delay parameter is reset to 0 each time \ref SpiInit() is called.
 *
 *  \param delay  Extra delay (in microseconds).
 */
/*---------------------------------------------------------------------------*/
extern void SpiConfigReadRegisterDelay(uint16 delay);


/*----------------------------------------------------------------------------*
 *  SpiConfigWriteTerminationDelay
 */
/*! \brief Set the Write Termination Delay configuration parameter.
 *
 *  The Write Termination Delay is the minimum period in microseconds that the
 * chip select line will be held active after a write completes. Some devices
 * may require a long period to allow the data to be stored before CS goes
 * inactive.
 *
 * With no delays specified the SPI driver code has an approximate delay of 8us
 * due to internal processing, although this is not guaranteed.
 *
 *  \param delay  Extra delay (in microseconds).
 */
/*---------------------------------------------------------------------------*/
extern void SpiConfigWriteTerminationDelay(uint16 delay);


/*----------------------------------------------------------------------------*
 *  SpiConfigWriteIntervalDelay
 */
/*! \brief Set the Write Interval Delay configuration parameter.
 *
 *  The Write Interval Delay is the minimum period in microseconds between
 * subsequent byte transfers under SpiWrite. Some devices may require a long
 * period to allow the data to be processed before the next byte.
 *
 * With no delays specified the SPI driver code has an approximate delay of 5us
 * due to internal processing, although this is not guaranteed.
 *
 *  \param delay  Extra delay (in microseconds).
 */
/*---------------------------------------------------------------------------*/
extern void SpiConfigWriteIntervalDelay(uint16 delay);



/*----------------------------------------------------------------------------*
 *  SpiWriteByte
 */
/*! \brief Write one byte of data to a SPI peripheral device.
 *
 *  The SPI slave device is selected, the byte is clocked out, and then the
 *  device is de-selected.
 *
 *  \param byte The byte to write.
 */
/*---------------------------------------------------------------------------*/
extern void SpiWriteByte(uint8 byte);


/*----------------------------------------------------------------------------*
 *  SpiWrite
 */
/*! \brief Write a sequence of bytes of data to a SPI peripheral device.
 *
 *  The SPI slave device is selected, the array of bytes is clocked out, and
 *  then the device is de-selected.
 *
 *  \param out_buffer Array of bytes to write to the SPI device.
 *  \param length Number of bytes in the array to write.
 *
 *  \return The number of bytes written (which will always be \c 'length').
 */
/*---------------------------------------------------------------------------*/
extern uint16 SpiWrite(const uint8* out_buffer, uint16 length);


/*----------------------------------------------------------------------------*
 *  SpiWriteRegister
 */
/*! \brief Write to a register on a SPI peripheral device.
 *
 *  The SPI slave device is selected, the register address is clocked out, the
 *  register value is clocked out, and then the device is de-selected.
 *
 *  \param reg_address The address of the register to be written.
 *  \param reg_value The value to write to the register.
 */
/*---------------------------------------------------------------------------*/
extern void SpiWriteRegister(uint8 reg_address, uint8 reg_value);


/*----------------------------------------------------------------------------*
 *  SpiReadByte
 */
/*! \brief Read one byte of data from a SPI peripheral device.
 *
 *  The SPI slave device is selected, the byte is clocked in, and then the
 *  device is de-selected.
 *
 *  \return The byte value read from the SPI device.
 */
/*---------------------------------------------------------------------------*/
extern uint8 SpiReadByte(void);


/*----------------------------------------------------------------------------*
 *  SpiRead
 */
/*! \brief Read a sequence of bytes of data from a SPI peripheral device and
 *  copy the data into the supplied buffer.
 *
 *  The SPI slave device is selected, the array of bytes is clocked in, and
 *  then the device is de-selected.
 *
 *  \param in_buffer Array of bytes to store data read from the SPI device.
 *  \param length Number of bytes to read.
 *
 *  \return The number of bytes read (which will always be \c 'length').
 */
/*---------------------------------------------------------------------------*/
extern uint16 SpiRead(uint8* in_buffer, uint16 length);


/*----------------------------------------------------------------------------*
 *  SpiReadRegister
 */
/*! \brief Read from a register on a SPI peripheral device.
 *
 *  The SPI slave device is selected, the register address is clocked out, then
 *  the register value is clocked in, and then the device is de-selected.
 *
 *  There is a delay of about 3.5us between writing the register address and
 *  reading the values. Some of this delay is fixed due to the internal
 *  processing carried out by the SPI device driver. However, as some devices
 *  may require longer delays to prepare the burst response, the application
 *  can use the function \ref SpiConfigReadRegisterDelay() to set an additional
 *  delay. During this delay the SPI slave device remains selected.
 *
 *  \param reg_address The address of the register to be written.
 *
 *  \return The register value read from the SPI device.
 */
/*---------------------------------------------------------------------------*/
extern uint8 SpiReadRegister(uint8 reg_address);


/*----------------------------------------------------------------------------*
 *  SpiReadRegisterBurst
 */
/*! \brief Burst read multiple registers on a SPI peripheral device.
 *
 *  The SPI slave device is selected, the register address is clocked out,
 *  multiple register values are clocked in, and then the device is de-selected.
 *
 *  This procedure assumes that the SPI slave device will prepare the Burst
 *  Read response, such that from the first register address it will return a
 *  device-specific series of register values within one SPI transaction, with
 *  no further address writes required between values.
 *
 *  There is a delay of about 3.5us between writing the register address and
 *  reading the values. Some of this delay is fixed due to the internal
 *  processing carried out by the SPI device driver. However, as some devices
 *  may require longer delays to prepare the burst response, the application
 *  can use the function \ref SpiConfigReadRegisterDelay() to set an additional
 *  delay. During this delay the SPI slave device remains selected.
 *
 *  The parameter \c 'toggle_clk' should normally be left set to FALSE. It is
 *  provided for compatibility with certain SPI devices. It adds an extra clock
 *  cycle between writing the register address and reading the first register
 *  value.
 *
 *  \param reg_address The first register address.
 *  \param in_buffer Array of bytes to store register values read from the SPI device.
 *  \param length Number of bytes to read.
 *  \param toggle_clk Configuration parameter to toggle clock after write
 *
 *  \return The number of bytes read (which will always be \c 'length').
 */
/*---------------------------------------------------------------------------*/
extern uint16 SpiReadRegisterBurst(uint8 reg_address,
                                   uint8* in_buffer,
                                   uint16 length,
                                   bool toggle_clk);

/*! @} */
#endif /* __SPI_H__ */
