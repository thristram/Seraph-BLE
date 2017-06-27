/*! \file ble_hci_test.h
 *  \brief Defines common functions that provide control over RF test functions,
 *  that can be used directly by applications or indirectly via the the BLE
 *  2-wire Direct Test mode.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 */

#ifndef __BLE_HCI_TEST_H__
#define __BLE_HCI_TEST_H__

#include "types.h"


/*! \addtogroup TST
 *
 *  \brief These functions allow the application to enable the BLE 2-wire
 *  Direct Test Mode, or to manually call directly into the Host Controller
 *  test modes for LE Transmit and LE Receive tests.
 *
 *  Direct Test Mode can either be built into a standalone application or can
 *  be integrated into a end product application. This can be possible even if
 *  the application normally uses the UART (or its PIOs) for something else. For
 *  example, if a product has one spare PIO, this PIO can be used in a
 *  production test environment to provide a switch checked by the application
 *  to determine if Direct Test Mode is required. This check can be performed
 *  in AppInit() before the system has fully initialised.
 *
 *  Further information on Direct Test Mode can be found in the Bluetooth
 *  Specification v4.0 Volume 6 Part F.
 *
 *  Further information on the Host Controller test modes can be found in the
 *  Bluetooth Specification v4.0 Volume 2 Part E sections 7.8.28 - 7.8.30.
 *
 * @{
 */


/*! \brief Supported packet payloads for BLE Transmit test.
 */
typedef enum
{
    /*! Pseudo-Random Bit Sequence 9 */
    ble_test_pkt_prbs9,

    /*! Pattern of alternating bits '11110000' */
    ble_test_pkt_11110000,

    /*! Pattern of alternating bits '10101010' */
    ble_test_pkt_10101010,

    /*! Pseudo-Random Bit Sequence 15 */
    ble_test_pkt_prbs15,

    /*! Pattern of all '1' bits */
    ble_test_pkt_all_1,

    /*! Pattern of all '0' bits */
    ble_test_pkt_all_0,

    /*! Pattern of alternating bits '00001111' */
    ble_test_pkt_00001111,

    /*! Pattern of alternating bits '01010101' */
    ble_test_pkt_01010101,

} ble_test_pkt_type;


/****************************************************************************
 *  DirectTestReceive
 */
/*! \brief Start radio test mode, receiving test packets generated by a
 *  Bluetooth tester.
 *
 *  The caller should supply the RF channel number to receive packets on. The
 *  valid range for the channel is 0x00 to 0x27, which corresponds to a
 *  frequency range of 2402MHz - 2480MHz, calculated as F = (2*rx_channel) + 2402.
 *
 *  This function should not be called if a test is already running.
 *
 *  \warning THIS FUNCTION IS FOR RF TESTING DURING DEVELOPMENT AND PRODUCTION TEST.
 *  It must not be used during normal application operation. After calling this
 *  and the other BLE Test functions, the radio may be in an unknown state
 *  therefore a device reset is recommended.
 *
 *  \param rx_channel  Channel number to use for the test
 *
 *  \returns  Status of operation
 */
/****************************************************************************/
extern sys_status DirectTestReceive(uint8 rx_channel);


/****************************************************************************
 *  DirectTestReceiveResults
 */
/*! \brief Return the number of packets received since the start of the test.
 *  If the passed in pointer is not NULL use that to store the averaged RSSI.
 *
 *  \param p_average_rssi  Pointer to variable to hold averaged RSSI value
 *  or NULL, if the RSSI is not required
 *
 *  \returns  Number of packets
 */
/****************************************************************************/
extern uint16 DirectTestReceiveResults(int16 *p_average_rssi);


/****************************************************************************
 *  DirectTestTransmitCount
 */
/*! \brief Return the number of packets transmitted since the start of the test.
 *
 *  For now, the RX and TX counts are shared, so this function redirects to
 *  DirectTestReceiveResults(). However, this may change in future, so
 *  applications should always call DirectTestTransmitCount() to get the
 *  transmit count.
 *
 *  \returns  Number of packets
 */
/****************************************************************************/
#define DirectTestTransmitCount()   DirectTestReceiveResults(NULL)


/****************************************************************************
 *  DirectTestTransmit
 */
/*! \brief Start radio test mode, transmitting test packets to a Bluetooth tester.
 *
 *  The caller should supply the RF channel number to transmit packets on. The
 *  valid range for the channel is 0x00 to 0x27, which corresponds to a
 *  frequency range of 2402MHz - 2480MHz, calculated as F = (2*rx_channel) + 2402.
 *
 *  The payload length ranges from 0x00 to 0x25 bytes. All standard Bluetooth
 *  test packet types are supported.
 *
 *  The caller can also provide the number of packets to transmit. If the test
 *  should run indefinitely, set the count to 0, and the test will only stop
 *  transmitting if DirectTestEnd() is called.
 *
 *  This function should not be called if a test is already running.
 *
 *  \warning THIS FUNCTION IS FOR RF TESTING DURING DEVELOPMENT AND PRODUCTION TEST.
 *  It must not be used during normal application operation. After calling this
 *  and the other BLE Test functions, the radio may be in an unknown state
 *  therefore a device reset is recommended.
 *
 *  \param tx_channel  Channel number to use for the test
 *  \param payload_length  Number of bytes to transmit in the data payload
 *  \param payload_type  Data payload format
 *  \param num_packets  Number of packets to transmit, or 0 for indefinite
 *
 *  \returns  Status of operation
 */
/****************************************************************************/
extern sys_status DirectTestTransmit(uint8 tx_channel,
                                     uint8 payload_length,
                                     ble_test_pkt_type payload_type,
                                     uint16 num_packets);


/****************************************************************************
 *  DirectTestEnd
 */
/*! \brief End the current radio test and return the number of packets processed.
 *
 *  This function can only be called when a test is running.
 *
 *  \param num_packets  Pointer to storage for number of packets processed
 *
 *  \returns  Status of operation
 */
/****************************************************************************/
extern sys_status DirectTestEnd(uint16 *num_packets);


/****************************************************************************
 *  TestSetXtalTrim
 */
/*! \brief Extended test function to allow the application to adjust the 16MHz
 *  crystal trim value.
 *
 *  Legitimate values for the xtal_trim parameter are 0x00 to 0x3F (bits 0 to 5).
 *  Any other bits set in the xtal_trim parameter will be ignored.
 *
 *  This function is typically used in conjunction with TestEnableCarrierWave()
 *  to allow the crystal frequency to be measured and trimmed, to get as close
 *  to an ideal value as possible. Once the best value has been found it should
 *  be written to the device Configuration Store (using appropriate tools on
 *  the host PC).
 *
 *  \warning This function must only be called during production test. Adjusting
 *  this parameter during normal operation could result in the chip operating
 *  in a non-compliant manner.
 *
 *  \param xtal_trim  Crystal trim value in range 0x00 to 0x3F
 *
 *  \returns  Nothing
 */
/****************************************************************************/
extern void TestSetXtalTrim(uint16 xtal_trim);


/****************************************************************************
 *  TestGetXtalTrim
 */
/*! \brief Read the current crystal trim value from the hardware and return
    the value to the caller.
 *
 *  Legitimate values for the xtal_trim parameter are 0x00 to 0x3F (bits 0 to 5).
 *
 *  This function is only for use during production test. During normal
 *  operation the crystal trim value will not change.
 *
 *  \returns  Crystal trim value in range 0x00 to 0x3F
 */
/****************************************************************************/
extern uint16 TestGetXtalTrim(void);


/****************************************************************************
 *  TestEnableCarrierWave
 */
/*! \brief Extended test function to allow the application to request continuous
 *  transmission of an unmodulated carrier wave on a given RF channel.
 *
 *  Legitimate values for the rf_channel parameter are 0 to 39. If the
 *  application attempts to use a channel higher than 39 the request will be
 *  ignored. The carrier wave transmission can be disabled by calling
 *  TestDisableCarrierWave().
 *
 *  \warning This function must only be called during production test.
 *
 *  \param rf_channel  RF channel range 0-39
 *
 *  \returns  Nothing
 */
/****************************************************************************/
extern void TestEnableCarrierWave(uint16 rf_channel);


/****************************************************************************
 *  TestDisableCarrierWave
 */
/*! \brief Extended test function to allow the application to stop transmitting
 *  an unmodulated carrier wave.
 *
 *  This function only guarantees to stop radio transmission. It will leave the
 *  radio hardware and other parts of the chip in an undefined state unsuitable
 *  for normal operation. After CW testing has been completed it is recommended
 *  that the chip is reset using WarmReset().
 *
 *  \warning This function must only be called during production test.
 *
 *  \returns  Nothing
 */
/****************************************************************************/
extern void TestDisableCarrierWave(void);

/*! @} */

#endif /* __BLE_HCI_TEST_H__ */
