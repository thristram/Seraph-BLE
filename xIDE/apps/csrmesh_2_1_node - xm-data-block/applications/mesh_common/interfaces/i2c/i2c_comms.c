/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file i2c_comms.c
 *
 *  This file implements the I2C procedures.
 */

/*==========================================================================*
    SDK Header Files
 *==========================================================================*/
#include <types.h>
#include <i2c.h>


/*==========================================================================*
    Local Header Files
 *==========================================================================*/
#include "i2c_comms.h"
#include "gatt_uuid.h"


/*==========================================================================*
    Private Data Types
*===========================================================================*/
/* The following enum will tell if the I2C bus is in Released state or in
 * Acquired state.
 */
typedef enum
{
    i2c_bus_released = 0x00,
    i2c_bus_acquired = 0x01
} i2c_bus_status;


/*==========================================================================*
    Private Data
*===========================================================================*/
/* Flag for the I2C bus status */
i2c_bus_status bus_I2C_status = i2c_bus_released;

bool i2c_initialised = FALSE;


/*==========================================================================*
    Public Function Implementations
*===========================================================================*/

/****************************************************************************/
/*! I2c_Init
 */
/****************************************************************************/
#if defined (CSR101x_A05)
extern sys_status I2c_Init(uint8 sda, uint8 scl, i2c_pull_mode_t pullmode)
{
#error "CSR101x is not supported at this time."
}
#else
extern sys_status I2c_Init(pio_t sda, pio_t scl, i2c_pull_mode_t pullmode)
{
    /* Configure the I2C controller */
    return I2cOpen(sda, scl, pullmode, I2C_BUS_MODE_MASTER);
}
#endif /* CSR101x_A05 */


/****************************************************************************/
/*! I2c_Acquire
 */
/****************************************************************************/
extern bool I2c_Acquire(void)
{
    /* If I2C bus is in Released state, acquire it and return success */
    if(bus_I2C_status == i2c_bus_released)
    {
        bus_I2C_status = i2c_bus_acquired;
        return TRUE;
    }
    else /* Else return Failure */
    {
        return FALSE;
    }
}


/****************************************************************************/
/*! I2c_Release
 */
/****************************************************************************/
extern void I2c_Release(void)
{
    bus_I2C_status = i2c_bus_released;
#if defined (CSR101x_A05)
#error "CSR101x is not supported at this time."
#endif /* CSR101x_A05 */
}

/****************************************************************************/
/*! I2c_ReadRegister
 */
/****************************************************************************/
extern sys_status I2c_ReadRegister(i2c_address_t slave_addr,
        uint8 register_address, uint8 *register_value)
{
#if defined (CSR101x_A05)
#error "CSR101x is not supported at this time."
#else
    return I2c_ReadRegisters(slave_addr, register_address, 1, (uint16*)register_value);
#endif /* CSR101x_A05 */
}


/****************************************************************************/
/*! I2c_ReadRegisters
 */
/****************************************************************************/
extern sys_status I2c_ReadRegisters(i2c_address_t slave_addr,
        uint8 register_address, uint16 rx_length, uint16 *rx_buffer)
{
#if defined (CSR101x_A05)
#error "CSR101x is not supported at this time."
#else
    uint16 tx_buffer = register_address;
    uint16 read_bytes = rx_length;
    return I2cTransfer(slave_addr, &tx_buffer, 1, rx_buffer, &read_bytes,
                              (I2C_FLAG_START |
                               I2C_FLAG_STOP  |
                               I2C_FLAG_BLOCKING));
#endif /* CSR101x_A05 */
}


/****************************************************************************/
/*! I2c_WriteRegister
 */
/****************************************************************************/
extern sys_status I2c_WriteRegister(i2c_address_t slave_addr,
        uint8 register_address, uint8 register_value)
{
#if defined (CSR101x_A05)
#error "CSR101x is not supported at this time."
#else
    uint16 data = BYTE_JOIN_16(register_value, register_address);
    return I2cWrite(slave_addr, &data, 2,
            (I2C_FLAG_START | I2C_FLAG_STOP  | I2C_FLAG_BLOCKING));
#endif /* CSR101x_A05 */
}


/****************************************************************************/
/*! I2c_WriteRegisters
 */
/****************************************************************************/
extern sys_status I2c_WriteRegisters(i2c_address_t slave_addr,
        uint8 register_address, uint16 tx_length, uint16 *tx_buffer)
{
#if defined (CSR101x_A05)
#error "CSR101x is not supported at this time."
#else
    uint16 data = register_address;
    return ( (I2cWrite(slave_addr, &data, 1,
                    (I2C_FLAG_START | I2C_FLAG_BLOCKING) ) == STATUS_SUCCESS) &&
             (I2cWrite(slave_addr, tx_buffer, tx_length,
                     (I2C_FLAG_STOP | I2C_FLAG_BLOCKING) ) == STATUS_SUCCESS) );
#endif /* CSR101x_A05 */
}


