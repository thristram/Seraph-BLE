/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file
 *
 *  \brief
 *
 *  \details
 */
/****************************************************************************/

#ifndef __I2C_COMMS_H__
#define __I2C_COMMS_H__

/*! \addtogroup I2C_COMMS
 *
 *  \brief
 *
 *  \details
 *
 *
 * @{
 */

/*==========================================================================*
    SDK Header Files
*===========================================================================*/
#ifndef CSR101x_A05
#include <i2c_common.h>
#endif /* !CSR101x_A05 */


/*==========================================================================*
    Public Function Prototypes
*===========================================================================*/

/****************************************************************************/
/*!
 * \brief Initialise the I2C hardware.
 *
 *  \param sda      The serial data PIO pin. Valid values are chip/package
 *                      dependent and detailed in the accompanying data sheet.
 *  \param scl      The serial clock PIO pin. Valid values are chip/package
 *                      dependent and detailed in the accompanying data sheet.
 *  \param pullmode Whether the chip provides hard pull-up or not. Options are
 *                  \ref pio_i2c_pull_mode for CSR101x and \ref i2c_pull_mode_t for
 *                  CSR102x.
 *
 *  \retval ::STATUS_SUCCESS    The I2C peripheral has been initalised successfully.
 *  \retval ::STATUS_BAD_PARAMS One or more of the parameters are out of range.
 *  \retval ::STATUS_FAIL       Other error condition, such as the I2C already being open.
 *
 *  \details <b>Detailed description</b>\n
 *      This function must be called once to initialise the I2C bus.
 *
 *      The I2C bus is set to the defaults, see ref \I2cOpen
 *
 *      The I2C bus must be closed by a call to I2cClose() before any
 *      subsequent call to (re)initialise the I2C in either master or slave mode.
 */
/****************************************************************************/
#if defined (CSR101x_A05)
extern sys_status I2c_Init(uint8 sda, uint8 scl, pio_i2c_pull_mode pullmode);
#else
extern sys_status I2c_Init(pio_t sda, pio_t scl, i2c_pull_mode_t pullmode);
#endif /* CSR101x_A05 */

/****************************************************************************/
/*!
 * \brief This function acquires the I2C bus
 *
 * \note  This function is not used CSR102x.
 *
 * \return TRUE if successful.
 */
/****************************************************************************/
extern bool I2c_Acquire(void);


/****************************************************************************/
/*!
 * \brief This function releases the I2C bus
 *
 * \note  This function is not used CSR102x.
 */
/****************************************************************************/
extern void I2c_Release(void);


/****************************************************************************/
/*!
 * \brief Reads 8-bit register from the specified device.
 *
 *  \param    slave_addr            The 7 or 10-bit I2C address of the slave
 *                                  device (the device being accessed).
 *  \param    register_address      The 8-bit register address of the slave device
 *  \param    register_value        The 8-bit data to transmit to the slave device
 *
 *  \retval  ::sys_status_success   The operation is complete.
 *  \retval  ::sys_status_busy      The operation cannot proceed because the I2C
 *                                  peripheral is currently busy with a previous request.
 *  \retval  ::sys_status_failed    The operation cannot proceed because the I2C
 *                                  peripheral is not open or is not in Master mode.
 */
/****************************************************************************/
extern sys_status I2c_ReadRegister(i2c_address_t slave_addr,
        uint8 register_address, uint8 *register_value);


/****************************************************************************/
/*!
 * \brief Reads a contiguous sequence of registers from the specified device.
 *
 *  \param    slave_addr            The 7 or 10-bit I2C address of the slave
 *                                  device (the device being accessed).
 *  \param    register_address      The 8-bit register address of the slave device
 *  \param    rx_length             The number of bytes to be read.
 *  \param    rx_buffer             A uint16 pointer to the receive buffer.
 *                                  <b>Note</b>: The received bytes are packed into uint16's.
 *                                  <b>Note</b>: It is the callers responsibility to verify that the buffer
 *                                  is big enough to hold <c>rx_length</c> bytes.
 *                                  This is packed in little endian format e.g. the following array
 *                                  will have received the bytes 1,2,3,4 {0x0201,0x0403}
 *
 *  \retval  ::sys_status_success   The operation is complete.
 *  \retval  ::sys_status_busy      The operation cannot proceed because the I2C
 *                                  peripheral is currently busy with a previous request.
 *  \retval  ::sys_status_failed    The operation cannot proceed because the I2C
 *                                  peripheral is not open or is not in Master mode.
 */
/****************************************************************************/
extern sys_status I2c_ReadRegisters(i2c_address_t slave_addr,
        uint8 register_address, uint16 rx_length, uint16 *rx_buffer);


/****************************************************************************/
/*!
 * \brief Writes 8-bit to the specified register on the specified device.
 *
 *  \param    slave_addr            The 7 or 10-bit I2C address of the slave
 *                                  device (the device being accessed).
 *  \param    register_address      The 8-bit register address of the slave device
 *  \param    register_value        A uint8 pointer to the receive data.
 *
 *  \retval ::sys_status_success    The transmission has completed successfully.
 *  \retval ::sys_status_busy       The operation cannot proceed because the I2C
 *                                  peripheral is currently busy with a previous request.
 *  \retval ::sys_status_failed     The operation cannot proceed because the I2C
 *                                  peripheral is not open or is not in Master mode.
 *  \retval ::sys_status_bad_params The transaction is too big and should be
 *                                  performed with an asynchronous call.
 */
/****************************************************************************/
extern sys_status I2c_WriteRegister(i2c_address_t slave_addr,
        uint8 register_address, uint8 register_value);


/****************************************************************************/
/*!
 * \brief Writes a contiguous sequence of registers on the specified device.
 *
 *  \param    slave_addr            The 7 or 10-bit I2C address of the slave
 *                                  device (the device being accessed).
 *  \param    register_address      The 8-bit register address of the slave device
 *  \param    tx_length             The number of bytes to be written.
 *  \param    tx_buffer             A buffer of uint16's to write.
 *                                  <b>Note</b>: <c>tx_buffer</c> is a packed buffer of uint16's
 *                                  containing <c>tx_length</c> bytes of data.
 *                                  The buffer is packed in little endian format e.g. the following array
 *                                  will send the bytes 1,2,3,4 {0x0201,0x0403}
 *
 *  \retval  ::sys_status_success   The operation is complete.
 *  \retval  ::sys_status_busy      The operation cannot proceed because the I2C
 *                                  peripheral is currently busy with a previous request.
 *  \retval  ::sys_status_failed    The operation cannot proceed because the I2C
 *                                  peripheral is not open or is not in Master mode.
 */
/****************************************************************************/
extern sys_status I2c_WriteRegisters(i2c_address_t slave_addr,
        uint8 register_address, uint16 tx_length, uint16 *tx_buffer);


/*! @} */
#endif /* __I2C_COMMS_H__ */
