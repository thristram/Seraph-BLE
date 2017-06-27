/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      OTAInteface.cpp
 *
 *  DESCRIPTION
 *      This class handles communications with the remote device using the
 *      uEnergy DLL.
 *
 ******************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "OTAInterface.h"
#include "BleRadio.h"
#include "HelperFunc.h"
#include "Resource.h"
#include "uuids.h"
#include "uiMsg.h"
#include "uEnergyImage.h"

/*============================================================================*
 *  Standard Header Files
 *============================================================================*/
using namespace std;
#include <string>


/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
*  NAME
*      ~CBLEInterface
*
*  DESCRIPTION
*      CBLEInterface destructor
*---------------------------------------------------------------------------*/
COTAInterface::~COTAInterface(void)
{
   Aes::OnShutdown();
}

/*----------------------------------------------------------------------------*
*  NAME
*      BleInit
*
*  DESCRIPTION
*      Initialise the underlying dongle
*---------------------------------------------------------------------------*/
RADIO_FAILURES COTAInterface::BleInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging)
{
   mExpectChallenge = FALSE;
   mDeviceMode = NOT_SUPPORTING_OTA;
   mAppId = 0;
   mServiceChangeConfigured = FALSE;
   mBatteryLevelConfigured = FALSE;
   m_BootVersion = 0;

   char *xmlFilename = new char[MAX_PATH];
   UeCsResultEnum status = UE_CS_RES_OK;

   strcpy_s(xmlFilename,MAX_PATH, "..\\tools\\bin\\cskey_db.xml");
   WCHAR xmlFile[] =  L"..\\tools\\bin\\cskey_db.xml";  
   CFileStatus fStatus;
   if(CFile::GetStatus(xmlFile,fStatus) == FALSE)
   {
	   strcpy_s(xmlFilename,MAX_PATH, "cskey_db.xml");
   }
   status = m_KeysInfo.LoadDatabase(xmlFilename);
   delete [] xmlFilename;

   if(UE_CS_RES_OK != status)
      return RADIO_FAIL_CLIENT_INIT;

   return CBLEInterface::BleInit(hWnd, bleClientHandle, bLogging);
}


/*----------------------------------------------------------------------------*
*  NAME
*      CheckDeviceOTAMode
*
*  DESCRIPTION
*      Check if the device is OTA'able
*---------------------------------------------------------------------------*/
WORD COTAInterface::CheckDeviceOTAMode()
{
   WORD mode = NOT_SUPPORTING_OTA;

   CSR_BLE_UUID boot_loader_svc = {0};
   Init128BitUUID(boot_loader_svc, (LPBYTE)Uuids::OtaUpgradeBootloaderServiceUuid);
   BleService bootService;
   if(m_CurBleDevice->GetService(boot_loader_svc, bootService) == TRUE)
   {
      mode = OTA_BOOTLOADER_MODE;
      memcpy(&m_OtaServiceUuid, &boot_loader_svc, sizeof(m_OtaServiceUuid));
      m_BleOtaService = bootService;
   }
   else
   {
      CSR_BLE_UUID ota_app_svc = {0};
      Init128BitUUID(ota_app_svc, (LPBYTE)Uuids::OtaUpgradeApplicationServiceUuid);
      BleService otaAppService;
      if(m_CurBleDevice->GetService(ota_app_svc, otaAppService) == TRUE)
      {
         mode = OTA_APPLICATION_MODE;
         memcpy(&m_OtaServiceUuid, &ota_app_svc, sizeof(m_OtaServiceUuid));
         m_BleOtaService = otaAppService;
      }
   }
   return mode;
}

/*----------------------------------------------------------------------------*
*  NAME
*      getReadCsKeyHandle
*
*  DESCRIPTION
*      Get the CS Key Read handle 
*---------------------------------------------------------------------------*/
WORD COTAInterface::getReadCsKeyHandle()
{
   CSR_BLE_UUID _uuid; 
   Init128BitUUID(_uuid, (LPBYTE)Uuids::OtaReadCSKeyCharUuid);
   WORD handle = m_CurBleDevice->FindCharHandle(m_OtaServiceUuid, _uuid);
   return handle;
}

/*----------------------------------------------------------------------------*
*  NAME
*      getReadCsBlockHandle
*
*  DESCRIPTION
*      Get the CS Key Block handle 
*---------------------------------------------------------------------------*/
WORD COTAInterface::getReadCsBlockHandle()
{
   CSR_BLE_UUID _uuid; 
   Init128BitUUID(_uuid, (LPBYTE)Uuids::OtaReadCSBlockCharUuid);
   WORD handle = m_CurBleDevice->FindCharHandle(m_OtaServiceUuid, _uuid);
   return handle;
}

/*----------------------------------------------------------------------------*
*  NAME
*      getOtaVersionHandle
*
*  DESCRIPTION
*      Get the OTA Version handle 
*---------------------------------------------------------------------------*/
WORD COTAInterface::getOtaVersionHandle()
{
   CSR_BLE_UUID _uuid; 
   Init128BitUUID(_uuid, (LPBYTE)Uuids::OtaVersionCharUuid);
   WORD handle = m_CurBleDevice->FindCharHandle(m_OtaServiceUuid, _uuid);
   return handle;
}

/*----------------------------------------------------------------------------*
*  NAME
*      getTransCtrlHandle
*
*  DESCRIPTION
*      Get the transfer control handle 
*---------------------------------------------------------------------------*/
WORD COTAInterface::getTransCtrlHandle()
{
   CSR_BLE_UUID _uuid; 
   Init128BitUUID(_uuid, (LPBYTE)Uuids::OtaTransCtrlCharUuid);
   WORD handle = m_CurBleDevice->FindCharHandle(m_OtaServiceUuid, _uuid);
   return handle;
}
/*----------------------------------------------------------------------------*
*  NAME
*      getCurAppHandle
*
*  DESCRIPTION
*      Get the current app handle 
*---------------------------------------------------------------------------*/
WORD COTAInterface::getCurAppHandle()
{
   CSR_BLE_UUID _uuid; 
   Init128BitUUID(_uuid, (LPBYTE)Uuids::OtaCurAppCharUuid);
   WORD handle = m_CurBleDevice->FindCharHandle(m_OtaServiceUuid, _uuid);
   return handle;
}

/*----------------------------------------------------------------------------*
*  NAME
*      getDataTransHandle
*
*  DESCRIPTION
*      Get the Data Transfer handle 
*---------------------------------------------------------------------------*/
WORD COTAInterface::getDataTransHandle()
{
   CSR_BLE_UUID _uuid; 
   Init128BitUUID(_uuid, (LPBYTE)Uuids::OtaDataTransCharUuid);
   WORD handle = m_CurBleDevice->FindCharHandle(m_OtaServiceUuid, _uuid);
   return handle;
}

/*----------------------------------------------------------------------------*
*  NAME
*      AppInit
*
*  DESCRIPTION
*      Initailize the application mode
*---------------------------------------------------------------------------*/
void COTAInterface::AppInit()
{
   CString logString;

   if(CheckDeviceOTAMode()== OTA_APPLICATION_MODE)
   {
	   if(GetBatteryLevelHandle()!= HANDLE_UNKNOWN)
	   {
		   //Check battery level to see if it is okay to proceed with OTA
		   logString.LoadString(IDS_OTA_CHECK_BATT_LEVEL);
		   UiMsg::LogMessage(logString, STATUS_TYPE);
		   AddAttributeForReading(GetBatteryLevelHandle());
		   return;
	   }
   }

   /* Start OTA */
   StartOTA();
}

/*----------------------------------------------------------------------------*
*  NAME
*      ConfigureChallengeResponse
*
*  DESCRIPTION
*      Configure challenge to be enabled or disabled
*---------------------------------------------------------------------------*/
void COTAInterface::ConfigureChallengeResponse(BOOL enable)
{
   mExpectChallenge = enable;
}


/*----------------------------------------------------------------------------*
*  NAME
*      OtaAbortUpdateRsp
*
*  DESCRIPTION
*      This method handles the "abort download" response
*---------------------------------------------------------------------------*/
void COTAInterface::OtaAbortUpdateRsp(int result)
{
   PROGRESS_STATUS_T *progressMessage = new PROGRESS_STATUS_T;

   progressMessage->progress = 0;
   progressMessage->status = DOWNLOAD_CANCELLED;

   //inform ui
   UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);

   //reset OTA update state
   mImageFileHandler.ResetTransfer();
}   

/*----------------------------------------------------------------------------*
*  NAME
*      OtaSetTransferComplete
*
*  DESCRIPTION
*      This method sends a "transfer completed" message to the remote device
*---------------------------------------------------------------------------*/
void COTAInterface::OtaSetTransferComplete()
{
   //transfer complete
   BYTE data = CMD_SET_TRANSFER_COMPLETED;
   PROGRESS_STATUS_T *progressMessage = new PROGRESS_STATUS_T;

   AddAttributeForWriting(getTransCtrlHandle(), 0x1, (LPBYTE)&data);     

   //reset the packet count
   mImageFileHandler.ResetTransfer();

   //update the state
   stateSetOta(STATE_OTA_RESET_TRANSFER_CTRL);

   //inform ui
   progressMessage->progress = 100;
   progressMessage->status = DOWNLOAD_COMPLETE;
   UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
}

/*----------------------------------------------------------------------------*
*  NAME
*      OtaSetTransferControlReq
*
*  DESCRIPTION
*      This method sends a "starting transfer now" message to the remote device
*---------------------------------------------------------------------------*/
void COTAInterface::OtaSetTransferControlReq(void)
{ 
	// Move to faster parameter while transfering
	CsrBleHostUpdateConnectionParams(m_CurBleDevice->GetAddress(), FAST_MIN_CONN_INTERVAL, FAST_MAX_CONN_INTERVAL,
		FAST_SLAVE_LATENCY, FAST_LINK_TIMEOUT, CSR_BLE_PARAMETER_IGNORE, CSR_BLE_PARAMETER_IGNORE);
	
	BYTE data = CMD_SET_TRANSFER_CONTROL; 
    AddAttributeForWriting(getTransCtrlHandle(), 0x1, LPBYTE(&data)); 

    stateSetOta(STATE_OTA_SET_TRANSFER_CTRL);
}

/*----------------------------------------------------------------------------*
*  NAME
*      OtaSetTransferControlRsp
*
*  DESCRIPTION
*      This method handles a "set transfer control" response 
*---------------------------------------------------------------------------*/
void COTAInterface::OtaSetTransferControlRsp(int result)
{
   PROGRESS_STATUS_T *progressMessage;

   if(result == 0x0)   /* success */
   {
      // update the state
      stateSetOta(STATE_OTA_DATA_TRANSFER);

      // Try to send the next chunk of the image file
      if(SendNextImageChunk() == false)
      {
         // Complete Image Read
         mImageFileHandler.ResetTransfer();

         // Tell the remote device that all data is now sent,
         // probably triggering a device reset.
         OtaSetTransferComplete();
      }
   }
   else
   {
      // Set Transfer Control failed
      progressMessage = new PROGRESS_STATUS_T;

      progressMessage->progress = 0;
      progressMessage->status = ERR_IMAGE_TRANSFER_FAILED;

      // send message to UI
      UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
   }
}    


/*----------------------------------------------------------------------------*
*  NAME
*      ValidateImageFile
*
*  DESCRIPTION
*      Verifies the validity of the specified image file. Returns false if the 
*      the check fails.
*---------------------------------------------------------------------------*/
BOOL COTAInterface::ValidateImageFile()
{
   OpenImageFileForUpdate();
   BOOL retVal = mImageFileHandler.ValidateImageFile();
   CloseImageFile();

   return retVal;
}

/*----------------------------------------------------------------------------*
*  NAME
*      InitializeOta
*
*  DESCRIPTION
*      Initialisation is a sequence of transactions. Given the current transaction,
*      determine what the next one should be (and do it).
*---------------------------------------------------------------------------*/
void COTAInterface::InitializeOta()
{
   BYTE data;
   CString logString;

   // Check that we are still connected
   if(m_DeviceState == DEV_CONNECTED)
   {
      switch(StateGetOta())
      {
      case STATE_OTA_IDLE:
         {
            // If this device uses challenge-response,
            // rise to the challenge now
            if(mDeviceMode == OTA_BOOTLOADER_MODE)
            {
               logString.LoadString(IDS_APP_BL_CONNECTED);
               UiMsg::LogMessage(logString, INFO_TYPE);
               if(mExpectChallenge)
               {
                  logString.LoadString(IDS_OTA_GET_CLVAL);
                  UiMsg::LogMessage(logString, STATUS_TYPE);

                  AddAttributeForReading(getDataTransHandle());
                  stateSetOta(STATE_OTA_INIT_READ_CHALLENGE);
               }
               else
               {
                  logString.LoadString(IDS_OTA_GET_BLVER);
                  UiMsg::LogMessage(logString, STATUS_TYPE);

                  AddAttributeForReading(getOtaVersionHandle());
                  stateSetOta(STATE_OTA_INIT_READ_VERSION);
               }
            }
            else
            {
               if(HANDLE_UNKNOWN != getOtaVersionHandle())
               {
                  logString.LoadString(IDS_OTA_GET_BLVER);
                  UiMsg::LogMessage(logString, STATUS_TYPE);

                  AddAttributeForReading(getOtaVersionHandle());
                  stateSetOta(STATE_OTA_INIT_READ_VERSION);
               }
               else
               {
                  logString.LoadString(IDS_APP_CONNECTED);
                  UiMsg::LogMessage(logString, INFO_TYPE);

                  // Configure client descriptors for notification
                  if(GATT_OP_WRITE_PENDING == WriteGattClientCharDescValue())
                  {
                     // Record the new state
                     stateSetOta(STATE_OTA_INIT_CONFIGURING_CCD);
                  }
                  else
                  {
                     // If we are here then this is probably an error condition
                     logString.LoadString(IDS_OTA_CFG_NOT_ERR);
                     UiMsg::LogMessage(logString, ERROR_TYPE);
                  }
               }
            }
         }
         break;

      case STATE_OTA_INIT_READ_VERSION:
         {
            // Configure client descriptors for notification
            if(GATT_OP_WRITE_PENDING == WriteGattClientCharDescValue())
            {
               // Record the new state
               stateSetOta(STATE_OTA_INIT_CONFIGURING_CCD);
            }
            else
            {
               // If we are here then this is probably an error condition
               logString.LoadString(IDS_OTA_READ_BLVER_ERR);
               UiMsg::LogMessage(logString, ERROR_TYPE);
            }
         }
         break;

         // Requesting notifications on all relevant characteristics
      case STATE_OTA_INIT_CONFIGURING_CCD:      
         // Now request the current active application
         {
            stateSetOta(STATE_OTA_INIT_READ_CURRENT_APP);

            logString.LoadString(IDS_OTA_GET_CURAPP);
            UiMsg::LogMessage(logString, STATUS_TYPE);

            AddAttributeForReading(getCurAppHandle());
         }
         break;

         // Reading the active application
      case STATE_OTA_INIT_READ_CURRENT_APP:     
         {
            // If the connected device is running the device application (is not
            // in boot-loader mode), then request the current application id.
            if(OTA_BOOTLOADER_MODE == mDeviceMode)
            {
               // Now read the set the current app here
               if(mAppId == 1)
               {
                  mAppId = 2;
               }
               else
               {
                  mAppId = 1;
               }

               BYTE app = mAppId;
               AddAttributeForWriting(getCurAppHandle(), 0x1, LPBYTE(&app));
               stateSetOta(STATE_OTA_INIT_SET_CURRENT_APP);

               logString.Format(IDS_OTA_SET_CURAPP, mAppId);
               UiMsg::LogMessage(logString, STATUS_TYPE);
            }
            else    // OTA_APPLICATION_MODE
            {
               // Depending on the supported characteristic request the read...
               if(getReadCsKeyHandle() != HANDLE_UNKNOWN)
               {
                  data = KEY_BT_ADDRESS;
                  AddAttributeForWriting(getReadCsKeyHandle(), 0x1, (LPBYTE)&data);
                  stateSetOta(STATE_OTA_INIT_READ_BT_ADDRESS);

                  logString.LoadString(IDS_OTA_GET_BDADDR);
                  UiMsg::LogMessage(logString, INFO_TYPE);
               }
               else if(getReadCsBlockHandle() != HANDLE_UNKNOWN)
               {
                  // offset 0 Length 2;
                  BYTE data_block[4] = {0x00, 0x00, 0x02, 0x00};

                  AddAttributeForWriting(getReadCsBlockHandle(), sizeof(data_block), (LPBYTE)&data_block);
                  stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK);

                  logString.LoadString(IDS_OTA_READ_CSBLOCK);
                  UiMsg::LogMessage(logString, INFO_TYPE);
               }
            }
         }
         break;

      case STATE_OTA_INIT_READ_BT_ADDRESS:     
         {
            // Now read the crystal trim
            data = KEY_XTAL_TRIM;

            stateSetOta(STATE_OTA_INIT_READ_XTAL_TRIM);
            AddAttributeForWriting(getReadCsKeyHandle(), 0x1, (LPBYTE)&data);

            logString.LoadString(IDS_OTA_GET_XTALTRIM);
            UiMsg::LogMessage(logString, INFO_TYPE);
         }
         break;

      case STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR:
         {
            uint16 index = 0;
            m_KeysInfo.KeyIdFromName("crystal_ftrim", index);
            CCsKeysInfo::CsKeyStruct keyInfo;
            m_KeysInfo.KeyDetailsFromId(index, keyInfo);

            WORD len_in_octets = keyInfo.length * 2;
            BYTE data_block[4] = {keyInfo.offset & 0xFF, keyInfo.offset >> 8, len_in_octets & 0xFF, len_in_octets>> 8};
            AddAttributeForWriting(getReadCsBlockHandle(), sizeof(data_block), (LPBYTE)&data_block);
            stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM);

            logString.LoadString(IDS_OTA_GET_XTALTRIM);
            UiMsg::LogMessage(logString, INFO_TYPE);
         }
         break;

      case STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM:
         {
            uint16 index = 0;
            m_KeysInfo.KeyIdFromName("identity_root", index);
            CCsKeysInfo::CsKeyStruct keyInfo;
            m_KeysInfo.KeyDetailsFromId(index, keyInfo);

            WORD len_in_octets = keyInfo.length * 2;
            BYTE data_block[4] = {keyInfo.offset & 0xFF, keyInfo.offset >> 8, len_in_octets & 0xFF, len_in_octets>> 8};
            AddAttributeForWriting(getReadCsBlockHandle(), sizeof(data_block), (LPBYTE)&data_block);
            stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_IDROOT);

            logString.LoadString(IDS_OTA_GET_IDROOT);
            UiMsg::LogMessage(logString, INFO_TYPE);
         }
         break;

      case STATE_OTA_INIT_READ_CS_BLOCK_IDROOT:
         {
            uint16 index = 0;
            m_KeysInfo.KeyIdFromName("encryption_root", index);
            CCsKeysInfo::CsKeyStruct keyInfo;
            m_KeysInfo.KeyDetailsFromId(index, keyInfo);

            WORD len_in_octets = keyInfo.length * 2;
            BYTE data_block[4] = {keyInfo.offset & 0xFF, keyInfo.offset >> 8, len_in_octets & 0xFF, len_in_octets>> 8};
            AddAttributeForWriting(getReadCsBlockHandle(), sizeof(data_block), (LPBYTE)&data_block);
            stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT);

            logString.LoadString(IDS_OTA_GET_ENCRROOT);
            UiMsg::LogMessage(logString, INFO_TYPE);
         }
         break;

      case STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT:
         {
            if(mImageFileHandler.MergeKeys())
            {
               logString.LoadString(IDS_OTA_SWITCH_BL);
               UiMsg::LogMessage(logString, STATUS_TYPE);

               BYTE app = CSR_OTA_BOOT_OTA;
               AddAttributeForWriting(getCurAppHandle(), 0x1, LPBYTE(&app));
               stateSetOta(STATE_OTA_INIT_SET_CURRENT_APP);
            }
            else
            {
               UiMsg::SendMsgToUI(OTA_MERGE_CSKEY_FAILED, NULL);
               SetDeviceState(DEV_DISCONNECTING);
            }
         }
         break;

      // Reading the device crystal trim
      case STATE_OTA_INIT_READ_XTAL_TRIM:       
         // Pass the app id to the remote device
         {
            data = KEY_IDTY_ROOT;

            stateSetOta(STATE_OTA_INIT_READ_IDENTITY_ROOT);
            AddAttributeForWriting(getReadCsKeyHandle(), 0x1, (LPBYTE)&data);

            logString.LoadString(IDS_OTA_GET_IDROOT);
            UiMsg::LogMessage(logString, INFO_TYPE);
         }
         break;

      // Reading the device identity root
      case STATE_OTA_INIT_READ_IDENTITY_ROOT:
           {
            data = KEY_ENCR_ROOT;

            stateSetOta(STATE_OTA_INIT_READ_ENCRYPTION_ROOT);
            AddAttributeForWriting(getReadCsKeyHandle(), 0x1, (LPBYTE)&data);

            logString.LoadString(IDS_OTA_GET_ENCRROOT);
            UiMsg::LogMessage(logString, INFO_TYPE);
         }
         break;

      // Reading the device encryption root
      case STATE_OTA_INIT_READ_ENCRYPTION_ROOT:
         {
            if(OTA_APPLICATION_MODE == mDeviceMode)
            {
               logString.LoadString(IDS_OTA_SWITCH_BL);
               UiMsg::LogMessage(logString, STATUS_TYPE);

               BYTE app = CSR_OTA_BOOT_OTA;
               AddAttributeForWriting(getCurAppHandle(), 0x1, LPBYTE(&app));
               stateSetOta(STATE_OTA_INIT_SET_CURRENT_APP);
            }
            else
            {
               // version greater than 3???
               // Start merging the keys that were read formo the device.
                if(mImageFileHandler.MergeKeys())
                {
                  stateSetOta(STATE_OTA_IDLE);
                  UiMsg::SendMsgToUI(OTA_READY, NULL);
                }
                else
                {
                   UiMsg::SendMsgToUI(OTA_MERGE_CSKEY_FAILED, NULL);
                   SetDeviceState(DEV_DISCONNECTING);
                }
            }
         }
         break;

      // Read the challenge value from the connected device
      case STATE_OTA_INIT_READ_CHALLENGE:       
         {
            // Now read the specification version
            AddAttributeForReading(getOtaVersionHandle());
            stateSetOta(STATE_OTA_INIT_READ_VERSION);

            logString.LoadString(IDS_OTA_GET_BLVER);
            UiMsg::LogMessage(logString, STATUS_TYPE);
         }
         break;
      case STATE_OTA_INIT_READ_CS_BLOCK:
         {
            if(OTA_APPLICATION_MODE == mDeviceMode)
            {
               uint16 index = 0;
               m_KeysInfo.KeyIdFromName("bdaddr", index);
               CCsKeysInfo::CsKeyStruct keyInfo;
               m_KeysInfo.KeyDetailsFromId(index, keyInfo);

               WORD len_in_octets = keyInfo.length * 2;
               BYTE data_block[4] = {keyInfo.offset & 0xFF, keyInfo.offset >> 8, len_in_octets & 0xFF, len_in_octets>> 8};

               AddAttributeForWriting(getReadCsBlockHandle(), sizeof(data_block), (LPBYTE)&data_block);
               stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR);

               logString.LoadString(IDS_OTA_GET_BDADDR);
               UiMsg::LogMessage(logString, INFO_TYPE);
            }
         }
         break;

      case STATE_OTA_INIT_SET_CURRENT_APP:
         // Wait for user input
         {
            if(OTA_APPLICATION_MODE == mDeviceMode)
            {
               stateSetOta(STATE_OTA_SET_MODE);
               BYTE data = CSR_OTA_BOOT_OTA;
               AddAttributeForWriting(getReadCsKeyHandle(), 0x1, (LPBYTE)&data);

               logString.LoadString(IDS_OTA_SWITCH_BL);
               UiMsg::LogMessage(logString, INFO_TYPE);
               UiMsg::SendMsgToUI(OTA_READY, NULL);
            }
            else // In boot loader mode
            {
               if(m_BootVersion > 3)
               {
                  data = KEY_BT_ADDRESS;
                  AddAttributeForWriting(getReadCsKeyHandle(), 0x1, (LPBYTE)&data);
                  stateSetOta(STATE_OTA_INIT_READ_BT_ADDRESS);

                  logString.LoadString(IDS_OTA_GET_BDADDR);
                  UiMsg::LogMessage(logString, INFO_TYPE);
               }
               else
               {
                  if(mImageFileHandler.MergeKeys())
                  {
                      // version greater than 3???
                     stateSetOta(STATE_OTA_IDLE);
                     UiMsg::SendMsgToUI(OTA_READY, NULL);
                  }
                  else
                  {
                     UiMsg::SendMsgToUI(OTA_MERGE_CSKEY_FAILED, NULL);
                     SetDeviceState(DEV_DISCONNECTING);
                  }
               }
            }
         }
         break;

      default:
         break;
      }
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnOtaImagePacketTransferResult
*
*  DESCRIPTION
*      Called when the transfer of the latest chunk of image file has completed  
*      or failed.
*---------------------------------------------------------------------------*/
void COTAInterface::OnOtaImagePacketTransferResult(WORD result)
{
   PROGRESS_STATUS_T *progressMessage = new PROGRESS_STATUS_T;

   if(result == 0x0)   /* SUCCESS */
   {
      // last packet sent success
      // Inform the UI of progress
      progressMessage->status = DOWNLOADING;
      progressMessage->progress = (WORD)mImageFileHandler.GetTransferProgress();

      UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);

      if(StateGetOta() == STATE_OTA_DATA_TRANSFER)
      {
         // Continue sending image
         if(SendNextImageChunk() == false)
         {
            // Tell the remote device that all data is now sent,
            // probably triggering a device reset.
            OtaSetTransferComplete();
         }
      }
   }
   else
   {
	   CString logString;
	   if (result == CSR_OTA_IMAGE_TOO_BIG)
	   {
		   // Record that the update failed because the image is too big
		   logString.LoadString(IDS_OTA_IMAGE_BIG_ERR);
		   UiMsg::LogMessage(logString, ERROR_TYPE);

		   /* Display a window containing the error message */
		   progressMessage->status = ERR_IMAGE_TRANSFER_ABORTED;
		   progressMessage->progress = 0;
	   }
	   else if (result == CSR_OTA_BATTERY_LOW)
	   {
		   // Record that the update failed because the battery voltage is too low
		   logString.LoadString(IDS_OTA_BATTERY_LOW_ERR);
		   UiMsg::LogMessage(logString, ERROR_TYPE);
		   
		   /* Display a window containing the error message */
		   progressMessage->status = ERR_IMAGE_TRANSFER_ABORTED;
		   progressMessage->progress = 0;
	   }
	   else
	   {
		   // Data transfer failed
		   progressMessage->status = DOWNLOAD_CANCELLED;
		   progressMessage->progress = 0;
	   }

      //Inform ui
      UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);

      // Reset OTA update state
      mImageFileHandler.ResetTransfer();
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleWriteCharResult
*
*  DESCRIPTION
*      Called when a characteristic write action completes. Handle this 
*      information according to the current OTA state.
*---------------------------------------------------------------------------*/
 BOOL COTAInterface::OnBleWriteCharResult(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_WRITE_RESULT writeResult = (PCSR_BLE_WRITE_RESULT)lParam;
   int *result;

   if(CBLEInterface::OnBleWriteCharResult(wParam, lParam) == FALSE)
      return FALSE;

   switch(StateGetOta())
   {
   case STATE_OTA_INIT_READ_CHALLENGE:
      {
         if(writeResult->charHandle == getDataTransHandle())
         {
            if(writeResult->result == 0)
            {
               CString logString;
               logString.LoadString(IDS_OTA_CR_CFM_SUCCESS);
               UiMsg::LogMessage(logString, STATUS_TYPE);
               InitializeOta();
            }
            else
            {
               // Challenge-response failed - tell the user
               result = new int;
               *result = CHALLENGE_RESPONSE_FAILED;
               UiMsg::SendMsgToUI(CHALLENGE_RESPONSE_STATUS, result);

               stateSetOta(STATE_OTA_IDLE);
            }
         }
      }
      break;

   case STATE_OTA_INIT_CONFIGURING_CCD:
      if(writeResult->result == 0)
      {
         // Check whether all client descriptors are configured
         if(GATT_OP_WRITE_COMPLETE == WriteGattClientCharDescValue())
         {
            // We have requested notifications on every characteristic that
            // supports them.
            InitializeOta();
         }
         // else: we remain in this state for a little longer
      }
      else
      {
         stateSetOta(STATE_OTA_IDLE);
      }
      break;

   case STATE_OTA_INIT_READ_BT_ADDRESS:
      {
         if(writeResult->charHandle == getReadCsKeyHandle())
         {
            if(writeResult->result != 0)
            {
               // Reading the BT address failed - panic the user
               UiMsg::SendMsgToUI(BT_ADDRESS_READ_FAILED, NULL);
               // Disconnect - it would be unsafe to perform an update
               SetDeviceState(DEV_DISCONNECTING);
            }
            else
            {
               if(OTA_BOOTLOADER_MODE == mDeviceMode && m_BootVersion> 3)
               {
                  AddAttributeForReading(getDataTransHandle());
                  stateSetOta(STATE_OTA_INIT_READ_BT_ADDRESS);
               }
            }
         }
      }
      break;
   case STATE_OTA_INIT_READ_CS_BLOCK:
      {
         if(writeResult->charHandle == getReadCsBlockHandle())
         {
            if(writeResult->result != 0)
            {
               // Reading the BT address failed - panic the user
               UiMsg::SendMsgToUI(IDS_OTA_READ_CSBLOCKFAILED, NULL);
               // Disconnect - it would be unsafe to perform an update
               SetDeviceState(DEV_DISCONNECTING);
            }
            //else
            //{
            //   //AddAttributeForReading(getDataTransHandle());
            //   //stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK);
            //}
         }
      }
      break;
   case STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR:
      if(writeResult->charHandle == getReadCsBlockHandle())
      {
         if(writeResult->result != 0)
         {
            // Reading the BT address failed - panic the user
            UiMsg::SendMsgToUI(IDS_OTA_READ_CSBLOCKFAILED, NULL);
            // Disconnect - it would be unsafe to perform an update
            SetDeviceState(DEV_DISCONNECTING);
         }
         //else
         //{
         //   //AddAttributeForReading(getDataTransHandle());
         //   //stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR);
         //}
      }
      break;
   case STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM:
      if(writeResult->charHandle == getReadCsBlockHandle())
      {
         if(writeResult->result != 0)
         {
            // Reading the BT address failed - panic the user
            UiMsg::SendMsgToUI(IDS_OTA_READ_CSBLOCKFAILED, NULL);
            // Disconnect - it would be unsafe to perform an update
            SetDeviceState(DEV_DISCONNECTING);
         }
         //else
         //{
         //   //AddAttributeForReading(getDataTransHandle());
         //   //stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM);
         //}
      }
      break;
   case STATE_OTA_INIT_READ_CS_BLOCK_IDROOT:
      if(writeResult->charHandle == getReadCsBlockHandle())
      {
         if(writeResult->result != 0)
         {
            // Reading the BT address failed - panic the user
            UiMsg::SendMsgToUI(IDS_OTA_READ_CSBLOCKFAILED, NULL);
            // Disconnect - it would be unsafe to perform an update
            SetDeviceState(DEV_DISCONNECTING);
         }
         //else
         //{
         //   //AddAttributeForReading(getDataTransHandle());
         //   //stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_IDROOT);
         //}
      }
      break;
   case STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT:
      if(writeResult->charHandle == getReadCsBlockHandle())
      {
         if(writeResult->result != 0)
         {
            // Reading the BT address failed - panic the user
            UiMsg::SendMsgToUI(IDS_OTA_READ_CSBLOCKFAILED, NULL);
            // Disconnect - it would be unsafe to perform an update
            SetDeviceState(DEV_DISCONNECTING);
         }
         //else
         //{
         //   //AddAttributeForReading(getDataTransHandle());
         //   //stateSetOta(STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT);
         //}
      }
      break;
   case STATE_OTA_INIT_READ_XTAL_TRIM:

      if(OTA_BOOTLOADER_MODE == mDeviceMode && m_BootVersion> 3 
         && writeResult->charHandle == getReadCsKeyHandle())
      {
         AddAttributeForReading(getDataTransHandle());
         stateSetOta(STATE_OTA_INIT_READ_XTAL_TRIM);
      }
      else
      {
         if(writeResult->charHandle == getReadCsKeyHandle())
         {
             if(writeResult->result != 0)
            {
               // Reading the BT address failed - panic the user
               UiMsg::SendMsgToUI(IDS_OTA_READ_XTALTRIM_ERROR, NULL);
               // Disconnect - it would be unsafe to perform an update
               SetDeviceState(DEV_DISCONNECTING);
            }
         }
      }
      break;

   case STATE_OTA_INIT_READ_IDENTITY_ROOT:
      if(OTA_BOOTLOADER_MODE == mDeviceMode && m_BootVersion> 3 
         && writeResult->charHandle == getReadCsKeyHandle())
      {
         AddAttributeForReading(getDataTransHandle());
         stateSetOta(STATE_OTA_INIT_READ_IDENTITY_ROOT);
      }
      else
      {
         if(writeResult->charHandle == getReadCsKeyHandle())
         {
             if(writeResult->result != 0)
            {
               // Reading the BT address failed - panic the user
               UiMsg::SendMsgToUI(IDS_OTA_READ_IDROOT_ERROR, NULL);
               // Disconnect - it would be unsafe to perform an update
               SetDeviceState(DEV_DISCONNECTING);
            }
         }
      }
      break;

   case STATE_OTA_INIT_READ_ENCRYPTION_ROOT:
      if(OTA_BOOTLOADER_MODE == mDeviceMode && m_BootVersion> 3 
         && writeResult->charHandle == getReadCsKeyHandle())
      {
         AddAttributeForReading(getDataTransHandle());
         stateSetOta(STATE_OTA_INIT_READ_ENCRYPTION_ROOT);
      }
      else
      {
         if(writeResult->charHandle == getReadCsKeyHandle())
         {
             if(writeResult->result != 0)
            {
               // Reading the BT address failed - panic the user
               UiMsg::SendMsgToUI(IDS_OTA_READ_ENROOT_ERROR, NULL);
               // Disconnect - it would be unsafe to perform an update
               SetDeviceState(DEV_DISCONNECTING);
            }
         }
      }
      break;

   case STATE_OTA_INIT_SET_CURRENT_APP:
      if(writeResult->charHandle == getCurAppHandle())
      {
         if(writeResult->result != 0)
         {
            result = new int;
            *result = writeResult->result;

            // Setting the current app
            UiMsg::SendMsgToUI(SET_CUR_APP_RSP, result);
         }
         else
         {
            if(OTA_APPLICATION_MODE == mDeviceMode)
            {
               m_BLSwitched = TRUE;
            }

            CString logString;
            logString.LoadString(IDS_OTA_SETCUTAPP_SUCCESS);
            UiMsg::LogMessage(logString, STATUS_TYPE);
         }

         // Try to continue, even if the action failed
         InitializeOta();
      }
      break;

   case STATE_OTA_SET_CURRENT_APP:
      if(writeResult->charHandle == getCurAppHandle())
      {
         result = new int;
         *result = writeResult->result;

         UiMsg::SendMsgToUI(SET_CUR_APP_RSP, result);

         stateSetOta(STATE_OTA_IDLE);
      }
      break;

   case STATE_OTA_SET_MODE:
      if(writeResult->charHandle == getReadCsKeyHandle())
      {
         result = new int;
         *result = writeResult->result;
   
         if(writeResult->result != 0)
         {
            m_BLSwitched = TRUE;
         }
         UiMsg::SendMsgToUI(SET_OTA_MODE_RSP, result);
         stateSetOta(STATE_OTA_IDLE);
      }
      break;

   case STATE_OTA_SET_TRANSFER_CTRL:
      if(writeResult->charHandle == getTransCtrlHandle())
      {
         OtaSetTransferControlRsp(writeResult->result);
      }
      break;

   case STATE_OTA_DATA_TRANSFER:
      if(writeResult->charHandle == getDataTransHandle())
      {
		 OnOtaImagePacketTransferResult(writeResult->result);
	  }
      break;

   case STATE_OTA_PAUSE_DATA_TRANSFER:
      if(writeResult->charHandle == getTransCtrlHandle())
      {
         PROGRESS_STATUS_T *result = new PROGRESS_STATUS_T;
         result->progress = 0;
         result->status = DOWNLOAD_PAUSED;

         UiMsg::SendMsgToUI(PROGRESS_STATUS, result);
      }
      else if(writeResult->charHandle == getDataTransHandle())
      {
         OnOtaImagePacketTransferResult(writeResult->result);
      }
      break;

   case STATE_OTA_ABORT_DATA_TRANSFER:
      if(writeResult->charHandle == getTransCtrlHandle())
      {
         OtaAbortUpdateRsp(writeResult->result);
      }
      break;

   case STATE_OTA_RESET_TRANSFER_CTRL:
      if(writeResult->charHandle == getTransCtrlHandle())
      {
		  if (writeResult->supplier == CSR_BT_SUPPLIER_ATT)
		  {
			  CString logString;
			  if(writeResult->result == CSR_OTA_IMAGE_CORRUPT)
			  {
				  // Record that the update failed because the image is corrupt
				  logString.LoadString(IDS_OTA_IMAGE_CORRUPT_ERR);
				  UiMsg::LogMessage(logString, ERROR_TYPE);
				  
				  /* Display a window containing the error message */
				  PROGRESS_STATUS_T *progressMessage = new PROGRESS_STATUS_T;
				  progressMessage->progress = 0;
				  progressMessage->status = ERR_IMAGE_TRANSFER_FAILED;

				  UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
			  }
			  else if(writeResult->result == CSR_OTA_BATTERY_LOW)
			  {
				  // Record that the update failed because the battery voltage is too low
				  logString.LoadString(IDS_OTA_BATTERY_LOW_ERR);
				  UiMsg::LogMessage(logString, ERROR_TYPE);
				  
				  /* Display a window containing the error message */
				  PROGRESS_STATUS_T *progressMessage = new PROGRESS_STATUS_T;
				  progressMessage->progress = 0;
				  progressMessage->status = ERR_IMAGE_TRANSFER_FAILED;

				  UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
			  }
		  }
		  stateSetOta(STATE_OTA_IDLE);
      }
      break;
   }
   return TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetImageFileName
*
*  DESCRIPTION
*      Set the Image File Name that is to be uploaded.
*---------------------------------------------------------------------------*/
bool COTAInterface::SetImageFileName(CString fileName)
{
   return mImageFileHandler.SetImageFileName(fileName);
}

/*----------------------------------------------------------------------------*
*  NAME
*      OpenImageFileForUpdate
*
*  DESCRIPTION
*      Set the Image File Name that is to be uploaded.
*---------------------------------------------------------------------------*/
bool COTAInterface::OpenImageFileForUpdate()
{
   return mImageFileHandler.OpenImageFile();
}

/*----------------------------------------------------------------------------*
*  NAME
*      OpenImageFile
*
*  DESCRIPTION
*      Set the Image File Name that is to be uploaded.
*---------------------------------------------------------------------------*/
bool COTAInterface::CloseImageFile()
{
   return mImageFileHandler.CloseImageFile();
}



/*----------------------------------------------------------------------------*
*  NAME
*      OnBleNotificationReceived
*
*  DESCRIPTION
*      Called when a notification is received from the remote device.
*      Handle according to the current OTA state.
*---------------------------------------------------------------------------*/
void COTAInterface::OnBleNotificationReceived(WPARAM wParam, LPARAM lParam)
{
   PCSR_BLE_CHAR_NOTIFICATION notification = (PCSR_BLE_CHAR_NOTIFICATION)lParam;
   PROGRESS_STATUS_T *progressMessage;
   
   BOOL bOTAMsg = FALSE;
   switch(StateGetOta())
   {
   case STATE_OTA_INIT_READ_CS_BLOCK:
      if(notification->charHandle == getDataTransHandle())
      {
         if(notification->charValueSize == 2)
         {
            CString logString;
            // Read the version of the CS Key DB
            LPBYTE data = (LPBYTE)notification->charValue;
            WORD csVersion = ((WORD)data[1] << 8) | data[0];
            m_KeysInfo.SetInterestedVersion(csVersion);
            InitializeOta();
         }
         bOTAMsg = TRUE;
      }
      break;
 
   case STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR:
   case STATE_OTA_INIT_READ_BT_ADDRESS:
      if(notification->charHandle == getDataTransHandle())
      {
         if(notification->charValueSize == 6)
         {
            CString logString;
            // Store the BT address
            mImageFileHandler.SetImageBtAddress((BYTE*)(notification->charValue));

            CString address;
            addressToString((BYTE*)(notification->charValue), &address);
            logString.Format(IDS_OTA_READ_BDADDRESS, address);
            UiMsg::LogMessage(logString, INFO_TYPE);

            InitializeOta();
         }
         else
         {
            // Reading the BT address failed - panic the user
            UiMsg::SendMsgToUI(BT_ADDRESS_READ_FAILED, NULL);
            // Disconnect - it would be unsafe to perform an update
            SetDeviceState(DEV_DISCONNECTING);
         }
         bOTAMsg = TRUE;
      }
      break;

   case STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM:
   case STATE_OTA_INIT_READ_XTAL_TRIM:
      if(notification->charHandle == getDataTransHandle())
      {
         if(notification->charValueSize > 1)
         {
            CString logString ;
            logString.Format(IDS_OTA_READ_XTALTRIM, *(BYTE*)(notification->charValue));
            UiMsg::LogMessage(logString, STATUS_TYPE);

            // Store the crystal trim
            mImageFileHandler.SetImageXtalTrim(*(BYTE*)(notification->charValue));

            // Now wait for user input
            InitializeOta();
         }
         bOTAMsg = TRUE;
      }
      break;

   case STATE_OTA_INIT_READ_CS_BLOCK_IDROOT:
   case STATE_OTA_INIT_READ_IDENTITY_ROOT:
      if(notification->charHandle == getDataTransHandle())
      {
         if(notification->charValueSize >=  8)
         {
            // Store the crystal trim
            mImageFileHandler.SetImageIdentityRoot((BYTE*)notification->charValue);

            CString pString = CString("Identity Root: 0x");
            IdentityRootToString((unsigned short *)notification->charValue, &pString);
            UiMsg::LogMessage(pString, STATUS_TYPE);


            // Now wait for user input
            InitializeOta();
         }
         bOTAMsg = TRUE;
      }
      break;

    case STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT:
    case STATE_OTA_INIT_READ_ENCRYPTION_ROOT:
      if(notification->charHandle == getDataTransHandle())
      {
         if(notification->charValueSize > 1)
         {
            // Store the crystal trim
            mImageFileHandler.SetImageEncryptionRoot((BYTE*)(notification->charValue));

            CString pString = CString("Encryption Root: 0x");
            EncryptionRootToString((unsigned short *)notification->charValue, &pString);
            UiMsg::LogMessage(pString, STATUS_TYPE);

            // Now wait for user input
            InitializeOta();
         }
         bOTAMsg = TRUE;
      }
      break;

   case STATE_OTA_DATA_TRANSFER:
   case STATE_OTA_PAUSE_DATA_TRANSFER:
   case STATE_OTA_RESET_TRANSFER_CTRL:
   case STATE_OTA_ABORT_DATA_TRANSFER:
      if(notification->charHandle == getTransCtrlHandle())
      {
         switch(*(BYTE*)(notification->charValue))
         {
         case CMD_SET_TRANSFER_CONTROL:
            break;

         case CMD_SET_TRANSFER_PAUSED:
            progressMessage = new PROGRESS_STATUS_T;
            progressMessage->progress = 0;
            progressMessage->status = DOWNLOAD_PAUSED;

            UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
            break;

         case CMD_SET_TRANSFER_COMPLETED:
            progressMessage = new PROGRESS_STATUS_T;
            progressMessage->progress = 0;
            progressMessage->status = DOWNLOAD_COMPLETE;

            UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
            break;

         case CMD_SET_TRANSFER_FAILED:
			progressMessage = new PROGRESS_STATUS_T;
            progressMessage->progress = 0;
            progressMessage->status = ERR_IMAGE_TRANSFER_FAILED;

            UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
            break;

         case CMD_ABORT_DATA_TRANSFER:
			progressMessage = new PROGRESS_STATUS_T;
            progressMessage->progress = 0;
            progressMessage->status = ERR_IMAGE_TRANSFER_ABORTED;

            UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);
            break;
         }
         bOTAMsg = TRUE;
      }
      break;
   }

   if(bOTAMsg == FALSE)
   {
      CSR_BLE_UUID _uuid, svc_uuid; 
      Init16BitUUID(svc_uuid, Uuids::UUID_GATT_SERVICE);
      Init16BitUUID(_uuid, Uuids::UUID_SERVICE_CHANGED);
      WORD handle = m_CurBleDevice->FindCharHandle(svc_uuid, _uuid);
      if(notification->charHandle == m_LastGattServChgHandle)
      {
         CString logString; 
         logString.LoadString(IDS_GATT_SERV_CHGD_IND);
         UiMsg::LogMessage(logString, STATUS_TYPE);

         m_LastGattServChgHandle = 0;
      }
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleReady
*
*  DESCRIPTION
*      Called when the Local device is ready. 
*---------------------------------------------------------------------------*/
void COTAInterface::OnBleReady(WPARAM wParam, LPARAM lParam)
{
   CBLEInterface::OnBleReady(wParam, lParam);
   if(lParam == TRUE)
   {
      // See whether the AES DLL is available
      if(Aes::CheckForDll() == TRUE)
      {
         UiMsg::SendMsgToUI(AES_STATUS, (void*)TRUE);
      }
      else
      {
         UiMsg::SendMsgToUI(AES_STATUS, (void*)FALSE);
      }
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      StartOTA
*
*  DESCRIPTION
*      Starts the OTA process. 
*---------------------------------------------------------------------------*/
void COTAInterface::StartOTA()
{
   mDeviceMode = CheckDeviceOTAMode(); 
   CSR_BLE_BLUETOOTH_ADDRESS devAddr = m_CurBleDevice->GetAddress();
   CSR_BLE_BT_ADDRESS addr;
   memcpy(&addr, &devAddr,  sizeof(addr));
   DeviceState state = GetConnectionState(&addr);

   if (mDeviceMode != NOT_SUPPORTING_OTA)
   {
      // Enable/disable bits of the display applicable to boot-loader
      // or application mode
      UiMsg::SendMsgToUI(REMOTE_APPLICATION_MODE, &mDeviceMode);

      // Initialise iterator to the beginning of characteristics
      m_BleOtaCharsTableIterator = m_BleOtaService.chars.begin();
      // Continue with the next step of device synchronisation
      InitializeOta();
   }
   else
   {
      if(state == DEV_CONNECTED)
      {
         //AfxMessageBox(TEXT("This device does not appear to support\nCSR OTA-update. Disconnecting..."));
         UiMsg::SendMsgToUI(REMOTE_APPLICATION_MODE, &mDeviceMode);
      }

      // cancel all ongoing operations
      CsrBleHostCancel();

      // disconnect
      CsrBleHostDisconnect(m_CurBleDevice->GetConnectionHandle());
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBleReadCharResult
*
*  DESCRIPTION
*      Called when a read action on the characteristic completes.
*      Handle the information according to the current OTA state.
*---------------------------------------------------------------------------*/
BOOL COTAInterface::OnBleReadCharResult(WPARAM wParam, LPARAM lParam)
{
   if(CBLEInterface::OnBleReadCharResult(wParam, lParam) == FALSE)
      return FALSE;


   PCSR_BLE_CHAR_READ_RESULT readResult = (PCSR_BLE_CHAR_READ_RESULT)lParam;
   
   if(readResult->charHandle == GetBatteryLevelHandle())
   {
	   // If the read was successful,read the battery level
	   if(readResult->result == 0)
	   {
		   int BatteryLevel = *(LPBYTE)readResult->charValue;
		   CString logString;
		   
		   // If the battery level is zero ,do not continue to OTA.
		   if(BatteryLevel == 0)
		   {
			   if(m_DeviceState == DEV_CONNECTED)
			   {
				   logString.LoadString(IDS_BATTERY_FAIL);
				   UiMsg::LogMessage(logString, ERROR_TYPE);
			   }

			   // cancel all ongoing operations
			   CsrBleHostCancel();
			   
			   // disconnect
			   CsrBleHostDisconnect(m_CurBleDevice->GetConnectionHandle());
		   }
		   else
		   {
			   StartOTA();
		   }
	   }
	   return TRUE;
   }
   unsigned char challengeResponseKey[AES_KEY_LENGTH_BYTES];
   unsigned short currentState = StateGetOta();

   // Is this the result of reading the current active application?
   if( (currentState == STATE_OTA_INIT_READ_CURRENT_APP) ||
      (currentState == STATE_OTA_GET_CURRENT_APP))
   {
      // Yup, thought so
      // If the read was successful, update the display with the result
      if(readResult->result == 0)
      {        
         mAppId = *(unsigned char*)(readResult->charValue);
         if(mAppId == 1 || mAppId == 2)
         {
            unsigned char *result = new unsigned char;
            *result = *(unsigned char*)(readResult->charValue); 
            UiMsg::SendMsgToUI(READ_CUR_APP_RSP, result);
         }
         else
         {
            // There is no application in the chip
            // invalid app id
            CString logString; 
            logString.Format(IDS_OTA_CURAPP_ERROR, mAppId);
            UiMsg::LogMessage(logString, ERROR_TYPE);
            UiMsg::SendMsgToUI(BT_CURAPP_NOT_SUPPORTED, NULL);
         }
      }
      else
      {
         // ATT_RESULT_REQUEST_NOT_SUPPORTED
         if(readResult->result == 6)
         {
            mAppId = 1;
         }
         //inform ui
         UiMsg::SendMsgToUI(BT_CURAPP_READ_FAILED, NULL);
      }

      if(currentState == STATE_OTA_INIT_READ_CURRENT_APP)
      {
         InitializeOta();
      }
      else
      {
         stateSetOta(STATE_OTA_IDLE);
      }
   }
   // Is this the result of reading the challenge value?
   else if(currentState == STATE_OTA_INIT_READ_CHALLENGE)
   {
      // If the read was successful, rise to the challenge
      if( (readResult->result == 0) &&
         (readResult->charValueSize == AES_KEY_LENGTH_BYTES))  // 128-bits == 16-bytes
      {
         memcpy(challengeResponseKey, readResult->charValue, AES_KEY_LENGTH_BYTES);

         Aes::ProcessChallenge(challengeResponseKey);

         // Write back the result...
         AddAttributeForWriting(getDataTransHandle(),AES_KEY_LENGTH_BYTES, 
            (LPBYTE)challengeResponseKey);
      }
      else
      {
         // Challenge-response failed
         unsigned char *result = new unsigned char;
         *result = CHALLENGE_RESPONSE_FAILED;
         UiMsg::SendMsgToUI(CHALLENGE_RESPONSE_STATUS, result);
         InitializeOta();
      }
   }
   else if(currentState == STATE_OTA_INIT_READ_VERSION)
   {
      if( (readResult->result == 0) &&
         (readResult->charValueSize == 1))
      {
         m_BootVersion =  *(unsigned char*)readResult->charValue;
         unsigned char *result  = new unsigned char;
         *result = m_BootVersion;
         UiMsg::SendMsgToUI(READ_VERSION_RESPONSE, result);
      }
      InitializeOta();
   }
   // Reading fronm bootloader
   else if((currentState == STATE_OTA_INIT_READ_BT_ADDRESS) ||
      (currentState == STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR))
   {
      if(readResult->charHandle == getDataTransHandle())
      {
         if(readResult->charValueSize == 6)
         {
            CString logString;
            // Store the BT address
            mImageFileHandler.SetImageBtAddress((BYTE*)(readResult->charValue));

            CString address;
            addressToString((BYTE*)(readResult->charValue), &address);
            logString.Format(IDS_OTA_READ_BDADDRESS, address);
            UiMsg::LogMessage(logString, INFO_TYPE);

            InitializeOta();
         }
         else
         {
            // Reading the BT address failed - panic the user
            UiMsg::SendMsgToUI(BT_ADDRESS_READ_FAILED, NULL);
            // Disconnect - it would be unsafe to perform an update
            SetDeviceState(DEV_DISCONNECTING);
         }
      }
   }
   else if ((currentState == STATE_OTA_INIT_READ_XTAL_TRIM) ||
       (currentState == STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM))
   {
       if(readResult->charHandle == getDataTransHandle())
      {
         if(readResult->charValueSize >= 1)
         {
            CString logString ;
            logString.Format(IDS_OTA_READ_XTALTRIM, *(BYTE*)(readResult->charValue));
            UiMsg::LogMessage(logString, STATUS_TYPE);

            // Store the crystal trim
            mImageFileHandler.SetImageXtalTrim(*(BYTE*)(readResult->charValue));

            // Now wait for user input
            InitializeOta();
         }
      }
   }
 else if ((currentState == STATE_OTA_INIT_READ_IDENTITY_ROOT) ||
         (currentState == STATE_OTA_INIT_READ_CS_BLOCK_IDROOT))
   {
       if(readResult->charHandle == getDataTransHandle())
      {
         if(readResult->charValueSize >= 16)
         {
            mImageFileHandler.SetImageIdentityRoot((BYTE*)readResult->charValue);

            CString pString = CString("Identity Root: 0x");
            IdentityRootToString((unsigned short *)readResult->charValue, &pString);
            UiMsg::LogMessage(pString, STATUS_TYPE);

            // Now wait for user input
            InitializeOta();
         }
      }
   }
 else if ((currentState == STATE_OTA_INIT_READ_ENCRYPTION_ROOT) ||
    (currentState == STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT))
   {
       if(readResult->charHandle == getDataTransHandle())
      {
         if(readResult->charValueSize >=  16)
         {
             mImageFileHandler.SetImageEncryptionRoot((BYTE*)(readResult->charValue));

            CString pString = CString("Encryption Root: 0x");
            EncryptionRootToString((unsigned short *)readResult->charValue, &pString);
            UiMsg::LogMessage(pString, STATUS_TYPE);

            // Now wait for user input
            InitializeOta();
         }
      }
   }
   else
   {
      // Whatever the read was, it is now processed
      stateSetOta(STATE_OTA_IDLE);
   }

   if(!m_ReadAttrList.empty())
   {
      ReadAttributes();
   }

   return TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SendNextImageChunk
*
*  DESCRIPTION
*      Send the next 20-bytes of the image file to the remote device 
*---------------------------------------------------------------------------*/
bool COTAInterface::SendNextImageChunk()
{
   // The next chunk of image file to be sent
   BYTE imgData[ImageFileHandler::IMAGE_NORMAL_CHUNK_SIZE_BYTES];    
   BYTE actualChunkSize;
   bool dataSent = false;

   // Get the next chunk of the image file
   actualChunkSize = 
      mImageFileHandler.GetNextChunk(ImageFileHandler::IMAGE_NORMAL_CHUNK_SIZE_BYTES, imgData);

   // Send it
   if(actualChunkSize > 0)
   {
      //update the state
      stateSetOta(STATE_OTA_DATA_TRANSFER);

      //write data to the remote device
      AddAttributeForWriting(getDataTransHandle(), actualChunkSize, LPBYTE(imgData));

      dataSent = true;
   }

   return dataSent;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetDeviceState
*
*  DESCRIPTION
*      Set the state of the OTA Device 
*---------------------------------------------------------------------------*/
void COTAInterface::SetDeviceState(DeviceState newState)
{
   BOOL stateChange = FALSE;   // Has the state changed this time?
   if(m_DeviceState == newState)
   {
      return;
   }

   CSR_BLE_BLUETOOTH_ADDRESS address;
   BOOL devicePresent = FALSE;
   
   switch(newState)
   {
   case DEV_IDLE:
      stateSetOta(STATE_OTA_IDLE);
      // Reset OTA update state
      mImageFileHandler.ResetTransfer();
      stateChange = TRUE;
      if(m_CurBleDevice != NULL)
      {
         address = m_CurBleDevice->GetAddress();
         devicePresent = TRUE;
      }
      break;
   default:
      break;
   }

   CBLEInterface::SetDeviceState(newState);

   if(mDeviceMode == OTA_APPLICATION_MODE && devicePresent == TRUE && m_BLSwitched == TRUE)
   {
      m_BLSwitched = FALSE;
      ToggleConnection((CSR_BLE_BT_ADDRESS*)&address);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      WriteGattClientCharDescValue
*
*  DESCRIPTION
*      This method calls the Gatt write using attribute handles.
*      It enables the OTA service for indications.
*      This api is called repeatedly until all the indications are enabled.
*---------------------------------------------------------------------------*/
WORD COTAInterface::WriteGattClientCharDescValue()
{
   WORD state = GATT_OP_WRITE_COMPLETE;
   CString logString;

   //check if all the indications are enabled
   UUIDCharacteristicPair::iterator it = m_BleOtaService.chars.end();
   while((m_BleOtaCharsTableIterator != it) && (state == GATT_OP_WRITE_COMPLETE))
   {
      //check for valid handle
      if(m_BleOtaCharsTableIterator->second.cconfigHandle != 0)
      {
         //Gatt write client configuation
         AddAttributeForWriting(m_BleOtaCharsTableIterator->second.valueHandle, 
            m_BleOtaCharsTableIterator->second.cconfigHandle, CSR_BLE_NOTIFICATION);
         
         //return write pending
         state = GATT_OP_WRITE_PENDING;
         logString.LoadString(IDS_OTA_CFG_NOTIFICATION);
         UiMsg::LogMessage(logString, STATUS_TYPE);
      }
      ++m_BleOtaCharsTableIterator;
   }
   CSR_BLE_UUID _uuid, svc_uuid; 
   Init16BitUUID(svc_uuid, Uuids::UUID_GATT_SERVICE);
   Init16BitUUID(_uuid, Uuids::UUID_SERVICE_CHANGED);
   WORD cfg_handle = m_CurBleDevice->FindCharHandle(svc_uuid, _uuid, true);
   WORD handle = m_CurBleDevice->FindCharHandle(svc_uuid, _uuid);
   

   if((state == GATT_OP_WRITE_COMPLETE) && (mServiceChangeConfigured == FALSE) &&
      (cfg_handle != HANDLE_UNKNOWN))
   {
      //Gatt write client configuation
      AddAttributeForWriting(handle, cfg_handle, CSR_BLE_INDICATION);
      //return write pending
      state = GATT_OP_WRITE_PENDING;
      mServiceChangeConfigured = TRUE;
      m_LastGattServChgHandle = handle;

      logString.LoadString(IDS_OTA_CFG_INDICATION);
      UiMsg::LogMessage(logString, STATUS_TYPE);
   }

   WORD batt_lvl_cfg_handle = GetBatteryLevelCfgHandle();
   WORD batt_lvl_handle = GetBatteryLevelHandle();

   if((state == GATT_OP_WRITE_COMPLETE) && (mBatteryLevelConfigured == FALSE) &&
      (batt_lvl_cfg_handle != HANDLE_UNKNOWN))
   {
      //Battery level client configuation
      AddAttributeForWriting(batt_lvl_handle, batt_lvl_cfg_handle, CSR_BLE_NOTIFICATION);
      //return write pending
      state = GATT_OP_WRITE_PENDING;
      mBatteryLevelConfigured = TRUE;

      logString.LoadString(IDS_BATT_CFG_NOTIFICATION);
      UiMsg::LogMessage(logString, STATUS_TYPE);
   }


   return state;
}

