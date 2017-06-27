/*! \file i2c.h
 *
 *  \brief Public header file for functions relating to I2C bus transactions.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2010-2011
 */

#ifndef __I2C_H__
#define __I2C_H__

/*! \addtogroup I2C
 *
 *  \brief The I2C library functions provide access to I2C devices attached to
 *  the dedicated I2C bus or any group of PIOs. The CSR1000 acts as an I2C Bus
 *  Master. The library supports generic access to any device that appears as
 *  a standard I2C EEPROM (i.e. real EEPROM devices or other peripherals that
 *  provide a memory-mapped register interface). For devices that use a
 *  non-standard protocol, raw access to the I2C bus is also available,
 *  allowing the application to generate discrete I2C conditions such as
 *  START, RESTART, STOP, ACKs and NACKs, and send or receive individual bytes.
 *
 * @{
 */

/*============================================================================*
Header Files
*============================================================================*/
#include "types.h"
#include "time.h"
#include "pio.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/*! @{
 *  \brief PIO selection to use dedicated I2C bus pins instead of general
 *  purpose PIOs.
 *
 *  This constant can be used when calling \ref I2cInit() if the application
 *  wants to use the reserved I2C pins for the I2C clock or data signals.
 */
#define I2C_RESERVED_PIO                0xFF
/*! @} */

/*! @{
 *  \brief PIO value indicating that the I2C device driver should not manage
 *  the power for the I2C device(s).
 *
 *  This constant can be used when calling \ref I2cInit().
 */
#define I2C_POWER_PIO_UNDEFINED         0xFF
/*! @} */

/*! @{
 *  \brief This constant can be used when setting the EEPROM Write Cycle Time
 *  with I2cEepromSetWriteCycleTime() to indicate that the I2C driver should
 *  poll the EEPROM for write completion instead of waiting a fixed period.
 *
 *  The EEPROM Write Cycle Time is the period of time to wait at the end
 *  of an EEPROM write transaction to allow the write to complete within the
 *  device. Some EEPROMs support a mode where they will not ACKnowledge any
 *  further activity during this internal write cycle, which allows the EEPROM
 *  driver to poll them and return immediately after the write has completed
 *  rather than waiting a fixed length of time (which, depending on the device,
 *  may be quite pessimistic in normal operating conditions).
 *
 *  \warning To avoid a software lock-up due to an unresponsive EEPROM, the
 *  driver will abort polling and return if the EEPROM has not generated an
 *  ACK within 64ms. If this happens status \ref i2c_status_fail_write_poll_timeout
 *  will be returned.
 */
#define I2C_EEPROM_POLLED_WRITE_CYCLE   0
/*! @} */


/*! \name I2C Clock Configurations
 *
 *  \brief Standard I2C clock configurations for 100kHz and 400kHz operation,
 *  optimised for EEPROM devices.
 *
 *  These constants can be used when calling \ref I2cConfigClock() if the
 *  values are suitable for use with the I2C device.
 *
 *  \note Customers are advised to always confirm device timing requirements in
 *  the datasheet for the I2C device used in their product. These default clock
 *  values are specified for use with the parts used in CSR10xx reference
 *  designs. However other devices may have different minimum times for the high
 *  and low periods of SCL. In particular some devices require more time low than
 *  high, hence the values defined here do not result in a perfectly square clock
 *  signal at 400kHz, in order to optimise the timing for these devices. (Whereas
 *  at 100kHz it is possible to use the same periods for SCL high and SCL low,
 *  resulting in a square clock signal).
 */
/**@{*/
#define I2C_SCL_100KBPS_HIGH_PERIOD     78  /*!< SCL high period for 100kHz clock */
#define I2C_SCL_100KBPS_LOW_PERIOD      78  /*!< SCL low  period for 100kHz clock */
#define I2C_SCL_400KBPS_HIGH_PERIOD     15  /*!< SCL high period for 400kHz clock */
#define I2C_SCL_400KBPS_LOW_PERIOD      21  /*!< SCL low  period for 400kHz clock */
/**@}*/

/*! \brief Raw I2C commands */
typedef enum
{
    i2c_cmd_send_start =        0x00,       /*!< Send START condition */
    i2c_cmd_send_restart =      0x01,       /*!< Send RESTART condition */
    i2c_cmd_send_stop =         0x02,       /*!< Send STOP condition */
    i2c_cmd_wait_ack =          0x03,       /*!< Wait for an ACK */
    i2c_cmd_send_ack =          0x04,       /*!< Send an ACK */
    i2c_cmd_send_nack =         0x05,       /*!< Send a NACK */
    i2c_cmd_tx_data =           0x06,       /*!< Internal use only */
    i2c_cmd_rx_data =           0x07        /*!< Internal use only */
} i2c_command;

/*! \name I2C Timeouts
 *
 *  \brief Default timeouts for I2C operations.
 */
/**@{*/
#define I2C_WAIT_ACK_TIMEOUT    (1UL * MILLISECOND)
#define I2C_WAIT_CMD_TIMEOUT    (1UL * MILLISECOND)
/**@}*/


/*============================================================================*
Public Data
*============================================================================*/


/*============================================================================*
Public Function Implementations
*============================================================================*/

/*----------------------------------------------------------------------------*
 *  I2cInit
 */
/*! \brief Initialise the I2C library.
 *
 *  Configure the PIOs required for I2C bus communication, with the CSR1000
 *  operating as I2C Bus Master. The I2C bus can be assigned to any of the
 *  32 general purpose PIO pins (PIO[31:0] or to the reserved I2C bus pins by
 *  specifying a PIO of \ref I2C_RESERVED_PIO for the \c sda_pio and \c scl_pio
 *  parameters. If the application selects PIO[31:16] (only available on a
 *  CSR1001 chip) while running on a CSR1000 chip no error will be returned,
 *  but of course the bus will not be externally available.
 *
 *  The \c 'power_pio' parameter is used to assign an optional PIO to manage the
 *  power rail to the I2C device. If the I2C device is permanently powered, or
 *  if power is managed directly by the application, then this PIO value should
 *  be set to \ref I2C_POWER_PIO_UNDEFINED to disable it.
 *
 *  The \c 'pull' parameter sets the default pulling mode for the I2C pins. The
 *  application can change the pulling mode at any time after calling I2cInit
 *  by calling \ref PioSetPullModes() for general PIOs or \ref PioSetI2CPullMode()
 *  for the reserved I2C pins. If the I2C bus is initialised to use general PIOs
 *  then the reserved I2C pins will have their pulling mode set to
 *  pio_i2c_pull_mode_no_pulls.
 *
 *  After calling this function all subsequent I2C operations will use the
 *  selected PIOs for communicating with the I2C peripheral.
 *
 *  Calling this function will reset the I2C clock configuration parameters so
 *  that devices are clocked at the standard rate of 100kHz.
 *
 *  \param sda_pio PIO (0-31, 0xFF) to use for I2C Serial Data.
 *  \param scl_pio PIO (0-31, 0xFF) to use for I2C Serial Clock.
 *  \param power_pio PIO (0-31, 0xFF) to use for I2C power control
 *  \param pull The default PIO pull mode to use for the I2C bus.
 */
/*---------------------------------------------------------------------------*/
extern void I2cInit(uint8 sda_pio, uint8 scl_pio, uint8 power_pio, pio_pull_mode pull);


/*----------------------------------------------------------------------------*
 *  I2cConfigClock
 */
/*! \brief Set the high and low periods of the I2C clock.
 *
 *  The periods are given in 16th of a microsecond. When using a standard 100kHz
 *  or 400kHz period, the constants \ref I2C_SCL_100KBPS_HIGH_PERIOD,
 *  \ref I2C_SCL_100KBPS_LOW_PERIOD, \ref I2C_SCL_400KBPS_HIGH_PERIOD, and
 *  \ref I2C_SCL_400KBPS_LOW_PERIOD can be used to supply the correct values.
 *
 *  \param scl_high High period of I2C clock
 *  \param scl_low Low period of I2C clock
 */
/*---------------------------------------------------------------------------*/
extern void I2cConfigClock(uint8 scl_high, uint8 scl_low);


/*----------------------------------------------------------------------------*
 *  I2cEnable
 */
/*! \brief Enable or disable the I2C controller.
 *
 *  \param enable TRUE to enable the controller or FALSE to disable it
 */
/*---------------------------------------------------------------------------*/
extern void I2cEnable(bool enable);


/*----------------------------------------------------------------------------*
 *  I2cReady
 */
/*! \brief Test to see if the current I2C transaction has completed.
 *
 *  \return TRUE if I2C transaction has completed or FALSE if it is ongoing.
 */
/*---------------------------------------------------------------------------*/
extern bool I2cReady(void);


/*----------------------------------------------------------------------------*
 *  I2cWaitReady
 */
/*! \brief Busy-wait until the current I2C transaction has completed.
 */
/*---------------------------------------------------------------------------*/
extern void I2cWaitReady(void);


/*----------------------------------------------------------------------------*
 *  I2cReset
 */
/*! \brief Reset the I2C controller.
 *
 *  Resets the I2C controller without waiting for any current read or write
 *  commands to finish. This is advised only as part of the initialisation
 *  procedure, or to recover from an incorrect state in the I2C controller.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cReset(void);


/*----------------------------------------------------------------------------*
 *  I2cEepromSetWritePageSize
 */
/*! \brief Set the EEPROM page size for write operations.
 *
 *  The default page size is 128 bytes. An application only needs to call this
 *  function if it wishes to use an alternate page size. A page size of zero is
 *  not allowed / ignored.
 */
/*---------------------------------------------------------------------------*/
extern void I2cEepromSetWritePageSize(uint16 page_size);


/*----------------------------------------------------------------------------*
 *  I2cEepromSetWriteCycleTime
 */
/*! \brief Set the EEPROM write cycle time (in microseconds).
 *
 *  The write cycle time is the amount of time required by the I2C EEPROM after
 *  a STOP condition to complete the write activity. The default time is 5ms.
 */
/*---------------------------------------------------------------------------*/
extern void I2cEepromSetWriteCycleTime(uint16 cycle_time);


/*----------------------------------------------------------------------------*
 *  I2cEepromRead
 */
/*! \brief Read bytes from a standard I2C EEPROM.
 *
 *  The data read from the device is stored as packed data in the buffer
 *  pointed to by \c data. Despite storing packed data, the length must be the
 *  number of \b bytes to read. Therefore, if the length is odd the last word of
 *  the buffer will have an undefined value in the Most-Significant Byte (MSB).
 *
 *  The data is read from the EEPROM within one I2C transaction. Therefore the
 *  EEPROM must support Sequential Reads. It is also assumed that the EEPROM
 *  does not have any page size restrictions on the Sequential Read (i.e. if
 *  requested it can support reading the entire memory contents at once). If
 *  sequential reads are not supported, the application can use this function
 *  to read one byte at a time.
 *
 *  Non-blocking reads are supported. If the \c wait parameter is set to FALSE
 *  the function will return as soon as the I2C hardware has been configured to
 *  start receiving. The application can then use \ref I2cReady() to check if
 *  the read operation has completed, although it must also always call
 *  \ref I2cEepromReadComplete() to properly terminate the procedure within the
 *  driver.
 *
 *  \note The device address is a 7-bit value, and does not include the read/
 *  write indicator bit.
 *
 *  \param device   I2C device address
 *  \param address  Address in EEPROM memory to start reading the data from
 *  \param wait     Wait for read to complete if TRUE
 *  \param length   The number of \b bytes to read
 *  \param data     Pointer to storage for the data that is read
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
sys_status I2cEepromRead(uint16 device,
                         uint16 address,
                         bool wait,
                         uint16 length,
                         uint16* data);


/*----------------------------------------------------------------------------*
 *  I2cEepromReadComplete
 */
/*! \brief Finish writing bytes to the I2C bus.
 *
 *  This function must be called if the application previously started an I2C
 *  byte write using \ref I2cEepromRead() but didn't wait for it to finish. An
 *  error will be returned if no read has been started. If the read operation
 *  has already completed then the function will return immediately, otherwise
 *  it will wait until the operation has completed and then return.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cEepromReadComplete(void);


/*----------------------------------------------------------------------------*
 *  I2cEepromWrite
 */
/*! \brief Write bytes to a standard I2C EEPROM.
 *
 *  This function performs a complete I2C transaction, from START condition,
 *  sending the device address, writing the address in memory, writing the
 *  data bytes and generating a STOP condition.
 *
 *  The data to be written to the device should be passed to the function as
 *  packed data in the buffer pointed to by \c data. Despite being packed, the
 *  length must be the number of \b bytes to write. If the length is odd the
 *  last byte will be taken from the Least-Significant Byte (LSB) of the last
 *  word of the buffer.
 *
 *  The EEPROM Page Size parameter (see \ref I2cEepromSetWritePageSize) defines
 *  the maximum number of bytes that will be written to an EEPROM within one
 *  I2C transaction (START condition to STOP condition). After each transaction
 *  the driver will wait for Write Cycle Time (see \ref I2cEepromSetWriteCycleTime)
 *  to elapse before it starts the next transaction or completes.
 *
 *  If the start address for the write does not lie on a page boundary or the
 *  data to be written crosses page boundaries, the driver will also ensure
 *  that write is broken down into multiple writes, with one write per page.
 *  For example, with a Page Size of 128, a write starting at address 100 for
 *  40 bytes would result in two I2C transactions: the first writing 28 bytes
 *  from 100 - 127, and the second writing 12 bytes from 128 to 139.
 *
 *  \note The device address is a 7-bit value, and does not include the read/
 *  write indicator bit.
 *
 *  \warning The EEPROM Write procedure does not currently support non-blocking
 *  operation. However to ensure forward compatibility applications are
 *  recommended to always set the \c wait parameter to TRUE. This will mean
 *  that if a future release supports non-blocking writes that existing
 *  applications will continue to work as expected.
 *
 *  \param device   I2C device address
 *  \param address  Address in EEPROM memory to start writing the data to
 *  \param wait     [Not currently used - application \b must set to \b TRUE]
 *  \param length   The number of \b bytes to write
 *  \param data     Pointer to the data to be written
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cEepromWrite(uint16 device,
                                 uint16 address,
                                 bool wait,
                                 uint16 length,
                                 const uint16* data);


/*----------------------------------------------------------------------------*
 *  I2cRawCommand
 */
/*! \brief Send a raw I2C command to I2C controller.
 *
 *  This function is used to generate single I2C events on the bus. This allows
 *  the application to communicate with I2C devices other than standard EEPROMs.
 *
 *  The device driver can optionally either wait for the command to complete,
 *  or can return immediately, with the command pending. If the command is left
 *  pending then the application should call I2cRawComplete() later on to
 *  complete and return the status of the command.
 *
 *  There are also a number of helper macros defined to make it simpler to
 *  generate the various I2C commands, using default timeout periods. These
 *  macros may be more obvious to use than directly calling I2cRawCommand.
 *
 *  \warning If the application requests a \ref i2c_cmd_wait_ack command but no
 *  ACK is received within the timeout period then the I2C transaction will have
 *  completed. The application must either then terminate the raw command
 *  sequence with I2cRawTerminate() or start a new sequence with I2cRawStart().
 *
 *  \param cmd The command to send
 *  \param wait TRUE if the function should wait for the transaction to complete
 *  \param timeout Timeout period (in microseconds) to wait for the transaction to complete
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawCommand(i2c_command cmd, bool wait, uint16 timeout);


/*----------------------------------------------------------------------------*
 *  I2cRawCommandXXX
 */
/*! \name I2C Raw Commands
 *
 *  \brief Send raw commands to the I2C controller.
 *
 *  These macros are used to send START, RESTART, STOP, ACK, or NACK over the
 *  I2C bus, or wait for an ACK for the addressed slave.
 *
 *  \param wait TRUE if the function should wait for the transaction to complete
 */
/*---------------------------------------------------------------------------*/
/**@{*/
/*! Send a START condition */
#define I2cRawStart(wait)       I2cRawCommand(i2c_cmd_send_start, wait, I2C_WAIT_CMD_TIMEOUT)
/*! Send a RESTART condition */
#define I2cRawRestart(wait)     I2cRawCommand(i2c_cmd_send_restart, wait, I2C_WAIT_CMD_TIMEOUT)
/*! Send a STOP condition */
#define I2cRawStop(wait)        I2cRawCommand(i2c_cmd_send_stop, wait, I2C_WAIT_CMD_TIMEOUT)
/*! Wait for an ACK */
#define I2cRawWaitAck(wait)     I2cRawCommand(i2c_cmd_wait_ack, wait, I2C_WAIT_ACK_TIMEOUT)
/*! Send an ACK condition */
#define I2cRawSendAck(wait)     I2cRawCommand(i2c_cmd_send_ack, wait, I2C_WAIT_CMD_TIMEOUT)
/*! Send a NACK condition */
#define I2cRawSendNack(wait)    I2cRawCommand(i2c_cmd_send_nack, wait, I2C_WAIT_CMD_TIMEOUT)
/**@}*/


/*----------------------------------------------------------------------------*
 *  I2cRawComplete
 */
/*! \brief Wait for the last issued I2C command to complete.
 *
 *  If the command completes within the timeout period, then the controller
 *  status will be returned, otherwise \ref i2c_status_fail_timeout will be
 *  returned.
 *
 *  \param timeout Timeout period (in microseconds) to wait for the transaction to complete
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawComplete(uint16 timeout);


/*----------------------------------------------------------------------------*
 *  I2cRawTerminate
 */
/*! \brief Ends the sequence of I2C raw commands.
 *
 *  If an application uses both raw commands and the atomic read/write functions
 *  to access an I2C device it must properly terminate the raw command sequence
 *  by calling this function before it can use the byte read/write functions
 *  again. This is to ensure that the device is in a known state for the
 *  read/write functions.
 *
 *  This function only updates internal driver state - it does not generate any
 *  further transactions on the I2C bus. Therefore the application must ensure
 *  that it properly completes a raw command sequence with an I2C STOP condition
 *  before calling this function.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawTerminate(void);


/*----------------------------------------------------------------------------*
 *  I2cRawReadByte
 */
/*! \brief Read a byte of data from the I2C bus.
 *
 *  This command assumes the I2C slave device has been put into a state where
 *  it is ready to transmit a byte. The function will block until the read
 *  operation has completed.
 *
 *  \warning This function does NOT generate the I2C ACK/NACK condition after
 *  reading the byte. If the application needs to read one or more bytes and
 *  generate standard ACK/NACK conditions after each byte then \ref I2cRawRead()
 *  may be more suitable.
 *
 *  \param data Pointer to storage for the byte that is read.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawReadByte(uint8* data);


/*----------------------------------------------------------------------------*
 *  I2cRawRead
 */
/*! \brief Read data from the I2C bus, ACKing received bytes and NACKing the
 *   last byte.
 *
 *  This command assumes the I2C slave device has been put into a state where
 *  it is ready to transmit bytes. The function will block until the read
 *  operation has completed, and all bytes have been read.
 *
 *  \param data     Pointer to storage for the bytes that are read.
 *  \param length   The number of bytes to read.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawRead(uint8* data, uint16 length);


/*----------------------------------------------------------------------------*
 *  I2cRawWriteByte
 */
/*! \brief Write a byte of data to the I2C bus.
 *
 *  This command assumes the I2C slave device has been put into a state where
 *  it is ready to receive a byte. The function will block until the write
 *  operation has completed.
 *
 *  \param data     The byte that is to be written.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawWriteByte(uint8 data);


/*----------------------------------------------------------------------------*
 *  I2cRawWrite
 */
/*! \brief Write data to the I2C bus, waiting for ACK after each byte.
 *
 *  This command assumes the I2C slave device has been put into a state where
 *  it is ready to receive bytes. The function will block until the write
 *  operation has completed, and all bytes have been transmitted.
 *
 *  \param data     Pointer to the bytes that are to be written.
 *  \param length   The number of bytes to write.
 *
 *  \return Status of operation
 */
/*---------------------------------------------------------------------------*/
extern sys_status I2cRawWrite(const uint8* data, uint16 length);


#endif /* __I2C_H__ */
