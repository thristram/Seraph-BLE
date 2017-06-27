/*! \file pio.h
 *  \brief PIO configuration and control functions.
 *
 *  This file contains the functions to provide the application with
 *  access to the PIOs.
 *
 * Copyright (c) 2011 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __PIO_H__
#define __PIO_H__


#include "types.h"

/*! \addtogroup PIO_B
 * @{
 */
/*! \brief The mode of operation of an individual PIO.
 *
 *  The CSR1000 has a highly configurable set of PIO pads. Each pad can be
 *  configured either for direct control by the application (\ref pio_mode_user)
 *  or assigned to specific hardware blocks within the chip, for example the PWM
 *  outputs.
 */
typedef enum
{
    /*! The PIO is under direct application control via PioSet() and PioGet() */
    pio_mode_user               =  0,

    /*! Counts the number of edges subject to PioEnableEdgeCapture() */
    pio_mode_edge_capture       =  1,

    pio_mode_clk_req            =  3,

    /*! Control the PIO via PWM0 */
    pio_mode_pwm0               =  4,

    /*! Control the PIO via PWM1 */
    pio_mode_pwm1               =  5,

    /*! Control the PIO via PWM2 */
    pio_mode_pwm2               =  6,

    /*! Control the PIO via PWM3 */
    pio_mode_pwm3               =  7,

    /*! \brief Use PIO for quadrature decoder. 
     *         Even PIOs = Phase A, Odd PIOs = Phase B. 
     */

    /*! Unavailable on CSR100x devices */
    pio_mode_quadrature0        =  8,

    /*! Unavailable on CSR100x devices */
    pio_mode_quadrature1        =  9,

    /*! Unavailable on CSR100x/CR101x devices */
    pio_mode_quadrature2        = 10,

    /*! Unavailable on CSR100x/CR101x devices */
    pio_mode_quadrature3        = 11,

    /*! Use PIO for UART receive (odd-numbered PIOs) or transmit
     * (even-numbered PIOs).  PIOs 0 and 1 are configured for UART
     * during firmware initialisation.
     */
    pio_mode_uart               = 12,

    /*! Use PIO for radio reception debug signals.  The CS key \c
     * debug_radio_rx controls which PIO is set to this mode during
     * firmware initialisation.
     */
    pio_mode_radio_rx_en        = 13,

    /*! Use PIO for radio transmission debug signals.  The CS key \c
     * debug_radio_tx controls which PIO is set to this mode during
     * firmware initialisation.
     */
    pio_mode_radio_tx_en        = 14,

    /*! Use PIO to control power to non-volatile memory devices.  PIO
     * 2 is configured for this by default.
     */
    pio_mode_nvm_power_en       = 15,

    /*! Control the PIO via the 8051 PIO Controller unit */
    pio_mode_pio_controller     = 16,

    /*! Control the PIO from the 8051 PIO Controller UART TX */
    pio_mode_pio_control_txd    = 17,

    /*! Control the PIO from the 8051 PIO Controller UART RX */
    pio_mode_pio_control_rxd    = 18,

    /*! Assign SPI Flash MOSI to this PIO */
    pio_mode_ser_flash_dout     = 19,

    /*! Assign SPI Flash Chip Select to this PIO */
    pio_mode_ser_flash_csb      = 20,

    /*! Assign I2C Serial Data / SPI Flash MISO to this PIO */
    pio_mode_i2c_data           = 21,

    /*! Assign I2C Serial Clock / SPI Flash Clock to this PIO */
    pio_mode_i2c_clock          = 22,

    pio_mode_uart_flow_ctrl     = 23,
    pio_mode_pio_control_timer  = 24,

    /*! Monitor one of the internal clocks on a PIO. PioSetAnaMonClk()
     * can be used to select which clock will be output to the PIO
     */
    pio_mode_ana_mon_clk_pio    = 25

} pio_mode;

/*! \brief The clock source to monitor on a PIO.
 *
 *  This type defines the available internal clock sources. One of these sources
 *  can be selected for monitoring on a PIO by calling PioSetAnaMonClk(). The
 *  PIO used for monitoring the clock can be selected by calling PioSetMode()
 *  with mode \ref pio_mode_ana_mon_clk_pio.
 */
typedef enum
{
    pio_ana_mon_clk_32k         = 1,    /*!< Select 32kHz clock */
    pio_ana_mon_clk_16m         = 2,    /*!< Select 16MHz clock */

} pio_ana_mon_clk;

/*! \brief PIO pad pull modes
 *
 *  This enumeration behaves as a bitfield for enabling the possible pull modes
 *  for general PIO pads. The defined enumerated values represent the valid
 *  combinations of the individual bits.
 *
 *      Bit 0 - enable bit.
 *      Bit 1 - pull direction (0 = down, 1 = up).
 *      Bit 2 - pull strength (0 = weak, 1 = strong).
 *      Bit 3 - sticky pull (0 = off, 1 = on).
 */
typedef enum
{
    pio_mode_no_pulls                   = 0x00,   /*!< No pulling enabled */
    pio_mode_weak_pull_down             = 0x01,   /*!< Use weak pull-down */
    pio_mode_weak_pull_up               = 0x03,   /*!< Use weak pull-up */
    pio_mode_strong_pull_down           = 0x05,   /*!< Use strong pull-down */
    pio_mode_strong_pull_up             = 0x07,   /*!< Use strong pull-up */
    pio_mode_weak_sticky                = 0x09,   /*!< Use weak pull-down with sticky (non-floating) inputs */
    pio_mode_strong_sticky              = 0x0d    /*!< Use strong pull-down with sticky (non-floating) inputs */

} pio_pull_mode;

/*! \brief I2C Pad pull modes
 *
 *  This enumeration behaves as a bitfield for enabling the possible pull modes
 *  for dedicated I2C pads. The defined enumerated values represent the valid
 *  combinations of the individual bits.
 *
 *      Bit 0 - enable bit.
 *      Bit 1 - pull direction (0 = down, 1 = up).
 *      Bit 2 - pull strength (0 = weak, 1 = strong).
 *      Bit 3 - sticky pull (0 = off, 1 = on).
 */
typedef enum
{
    pio_i2c_pull_mode_no_pulls          = 0x00,   /*!< No pulling enabled */
    pio_i2c_pull_mode_weak_pull_down    = 0x01,   /*!< Use weak pull-down */
    pio_i2c_pull_mode_weak_pull_up      = 0x03,   /*!< Use weak pull-up */
    pio_i2c_pull_mode_strong_pull_down  = 0x05,   /*!< Use strong pull-down */
    pio_i2c_pull_mode_strong_pull_up    = 0x07,   /*!< Use strong pull-up */
    pio_i2c_pull_mode_weak_sticky       = 0x09,   /*!< Use weak pull-down with sticky (non-floating) inputs */
    pio_i2c_pull_mode_strong_sticky     = 0x0d    /*!< Use strong pull-down with sticky (non-floating) inputs */

} pio_i2c_pull_mode;

/*! \brief Event modes controlling when #sys_event_pio_changed events are generated */
/* This enumeration behaves as a bitfield for enabling event generation on 
 * either or both edges:
 *      bit 0 - rising edge
 *      bit 1 - falling edge
 */
typedef enum
{
    /*! Generate no events for these PIOs */
    pio_event_mode_disable = 0x00,

    /*! Generate events on a rising edge on these PIOs */
    pio_event_mode_rising  = 0x01,

    /*! Generate events on a falling edge on these PIOs */
    pio_event_mode_falling = 0x02,

    /*! Generate events on a rising or falling edge on these PIOs */
    pio_event_mode_both    = 0x03,

} pio_event_mode;

/*! \name User-Mode PIO functions
 *  Functions for controlling the behaviour of PIOs which have a mode
 *  of #pio_mode_user
 *  @{
 */

/*----------------------------------------------------------------------------*
 *  PioSet
 */
/*! \brief  Allows the user application to set the state of a particular PIO.
 *
 *  \param  pio  The PIO to set.
 *  \param  set  TRUE to set the PIO high, FALSE to set it low.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSet(uint16 pio, bool set);

/*----------------------------------------------------------------------------*
 *  PioGet
 */
/*! \brief  Allows the user application to read the high/low state of a PIO
 *
 *  \param  pio  The index (0-31) of the PIO to be read.
 *
 *  \returns  TRUE if the PIO is currently being driven high either internally
 *      (for a PIO that is set as an output) or externally (for a PIO that
 *      is set as an input).
 */
/*---------------------------------------------------------------------------*/
extern bool PioGet(uint16 pio);

/*----------------------------------------------------------------------------*
 *  PioSets
 */
/*! \brief  Allows the user application to set the state of a number of PIOs.
 *
 *  \param  mask  A bit-mask of PIOs to set.  If bit <n> of \a mask is set,
 *  PIO <n> will be set from bit <n> of \a data.  If bit <n> of \a mask is
 *  clear, PIO <n> will be left alone.
 *
 *  \param data  Subject to \a mask, PIO <n> is high or low corresponding
 *  to bit <n> of \a data.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSets(uint32 mask, uint32 data);

/*----------------------------------------------------------------------------*
 *  PioGets
 */
/*! \brief  Allows the user application to read the high/low state of the PIOs.
 *
 *  \returns  A bit-mask indicating which PIOs are currently being driven high. A
 *      bit set in the bit-mask indicates that the corresponding PIO is being
 *      driven high either internally (for a PIO that is set as an output) or
 *      externally (for a PIO that is set as an input).
 */
/*---------------------------------------------------------------------------*/
extern uint32 PioGets(void);

/*----------------------------------------------------------------------------*
 *  PioSetDir
 */
/*! \brief  Allows the user application to set the direction (input or output) of
 *      a particular PIO.
 *
 *  \param  pio  The index (0-31) of the PIO to be updated.
 *
 *  \param  output  If set to TRUE, makes the specified PIO an output.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetDir(uint16 pio, bool output);

/*----------------------------------------------------------------------------*
 *  PioGetDir
 */
/*! \brief  Allows the user application to read the direction (input or output) of
 *      a particular PIO.
 *
 *  \param  pio  The PIO to read the direction of.
 *
 *  \returns  TRUE if the specified PIO is currently an output.
 */
/*---------------------------------------------------------------------------*/
extern bool PioGetDir(uint16 pio);

/*----------------------------------------------------------------------------*
 *  PioSetDirs
 */
/*! \brief  Allows the user application to set the direction (input or output) of
 *      a number of PIOs.
 *
 *  \param  mask  A bit-mask of PIOs to set the direction of.  If bit
 *  <n> of \a mask is set, PIO <n> will have its direction configured
 *  according to bit <n> of \a outputs.  If bit <n> of \a mask is clear, PIO
 *  <n> will be left alone.
 *
 *  \param  outputs  Subject to \a mask, PIO <n> is set as an output if
 *  bit <n> of \a outputs is high, and is set as an input if bit <n> of
 *  \a outputs is low.
 *
 *  \returns  Nothing.
 */
extern void PioSetDirs(uint32 mask, uint32 outputs);

/*---------------------------------------------------------------------------*
 *  PioGetDirs
 */
/*! \brief  Reads the directions (input or output) of the PIOs.
 *
 *  \returns  A bit-mask indicating which PIOs are currently set as outputs. A bit
 *      set in the bit-mask indicates that the corresponding PIO is an output.
 */
/*---------------------------------------------------------------------------*/
extern uint32 PioGetDirs(void);

/*----------------------------------------------------------------------------*
 *  PioSetPullModes
 */
/*! \brief  Allows the user application to set the pull mode of a number of PIOs.
 *
 *  \param mask  A bit-mask of PIOs to set.  If bit <n> of \a mask is
 *  set, PIO <n> will have its pull mode set.
 *
 *  \param mode  The #pio_pull_mode to set the selected PIOs to.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetPullModes(uint32 mask, pio_pull_mode mode);

/*----------------------------------------------------------------------------*
 *  PioSetMode
 */
/*! \brief Allows the user application to set the operating mode of a
 *  particular PIO.
 *
 *  \param  pio  The PIO to set the mode of.
 *
 *  \param  mode  The #pio_mode to set the selected PIO to.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetMode(uint16 pio, pio_mode mode);


/*----------------------------------------------------------------------------*
 *  PioSetModes
 */
/*! \brief Allows the user application to set the operating mode of a
 *  number of PIOs.
 *
 *  \param  mask  A bit-mask of PIOs to affect.  If bit <n> of \a mask is
 *  set, PIO <n> will have its mode changed.
 *
 *  \param  mode  The #pio_mode to set the selected PIOs to.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetModes(uint32 mask, pio_mode mode);


/*----------------------------------------------------------------------------*
 *  PioSetAnaMonClk
 */
/*! \brief  Set the internal clock source for any PIO configured with mode
 *  pio_mode_ana_mon_clk_pio.
 *
 *  \param pio_clk_source  The clock source to route to the PIOs.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetAnaMonClk(pio_ana_mon_clk pio_clk_source);


/*----------------------------------------------------------------------------*
 *  PioSetEventMask
 */
/*! \brief  Allows the user application to enable the generation of
 *      #sys_event_pio_changed system events when any of the specified
 *      PIOs change input state.
 *
 *  \param  mask  A bit-mask of PIOs to affect.
 *
 *  \param  mode  The PIO event mode to set for the specified PIOs.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetEventMask(uint32 mask, pio_event_mode mode);

/*! @} */

/*----------------------------------------------------------------------------*
 *  PioSetI2CPullMode
 */
/*! \brief  Allows the user application to set the pull mode of PIOs
 *      configured for I2C operation.
 *
 *  \param  mode  The #pio_i2c_pull_mode to apply.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioSetI2CPullMode(pio_i2c_pull_mode mode);

/*! @} */

/*! \addtogroup PIO_W
 * @{
 */
/*! \brief Pulse Width Modulator PIO pin modes */
typedef enum
{
    pio_pwm_mode_push_pull,
    pio_pwm_mode_open_drain,
    pio_pwm_mode_open_source,
    pio_pwm_mode_inverted_push_pull

} pio_pwm_mode;

/*! \name PWM-Mode PIO functions
 *  Functions for controlling the behaviour of PIOs which have a mode
 *  of #pio_mode_pwm0, #pio_mode_pwm1, #pio_mode_pwm2 or #pio_mode_pwm3.
 *
 *  The PIO's mode setting controls which of the Pulse Width Modulation
 *  drivers the PIO is connected to.  The following functions
 *  configure how each PWM driver behaves.
 *
 *  @{
 */

/*----------------------------------------------------------------------------*
 *  PioConfigPWM
 */
/*! \brief  Allows the user application to configure the PIO's Pulse Width
 *      Modulation driver.
 *
 *      The parameter descriptions are written in terms of LEDs and
 *      brightness since that will be the most common use case.
 *      However there is no reason why PIOs controlled by a PWM unit
 *      can't drive other devices than LEDs.
 *
 *  \param  pwm_id  the index (0-3) of the PWM unit to be configured.
 *
 *  \param  mode  the operating mode (#pio_pwm_mode) of the PIO pins
 *          used.
 *
 *  \param  dull_off_time  the amount of time, in units of ~30us, for which the
 *          LED should be off during the dullest part of the flash sequence.
 *
 *  \param  dull_on_time  the amount of time, in units of ~30us, for which the
 *          LED should be on during the dullest part of the flash sequence.
 *
 *  \param  dull_hold_time  the amount of time, in units of ~16ms, for which the
 *          LED should be held in the dullest part of the flash sequence.
 *
 *  \param  bright_off_time  the amount of time, in units of ~30us, for which the
 *          LED should be off during the brightest part of the flash sequence.
 *
 *  \param  bright_on_time  the amount of time, in units of ~30us, for which the
 *          LED should be on during the brightest part of the flash sequence.
 *
 *  \param  bright_hold_time  the amount of time, in units of ~16ms, for which the
 *          LED should be held in the brightest part of the flash sequence.
 *
 *  \param  ramp_rate  the ramp rate for ramping between brightness levels, in
 *          units of ~30us per step with 0 being instantaneous (no ramp).
 *
 *  \returns  TRUE if request was successful.
 */
/*---------------------------------------------------------------------------*/
extern bool PioConfigPWM(uint16 pwm_id, pio_pwm_mode mode,
        uint8 dull_on_time, uint8 dull_off_time, uint8 dull_hold_time,
        uint8 bright_on_time, uint8 bright_off_time, uint8 bright_hold_time,
        uint8 ramp_rate);

/*----------------------------------------------------------------------------*
 *  PioEnablePWM
 */
/*! \brief  Allows the user application to enable or disable the PIO's Pulse Width
 *      Modulation drivers.
 *
 *  \param pwm_id  the index (0-3) of the PWM unit to be enabled or disabled.
 *
 *  \param enable  TRUE to enable the unit, FALSE to disable it.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioEnablePWM(uint16 pwm_id, bool enable);

/*! @} */
/*! @} */

/*! \addtogroup PIO_E
 * @{
 */
/*! \name Edge Capture Mode PIO functions
 *  Functions for controlling the behaviour of PIOs which have a mode
 *  of #pio_mode_edge_capture.
 *
 *  These functions only allow for overall control of all PIOs in
 *  #pio_mode_edge_capture mode.  There is no way to enable or disable
 *  edge capture on an individual PIO except by changing the mode of
 *  the PIO.
 *
 *  @{
 */

/*----------------------------------------------------------------------------*
 *  PioEnableEdgeCapture
 */
/*! \brief  Allows the user application to enable or disable edge capture
 *      mode for all PIOs.
 *
 *  \param  enable  TRUE to enable, FALSE to disable
 *  \param  rising  TRUE to capture rising edges, FALSE for falling edges
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PioEnableEdgeCapture(bool enable, bool rising);

/*----------------------------------------------------------------------------*
 *  PioReadEdgeCapture
 */
/*! \brief  Allows the user application to take a reading from the edge capture.
 *
 *  \returns  A 24-bit reading of the number of edges detected.
 */
/*---------------------------------------------------------------------------*/
extern uint32 PioReadEdgeCapture(void);

/*! @} */
/*! @} */

/*! \addtogroup PIO_Q
 * @{
 */

 /*! \name Quadrature Decoder PIO functions
 *  Functions for controlling the behaviour of PIOs which have a mode
 *  of #pio_mode_quadrature. 
 * 
 *  \note No quadrature decoders are available on the CSR100x devices and 
 *  therefore these functions may be unavailable/disabled in libraries for
 *  these devices. 
 * 
 *  Two quadrature decoders are available on CSR101x devices, with ids of 0 and 1
 *  
 *  \note Setting an odd numbered PIO pin with a mode of pio_mode_quadratureN to use
 *  it as the Phase B input to a hardware quadrature decoder, causes the next lower 
 *  pin to actually be used as the decoder's input. 
 *  For example, after the following sequence: 
 *  \code
 *     PioSetMode(24, pio_mode_quadrature0);  
 *     PioSetMode(23, pio_mode_quadrature0);  
 *  \endcode
 *
 *   Pins 24 and 22 (i.e 23 - 1) will be the actual input pins for quadrature decoder 0. 
 *   However pin 23 is not available for other purposes.
 *
 *  @{
 */

/*----------------------------------------------------------------------------*
 *  PioEnableQuadratureDecoder
 */
/*! \brief  Allows the user application to enable or disable a quadrature 
 *          decoder. 
 *
 *  \param  quad_id The index (0-1) of the quadrature decoder to be enabled/disabled.
 *
 *  \param  enable  TRUE to enable the selected decoders, FALSE to disable.
 *
 *  \returns  Nothing.
 */
extern void PioEnableQuadratureDecoder(uint16 quad_id, bool enable);

/*----------------------------------------------------------------------------*
 *  PioEnableQuadratureDecoders
 */
/*! \brief  Allows the user application to enable or disable the quadrature 
 *          decoders. 
 *
 *  \param  id_mask Bit mask indicating which of the available decoders to 
 *          enable or disable. Bit 0 corresponds to quadrature decoder 0, bit 1 to
 *          quadrature decoder 1, etc. Only decoders with their corresponding
 *          bit set will be affected; the remaining decoders will be left in
 *          their present enabled/disabled state.
 *
 *  \param enables Subject to \a id_mask, quadrature decoder <n> is enabled (1) or 
 *                 disabled (0) corresponding to bit <n> of \a data.
 *
 *  \returns  Nothing.
 */
extern void PioEnableQuadratureDecoders(uint16 id_mask, uint16 enables);


/*----------------------------------------------------------------------------*
 *  PioReadQuadratureDecoder
 */
/*! \brief  Allows the user application to take a reading from the given decoder.
 *
 *  \param  quad_id The ID (0/1) of the quadrature decoder to read from.
 *
 *  \returns  A 16-bit counter reading.
 */
extern uint16 PioReadQuadratureDecoder(uint16 quad_id);

/*! @} */
/*! @} */

#endif /* __PIO_H__ */
