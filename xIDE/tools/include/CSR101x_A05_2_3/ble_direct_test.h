/*! \file ble_direct_test.h
 *  \brief Implements the BLE 2-wire Direct Test mode, using the UART.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 */

#ifndef __BLE_DIRECT_TEST_H__
#define __BLE_DIRECT_TEST_H__

#include "types.h"
#include "ble_hci_test.h"
#include "sleep.h"


/*! \addtogroup TST
 *
 * @{
 */


/****************************************************************************
 *  DirectTestInit
 */
/*! \brief Initialise the Bluetooth low energy 2-wire UART Direct Test Mode.
 *
 *  This function configures the UART for Direct Test Mode. While enabled the
 *  firmware will automatically handle all test commands received over the UART
 *  and start or stop the requested test mode.
 *
 *  The normal way to terminate Direct Test Mode is to perform a device reset
 *  (by sending the LE_Reset from the tester, or if the application calls
 *  WarmReset()). If the application manually reconfigures the UART without
 *  first resetting (e.g. directly calling UartInit(), or by calling DebugInit()),
 *  the test mode handler will be disabled, but there is a risk that a radio
 *  test will remain active.
 *
 *  The UART data rate and configuration is taken from the CS.
 *
 *  \warning After calling this function it is recommended that the
 *  application does not directly call DirectTestReceive(), DirectTestTransmit()
 *  or DirectTestEnd(), as this could interfere with any tests initiated by an
 *  external tester via the UART.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void DirectTestInit(sleep_state last_sleep_state);


/****************************************************************************
 *  DirectTestExtended
 */
/*! \brief Enable or disable CSR1000 Extended Direct Test Mode.
 *
 *  Extended Direct Test Mode allows the test system to use proprietary
 *  commands for trimming the 16MHz crystal or continuously transmitting a
 *  Carrier Wave on a specific RF channel. As these functions may interfere
 *  with third-party test equipment the application needs to enable this mode
 *  before it can be used.
 *
 *  This function should be called after calling DirectTestInit(). Extended
 *  Direct Test Mode is disabled if DirectTestInit() is called again.
 *
 *  \param enable  TRUE to enable Extended Direct Test Mode, or FALSE to disable.
 *
 *  \returns  Nothing.
 */
/****************************************************************************/
extern void DirectTestExtended(bool enable);


/*! @} */

#endif /* __BLE_DIRECT_TEST_H__ */
