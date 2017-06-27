/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BLEInterface.h
 *
 *  DESCRIPTION
 *      This class handles  with the remote device using the
 *      uEnergy DLL.
 *
 ******************************************************************************/

#ifndef _BLE_INTERFACE_H
#define _BLE_INTERFACE_H

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include "stdafx.h"
#include <queue>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "BleRadio.h"
#include "uEnergyImage.h"
#include "MessageDefs.h"
#include "BLESupport.h"
#include "BleDevice.h"

/*============================================================================*
 *  Public Data Types
 *============================================================================*/
typedef enum RadioState
{
   APP_IDLE,
   APP_SCANNING,
};

typedef enum DeviceState
{
   DEV_INIT,
   DEV_IDLE,
   DEV_CONNECTING,
   DEV_CONNECTED,
   DEV_CANCEL_CONNECTING,
   DEV_DISCOVERING_SVCS,
   DEV_DISCONNECTING,
   DEV_RENAMING,
   DEV_DEBONDING,
   DEV_UPDATING_CONNPARAM,
};


/*============================================================================*
 *  Public Class Definition
 *============================================================================*/
class CBLEInterface: public CBleRadio
{
   enum WriteType
   {
      writeRequest,
      writeCommand
   };


   struct CharWriteValue
   {
      DWORD connHandle;
      WORD charHandle;
      WORD charCfgHandle;
      BYTE charCfgValue;
      BYTE charValue[128];
      BYTE charValueLen;
      WriteType writeType;
   };
public:

    /*============================================================================*
     *  Public Function Prototypes
     *============================================================================*/

    /* The class destructor */
    ~CBLEInterface();

    /* This function initialises the underlying radio dongle */
    virtual RADIO_FAILURES BleInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging);

    virtual void BleDeInit();

    /* Process a message from the uEnergy DLL */
    void BleProcessMessage(WPARAM wParam, LPARAM lParam);

    /* Cancel any outstanding transaction */
    void BleCancelCurrentOp();

   /* Remove the bonding information associated with the specified address */
    BOOL BleRemoveBond(CSR_BLE_BT_ADDRESS *BluetoothAddress);

    // Returns the state of the radio
    RadioState GetRadioState() { return m_CurRadioState;}

    // Gets the connection state of the device
    DeviceState GetConnectionState(CSR_BLE_BT_ADDRESS *device) ;

    // Handles all BLE message from the stack
    void BLEProcessMessage(WPARAM wParam, LPARAM lParam);

    // Returns the state of the bonded infromation of the device
    BOOL IsBonded(CSR_BLE_BLUETOOTH_ADDRESS DeviceAddr);

    // Interface to start the scan.
    BOOL BleStartScan();
    
    // Interface to stop the scan.
    BOOL BleStopScan();
    
    // Interface to initiate a connection or disconnect 
    BOOL ToggleConnection(CSR_BLE_BT_ADDRESS *pAddr);
    
    // Cancel an initiated connection 
    void CancelConnection();

	// Get Battery level handle
    WORD GetBatteryLevelHandle();

	// Get Battery level configuration handle
	WORD GetBatteryLevelCfgHandle();

	void SetMITMFlag(BOOL value);

	void SetMITMStatus(BOOL value);
    
	//Data
	WORD        m_nMitmAttempts; 
	BleDevice  *m_CurBleDevice;
		
protected:
    /*============================================================================*
     *  Private Functions
     *============================================================================*/
      virtual void OnBleSearchResult(WPARAM wParam, LPARAM lParam);
      virtual void OnBleConnectionUpdateRequest(WPARAM wParam, LPARAM lParam);
      virtual void OnBleDatabaseDiscoveryResult(WPARAM wParam, LPARAM lParam);
      virtual void OnBleSetEncryptionResult(WPARAM wParam, LPARAM lParam);
      virtual void OnBlePanic(WPARAM wParam, LPARAM lParam);
      virtual void OnBleReady(WPARAM wParam, LPARAM lParam);
      virtual void OnBleSearchStopped(WPARAM wParam, LPARAM lParam);
      virtual void OnBleLEScanStatus(WPARAM wParam, LPARAM lParam);
      virtual void OnBleConnect(WPARAM wParam, LPARAM lParam);
      virtual void OnBleHostSecurityResult(WPARAM wParam, LPARAM lParam);
      virtual void OnBleJustWorksRequest(WPARAM wParam, LPARAM lParam);
      virtual BOOL OnBleWriteCharResult(WPARAM wParam, LPARAM lParam);
      virtual void OnBleNotificationReceived(WPARAM wParam, LPARAM lParam) = 0;
      virtual BOOL OnBleReadCharResult(WPARAM wParam, LPARAM lParam);
      virtual void OnBleDebondResult(WPARAM wParam, LPARAM lParam);
      
      WORD BleParseDatabase(PCSR_BLE_DATABASE_DISCOVERY_RESULT dbRes);
      void ClearDatabase();

      void SendAllPairedDevicesToUI();
      virtual void SetDeviceState(DeviceState devState);
      void SetRadioState(RadioState radioState) {m_CurRadioState = radioState;}
   
      CString FromUtf8(LPCSTR str);
      virtual void AppInit() = 0;

      // Reading an attribute
      void AddAttributeForReading(WORD handle);
      void AddAttributeForWriting(WORD handle, WORD cCfgHandle, BYTE charValue);
      void AddAttributeForWriting(WORD handle, BYTE charValueLen, 
                                           LPBYTE charValue, WriteType wType = writeRequest);

      void ReadAttributes();
      void BleReadCharacteristic(DWORD conHandle, WORD handle);
      void WriteAttributes();
      BOOL BleWriteCharacteristic(CharWriteValue *value);

    /*============================================================================*
     *  Private Data
     *============================================================================*/
protected:
	
    /* The Bluetooth address of the remote device. */
    BOOL m_bEncrypted ;
	BOOL m_EnableMitm;
	BOOL bMitmInProgress;
    RadioState m_CurRadioState;
   

    DeviceState m_DeviceState;

    std::vector<BleDevice*> m_Devices;
    std::vector<BleDevice*> m_PairedDevices;

    BOOL mRetryDbRead;
    
    /* Record whether the service-change indications have been requested */
    BOOL mServiceChangeConfigured;

	/* Record whether the battery level notifications have been requested */
	BOOL mBatteryLevelConfigured;

    // Handle to the service changed indication...
    WORD m_LastGattServChgHandle;

    std::queue<WORD> m_ReadAttrList;
    bool m_ReadingAttributes;

    std::queue<CharWriteValue> m_WriteAttrList;
    bool m_WritingAttributes;
    CharWriteValue m_CurWriteAttribute;

    bool encrypting;
};


#endif /* _BLE_INTERFACE_H */