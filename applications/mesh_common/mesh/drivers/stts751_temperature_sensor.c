/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      stts751_temperature_sensor.c
 *
 *  DESCRIPTION
 *      This file implements the procedure for communicating with a STTS751
 *      Sensor.
 *
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <types.h>
#include <macros.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "stts751_temperature_sensor.h"
#include "app_debug.h"
#include "app_hw.h"
#ifndef CSR101x_A05
#include "interfaces\i2c\i2c_comms.h"
#else
#include "i2c_comms.h"
#endif

#ifdef TEMPERATURE_SENSOR_STTS751
#ifdef CSR101x_A05
/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_Init
 *
 *  DESCRIPTION
 *      This function initialises the temperature sensor STTS751
 *
 *  RETURNS
 *      TRUE if success.
 *
 *---------------------------------------------------------------------------*/
extern bool STTS751_Init(void)
{
    uint8 manu_id = 0;
    bool  success = FALSE;

    /* Acquire I2C bus */
    if(I2CAcquire())
    {
        /* Initialise I2C communication. */
        I2CcommsInit();

        /* Read the Manufacturer's ID and check */
        success = I2CReadRegister(STTS751_I2C_ADDRESS, REG_MANU_ID, &manu_id);
        if ((!success) || (STTS751_MANU_ID != manu_id))
        {
            success = FALSE;
        }
        else
        {
            /* Configure the temperature sensor
             * - Standby mode for one shot readings
             * - 12 - bit resolution for 0.0625 degrees Centigrade accuracy.
             */
            success = I2CWriteRegister(STTS751_I2C_ADDRESS, REG_CONFIG,
                                       (STANDBY_MODE|TRES_12_BITS));
        }

        /* Release the I2C bus */
        I2CRelease();
    }
    return success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_Calibration
 *
 *  DESCRIPTION
 *      This function calibrates the temperature sensor STTS751.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void STTS751_Calibration(void)
{
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_InitiateOneShotRead
 *
 *  DESCRIPTION
 *      This function initiates Read from the temperature sensor STTS751.
 *
 *  RETURNS
 *      TRUE if read succeeds.
 *
 *----------------------------------------------------------------------------*/
extern bool STTS751_InitiateOneShotRead(void)
{
    bool success = FALSE;

    if (I2CAcquire())
    {
        /* Initialise I2C. */
        I2CcommsInit();

        /* Write to One Shot Register. */
        success = I2CWriteRegister(STTS751_I2C_ADDRESS, REG_ONE_SHOT_READ, 0x1);

        /* Release the I2C bus */
        I2CRelease();
    }

    return success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_ReadTemperature
 *
 *  DESCRIPTION
 *      This function read the temperature from sensor STTS751.
 *
 *  RETURNS
 *      TRUE if read succeeds.
 *
 *----------------------------------------------------------------------------*/
extern bool STTS751_ReadTemperature(int16 *temp)
{
    bool  success = FALSE;
    uint8 status  = 0x00;
    uint8 integer, fraction;

    /* Set temperature to Invalid value */
    *temp = INVALID_TEMPERATURE;

    if (I2CAcquire())
    {
        /* Initialise I2C. */
        I2CcommsInit();

        /* Check if conversion is in progress. */
        success = I2CReadRegister(STTS751_I2C_ADDRESS, REG_STATUS, &status);

        /* If read is successful and conversion is not in progress,
         * Read the temperature from the device.
         */
        if ((success) && (!(status & STATUS_BUSY_BITMASK)))
        {
            /* Read the temperature. */
            success  = 
                I2CReadRegister(STTS751_I2C_ADDRESS, REG_TEMP_MSB, &integer);
            success &= 
                I2CReadRegister(STTS751_I2C_ADDRESS, REG_TEMP_LSB, &fraction);

            if (success)
            {
                /* Sign extend the temperature value. */
                if (integer & 0x80) integer |= 0xFF00;

                /* Fill the Integer part into temperature value. */
                *temp = (int16)((uint16)integer << TRES_BITS_SHIFT(12));

                /* Right Shift the Fractional part as per resolution. */
                fraction = (fraction & 0xFF) >> TRES_BITS_SHIFT(12);

                *temp |= fraction;
            }
        }

        /* Release the I2C bus */
        I2CRelease();
    }

    return success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_ReadCallback
 *
 *  DESCRIPTION
 *      This function implements the callback function for read operation.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void STTS751_ReadCallback(void)
{
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_ShutDown
 *
 *  DESCRIPTION
 *      This function shuts down the temperature sensor STTS751
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void STTS751_ShutDown(void)
{
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_InterruptHandler
 *
 *  DESCRIPTION
 *      This function handles the interrupt from temperature sensor STTS751
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void STTS751_InterruptHandler(void)
{
}

#else

/*----------------------------------------------------------------------------*
 *  NAME
 *      setPioMask
 *
 *  DESCRIPTION
 *      Set a bit in the passed PIO mask for the indexed PIO line
 *
 *  PARAMETERS
 *      uint16      pio         Index of the PIO line
 *      pio_mask_t *pio_mask    Pointer to pio_mask_t structure
 *
 *  RETURNS
 *      Index of the mask associated with the passed PIO line
 *----------------------------------------------------------------------------*/
static uint16 setPioMask(uint16 pio, pio_mask_t *pio_mask)
{
    uint16 pio_in_mask = 0;

    /* Configure PIO mask  */
    if ( pio <= 15 )
    {
        /* PIO is in bank 0 */
        pio_mask->mask[0] |= (1 << pio);
        pio_in_mask = 0;
    }
    else
    if ( pio <= 31 )
    {
        /* PIO is in bank 1 */
        pio_mask->mask[1] |= (1 << (pio - 16));
        pio_in_mask = 1;
    }
    else
    if ( pio <= 37 )
    {
        /* PIO is in bank 2 */
        pio_mask->mask[2] |= (1 << (pio - 32));
        pio_in_mask = 2;
    }

    return pio_in_mask;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_Init
 *
 *  DESCRIPTION
 *      This function initialises the temperature sensor STTS751
 *
 *  RETURNS
 *      TRUE if success.
 *
 *---------------------------------------------------------------------------*/
extern bool STTS751_Init(void)
{
    uint8 manu_id = 0;
    bool  success = FALSE;
    sys_status i2c_status = sys_status_failed;
    /* pio configuration */
    pio_mask_t dirs;
    pio_mask_t  pio_msk_sw2;
    uint16  sw2_mask_idx;
    pio_event_mode_t event_mode;

   /* The PIO 14 connected to the SDA of the STTS751 needs to be disabled in the
    * version V2 board as the CSR102x does not support connecting even numbered
    * PIO on the SDA. Hence the PIO 14 is disabled and PIO 17 is connected 
    * externally as a workaround to work with STTS751.The disabling of this PIO
    * code could be removed on next versions of the board fixed.
    */

    /* Initialise the mask for the button PIO */
    pio_msk_sw2.mask[0] = 0;
    pio_msk_sw2.mask[1] = 0;
    pio_msk_sw2.mask[2] = 0;

    /* Set the mask for the button PIO */
    sw2_mask_idx = setPioMask(14, &pio_msk_sw2);

    /* Set the dirs mask to all zeros (inputs) */
    dirs.mask[0] = 0;
    dirs.mask[1] = 0;
    dirs.mask[2] = 0;

    /* Configure button PIO as an input */
    PioSetDirMultiple(pio_msk_sw2, dirs);

    /* Configure pull-up on button PIO */
    PioSetPullModeMultiple(pio_msk_sw2, pio_mode_strong_pull_up);

    /* Event mode selection */    
    event_mode = pio_event_mode_disable ;
    
    /* Configure event notification */
    PioSetEventMultiple(pio_msk_sw2, event_mode);  /* Event mode flags */

    /* Acquire I2C bus */
    if(I2c_Acquire())
    {
        /* Initialise I2C communication. */
        I2c_Init(I2C_SDA_PIO, I2C_SCL_PIO, I2C_PULL_MODE_STRONG_PULL_UP);

        /* Read the Manufacturer's ID and check */
        i2c_status = I2c_ReadRegister(STTS751_I2C_ADDRESS, REG_MANU_ID, &manu_id);
        if ((i2c_status != sys_status_success) || (STTS751_MANU_ID != manu_id))
        {
            success = FALSE;
        }
        else
        {
            /* Configure the temperature sensor
             * - Standby mode for one shot readings
             * - 12 - bit resolution for 0.0625 degrees Centigrade accuracy.
             */
            i2c_status = I2c_WriteRegister(STTS751_I2C_ADDRESS, REG_CONFIG,
                                          (STANDBY_MODE|TRES_12_BITS));
        }

        /* Release the I2C bus */
        I2c_Release();
    }

    if(i2c_status == sys_status_success)
        success = TRUE;

    return success;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_InitiateOneShotRead
 *
 *  DESCRIPTION
 *      This function initiates Read from the temperature sensor STTS751.
 *
 *  RETURNS
 *      TRUE if read succeeds.
 *
 *----------------------------------------------------------------------------*/
extern bool STTS751_InitiateOneShotRead(void)
{
    bool success = FALSE;
    sys_status i2c_status = sys_status_failed;

    if (I2c_Acquire())
    {
        /* Initialise I2C communication. */
        I2c_Init(I2C_SDA_PIO, I2C_SCL_PIO, I2C_PULL_MODE_STRONG_PULL_UP);

        /* Write to One Shot Register. */
        i2c_status = I2c_WriteRegister(STTS751_I2C_ADDRESS, 
                                    REG_ONE_SHOT_READ, 
                                    0x1);

        /* Release the I2C bus */
        I2c_Release();
    }

    if(i2c_status == sys_status_success)
        success = TRUE;

    return success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      STTS751_ReadTemperature
 *
 *  DESCRIPTION
 *      This function read the temperature from sensor STTS751.
 *
 *  RETURNS
 *      TRUE if read succeeds.
 *
 *----------------------------------------------------------------------------*/
extern bool STTS751_ReadTemperature(int16 *temp)
{
    bool  success = FALSE;
    uint8 status  = 0x00;
    uint8 integer, fraction;
    sys_status i2c_status = sys_status_failed;

    /* Set temperature to Invalid value */
    *temp = INVALID_TEMPERATURE;

    if (I2c_Acquire())
    {
        /* Initialise I2C communication. */
        I2c_Init(I2C_SDA_PIO, I2C_SCL_PIO, I2C_PULL_MODE_STRONG_PULL_UP);

        /* Check if conversion is in progress. */
        i2c_status = I2c_ReadRegister(STTS751_I2C_ADDRESS, REG_STATUS, &status);

        /* If read is successful and conversion is not in progress,
         * Read the temperature from the device.
         */
        if ((i2c_status == sys_status_success) &&
                                            (!(status & STATUS_BUSY_BITMASK)))
        {
            /* Read the temperature. */
            i2c_status  = 
                I2c_ReadRegister(STTS751_I2C_ADDRESS, REG_TEMP_MSB, &integer);
            i2c_status &= 
                I2c_ReadRegister(STTS751_I2C_ADDRESS, REG_TEMP_LSB, &fraction);

            if (i2c_status == sys_status_success)
            {
                /* Sign extend the temperature value. */
                if (integer & 0x80) integer |= 0xFF00;

                /* Fill the Integer part into temperature value. */
                *temp = (int16)((uint16)integer << TRES_BITS_SHIFT(12));

                /* Right Shift the Fractional part as per resolution. */
                fraction = (fraction & 0xFF) >> TRES_BITS_SHIFT(12);

                *temp |= fraction;
            }
        }

        /* Release the I2C bus */
        I2c_Release();
    }
    if(i2c_status == sys_status_success)
        success = TRUE;

    return success;
}
#endif
#endif /* TEMPERATURE_SENSOR_STTS751 */
