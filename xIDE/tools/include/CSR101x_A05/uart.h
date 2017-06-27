/*! \file uart.h
 *
 *  \brief Functions to interface with the chip's UART
 *
 * Copyright (c) 2009 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __UART_H__
#define __UART_H__

#include "types.h"

/*! \addtogroup UART
 *
 * \brief The CSR1000 includes a basic UART, capable of running across a wide
 * range of baud rates. The UART driver can be configured to operate in unpacked
 * (byte) mode or packed (word) mode, and utilises an non-blocking (interrupt
 * driven) mechanism to read data received from the UART.
 * The CSR10xx UART does not support RTS/CTS hardware flow control.
 * @{
 */
/*! \ingroup UART
 *  \defgroup UartBaudrate UART baudrate
 *  @{
 *  \brief Defines used to configure the baud rate of the UART
 *
 *  If UART_RATE_DEFAULT is used as the baud_rate_enum parameter, then the
 *  rate and UART config are defined by the current CS settings.
 *
 *  Otherwise, baud_rate_enum is selected from the values below, and config
 *  is a bitfield described by \ref UartConfig.  Non-standard baud
 *  rates can be configured; contact CSR for more information.
 *
 *      - UART_RATE_2K4	    = 0x000a
 *      - UART_RATE_9K6	    = 0x0028
 *      - UART_RATE_19K2    = 0x004e
 *      - UART_RATE_38K4    = 0x009e
 *      - UART_RATE_57K6    = 0x00eb
 *      - UART_RATE_115K2	= 0x01d9
 *      - UART_RATE_230K4	= 0x03af
 *      - UART_RATE_460K8	= 0x0760
 *      - UART_RATE_921K6	= 0x0ebf
 *      - UART_RATE_1382K4	= 0x161f
 *      - UART_RATE_1843K2	= 0x1d7e
 *      - UART_RATE_2764K8	= 0x2c3d
 *      - UART_RATE_3686K4	= 0x3afc
 *
 *      - UART_SLOW_RATE_2K4 = 0x12c0
 *
 * Baud rates defined as UART_SLOW_RATE_X indicate rates that
 * drive the UART from the 32kHz clock rather
 * than the 16MHz crystal. This has the advantage that the
 * UART can be used when Deep Sleep is enabled without
 * having to wake the chip via the WAKE pin before transmitting
 * to it. However, the disadvantage is that only a limited set
 * of low baud rates are available in this mode. If a higher
 * baud rate is required then either Deep Sleep cannot be used
 * or the Host processor must assert WAKE before transmitting.
 */
#define UART_RATE_DEFAULT           0
/*! @} */

/*! \ingroup UART
 *  \defgroup UartConfig UART configuration
 *  @{
 *  \brief Defines used to configure the UART
 *
 *      - 0 - if set, use two stop bits
 *      - 1 - if set, use one parity bit
 *      - 2 - if set, use even parity
 *      - 6 - if set, forces the transmit output to zero
 *      - all other bits are reserved and must be left clear.
 */
#define UARTCFG_TWO_STOP_BITS       0x01
#define UARTCFG_ONE_PARITY_BIT      0x02
#define UARTCFG_EVEN_PARITY         0x04
#define UARTCFG_FORCE_ZERO_OUT      0x40
/*! @} */

/*! \brief UART buffer size, in bytes.
 *
 * These constants represent the possible size of UART buffers that be created.
 * No other buffer sizes are supported by the UART interface.
 */
typedef enum
{
    UART_BUF_SIZE_BYTES_32  = 0,    /*!< 32 byte buffer */
    UART_BUF_SIZE_BYTES_64  = 1,    /*!< 64 byte buffer */
    UART_BUF_SIZE_BYTES_128 = 2,    /*!< 128 byte buffer */
    UART_BUF_SIZE_BYTES_256 = 3     /*!< 256 byte buffer */
} uart_buf_size_bytes;

/*! \brief UART data mode.
 *
 * The UART can be operated in packed or unpacked mode. By default the UART is
 * unpacked, but the application can change this by calling \ref UartConfig.
 * The data mode affects the type of data passed from application to UART via
 * the \ref UartWrite or \ref UartWriteBlocking functions, and the type of data
 * passed back from the UART to application in the UartRead() callback function.
 *
 * The data mode does not affect the TX & RX buffers passed to \ref UartInit -
 * these buffers are always packed uint16 buffers, with the UART driver
 * unpacking & packing data as required.
 */
typedef enum
{
    /* Do not change these enumeration values - see uart.c */
    uart_data_packed   = 0x01,  /*!< UART data is packed (arrays of uint16) */
    uart_data_unpacked = 0x02,  /*!< UART data is unpacked (arrays of uint8) */
} uart_data_mode;

/*! \brief Declare RAM buffer for UART RX or UART TX operation.
 *
 * This macro creates a buffer to be used by the UART interface for transmit
 * or receive operations. The application is required to create a pair of
 * buffers, one for RX and one for TX. The size of the buffers should be defined
 * using one of the constants defined in \ref uart_buf_size_bytes. The RX & TX
 * buffers do not need to be the same size.
 *
 * \warning The GCC alignment attribute is required when declaring buffers to
 * ensure that the buffers meets the alignment requirements of the hardware.
 *
 * \param _name The name of the buffer
 * \param _size The size of the buffer, using one of the constants from \ref uart_buf_size_bytes
 *
 */
#define UART_DECLARE_BUFFER(_name, _size)   \
        static uint16 _name[16<<(_size)] GCC_ATTRIBUTE(aligned (2));

/*! \brief Receive handler function
 *  
 *  A function that will be called whenever \c length number of 
 *  bytes (which is set by \ref uart_fetch_data or 
 *  \ref DebugInit) have been received over the UART.  
 *
 *  The function takes three parameters: a pointer to a \c void buffer of the
 *  received data (which will be a uint8* if the UART data mode is 'unpacked'
 *  or a uint16* if the UART data mode is 'packed'), a \c uint16 containing
 *  the number of bytes ('unpacked') or words ('packed') received, and a
 *  pointer to a \c uint16 which should be filled in with the number of
 *  additional bytes ('unpacked') or words ('packed') the application wishes
 *  to receive (set to 0 if no further data is required at this time).
 *
 *  The function must return the number of bytes ('unpacked') or words ('packed')
 *  that the application has processed out of the available data (which may be
 *  less than was originally provided to the application). If the application
 *  does not process all of the data then the remaining data will remain in the
 *  buffer until a further amount of data requested by the application has been
 *  received.
 *
 *  NOTE: The application does not have to check for wrapping within the UART RX
 *  circular buffer. The memory management hardware ensures that the the data
 *  pointer supplied to the application presents the received data sequentially.
 *
 *  If the callback function is NULL, it indicates that the application is not
 *  interested in receiving data.
 */
typedef uint16 (*uart_data_in_fn)(void*, uint16, uint16*);

/*! \brief Transmit handler function 
 *  
 *  A function that will be called whenever a UART 
 *  transmission has finished.  The function takes
 *  no parameters and must return no value.  
 *
 *  May be NULL, which indicates that these events are 
 *  uninteresting to the application.
 */
typedef void (*uart_data_out_fn)(void);


/*----------------------------------------------------------------------------*
 *  UartInit
 */
/*! \brief Initialise the UART interface
 *
 * Sets the function pointers to be called on data in/out events. Defines the
 * TX & RX buffers (start address and size for each buffer). RX & TX buffers
 * are normally declared using the \ref UART_DECLARE_BUFFER macro. If the
 * application wants to use the UART in 'packed' mode (see \ref UartConfig) it
 * must still provide the length of the TX & RX buffers to this function in
 * bytes. The rx_size and tx_size parameters are an enum \ref uart_buf_size_bytes
 * to restrict the possible values that these parameters can accept.
 *
 * This function also configures the UART with the default settings stored in
 * the CS keys. If the application wishes to change the port configuration it
 * should call \ref UartConfig after calling this function.
 *
 * The application may provide NULL pointers for the data in and/or data out
 * events, if it it does not care about receiving and/or transmitting data.
 * For example, a simple debug interface that only uses the UART to send
 * debug messages would not need an RX callback at all, and would likely not
 * need a TX callback either.
 *
 * \warning The UART interface only supports buffers of size 32, 64, 128, or 256
 * bytes. If the application tries to create a buffer that is a different size
 * the UART interface will generate a fault.
 *
 *  \param data_in_clbk     Pointer to a function of type \ref uart_data_in_fn.
 *  \param data_out_clbk    Pointer to a function of type \ref uart_data_out_fn.
 *  \param rx_buffer        Pointer to the RX (read) buffer
 *  \param rx_size_bytes    Size of the RX buffer
 *  \param tx_buffer        Pointer to the TX (write) buffer
 *  \param tx_size_bytes    Size of the TX buffer
 *  \param data_mode        Desired data packing mode for UART (unpacked or packed)
 */
/*---------------------------------------------------------------------------*/
extern void UartInit(uart_data_in_fn data_in_clbk,
                     uart_data_out_fn data_out_clbk,
                     uint16* rx_buffer, uart_buf_size_bytes rx_size_bytes,
                     uint16* tx_buffer, uart_buf_size_bytes tx_size_bytes,
                     uart_data_mode new_data_mode);


/*----------------------------------------------------------------------------*
 *  UartEnable
 */
/*! \brief Enable/disable UART interface hardware
 *
 *  Enabling the UART interface defaults to waking the CSR1000 when it
 *  receives RX data (see \ref SleepWakeOnUartRX).
 *
 * \param enable TRUE to enable, FALSE to disable.
 */
/*---------------------------------------------------------------------------*/
extern void UartEnable(bool enable);


/*----------------------------------------------------------------------------*
 *  UartConfig
 */
/*! \brief Configure the UART baud rate and port configuration.
 *
 *  After calling this function the UART will be left disabled. The caller must
 *  therefore call \ref UartEnable before transmitting or receiving any data.
 *
 *  If the baud rate is set to UART_RATE_DEFAULT then the baud rate and port
 *  configuration will be read from the corresponding Configuration Store keys.
 *
 *  \param baud_rate_enum Described by \ref UartBaudrate.
 *  \param config A 16-bit bitfield described by \ref UartConfig.
 */
/*---------------------------------------------------------------------------*/
extern void UartConfig(uint16 baud_rate_enum, uint16 config);


/*----------------------------------------------------------------------------*
 *  UartRead
 */
/*! \brief Read the specified amount of data from the UART
 *
 *  Requests that the UART driver returns the specified amount of received
 *  UART data to the application once said amount of data is available. The
 *  actual data is made available to the caller via the \c data_in_clbk
 *  function that was registered with \ref UartInit.
 *
 *  The length parameter depends on the current UART data mode ('unpacked' or
 *  'packed'). For packed data, the length is the number of words to read,
 *  therefore the UART driver will wait until an even number of bytes have been
 *  received over the wire (for example if the application has set 'packed' mode
 *  and requests length=4, the driver will wait until 4 words have been received
 *  (8 bytes) before calling the \c data_in_clbk callback function). When the
 *  UART data mode is 'unpacked' the length parameter is the number of bytes
 *  to receive.
 *
 *  If the application has not provided an RX callback function in UartInit()
 *  then requesting a read of an amount of data will result in that data
 *  being read from the UART buffer and discarded. However, as the application
 *  does not receive any event when this happens it should not be used to
 *  "empty" the UART buffer, as there is no way for the application to
 *  determine exactly how much data has been discarded.
 *
 *  \warning The data pointer passed to the application in the \c data_in_clbk
 *  must not be used directly for writing data straight back to the UART. If the
 *  application needs to implement a loopback mechanism it must first copy the
 *  received data to a local buffer and then use that as the source for the
 *  data passed to \ref UartWrite.
 *
 *  \param length Amount of data to get (in bytes or words depending on UART data mode).
 *  \param timeout Not currently used. Must always be set to 0 by the application
 */
/*---------------------------------------------------------------------------*/
extern bool UartRead(uint16 length, uint32 timeout);


/*----------------------------------------------------------------------------*
 *  UartWrite
 */
/*! \brief Write a number of data bytes/words to the UART.
 *
 *  Given a pointer to an array, write this data into the UART's transmit
 *  buffer and initiate the UART transmit. The length parameter is the number
 *  of array "elements" to be written - depending on the UART mode this is
 *  either the number of bytes (uart_data_unpacked) or number of words
 *  (uart_data_packed).
 *
 *  The size of each element of array is defined by the current data mode of
 *  the UART (see \ref UartInit and \ref uart_data_mode). If the data mode
 *  is \c uart_data_unpacked then the \c data pointer is assumed to point to
 *  an array of (unpacked) uint8 data, and the LSB of each element will be
 *  copied into the UART transmit buffer. If the data mode is \c uart_data_packed
 *  then the \c data pointer is assumed to point to (packed) uint16 data and
 *  the LSB and MSB of each word will be copied into the UART transmit buffer,
 *  LSB first.
 *
 *  This function will return without writing any data if the internal
 *  transmit buffer does not have enough space to store all of the data. To
 *  ensure that the data is written on the first attempt (blocking write
 *  behaviour), the \ref UartWriteBlocking function must be used instead. (The
 *  application must not use a while() loop to "poll" the UartWrite() function
 *  as doing so will not allow the UART to clean up after existing bytes have
 *  been transmitted).
 *
 *  INTERRUPTS: This function is non-reentrant.
 *
 *  \warning The data pointer passed to the application in the \c data_in_clbk
 *  must not be used directly for writing data straight back to the UART. If the
 *  application needs to implement a loopback mechanism it must first copy the
 *  received data to a local buffer and then use that as the source for the
 *  data passed to \ref UartWrite.
 *
 *  \param data Pointer to the data buffer
 *  \param length Length of data
 *
 *  \returns TRUE on success or FALSE if there was insufficient space in the buffer.
 */
/*---------------------------------------------------------------------------*/
extern bool UartWrite(const void *data, uint16 length);


/*----------------------------------------------------------------------------*
 *  UartWriteBlocking
 */
/*! \brief Write a number of data bytes/words to the UART, blocking until all
 * bytes have been copied to the UART transmit buffer.
 *
 * This function behaves very much like \ref UartWrite, with the exception
 * that it will not return until all the data has been copied to the UART
 * transmit buffer. Therefore there is no return value (it will always
 * succeed).
 *
 * NOTE: If the buffer is full when called, then the time taken to return will
 * depend on how much data needs to be emptied before the all of the new data
 * can be stored in the UART buffer, and the baud rate of the UART (which
 * controls how quickly the hardware can transmit the contents of the transmit
 * buffer over the wire).
 *
 * INTERRUPTS: This function is non-reentrant.
 *
 * \warning This function will return when all supplied bytes have been
 * buffered by the UART. This is not the same as having actually transmitted
 * all bytes over the wire.
 *
 * \param data Pointer to the data buffer
 * \param length Length of data
 *
 * \returns Nothing
 */
/*---------------------------------------------------------------------------*/
extern void UartWriteBlocking(const void *data, uint16 length);


/*----------------------------------------------------------------------------*
 *  UartTxIsBusy
 */
/*! \brief Check the UART for TX activity
 *
 *  Returns TRUE if the UART is transmitting a packet.
 *
 *  \warning This function is not intended for customer use.
 */
/*---------------------------------------------------------------------------*/
extern bool UartTxIsBusy(void);


/*! @} */
#endif /* __UART_H__ */

