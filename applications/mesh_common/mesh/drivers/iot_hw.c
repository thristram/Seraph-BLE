/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      iot_hw.c
 *
 *  DESCRIPTION
 *      This file implements the LED Controller of IOT hardware
 *
 *****************************************************************************/

#include <mem.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "iot_hw.h"
#include "fast_pwm.h"
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
/* Colour depth in bits, as passed by application. */
#define LIGHT_INPUT_COLOR_DEPTH  (8)
/* Colour depth in bits, mapped to actual hardware. */
#define LIGHT_MAPPED_COLOR_DEPTH (6)
/* Colour depth lost due to re-quantization of levels. */
#define QUANTIZATION_ERROR       (LIGHT_INPUT_COLOR_DEPTH -\
                                  LIGHT_MAPPED_COLOR_DEPTH)

#ifdef CSR101x_A05
/* Maximum colour level supported by mapped colour depth bits. */
#define COLOR_MAX_VALUE          ((0x1 << LIGHT_MAPPED_COLOR_DEPTH) - 1)
#else
#define COLOR_MAX_VALUE          (255)
/*============================================================================*
 *  Private Data
 *============================================================================*/
/* Handle to PWM module */
static handle_t pwm_ramp_red;
static handle_t pwm_ramp_green;
static handle_t pwm_ramp_blue;
static pio_mask_t  pio_msk_sw2;
static pio_mask_t  pio_msk_sw3;
static pio_mask_t  pio_msk_sw4;
static uint16  sw2_mask_idx;
static uint16  sw3_mask_idx;
static uint16  sw4_mask_idx;
static bool pio_open = FALSE;
#endif
/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
#ifndef CSR101x_A05

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

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceInit
 *
 *  DESCRIPTION
 *      This function initialises the LED.
 *      Configure the IO lines connected to Switch as inputs.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceInit(void)
{
    /* Configure the LED_PIO_RED PIO as output PIO */
    PioSetDir(LED_PIO_RED, PIO_DIRECTION_OUTPUT);

    /* Configure the LED_PIO_GREEN PIO as output PIO */
    PioSetDir(LED_PIO_GREEN, PIO_DIRECTION_OUTPUT);

    /* Configure the LED_PIO_BLUE PIO as output PIO */
    PioSetDir(LED_PIO_BLUE, PIO_DIRECTION_OUTPUT);

    /* Open a PWM instance */
    pwm_ramp_red = PioOpenPWM(LED_PIO_RED, pio_pwm_ramp);
    pwm_ramp_green = PioOpenPWM(LED_PIO_GREEN, pio_pwm_ramp);
    pwm_ramp_blue = PioOpenPWM(LED_PIO_BLUE, pio_pwm_ramp);
    pio_open = TRUE;

    /* Set the light to OFF state on initialization */
    IOTLightControlDevicePower(FALSE);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDevicePower
 *
 *  DESCRIPTION
 *      This function sets power state of LED.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDevicePower(bool power_on)
{
    if (power_on == TRUE)
    {
        if(pio_open == FALSE)
        {
            /* Open the pwm on switch on */
            pwm_ramp_red = PioOpenPWM(LED_PIO_RED, pio_pwm_ramp);
            pwm_ramp_green = PioOpenPWM(LED_PIO_GREEN, pio_pwm_ramp);
            pwm_ramp_blue = PioOpenPWM(LED_PIO_BLUE, pio_pwm_ramp);
            pio_open = TRUE;
        }
        /* Enable PWM channel */
        PioEnablePWM(pwm_ramp_red, TRUE);
        PioEnablePWM(pwm_ramp_green, TRUE);
        PioEnablePWM(pwm_ramp_blue, TRUE);
    }
    else if(pio_open == TRUE)
    {
        /* When power off is selected, disable and close all PWMs */
        PioEnablePWM(pwm_ramp_red, FALSE);
        PioEnablePWM(pwm_ramp_green, FALSE);
        PioEnablePWM(pwm_ramp_blue, FALSE);

        PioClosePWM(pwm_ramp_red);
        PioClosePWM(pwm_ramp_green);
        PioClosePWM(pwm_ramp_blue);

        PioSet(LED_PIO_RED, 1);
        PioSet(LED_PIO_GREEN, 1);
        PioSet(LED_PIO_BLUE, 1);
        pio_open = FALSE;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceSetLevel
 *
 *  DESCRIPTION
 *      This function sets the brightness level.
 *      Convert Level to equal values of RGB.
 *      Note that linear translation has been assumed for now.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceSetLevel(uint8 level)
{
    /* Maps level to equal values of Red, Green and Blue */
    IOTLightControlDeviceSetColor(level, level, level);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceSetColor
 *
 *  DESCRIPTION
 *      This function sets the colour as passed in argument values.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceSetColor(uint8 red, uint8 green, uint8 blue)
{
    pio_pwm_config_t pwm_cfg;

    IOTLightControlDevicePower(TRUE);

    /* Common anode LED's are used hence set the configured color values to the
     * off times and compliment value on the on time.
     */
    pwm_cfg.ramp.dull_off_time = red;
    pwm_cfg.ramp.dull_on_time = (COLOR_MAX_VALUE - red);
    pwm_cfg.ramp.dull_hold_time = 1U;
    pwm_cfg.ramp.bright_off_time = red;
    pwm_cfg.ramp.bright_on_time = (COLOR_MAX_VALUE - red);
    pwm_cfg.ramp.bright_hold_time = 1U;
    pwm_cfg.ramp.number_of_steps = 0U;

    PioConfigPWM(pwm_ramp_red,&pwm_cfg);

    pwm_cfg.ramp.dull_off_time = green;
    pwm_cfg.ramp.dull_on_time = (COLOR_MAX_VALUE - green);
    pwm_cfg.ramp.dull_hold_time = 1U;
    pwm_cfg.ramp.bright_off_time = green;
    pwm_cfg.ramp.bright_on_time = (COLOR_MAX_VALUE - green);
    pwm_cfg.ramp.bright_hold_time = 1U;
    pwm_cfg.ramp.number_of_steps = 0U;

    PioConfigPWM(pwm_ramp_green,&pwm_cfg);

    pwm_cfg.ramp.dull_off_time = blue;
    pwm_cfg.ramp.dull_on_time = (COLOR_MAX_VALUE - blue);
    pwm_cfg.ramp.dull_hold_time = 1U;
    pwm_cfg.ramp.bright_off_time = blue;
    pwm_cfg.ramp.bright_on_time = (COLOR_MAX_VALUE - blue);
    pwm_cfg.ramp.bright_hold_time = 1U;
    pwm_cfg.ramp.number_of_steps = 0U;

    PioConfigPWM(pwm_ramp_blue,&pwm_cfg);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceBlink
 *
 *  DESCRIPTION
 *      This function sets colour and blink time for LEDs.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceBlink(uint8 red, uint8 green, uint8 blue,
                                       uint8 on_time, uint8 off_time)
{
    /* PWM configuration structure */
    pio_pwm_config_t pwm_cfg;

    IOTLightControlDevicePower(TRUE);

    /* Invert the On and Off times as LEDs on IOT board are Common-Anode type
     */

    pwm_cfg.ramp.dull_on_time = COLOR_MAX_VALUE - red;
    pwm_cfg.ramp.dull_off_time = red;
    pwm_cfg.ramp.dull_hold_time = off_time;
    pwm_cfg.ramp.bright_on_time = COLOR_MAX_VALUE;
    pwm_cfg.ramp.bright_off_time = 0;
    pwm_cfg.ramp.bright_hold_time = on_time;
    pwm_cfg.ramp.number_of_steps = 0U;

    PioConfigPWM(pwm_ramp_red,&pwm_cfg);

    pwm_cfg.ramp.dull_on_time = COLOR_MAX_VALUE - green;
    pwm_cfg.ramp.dull_off_time = green;
    pwm_cfg.ramp.dull_hold_time = off_time;
    pwm_cfg.ramp.bright_on_time = COLOR_MAX_VALUE;
    pwm_cfg.ramp.bright_off_time = 0;
    pwm_cfg.ramp.bright_hold_time = on_time;
    pwm_cfg.ramp.number_of_steps = 0U;

    PioConfigPWM(pwm_ramp_green,&pwm_cfg);

    pwm_cfg.ramp.dull_on_time = COLOR_MAX_VALUE - blue;
    pwm_cfg.ramp.dull_off_time = blue;
    pwm_cfg.ramp.dull_hold_time = off_time;
    pwm_cfg.ramp.bright_on_time = COLOR_MAX_VALUE;
    pwm_cfg.ramp.bright_off_time = 0;
    pwm_cfg.ramp.bright_hold_time = on_time;
    pwm_cfg.ramp.number_of_steps = 0U;

    PioConfigPWM(pwm_ramp_blue,&pwm_cfg);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTSwitchInit
 *
 *  DESCRIPTION
 *      This function sets GPIO to switch mode.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTSwitchInit(void)
{
    /* Direction mask */
    pio_mask_t dirs;
    
    /* Event mode */
    pio_event_mode_t event_mode;

    /* Initialise the mask for the button PIO */
    pio_msk_sw2.mask[0] = 0;
    pio_msk_sw2.mask[1] = 0;
    pio_msk_sw2.mask[2] = 0;

    pio_msk_sw3.mask[0] = 0;
    pio_msk_sw3.mask[1] = 0;
    pio_msk_sw3.mask[2] = 0;

    pio_msk_sw4.mask[0] = 0;
    pio_msk_sw4.mask[1] = 0;
    pio_msk_sw4.mask[2] = 0;

    /* Set the mask for the button PIO */
    sw2_mask_idx = setPioMask(SW2_PIO, &pio_msk_sw2);

    /* Set the mask for the button PIO */
    sw3_mask_idx = setPioMask(SW3_PIO, &pio_msk_sw3);

    /* Set the mask for the button PIO */
    sw4_mask_idx = setPioMask(SW4_PIO, &pio_msk_sw4);

    /* Set the dirs mask to all zeros (inputs) */
    dirs.mask[0] = 0;
    dirs.mask[1] = 0;
    dirs.mask[2] = 0;

    /* Configure button PIO as an input */
    PioSetDirMultiple(pio_msk_sw2, dirs);
    PioSetDirMultiple(pio_msk_sw3, dirs);
    PioSetDirMultiple(pio_msk_sw4, dirs);

    /* Configure pull-up on button PIO */
    PioSetPullModeMultiple(pio_msk_sw2, pio_mode_strong_pull_up);
    PioSetPullModeMultiple(pio_msk_sw3, pio_mode_strong_pull_up);
    PioSetPullModeMultiple(pio_msk_sw4, pio_mode_strong_pull_up);

    /* Event mode selection */    
    event_mode = pio_event_mode_both | pio_event_mode_wake_both;
    
    /* Configure event notification */
    PioSetEventMultiple(pio_msk_sw2, event_mode);  /* Event mode flags */
    PioSetEventMultiple(pio_msk_sw3, event_mode);
    PioSetEventMultiple(pio_msk_sw4, event_mode);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetIOTSwitchMask
 *
 *  DESCRIPTION
 *      This function gets the IOT switch mask for the pio specified.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern bool GetIOTSwitchMask(uint8 pio, pio_mask_t *switch_mask,
                             uint16 *switch_index)
{
    bool return_val = FALSE;

    if(pio == SW2_PIO)
    {
        MemCopy(switch_mask, &pio_msk_sw2, sizeof(pio_msk_sw2));
        *switch_index = sw2_mask_idx;
        return_val = TRUE;
    }

    if(pio == SW3_PIO)
    {
        MemCopy(switch_mask, &pio_msk_sw3, sizeof(pio_msk_sw3));
        *switch_index = sw3_mask_idx;
        return_val = TRUE;
    }

    if(pio == SW4_PIO)
    {
        MemCopy(switch_mask, &pio_msk_sw4, sizeof(pio_msk_sw4));
        *switch_index = sw4_mask_idx;
        return_val = TRUE;
    }
    return return_val;
}

#else

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceInit
 *
 *  DESCRIPTION
 *      This function initialises the Red, Green and Blue LED lines.
 *      Configure the IO lines connected to Switch as inputs.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceInit(void)
{
#ifdef ENABLE_FAST_PWM
    PioFastPwmConfig(PIO_BIT_MASK(LED_PIO_RED) | \
                     PIO_BIT_MASK(LED_PIO_GREEN) | \
                     PIO_BIT_MASK(LED_PIO_BLUE));
#else
    /* Configure the LED_PIO_RED PIO as output PIO */
    PioSetDir(LED_PIO_RED, PIO_DIRECTION_OUTPUT);

    /* Configure the LED_PIO_GREEN PIO as output PIO */
    PioSetDir(LED_PIO_GREEN, PIO_DIRECTION_OUTPUT);

    /* Configure the LED_PIO_BLUE PIO as output PIO */
    PioSetDir(LED_PIO_BLUE, PIO_DIRECTION_OUTPUT);

    /* Set the LED to OFF on Initialization */
    PioSet(LED_PIO_RED, 1);
    PioSet(LED_PIO_GREEN, 1);
    PioSet(LED_PIO_BLUE, 1);
#endif /* ENABLE_FAST_PWM */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDevicePower
 *
 *  DESCRIPTION
 *      This function sets power state of LED.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDevicePower(bool power_on)
{
#ifdef ENABLE_FAST_PWM
    PioFastPwmEnable(power_on);
#else
    if (power_on == TRUE)
    {
        /* Configure the LED's */

        /* Connect PWM0 output to LED_PIO_RED */
        PioSetMode(LED_PIO_RED, pio_mode_pwm0);

        /* Connect PWM1 output to LED_PIO_GREEN */
        PioSetMode(LED_PIO_GREEN, pio_mode_pwm1);

         /* Connect PWM2 output to LED_PIO_BLUE */
        PioSetMode(LED_PIO_BLUE, pio_mode_pwm2);

        /* Enable the PIO's */
        PioEnablePWM(LED_PWM_RED, TRUE);
        PioEnablePWM(LED_PWM_GREEN, TRUE);
        PioEnablePWM(LED_PWM_BLUE, TRUE);
    }
    else
    {
         /* When power off is selected, disable all PWMs and
          * set all PIOs to HIGH, as IOT board uses common anode LED.
          */
         PioSetMode(LED_PIO_RED, pio_mode_user);
         PioSetMode(LED_PIO_GREEN, pio_mode_user);
         PioSetMode(LED_PIO_BLUE, pio_mode_user);

         PioSet(LED_PIO_RED, 1);
         PioSet(LED_PIO_GREEN, 1);
         PioSet(LED_PIO_BLUE, 1);
    }
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceSetLevel
 *
 *  DESCRIPTION
 *      This function sets the brightness level.
 *      Convert Level to equal values of RGB.
 *      Note that linear translation has been assumed for now.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceSetLevel(uint8 level)
{
    /* Maps level to equal values of Red, Green and Blue */
    IOTLightControlDeviceSetColor(level, level, level);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceSetColor
 *
 *  DESCRIPTION
 *      This function sets the colour as passed in argument values.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceSetColor(uint8 red, uint8 green, uint8 blue)
{
#ifdef ENABLE_FAST_PWM
    PioFastPwmSetWidth(LED_PIO_RED, red, 0xFF - red, TRUE);
    PioFastPwmSetWidth(LED_PIO_GREEN, green, 0xFF - green, TRUE);
    PioFastPwmSetWidth(LED_PIO_BLUE, blue, 0xFF - blue, TRUE);
    PioFastPwmSetPeriods(1, 0);
    PioFastPwmEnable(TRUE);
#else
    /* When level is Lowest (0-3) simply disable PWM to avoid flicker */
    if ((red >> QUANTIZATION_ERROR) == 0)
    {
         PioSetMode(LED_PIO_RED, pio_mode_user);
         PioSet(LED_PIO_RED, 1);
    }
    else
    {
        PioSetMode(LED_PIO_RED, pio_mode_pwm0);
    }

    if ((green >> QUANTIZATION_ERROR) == 0)
    {
         PioSetMode(LED_PIO_GREEN, pio_mode_user);
         PioSet(LED_PIO_GREEN, 1);
    }
    else
    {
        PioSetMode(LED_PIO_GREEN, pio_mode_pwm1);
    }

    if ((blue >> QUANTIZATION_ERROR) == 0)
    {
         PioSetMode(LED_PIO_BLUE, pio_mode_user);
         PioSet(LED_PIO_BLUE, 1);
    }
    else
    {
        PioSetMode(LED_PIO_BLUE, pio_mode_pwm2);
    }

    /* Invert values as its a pull down */
    red   = COLOR_MAX_VALUE - (red >> QUANTIZATION_ERROR);
    green = COLOR_MAX_VALUE - (green >> QUANTIZATION_ERROR);
    blue  = COLOR_MAX_VALUE - (blue >> QUANTIZATION_ERROR);

    PioConfigPWM(LED_PWM_RED, pio_pwm_mode_push_pull,
                 red, (COLOR_MAX_VALUE - red), 1U,
                 red, (COLOR_MAX_VALUE - red), 1U, 0U);
    PioConfigPWM(LED_PWM_GREEN, pio_pwm_mode_push_pull,
                 green, (COLOR_MAX_VALUE - green), 1U,
                 green, (COLOR_MAX_VALUE - green), 1U, 0U);
    PioConfigPWM(LED_PWM_BLUE, pio_pwm_mode_push_pull,
                 blue, (COLOR_MAX_VALUE - blue), 1U,
                 blue, (COLOR_MAX_VALUE - blue), 1U, 0U);
#endif /* ENABLE_FAST_PWM */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTLightControlDeviceBlink
 *
 *  DESCRIPTION
 *      This function sets colour and blink time for LEDs.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTLightControlDeviceBlink(uint8 red, uint8 green, uint8 blue,
                                       uint8 on_time, uint8 off_time)
{
#ifdef ENABLE_FAST_PWM    
    PioFastPwmSetWidth(LED_PIO_RED, red, 0, TRUE);
    PioFastPwmSetWidth(LED_PIO_GREEN, green, 0, TRUE);
    PioFastPwmSetWidth(LED_PIO_BLUE, blue, 0, TRUE);
    PioFastPwmSetPeriods((on_time << 4), (off_time << 4));
    PioFastPwmEnable(TRUE);
#else
    IOTLightControlDevicePower(TRUE);

    IOTLightControlDeviceSetColor(red, green, blue);

    /* Invert the On and Off times as LEDs on
     * IOT board are Common-Anode type
     */
    PioConfigPWM(LED_PWM_RED, pio_pwm_mode_push_pull,
                 red, 0, off_time, 0, red, on_time, 0U);

    PioConfigPWM(LED_PWM_GREEN, pio_pwm_mode_push_pull,
                 green, 0, off_time, 0, green, on_time, 0U);

    PioConfigPWM(LED_PWM_BLUE, pio_pwm_mode_push_pull,
                 blue, 0, off_time, 0, blue, on_time, 0U);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IOTSwitchInit
 *
 *  DESCRIPTION
 *      This function sets GPIO to switch mode.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void IOTSwitchInit(void)
{
    /* Set-up the PIOs for the switches */
    PioSetDir(SW2_PIO, PIO_DIRECTION_INPUT);
    PioSetMode(SW2_PIO, pio_mode_user);

    PioSetDir(SW3_PIO, PIO_DIRECTION_INPUT);
    PioSetMode(SW3_PIO, pio_mode_user);

    PioSetDir(SW4_PIO, PIO_DIRECTION_INPUT);
    PioSetMode(SW4_PIO, pio_mode_user);

    PioSetPullModes(BUTTONS_BIT_MASK , pio_mode_strong_pull_up);
    PioSetEventMask(BUTTONS_BIT_MASK , pio_event_mode_both);
}

#endif
