/*! \file debug.h
 *  \brief Simple host interface to the uart driver
 *
 * Copyright (c) 2010 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "uart.h"

/*! \addtogroup DBG
 *
 *  \brief These functions provide a simple interface to the \ref UART driver,
 *  to allow an application to send basic text strings across the UART,
 *  primarily for sending debugging messages to a serial terminal. If this
 *  module is used then the application is unable to make direct use of the
 *  UART. The functions allow for writing a single character, a NUL-terminated
 *  string, and can also convert integers (8-, 16- or 32-bit and times (48-bit)
 *  to ASCII representation (as unsigned hexadecimal).
 *
 * @{
 */
/****************************************************************************
 *  DebugInit
 */
/*! \brief Set up the IO system.
 *
 *  \param  rx_threshold  Number of bytes of input required to trigger
 *     a call to \a rx_event_handler.  1 is a safe value to pass if
 *     the application does not intend to receive data.
 *
 *  \param  tx_event_handler  Pointer to a function of type 
 *     \ref uart_data_out_fn that will be called whenever a UART
 *     transmission has finished.
 *
 *  \param  rx_event_handler  Pointer to a function of type
 *     \ref uart_data_in_fn that will be called whenever the threshold 
 *     number of bytes have been received over the UART. 
 *
 *  \returns  Nothing.
 *
 *  This function configures how the application wishes to use the
 *  debug output.  For normal debug, pass \c 0, \c NULL, \c NULL
 *  as the parameters; the various DebugWrite functions will operate
 *  normally with those parameters.
 *
 *  If an application wishes to use the API to send debug messages, but also
 *  wishes to receive data (e.g. simple key presses from a PC-based terminal
 *  application) it may register an RX callback function to handle received
 *  bytes. No additional support is supplied for processing UART input, so it
 *  needs to be handled entirely by the application.
 *
 *  Note that the UART data rate and configuration is taken from the CS.
 */
/****************************************************************************/
void DebugInit(uint16 rx_threshold,
               uart_data_in_fn rx_event_handler,
               uart_data_out_fn tx_event_handler);


/****************************************************************************
 *  DebugWriteChar
 */
/*! \brief  Write an ASCII character (unsigned 8-bit value) to the UART.
 *
 *  \param  val  The character to send.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
void DebugWriteChar( char const val );


/****************************************************************************
 *  DebugWriteUint8
 */
/*! \brief Convert an 8-bit value into an ASCII string of two
 *      hexadecimal digits and send it to the UART.
 *
 *  \param  val  The value to convert and send.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
void DebugWriteUint8( uint8 const val );


/****************************************************************************
 *  DebugWriteUint16
 */
/*! \brief Convert a 16-bit value into an ASCII string of four
 *      hexadecimal digits and send it to the UART.
 *
 *  \param  val  The value to convert and send.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
void DebugWriteUint16(uint16 const val);


/****************************************************************************
 *  DebugWriteUint32
 */
/*! \brief Convert a 32-bit value into an ASCII string of eight
 *      hexadecimal digits and send it to the UART.
 *
 *  \param  val  The value to convert and send.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
void DebugWriteUint32(uint32 const val);


/****************************************************************************
 *  DebugWriteTime48
 */
/*! \brief Convert a 48-bit time value into an ASCII string of twelve
 *      hexadecimal digits and send it to the UART.
 *
 *  \param  val  The \c time48 value to convert and send.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
void DebugWriteTime48(uint16 const *val);


/****************************************************************************
 * DebugWriteString
 */
/*! \brief Write a C string (i.e. unpacked data) over the UART.
 *
 *  \param  string  The NUL-terminated string to send.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
void DebugWriteString(const char *string);


/****************************************************************************
 * DebugWriteData
 */
/*! \brief Write unpacked data over the UART.
 *
 *  \param  data  The \c uint8 data to send.
 *  \param  nbytes  The number of \e bytes of data to send.
 *
 *  \returns  Nothing.
*/
/****************************************************************************/
#define DebugWriteData(data, nbytes) UartWriteBlocking(data, nbytes)

/*! @} */
#endif /* __DEBUG_H__ */
