/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      CBleRadio.h
 *
 *  DESCRIPTION
 *      This class implements the lowest-level communication with the DLL
 *      controlling the radio dongle.
 *
 ******************************************************************************/

#ifndef _CBLERADIO_H
#define _CBLERADIO_H

/*============================================================================*
*  SDK Header Files
*============================================================================*/

#include <regstr.h>
#include <setupapi.h>

/*============================================================================*
*  Local Header Files
*============================================================================*/
#include "UsbTransport.h"
#include "MessageDefs.h"
#include "uEnergyHost.h"
#include "Aes.h"

/*=============================================================================
*  Public definitions
*============================================================================*/

typedef enum {
   RADIO_INIT_SUCCESS = 0,
   RADIO_FAIL_CLIENT_INIT,     // Unable to initialize BLE client.
   RADIO_FAIL_CLIENT_START     // Unable to start BLE client
} RADIO_FAILURES;

const GUID GUID_DEVINTERFACE_USB_DEVICE = {0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00,
      0xC0, 0x4F, 0xB9, 0x51, 0xED};

class CBleRadio
{
public:

   /*=============================================================================
   *  Public function prototypes
   *============================================================================*/

   /* Class constructor/destructor */
   CBleRadio(void);
   ~CBleRadio(void);

   /* Ble radio initilisation method prototype */
   RADIO_FAILURES RadioInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging);
   void RadioDeInit();

   BOOL IsRadioAvailable();
   BOOL IsRadioBusy();

protected:

   /*=============================================================================
   *  Class Private definitions
   *============================================================================*/
   static const unsigned short MIN_CONN_INTERVAL = 0x18;
   static const unsigned short MAX_CONN_INTERVAL = 0x18;
   static const unsigned short SLAVE_LATENCY     = 0x00;
   static const unsigned short LINK_TIMEOUT      = 0x1F4;

   static const unsigned short FAST_MIN_CONN_INTERVAL = 0x06;
   static const unsigned short FAST_MAX_CONN_INTERVAL = 0x06;
   static const unsigned short FAST_SLAVE_LATENCY     = 0x00;
   static const unsigned short FAST_LINK_TIMEOUT     = 0x1F4;


   /*=============================================================================*
   *  Class Private Data
   *============================================================================*/

   /* Radio interface */
   CUSBTransport *mDongle;
   BOOL m_bSynergyInitialised;

   HWND m_hWnd;

   HDEVNOTIFY hDeviceNotify;
private:
   /*=============================================================================*
   *  Private Function Implementations
   *============================================================================*/
      /******************************************************************************
   * uEnergyHostInit 
   * This method initialises the radio device.
   */
   RADIO_FAILURES uEnergyHostInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging);


};

#endif /* _CBLERADIO_H */