/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BleRadio.cpp
 *
 *  DESCRIPTION
 *      This class implements Radio interface functions.
 *
 ******************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "StdAfx.h"
#include "BleRadio.h"
#include <setupapi.h>
#include <Dbt.h>
/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CBleRadio
 *
 *  DESCRIPTION
 *      CBleRadio constructor
 *---------------------------------------------------------------------------*/
CBleRadio::CBleRadio(void)
{
    //Handles::handleSetConnection(0);
    mDongle = NULL;
    hDeviceNotify = NULL;
    m_hWnd = NULL;
    m_bSynergyInitialised = FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ~CBleRadio
 *
 *  DESCRIPTION
 *      CBleRadio destructor
 *---------------------------------------------------------------------------*/
CBleRadio::~CBleRadio(void)
{
    if(m_bSynergyInitialised)
    {
        CsrBleHostDeinit();
        m_bSynergyInitialised = FALSE;
    }

    if(mDongle)
    {
        delete mDongle;
        mDongle = NULL;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      RadioInit
 *
 *  DESCRIPTION
 *      This method intitialises the Bluetooth radio and controlling DLL
 *---------------------------------------------------------------------------*/
RADIO_FAILURES CBleRadio::RadioInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging)
{
    RADIO_FAILURES result = RADIO_FAIL_CLIENT_INIT;

    // Create a new dongle instance
    mDongle = new CUSBTransport();
    
    // Initialise the dongle and uEnergy Host stack
    if(mDongle->IsTransportReady())
    {
        result = uEnergyHostInit(hWnd, bleClientHandle, bLogging);
        m_bSynergyInitialised = TRUE;
        m_hWnd = hWnd;

        DEV_BROADCAST_DEVICEINTERFACE deviceDB;
        ZeroMemory(&deviceDB, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
        deviceDB.dbcc_size = sizeof(deviceDB);
        deviceDB.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        deviceDB.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
        hDeviceNotify = RegisterDeviceNotification(m_hWnd, &deviceDB,
           DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      RadioDeInit
 *
 *  DESCRIPTION
 *      Deinitializes the radio during shutdown
 *---------------------------------------------------------------------------*/
void CBleRadio::RadioDeInit()
{
   if(m_bSynergyInitialised)
   {
      CsrBleHostDeinit();
      m_bSynergyInitialised = FALSE;
   }

   UnregisterDeviceNotification(hDeviceNotify);
   hDeviceNotify = NULL;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      uEnergyHostInit
 *
 *  DESCRIPTION
 *      This method initialises the radio device.
 *---------------------------------------------------------------------------*/
RADIO_FAILURES CBleRadio::uEnergyHostInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging)
{
   CSR_BLE_TRANSPORT transport;
   RADIO_FAILURES result = RADIO_INIT_SUCCESS;

   // If the device is not already being used by other application, go
   // ahead and initialize the BLE Stack.
   transport.transportType = CSR_BLE_TRANSPORT_USB;
   transport.usbDeviceNumber = CSR_BLE_DEFAULT_USB_DEVICE_NUMBER;

   if(0 == (bleClientHandle = CsrBleHostInit(bLogging, NULL, &transport)))
   {
      //CsrBleHostInit Failed
      result = RADIO_FAIL_CLIENT_INIT;
      bleClientHandle = 0;
   }

   if(!CsrBleHostStartWnd(hWnd))
   {
      //CsrBleHostStartWnd Failed
      result = RADIO_FAIL_CLIENT_START;
      bleClientHandle = 0;
   }

   return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IsRadioAvailable
 *
 *  DESCRIPTION
 *      Check if the radio is present
 *---------------------------------------------------------------------------*/
BOOL CBleRadio::IsRadioAvailable()
{
   if(mDongle)
   {
      return mDongle->IsTransportReady(FALSE);
   }
   return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      IsRadioBusy
 *
 *  DESCRIPTION
 *      Check if the radio is busy
 *---------------------------------------------------------------------------*/
BOOL CBleRadio::IsRadioBusy()
{
   return mDongle->IsDeviceBusy();
}