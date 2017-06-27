/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      USBTransport.cpp
 *
 *  DESCRIPTION
 *      Implements the USB Transport related interafces
 *
 ******************************************************************************/
#include "StdAfx.h"

/*=============================================================================
 *  SDK Header Files
 *============================================================================*/
#include <setupapi.h>

/*=============================================================================
 *  Local Header Files
 *============================================================================*/
#include "resource.h"
#include "USBTransport.h"

/*============================================================================*
 *  Private Data
 *============================================================================*/
const GUID CUSBTransport::GUID_DEVINTERFACE_USB_DEVICE = 
   {0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED};

/*=============================================================================
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
*  NAME
*      ParseDeviceList
*
*  DESCRIPTION
*      This method parses the radio device connected. returns true or false
*---------------------------------------------------------------------------*/
bool CUSBTransport::ParseDeviceList(HDEVINFO& hDevInfo, 
                                bool& bIsDriverLoaded,
                                SP_DEVINFO_DATA& deviceInfoData)
{
    bool bIsDevicePresent = false;
    DWORD i;
    SP_DEVINFO_DATA devInfoData = {0};
    
    
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    // Enumerate through all devices in Set.
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        LPTSTR buffer = NULL;
        DWORD bufferSize = 0;
        
        //If SetupDiGetDeviceRegistryProperty() is called with
        // buffer = NULL and bufferSize = 0, the function returns the
        // required size for the buffer in bufferSize.
        //
        SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData,
        SPDRP_HARDWAREID, NULL, (PBYTE)buffer,
        bufferSize, &bufferSize);

        // Double the size to avoid problems on W2k MBCS systems per
        // KB 888609.
        //
        buffer = (LPTSTR)LocalAlloc(LPTR, bufferSize * 2);
        memset(buffer, 0, bufferSize * 2);

        // Again call SetupDiGetDeviceRegistryProperty() is called with
        // proper values of 'buffer' and 'bufferSize'. The function returns
        // the required property in the 'buffer'.
        //
        SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, 
           SPDRP_HARDWAREID, NULL, (PBYTE)buffer, 
           bufferSize, &bufferSize);
        
        CString hardwareId(buffer);
        hardwareId.MakeLower();

        bool tempDevicePresent = false;
        if(hardwareId.Find(CSR_4_0_RADIO_HW_ID, 0) != -1)
        {
            tempDevicePresent = true;
            bIsDevicePresent = true;
        }

        LocalFree(buffer);
        buffer = NULL;

        // Set the bufferSize back to 0.
        bufferSize = 0;

        if(tempDevicePresent)
        {
            // Check whether the 'service' property of the USB device is
            // 'CSRBC', then the driver is considered to be loaded.
            //

            // If SetupDiGetDeviceRegistryProperty() is called with
            // buffer = NULL and bufferSize = 0, the function returns the
            // required size for the buffer in bufferSize.
            //
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData,
            SPDRP_SERVICE, NULL, (PBYTE)buffer,
            bufferSize, &bufferSize);

            // Double the size to avoid problems on W2k MBCS systems per
            // KB 888609.
            //
            buffer = (LPTSTR)LocalAlloc(LPTR, bufferSize * 2);

            // Again call SetupDiGetDeviceRegistryProperty() is called with
            // proper values of 'buffer' and 'bufferSize'. The function returns
            // the required property in the 'buffer'.
            //
            SetupDiGetDeviceRegistryProperty(hDevInfo, 
                                             &devInfoData,
                                             SPDRP_SERVICE, 
                                             NULL, 
                                             (PBYTE)buffer,
                                             bufferSize, 
                                             &bufferSize);

            CString service(buffer);
            service.MakeLower();

            if(service.Find(SERVICE_BLUESUITE_DRIVER, 0) != -1)
            {
                bIsDriverLoaded = true;
                deviceInfoData = devInfoData;
               
                //break;
            }
            LocalFree(buffer);
            buffer = NULL;
        }

        memset(&devInfoData, 0, sizeof(devInfoData));
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    }
    return bIsDevicePresent;
}



/*----------------------------------------------------------------------------*
*  NAME
*      CheckForDeviceAndDriver
*
*  DESCRIPTION
*      This method checks for drivers. Returns true or false
*---------------------------------------------------------------------------*/
bool CUSBTransport::CheckForDeviceAndDriver(bool& bIsDriverLoaded)
{
    bool bIsDevicePresent = false;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA deviceInfoData = {0};

    bIsDriverLoaded = false;

    // Create a HDEVINFO with all the presently connected devices via USB.
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL,
                              DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDevInfo == INVALID_HANDLE_VALUE) // No USB device is connected.
    {
        bIsDevicePresent = false;
    }
    else
    {
        bIsDevicePresent = ParseDeviceList(hDevInfo, bIsDriverLoaded, deviceInfoData);
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return bIsDevicePresent;
}

/*----------------------------------------------------------------------------*
*  NAME
*      IsDeviceBusy
*
*  DESCRIPTION
*      This method checks if the radio device is busy. Returns true or false
*---------------------------------------------------------------------------*/
bool CUSBTransport::IsDeviceBusy(void)
{
    // Check whether we can exclusively CreateFile(). If yes, then the device is
    // free. Otherwise, it's being used by some other process.
    //
    bool isTheDongleBusy = false;
    bool bIsDriverLoaded = false;
    SP_DEVINFO_DATA deviceOfInterest;
    SP_DEVICE_INTERFACE_DATA devInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetailData = NULL;
    HANDLE hAppHandle;
    DWORD requiredSize;

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, 
                                            NULL,
                                            NULL, 
                                            DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDevInfo != INVALID_HANDLE_VALUE) // Some USB device is connected.
    {
        // As this function will be called once it has been made sure that the
        // USB device is present and the driver is loaded, these values will not
        // be of importance here.

        ParseDeviceList(hDevInfo, bIsDriverLoaded, deviceOfInterest);

        if(bIsDriverLoaded)
        {
            devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

            SetupDiEnumDeviceInterfaces(hDevInfo, 
                                        &deviceOfInterest,
                                        &GUID_DEVINTERFACE_USB_DEVICE, 
                                        DWORD(0), 
                                        &devInterfaceData);

            // Because the size of the SP_DEVICE_INTERFACE_DETAIL_DATA
            // structure varies, you need to call
            // SetupDiGetDeviceInterfaceDetail two times. The first call
            // gets the buffer size to allocate for the
            // SP_DEVICE_INTERFACE_DETAIL_DATA structure. The second call
            // fills the allocated buffer with detailed information about
            // the interface.
            //
            
            SetupDiGetDeviceInterfaceDetail(hDevInfo, 
                                            &devInterfaceData, 
                                            NULL,
                                            0, 
                                            &requiredSize, 
                                            NULL);

            pDevDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredSize);

            pDevDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            if(SetupDiGetDeviceInterfaceDetail(hDevInfo, 
                                               &devInterfaceData,
                                               pDevDetailData, 
                                               requiredSize, 
                                               NULL, NULL))
            {
                hAppHandle = CreateFile(pDevDetailData->DevicePath,
                                        GENERIC_READ | GENERIC_WRITE, 
                                        0, 
                                        NULL,
                                        OPEN_EXISTING, 
                                        FILE_FLAG_OVERLAPPED, 
                                        NULL);

                if(GetLastError() == ERROR_BUSY)
                {
                    isTheDongleBusy = true;
                }
                else
                {
                    CloseHandle(hAppHandle);
                }
            }

            LocalFree(pDevDetailData);
            pDevDetailData = NULL;
        }
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    return isTheDongleBusy;
}


/*=============================================================================
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
*  NAME
*      CUSBTransport
*
*  DESCRIPTION
*      CTOR
*---------------------------------------------------------------------------*/
CUSBTransport::CUSBTransport(void)
{
}


/*----------------------------------------------------------------------------*
*  NAME
*      CUSBTransport
*
*  DESCRIPTION
*      DTOR
*---------------------------------------------------------------------------*/
CUSBTransport::~CUSBTransport(void)
{
}


/*----------------------------------------------------------------------------*
*  NAME
*      IsTransportReady
*
*  DESCRIPTION
*      This method checks if the transport is ready.
*---------------------------------------------------------------------------*/
bool CUSBTransport::IsTransportReady(BOOL bCheckBusy /*= TRUE */)
{
    bool driverLoaded = false;
    bool donglePresent;
    bool success = false;

    donglePresent = CheckForDeviceAndDriver(driverLoaded);

    if(donglePresent && driverLoaded)
    {
       if(bCheckBusy == FALSE || IsDeviceBusy() == FALSE)
       {
         success = true;
       }
       else
       {
          CString title, message;
          title.LoadString(IDS_APP_TITLE);
          message.LoadString(IDS_ANOTHER_PROCESS_USING_RADIO);
          AfxGetMainWnd()->MessageBox(message, title);
       }
    }
    else if(donglePresent == false)
    {
       CString title, message;
       title.LoadString(IDS_APP_TITLE);
       message.LoadString(IDS_RADIO_ABSENT);
       AfxGetMainWnd()->MessageBox(message,title );
    }
    else if(driverLoaded == false)
    {
       CString title, message;
       title.LoadString(IDS_APP_TITLE);
       message.LoadString(IDS_DRIVER_ABSENT);
       AfxGetMainWnd()->MessageBox(message,title );
    }

    return success;
}