/*! \file pio_ctrlr.h
 *  \brief  Drivers for the 8051 PIO Controller
 *
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __PIO_CTRLR_H__
#define __PIO_CTRLR_H__

/*! \addtogroup PIO_C
 * @{
 */
/*============================================================================*
Header Files
*============================================================================*/


/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief Start address in XAP memory map of the PIO Controller's internal 128
 *  byte data memory. Two bytes are mapped to one word in the XAP memory map.
 */
#define PIO_CONTROLLER_RAM_START        ((uint16*)0xE800)

/*! \brief Size of PIO Controller's internal data memory, in *bytes* */
#define PIO_CONTROLLER_RAM_SIZE_BYTES   0x0080

/*! \brief Size of PIO Controller's internal data memory, in *words* */
#define PIO_CONTROLLER_RAM_SIZE_WORDS   0x0040

/*! \brief Offset into the PIO Controller's internal data memory of the data
 * word pointer passed to the application in a #sys_event_pio_ctrlr event.
 */
#define PIO_CONTROLLER_DATA_WORD        (PIO_CONTROLLER_RAM_START + 0x0020)


/*============================================================================*
Public Prototypes
*============================================================================*/

/****************************************************************************
 *  PioCtrlrInit
 */
/*! \brief  Initialise the 8051 subsystem.
 *
 *  Loads the program code into the 8051 subsystem memory
 *
 *  \param  code  A word array of the code to be loaded into the 8051's
 *      memory, starting with the length of the code in bytes.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void PioCtrlrInit(uint16 *code);


/****************************************************************************
 *  PioCtrlrStart
 */
/*! \brief Start the 8051 PIO Controller subsystem.
 *
 *  Enables the PIO Controller subsystem to start execution of the
 *  8051 application.  The application must have been loaded by
 *  calling PioCtrlrInit() first.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void PioCtrlrStart(void);


/****************************************************************************
 *  PioCtrlrStop
 */
/*! \brief Stop the 8051 PIO Controller subsystem.
 *
 *  Disables the PIO Controller subsystem to halt execution of the
 *  8051 application.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void PioCtrlrStop(void);


/****************************************************************************
 *  PioCtrlrInterrupt
 */
/*! \brief Generate an interrupt to the 8051 PIO Controller subsystem.
 *
 *  The PIO Controller subsystem must have been started with PioCtrlStart()
 *  prior to calling this function.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void PioCtrlrInterrupt(void);


/****************************************************************************
 *  PioCtrlrClock
 */
/*! \brief Select the clock source for the PIO Controller
 *
 *  The PIO Controller normally runs off the 32kHz clock. This
 *  function can be used to instead request the use of the fastest
 *  available clock, in which case the PIO Controller will run off the
 *  16MHz clock when that clock is running (which is any time the chip
 *  is not in Deep Sleep).
 *
 *  NOTE: if you need a guaranteed 16MHz clock source, then as well as
 *  calling this function you must also call SleepModeChange() and set
 *  the sleep mode to either sleep_mode_never or sleep_mode_shallow to
 *  ensure that the 16MHz clock is not turned off when the radio is
 *  idle.
 *
 *  \param  fastest_available  If TRUE then the firmware will use the
 *      fastest available clock source for the PIO Controller. If
 *      FALSE, then the PIO Controller will always use the 32kHz
 *      clock.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void PioCtrlrClock(bool fastest_available);

/*! @} */
#endif /* __PIO_CTRLR_H__ */

