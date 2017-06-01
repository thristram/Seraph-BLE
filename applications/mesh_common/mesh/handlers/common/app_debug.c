/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_debug.c
 *
 *  DESCRIPTION
 *      Header definitions for debug defines
 *
 ******************************************************************************/

/*============================================================================*
 *  SDK Header File
 *============================================================================*/
#ifndef CSR101x_A05
#include <uart.h>           /* Functions to interface with the UART */
#include <uart_sdk.h>       /* Enums to interface with the UART */
#endif /* !CSR101x_A05 */

/*============================================================================*
 *  Local Header File
 *============================================================================*/
#include "app_debug.h"

#ifdef DEBUG_ENABLE
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#ifndef CSR101x_A05
/* Standard setup of CSR102x boards */
#define UART_PIO_TX 8
#define UART_PIO_RX 9
#define UART_PIO_RTS PIO_NONE
#define UART_PIO_CTS PIO_NONE
#endif /* !CSR101x_A05 */

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      AppDebugInit
 *
 *  DESCRIPTION
 *      This function initialises the application debug interface
 *
 *  RETURNS/MODIFIES
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void AppDebugInit(void)
{
#ifndef CSR101x_A05
    /* Configuration structure for the UART */
    uart_pio_pins_t uart;

    /* Standard setup of CSR102x boards */
    uart.rx  = UART_PIO_RX;
    uart.tx  = UART_PIO_TX;
    uart.rts = UART_PIO_RTS;
    uart.cts = UART_PIO_CTS;
    
    /* Initialise Default UART communications */
    DebugInit(1, UART_RATE_921K6, 0, &uart);
#else
    DebugInit(UART_BUF_SIZE_BYTES_256, UartDataRxCallback, NULL);
#endif /* !CSR101x_A05 */

    /* UART Rx threshold is set to 1,
     * so that every byte received will trigger the rx callback.
     */
    UartRead(1, 0);

    WriteString("In AppDebugInit");
}

#endif /* DEBUG_ENABLE */

