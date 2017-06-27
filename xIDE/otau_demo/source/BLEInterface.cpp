/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BLEInterface.cpp
 *
 *  DESCRIPTION
 *      This class handles communications with the remote device using the
 *      uEnergy DLL.
 *
 ******************************************************************************/

#include "stdafx.h"
#include "BLEInterface.h"
#include "BleRadio.h"
#include "uiMsg.h"
#include "uuids.h"
#include "HelperFunc.h"
#include "resource.h"

/*----------------------------------------------------------------------------*
*  NAME
*      ~CBLEInterface
*
*  DESCRIPTION
*      CBLEInterface destructor
*---------------------------------------------------------------------------*/
CBLEInterface::~CBLEInterface(void)
{
   RadioDeInit();
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleInit
*
*  DESCRIPTION
*      Initialise the underlying dongle
*---------------------------------------------------------------------------*/
RADIO_FAILURES CBLEInterface::BleInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging)
{
   m_DeviceState = DEV_INIT;
   m_CurRadioState = APP_IDLE;
   m_bEncrypted = FALSE;
   m_EnableMitm = FALSE;
   bMitmInProgress = FALSE;
   m_nMitmAttempts = 0;
   m_CurBleDevice = NULL;
   m_ReadingAttributes = false;
   m_WritingAttributes = false;
   encrypting = false;
   return RadioInit(hWnd, bleClientHandle, bLogging);
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleInit
*
*  DESCRIPTION
*      DeInitialises the underlying dongle
*---------------------------------------------------------------------------*/
void CBLEInterface::BleDeInit()
{
   for(std::vector<BleDevice*>::iterator i = m_Devices.begin(); i != m_Devices.end(); i++)
   {
      BleDevice* curDev = *i;
      delete curDev;
   }
   m_Devices.clear();
   RadioDeInit();
}


/*----------------------------------------------------------------------------*
*  NAME
*      BleStartScan
*
*  DESCRIPTION
*      Start scanning(searching) for devices in range...
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::BleStartScan()
{
   if(m_DeviceState == DEV_IDLE && m_CurRadioState == APP_IDLE)
   {
      // TODO delete BleDevices here...
      for(std::vector<BleDevice*>::iterator i = m_Devices.begin(); i != m_Devices.end(); i++)
      {
         BleDevice* curDev = *i;
         delete curDev;
      }
      m_Devices.clear();

      // Update the UI to show that scanning has started
      UiMsg::SendMsgToUI(DEVICE_SCANNING_UPDATE, NULL);

      // Start scanning
      CsrBleHostStartLEScan();
      return TRUE;
   }
   return FALSE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleStopScan
*
*  DESCRIPTION
*      Stop scanning(searching) for devices in range...
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::BleStopScan()
{
   if(m_CurRadioState == APP_SCANNING)
   {
      CsrBleHostStopLESearch();
   }

   return TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      ToggleConnection
*
*  DESCRIPTION
*      Connect/Disconnect to the specific device
*---------------------------------------------------------------------------*/

BOOL CBLEInterface::ToggleConnection(CSR_BLE_BT_ADDRESS *devAddr)
{
   if(devAddr == NULL)
   {
      return FALSE;
   }

   if(m_CurBleDevice == NULL)
   {
      BOOL devFound = FALSE;

      for(std::vector<BleDevice*>::iterator i = m_Devices.begin(); i != m_Devices.end(); i++)
      {
         BleDevice* curDev = *i;
         // add unique addresses only
         CSR_BLE_BLUETOOTH_ADDRESS bAddr = curDev->GetAddress();
         if((bAddr.lAp == devAddr->lAp) && 
            (bAddr.uAp == devAddr->uAp) &&
            (bAddr.nAp == devAddr->nAp))
         {
            m_CurBleDevice = curDev;
            devFound = TRUE;
			if(m_CurBleDevice->GetAppearance() == CSR_BLE_APPEARANCE_HID_KEYBOARD)
            {
				m_nMitmAttempts = 0;
				if(m_EnableMitm)
				{
					CsrBleHostChangeSecurityLevel(TRUE);
				}				
            }
			break;
         }
      }
   }
   else
   {
      // If the disconnection is requetsted for a different device...
      CSR_BLE_BLUETOOTH_ADDRESS bAddr = m_CurBleDevice->GetAddress();
      if((bAddr.lAp != devAddr->lAp) || 
         (bAddr.uAp != devAddr->uAp) ||
         (bAddr.nAp != devAddr->nAp))
      {
         return FALSE;
      }
   }

   if(m_DeviceState == DEV_IDLE)
   {
      // The user wants to try establishing a connection with a remote device.
      //intialise connection params
      //idle state to connecting state
      CString logString;
      logString.LoadString(IDS_DEV_CONNECTING);
      UiMsg::LogMessage(logString, STATUS_TYPE);
      SetDeviceState(DEV_CONNECTING);
   }
   else /*if(m_DeviceState == DEV_CONNECTED)*/
   {
      // The user wants to disconnect from the current device
      SetDeviceState(DEV_DISCONNECTING);
   }
   return TRUE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      BLEProcessMessage
*
*  DESCRIPTION
*      Handles the events received from the radio DLL.
*---------------------------------------------------------------------------*/
void CBLEInterface::BLEProcessMessage(WPARAM wParam, LPARAM lParam)
{
   CString pString;

   switch(wParam)
   {
   case CSR_BLE_HOST_PANIC:
      OnBlePanic(wParam, lParam);
      break;

   case CSR_BLE_HOST_READY:
      OnBleReady(wParam, lParam);
      break;

   case CSR_BLE_HOST_SEARCH_RESULT:
      OnBleSearchResult(wParam, lParam);
      break;

   case CSR_BLE_HOST_SEARCH_STOPPED:
      OnBleSearchStopped(wParam, lParam);
      break;

   case CSR_BLE_HOST_LE_SCAN_STATUS:
      OnBleLEScanStatus(wParam, lParam);
      break;

   case CSR_BLE_HOST_CONNECT_RESULT:
      OnBleConnect(wParam, lParam);
      break;

   case CSR_BLE_HOST_JUSTWORKS_REQUEST:
      OnBleJustWorksRequest(wParam, lParam);
      break;
   
   case CSR_BLE_HOST_DISPLAY_PASSKEY_IND:
      break;

   case CSR_BLE_HOST_SECURITY_RESULT:
      OnBleHostSecurityResult(wParam, lParam);
      break;

   case CSR_BLE_HOST_SET_ENCRYPTION_RESULT:
      OnBleSetEncryptionResult(wParam, lParam);
      break;

   case CSR_BLE_HOST_DISCONNECTED:
      {		 
      CSR_BLE_DISCONNECTED* pParam = (CSR_BLE_DISCONNECTED*)lParam;
      pString.LoadString(IDS_APP_DISCONNECTED);
      CString strMsg;
      strMsg.Format(pString, pParam->supplier, pParam->reason);
      UiMsg::LogMessage(strMsg, STATUS_TYPE);
      
	  /* Re-initialise the security level */
	  CsrBleHostChangeSecurityLevel(FALSE);

      /* Re-initialise encrypting to false */
	  encrypting = false;
	  
	  /* clear attribute list */
	  while(!m_ReadAttrList.empty())
            m_ReadAttrList.pop();
	  
	  while(!m_WriteAttrList.empty())
            m_WriteAttrList.pop();
	  
	  m_ReadingAttributes = false;
	  m_WritingAttributes = false;

      //Set the state to idle
      SetDeviceState(DEV_IDLE);
      }
      break;

   case CSR_BLE_CLIENT_DATABASE_DISCOVERY_RESULT:
      OnBleDatabaseDiscoveryResult(wParam, lParam);
      break;

   case CSR_BLE_CLIENT_CHAR_READ_RESULT:
      OnBleReadCharResult(wParam, lParam);
      break;

   case CSR_BLE_CLIENT_CHAR_WRITE_RESULT:
      OnBleWriteCharResult(wParam, lParam);
      break;

   case CSR_BLE_CLIENT_CHAR_NOTIFICATION:
      OnBleNotificationReceived(wParam, lParam);
      break;

   case CSR_BLE_HOST_CONNECTION_UPDATE_REQUEST:
      OnBleConnectionUpdateRequest(wParam, lParam);
      break;

   case CSR_BLE_HOST_DEBOND_RESULT:
      OnBleDebondResult(wParam, lParam);
      break;

   default:
      break;
   }

   //free the ble radio event
   CsrBleHostFreeMessageContents(wParam, lParam);
}


/*----------------------------------------------------------------------------*
*  NAME
*      AddAttributeForReading
*
*  DESCRIPTION
*      Add Attributes for reading from the device.
*---------------------------------------------------------------------------*/
void CBLEInterface::AddAttributeForReading(WORD handle)
{
   if(handle != Uuids::UUID_INVALID)
   {
      m_ReadAttrList.push(handle);

      if(!m_ReadingAttributes)
         ReadAttributes();
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      ReadAttributes
*
*  DESCRIPTION
*      Reads the queued attributes from the device.
*---------------------------------------------------------------------------*/
void CBLEInterface::ReadAttributes()
{
   if(m_CurBleDevice == NULL)
      return ;
    DWORD connHandle = m_CurBleDevice->GetConnectionHandle();

    if(!m_ReadingAttributes)
    {
        // try to continue reading
        WORD attrHandle = Uuids::UUID_INVALID;

        // find next valid handle
        while(!m_ReadAttrList.empty()) {
            attrHandle = m_ReadAttrList.front();
            m_ReadAttrList.pop();

            if(attrHandle != Uuids::UUID_INVALID)
                break;
        }

        if(attrHandle != Uuids::UUID_INVALID)
        {
            m_ReadingAttributes = true;
            BleReadCharacteristic(connHandle, attrHandle);
        }
    }
    else
    {
       // read the current char...
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleReadCharacteristic
*
*  DESCRIPTION
*      Reads the specific attributes from the device.
*---------------------------------------------------------------------------*/
void CBLEInterface::BleReadCharacteristic(DWORD conHandle, WORD handle)
{
    // read attribute if handle is valid
   if(handle != Uuids::UUID_INVALID)
        CsrBleClientReadCharByHandle(conHandle, handle);
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleWriteCharacteristic
*
*  DESCRIPTION
*      Writes the specific attribute to the device.
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::BleWriteCharacteristic(CharWriteValue *value)
{
   if(value == NULL)
      return FALSE;

   if(value->charCfgHandle != Uuids::UUID_INVALID)
   {
      m_WritingAttributes = true;

      CsrBleClientWriteConfiguration(value->connHandle,
         value->charCfgHandle, value->charHandle, value->charCfgValue);
   }
   else if(value->charHandle != Uuids::UUID_INVALID)
   {
      m_WritingAttributes = true;

      if(value->writeType == writeRequest)
      {
         CsrBleClientWriteCfmCharByHandle(value->connHandle,
            value->charHandle, 0, value->charValueLen, value->charValue);
      }
      else if(value->writeType == writeCommand)
      {
         CsrBleClientWriteCharByHandle(value->connHandle,0,
            value->charHandle, value->charValueLen, value->charValue);
      }
      else
      {
         // Log error here
      }             
   }
   return TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      AddAttributeForWriting
*
*  DESCRIPTION
*      Adds the attribute to write to a queue
*---------------------------------------------------------------------------*/
void CBLEInterface::AddAttributeForWriting(WORD handle, WORD cCfgHandle, BYTE charValue)
{
    if(handle != Uuids::UUID_INVALID && m_CurBleDevice != NULL)
    {
        CharWriteValue value;
        value.charHandle = handle;
        value.charCfgHandle = cCfgHandle;
        value.charCfgValue = charValue;
        value.writeType = writeRequest;
        value.connHandle = m_CurBleDevice->GetConnectionHandle();

        m_WriteAttrList.push(value);

        if(!m_WritingAttributes)
            WriteAttributes();
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      AddAttributeForWriting
*
*  DESCRIPTION
*      Adds the attribute to write to a queue
*---------------------------------------------------------------------------*/
void CBLEInterface::AddAttributeForWriting(WORD handle, BYTE charValueLen, 
                                           LPBYTE charValue, 
                                           WriteType wType/* = writeRequest*/)
{
    if(handle != Uuids::UUID_INVALID && m_CurBleDevice != NULL)
    {
        CharWriteValue value;
        value.charHandle = handle;
        value.charCfgHandle = Uuids::UUID_INVALID;
        value.charCfgValue = 0;
        value.charValueLen = charValueLen;
        value.writeType = wType;
        memcpy_s(value.charValue, 128, charValue, charValueLen);
        value.connHandle = m_CurBleDevice->GetConnectionHandle();

        m_WriteAttrList.push(value);

        if(!m_WritingAttributes)
            WriteAttributes();
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      WriteAttributes
*
*  DESCRIPTION
*      Writes the attribute to the remote device
*---------------------------------------------------------------------------*/
void CBLEInterface::WriteAttributes()
{
    if(!m_WritingAttributes)
    {
        // try to continue reading
        CharWriteValue value;
        value.charHandle = Uuids::UUID_INVALID;

        BOOL bCharFound = FALSE;

        // find next valid handle
        while(!m_WriteAttrList.empty()) {
            value = m_WriteAttrList.front();
            
            m_CurWriteAttribute = value;
            
            if(value.charHandle != Uuids::UUID_INVALID)
            {
               bCharFound = TRUE;
               break;
            }
            else
                m_WriteAttrList.pop();
        }

        if(bCharFound == TRUE)
        {
            BleWriteCharacteristic (&value);
        }
    }
    else
    {
       // write the current char...
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      FromUtf8
*
*  DESCRIPTION
*      Converts from Utf8 to String
*---------------------------------------------------------------------------*/
CString CBLEInterface::FromUtf8(LPCSTR str)
{
    CString devName;

    int strSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);

    if(strSize)
    {
        LPWSTR utfStr = new WCHAR[strSize];

        strSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, utfStr, strSize);

        devName = utfStr;

        delete [] utfStr;
    }
    else
    {
        devName = str;
    }

    return devName;
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleCancelCurrentOp
*
*  DESCRIPTION
*      This method cancels the current operation
*---------------------------------------------------------------------------*/
void CBLEInterface::BleCancelCurrentOp()
{
   CsrBleHostCancel();
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnBleSearchResult
*
*  DESCRIPTION
*      Another device has been found during the device scan.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleSearchResult(WPARAM wParam, LPARAM lParam)
{
   BleDevice* bleDevice = NULL;
   PCSR_BLE_DEVICE_SEARCH_RESULT searchResult = (PCSR_BLE_DEVICE_SEARCH_RESULT)lParam;

   for(std::vector<BleDevice*>::iterator i = m_Devices.begin(); i != m_Devices.end(); i++)
   {
      BleDevice* curDev = *i;
      // add unique addresses only
      if(*curDev == searchResult->deviceAddress)
      {
         if(curDev->IsPaired())
         {
            // device already paired and using undirected advertisments
            bleDevice = *i;
            break;
         }

         if(searchResult->deviceName != NULL)
         {
			 // found the device with the same address, update the name
			 if(curDev->GetName().Compare(FromUtf8(searchResult->deviceName)))
			 {
			     curDev->SetName(FromUtf8(searchResult->deviceName));

				 // Send the details to the UI
				 SEARCH_RESULT_T *pDevice = new SEARCH_RESULT_T;
				 CSR_BLE_BLUETOOTH_ADDRESS addr = curDev->GetAddress();
				 memcpy(&pDevice->DevAddr, &addr, sizeof(addr));
				 pDevice->DeviceName = curDev->GetName();
				 UiMsg::SendMsgToUI(SEARCH_RESULT, (LPVOID)pDevice); 
			 }
         }

         // update TX level if reported
         if(searchResult->informationReported & CSR_BLE_DEVICE_SEARCH_TX_LEVEL)
            curDev->SetTxLevel(searchResult->txPowerLevel);

         // update the appearance value.
         if(searchResult->informationReported & CSR_BLE_DEVICE_APPEARANCE)
         {
            curDev->SetAppearance(searchResult->appearance);
         }

         // update the connection intervals if reported
         if(searchResult->informationReported & CSR_BLE_DEVICE_SEARCH_CONNECTION_INTERVAL)
         {
            curDev->SetPreferredConnIntervals(searchResult->connIntervalMin,
               searchResult->connIntervalMax);
         }

         // ignore duplicates
         return;
      }
   }

   CString devName = FromUtf8(searchResult->deviceName);

   if(searchResult->deviceName == NULL)
      devName.Empty();

   if(NULL == bleDevice)
   {
      bleDevice = new BleDevice(searchResult->deviceAddress, searchResult->rssi,
         devName, searchResult->flags);
   }
   else
   {
      bool bDevPropertiesChanged = false;

      // Update paired device information with the data from discovery
      if(bleDevice->GetName() != devName)
      {
         /* If the device name has changed, it is likely that it's services
         * have also changed (as it has been found in our testing that
         * a chip is flashed with keyboard first and then with a mouse
         * having the same bluetooth address). So, later, remove the services
         * that are supported by the paired device.
         */
         bleDevice->SetName(devName);
         bDevPropertiesChanged = true;

         // Update the device name in the sc_db.db file.
         CSR_BLE_PAIRED_DEVICE pairedDevice;
         CW2AEX<51> devName(bleDevice->GetName(), CP_UTF8);

         pairedDevice.deviceAddress = bleDevice->GetAddress();
         _mbscpy_s((unsigned char*)pairedDevice.deviceName, sizeof(pairedDevice.deviceName), (unsigned char*)(LPSTR)devName);
         pairedDevice.appearance = bleDevice->GetAppearance();

         CsrBleHostUpdatePairedDeviceList(&pairedDevice);
      }

      bleDevice->SetFlags(searchResult->flags);
      bleDevice->SetRssi(searchResult->rssi);

      if(bDevPropertiesChanged)
      {
         bleDevice->ClearServices();
      }
   }

   // set TX level if reported
   if(searchResult->informationReported & CSR_BLE_DEVICE_SEARCH_TX_LEVEL)
      bleDevice->SetTxLevel(searchResult->txPowerLevel);

   // set connection intervals if reported
   if(searchResult->informationReported & CSR_BLE_DEVICE_SEARCH_CONNECTION_INTERVAL)
   {
      bleDevice->SetPreferredConnIntervals(searchResult->connIntervalMin,
         searchResult->connIntervalMax);
   }
   // set the appearance value
   if(searchResult->informationReported & CSR_BLE_DEVICE_APPEARANCE)
   {
      bleDevice->SetAppearance(searchResult->appearance);
   }

   if(!bleDevice->IsPaired())
   {
      m_Devices.push_back(bleDevice);
   }

   SEARCH_RESULT_T *pDevice = new SEARCH_RESULT_T;
   CSR_BLE_BLUETOOTH_ADDRESS addr = bleDevice->GetAddress();
   memcpy(&pDevice->DevAddr, &addr, sizeof(addr));
   pDevice->DeviceName = bleDevice->GetName();
   // Send the details to the UI
   UiMsg::SendMsgToUI(SEARCH_RESULT, (LPVOID)pDevice);  
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleRemoveBond
*
*  DESCRIPTION
*      Removes the bonding info of the specified device
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::BleRemoveBond(CSR_BLE_BT_ADDRESS *BluetoothAddress)
{
   BOOL result = CsrBleHostDebond(*(CSR_BLE_BLUETOOTH_ADDRESS*)BluetoothAddress);
   // This is heavy-handed, but it won't do any harm.
   mServiceChangeConfigured = FALSE;

   mBatteryLevelConfigured = FALSE;

   return result;
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleDebondResult
*
*  DESCRIPTION
*      Removes the bonding info of the specified device
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleDebondResult(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_DEBOND_RESULT debondRes = (PCSR_BLE_DEBOND_RESULT)lParam;
   
   BOOL bRemoved = FALSE;
   std::vector<BleDevice*>::iterator i;
   CSR_BLE_BLUETOOTH_ADDRESS devAddr = debondRes->deviceAddress;
   BleDevice* bleDevice = NULL;
   CSR_BLE_BLUETOOTH_ADDRESS BluetoothAddress = {0};
   for(i = m_Devices.begin(); i != m_Devices.end(); i++)
   {
      bleDevice = *i;
      BluetoothAddress = bleDevice->GetAddress();

      // find the removed device in the list
       if((BluetoothAddress.lAp == devAddr.lAp) && 
          (BluetoothAddress.uAp == devAddr.uAp) &&
          (BluetoothAddress.nAp == devAddr.nAp))
      {
         if(bleDevice->IsConnected())
         {
            // remove paired flag
            bleDevice->SetPaired(false);
            ToggleConnection((CSR_BLE_BT_ADDRESS*)&BluetoothAddress);
         }
         bRemoved = TRUE;
         break;
      }
   }

    if((i != m_Devices.end()) && (!(*i)->IsConnected()))
    {
        // remove the device from the list
        m_Devices.erase(i);
    }

    // remove the device from the paired devices list as well
    for(i = m_PairedDevices.begin(); i != m_PairedDevices.end(); i++)
    {
        CSR_BLE_BLUETOOTH_ADDRESS BluetoothAddress = (*i)->GetAddress();
        // find the removed device in the list
        if((BluetoothAddress.lAp == devAddr.lAp) && 
           (BluetoothAddress.uAp == devAddr.uAp) &&
           (BluetoothAddress.nAp == devAddr.nAp))
        {
            m_PairedDevices.erase(i);
            break;
        }
    }

    // Notify bonding removed...
    if(bRemoved == TRUE)
    {
       SEARCH_RESULT_T *pDevice = new SEARCH_RESULT_T;
       memcpy(&pDevice->DevAddr, &devAddr, sizeof(devAddr));
       pDevice->DeviceName = bleDevice->GetName();
       UiMsg::SendMsgToUI(DEBONDED_DEVICE, (LPVOID)pDevice);
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleReadCharResult
*
*  DESCRIPTION
*      Callback when a read characteristic result is available from the remote device
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::OnBleReadCharResult(WPARAM wParam, LPARAM lParam)
{
   if(m_CurBleDevice != NULL)
   {
	   PCSR_BLE_CHAR_READ_RESULT readResult = (PCSR_BLE_CHAR_READ_RESULT)lParam;
	   int result = readResult->result;
	   
	   if(result)
	   {
		   DWORD connHandle = readResult->connectHandle;
		   if((result == ATT_RESULT_READ_NOT_PERMITTED) || 
			   (result == ATT_RESULT_INSUFFICIENT_AUTHENTICATION) || 
			   (result == ATT_RESULT_INSUFFICIENT_ENCRYPTION))
		   {
			   if(!encrypting)
			   {
				   // request encryption
				   if(m_EnableMitm && m_CurBleDevice->GetAppearance() == CSR_BLE_APPEARANCE_HID_KEYBOARD)
				   {
					  CsrBleHostSetEncryption(connHandle,CSR_BLE_SECURITY_AUTHENTICATED);
				   }
				   else
				   {
					  CsrBleHostSetEncryption(connHandle,CSR_BLE_SECURITY_UNAUTHENTICATED);
				   }
				   encrypting = true;
			   }
			   // re-add the attribute for reading
			   BleReadCharacteristic(connHandle, readResult->charHandle);
			   return FALSE;
		   }
		   else
		   {
			   if(m_ReadAttrList.empty())
			   {
				   m_ReadingAttributes = false;
			   }
			   else
			   {
				   m_ReadingAttributes = false;
				   ReadAttributes();
			   }
			   return TRUE;	
		   }		   	  
	   }
	   else // (result == 0) //success
	   {
		   m_ReadingAttributes = false;
		   if(!m_ReadAttrList.empty())
		   {
			   ReadAttributes();
		   }
		   return TRUE;
	   }
   }
   return FALSE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleWriteCharResult
*
*  DESCRIPTION
*      Callback when a write characteristic result is available from the remote device
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::OnBleWriteCharResult(WPARAM wParam, LPARAM lParam)
{
   if(m_CurBleDevice != NULL)
   {
   PCSR_BLE_WRITE_RESULT writeResult = (PCSR_BLE_WRITE_RESULT)lParam;
   int result = writeResult->result;

   DWORD connHandle = writeResult->connectHandle;
   WORD charHandle = writeResult->charHandle;

   if(result)
   {
	   if((result == ATT_RESULT_INSUFFICIENT_ENCRYPTION) || 
		   (result == ATT_RESULT_INSUFFICIENT_AUTHENTICATION))
	   {
		   // insufficient encryption (deprecated) or authentication
		   // request encryption
		   if(!encrypting)
		   {
			   // request encryption
			   if(m_EnableMitm && m_CurBleDevice->GetAppearance() == CSR_BLE_APPEARANCE_HID_KEYBOARD)
			   {
				   CsrBleHostSetEncryption(connHandle,CSR_BLE_SECURITY_AUTHENTICATED);
			   }
			   else
			   {
				   CsrBleHostSetEncryption(connHandle,CSR_BLE_SECURITY_UNAUTHENTICATED);
			   }
			   encrypting = true;
		   }
		   BleWriteCharacteristic(&m_CurWriteAttribute);
		   return FALSE;
	   }
	   else
	   {
		   if(m_WritingAttributes)
		   {
			   CharWriteValue value;
			   value.charHandle = Uuids::UUID_INVALID;  
			   
			   while(!m_WriteAttrList.empty())
			   {
				   value = m_WriteAttrList.front();
				   if(value.charHandle != Uuids::UUID_INVALID)
					   break;
				   else
					   m_WriteAttrList.pop();
			   }
			   if((charHandle == value.charCfgHandle) || (charHandle == value.charHandle))
			   {
				   ATLTRACE("Failed to write characteristic %d\n", charHandle);
				   m_WritingAttributes = false;
				   m_WriteAttrList.pop();
				   // continue with attribute writing
				   WriteAttributes();
			   }
		   }
		   return TRUE;
	   }	   
   }
   else
   {
      if(m_WritingAttributes)
      {
         CharWriteValue value;
         value.charHandle = Uuids::UUID_INVALID;

         while(!m_WriteAttrList.empty())
         {
            value = m_WriteAttrList.front();

            if(value.charHandle != Uuids::UUID_INVALID)
               break;
            else
               m_WriteAttrList.pop();
         }

         if((charHandle == value.charCfgHandle) || (charHandle == value.charHandle))
         {
            ATLTRACE("Written characteristic %d\n", charHandle);

            m_WritingAttributes = false;
            m_WriteAttrList.pop();

            // continue with attribute writing
            WriteAttributes();
         }
      }
	  return TRUE;
   }
  }
  return FALSE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetDeviceState
*
*  DESCRIPTION
*      Update and act on the connection state according to the supplied information.
*---------------------------------------------------------------------------*/
void CBLEInterface::SetDeviceState(DeviceState newState)
{
   BOOL stateChange = FALSE;   // Has the state changed this time?

   if(m_DeviceState == newState)
   {
      return;
   }

   switch(newState)
   {
   case DEV_IDLE:
      {
         m_DeviceState = DEV_IDLE;
         // Dump the database contents
         ClearDatabase();
         m_CurBleDevice = NULL;
         stateChange = TRUE;
		 mBatteryLevelConfigured = FALSE;
      }
      break;
   case DEV_CONNECTING:
      {
         if(m_DeviceState == DEV_IDLE)
         {
            //stop the device search
            CsrBleHostStopDeviceSearch();

            // set connection parameters and connect
            if(CsrBleHostSetConnectionParams(CSR_BLE_PARAMETER_IGNORE, 
               CSR_BLE_PARAMETER_IGNORE, 
               MIN_CONN_INTERVAL, 
               MAX_CONN_INTERVAL, 
               SLAVE_LATENCY, 
               LINK_TIMEOUT))
            {
               //set parameters is success
               if(CsrBleHostConnect(m_CurBleDevice->GetAddress()))
               {
                  //connect request is success
                  m_DeviceState = DEV_CONNECTING;
                  stateChange = TRUE;
                  mRetryDbRead = FALSE;

               }
            }
         }
      }
      break;

   case DEV_CONNECTED:
      if(m_DeviceState == DEV_CONNECTING)
      {
         m_DeviceState = DEV_CONNECTED;
         stateChange = TRUE;

		 // request encryption
		 
		 if(m_CurBleDevice->GetAppearance() == CSR_BLE_APPEARANCE_HID_KEYBOARD && 
			 m_CurBleDevice->GetAppearance() == CSR_BLE_APPEARANCE_HID_MOUSE)
		 {
			 if(m_EnableMitm)
			 {
				 CsrBleHostSetEncryption(m_CurBleDevice->GetConnectionHandle(),CSR_BLE_SECURITY_AUTHENTICATED);
			 }
			 else
			 {
				 CsrBleHostSetEncryption(m_CurBleDevice->GetConnectionHandle(),CSR_BLE_SECURITY_UNAUTHENTICATED);
			 }
			 encrypting = true;
		 }

      }
      break;  

   case DEV_DISCONNECTING:
      if(m_DeviceState == DEV_CONNECTED)
      {
         m_DeviceState = DEV_DISCONNECTING;

         // Cancel on-going actions
         CsrBleHostCancel();

         // disconnect
         CsrBleHostDisconnect(m_CurBleDevice->GetConnectionHandle());

         stateChange = TRUE;
      }
      break;

   case DEV_CANCEL_CONNECTING:
      if(m_DeviceState == DEV_CONNECTING)
      {
         CsrBleHostCancelConnect();

		 /* Re-initialise the security level */
		 CsrBleHostChangeSecurityLevel(FALSE);

         m_DeviceState = DEV_CANCEL_CONNECTING;
      }
      break;

   default:
      break;
   }

   if(stateChange)
   {
      CSR_BLE_BT_ADDRESS   *b_addr = NULL;
      // Tell the UI that a state change has occurred.
      if(m_CurBleDevice != NULL)
      {
         CSR_BLE_BLUETOOTH_ADDRESS addr = m_CurBleDevice->GetAddress();
         b_addr = new CSR_BLE_BT_ADDRESS;
         memcpy(b_addr, &addr, sizeof(addr));
      }
      UiMsg::SendMsgToUI(DEVICE_CONNECTION_UPDATE, b_addr);
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      CancelConnection
*
*  DESCRIPTION
*      Cancel an ongoing connection with the remote device.
*---------------------------------------------------------------------------*/

void CBLEInterface::CancelConnection()
{
   if(m_CurBleDevice != NULL)
   {
      CSR_BLE_BLUETOOTH_ADDRESS devAddr = m_CurBleDevice->GetAddress();
      CSR_BLE_BT_ADDRESS addr;
      memcpy(&addr, &devAddr,  sizeof(addr));
      DeviceState state = GetConnectionState(&addr);

      if(state == DEV_CONNECTING)
      {
         //cancel the ongoing connect request
         SetDeviceState(DEV_CANCEL_CONNECTING);
      }
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      GetConnectionState
*
*  DESCRIPTION
*      Retrieve the current state of the device
*---------------------------------------------------------------------------*/
DeviceState CBLEInterface::GetConnectionState(CSR_BLE_BT_ADDRESS *device)
{
   if(m_CurBleDevice != NULL && device != NULL)
   {
      CSR_BLE_BLUETOOTH_ADDRESS devAddr = m_CurBleDevice->GetAddress();
      if((device->lAp == devAddr.lAp) && 
         (device->uAp == devAddr.uAp) &&
         (device->nAp == devAddr.nAp))
      {
         return m_DeviceState;
      }
   }
   return DEV_IDLE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      IsBonded
*
*  DESCRIPTION
*      Searches the current device in bonded device list
*---------------------------------------------------------------------------*/
BOOL CBLEInterface::IsBonded(CSR_BLE_BLUETOOTH_ADDRESS DeviceAddr)
{
   PCSR_BLE_PAIRED_DEVICE pDevices = NULL;
   WORD nDevices = 0;
   BYTE i;

   // Get the paired device list
   if(CsrBleHostGetPairedDeviceList(&nDevices, &pDevices))
   {
      // contains some bonded devices
      for( i = 0; i < nDevices; i++)
      {
         // compare each bonded device with the DeviceAddr
         if((pDevices[i].deviceAddress.lAp == DeviceAddr.lAp) &&
            (pDevices[i].deviceAddress.nAp == DeviceAddr.nAp) &&
            (pDevices[i].deviceAddress.uAp == DeviceAddr.uAp))
         {
            return TRUE; // bonded
         }
      }
   }

   // not bonded
   return FALSE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnBlePanic
*
*  DESCRIPTION
*      Handle a dongle communication failure.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBlePanic(WPARAM wParam, LPARAM lParam)
{
   CString str = _T("BLE stack failure detected.\n\nPlease unplug the BLE host dongle, plug it once again and restart the application.");

   switch(lParam)
   {
   case CSR_BLE_PANIC_TRANSPORT_INIT_FAILURE:
      str = _T("Unable to start up communication with the chip.\n\nPlease check that BLE host dongle is connected.");
      break;
   case CSR_BLE_PANIC_TRANSPORT_FAILURE:
      str = _T("Lost communication with the chip.");
      break;        
   }

   if(lParam != CSR_BLE_PANIC_NO_PANIC)
   {
      // close the main window
      CString title;
      title.LoadString(IDS_APP_TITLE);
      AfxGetMainWnd()->MessageBox((LPCTSTR)str, title);
      AfxGetMainWnd()->SendMessage(WM_CLOSE);
   }

   // Send Hardware Error message to the UI
   UiMsg::SendMsgToUI(HW_ERR, NULL);
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnBleReady
*
*  DESCRIPTION
*      Act on the radio dongle having completed initialisation.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleReady(WPARAM wParam, LPARAM lParam)
{
   if(lParam == TRUE)
   {
      // Set the current state to idle
      SetDeviceState(DEV_IDLE);
      SetRadioState(APP_IDLE);

      // Update the UI with a list of paired devices
      
      PCSR_BLE_PAIRED_DEVICE pDevices;
      WORD nDevices;
      if(CsrBleHostGetPairedDeviceList(&nDevices, &pDevices))
      {
         // paired devices found - clear the default items
         m_PairedDevices.clear();
         // add paired devices
         for(int i = 0; i < nDevices; i++)
         {
            CA2W devNameUtf8(pDevices[i].deviceName, CP_UTF8);
            CString devName(devNameUtf8);

            BleDevice* bleDevice = new BleDevice(pDevices[i].deviceAddress,
               BleDevice::InvalidRssi, devName, 0, true);
            m_PairedDevices.push_back(bleDevice);
         }

         // copy paired devices to the device list
         m_Devices = m_PairedDevices;

         CsrBleHostFreePairedDeviceList(pDevices);
      }

      SendAllPairedDevicesToUI();
      UiMsg::SendMsgToUI(HW_READY, NULL);
   }
   else
   {
      // Send Hardware Error message to the UI
      UiMsg::SendMsgToUI(HW_ERR, NULL);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleSearchStopped
*
*  DESCRIPTION
*      Called when the device search has been stopped.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleSearchStopped(WPARAM wParam, LPARAM lParam)
{
   m_CurRadioState = APP_IDLE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnBleLEScanStatus
*
*  DESCRIPTION
*      Called when the device search has been stopped.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleLEScanStatus(WPARAM wParam, LPARAM lParam)
{
   if(m_CurRadioState == APP_SCANNING)
   {
      SetRadioState(APP_IDLE);
   }
   else
   {
      SetRadioState(APP_SCANNING);
   }
}



/*----------------------------------------------------------------------------*
*  NAME
*      OnBleConnect
*
*  DESCRIPTION
*      Called when a connection attempt completes (either success or failure)
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleConnect(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_CONNECT_RESULT connectResult = (PCSR_BLE_CONNECT_RESULT)lParam;

   if(!connectResult->result)
   {
      // successfully connected
      m_CurBleDevice->SetConnectionHandle(connectResult->connectHandle);
      CString logString;
      logString.LoadString(IDS_APP_CONNECTED);
      UiMsg::LogMessage(logString, STATUS_TYPE);

      SetDeviceState(DEV_DISCOVERING_SVCS);
      logString.LoadString(IDS_APP_DB_READ);
      UiMsg::LogMessage(logString, STATUS_TYPE);
      CsrBleClientDiscoverDatabase(m_CurBleDevice->GetConnectionHandle());
   }
   else
   {
      // failed to connect
      SetDeviceState(DEV_IDLE);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleJustWorksRequest
*
*  DESCRIPTION
*      Handle a "just works" pairing request from the remote device.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleJustWorksRequest(WPARAM wParam, LPARAM lParam)
{
   // pairing request
   CString pString;
   pString.LoadString(IDS_APP_PAIRING_RCVD);
   UiMsg::LogMessage(pString, INFO_TYPE);
   PCSR_BLE_JUSTWORKS_REQUEST jwReq = (PCSR_BLE_JUSTWORKS_REQUEST)lParam;

   CsrBleHostJustWorksResult(jwReq->deviceAddress, TRUE, TRUE);
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleHostSecurityResult
*
*  DESCRIPTION
*      Handle the bonding request-response from the remote device.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleHostSecurityResult(WPARAM wParam, LPARAM lParam)
{
   if(m_CurBleDevice != NULL)
   {
	   encrypting = false;
	   // security result
	   PCSR_BLE_SECURITY_RESULT secRes = (PCSR_BLE_SECURITY_RESULT)lParam;
	   CString logString;
	   
	   if(secRes->result != CSR_BLE_SECURITY_BONDING_ESTABLISHED)
	   {
		   if(!bMitmInProgress)
		   {
			   logString.LoadString(IDS_APP_PAIRING_FAILURE);
			   UiMsg::LogMessage(logString, ERROR_TYPE);
			   SetDeviceState(DEV_DISCONNECTING);
		   }
	   }
	   else
	   {
		   m_bEncrypted = TRUE;
		   // Send logging message
		   logString.LoadString(IDS_APP_BONDED);
		   typedAddressToString(&(secRes->deviceAddress), &logString);
		   UiMsg::LogMessage(logString, INFO_TYPE);
		   
		   // If this message was received while we were trying to read the
		   // database, cancel the current read and set a flag to indicate
		   // that it should be re-tryed.
		   if(m_DeviceState == DEV_DISCOVERING_SVCS)
		   {
			   mRetryDbRead = TRUE;
		   }
		   
		   if(!m_CurBleDevice->IsPaired())
		   {
			   m_CurBleDevice->SetPaired(true);
			   m_PairedDevices.push_back(m_CurBleDevice);
			   SEARCH_RESULT_T *pDevice = new SEARCH_RESULT_T;
			   CSR_BLE_BLUETOOTH_ADDRESS addr = m_CurBleDevice->GetAddress();
			   memcpy(&pDevice->DevAddr, &addr, sizeof(addr));
			   pDevice->DeviceName = m_CurBleDevice->GetName();
			   // Send the details to the UI
			   UiMsg::SendMsgToUI(BONDED_DEVICE, (LPVOID)pDevice);
		   }
	   }
   }
}
/*----------------------------------------------------------------------------*
*  NAME
*      OnBleSetEncryptionResult
*
*  DESCRIPTION
*      Handle the encryption attempt result from the remote device.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleSetEncryptionResult(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_SET_ENCRYPTION_RESULT secRes = (PCSR_BLE_SET_ENCRYPTION_RESULT)lParam;
   CString logString;
   if(!secRes->result)
   {
      m_bEncrypted = TRUE;

      // retry writing attributes
      WriteAttributes();

      // retry reading attributes
      ReadAttributes();
   }
   else
   {
      //send UI message
      UiMsg::SendMsgToUI(SEC_ERR, NULL);
   }
}
/*----------------------------------------------------------------------------*
*  NAME
*      OnBleDatabaseDiscoveryResult
*
*  DESCRIPTION
*      Called when the GATT database read has completed.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleDatabaseDiscoveryResult(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_DATABASE_DISCOVERY_RESULT dbRes = (PCSR_BLE_DATABASE_DISCOVERY_RESULT)lParam;

   if(!dbRes->result)
   {
      CString logString = L"Discovery successful";
      UiMsg::LogMessage(logString, STATUS_TYPE);

      SetDeviceState(DEV_CONNECTED);
      //Query remote device database success
      //parse the remote gatt data base
      BleParseDatabase(dbRes);

      // Do app specific initialization here...
      AppInit();
   }
   else
   {
      // Re-discover... or disconnect
      if(DEV_DISCOVERING_SVCS == m_DeviceState && mRetryDbRead == TRUE)
      {
         mRetryDbRead = FALSE;
         CString logString;
         logString.LoadString(IDS_APP_DB_READ);
         UiMsg::LogMessage(logString, STATUS_TYPE);
         CsrBleClientDiscoverDatabase(m_CurBleDevice->GetConnectionHandle());
      }
   }
}

WORD CBLEInterface::GetBatteryLevelHandle()
{
   CSR_BLE_UUID battery_svc = {0};
   Init16BitUUID(battery_svc, Uuids::UUID_BATTERY_SERVICE);
   CSR_BLE_UUID battery_char = {0};
   Init16BitUUID(battery_char, Uuids::UUID_BATTERY_LEVEL_CHAR);
   
   WORD handle = m_CurBleDevice->FindCharHandle(battery_svc, battery_char);
   return handle;
}

WORD CBLEInterface::GetBatteryLevelCfgHandle()
{
   CSR_BLE_UUID battery_svc = {0};
   Init16BitUUID(battery_svc, Uuids::UUID_BATTERY_SERVICE);
   CSR_BLE_UUID battery_char = {0};
   Init16BitUUID(battery_char, Uuids::UUID_BATTERY_LEVEL_CHAR);
   
   WORD handle = m_CurBleDevice->FindCharHandle(battery_svc, battery_char,true);
   return handle;
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleConnectionUpdateRequest
*
*  DESCRIPTION
*      Called when the remote device wants to update the connection parameters.
*---------------------------------------------------------------------------*/
void CBLEInterface::OnBleConnectionUpdateRequest(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_CONNECTION_UPDATE_REQUEST requestedParams = 
      (PCSR_BLE_CONNECTION_UPDATE_REQUEST)lParam;

   /* Always accept the requested connection parameters */
   CsrBleHostAcceptConnUpdate(m_CurBleDevice->GetConnectionHandle(), 
      requestedParams->id, 
      TRUE);
}

/*----------------------------------------------------------------------------*
*  NAME
*      SendAllPairedDevicesToUI
*
*  DESCRIPTION
*      This method sends all bonded devices to the UI module
*---------------------------------------------------------------------------*/
void CBLEInterface::SendAllPairedDevicesToUI(void)
{
   // add paired devices
   int num_elements = m_PairedDevices.size();
   for(int i = 0; i < num_elements; i++)
   {
      BleDevice* bleDevice = m_PairedDevices[i];
      SEARCH_RESULT_T *pDevice = new SEARCH_RESULT_T;
      CSR_BLE_BLUETOOTH_ADDRESS addr = bleDevice->GetAddress();
      memcpy(&pDevice->DevAddr, &addr, sizeof(addr));
      pDevice->DeviceName = bleDevice->GetName();
      // Send the details to the UI
      UiMsg::SendMsgToUI(BONDED_DEVICE, (LPVOID)pDevice);  
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleParseDatabase
*
*  DESCRIPTION
*      This method parses the gatt remote database.
*      It looks for bootloader services and stores all the service characteristics.
*---------------------------------------------------------------------------*/
WORD CBLEInterface::BleParseDatabase(PCSR_BLE_DATABASE_DISCOVERY_RESULT dbRes)
{
   if(m_CurBleDevice != NULL)
   {
      BleDevice* bleDevice = m_CurBleDevice;
      if(DEV_DISCOVERING_SVCS == m_DeviceState)
      {
         m_DeviceState = DEV_CONNECTED;
      }

      // add services
      bleDevice->AddServices(dbRes->nServices, dbRes->services);
   }
   return 1;
}

/*----------------------------------------------------------------------------*
*  NAME
*      ClearDatabase
*
*  DESCRIPTION
*      This method deletes the stored GATT database
*---------------------------------------------------------------------------*/
void CBLEInterface::ClearDatabase()
{
   if(m_CurBleDevice != NULL)
   {
      m_CurBleDevice->ClearServices();
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetMITMFlag
*
*  DESCRIPTION
*      This method enables/disables the MITM
*---------------------------------------------------------------------------*/
void CBLEInterface::SetMITMFlag(BOOL value)
{
	m_EnableMitm = value;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetMITMStatus
*
*  DESCRIPTION
*      This method sets the status of MITM procedure
*---------------------------------------------------------------------------*/
void CBLEInterface::SetMITMStatus(BOOL value)
{
	bMitmInProgress = value;
}