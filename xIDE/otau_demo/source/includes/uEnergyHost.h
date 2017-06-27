/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *
 *  FILE
 *      uEnergyHost.h
 *
 *  DESCRIPTION
 *      This file declares API for the CSR uEnergy host library.
 *
 *  DISCLAIMER
 *      uEnergy Host library beta is provided to the customer for evaluation
 *      purposes only and, as such early feedback on performance and operation
 *      is anticipated. The software library and code is subject to change and
 *      not intended for production. Use of developmental release software is
 *      at the user's own risk. This software is provided "as is," and CSR
 *      cautions users to determine for themselves the suitability of using the
 *      beta release version of this software. CSR makes no warranty or
 *      representation whatsoever of merchantability or fitness of the product
 *      for any particular purpose or use. In no event shall CSR be liable for
 *      any consequential, incidental or special damages whatsoever arising out
 *      of the use of or inability to use this software, even if the user has
 *      advised CSR of the possibility of such damages.
 *
 ******************************************************************************/

#ifndef __UENERGYHOST_H__
#define __UENERGYHOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

/*
 * API modifier
 */
#ifdef CSR_BLE_HOST_STATIC
#   define CSR_BLE_HOST_API extern
#else
#   ifdef CSR_BLE_HOST_EXPORTS
#       define CSR_BLE_HOST_API __declspec(dllexport)
#   else
#       define CSR_BLE_HOST_API __declspec(dllimport)
#   endif /* CSR_BLE_HOST_EXPORTS */
#endif /* CSR_BLE_HOST_STATIC */

#define CSR_BT_UUID16_SIZE_IN_BYTES                  (02)
#define CSR_BT_UUID32_SIZE_IN_BYTES                  (04)
#define CSR_BT_UUID128_SIZE_IN_BYTES                 (16)

// Response codes to be used in response to
// CSR_BLE_SERVER_DATABASE_READ_INDICATION and
// CSR_BLE_SERVER_DATABASE_WRITE_INDICATION
#define ACCESS_RESPONSE_SUCCESS                      (0)
#define ACCESS_ERROR_INVALID_HANDLE                  (01)
#define ACCESS_ERROR_READ_NOT_PERMITTED              (02)
#define ACCESS_ERROR_WRITE_NOT_PERMITTED             (03)
#define ACCESS_ERROR_INSUFFICIENT_ENCRYPTION         (0x0f)

/**
 *  \defgroup common Common definitions
 *  \defgroup initialization Initialisation
 *  \defgroup messages Messages
 *  \{
 *  \defgroup host_messages Host messages
 *  \defgroup gatt_client_messages GATT Client messages
 *  \}
 *  \defgroup host GATT common API
 *  \defgroup client GATT client API
 */

/**
 *  \mainpage   uEnergy Host library
 *              The CSR uEnergy Host library implements a Bluetooth Low Energy stack
 *              with master role support. Currently, only GATT client role is supported.
 *
 *              The library is provided as a Windows DLL which can be loaded at
 *              runtime by the application.
 *
 *              The application needs to provide the uEnergy Host library with a
 *              Windows thread id or window handle that will receive uEnergy messages.
 *              During initialisation, the uEnergy Host library provides the
 *              application with the message id that should be handled in the
 *              application message loop.
 *
 *              <i>uEnergy Host library 2.4.3.0 beta is provided to the customer
 *              for evaluation purposes only and, as such early feedback on
 *              performance and operation is anticipated. The software library 
 *              and code is subject to change and not intended for production.
 *              Use of developmental release software is at the user's own risk.
 *              This software is provided "as is," and CSR cautions users to
 *              determine for themselves the suitability of using the beta release
 *              version of this software. CSR makes no warranty or representation
 *              whatsoever of merchantability or fitness of the product for any
 *              particular purpose or use. In no event shall CSR be liable for
 *              any consequential, incidental or special damages whatsoever
 *              arising out of the use of or inability to use this software,
 *              even if the user has advised CSR of the possibility of such damages.</i>
 *
 *  \section library_initialization uEnergy Host library life cycle
 *
 *  The following diagram shows the life cycle of the uEnergy Host library.
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostInit()", URL="\ref #CsrBleHostInit()"];
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Initialise library"];
 *              Application<<"uEnergy Host Library" [label="initialisation result"];
 *              |||;
 *              Application=>"uEnergy Host Library" [label="CsrBleHostStart()", URL="\ref #CsrBleHostStart()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Initialise transport and start up host stack"];
 *              ...;
 *              --- [label=" Host stack is up and running "];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_READY", URL="\ref #CSR_BLE_HOST_READY"];
 *              ...;
 *              Application box "uEnergy Host Library" [label="\nApplication\noperation\n"];
 *              ...;
 *              Application=>"uEnergy Host Library" [label="CsrBleHostDeinit()", URL="\ref #CsrBleHostDeinit()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *  \endmsc
 *
 *  \section uEnergy_code_example uEnergy code example
 *              The source code below shows a typical uEnergy Host application
 *              structure. For the full example, please refer to the Examples
 *              section.
 *  \code
#include "uEnergyHost.h"

// link with the uEnergy Host library
#pragma comment( lib, "uEnergyHost" )

...

// shutdown flag
BOOL shutDown = FALSE;

// message id for the uEnergy Host library messages
UINT uEnergyHostMessage = 0;

// uEnergy Host library message handler
DWORD WINAPI ThreadProc(__in LPVOID lpParamter)
{
    MSG msg;
    BOOL bRet;

    // loop while shutdown is indicated
    while(!shutDown)
    {
        // get the next message
        while( (bRet = GetMessage(&msg, (HWND)-1, 0, 0)) != 0)
        { 
            if (bRet == -1)
            {
                // handle the error and possibly exit
            }
            else if(msg.message == uEnergyHostMessage)
            {
                // received the message from the uEnergy Host library
                switch(msg.wParam)
                {
                case CSR_BLE_HOST_READY:
                    // uEnergy Host library is intialized
                    break;

                ...

                default:
                    printf("Unhandled uEnergy Host library message (%X)\n", msg.wParam);
                    break;
                }

                // free the message contents
                CsrBleHostFreeMessageContents(msg.wParam, msg.lParam);
            } 
        } 
    }

    return 0;
}


// main initialisation code
int _tmain(int argc, _TCHAR* argv[])
{
    HANDLE thread;
    DWORD threadId;

    CSR_BLE_TRANSPORT transport;

    transport.transportType = CSR_BLE_TRANSPORT_USB;
    transport.usbDeviceNumber = CSR_BLE_DEFAULT_USB_DEVICE_NUMBER;

    // create a thread to listen to the uEnergy Host library messages
    if(NULL == (thread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadId)))
    {
        printf("Error creating thread (%X)\n", GetLastError());
        return -1;
    }

    // initialise uEnergy library
    if(0 == (uEnergyHostMessage = CsrBleHostInit(FALSE, NULL, &transport)))
    {
        printf("Unable to initialise uEnergy Host.\n");
        return -1;
    }

    // start the uEnergy Host stack and pass the listener thread id
    if(!CsrBleHostStart(threadId))
    {
        printf("Unable to start uEnergy Host.\n");
        return -1;
    }

    ...

    // deinitialise the uEnergy Host stack
    CsrBleHostDeinit();

    shutDown = TRUE;

    if(WaitForSingleObject(thread, 5000) == WAIT_TIMEOUT)
    {
        TerminateThread(thread, -1);
    }

    CloseHandle(thread);

    return 0;
}

 *  \endcode
 *
 */

/**
 *  \example main.c
 *  This example demonstrates basic GATT client implementation using uEnergy
 *  Host library.
 */

/* Top-level documentation */
/**
 *  \addtogroup initialization
 *  \{
 */

/**
 *  \brief Default USB device number
 *  \hideinitializer
 */
#define CSR_BLE_DEFAULT_USB_DEVICE_NUMBER       (0)

/**
 *  \name Transport type
 *  \anchor transport_type
 *  \{
 */

/**
 *  \brief USB transport
 *  \hideinitializer
 */
#define CSR_BLE_TRANSPORT_USB                   (0)     

/**
 *  \brief UART transport (BCSP protocol)
 *  \hideinitializer
 */
#define CSR_BLE_TRANSPORT_SERIAL_BCSP           (1)

/**
 *  \brief UART transport (H4DS protocol)
 *  \hideinitializer
 */
#define CSR_BLE_TRANSPORT_SERIAL_H4DS           (2)
/**
 *  \}
 */

/**
 *  \}
 */


/**
 *  \addtogroup common
 *  \{
 */

/**
 *  \brief      Ignore the function parameter
 */
#define CSR_BLE_PARAMETER_IGNORE                    (0xFFFF)

/**
 *  \}
 */

/**
 *  \addtogroup host_messages
 *  \{
 */

/**
 *  \anchor     panic_reasons
 *  \name       Panic reasons
 *      \{
 */

/**
 *  \brief      No panic
 *  \hideinitializer
 */
#define CSR_BLE_PANIC_NO_PANIC                      (0x0000)
/**
 *  \brief      Transport initialisation failed
 *  \hideinitializer
 */
#define CSR_BLE_PANIC_TRANSPORT_INIT_FAILURE        (0x0001)
/**
 *  \brief      Transport communication failure
 *  \hideinitializer
 */
#define CSR_BLE_PANIC_TRANSPORT_FAILURE             (0x0002)
/**
 *  \brief      The other panic reason
 *  \hideinitializer
 */
#define CSR_BLE_PANIC_UNKNOWN                       (0x0003)
/**
 *      \}
 */

/**
 *  \anchor information_reported
 *  \name   Information reported regarding the discovered device
 *      \{
 */

/**
 *  \brief  Shortened local name
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_SHORT_NAME            ((0x0001) << 0)
/**
 *  \brief  Complete local name
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_COMPLETE_NAME         ((0x0001) << 1)
/**
 *  \brief  Flags
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_FLAGS                 ((0x0001) << 2)
/**
 *  \brief  Complete list of the device services
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_COMPLETE_SERVICES     ((0x0001) << 3)
/**
 *  \brief  Incomplete list of the device services
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_INCOMPLETE_SERVICES   ((0x0001) << 4)
/**
 *  \brief  TX power level
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_TX_LEVEL              ((0x0001) << 5)
/**
 *  \brief  Security manager OOB flags
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_OOB                   ((0x0001) << 6)
/**
 *  \brief  Security manager TK value
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_TK                    ((0x0001) << 7)
/**
 *  \brief  Slave connection interval range
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_CONNECTION_INTERVAL   ((0x0001) << 8)
/**
 *  \brief  Service solicitation
 *  \hideinitializer
 *  \todo   Currently not supported
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_SERVICE_SOLICITATION  ((0x0001) << 9)     /* TODO: not supported */
/**
 *  \brief  Service data
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_SERVICE_DATA          ((0x0001) << 10)
/**
 *  \brief  Manufacturer specific data
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_SEARCH_MANUFACTURER          ((0x0001) << 11)

/**
 *  \brief  Appearance
 *  \hideinitializer
 *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
 *          Profile, Section 11.1.
 */
#define CSR_BLE_DEVICE_APPEARANCE                   ((0x0001) << 12)

/**
 *      \}
 */
/*************************************************************************************
    Defines Appearance AD types values. 
    The values are composed of a category (10-bits) and sub-categories (6-bits). 
    and can be found in assigned numbers
************************************************************************************/
#define CSR_BLE_APPEARANCE_UNKNOWN                  (0x0000)
#define CSR_BLE_APPEARANCE_HID_GENERIC              (0x03c0)                
#define CSR_BLE_APPEARANCE_HID_KEYBOARD             (0x03c1)       
#define CSR_BLE_APPEARANCE_HID_MOUSE                (0x03c2)    
#define CSR_BLE_APPEARANCE_HID_MFSW                 (0x03ca)

#define CSR_BLE_SECURITY_BONDING_ESTABLISHED        (0)
#define CSR_BLE_SECURITY_BONDING_FAILED             (1)
#define CSR_BLE_SECURITY_BONDING_TIMEOUT            (2)

#define CSR_BLE_SECURITY_DEFAULT                    (0)
#define CSR_BLE_SECURITY_UNAUTHENTICATED            (1)
#define CSR_BLE_SECURITY_AUTHENTICATED              (2)

/**
 *  \name Messages
 *      \{
 */

/**
 *  \brief      Host ready message
 *  \hideinitializer
 *  \details    This message is sent by the uEnergy Host library after
 *              #CsrBleHostStart() function is called and initialisation is
 *              complete. The host application should wait for this message
 *              before calling any other uEnergy functions.
 *
 *  \param      LPARAM
 *              TRUE in case of successful initialisation, FALSE otherwise.
 * *  \see        #CsrBleHostStart()
 */
#define CSR_BLE_HOST_READY                          (0x0000)

/**
 *  \brief      Host panic message
 *  \hideinitializer
 *  \details    This message is sent by the uEnergy Host library in response to the
 *              critical error (such as transport failure). The host
 *              application should deinitialise the uEnergy Host library and stop
 *              all further LE functionality.
 *  \param      LPARAM
 *              \ref panic_reasons "Panic reason".
 *          
 */
#define CSR_BLE_HOST_PANIC                          (0x0001)

/**
 *  \brief      Device discovery result
 *  \hideinitializer
 *  \details    These messages are sent in response to
 *              #CsrBleHostStartDeviceSearch() call. It contains information
 *              regarding the discovered device.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_DEVICE_SEARCH_RESULT, pointer to
 *              #CSR_BLE_DEVICE_SEARCH_RESULT structure.
 *
 *  \see        #CsrBleHostStartDeviceSearch()\n
 *              #CSR_BLE_DEVICE_SEARCH_RESULT
 */
#define CSR_BLE_HOST_SEARCH_RESULT                  (0x0002)

/**
 *  \brief      Device discovery stopped indication
 *  \hideinitializer
 *  \details    This messages is sent when device search initiated by
 *              #CsrBleHostStartDeviceSearch() call has stopped due to timeout
 *              or #CsrBleHostStopDeviceSearch() call.
 *
 *  \param      LPARAM
 *              Ignored.
 *
 *  \see        #CsrBleHostStartDeviceSearch()\n
 *              #CsrBleHostStopDeviceSearch()
 */
#define CSR_BLE_HOST_SEARCH_STOPPED                 (0x0003)


/**
 *  \brief      Connection indication
 *  \hideinitializer
 *  \details    This messages indicates the result of the connection request.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CONNECT_RESULT, pointer to
 *              #CSR_BLE_CONNECT_RESULT structure.
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostCancelConnect()\n
 *              #CSR_BLE_CONNECT_RESULT
 */
#define CSR_BLE_HOST_CONNECT_RESULT                 (0x0004)

/**
 *  \brief      Disconnection indication
 *  \hideinitializer
 *  \details    This messages is sent to the application when disconnection
 *              is detected. The disconnection could be triggered locally by
 *              call to #CsrBleHostDisconnect().
 *
 *  \param      LPARAM
 *              #PCSR_BLE_DISCONNECTED, pointer to
 *              #CSR_BLE_DISCONNECTED structure.
 *
 *  \see        #CsrBleHostDisconnect()\n
 *              #CSR_BLE_DISCONNECTED
 */
#define CSR_BLE_HOST_DISCONNECTED                   (0x0005)

/**
 *  \brief      Connection parameters updated
 *  \hideinitializer
 *  \details    This messages indicates the result of the connection parameter
 *              update request sent by #CsrBleHostUpdateConnectionParams().
 *
 *  \param      LPARAM
 *              Result of the operation. <b>0</b>, in case of success.
 *
 *  \see        #CsrBleHostSetConnectionParams()
 *              #CsrBleHostUpdateConnectionParams()
 */
#define CSR_BLE_HOST_CONN_PARAM_UPDATE              (0x0006)

/**
 *  \brief      RSSI measurement
 *  \hideinitializer
 *  \details    This messages is sent in response to #CsrBleHostGetRssi()
 *              call with the measured RSSI value for the specified device.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_RSSI_RESULT, pointer to #CSR_BLE_RSSI_RESULT
 *              structure.
 *
 *  \see        #CsrBleHostGetRssi()
 */
#define CSR_BLE_HOST_RSSI_RESULT                    (0x0007)

/**
 *  \brief      Debond Result
 *  \hideinitializer
 *  \details    This messages is sent in response to #CsrBleHostDebond()
 *              call with the address of the specified device.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_DEBOND_RESULT, pointer to #CSR_BLE_DEBOND_RESULT
 *              structure.
 *
 *  \see        #CsrBleHostDebond()
 */
#define CSR_BLE_HOST_DEBOND_RESULT                  (0x0008)

/**
 *  \brief      Just Works Request
 *  \hideinitializer
 *  \details    Request for accepting a pairing attempt. The application may respond 
 *              to the request or may forward the request to the user, e.g. through 
 *              the MMI. Note: This primitive is only applicable when both the local 
 *              and remote device supports SSP and MITM is not required by both parties.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_JUSTWORKS_REQUEST, pointer to #CSR_BLE_JUSTWORKS_REQUEST
 *              structure.
 *
 *  \see        #CsrBleHostSetEncryption()
 */
#define CSR_BLE_HOST_JUSTWORKS_REQUEST              (0x0009)

/**
 *  \brief      Security/Pairing Indication
 *  \hideinitializer
 *  \details    This indication is sent to the registered application whenever a Low
 *              Energy security procedure has completed either successfully or if the 
 *              procedure failed.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_SECURITY_RESULT, pointer to #CSR_BLE_SECURITY_RESULT
 *              structure.
 *
 *  \see        #CsrBleHostSetEncryption()
 */
#define CSR_BLE_HOST_SECURITY_RESULT                (0x000A)

/**
 *  \brief      Request LE Authentication Procedure
 *  \hideinitializer
 *  \details    This is a confirmation to the initiation of the security request from  
 *              the application.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_SET_ENCRYPTION_RESULT, pointer to #CSR_BLE_SET_ENCRYPTION_RESULT
 *              structure.
 *
 *  \see        #CsrBleHostSetEncryption()
 */
#define CSR_BLE_HOST_SET_ENCRYPTION_RESULT          (0x000B)

/**
 *  \brief      Connection Parameter Update Request
 *  \hideinitializer
 *  \details    The application will receive a CSR_BLE_HOST_CONNECTION_UPDATE_REQUEST 
 *              message whenever another local application or a peer Slave requests to 
 *              update the connection parameter into an interval that is outside the 
 *              range of which it has registered to GATT.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CONNECTION_UPDATE_REQUEST, pointer to 
 *              #CSR_BLE_CONNECTION_UPDATE_REQUEST structure.
 *
 *  \see        #CsrBleHostSetConnectionParams()
 */
#define CSR_BLE_HOST_CONNECTION_UPDATE_REQUEST      (0x000C)

/**
 *  \brief      Connection Parameter Update Indication
 *  \hideinitializer
 *  \details    The application will receive a CSR_BLE_HOST_CONNECTION_UPDATED 
 *              message whenever the actual connection parameters have changed. 
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CONNECTION_UPDATED, pointer to 
 *              #CSR_BLE_CONNECTION_UPDATED structure.
 *
 *  \see        #CsrBleHostSetConnectionParams()
 */
#define CSR_BLE_HOST_CONNECTION_UPDATED             (0x000D)

/**
 *  \brief      The indication contains a passkey that must be shown on the display 
 *              so that the remote side will be able to enter the same passkey in the
 *              responding side.  
 *
 *  \param      LPARAM
 *              #PCSR_BLE_DISPLAY_PASSKEY_IND, pointer to
 *              #CSR_BLE_DISPLAY_PASSKEY_IND structure.
 *
 *  \see        #CsrBleHostPasskeyNotificationResult()\n
 */
#define CSR_BLE_HOST_DISPLAY_PASSKEY_IND            (0x000E)

/**
 *  \brief      The indication is confirmation to the GATT scan request initiated 
 *              by the application.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_MSG_NOTIFY, pointer to
 *              #CSR_BLE_MSG_NOTIFY structure.
 *
 *  \see        #CsrBleHostStartLEScan()\n
 */
#define CSR_BLE_HOST_LE_SCAN_STATUS                 (0x000F)


/**
 *  \brief      This is confirmation to any GATT event send request initiated 
 *              by the application.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_MSG_NOTIFY, pointer to
 *              #CSR_BLE_MSG_NOTIFY structure.
 *
 *  \see        #CsrBleHostStartLEScan()\n
 */
#define CSR_BLE_HOST_LE_EVENT_SEND_STATUS          (0x0010)



/**
 *  \brief      Invalid connection handle
 */
#define CSR_BLE_INVALID_HANDLE                      (0xFFFF)

/**
 *  \brief      No updates
 */
#define CSR_BLE_NONE                                (0x00)
/**
 *  \brief      Notification
 */
#define CSR_BLE_NOTIFICATION                        (0x01)
/**
 *  \brief      Indication
 */
#define CSR_BLE_INDICATION                          (0x02)

/**
 *  \brief      Discovered primary service
 *  \hideinitializer
 *  \details    This messages is sent with the information of the
 *              discovered primary service.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_SERVICE_DISCOVERY_RESULT, pointer to
 *              #CSR_BLE_SERVICE_DISCOVERY_RESULT structure.
 *
 *  \see        #CsrBleClientDiscoverServices()
 */
#define CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT     (0x0100)

/**
 *  \brief      Result of the primary service discovery
 *  \hideinitializer
 *  \details    This messages is sent when all primary services were discovered
 *              or when discovery failed.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_SERVICE_DISCOVERY_STOPPED, pointer to
 *              #CSR_BLE_SERVICE_DISCOVERY_STOPPED structure.
 *
 *  \see        #CsrBleClientDiscoverServices()
 */
#define CSR_BLE_CLIENT_SERVICE_DISCOVERY_STOPPED    (0x0101)

/**
 *  \brief      Discovered service characteristic
 *  \hideinitializer
 *  \details    This messages is sent with the information of the
 *              discovered service characteristic.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_DISCOVERY_RESULT, pointer to
 *              #CSR_BLE_CHAR_DISCOVERY_RESULT structure.
 *
 *  \see        #CsrBleClientDiscoverCharacteristics()\n
 *              #CsrBleClientDiscoverCharacteristicsByUuid()
 */
#define CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT        (0x0102)

/**
 *  \brief      Result of service characteristics discovery
 *  \hideinitializer
 *  \details    This messages is sent when all characteristics of the service
 *              were discovered or when discovery failed.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_DISCOVERY_STOPPED, pointer to
 *              #CSR_BLE_CHAR_DISCOVERY_STOPPED structure.
 *
 *  \see        #CsrBleClientDiscoverCharacteristics()\n
 *              #CsrBleClientDiscoverCharacteristicsByUuid()
 */
#define CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED       (0x0103)

/**
 *  \brief      Discovered characteristic descriptor
 *  \hideinitializer
 *  \details    This messages is sent with the information of the
 *              discovered characteristic descriptor.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_DSC_DISCOVERY_RESULT, pointer to
 *              #CSR_BLE_CHAR_DSC_DISCOVERY_RESULT structure.
 *
 *  \see        #CsrBleClientDiscoverCharacteristicDsc()
 */
#define CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT    (0x0104)

/**
 *  \brief      Result of characteristic descriptors discovery
 *  \hideinitializer
 *  \details    This messages is sent when all descriptor of the
 *              characteristic were discovered or when discovery failed.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_DSC_DISCOVERY_STOPPED, pointer to
 *              #CSR_BLE_CHAR_DSC_DISCOVERY_STOPPED structure.
 *
 *  \see        #CsrBleClientDiscoverCharacteristicDsc()
 */
#define CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_STOPPED   (0x0105)

/**
 *  \brief      Discovered GATT database
 *  \hideinitializer
 *  \details    This messages is sent when all GATT database of the remote GATT
 *              server is read or when discovery operation fails.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_DATABASE_DISCOVERY_RESULT, pointer to
 *              #CSR_BLE_DATABASE_DISCOVERY_RESULT structure.
 *
 *  \see        #CsrBleClientDiscoverDatabase()
 */
#define CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT    (0x0106)

/**
 *  \brief      Read characteristic value
 *  \hideinitializer
 *  \details    This messages is sent with the value of the read
 *              characteristic.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_READ_RESULT, pointer to
 *              #CSR_BLE_CHAR_READ_RESULT structure.
 *
 *  \see        #CsrBleClientReadCharByHandle()\n
 *              #CsrBleClientReadCharByUuid()
 */
#define CSR_BLE_CLIENT_CHAR_READ_RESULT             (0x0107)

/**
 *  \brief      Read characteristics values
 *  \hideinitializer
 *  \details    This messages is sent with the values of the read
 *              characteristics.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_READ_MULTI_RESULT, pointer to
 *              #CSR_BLE_CHAR_READ_MULTI_RESULT structure.
 *
 *  \see        #CsrBleClientReadCharsByHandles()
 */
#define CSR_BLE_CLIENT_CHAR_READ_MULTI_RESULT       (0x0108)

/**
 *  \brief      Characteristic write result
 *  \hideinitializer
 *  \details    This messages is sent to indicate local or remote write result.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_WRITE_RESULT, pointer to
 *              #CSR_BLE_WRITE_RESULT structure.
 *
 *  \see        #CsrBleClientWriteCharByHandle()\n
 *              #CsrBleClientWriteCfmCharByHandle()
 */
#define CSR_BLE_CLIENT_CHAR_WRITE_RESULT            (0x0109)

/**
 *  \brief      Incoming GATT server notification or indication
 *  \hideinitializer
 *  \details    This messages is sent by the uEnergy Host library when remote
 *              GATT server sends notification or indication to the
 *              connected client.
 *
 *  \param      LPARAM
 *              #PCSR_BLE_CHAR_NOTIFICATION, pointer to
 *              #CSR_BLE_CHAR_NOTIFICATION structure.
 *
 *  \see        #CsrBleClientWriteConfiguration()
 */
#define CSR_BLE_CLIENT_CHAR_NOTIFICATION            (0x010A)

#define CSR_BLE_SERVER_ALLOC_DATABASE_RESULT        (0x0200)
#define CSR_BLE_SERVER_ADD_DATABASE_RESULT          (0x0201)
#define CSR_BLE_SERVER_DATABASE_READ_INDICATION     (0x0202)
#define CSR_BLE_SERVER_DATABASE_WRITE_INDICATION    (0x0203)

/**
 *  \brief Transport configuration
 *  This structure provides transport configuration for #CsrBleHostInit()
 */
typedef struct _CSR_BLE_TRANSPORT {
    BYTE                        transportType;          /**< Transport type (use one of the \ref transport_type transport constants) */

    /**
     *  \name USB transport configuration
     *  \{
     */
    BYTE                    usbDeviceNumber;            /**< USB device number (use #CSR_BLE_DEFAULT_USB_DEVICE_NUMBER for default device) */
    /**
     *  \}
     */

    /**
     *  \name UART transport configuration
     *  \{
     */
    DWORD                   serialBaudRate;             /**< Serial baud rate */
    DWORD                   serialResetBaudRate;        /**< Reset serial baud rate */
    BYTE                    serialComPort;              /**< COM port number */
    /**
     *  \}
     */
} CSR_BLE_TRANSPORT;

typedef CSR_BLE_TRANSPORT *PCSR_BLE_TRANSPORT;

/**
 *  \brief Bluetooth address
 */
typedef struct _CSR_BLE_BLUETOOTH_ADDRESS {
    BYTE                        type;  /* Type of Bluetooth address (Public or random) */
    WORD                        nAp;   /**< Non-significant part of Bluetooth address [47..32] */
    BYTE                        uAp;   /**< Upper part of Bluetooth address [31..24] */
    DWORD                       lAp;   /**< Lower part of Bluetooth address [23..0] */
} CSR_BLE_BLUETOOTH_ADDRESS;

/**
 *  \brief UUID
 */
typedef struct _CSR_BLE_UUID {
    BYTE                lengthInBytes;                  /* returns the length of the UUID in bytes */
    WORD                uuid16;                         /**< 16-bit UUID */
    BYTE                uuid128[16];                    /**< 128-bit UUID */
} CSR_BLE_UUID;

/**
 *  \brief Pointer to #CSR_BLE_UUID.
 */

typedef CSR_BLE_UUID *PCSR_BLE_UUID;

/**
 *  \brief  Paired device description
 *  \details    This structure describes paired device
 *
 *  \see
 */
typedef struct _CSR_BLE_PAIRED_DEVICE {
    /**
     *  Device address
     */
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;

    /**
     *  Device name
     */
    CHAR                        deviceName[51];
 
    /**
     *  Device appearance value
     */
    WORD                                appearance;
} CSR_BLE_PAIRED_DEVICE;

/**
 *  \brief Pointer to #CSR_BLE_PAIRED_DEVICE.
 */
typedef CSR_BLE_PAIRED_DEVICE *PCSR_BLE_PAIRED_DEVICE;

/**
 *  \brief      GATT characteristic descriptor description
 *  \details    This structure describes GATT characteristic descriptor.
 *
 *  \see        #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_CHARACTERISTIC\n
 *              #CSR_BLE_SERVICE\n
 */
typedef struct _CSR_BLE_CHARACTERISTIC_DSC {
    /**
     *  Characteristic descriptor UUID
     */
    CSR_BLE_UUID                uuid;
    /**
     *  Characteristic descriptor value handle
     */
    WORD                        handle;
} CSR_BLE_CHARACTERISTIC_DSC;

/**
 *  \brief  Pointer to #CSR_BLE_CHARACTERISTIC_DSC.
 */
typedef CSR_BLE_CHARACTERISTIC_DSC *PCSR_BLE_CHARACTERISTIC_DSC;

/**
 *  \brief      GATT characteristic description
 *  \details    This structure describes GATT characteristic.
 *
 *  \see        #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_SERVICE\n
 *              #CSR_BLE_CHARACTERISTIC_DSC
 */
typedef struct _CSR_BLE_CHARACTERISTIC {
    /**
     *  Characteristic UUID
     */
    CSR_BLE_UUID                uuid;
    /**
     *  Characteristic declaration handle
     */
    WORD                        declHandle;
    /**
     *  Characteristic value handle
     */
    WORD                        handle;
    /**
     *  Characteristic properties
     */
    BYTE                        properties;

    /**
     *  Number of characteristic descriptors
     */
    WORD                        nDescriptors;
    /**
     *  Characteristic descriptors
     */
    PCSR_BLE_CHARACTERISTIC_DSC descriptors;
} CSR_BLE_CHARACTERISTIC;

/**
 *  \brief  Pointer to #CSR_BLE_CHARACTERISTIC.
 */
typedef CSR_BLE_CHARACTERISTIC *PCSR_BLE_CHARACTERISTIC;

/**
 *  \brief      GATT service description
 *  \details    This structure describes GATT service.
 *
 *  \see        #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_CHARACTERISTIC\n
 *              #CSR_BLE_CHARACTERISTIC_DSC
 */
typedef struct _CSR_BLE_SERVICE {
    /**
     *  Service UUID
     */
    CSR_BLE_UUID                uuid;
    /**
     *  First handle of the service
     */
    WORD                        startHandle;
    /**
     *  Last handle of the service
     */
    WORD                        endHandle;

    /**
     *  Number of characteristics in this service
     */
    WORD                        nCharacteristics;
    /**
     *  Characteristics in this service
     */
    PCSR_BLE_CHARACTERISTIC     characteristics;
} CSR_BLE_SERVICE;

/**
 *  \brief  Pointer to the #CSR_BLE_SERVICE.
 */
typedef CSR_BLE_SERVICE *PCSR_BLE_SERVICE;

typedef struct _CSR_BLE_DB_CHARACTERISTIC {
    CSR_BLE_UUID                uuid;                   /* characteristic UUID */
    BYTE                        properties;             /* properties */
    WORD                        permission;             /* permisssion bits */
    WORD                        flags;                  /* flags */

    WORD                        handle;
    WORD                        clientCfgHandle;

    WORD                        length;                 /* value length */
    LPVOID                      value;                  /* value */
} CSR_BLE_DB_CHARACTERISTIC, *PCSR_BLE_DB_CHARACTERISTIC;

typedef struct _CSR_BLE_DB_SERVICE {
    CSR_BLE_UUID                uuid;                   /* Service UUID */

    WORD                        handle;

    WORD                        nCharacteristics;       /* number of characteristics */
    PCSR_BLE_DB_CHARACTERISTIC  characteristics;        /* service characteristics */
} CSR_BLE_DB_SERVICE, *PCSR_BLE_DB_SERVICE;

/**
 *  \brief      Discovered device
 *  \details    This structure contains information regarding the discovered device.
 *              It is returned with the #CSR_BLE_HOST_SEARCH_RESULT message.
 */
typedef struct _CSR_BLE_DEVICE_SEARCH_RESULT {
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;          /**< Device address */
    signed char                 rssi;                   /**< Device RSSI */

    /**
     *  Bitmask of the reported data, combination of the \ref information_reported
     *  "information reported flags".
     */
    WORD                        informationReported;
                                                             

    /**
     *  Device local name, either shortened or complete.
     *  \note   This field contains valid information when one of the
     *          #CSR_BLE_DEVICE_SEARCH_SHORT_NAME or
     *          #CSR_BLE_DEVICE_SEARCH_COMPLETE_NAME bits is set in the
     *          #informationReported field.
     */
    LPCSTR                      deviceName;

    /**
     *  Device flags.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_FLAGS bit is set in the
     *          #informationReported field.
     *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 18.1.
     */
    BYTE                        flags;

    /**
     *  Device advertised TX Power level.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_TX_LEVEL bit is set in the
     *          #informationReported field.
     *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 18.4.
     */
    signed char                 txPowerLevel;

        /**
     *  Device advertised appearance value.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_APPEARANCE bit is set in the
     *          #informationReported field.
     *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 12.2.
     */
    WORD                        appearance;
    /**
     *  Number of the advertised device primary services.
     *  \note   This field contains valid information when either
     *          #CSR_BLE_DEVICE_SEARCH_COMPLETE_SERVICES or
     *          #CSR_BLE_DEVICE_SEARCH_INCOMPLETE_SERVICES bit is set in the
     *          #informationReported field.
     *  \see    #deviceServices
     */
    BYTE                        nDeviceServices;
    /**
     *  Complete or incomplete UUID list of the device primary services.
     *  \note   This field contains valid information when either
     *          #CSR_BLE_DEVICE_SEARCH_COMPLETE_SERVICES or
     *          #CSR_BLE_DEVICE_SEARCH_INCOMPLETE_SERVICES bit is set in the
     *          #informationReported field.
     *  \see    #nDeviceServices
     */
    PCSR_BLE_UUID               deviceServices;

    /**
     *  Security manager OOB flags.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_OOB bit is set in the
     *          #informationReported field.
     *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 18.5.
     */
    BYTE                        oobFlags;

    /**
     *  Security manager TK value.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_TK bit is set in the
     *          #informationReported field.
     *  \see    BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 18.6.
     */
    DWORD                       oobTk[4];

    /**
     *  Minimum value of the slave connection interval range.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_CONNECTION_INTERVAL bit is set in the
     *          #informationReported field.
     *  \see    #connIntervalMax\n
     *          BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 18.8.
     */
    WORD                        connIntervalMin;
    /**
     *  Maximum value of the slave connection interval range.
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_CONNECTION_INTERVAL bit is set in the
     *          #informationReported field.
     *  \see    #connIntervalMin\n
     *          BLUETOOTH SPECIFICATION Version 4.0 [Vol 3], Generic Access
     *          Profile, Section 18.8.
     */
    WORD                        connIntervalMax;

    BYTE                        nServiceSolicitation;   /* number of solicited services */
    PCSR_BLE_UUID               serviceSolicitation;    /* solicited services */

    /**
     *  Service-specific data size
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_SERVICE_DATA bit is set in the
     *          #informationReported field.
     *  \see    #serviceUuid\n
     *          #serviceData
     */
    BYTE                        serviceDataSize;
    /**
     *  Service 16-bit UUID
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_SERVICE_DATA bit is set in the
     *          #informationReported field.
     *  \see    #serviceDataSize\n
     *          #serviceData
     */
    WORD                        serviceUuid;

    /**
     *  Service 128-bit UUID
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_SERVICE_DATA bit is set in the
     *          #informationReported field.
     *  \see    #serviceDataSize\n
     *          #serviceData
     */
    BYTE                service128Uuid[16]; 
    /**
     *  Service-specific data
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_SERVICE_DATA bit is set in the
     *          #informationReported field.
     *  \see    #serviceUuid\n
     *          #serviceDataSize
     */
    LPVOID                      serviceData;

    /**
     *  Manufacturer specific data size
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_MANUFACTURER bit is set in the
     *          #informationReported field.
     *  \see    #manufCode\n
     *          #manufData
     */
    BYTE                        manufDataSize;
    /**
     *  Manufacturer assigned ID
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_MANUFACTURER bit is set in the
     *          #informationReported field.
     *  \see    #manufDataSize\n
     *          #manufData
     */
    WORD                        manufCode;
    /**
     *  Manufacturer specific data
     *  \note   This field contains valid information when
     *          #CSR_BLE_DEVICE_SEARCH_MANUFACTURER bit is set in the
     *          #informationReported field.
     *  \see    #manufDataSize\n
     *          #manufCode
     */
    LPVOID                      manufData;
} CSR_BLE_DEVICE_SEARCH_RESULT;

/**
 *  \brief      Pointer to #CSR_BLE_DEVICE_SEARCH_RESULT.
 */
typedef CSR_BLE_DEVICE_SEARCH_RESULT *PCSR_BLE_DEVICE_SEARCH_RESULT;

/**
 *  \brief      Connection result
 *  \details    This structure is returned with the
 *              #CSR_BLE_HOST_CONNECT_RESULT message.
 */
typedef struct _CSR_BLE_CONNECT_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Connection result. <b>0</b> means success.
     */
    WORD                        result;

    /**
     *  Connection handle. The application needs to provide this handle for all
     *  operations during this connection.
     */
    DWORD                       connectHandle;
} CSR_BLE_CONNECT_RESULT;

/**
 *  \brief      Pointer to #CSR_BLE_CONNECT_RESULT.
 */
typedef CSR_BLE_CONNECT_RESULT *PCSR_BLE_CONNECT_RESULT;

/**
 *  \brief      Disconnection indication
 *  \details    This structure is returned with the
 *              #CSR_BLE_HOST_DISCONNECTED message.
 */
typedef struct _CSR_BLE_DISCONNECTED {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Disconnection reason, <b>0</b> means success
     */
    WORD                        reason;
    /**
     *  Handle of the disconnected connection
     */
    DWORD                       connectHandle;
} CSR_BLE_DISCONNECTED;

/**
 *  \brief      Pointer to #CSR_BLE_DISCONNECTED.
 */
typedef CSR_BLE_DISCONNECTED *PCSR_BLE_DISCONNECTED;

/**
 *  \brief      RSSI measured value
 *  \details    This structure is returned with the
 *              #CSR_BLE_HOST_RSSI_RESULT message.
 */
typedef struct _CSR_BLE_RSSI_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Measurement result code. <b>0</b> means successful measurement.
     */
    WORD                        result;
    /**
     *  Device Bluetooth address
     */
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;
    /**
     *  Measured RSSI value
     */
    signed char                 rssi;
} CSR_BLE_RSSI_RESULT;

/**
 *  \brief      Pointer to #CSR_BLE_RSSI_RESULT.
 */
typedef CSR_BLE_RSSI_RESULT *PCSR_BLE_RSSI_RESULT;

/**
 *  \brief      Result of Debonding
 *  \details    This structure is returned with the
 *              #CSR_BLE_DEBOND_RESULT  message.
 */
typedef struct _CSR_BLE_DEBOND_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Connection result. <b>0</b> means success.
     */
    WORD                        result;
    /**
     *  Address of the disconnected device
     */
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;
} CSR_BLE_DEBOND_RESULT;

typedef CSR_BLE_DEBOND_RESULT *PCSR_BLE_DEBOND_RESULT;

/**
 *  \brief      Result of Just Works
 *  \details    This structure is returned with the
 *              #CSR_BLE_JUSTWORKS_REQUEST  message.
 */
typedef struct _CSR_BLE_JUSTWORKS_REQUEST {
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;
    LPCSTR                      deviceName;
    BOOL                        paired;
} CSR_BLE_JUSTWORKS_REQUEST;

typedef CSR_BLE_JUSTWORKS_REQUEST *PCSR_BLE_JUSTWORKS_REQUEST;

/**
 *  \brief      Display passkey to be entered on the remote device
 *  \details    This structure is returned with the
 *              #CSR_BLE_HOST_DISPLAY_PASSKEY_IND message.
 */
typedef struct _CSR_BLE_DISPLAY_PASSKEY_IND {
    /**
     *  Bluetooth address of the remote device
     */
    CSR_BLE_BLUETOOTH_ADDRESS deviceAddress;
    /**
     *  Numeric value to be displayed to the user.
     */
    DWORD                       numericValue;

    WORD appQId;
} CSR_BLE_DISPLAY_PASSKEY_IND;

/**
 *  \brief      Pointer to #CSR_BLE_DISPLAY_PASSKEY.
 */
typedef CSR_BLE_DISPLAY_PASSKEY_IND *PCSR_BLE_DISPLAY_PASSKEY_IND;


typedef struct _CSR_BLE_SECURITY_RESULT {
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;
     /**
    *  Discovery result supplier, <b>0</b> means success.
    */
    WORD                        supplier;
    WORD                        result;
} CSR_BLE_SECURITY_RESULT;

typedef CSR_BLE_SECURITY_RESULT *PCSR_BLE_SECURITY_RESULT;

typedef struct _CSR_BLE_SET_ENCRYPTION_RESULT {
    DWORD                       connectHandle;
     /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    WORD                        result;
} CSR_BLE_SET_ENCRYPTION_RESULT;

typedef CSR_BLE_SET_ENCRYPTION_RESULT *PCSR_BLE_SET_ENCRYPTION_RESULT;

/**
 *  \brief      Connection Update Request
 *  \details    This structure is filled with the
 *              #CSR_BLE_CONNECTION_UPDATE_REQUEST  message.
 */
typedef struct _CSR_BLE_CONNECTION_UPDATE_REQUEST {
    DWORD                       connectHandle;
    WORD                        conIntervalMin;
    WORD                        conIntervalMax;
    WORD                        conLatency;
    WORD                        conSupTimeout;
    WORD                        id;
} CSR_BLE_CONNECTION_UPDATE_REQUEST;

typedef CSR_BLE_CONNECTION_UPDATE_REQUEST *PCSR_BLE_CONNECTION_UPDATE_REQUEST;

typedef struct _CSR_BLE_CONNECTION_UPDATED {
    DWORD                       connectHandle;
    WORD                        conInterval;
    WORD                        conLatency;
    WORD                        conSupTimeout;
} CSR_BLE_CONNECTION_UPDATED;

typedef CSR_BLE_CONNECTION_UPDATED *PCSR_BLE_CONNECTION_UPDATED;

/**
 *  \}
 */

/**
 *  \addtogroup gatt_client_messages
 *  \{
 */

/**
 *  \brief      Discovered primary service 
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT message.
 */
typedef struct _CSR_BLE_SERVICE_DISCOVERY_RESULT {
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  Handle of the first attribute in discovered service
     */
    WORD                        startHandle;
    /**
     *  Handle of the last attribute in discovered service
     */
    WORD                        endHandle;
    /**
     *  Service UUID
     */
    CSR_BLE_UUID                deviceService;
} CSR_BLE_SERVICE_DISCOVERY_RESULT;

/**
 *  \brief      Pointer to the #CSR_BLE_SERVICE_DISCOVERY_RESULT.
 */
typedef CSR_BLE_SERVICE_DISCOVERY_RESULT *PCSR_BLE_SERVICE_DISCOVERY_RESULT;

/**
 *  \brief      Primary service discovery finished
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_SERVICE_DISCOVERY_STOPPED message.
 */
typedef struct _CSR_BLE_SERVICE_DISCOVERY_STOPPED {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Status of the service discovery, <b>0</b> means successful stop
     */
    WORD                        result;
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
} CSR_BLE_SERVICE_DISCOVERY_STOPPED;

/**
 *  \brief      Pointer to #CSR_BLE_SERVICE_DISCOVERY_STOPPED.
 */
typedef CSR_BLE_SERVICE_DISCOVERY_STOPPED *PCSR_BLE_SERVICE_DISCOVERY_STOPPED;

/**
 *  \brief      Discovered service characteristic
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT message.
 */
typedef struct _CSR_BLE_CHAR_DISCOVERY_RESULT {
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  Characteristic UUID
     */
    CSR_BLE_UUID                uuid;
    /**
     *  Attribute handle to the characteristic value
     */
    WORD                        valueHandle;
    /**
     *  Characteristic properties
     */
    BYTE                        properties;
} CSR_BLE_CHAR_DISCOVERY_RESULT;

/**
 *  \brief      Pointer to #CSR_BLE_CHAR_DISCOVERY_RESULT.
 */
typedef  CSR_BLE_CHAR_DISCOVERY_RESULT *PCSR_BLE_CHAR_DISCOVERY_RESULT;

/**
 *  \brief      Characteristic discovery finished
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED message.
 */
typedef struct _CSR_BLE_CHAR_DISCOVERY_STOPPED {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Status of the service discovery, <b>0</b> means successful stop
     */
    WORD                        result;
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
} CSR_BLE_CHAR_DISCOVERY_STOPPED;

/**
 *  \brief      Pointer to CSR_BLE_CHAR_DISCOVERY_STOPPED
 */
typedef  CSR_BLE_CHAR_DISCOVERY_STOPPED *PCSR_BLE_CHAR_DISCOVERY_STOPPED;

/**
 *  \brief      Discovered characteristic descriptor
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT message.
 */
typedef struct _CSR_BLE_CHAR_DSC_DISCOVERY_RESULT {
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  Descriptor UUID
     */
    CSR_BLE_UUID                uuid;
    /**
     * Descriptor value attribute handle
     */
    WORD                        dscHandle;
} CSR_BLE_CHAR_DSC_DISCOVERY_RESULT;

/**
 *  \brief      Pointer to CSR_BLE_CHAR_DSC_DISCOVERY_RESULT
 */
typedef CSR_BLE_CHAR_DSC_DISCOVERY_RESULT *PCSR_BLE_CHAR_DSC_DISCOVERY_RESULT;

/**
 *  \brief      Characteristic descriptors discovery finished
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_STOPPED message.
 */
typedef struct _CSR_BLE_CHAR_DSC_DISCOVERY_STOPPED {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Discovery result, <b>0</b> means success
     */
    WORD                        result;
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
} CSR_BLE_CHAR_DSC_DISCOVERY_STOPPED;

/**
 *  \brief      Pointer to CSR_BLE_CHAR_DSC_DISCOVERY_STOPPED
 */
typedef CSR_BLE_CHAR_DSC_DISCOVERY_STOPPED *PCSR_BLE_CHAR_DSC_DISCOVERY_STOPPED;

/**
 *  \brief      Discovered GATT database
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT message.
 */
typedef struct _CSR_BLE_DATABASE_DISCOVERY_RESULT {
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;

    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;

    /**
     *  Discovery result, <b>0</b> means success.
     */
    WORD                        result;

    /**
     *  Number of services discovered
     */
    WORD                        nServices;
    /**
     *  Discovered services
     */
    PCSR_BLE_SERVICE            services;
} CSR_BLE_DATABASE_DISCOVERY_RESULT;

/**
 *  \brief  Pointer to #CSR_BLE_DATABASE_DISCOVERY_RESULT.
 */
typedef CSR_BLE_DATABASE_DISCOVERY_RESULT *PCSR_BLE_DATABASE_DISCOVERY_RESULT;

/**
 *  \brief      Read characteristic value
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_READ_RESULT message.
 */
typedef struct _CSR_BLE_CHAR_READ_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Read result, <b>0</b> means success
     */
    WORD                        result;
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  Characteristic value handle
     */
    WORD                        charHandle;
    /**
     *  Characteristic value size in octets
     */
    WORD                        charValueSize;
    /**
     *  Characteristic value
     */
    LPVOID                      charValue;
} CSR_BLE_CHAR_READ_RESULT;

/**
 *  \brief  Pointer to #CSR_BLE_CHAR_READ_RESULT.
 */
typedef CSR_BLE_CHAR_READ_RESULT *PCSR_BLE_CHAR_READ_RESULT;

/**
 *  \brief      Read characteristics values
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_READ_RESULT message.
 */
typedef struct _CSR_BLE_CHAR_READ_MULTI_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Read result, <b>0</b> means success
     */
    WORD                        result;
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  User tag, specified in #CsrBleClientReadCharsByHandles() call
     */
    WORD                        userTag;
    /**
     *  Combined characteristics values size
     */
    WORD                        charValueSize;
    /**
     *  Combined characteristic values, one after another as specified in request
     */
    LPVOID                      charValue;
} CSR_BLE_CHAR_READ_MULTI_RESULT;

/**
 *  \brief  Pointer to #CSR_BLE_CHAR_READ_MULTI_RESULT
 */
typedef CSR_BLE_CHAR_READ_MULTI_RESULT *PCSR_BLE_CHAR_READ_MULTI_RESULT;

/**
 *  \brief      Write confirmation
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_WRITE_RESULT message.
 */
typedef struct _CSR_BLE_WRITE_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
     WORD                        supplier;
    /**
     *  Characteristic write result, <b>0</b> means success
     */
    WORD                        result;
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  Characteristic handle
     */
    WORD                        charHandle;
} CSR_BLE_WRITE_RESULT;

/**
 *  \brief  Pointer to #CSR_BLE_CHAR_READ_MULTI_RESULT
 */
typedef CSR_BLE_WRITE_RESULT *PCSR_BLE_WRITE_RESULT;

/**
 *  \brief      GATT server notification or indication
 *  \details    This structure is returned with the
 *              #CSR_BLE_CLIENT_CHAR_NOTIFICATION message.
 */
typedef struct _CSR_BLE_CHAR_NOTIFICATION {
    /**
     *  Connection handle
     */
    DWORD                       connectHandle;
    /**
     *  GATT server Bluetooth address
     */
    CSR_BLE_BLUETOOTH_ADDRESS   deviceAddress;

    /**
     *  Characteristic handle
     */
    WORD                        charHandle;
    /**
     *  Notified value size
     */
    WORD                        charValueSize;
    /**
     *  Notified value
     */
    LPVOID                      charValue;
} CSR_BLE_CHAR_NOTIFICATION;

/**
 *  \brief  Pointer to CSR_BLE_CHAR_NOTIFICATION
 */
typedef CSR_BLE_CHAR_NOTIFICATION *PCSR_BLE_CHAR_NOTIFICATION;

typedef struct _CSR_BLE_DB_ALLOC_RESULT {
    /**
    *  Discovery result supplier, <b>0</b> means success.
    */
    WORD                        supplier;
    WORD                        result;                 /* result */

    WORD                        start;                  /* start handle */
    WORD                        end;                    /* end offset */
} CSR_BLE_DB_ALLOC_RESULT, *PCSR_BLE_DB_ALLOC_RESULT;

typedef struct _CSR_BLE_DB_READ_ACCESS_IND {
    DWORD                       connectHandle;          /* connection handle */

    WORD                        handle;                 /* attribute handle */
    WORD                        offset;                 /* attribute offset */
    WORD                        maximumLength;          /* maximum response length */
} CSR_BLE_DB_READ_ACCESS_IND, *PCSR_BLE_DB_READ_ACCESS_IND;

typedef struct _CSR_BLE_DB_WRITE_ACCESS_PAIR {
    WORD                        handle;                 /* attribute handle */
    WORD                        offset;                 /* attribute offset */
    WORD                        length;                 /* value length */
    LPVOID                      value;                  /* value */
} CSR_BLE_DB_WRITE_ACCESS_PAIR, *PCSR_BLE_DB_WRITE_ACCESS_PAIR;

typedef struct _CSR_BLE_DB_WRITE_ACCESS_IND {
    DWORD                       connectHandle;          /* connection handle */

    WORD                        handle;                 /* attribute handle */

    WORD                        nPairs;                 /* number of write unit pairs */
    PCSR_BLE_DB_WRITE_ACCESS_PAIR valuePairs;               /* write unit pairs */
} CSR_BLE_DB_WRITE_ACCESS_IND, *PCSR_BLE_DB_WRITE_ACCESS_IND;

typedef struct _CSR_BLE_MSG_NOTIFY {
   /**
    *  Result supplier, <b>0</b> means success.
    */
    WORD                        supplier;
    WORD                        result;
} CSR_BLE_MSG_NOTIFY, *PCSR_BLE_MSG_NOTIFY;


/**
 *  \}
 */

/**
 *  \addtogroup initialization
 *  \{
 */

/**
 *  \brief      This function initialises the uEnergy host library
 *  \details    #CsrBleHostInit() should be called before any other uEnergy Host
 *              library functions. It initialises the transport layer and
 *              configures the logging options. Note that this function does
 *              not start the LE stack operation and should be followed by the
 *              consequent call to #CsrBleHostStart().
 *
 *  \param      useLogging
 *              Sets whether uEnergy host library should produce internal debug
 *              log. Note that debug log may significantly slow down the
 *              overall application performance.
 *  \param      logFilename
 *              If debug logging is enabled, this string would be used as a
 *              file name for the internal log file. The extension would be
 *              .cap. If this parameter is NULL the default log file name would
 *              be used (uEnergyHost<i>HHMMSSDDMMYYYY</i>.cap where
 *              <i>HHMMSSDDMMYYYY</i> is the current timestamp).
 *  \param      transport
 *              Transport initialisation parameters, see
 *              #CSR_BLE_TRANSPORT.
 *
 *  \returns    Windows user message id for the uEnergy Host library messages that
 *              should be handled in the application message loop. <b>0</b> -
 *              in case of failure.
 *
 *  \see        #CsrBleHostStart()\n
 *              #CsrBleHostDeinit()\n
 *              #CSR_BLE_TRANSPORT\n
 *              \ref library_initialization
 */
CSR_BLE_HOST_API UINT CsrBleHostInit(BOOL useLogging, LPCSTR logFilename,
                                     PCSR_BLE_TRANSPORT transport);

/**
 *  \brief      This function starts uEnergy LE host stack
 *  \details    #CsrBleHostStart() should be called after succesful call to
 *              #CsrBleHostInit() and before any other uEnergy Host library
 *              functions. It sets up the hardware and starts up LE host
 *              stack.
 *
 *              The call to #CsrBleHostStart() is asynchronous and the application
 *              should wait for the #CSR_BLE_HOST_READY message that indicates
 *              the result of the operation.
 *
 *              If the application wants to receive messages to the application window
 *              the uEnergy library should be started with a call to
 *              #CsrBleHostStartWnd().
 *
 *  \param      threadId
 *              The application has to pass the thread id that will receive
 *              uEnergy Host messages.
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostInit()\n
 *              #CsrBleHostStartWnd()\n
 *              #CSR_BLE_HOST_READY\n
 *              \ref library_initialization
 */
CSR_BLE_HOST_API BOOL CsrBleHostStart(DWORD threadId);

/**
 *  \brief      This function starts uEnergy LE host stack
 *  \details    #CsrBleHostStartWnd() is similar to the #CsrBleHostStart() function
 *              but it configures the uEnergy host library to send the uEnergy messages
 *              to the specified application window instead of the thread.
 *
 *              #CsrBleHostStartWnd() should be called after succesful call to
 *              #CsrBleHostInit() and before any other uEnergy Host library
 *              functions. It sets up the hardware and starts up LE host
 *              stack.
 *
 *              The call to #CsrBleHostStartWnd() is asynchronous and the application
 *              should wait for the #CSR_BLE_HOST_READY message that indicates
 *              the result of the operation.
 *
 *  \param      hWnd
 *              The application window handle that will receive the library messages.
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostInit()\n
 *              #CsrBleHostStart()\n
 *              #CSR_BLE_HOST_READY\n
 *              \ref library_initialization
 */
CSR_BLE_HOST_API BOOL CsrBleHostStartWnd(HWND hWnd);

/**
 *  \brief      This function stops and deinitialises uEnergy LE host stack
 *  \details    No other uEnergy host functions should be called after
 *              #CsrBleHostDeinit().
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostInit()\n
 *              \ref library_initialization
 */
CSR_BLE_HOST_API BOOL CsrBleHostDeinit(void);

/**
 *  \brief      This function starts the LE device discovery
 *  \details    #CsrBleHostStartDeviceSearch() starts the LE device discovery
 *              for a specified period of time. Discovered devices are reported
 *              with #CSR_BLE_HOST_SEARCH_RESULT messages. On the completion
 *              of the device discovery, the application is notified with
 *              #CSR_BLE_HOST_SEARCH_STOPPED message.
 *
 *              The search could be stopped at any moment using
 *              #CsrBleHostStopDeviceSearch().
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostStartDeviceSearch()", URL="\ref #CsrBleHostStartDeviceSearch()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Start LE device discovery"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_RESULT", URL="\ref #CSR_BLE_HOST_SEARCH_RESULT"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_RESULT", URL="\ref #CSR_BLE_HOST_SEARCH_RESULT"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_RESULT", URL="\ref #CSR_BLE_HOST_SEARCH_RESULT"];
 *              ...;
 *              --- [label=" Device discovery time expired or device discovery was stopped "];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_STOPPED", URL="\ref #CSR_BLE_HOST_SEARCH_STOPPED"];
 *  \endmsc
 *
 *  \param      searchTime
 *              Device discovery time, in milliseconds. Use <b>0</b> for the
 *              infinite search.
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostStopDeviceSearch()\n
 *              #CSR_BLE_HOST_SEARCH_RESULT\n
 *              #CSR_BLE_HOST_SEARCH_STOPPED
  */
CSR_BLE_HOST_API BOOL CsrBleHostStartDeviceSearch(DWORD searchTime);

/**
 *  \brief      This function stops the LE device discovery
 *  \details    #CsrBleHostStartDeviceSearch() stops the LE device discovery
 *              started with #CsrBleHostStartDeviceSearch(). On the
 *              completion of the device discovery, the application is
 *              notified with #CSR_BLE_HOST_SEARCH_STOPPED message.
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostStartDeviceSearch()", URL="\ref #CsrBleHostStartDeviceSearch()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Start LE device discovery"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_RESULT", URL="\ref #CSR_BLE_HOST_SEARCH_RESULT"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_RESULT", URL="\ref #CSR_BLE_HOST_SEARCH_RESULT"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_RESULT", URL="\ref #CSR_BLE_HOST_SEARCH_RESULT"];
 *              ...;
 *              Application=>"uEnergy Host Library" [label="CsrBleHostStopDeviceSearch()", URL="\ref #CsrBleHostStopDeviceSearch()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_SEARCH_STOPPED", URL="\ref #CSR_BLE_HOST_SEARCH_STOPPED"];
 *  \endmsc
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostStartDeviceSearch()\n
 *              #CSR_BLE_HOST_SEARCH_STOPPED
 */
CSR_BLE_HOST_API BOOL CsrBleHostStopDeviceSearch(void);



CSR_BLE_HOST_API BOOL CsrBleHostStartLEScan();

CSR_BLE_HOST_API BOOL CsrBleHostStopLESearch();
/**
 *  \brief      This function initiates LE connection
 *  \details    The application should call this function to intiate connection to the
 *              LE slave with the specifed Bluetooth address. The result of the
 *              connection request is provided with #CSR_BLE_HOST_CONNECT_RESULT
 *              message.
 *
 *              The ongoing connection initiation could be cancelled using
 *              #CsrBleHostCancelConnect().
 *
 *              The connection would be established with the default connection
 *              parameters, unless set in advance with the call to
 *              #CsrBleHostSetConnectionParams().
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote LE device";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostConnect()", URL="\ref #CsrBleHostConnect()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote LE device" [label="Initiate connection"];
 *              ...;
 *              --- [label=" Connection was established, cancelled or failed "];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_CONNECT_RESULT", URL="\ref #CSR_BLE_HOST_CONNECT_RESULT"];
 *  \endmsc
 *
 *  \param      address
 *              LE slave Bluetooth address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostCancelConnect()\n
 *              #CsrBleHostSetConnectionParams()\n
 *              #CSR_BLE_HOST_CONNECT_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleHostConnect(CSR_BLE_BLUETOOTH_ADDRESS address);

/**
 *  \brief      This function instructs the host library to use random address
 *  \details    The application should call this function in order to enbale or
 *              disable the use of the private resolvable address. The address
 *              would be regenerated randomly every 20 minutes.
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      useRandomAddress
 *              Disable or enable the use of the private resolvable address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostStartDeviceSearch()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostUseRandomAddress(BOOL useRandomAddress);

/**
 *  \brief      Cancel LE connection request
 *  \details    After connection was initiated with the call to
 *              #CsrBleHostConnect() and before it was established with the LE
 *              slave, it could be cancelled with this function. After
 *              connection is established, you need to use
 *              #CsrBleHostDisconnect() instead.
 *
 *              The application will receive #CSR_BLE_HOST_CONNECT_RESULT
 *              message.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote LE device";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostConnect()", URL="\ref #CsrBleHostConnect()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote LE device" [label="Initiate connection"];
 *              ...;
 *              --- [label=" Connection is not established yet "];
 *              |||;
 *              Application=>"uEnergy Host Library" [label="CsrBleHostCancelConnect()", URL="\ref #CsrBleHostCancelConnect()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_CONNECT_RESULT", URL="\ref #CSR_BLE_HOST_CONNECT_RESULT"];
 *  \endmsc
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostDisconnect()\n
 *              #CSR_BLE_HOST_CONNECT_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleHostCancelConnect(void);

/**
 *  \brief      Change the connection parameters
 *  \details    If not preceded with the call to this function,
 *              #CsrBleHostConnect() is using default connection parameters.
 *
 *              The application receives #CSR_BLE_HOST_CONN_PARAM_UPDATE message
 *              when connection parameters are set.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote LE device";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostSetConnectionParams()", URL="\ref #CsrBleHostSetConnectionParams()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_CONN_PARAM_UPDATE", URL="\ref #CSR_BLE_HOST_CONN_PARAM_UPDATE"];
 *              |||;
 *              Application=>"uEnergy Host Library" [label="CsrBleHostConnect()", URL="\ref #CsrBleHostConnect()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote LE device" [label="Initiate connection"];
 *              ...;
 *  \endmsc
 *
 *  \note       The default connection parameters are as follows:\n
 *              <b>scan interval</b>: 4\n
 *              <b>scan window</b>: 4\n
 *              <b>connection interval minimum</b>: 40\n
 *              <b>connection interval maximum</b>: 40\n
 *              <b>slave latency</b>: 12\n
 *              <b>supervision timeout</b>: 360
 *
 *  \param      scanInterval
 *              Scan interval
 *  \param      scanWindow
 *              Scan window
 *  \param      connMin
 *              Connection interval minimum value
 *  \param      connMax
 *              Connection interval maximum value
 *  \param      latency
 *              Slave latency
 *  \param      timeout
 *              Supervision timeout
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()
 *              #CSR_BLE_HOST_CONN_PARAM_UPDATE
 */
CSR_BLE_HOST_API BOOL CsrBleHostSetConnectionParams(WORD scanInterval,
    WORD scanWindow, WORD connMin, WORD connMax, WORD latency, WORD timeout);

/**
 *  \brief      Update the connection parameters
 *  \details    While connected, LE master can change connection parameters
 *              with the call to this function. The result of this request is
 *              reported with #CSR_BLE_HOST_CONN_PARAM_UPDATE message.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote LE device";
 *
 *              ...;
 *              --- [label=" Connection established "];
 *              |||;
 *              Application=>"uEnergy Host Library" [label="CsrBleHostUpdateConnectionParams()", URL="\ref #CsrBleHostUpdateConnectionParams()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              |||;
 *              "uEnergy Host Library"->"Remote LE device" [label="Negotiate new connection parameters"];
 *              ...;
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Connection parameters updated"];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_CONN_PARAM_UPDATE", URL="\ref #CSR_BLE_HOST_CONN_PARAM_UPDATE"];
 *  \endmsc
 *
 *  \param      address
 *              Bluetooth address of the connected LE slave
 *  \param      connMin
 *              Connection interval minimum value
 *  \param      connMax
 *              Connection interval maximum value
 *  \param      latency
 *              Slave latency
 *  \param      timeout
 *              Supervision timeout
 *  \param      minCeLength
 *              Supervision timeout
 *  \param      maxCeLength
 *              Supervision timeout
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()
 *              #CSR_BLE_HOST_CONN_PARAM_UPDATE
 */
CSR_BLE_HOST_API BOOL CsrBleHostUpdateConnectionParams(
    CSR_BLE_BLUETOOTH_ADDRESS address, WORD connMin, WORD connMax,
    WORD latency, WORD timeout, WORD minCeLength, WORD maxCeLength);

/**
 *  \brief      Disconnect
 *  \details    This function disconnects specified connection.
 *
 *              On disconnection, the uEnergy Host library sends
 *              #CSR_BLE_HOST_DISCONNECTED message.
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote LE device";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostDisconnect()", URL="\ref #CsrBleHostDisconnect()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote LE device" [label="Initiate disconnection"];
 *              ...;
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Disconnection"];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_DISCONNECTED", URL="\ref #CSR_BLE_HOST_DISCONNECTED"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CSR_BLE_HOST_DISCONNECTED
 */
CSR_BLE_HOST_API BOOL CsrBleHostDisconnect(DWORD connectHandle);

/**
 *  \}
 */

/**
 *  \name Generic Library interface
 *  \{
 */

/**
 *  \brief      CsrBleHostGetPairedDeviceList
 *  \details    This function returns the set of paired device list from the device database.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote LE device";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostGetPairedDeviceList()", URL="\ref #CsrBleHostGetPairedDeviceList()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote LE device" [label="Get Paired Device List"];
 *              ...;
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="PairedDeviceList"];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CsrBleHostGetPairedDeviceList", URL="\ref #CsrBleHostGetPairedDeviceList"];
 *  \endmsc
 *
 *  \param      nPairedDevices
 *              No of paired devices returned.
 *  \param      pairedDevices
 *              Paired Device list returned.
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostSetEncryption
 */
CSR_BLE_HOST_API BOOL CsrBleHostGetPairedDeviceList(LPWORD nPairedDevices, PCSR_BLE_PAIRED_DEVICE* pairedDevices);

/**
 *  \brief      CsrBleHostFreePairedDeviceList
 *  \details    This function frees the set of paired devices in the database.
 *              This function needs to be called before deinitailizing the host library 
 *              else it will result in a memory leak.
 *
 *  \param      pairedDevices
 *              Paired Device list.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostFreePairedDeviceList()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostFreePairedDeviceList(PCSR_BLE_PAIRED_DEVICE pairedDevices);

/**
 *  \brief      CsrBleHostUpdatePairedDeviceList
 *  \details    The function instructs the library to update the paired device database 
 *              in the host library. 
 *
 *  \param      pairedDevice
 *              The device that needs updation.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostSetEncryption()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostUpdatePairedDeviceList(PCSR_BLE_PAIRED_DEVICE pairedDevice);

/**
 *  \brief      Free uEnergy Host library message
 *  \details    The application should call this function to free uEnergy message
 *              contents.
 *
 *  \param      wParam
 *              WPARAM of the received uEnergy message
 *
 *  \param      lParam
 *              LPARAM of the received uEnergy message
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 */
CSR_BLE_HOST_API BOOL CsrBleHostFreeMessageContents(WPARAM wParam, LPARAM lParam);

/**
 *  \brief      Cancel current GATT operation
 *  \details    This function tries to cancel current GATT operation.
 *              The application may receive corresponding result code after
 *              calling to #CsrBleHostCancel(). Some operations may not be
 *              cancelled with this routine.
 *
 *  \returns    TRUE in case of that cancel request was successfully sent,
 *              FALSE otherwise. Positive result does not mean that cancel
 *              request was sucessfully processsed.
 */
CSR_BLE_HOST_API BOOL CsrBleHostCancel();


/**
 *  \brief      Returns base Bluetooth UUID
 *  \details    This function retrieves the base bluetooth ID used for 
 *              adopted profiles and services.
 *
 *  \param      pBase
 *              array to be filled.
 *
 *  \param      Length
 *              length of the array.
 *
 *  \returns    TRUE in case the Base ID was successfully retrieved,
 *              FALSE otherwise. 
 */
CSR_BLE_HOST_API BOOL CsrBleHostGetBtBaseUuid(DWORD *pBase, int Length);

/**
 *  \brief      Get RSSI value for the device
 *  \details    This function reads the RSSI value for the specified connected
 *              device.
 *
 *              The application receives the measured RSSI with
 *              #CSR_BLE_HOST_RSSI_RESULT message.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleHostGetRssi()", URL="\ref #CsrBleHostGetRssi()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              |||;
 *              "uEnergy Host Library" box "uEnergy Host Library" [label="Measure\nRSSI value"];
 *              |||;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_HOST_RSSI_RESULT", URL="\ref #CSR_BLE_HOST_RSSI_RESULT"];
 *  \endmsc
 *
 *  \param      address
 *              Bluetooth address for the connected device.
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 */
CSR_BLE_HOST_API BOOL CsrBleHostGetRssi(CSR_BLE_BLUETOOTH_ADDRESS address);


/**
 *  \name Low Energy Security interfaces 
 *  \{
 */
/**
 *  \brief      CsrBleHostDebond
 *  \details    The application should call this function in order to enbale or
 *              disable the use of the private resolvable address. The address
 *              would be regenerated randomly every 20 minutes.
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      useRandomAddress
 *              Disable or enable the use of the private resolvable address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostStartDeviceSearch()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostDebond(CSR_BLE_BLUETOOTH_ADDRESS device);

/**
 *  \brief      CsrBleHostJustWorksResult
 *  \details    The application should call this function in order to enbale or
 *              disable the use of the private resolvable address. The address
 *              would be regenerated randomly every 20 minutes.
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      useRandomAddress
 *              Disable or enable the use of the private resolvable address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostStartDeviceSearch()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostJustWorksResult(CSR_BLE_BLUETOOTH_ADDRESS device, BOOL accept, BOOL bond);

/**
 *  \brief      CsrBleHostPasskeyNotificationResult
 *  \details    The application should call this function in order to enbale or
 *              disable the use of the private resolvable address. The address
 *              would be regenerated randomly every 20 minutes.
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      useRandomAddress
 *              Disable or enable the use of the private resolvable address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostStartDeviceSearch()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostPasskeyNotificationResult(BOOL accept, CSR_BLE_BLUETOOTH_ADDRESS device);

/**
 *  \brief      CsrBleHostSetEncryption
 *  \details    The application should call this function in order to enbale or
 *              disable the use of the private resolvable address. The address
 *              would be regenerated randomly every 20 minutes.
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      useRandomAddress
 *              Disable or enable the use of the private resolvable address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostStartDeviceSearch()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostSetEncryption(DWORD connectHandle, BYTE security);

/**
 *  \brief      CsrBleHostChangeSecurityLevel
 *  \details    The application should call this function in order to change the  
 *              authentication level with the peer device. 
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      mitmRequired
 *              Mitm support is required or not.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 */
CSR_BLE_HOST_API void CsrBleHostChangeSecurityLevel(BOOL mitmRequired);

/**
 *  \brief      CsrBleHostCancelBonding
 *  \details    The application should call this function to cancel any ongoing bonding 
 *              request initiated by the user.
 *
 *
 *  \param      address
 *              address of the device on which bonding was initiated.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostSetEncryption()\n
 */

CSR_BLE_HOST_API void CsrBleHostCancelBonding(CSR_BLE_BLUETOOTH_ADDRESS address);

/**
 *  \name Connection Parameter Update
 *  \{
 */
/**
 *  \brief      CsrBleHostAcceptConnUpdate
 *  \details    The application should call this function in order to enbale or
 *              disable the use of the private resolvable address. The address
 *              would be regenerated randomly every 20 minutes.
 *
 *              This function should be called before connection establishment
 *              and device discovery.
 *
 *  \param      useRandomAddress
 *              Disable or enable the use of the private resolvable address.
 *
 *  \returns    TRUE in case of successful request, FALSE otherwise
 *
 *  \see        #CsrBleHostConnect()\n
 *              #CsrBleHostStartDeviceSearch()\n
 */
CSR_BLE_HOST_API BOOL CsrBleHostAcceptConnUpdate(DWORD connectHandle, WORD id, BOOL accept);


/**
 *  \name Remote GATT database discovery
 *  \{
 */

/**
 *  \brief      Discover all primary services
 *  \details    This function discovers all primary services of the connected
 *              GATT server. The information regarding found services is
 *              returned to the application with
 *              #CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT messages. The service
 *              discovery is completed with
 *              #CSR_BLE_CLIENT_SERVICE_DISCOVERY_STOPPED message.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientDiscoverServices()", URL="\ref #CsrBleClientDiscoverServices()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover all primary services"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="primary service description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="primary service description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="primary service description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="all services discovered"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_SERVICE_DISCOVERY_STOPPED", URL="\ref #CSR_BLE_CLIENT_SERVICE_DISCOVERY_STOPPED"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_CLIENT_SERVICE_DISCOVERY_RESULT\n
 *              #CSR_BLE_CLIENT_SERVICE_DISCOVERY_STOPPED
 */
CSR_BLE_HOST_API BOOL CsrBleClientDiscoverServices(DWORD connectHandle);

/**
 *  \brief      Discover service characteristics
 *  \details    This function discovers all characteristics of the specified
 *              service. The characteristics information is returned to the
 *              application with 
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT messages. The
 *              characterstic discovery is completed with
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED message.
 *
 *              #CsrBleClientDiscoverCharacteristicsByUuid() could be used to
 *              discover characteristic with specific UUID.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientDiscoverCharacteristics()", URL="\ref #CsrBleClientDiscoverCharacteristics()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover service characteristics"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="all characteristics discovered"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      startHandle
 *              First handle of the specified service, as returned from #CsrBleClientDiscoverServices()
 *
 *  \param      endHandle
 *              Last handle of the specified service, as returned from #CsrBleClientDiscoverServices()
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientDiscoverCharacteristicsByUuid()\n
 *              #CsrBleClientDiscoverServices()\n
 *              #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT\n
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED
 */
CSR_BLE_HOST_API BOOL CsrBleClientDiscoverCharacteristics(DWORD connectHandle, WORD startHandle, WORD endHandle);

/**
 *  \brief      Discover service characteristic by UUID
 *  \details    This function discovers all characteristics of the specified
 *              service with the given UUID. The characteristics information
 *              is returned to the application with 
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT messages. The
 *              characterstic discovery is completed with
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED message.
 *
 *              #CsrBleClientDiscoverCharacteristics() could be used to
 *              discover all characteristics of the service.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientDiscoverCharacteristicsByUuid()", URL="\ref #CsrBleClientDiscoverCharacteristicsByUuid()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover service characteristics"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic description"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="all characteristics discovered"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED", URL="\ref #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      uuid
 *              Characteristic UUID
 *
 *  \param      startHandle
 *              First handle of the specified service, as returned from #CsrBleClientDiscoverServices()
 *
 *  \param      endHandle
 *              Last handle of the specified service, as returned from #CsrBleClientDiscoverServices()
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientDiscoverCharacteristics()\n
 *              #CsrBleClientDiscoverServices()\n
 *              #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_RESULT\n
 *              #CSR_BLE_CLIENT_CHAR_DISCOVERY_STOPPED
 */
CSR_BLE_HOST_API BOOL CsrBleClientDiscoverCharacteristicsByUuid(DWORD connectHandle, CSR_BLE_UUID uuid, WORD startHandle, WORD endHandle);

/**
 *  \brief      Discover characteristic descriptors
 *  \details    This function discovers all characteristic descriptors for the
 *              given characteristic. The descriptor information is returned
 *              to the application with
 *              #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT messages. The
 *              descriptors discovery is completed with
 *              #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_STOPPED message.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientDiscoverCharacteristicDsc()", URL="\ref #CsrBleClientDiscoverCharacteristicDsc()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover characteristic descriptors"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="descriptor information"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="descriptor information"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="descriptor information"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="all descriptors discovered"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_STOPPED", URL="\ref #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_STOPPED"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      startHandle
 *              Starting handle of the discovery. Should be characteristic
 *              first handle + 1
 *
 *  \param      endHandle
 *              Last handle of the discovery. Set to the last service handle,
 *              as returned from #CsrBleClientDiscoverServices()
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientDiscoverCharacteristics()\n
 *              #CsrBleClientDiscoverServices()\n
 *              #CsrBleClientDiscoverDatabase()\n
 *              #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_RESULT\n
 *              #CSR_BLE_CLIENT_CHAR_DSC_DISCOVERY_STOPPED
 */
CSR_BLE_HOST_API BOOL CsrBleClientDiscoverCharacteristicDsc(DWORD connectHandle, WORD startHandle, WORD endHandle);

/**
 *  \brief      Discover GATT server database
 *  \details    This helper function discovers whole GATT database of the
 *              connected GATT server. After database discovered, the
 *              application receives #CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT
 *              message with the database contents.
 *
 *              #CsrBleClientDiscoverServices(),
 *              #CsrBleClientDiscoverCharacteristics(),
 *              #CsrBleClientDiscoverCharacteristicsByUuid() and
 *              #CsrBleClientDiscoverCharacteristicDsc() could be used to
 *              discover specific GATT server services and characteristics.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientDiscoverDatabase()", URL="\ref #CsrBleClientDiscoverDatabase()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover all services"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="services information"];
 *              ...;
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover services characteristics"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristics information"];
 *              ...;
 *              "uEnergy Host Library"->"Remote GATT server" [label="discover characteristics descriptors"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="descriptors information"];
 *              ...;
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT", URL="\ref #CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientDiscoverServices()\n
 *              #CsrBleClientDiscoverCharacteristics()\n
 *              #CsrBleClientDiscoverCharacteristicsByUuid()\n
 *              #CsrBleClientDiscoverCharacteristicDsc()\n
 *              #CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleClientDiscoverDatabase(DWORD connectHandle);

/**
 *  \}
 */

/**
 *  \name Remote GATT database access
 *  \{
 */

/**
 *  \brief      Read characteristic value by handle
 *  \details    This function reads the value of the specified characteristic
 *              in the remote GATT database. The read value is returned with
 *              #CSR_BLE_CLIENT_CHAR_READ_RESULT message.
 *
 *              The reading fails when remote characteristic does not have read
 *              permissions.
 *
 *              Multiple characteristics could be read using
 *              #CsrBleClientReadCharsByHandles().
 *
 *              Characteristic value could be read by characteristic UUID using
 *              #CsrBleClientReadCharByUuid().
 *
 *              This operation could be cancelled by call to
 *              #CsrBleHostCancel().
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientReadCharByHandle()", URL="\ref #CsrBleClientReadCharByHandle()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="read characteristic by handle"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic value or error"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_READ_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_READ_RESULT"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      charHandle
 *              Characteristic value handle
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientReadCharsByHandles()\n
 *              #CsrBleClientReadCharByUuid()\n
 *              #CsrBleHostCancel()\n
 *              #CSR_BLE_CLIENT_CHAR_READ_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleClientReadCharByHandle(DWORD connectHandle, WORD charHandle);

/**
 *  \brief      Read characteristics by handle
 *  \details    This function reads the values of the specified characteristics
 *              in the remote GATT database. The read value is returned with
 *              #CSR_BLE_CLIENT_CHAR_READ_MULTI_RESULT message.
 *
 *              The reading fails when remote characteristic does not have read
 *              permissions. The remote GATT server may not support multiple
 *              attribute reading.
 *
 *              This operation could be cancelled by call to
 *              #CsrBleHostCancel().
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientReadCharsByHandles()", URL="\ref #CsrBleClientReadCharsByHandles()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="read characteristics by handles"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristics values or error"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_READ_MULTI_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_READ_MULTI_RESULT"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      userTag
 *              User-specified value to distinguish between several read requests
 *
 *  \param      nHandles
 *              Number of characteristics to read
 *
 *  \param      charHandle
 *              Characteristics value handles
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleHostCancel()\n
 *              #CSR_BLE_CLIENT_CHAR_READ_MULTI_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleClientReadCharsByHandles(DWORD connectHandle, WORD userTag, int nHandles, WORD* charHandle);

/**
 *  \brief      Read characteristic value by UUID
 *  \details    This function searches the remote GATT database for the
 *              characteristic with the specified UUID and reads its value, if
 *              permitted. The read value is returned with
 *              #CSR_BLE_CLIENT_CHAR_READ_RESULT message.
 *
 *              The reading fails when remote attribute does not have read
 *              permissions.
 *
 *              This operation could be cancelled by call to
 *              #CsrBleHostCancel().
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientReadCharByUuid()", URL="\ref #CsrBleClientReadCharByUuid()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="read characteristic value by UUID"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="characteristic value or error"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_READ_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_READ_RESULT"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      uuid
 *              Characteristic UUID
 *
 *  \param      startHandle
 *              First handle of the characteristic service
 *
 *  \param      endHandle
 *              Last handle of the characteristic service
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientReadCharByHandle()\n
 *              #CsrBleHostCancel()\n
 *              #CSR_BLE_CLIENT_CHAR_READ_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleClientReadCharByUuid(DWORD connectHandle, CSR_BLE_UUID uuid, WORD startHandle, WORD endHandle);

/**
 *  \brief      Write characteristic value
 *  \details    This function writes without confirmation to the remote GATT
 *              database.
 *
 *              The write procedure fails when remote attribute does not have
 *              "write-without-response" permissions. This function cannot be
 *              used to write long characteristic values (i.e. over (MTU - 3)
 *              octets), #CsrBleClientWriteCfmCharByHandle() should be used
 *              instead.
 *
 *              The application receives #CSR_BLE_CLIENT_CHAR_WRITE_RESULT to
 *              indicate local result of the operation.
 *              #CsrBleClientWriteCfmCharByHandle() requests acknowledgement
 *              from the remote side.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientWriteCharByHandle()", URL="\ref #CsrBleClientWriteCharByHandle()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="write characteristic value w/out response"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_WRITE_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_WRITE_RESULT"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      isSigned
 *              Specifies whether signed write should be used for this
 *              operation. Characteristic should have "signed write"
 *              permission in this case.
 *
 *  \param      charHandle
 *              Characteristic value handle
 *
 *  \param      valueSize
 *              Value size
 *
 *  \param      value
 *              Value to write
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientWriteCfmCharByHandle()\n
 *              #CSR_BLE_CLIENT_CHAR_WRITE_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleClientWriteCharByHandle(DWORD connectHandle, BOOL isSigned, WORD charHandle, WORD valueSize, LPVOID value);

/**
 *  \brief      Write characteristic value with confirmation
 *  \details    This function writes with confirmation to the remote GATT
 *              database.
 *
 *              The write procedure fails when remote attribute does not have
 *              "write" permissions. This function can be used to write long
 *              characteristic values (i.e. over (MTU - 3) octets).
 *
 *              The application receives #CSR_BLE_CLIENT_CHAR_WRITE_RESULT to
 *              indicate the result of the operation.
 *
 *              This write of the long value could be cancelled by call to
 *              #CsrBleHostCancel().
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientWriteCfmCharByHandle()", URL="\ref #CsrBleClientWriteCfmCharByHandle()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="write characteristic value"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="write confirmation"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_WRITE_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_WRITE_RESULT"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      charHandle
 *              Characteristic value handle
 *
 *  \param      offset
 *              Characteristic value offset in the octets 
 *
 *  \param      valueSize
 *              Value size
 *
 *  \param      value
 *              Value to write
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleClientWriteCfmCharByHandle()\n
 *              #CsrBleHostCancel()\n
 *              #CSR_BLE_CLIENT_CHAR_WRITE_RESULT
 */
CSR_BLE_HOST_API BOOL CsrBleClientWriteCfmCharByHandle(DWORD connectHandle, WORD charHandle, WORD offset, WORD valueSize, LPVOID value);

/**
 *  \brief      Write client configuration value
 *  \details    This function writes client configuration value for the
 *              specified characteristic in the remote GATT database.
 *
 *              The application receives #CSR_BLE_CLIENT_CHAR_WRITE_RESULT to
 *              indicate the result of the operation.
 *
 *              The notifications and indications from GATT server are
 *              delivered to the application via
 *              #CSR_BLE_CLIENT_CHAR_NOTIFICATION messages.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientWriteCharByHandle()", URL="\ref #CsrBleClientWriteCharByHandle()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="write client configuration"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="write response"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_WRITE_RESULT", URL="\ref #CSR_BLE_CLIENT_CHAR_WRITE_RESULT"];
 *              ...;
 *              "uEnergy Host Library"<-"Remote GATT server" [label="notification or indication"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_CLIENT_CHAR_NOTIFICATION", URL="\ref #CSR_BLE_CLIENT_CHAR_NOTIFICATION"];
 *  \endmsc
 *
 *  \param      connectHandle
 *              Connection handle
 *
 *  \param      charDscHandle
 *              Client configuration handle
 *
 *  \param      charHandle
 *              Characteristic value handle
 *
 *  \param      config
 *              Client configuration value, see \ref client_config_values "client configurations".
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CSR_BLE_CLIENT_CHAR_WRITE_RESULT\n
 *              #CSR_BLE_CLIENT_CHAR_NOTIFICATION
 */
CSR_BLE_HOST_API BOOL CsrBleClientWriteConfiguration(DWORD connectHandle, WORD charDscHandle, WORD charHandle, BYTE config);

/**
 *  \brief      Allocates memory for database handles for this server
 *  \details    This function allocates memory for the specified no. of
 *              of GATT database handles.
 *
 *              The application receives #CSR_BLE_SERVER_ALLOC_DATABASE_RESULT to
 *              indicate the result of the operation.
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="CsrBleClientWriteConfiguration()", URL="\ref #CsrBleClientWriteConfiguration()"];
 *              Application<<"uEnergy Host Library" [label="request result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="allocate server database"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="allocate server db result"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_SERVER_ALLOC_DATABASE_RESULT", URL="\ref #CSR_BLE_SERVER_ALLOC_DATABASE_RESULT"];
 *              ...;
 *  \endmsc
 *
 *  \param      nHandles
 *              number of handles to store
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CSR_BLE_SERVER_ALLOC_DATABASE_RESULT\n
 */
CSR_BLE_HOST_API BOOL CsrBleServerAllocDatabase(WORD nHandles);

/**
 *  \brief      Adds the service to the server database 
 *  \details    This function adds the specified service to the server database. 
 *              if a Service has been added to the database, the server application shall notify the 
 *              clients which it is bonded to. 
 *
 *              The application receives #CSR_BLE_SERVER_ADD_DATABASE_RESULT to
 *              indicate the result of the operation.
 *
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="add database()", URL="\ref #CsrBleServerAddDatabase()"];
 *              Application<<"uEnergy Host Library" [label="add database result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="add database"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="add database response"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_SERVER_ADD_DATABASE_RESULT", URL="\ref #CSR_BLE_SERVER_ADD_DATABASE_RESULT"];
 *              ...;
 *  \endmsc
 *
 *  \param      startHandle
 *              start handle
 *
 *  \param      nServices
 *              no of services to register
 *
 *  \param      services
 *              Service data
 *
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleServerAllocDatabase\n
 */
CSR_BLE_HOST_API BOOL CsrBleServerAddDatabase(WORD startHandle, WORD nServices, PCSR_BLE_DB_SERVICE services);

/**
 *  \brief      Response to a read request 
 *  \details    This function is called on the server in response to a read request.
 *              To this the application should respond by invoking the below API. 
 *
 *              The application receives #PCSR_BLE_MSG_NOTIFY structure as a confirmation result to this request.
 *
 *              The application receives #CSR_BLE_SERVER_DATABASE_READ_INDICATION to
 *              indicate the read request from the client.
 *
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="read data()", URL="\ref #CsrBleServerReadResponse()"];
 *              Application<<"uEnergy Host Library" [label="add database result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="read data"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="read data response"];
 *              Application<-"uEnergy Host Library" [label="CSR_BLE_SERVER_DATABASE_READ_INDICATION", URL="\ref #CSR_BLE_SERVER_DATABASE_READ_INDICATION"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              connection handle
 *
 *  \param      handle
 *              handle which is read
 *
 *  \param      response
 *              Response status
 *
 *  \param      length
 *              data length
 *
 *  \param      value
 *              Read response data
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #CsrBleServerReadResponse\n
 */
CSR_BLE_HOST_API BOOL CsrBleServerReadResponse(DWORD connectHandle, WORD handle, WORD response, WORD length, LPVOID value);

/**
 *  \brief      Response to a write request 
 *  \details    This function is called on the server in response to a write request from the client
 *
 *              The application receives #CSR_BLE_SERVER_DATABASE_WRITE_INDICATION to
 *              indicate the write request from the client. To this the application should respond by
 *              invoking the below API. 
 *
 *              The application receives #PCSR_BLE_MSG_NOTIFY structure as a confirmation result to this request.
 *
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="write data()", URL="\ref #CsrBleServerWriteResponse()"];
 *              Application<<"uEnergy Host Library" [label="write data result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="write data"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="write data response"];
 *              Application<-"uEnergy Host Library" [label="PCSR_BLE_MSG_NOTIFY", URL="\ref #PCSR_BLE_MSG_NOTIFY"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              connect handle
 *
 *  \param      handle
 *              handle on which written
 *
 *  \param      response
 *              Write response status
 *
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #PCSR_BLE_MSG_NOTIFY\n
 */
CSR_BLE_HOST_API BOOL CsrBleServerWriteResponse(DWORD connectHandle, WORD handle, WORD response);

/**
 *  \brief      Service Notifications 
 *  \details    If the client has configured the server to send a notification or an indication to the client 
 *              the server application shall send an event to the client when the Characteristic Value is changed. 
 *
 *              The application receives #PCSR_BLE_MSG_NOTIFY structure as a confirmation result.
 *
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="write notification()", URL="\ref #CsrBleServerSendNotification()"];
 *              Application<<"uEnergy Host Library" [label="write notification result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="write notification"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="write notification response"];
 *              Application<-"uEnergy Host Library" [label="CsrBleServerSendNotification", URL="\ref #CsrBleServerSendNotification"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              connect handle
 *
 *  \param      handle
 *              handle on which written
 *
 *  \param      length
 *              length of the data notified
 *
 *  \param      value
 *              array of data that is notified
 *
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #PCSR_BLE_MSG_NOTIFY\n
 */
CSR_BLE_HOST_API BOOL CsrBleServerSendNotification(DWORD connectHandle, WORD handle, WORD length, LPVOID value);

/**
 *  \brief      Service Indications
 *  \details    If the client has configured the server to send a notification or an indication to the client 
 *              the server application shall send an event to the client when the Characteristic Value is changed. 
 *
 *              The application receives #PCSR_BLE_MSG_NOTIFY structure as a confirmation result.
 *
 *
 *  \msc
 *              hscale="1.5", wordwraparcs="true";
 *              Application, "uEnergy Host Library", "Remote GATT server";
 *
 *              Application=>"uEnergy Host Library" [label="write indication()", URL="\ref #CsrBleServerSendIndication()"];
 *              Application<<"uEnergy Host Library" [label="write indication result"];
 *              "uEnergy Host Library"->"Remote GATT server" [label="write indication"];
 *              "uEnergy Host Library"<-"Remote GATT server" [label="write indication response"];
 *              Application<-"uEnergy Host Library" [label="CsrBleServerSendIndication", URL="\ref #CsrBleServerSendIndication"];
 *              ...;
 *  \endmsc
 *
 *  \param      connectHandle
 *              connect handle
 *
 *  \param      handle
 *              handle on which written
 *
 *  \param      length
 *              length of the data indicated
 *
 *  \param      value
 *              array of data that is indicated
 *
 *
 *  \returns    TRUE in case of success, FALSE otherwise
 *
 *  \see        #PCSR_BLE_MSG_NOTIFY\n
 */
CSR_BLE_HOST_API BOOL CsrBleServerSendIndication(DWORD connectHandle, WORD handle, WORD length, LPVOID value);

#ifdef __cplusplus
}
#endif

#endif /* __UENERGYHOST_H__ */
