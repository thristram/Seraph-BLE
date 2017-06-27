/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      USBtransport.h
 *
 *  DESCRIPTION
 *      Determines whether the required USB dongle radio is present.
 *
 ******************************************************************************/

#ifndef _USB_TRANSPORT_H
#define _USB_TRANSPORT_H

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <Guiddef.h>

/*============================================================================*
 *  Public Class Definitions
 *============================================================================*/
class CUSBTransport
{
protected:
    /*=============================================================================
     *  Private Definitions/Data
     *============================================================================*/
    // BLE Radio initilisation variables
    #define CSR_4_0_RADIO_HW_ID           (TEXT("vid_0a12&pid_0001"))
    #define SERVICE_BLUESUITE_DRIVER      (TEXT("csrbc"))
    #define BYTE_JOIN_WORD(_msb, _lsb)    ((WORD)_msb << 8 | (WORD)_lsb)

    /*============================================================================*
     *  Private Data
     *============================================================================*/
    static const GUID GUID_DEVINTERFACE_USB_DEVICE;

    /*============================================================================*
     *  Private Function Prototypes
     *============================================================================*/
    bool ParseDeviceList(HDEVINFO& hDevInfo, bool& bIsDriverLoaded, SP_DEVINFO_DATA& deviceInfoData);

public:
    CUSBTransport(void);
    ~CUSBTransport(void);

    bool IsDeviceBusy(void);
    bool IsTransportReady(BOOL bCheckBusy = TRUE);
    bool CheckForDeviceAndDriver(bool& bIsDriverLoaded);
};

#endif /* _USB_TRANSPORT_H */