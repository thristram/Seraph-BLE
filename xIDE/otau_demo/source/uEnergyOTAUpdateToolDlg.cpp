/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uEnergyOTAUpdateToolDlg.cpp
 *
 *  DESCRIPTION
 *      Defines the class behaviors for the main application dialog.
 *
 ******************************************************************************/


#include "stdafx.h"
#include "UiMsg.h"
#include "uEnergyOTAUpdateTool.h"
#include "uEnergyOTAUpdateToolDlg.h"
#include "CUserPromptDlg.h"
#include "DlgProxy.h"
#include <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define OTA_CONNECTION_TIMER              10211
#define OTA_CONNECTION_TIMEOUT_INTERVAL   15000

UINT CsrBleClientMessage;
UINT OTABroadcastMessageId;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

   // Dialog Data
   enum { IDD = IDD_ABOUTBOX };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
   // Implementation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
   ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()

void CAboutDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
        PNMLINK pNMLink = (PNMLINK) pNMHDR;   

        ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);

        *pResult = 0;
}


// COTAUpdateToolDlg dialog




IMPLEMENT_DYNAMIC(COTAUpdateToolDlg, CDialog);

COTAUpdateToolDlg::COTAUpdateToolDlg(CString strRegPath, CWnd* pParent /*=NULL*/)
: CDialog(COTAUpdateToolDlg::IDD, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   pUserPrompt = NULL;
   usrDlgCurrentUsage = UserDialogUsageNone;
   m_pAutoProxy = NULL;
   m_AutoLog = FALSE;
   m_DownLoadInProgress = FALSE;
   m_IsStartingUp = TRUE;
   connectionTimer = 0;
   m_RegPath = CString("Software\\Cambridge Silicon Radio\\CSR µEnergy Over-the-Air Updater\\") + strRegPath;
   m_AuthEnabled = FALSE;
   m_MitmEnabled = FALSE;
   mAesAvailable = FALSE;
   srand((unsigned int)time(NULL));
}

COTAUpdateToolDlg::~COTAUpdateToolDlg()
{
   // If there is an automation proxy for this dialog, set
   //  its back pointer to this dialog to NULL, so it knows
   //  the dialog has been deleted.
   if (m_pAutoProxy != NULL)
      m_pAutoProxy->m_pDialog = NULL;

   delete m_pRadioInterface;
}

void COTAUpdateToolDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LOGGING_LIST, m_LogListBox);
   DDX_Control(pDX, IDC_SECRETKEY_EDIT, m_SharedKeyEdit);
}

BEGIN_MESSAGE_MAP(COTAUpdateToolDlg, CDialog)
   ON_WM_SYSCOMMAND()
   ON_WM_CLOSE()
   ON_WM_PAINT()
   ON_WM_TIMER()
   ON_WM_QUERYDRAGICON()
   ON_WM_INITMENUPOPUP()
   ON_MESSAGE(WM_DEVICECHANGE, OnDeviceChange)
   ON_MESSAGE(WM_POWERBROADCAST, OnPowerStateChange)
   ON_MESSAGE(DM_GETDEFID,                    OnGetDefID)
   ON_MESSAGE(DIALOG_MESSAGE_ID,              OnDialogMessage)
   ON_REGISTERED_MESSAGE(CsrBleClientMessage, OnBleMessage)
   ON_BN_CLICKED(IDC_SELECTFILE_BUTTON,       OnBnClickedBrowseImage)
   ON_BN_CLICKED(IDC_SCAN_BUTTON,             OnBnClickedButtonDeviceScan)
   ON_BN_CLICKED(IDC_UPDATE_BUTTON,           OnBnClickedImageDownload)
   ON_BN_CLICKED(IDC_REMOVE_BUTTON,           OnBnClickedRemoveDevice)
   ON_BN_CLICKED(IDC_ABOUT_BOX,               OnBnClickedAbout)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_DEVICE_LIST,OnLvnItemChangedDevList)
   ON_COMMAND(ID_LOG_AUTOLOG, OnLogAutoLog)
   ON_COMMAND(ID_LOG_CLEARLOG, OnLogClearlog)
   ON_UPDATE_COMMAND_UI(ID_LOG_AUTOLOG, OnUpdateAutoLog)
   //}}AFX_MSG_MAP
   ON_EN_UPDATE(IDC_SECRETKEY_EDIT, &COTAUpdateToolDlg::OnEnUpdateSecretkeyEdit)
   ON_EN_CHANGE(IDC_SECRETKEY_EDIT, &COTAUpdateToolDlg::OnEnChangeSecretkeyEdit)
   ON_BN_CLICKED(IDC_CHECK_AUTH, &COTAUpdateToolDlg::OnBnClickedCheckAuth)
   ON_BN_CLICKED(IDC_GENERATEKEY_BTN, &COTAUpdateToolDlg::OnBnClickedGeneratekeyBtn)
   ON_BN_CLICKED(IDC_ENABLE_MITM, &COTAUpdateToolDlg::OnBnClickedEnableMitm)
END_MESSAGE_MAP()


// COTAUpdateToolDlg message handlers
/*----------------------------------------------------------------------------*
*  NAME
*      OnInitDialog
*
*  DESCRIPTION
*      Dialog Initialization function.
*---------------------------------------------------------------------------*/
BOOL COTAUpdateToolDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      BOOL bNameValid;
      CString strAboutMenu;
      bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
      ASSERT(bNameValid);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   // TODO: Add extra initialization here
   HKEY lhKey;
   LSTATUS stat;
   if (RegOpenKeyEx(HKEY_CURRENT_USER, m_RegPath, 0, KEY_ALL_ACCESS, &lhKey) != 0)
   {
      stat = RegCreateKeyEx(HKEY_CURRENT_USER, m_RegPath, 0, 0, 0, KEY_ALL_ACCESS, 0, &lhKey, 0);
   }

   WCHAR wPath[MAX_PATH] = {0};
   DWORD len = sizeof(wPath) ;
   DWORD type = REG_SZ;

   DWORD dwAutoLog;
   len = sizeof(DWORD) ;
   type = REG_DWORD;
   stat = RegQueryValueEx(lhKey, __T("LogToText"), 0, &type, (LPBYTE)&dwAutoLog, &len);
   if(stat == ERROR_SUCCESS)
   {
      m_AutoLog = dwAutoLog;
      if(m_AutoLog == TRUE)
      {
         SYSTEMTIME lt;
         GetLocalTime(&lt);
         CTime t(lt);
         CString strFileName = t.Format(__T("OTAUpgradeLog_%d%m%y%H%M%S.log"));

         m_LogFile.Open(strFileName, CFile::modeCreate|CFile::modeReadWrite);
      }
   }

   CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
   WCHAR secret_key[MAX_PATH];
   len = MAX_PATH;
   type = REG_SZ;
   stat = RegQueryValueEx(lhKey, __T("SecretKey"), 0, &type, (LPBYTE)secret_key, &len);
   if(stat == ERROR_SUCCESS)
   {
      pEdit->SetWindowText(secret_key);
   }
   else
   {
      m_keyText = __T("00112233445566778899aabbccddeeff");
      pEdit->SetWindowText(__T("00112233445566778899aabbccddeeff"));
   }

   CButton* pAuthBtn = (CButton*)GetDlgItem(IDC_CHECK_AUTH);
   DWORD dwAuthBtn = 0;
   len = sizeof(DWORD) ;
   type = REG_DWORD;
   stat = RegQueryValueEx(lhKey, __T("Challenge-Response"), 0, &type, (LPBYTE)&dwAuthBtn, &len);
   if(stat == ERROR_SUCCESS)
   {
      m_AuthEnabled = dwAuthBtn;
   }

   if(m_AuthEnabled == TRUE)
   {
      pAuthBtn->SetCheck(BST_CHECKED);
   }
   else
   {
      pAuthBtn->SetCheck(BST_UNCHECKED);
   }

   CButton* pGenerateKey = (CButton*)GetDlgItem(IDC_GENERATEKEY_BTN);
   if(m_AuthEnabled == TRUE)
   {
      pGenerateKey->EnableWindow(TRUE);
      pEdit->EnableWindow(TRUE);
   }
   else
   {
      pGenerateKey->EnableWindow(FALSE);
      pEdit->EnableWindow(FALSE);
   }
  
   CButton* pMitmBtn = (CButton*)GetDlgItem(IDC_ENABLE_MITM);
   DWORD dwMitmBtn = 0;
   len = sizeof(DWORD) ;
   type = REG_DWORD;
   stat = RegQueryValueEx(lhKey, __T("MITM Protection"), 0, &type, (LPBYTE)&dwMitmBtn, &len);
   if(stat == ERROR_SUCCESS)
   {
      m_MitmEnabled = dwMitmBtn;
	  m_pRadioInterface->SetMITMFlag(m_MitmEnabled);
   }
   if(m_MitmEnabled == TRUE)
   {
      pMitmBtn->SetCheck(BST_CHECKED);
   }
   else
   {
      pMitmBtn->SetCheck(BST_UNCHECKED);
   }

   DWORD dwLogSynergy = 0;
   len = sizeof(DWORD) ;
   type = REG_DWORD;
   stat = RegQueryValueEx(lhKey, __T("LogSynergy"), 0, &type, (LPBYTE)&dwLogSynergy, &len);
   m_SynergyLog = dwLogSynergy;

   CString title;
   title.LoadString(IDS_APP_TITLE);
  
   OTABroadcastMessageId = RegisterWindowMessage(_T("{97945B19-89A6-4b50-B2AF-7EAF53665244}"));
   singleInstanceMutex = CreateMutex(NULL, FALSE, _T("{68730F6C-4BE3-420c-8CBF-05B5CC860060}"));

   if(singleInstanceMutex != NULL && GetLastError()!= ERROR_ALREADY_EXISTS)
   {
      CString logMsg;
      logMsg.LoadString(IDS_APP_INIT);
      UiMsg::LogMessage(logMsg, STATUS_TYPE);
      m_pRadioInterface = new COTAInterface();
      RADIO_FAILURES dllInitResult;
      if((dllInitResult = m_pRadioInterface->BleInit(m_hWnd, CsrBleClientMessage, dwLogSynergy)) != RADIO_INIT_SUCCESS)
      {
         EndDialog(0);
         return FALSE;
      }
      //intialise Ble radio
      if(!(CsrBleClientMessage)) 
      {
         // BLE dongle initialisation failed
         EndDialog(0);
         return FALSE;
      }
      else if(dllInitResult != RADIO_INIT_SUCCESS)
      {
         switch(dllInitResult)
         {
         default:
         case RADIO_FAIL_CLIENT_INIT:
            logMsg.LoadString(IDS_APP_INIT_ERROR);
            MessageBox(logMsg, title);
            break;
         case RADIO_FAIL_CLIENT_START:
            logMsg.LoadString(IDS_APP_START_ERROR);
            MessageBox(logMsg, title);
            break;
         }
         EndDialog(0);
         return FALSE;
      }
   }
   else
   {
      /* The process is already running. Tell the same to the user and
      * send the message to all the windows. This message when received by
      * the already running demo app will bring itself to the foreground.
      */

      CString logMsg;
      logMsg.LoadString(IDS_PROCESS_RUNNING);
      MessageBox(logMsg, title);
      ::PostMessage(HWND_BROADCAST, OTABroadcastMessageId, 0, 0);
       EndDialog(0);
      return FALSE;
   }
   
   len = MAX_PATH;
   type = REG_SZ;
   stat = RegQueryValueEx(lhKey, __T("Image Path"), 0, &type, (LPBYTE)wPath, &len);
   if(stat == ERROR_SUCCESS)
   {
      m_PathName = CString(wPath);
      CEdit *cEdit = (CEdit*)GetDlgItem(IDC_IMAGE_PATH);
      cEdit->SetWindowText(m_PathName);
      m_pRadioInterface->SetImageFileName(m_PathName);
   }
   RegCloseKey(lhKey);

   // Set the progress bar to zero
   ((CProgressCtrl*)GetDlgItem(IDC_PROGRESS))->SetPos(0);

   CListCtrl* pCtrl = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   DWORD exStyle = pCtrl->GetExtendedStyle();
   pCtrl->SetExtendedStyle(exStyle | LVS_EX_FULLROWSELECT);
   pCtrl->InsertColumn(0, __T("Address"), LVCFMT_LEFT, 100); 
   pCtrl->InsertColumn(1, __T("Name"), LVCFMT_LEFT, 200);

   m_bHandleKeyUpdateMsg = FALSE;   

   return TRUE;  // return TRUE  unless you set the focus to a control
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnTimer
*
*  DESCRIPTION
*      Timer callback
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnTimer(UINT_PTR nIDEvent)
{
   if(nIDEvent == connectionTimer)
   {
      // Disconnect and kill the timer

      CString logMsg;
      logMsg.LoadString(IDS_DEV_CONTIMEOUT);
      UiMsg::LogMessage(logMsg, ERROR_TYPE);

      m_pRadioInterface->CancelConnection();
      ResetConnection();

      KillTimer(connectionTimer);
      connectionTimer = 0;
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBnClickedBrowseImage
*
*  DESCRIPTION
*      Called when the browse for file button is clicked when selcting the 
*      image file to download.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnBnClickedBrowseImage()
{
   CString PathName;
   PROGRESS_STATUS_T *progressMessage;

   //get the device from the device search list window

   CFileDialog FileDialog( TRUE,
      TEXT("*.img;*.flash.xuv"),
      NULL,
      OFN_HIDEREADONLY,
      TEXT("EEPROM Images: (*.img)|*.img|Flash Images: (*.flash.xuv)|*.flash.xuv||"));

   if(FileDialog.DoModal() == IDOK)
   {
      //load the file path field with the selected image
      CEdit *cEdit = (CEdit*)GetDlgItem(IDC_IMAGE_PATH);
      PathName = FileDialog.GetPathName();

      cEdit->SetWindowText(PathName);

      //pass the name to the image processor
      if(m_pRadioInterface->SetImageFileName(PathName) == false)
      {
         // The specified file could not be opened. Tell the user.
         // Set Transfer Control failed
         progressMessage = new PROGRESS_STATUS_T;

         progressMessage->progress = 0;
         progressMessage->status = ERR_UNABLE_TO_OPEN_FILE;

         UiMsg::SendMsgToUI(PROGRESS_STATUS, progressMessage);

      }
      else
      {
         m_PathName  = PathName;
         // TODO addd to registry
      }
   }


   CListCtrl *pDeviceList  = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   POSITION Pos = pDeviceList->GetFirstSelectedItemPosition();
   if(Pos != NULL)
   {
      CButton* updateBtn = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
      if(m_PathName.IsEmpty() == FALSE)
      {
         updateBtn->EnableWindow(TRUE);
      }
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      ResetConnection
*
*  DESCRIPTION
*      Reset the connection state .
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::ResetConnection()
{
   mRemoteDeviceMode = NOT_SUPPORTING_OTA;
   CListCtrl* pDevCtrl = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   POSITION Pos = pDevCtrl->GetFirstSelectedItemPosition();

   CButton *button = NULL;
   if(Pos != NULL)
   {
      if(m_PathName.IsEmpty() == FALSE)
      {
         //show connect button
         button = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
         button->EnableWindow(TRUE);
      }
      button = (CButton*)GetDlgItem(IDC_REMOVE_BUTTON);
      button->EnableWindow(TRUE);
   }

   button = (CButton*)GetDlgItem(IDC_SELECTFILE_BUTTON);
   button->EnableWindow(TRUE);

   button = (CButton*)GetDlgItem(IDC_SCAN_BUTTON);
   button->EnableWindow(TRUE);

   if(connectionTimer != 0)
   {
      KillTimer(connectionTimer);
      connectionTimer = 0;
   }
   
   //set progress bar to initial position
   CProgressCtrl *pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
   pCtrl->SetPos(0);

   ResetDownloadState();

   //refresh the dialog
   Invalidate();
}

/*----------------------------------------------------------------------------*
*  NAME
*      DisableControls
*
*  DESCRIPTION
*      Disable the controls.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::DisableControls()
{
   CListCtrl* pDevCtrl = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   pDevCtrl->EnableWindow(FALSE);

   CButton *button = NULL;
   button = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
   button->EnableWindow(FALSE);

   button = (CButton*)GetDlgItem(IDC_SELECTFILE_BUTTON);
   button->EnableWindow(FALSE);

   button = (CButton*)GetDlgItem(IDC_SCAN_BUTTON);
   button->EnableWindow(FALSE);

   button = (CButton*)GetDlgItem(IDC_REMOVE_BUTTON);
   button->EnableWindow(FALSE);
   
   //refresh the dialog
   CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
   pEdit->EnableWindow(FALSE);

   CButton* pAuthCheck = (CButton*)GetDlgItem(IDC_CHECK_AUTH);
   pAuthCheck->EnableWindow(FALSE);

   CButton* pGenKey = (CButton*)GetDlgItem(IDC_GENERATEKEY_BTN);
   pGenKey->EnableWindow(FALSE);

   Invalidate();
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleDeviceConnStatus
*
*  DESCRIPTION
*      Handle a change in the connection state
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleDeviceConnStatus(CSR_BLE_BT_ADDRESS *btAddr)
{
   CEdit   *mEdit = NULL;
   //handle the conn status message
   switch(m_pRadioInterface->GetConnectionState(btAddr))
   {
   case DEV_IDLE:
      //current conn state is idle
      {
         //remote device mode 
         ResetConnection();
      }
      break;

   case DEV_CONNECTING:
      //current conn state is connecting
      //Disable the Scan button as currently in connecting state
      // TODO

      break;

   case DEV_CONNECTED:
      //current conn state is connected
      // TODO
      if(connectionTimer != 0)
      {
         KillTimer(connectionTimer);
         connectionTimer = 0;
      }
      m_DownLoadInProgress = TRUE;
      m_pRadioInterface->OpenImageFileForUpdate();
      
      break;

   case DEV_DISCONNECTING:
      //current conn state is disconnecting
      //TODO : Can handle disconnecting state changes in the app
      break;

   case DEV_CANCEL_CONNECTING:
      //current conn state is cancel connecting
      //TODO : Can handle cancel connecting state changes in the app
      break;

   default:
      break;
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnBnClickedImageDownload
*
*  DESCRIPTION
*      On starting download, disable all controls, stop the scan...
*      connect, once connection is established, verify if the device supports ota
*      else disconnect. If ota is supported, start the download...
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnBnClickedImageDownload()
{
   StopScan();

   if(PathFileExists(m_PathName) == FALSE)
   {
      CString logMsg;
      logMsg.LoadString(IDS_OTA_FILE_MISSING);
      UiMsg::LogMessage(logMsg, ERROR_TYPE);
      return;
   }

   if(m_pRadioInterface->ValidateImageFile() == false)
   {
      CString logMsg;
      logMsg.LoadString(IDS_OTA_FILE_MISSING);
      UiMsg::LogMessage(logMsg, ERROR_TYPE);
      return;
   }
   DisableControls();

  //Share the secret key
   if(mAesAvailable == TRUE && m_AuthEnabled)
   {
      CEdit* pSecret = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
      WCHAR pBuff[256] = {0};
      pSecret->GetWindowText(pBuff, 256);
      CString strSecret(pBuff); 
      if(strSecret.IsEmpty() == FALSE)
      {
         m_pRadioInterface->ConfigureChallengeResponse(TRUE);
         unsigned char secret[256] = {0};

         WCHAR *ptr = &pBuff[0];
         for(int i = 0; i < 16; ++i)
         {
            WCHAR temp[2] = {0};
            temp[0] = *ptr++;
            temp[1] = *ptr++;
            secret[i] = (UCHAR)wcstoul(temp, NULL, 16);
         }
         Aes::SetSharedSecret(secret);
      }
      else
      {
           m_pRadioInterface->ConfigureChallengeResponse(FALSE);
      }
   }
   else
   {
      m_pRadioInterface->ConfigureChallengeResponse(FALSE);
   }

   CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
   pCtrl->EnableWindow(TRUE);

   CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   POSITION Pos = pList->GetFirstSelectedItemPosition();
   if(Pos != NULL)
   {
      int index = pList->GetNextSelectedItem(Pos);
      SEARCH_RESULT_T *searchRes = (SEARCH_RESULT_T*)pList->GetItemData(index);
      m_pRadioInterface->ToggleConnection(&searchRes->DevAddr);
      connectionTimer = SetTimer(OTA_CONNECTION_TIMER, OTA_CONNECTION_TIMEOUT_INTERVAL, 0); 
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBnClickedRemoveDevice
*
*  DESCRIPTION
*      Remove the paired device.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnBnClickedRemoveDevice()
{
   CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   POSITION Pos = pList->GetFirstSelectedItemPosition();
   if(Pos != NULL)
   {
      int index = pList->GetNextSelectedItem(Pos);
      SEARCH_RESULT_T *searchRes = (SEARCH_RESULT_T*)pList->GetItemData(index);
      m_pRadioInterface->BleRemoveBond(&searchRes->DevAddr);
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleDeviceBonded
*
*  DESCRIPTION
*      Add the supplied address to the list of bonded devices.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleDeviceBonded(LPVOID Msg)
{
   SEARCH_RESULT_T *searchRes = (SEARCH_RESULT_T*)Msg;
   CString strAddress;
   typedAddressToString((CSR_BLE_BLUETOOTH_ADDRESS*)&searchRes->DevAddr, &strAddress);
   if( IsItemPresent(&strAddress) == false)
   {
      LVITEM lvItem;
      int nItem;
      CListCtrl *scanDeviceList = (CListCtrl *)GetDlgItem(IDC_DEVICE_LIST);
      //searchRes->DeviceName.GetBuffer();
      lvItem.mask = LVIF_TEXT;
      lvItem.iItem = 0;
      lvItem.iSubItem = 0;
      lvItem.pszText = (LPWSTR)strAddress.GetBuffer(strAddress.GetLength()+1);
      nItem = scanDeviceList->InsertItem(&lvItem);
      scanDeviceList->SetItemText(nItem, 1, (LPWSTR)searchRes->DeviceName.GetBuffer(searchRes->DeviceName.GetLength()+1));
      searchRes->DeviceName.ReleaseBuffer();

      scanDeviceList->SetItemData(nItem,(DWORD_PTR)searchRes); 
      ATLTRACE("INSERT SEARCH_RESULT = 0x%x\r\n", searchRes);
   }
   else
   {
      ATLTRACE("DELETE SEARCH_RESULT = 0x%x\r\n", searchRes);
      delete searchRes;
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      HandleDeviceDebonded
*
*  DESCRIPTION
*      When the pairing with the device is removed.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleDeviceDebonded(LPVOID Msg)
{
   SEARCH_RESULT_T *searchRes = (SEARCH_RESULT_T*)Msg;
   CString strAddress;
   typedAddressToString((CSR_BLE_BLUETOOTH_ADDRESS*)&searchRes->DevAddr, &strAddress);
   if( IsItemPresent(&strAddress) == true)
   {
      CListCtrl *scanDeviceList = (CListCtrl *)GetDlgItem(IDC_DEVICE_LIST);
      int index = 0;
      int Count = scanDeviceList->GetItemCount();
      while(index < Count)
      {
         SEARCH_RESULT_T *localRes = (SEARCH_RESULT_T*)scanDeviceList->GetItemData(index++);

         if((searchRes->DevAddr.lAp == localRes->DevAddr.lAp) &&
            (searchRes->DevAddr.nAp == localRes->DevAddr.nAp) &&
            (searchRes->DevAddr.uAp == localRes->DevAddr.uAp))
         {
            delete localRes;
            break;
         }
      }
      scanDeviceList->DeleteItem(index-1);
   }
   else
   {
      delete searchRes;
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleDownloadStatus
*
*  DESCRIPTION
*      Handle a change in the image transfer state
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleDownloadStatus(LPVOID MsgData)
{
   PROGRESS_STATUS_T *downloadStatus = (PROGRESS_STATUS_T*)MsgData;
   CButton *cBtn; 
   CProgressCtrl *pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
  
   if(downloadStatus->status == DOWNLOADING)
   {
      if(!mStartTimeRecorded)
      {
         //record initial time
         mStartTimeRecorded = true;
         mStartTime = CTime::GetCurrentTime();
         //enable pause download button
      }

      //disable "download" button
      cBtn = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
      cBtn->EnableWindow(FALSE);

      pCtrl->SetPos(downloadStatus->progress);
   }
   else if(downloadStatus->status == DOWNLOAD_COMPLETE)
   {
      //download image complete
      //calculate the totol download time
      CTime endTime = CTime::GetCurrentTime();
      CTimeSpan elapsedTime = endTime - mStartTime;

      //set to 100% progress bar
      pCtrl->SetPos(100);

      //update the downloading message
      CString time(TEXT(""));

      if(elapsedTime.GetMinutes() > 0)
      {
         if(elapsedTime.GetSeconds() > 0)
         {
            time.Format(TEXT("Downloading completed in %ld minutes and %ld seconds. Waiting for remote device to disconnect..."),
               elapsedTime.GetMinutes(),
               elapsedTime.GetSeconds());
         }
         else
         {
            time.Format(TEXT("Downloading completed in %ld minutes. Waiting for remote device to disconnect..."),\
               elapsedTime.GetSeconds());
         }
      }
      else
      {
         time.Format(TEXT("Downloading completed in %ld seconds. Waiting for remote device to disconnect..."),\
            elapsedTime.GetSeconds());
      }

      CString logMsg;
      logMsg.LoadString(IDS_OTA_DNLOAD_SUCCESS);
      UiMsg::LogMessage(logMsg, STATUS_TYPE);

      ResetDownloadState();
   }
   else if(downloadStatus->status == DOWNLOAD_CANCELLED)
   {
	  CString logMsg;
	  logMsg.Format(IDS_OTA_DNLOAD_CANCELLED);
	  UiMsg::LogMessage(logMsg,STATUS_TYPE);
      ResetDownloadState();
   }
   else if(downloadStatus->status == DOWNLOAD_PAUSED)
   {
    
   }
   else if(downloadStatus->status == ERR_IMAGE_TRANSFER_FAILED)
   {
	  CString logMsg;
	  logMsg.Format(IDS_IMAGE_TRANSFER_FAILED, downloadStatus->status);
	  UiMsg::LogMessage(logMsg, ERROR_TYPE);
	  ResetDownloadState();
   }	
   else if(downloadStatus->status == ERR_IMAGE_TRANSFER_ABORTED)
   {
	  CString logMsg;
	  logMsg.Format(IDS_IMAGE_TRANSFER_ABORTED, downloadStatus->status);
	  UiMsg::LogMessage(logMsg, ERROR_TYPE);
	  ResetDownloadState();
   }      
   else
   {
      CString logMsg;
	  logMsg.Format(IDS_FAILURE_UNKNOWN_REASON);
      UiMsg::LogMessage(logMsg, ERROR_TYPE);
      ResetDownloadState();
   }
   delete downloadStatus;
}

/*----------------------------------------------------------------------------*
*  NAME
*      HandleSetOtaModeRsp
*
*  DESCRIPTION
*      Handle the response to trying to switch the connected device into OTA mode.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleSetOtaModeRsp(LPVOID Msg)
{
   int *res = (int*)Msg;
   if(*res)
   {
      CString logString;
      logString.LoadString(IDS_OTA_SETMODE_ERROR);
      UiMsg::LogMessage(logString, ERROR_TYPE);
   }
   delete res;
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleSetCurAppRsp
*
*  DESCRIPTION
*      Handle the response to trying to change the active application on the 
*      connected device.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleSetCurAppRsp(LPVOID Msg)
{
   int *res = (int*)Msg;

   if((*res) != 0)    /* 0 indicates success */
   {
      CString logString ;
      logString.Format(IDS_OTA_SETCURAPP_ERROR, *res);
      UiMsg::LogMessage(logString, ERROR_TYPE);
   }

   delete Msg;
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleReadCurAppRsp
*
*  DESCRIPTION
*      Handle the reponse to trying to read the current active application.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleReadCurAppRsp(LPVOID Msg)
{
   unsigned char *currentApp = (unsigned char*)Msg;
   CString logString;
   logString.Format(IDS_OTA_CURAPP, *currentApp);
   UiMsg::LogMessage(logString, INFO_TYPE);
   delete Msg;
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleRemoteApplicationMode
*
*  DESCRIPTION
*      Update the display now that we know whether the connected device is running
*      the boot-loader or a device application.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleRemoteApplicationMode(LPVOID MsgData)
{
   unsigned char *mode = (unsigned char*)MsgData;
   CButton *button = NULL;
   if(*mode == OTA_APPLICATION_MODE)
   {
      mRemoteDeviceMode = OTA_APPLICATION_MODE;
      CString logMsg;
      logMsg.LoadString(IDS_OTA_APP_SUPPORTED);
      UiMsg::LogMessage(logMsg, STATUS_TYPE);
   }
   else if(*mode == OTA_BOOTLOADER_MODE)
   {
      mRemoteDeviceMode = OTA_BOOTLOADER_MODE;
      CString logMsg;
      logMsg.LoadString(IDS_OTA_BOOTLDR_SUPPORTED);
      UiMsg::LogMessage(logMsg, STATUS_TYPE);
   }
   else
   {
      CString logMsg;
      logMsg.LoadString(IDS_OTA_NOT_SUPPORTED);
      UiMsg::LogMessage(logMsg, ERROR_TYPE);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      HandleChallengeResponseStatus
*
*  DESCRIPTION
*      Alert the user if the challenge-response mechanism failed.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleChallengeResponseStatus(LPVOID Msg)
{
   int *result = (int*)Msg;

   if(*result == CHALLENGE_RESPONSE_FAILED)
   {
      CString logMsg;
      logMsg.LoadString(IDS_OTA_CR_CFM_ERROR);
      UiMsg::LogMessage(logMsg, ERROR_TYPE);
   }

   delete Msg;
}


/*----------------------------------------------------------------------------*
*  NAME
*      HandleVersionResponse
*
*  DESCRIPTION
*      Show the boot-loader version to the user
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleVersionResponse(LPVOID Msg)
{
   unsigned char version = *(unsigned char*)Msg;
   CString logMsg;

   logMsg.Format(IDS_OTA_BLDR_VERSION, version);
   UiMsg::LogMessage(logMsg, INFO_TYPE);

   delete Msg;
}


/*----------------------------------------------------------------------------*
*  NAME
*      ResetDownloadState
*
*  DESCRIPTION
*      Reset the image-transfer record.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::ResetDownloadState(void)
{
   // reset flag
   mStartTimeRecorded = false;

   CListCtrl *pDevCtrl = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   pDevCtrl->EnableWindow(TRUE);

   POSITION Pos = pDevCtrl->GetFirstSelectedItemPosition();
   CButton *button = NULL;
   if(Pos != NULL)
   {
      if(m_PathName.IsEmpty() == FALSE)
      {
         //show connect button
         button = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
         button->EnableWindow(TRUE);
      }
      button = (CButton*)GetDlgItem(IDC_REMOVE_BUTTON);
      button->EnableWindow(TRUE);
   }

   if(mAesAvailable)
   {
      CButton* pAuthCheck = (CButton*)GetDlgItem(IDC_CHECK_AUTH);
      pAuthCheck->EnableWindow(TRUE);

      CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
      CButton* pGenKey = (CButton*)GetDlgItem(IDC_GENERATEKEY_BTN);
      
      if(m_AuthEnabled)
      {
         pEdit->EnableWindow(TRUE);
         pGenKey->EnableWindow(TRUE);
      }
   }

   m_pRadioInterface->CloseImageFile();
   m_DownLoadInProgress = FALSE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      IsItemPresent
*
*  DESCRIPTION
*      Determine whether the supplied device address is already displayed in the
*      list of devices found during a device scan.
*---------------------------------------------------------------------------*/
bool COTAUpdateToolDlg::IsItemPresent(CString *DevAddr)
{
   CString strText;
   CListCtrl *scanDeviceList = (CListCtrl *)GetDlgItem(IDC_DEVICE_LIST);

   for(int row = 0; row < scanDeviceList->GetItemCount(); row++)
   {
      strText = scanDeviceList->GetItemText(row,0);

      if(!strText.Compare(*DevAddr))
      {
         return true;
      }
   }
   return false;
}


/*----------------------------------------------------------------------------*
*  NAME
*      StringToBdAddr
*
*  DESCRIPTION
*      Convert a bluetooth-address-as-a-string into a address structure.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::StringToBdAddr(TCHAR *StrAddr, CSR_BLE_BLUETOOTH_ADDRESS *BluetoothAddress)
{
   CString strText;

   //get initial 6 chars ie nap, eg: 0x0002
   strText = CString(StrAddr, 6);

   //assign nap
   BluetoothAddress->nAp = (WORD) _tcstol(strText.GetBuffer(6), NULL, 16);

   //increament the temp pointer by 6 chars
   StrAddr = StrAddr + 6;

   //get the next 2 chars ie uap, eg: 0x5b
   strText = CString(StrAddr, 2);

   //assign uap
   BluetoothAddress->uAp = (BYTE)_tcstol(strText.GetBuffer(2), NULL, 16);

   //increament the temp pointer by 2 chars
   StrAddr = StrAddr + 2;

   //get the next 6 chars ie lap, eg: 0x03c313
   strText = CString(StrAddr, 6);

   //assign lap
   BluetoothAddress->lAp = (DWORD)_tcstol(strText.GetBuffer(6), NULL, 16);
}



/*----------------------------------------------------------------------------*
*  NAME
*      GetImgFileName
*
*  DESCRIPTION
*      This method returns the image file name from the ui edit box
*---------------------------------------------------------------------------*/
CString COTAUpdateToolDlg::GetImgFileName()
{
   //temp variables
   TCHAR   atchBuffer[1024] = {0};  
   int     nChrCount = 0;
  

   //Read Image Path 
   CEdit  *cEdit = (CEdit*)GetDlgItem(IDC_IMAGE_PATH);
   nChrCount = cEdit->GetLine(0, atchBuffer, 1024);

   if(nChrCount == 0)
   {
      CString title, message;
      title.LoadString(IDS_APP_TITLE);
      message.LoadString(IDS_OTA_SELECT_IMAGEFILE);
      MessageBox(message, title);
   }

   //convert to string
   return CString(atchBuffer, nChrCount);
}

/*----------------------------------------------------------------------------*
*  NAME
*      ReceiveOtaUpdateUIMsg
*
*  DESCRIPTION
*      Handler for General Dialog Messages.
*      Parses the message types ans routes them to corresponding handlers.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::ReceiveOtaUpdateUIMsg(LPARAM l_param)
{
   UPDATE_UI_MSG_T *OtaUIMsg = (UPDATE_UI_MSG_T *)l_param;
 //  CButton *pButton;
   CString logString;

   switch(OtaUIMsg->MsgId)
   {
   case SEARCH_RESULT:
      {
         //search result messge received
         SEARCH_RESULT_T *searchRes = (SEARCH_RESULT_T*)OtaUIMsg->MsgData;
		 int nItem;
         CString strAddress;
         typedAddressToString((CSR_BLE_BLUETOOTH_ADDRESS*)&searchRes->DevAddr, &strAddress);
         if( IsItemPresent(&strAddress) == false)
         {
            LVITEM lvItem;            
            CListCtrl *scanDeviceList = (CListCtrl *)GetDlgItem(IDC_DEVICE_LIST);
            //searchRes->DeviceName.GetBuffer();
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = 0;
            lvItem.iSubItem = 0;
            lvItem.pszText = (LPWSTR)strAddress.GetBuffer(strAddress.GetLength()+1);
            nItem = scanDeviceList->InsertItem(&lvItem);
            scanDeviceList->SetItemText(nItem, 1, (LPWSTR)searchRes->DeviceName.GetBuffer(searchRes->DeviceName.GetLength()+1));
            searchRes->DeviceName.ReleaseBuffer();

            scanDeviceList->SetItemData(nItem,(DWORD_PTR)searchRes); 
            ATLTRACE("INSERT SEARCH_RESULT = 0x%x\r\n", searchRes);
         }
         else
         {
			 LVFINDINFO info;	 		 
			 info.flags = LVFI_PARTIAL|LVFI_STRING;
			 info.psz = (LPCWSTR)strAddress.GetBuffer(strAddress.GetLength()+1);
			 CListCtrl *scanDeviceList = (CListCtrl *)GetDlgItem(IDC_DEVICE_LIST);
			 nItem = scanDeviceList->FindItem(&info);

			 scanDeviceList->SetItemText(nItem, 1, (LPWSTR)searchRes->DeviceName.GetBuffer(searchRes->DeviceName.GetLength()+1));
			 ATLTRACE("DELETE SEARCH_RESULT = 0x%x\r\n", searchRes);
             delete searchRes;
         }
      }
      break;

   case DEVICE_CONNECTION_UPDATE:
      {
         //conn state messge received
         CSR_BLE_BT_ADDRESS *btAddress = (CSR_BLE_BT_ADDRESS*)OtaUIMsg->MsgData;
         HandleDeviceConnStatus(btAddress);
         delete btAddress;
      }
      break;

   case DEBONDED_DEVICE:
      HandleDeviceDebonded(OtaUIMsg->MsgData);
      break;

   case BONDED_DEVICE:
      HandleDeviceBonded(OtaUIMsg->MsgData);
      break;

   case HW_ERR:
      //hardware error messge received
      //show hardware error message
      logString.LoadString(IDS_APP_HW_NOTREADY);
      UiMsg::LogMessage(logString, ERROR_TYPE);
      break;

   case HW_READY:
      logString.LoadString(IDS_APP_HW_READY);
      UiMsg::LogMessage(logString, INFO_TYPE);
      m_IsStartingUp = FALSE;
      break;

   case SEC_ERR:
      //security error messge received
      //show security error message
      logString.LoadString(IDS_OTA_ENCR_ERROR);
      UiMsg::LogMessage(logString, ERROR_TYPE);
      break;

   case AES_STATUS:
      if(OtaUIMsg->MsgData == FALSE)
      {
         logString.LoadString(IDS_OTA_AES_DLLNOTFOUND);
         UiMsg::LogMessage(logString, WARN_TYPE);

         mAesAvailable = FALSE;
         CEdit* pSecret = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
         pSecret->EnableWindow(FALSE);

         CButton* pAuthCheck = (CButton*)GetDlgItem(IDC_CHECK_AUTH);
         pAuthCheck->EnableWindow(FALSE);

         CButton* pGenKey = (CButton*)GetDlgItem(IDC_GENERATEKEY_BTN);
         pGenKey->EnableWindow(FALSE);
      }
      else
      {
         logString.LoadString(IDS_OTA_AES_DLLFOUND);
         UiMsg::LogMessage(logString, INFO_TYPE);

         mAesAvailable = TRUE;
      }
      break;

   case PROGRESS_STATUS:
      //scan state messge received
      HandleDownloadStatus(OtaUIMsg->MsgData);
      break;

   case SET_OTA_MODE_RSP:
      HandleSetOtaModeRsp(OtaUIMsg->MsgData);
      break;

   case BT_ADDRESS_READ_FAILED:
      logString.LoadString(IDS_OTA_READ_BDADDRERROR);
      UiMsg::LogMessage(logString, ERROR_TYPE);
      break;

   case SET_CUR_APP_RSP:
      HandleSetCurAppRsp(OtaUIMsg->MsgData);
      break;

   case READ_CUR_APP_RSP:
      HandleReadCurAppRsp(OtaUIMsg->MsgData);
      break;

   case REMOTE_APPLICATION_MODE:
      HandleRemoteApplicationMode(OtaUIMsg->MsgData);
      break;

   case CHALLENGE_RESPONSE_STATUS:
      HandleChallengeResponseStatus(OtaUIMsg->MsgData);
      break;

   case READ_VERSION_RESPONSE:
      HandleVersionResponse(OtaUIMsg->MsgData);
      break;

   case OTA_READY:
      if(mRemoteDeviceMode == OTA_BOOTLOADER_MODE)
      {
         HandleStartTransfer();
      }
      break;

   case BT_CURAPP_READ_FAILED:
      logString.LoadString(IDS_OTA_READ_CURAPPERROR);
      UiMsg::LogMessage(logString, ERROR_TYPE);
      break;

   case OTA_MERGE_CSKEY_FAILED:
      logString.LoadString(IDS_OTA_IMAGE_MERGE_FAILED);
      UiMsg::LogMessage(logString, ERROR_TYPE);
      ResetDownloadState();
      break;

   default:
      break;

   }
   delete OtaUIMsg;
}

/*----------------------------------------------------------------------------*
*  NAME
*      HandleStartTransfer
*
*  DESCRIPTION
*      Handles tranfer callbacks
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::HandleStartTransfer()
{
   CString logString;
   switch(m_pRadioInterface->StateGetOta())
   {
   case STATE_OTA_RESET_TRANSFER_CTRL:
      logString.LoadString(IDS_OTA_RESET_DEVICE);
      UiMsg::LogMessage(logString, INFO_TYPE);
      break;

   case STATE_OTA_PAUSE_DATA_TRANSFER:
      // Tell the remote device that the image transfer is continuing
      m_pRadioInterface->OtaSetTransferControlReq();
      break;

   case STATE_OTA_DATA_TRANSFER:
   case STATE_OTA_ABORT_DATA_TRANSFER:
      // We can ignore this press
      break;

   case STATE_OTA_IDLE:
      {
         // Tell the remote device that the image transfer is starting
         m_pRadioInterface->OtaSetTransferControlReq();
         logString.LoadString(IDS_OTA_DNLOAD_START);
         UiMsg::LogMessage(logString, INFO_TYPE);
         DisableControls();
      }
      break;
   default:
      break;
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnDialogMessage
*
*  DESCRIPTION
*      Handler for Dialog Messages 
*      Parses the message and routes to the corresponding UI objects
*---------------------------------------------------------------------------*/
LRESULT COTAUpdateToolDlg::OnDialogMessage(WPARAM w_param, LPARAM l_param)
{
   struct tm timeStruct;
   time_t now;
   //CListBox *loggingList;
   CString logMessage;
   CString completeMessage;
   CString timeMessage;
   LOGGING_MSG_T *loggingMessage;

   //handle UI messages
   switch(w_param)
   {
   case OTA_MSG:
      //general ui message received
      ReceiveOtaUpdateUIMsg(l_param);
      break;

   case LOGGING_MSG:
      // Record the current time
      time(&now);
      // Split the time into useful fields
      gmtime_s(&timeStruct, &now);
      // Get a pointer to the logging list
      //loggingList = (CListBox *)GetDlgItem(IDC_LOGGING_LIST);
      // This is the message that has been supplied:
      loggingMessage = (LOGGING_MSG_T*) l_param;
      logMessage = loggingMessage->logMessage;
      // Create a string containing both time and message
     
      //timeMessage.fo
      if(loggingMessage->msgType == STATUS_TYPE ||
         loggingMessage->msgType == INFO_TYPE)
      {
         completeMessage.Format(TEXT("Status: %s"), logMessage);
      }
      else if(loggingMessage->msgType == ERROR_TYPE)
      {
         completeMessage.Format(TEXT("Error: %s"), logMessage);
      } 
      else if(loggingMessage->msgType == WARN_TYPE)
      {
         completeMessage.Format(TEXT("Warning: %s"), logMessage);
      }
      else
      {
          completeMessage.Format(TEXT("Unknown: %s"), logMessage);
      }

      if(m_AutoLog && m_LogFile.m_pStream != NULL)
      {
         timeMessage.Format(_T("%02d:%02d:%02d  %s\r\n"), timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, completeMessage.GetBuffer(0));
         m_LogFile.WriteString(timeMessage); 
      }

      m_LogListBox.InsertString(-1, completeMessage);
      m_LogListBox.SetTopIndex(m_LogListBox.GetCount()-1);

      SetTextExtent(completeMessage);

      //delete logMessage;
      delete loggingMessage;
      break;
   }
   return 0;
}


/*----------------------------------------------------------------------------*
*  NAME
*      SetTextExtent
*
*  DESCRIPTION
*      Enables horizontal scroll based on text length
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::SetTextExtent(CString strText)
{
   CSize      sz;
   int        dx = 0;
   TEXTMETRIC tm;
   CDC*       pDC = m_LogListBox.GetDC();
   CFont*     pFont = m_LogListBox.GetFont();

   // Select the listbox font, save the old font
   CFont* pOldFont = pDC->SelectObject(pFont);
   // Get the text metrics for avg char width
   pDC->GetTextMetrics(&tm); 

   //for (int i = 0; i < m_LogListBox.GetCount(); i++)
   {
      //m_LogListBox.GetText(i, str);
      sz = pDC->GetTextExtent(strText);

      // Add the avg width to prevent clipping
      sz.cx += tm.tmAveCharWidth;

      if (sz.cx > dx)
         dx = sz.cx;
   }
   // Select the old font back into the DC
   pDC->SelectObject(pOldFont);
   m_LogListBox.ReleaseDC(pDC);

   // Set the horizontal extent so every character of all strings  
   // can be scrolled to.
   m_LogListBox.SetHorizontalExtent(dx);
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnGetDefID
*
*  DESCRIPTION
*      * Handler for Return presses
*---------------------------------------------------------------------------*/
LRESULT COTAUpdateToolDlg::OnGetDefID(WPARAM wp, LPARAM lp) 
{
   return MAKELONG(0,DC_HASDEFID); 
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnSysCommand
*
*  DESCRIPTION
*      Handler for General Dialog Messages.
*      Parses the message types ans routes them to corresponding handlers.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnPaint
*
*  DESCRIPTION
*      If you add a minimize button to your dialog, you will need the code below
*      to draw the icon.  For MFC applications using the document/view model,
*      this is automatically done for you by the framework.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      OnQueryDragIcon
*
*  DESCRIPTION
*      The system calls this function to obtain the cursor to display while 
*      the user drags the minimized window.
*---------------------------------------------------------------------------*/
HCURSOR COTAUpdateToolDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

/*----------------------------------------------------------------------------*
*  NAME
*      OnClose
*
*  DESCRIPTION
*      
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnClose()
{
   if (CanExit())
   {
      m_DownLoadInProgress = FALSE;
      OnExit();
      CDialog::OnClose();
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnOK
*
*  DESCRIPTION
*      
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnOK()
{
   if (CanExit())
   {
      m_DownLoadInProgress = FALSE;
      OnExit();
      CDialog::OnOK();
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnCancel
*
*  DESCRIPTION
*      
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnCancel()
{
   if (CanExit())
   {
      m_DownLoadInProgress = FALSE;
      OnExit(TRUE);
      CDialog::OnCancel();
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      CanExit
*
*  DESCRIPTION
*     
*---------------------------------------------------------------------------*/
BOOL COTAUpdateToolDlg::CanExit()
{
   // If the proxy object is still around, then the automation
   //  controller is still holding on to this application.  Leave
   //  the dialog around, but hide its UI.
   if (m_pAutoProxy != NULL)
   {
      ShowWindow(SW_HIDE);
      return FALSE;
   }

   if(m_DownLoadInProgress == TRUE)
   {
      CString title, message;
      title.LoadString(IDS_APP_TITLE);
      message.LoadString(IDS_OTA_DOWNLOADMSG);
      int result = MessageBox(message, title, MB_ICONWARNING|MB_YESNO);
      if(result != IDYES)
      {
         return FALSE;
      }
   }

   return TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnExit
*
*  DESCRIPTION
*      Handler for General Dialog Messages.
*     
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnExit(BOOL bCancel /*= FALSE*/)
{
   if(bCancel == TRUE)
   {
      HKEY lhKey;
      LSTATUS stat;
      if ((stat = RegOpenKeyEx(HKEY_CURRENT_USER, m_RegPath, 0, KEY_ALL_ACCESS, &lhKey)) != ERROR_SUCCESS)
      {
         stat = RegCreateKeyEx(HKEY_CURRENT_USER, m_RegPath, 0, 0, 0, KEY_ALL_ACCESS, 0, &lhKey, 0);
      }

      if(ERROR_SUCCESS == stat)
      {
         DWORD len = m_PathName.GetLength() + 1;
         DWORD type = REG_SZ;
         WCHAR *pBuff = m_PathName.GetBuffer(len);
         RegSetValueEx(lhKey, __T("Image Path"), 0, REG_SZ, (LPBYTE)pBuff, len*sizeof(WCHAR));

         DWORD dwAutoLog = m_AutoLog;
         len = sizeof(DWORD) ;
         type = REG_DWORD;
         RegSetValueEx(lhKey, __T("LogToText"), 0, REG_DWORD, (LPBYTE)&dwAutoLog, len);

         DWORD dwLogSynergy = m_SynergyLog;
         len = sizeof(DWORD) ;
         type = REG_DWORD;
         RegSetValueEx(lhKey, __T("LogSynergy"), 0, REG_DWORD, (LPBYTE)&dwLogSynergy, len);

         WCHAR secret_key[MAX_PATH];
         CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
         pEdit->GetWindowText(secret_key, MAX_PATH);

         len = MAX_PATH;
         type = REG_SZ;
         RegSetValueEx(lhKey, __T("SecretKey"), 0, REG_SZ, (LPBYTE)secret_key, len);

         DWORD dwChalResp = m_AuthEnabled;
         len = sizeof(DWORD) ;
         type = REG_DWORD;
         RegSetValueEx(lhKey, __T("Challenge-Response"), 0, REG_DWORD, (LPBYTE)&dwChalResp, len);
        
         RegCloseKey(lhKey);
      }
   }

   if(m_AutoLog && m_LogFile.m_pStream != NULL)
   {
      m_LogFile.Close();
   }
   StopScan();
   ClearDeviceList();

}


/*----------------------------------------------------------------------------*
*  NAME
*      OnBleMessage
*
*  DESCRIPTION
*      Callback for BLE events. 
*      This function routes the events to the BleRadio
*---------------------------------------------------------------------------*/
LRESULT COTAUpdateToolDlg::OnBleMessage(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case CSR_BLE_HOST_SECURITY_RESULT:
	{
		if(m_pRadioInterface->m_CurBleDevice != NULL)
		{
			PCSR_BLE_SECURITY_RESULT secRes = (PCSR_BLE_SECURITY_RESULT)lParam;

			if(secRes->result != CSR_BLE_SECURITY_BONDING_ESTABLISHED)
			{
				m_pRadioInterface->SetMITMStatus(FALSE);

				if(pUserPrompt != NULL)
				{
					if(usrDlgCurrentUsage == UserDialogUsageMitmPasskeyDisplay)
					{
						pUserPrompt->OnBnClickedCancel();
						pUserPrompt = NULL;
						
						if((m_pRadioInterface->m_nMitmAttempts) < MAX_MITM_ATTEMPTS)
						{
							m_pRadioInterface->SetMITMStatus(TRUE);
						}
						++(m_pRadioInterface->m_nMitmAttempts);
					}
				}
			}
			else
			{
				if(pUserPrompt != NULL)
				{
					if(usrDlgCurrentUsage == UserDialogUsageMitmPasskeyDisplay)
					{
						pUserPrompt->OnOK();
						pUserPrompt = NULL;
					}
				}
			}
		}
    }
	break;
	case CSR_BLE_HOST_DISPLAY_PASSKEY_IND:
	{	
			// pairing request
			PCSR_BLE_DISPLAY_PASSKEY_IND passkey = (PCSR_BLE_DISPLAY_PASSKEY_IND)lParam;
			CsrBleHostPasskeyNotificationResult(TRUE, passkey->deviceAddress);
           /* Create the user prompt.
		    * Display dialog box with the passkey.
			*/
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
			lf.lfHeight = 48;                      // request a 24-pixel-height font
			lf.lfWeight = FW_BOLD;
			_tcscpy_s(lf.lfFaceName,LF_FACESIZE,_T("Arial Black"));
			// Update the edit box to display the passkey.
			CString passkeyString;
			passkeyString.Format(_T("%06d"), passkey->numericValue);
			CUserPromptDlg userPrompt(IDS_PASSKEY_HEADING,
				IDS_PASSKEY_DISPLAY_MESSAGE,
				passkeyString, lf, true, FALSE, TRUE, this);
			pUserPrompt = &userPrompt;
			
			usrDlgCurrentUsage = UserDialogUsageMitmPasskeyDisplay;
			
			if(userPrompt.DoModal() == IDCANCEL)
			{
				CsrBleHostCancelBonding(passkey->deviceAddress);
			}
	}
	break;

	case CSR_BLE_HOST_DISCONNECTED:
	{
		if(pUserPrompt != NULL)
		{
			if(usrDlgCurrentUsage == UserDialogUsageMitmPasskeyDisplay)
			{
				pUserPrompt->OnBnClickedCancel();
				pUserPrompt = NULL;
			}
		}
		usrDlgCurrentUsage = UserDialogUsageNone;
    }
	break;

	default:
		break;
	}

   //handle all the radio events in BleRadio class
   m_pRadioInterface->BLEProcessMessage(wParam, lParam);

   return (LRESULT)0;
}

/*----------------------------------------------------------------------------*
*  NAME
*      ReceiveOtaUpdateUIMsg
*
*  DESCRIPTION
*      Handler for General Dialog Messages.
*      Parses the message types ans routes them to corresponding handlers.
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::ClearDeviceList()
{
   CListCtrl* pDevList = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   int Count = pDevList->GetItemCount();
   int index = 0;
   while (index < Count)
   {
      SEARCH_RESULT_T *pData = (SEARCH_RESULT_T*)pDevList->GetItemData(index);
      delete pData;
      index++;
   }
   pDevList->DeleteAllItems();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      OnBnClickedButtonDeviceScan
 *
 *  DESCRIPTION
 *      Handles the user selecting the "scan" button
 *---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnBnClickedButtonDeviceScan()
{
   // request initiate scanning
   ToggleScan();   
}

/*----------------------------------------------------------------------------*
*  NAME
*      ToggleScan
*
*  DESCRIPTION
*      Toggle scan
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::ToggleScan()
{
   CButton* button = (CButton*)GetDlgItem(IDC_SCAN_BUTTON);
   if(m_pRadioInterface->GetRadioState() == APP_IDLE)
   {
      //clear the items in the list; 
      ClearDeviceList();

      m_pRadioInterface->BleStartScan();
      button->SetWindowText(L"Stop S&canning");
      CButton* button = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
      button->EnableWindow(FALSE);

      button = (CButton*)GetDlgItem(IDC_REMOVE_BUTTON);
      button->EnableWindow(FALSE);

      CString logString;
      logString.LoadString(IDS_APP_SCANNING);
      UiMsg::LogMessage(logString, STATUS_TYPE);
   }
   else
   {
      StopScan();
   }
}
/*----------------------------------------------------------------------------*
*  NAME
*      StopScan
*
*  DESCRIPTION
*      Stop scan
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::StopScan()
{
    if(m_pRadioInterface->GetRadioState() == APP_SCANNING)
    {
       CButton* button = (CButton*)GetDlgItem(IDC_SCAN_BUTTON);
       
       CString logString;
       logString.LoadString(IDS_APP_STOPSCAN);
       UiMsg::LogMessage(logString, STATUS_TYPE);
       

       m_pRadioInterface->BleStopScan();
       button->SetWindowText(L"S&can for Device");
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnLvnItemChangedDevList
*
*  DESCRIPTION
*      Selection changed in the list box
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnLvnItemChangedDevList(NMHDR *pNMHDR, LRESULT *pResult)
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   
   TRACE2("chnage = %d, state = %d",pNMListView->uChanged,pNMListView->uNewState);
   CButton* updateBtn = (CButton*)GetDlgItem(IDC_UPDATE_BUTTON);
   CButton* removeBtn  = (CButton*)GetDlgItem(IDC_REMOVE_BUTTON);
   
   CListCtrl *pDeviceList  = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
   SEARCH_RESULT_T *searchRes = (SEARCH_RESULT_T*)pDeviceList->GetItemData(pNMListView->iItem);

   if ((pNMListView->uChanged & LVIF_STATE) 
      && (pNMListView->uNewState & LVNI_SELECTED))
   {
      if(m_PathName.IsEmpty() == FALSE)
      {
         updateBtn->EnableWindow(TRUE);
      }

      if(m_pRadioInterface->IsBonded(*(CSR_BLE_BLUETOOTH_ADDRESS*)&searchRes->DevAddr))
      {
         removeBtn->EnableWindow(TRUE);
      }
      else
      {
         removeBtn->EnableWindow(FALSE);
      }
   }
   else
   {
      updateBtn->EnableWindow(FALSE);
      removeBtn->EnableWindow(FALSE);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnBnClickedAbout
*
*  DESCRIPTION
*      
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnBnClickedAbout()
{
   CAboutDlg dlgAbout;
   dlgAbout.DoModal();
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnLogAutoLog
*
*  DESCRIPTION
*      Auto log event handler
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnLogAutoLog()
{
   // TODO: Add your command handler code here
   if(m_AutoLog == TRUE)
   {
      m_AutoLog = FALSE;
      m_LogFile.Close();
   }
   else
   {
      m_AutoLog = TRUE;

      SYSTEMTIME lt;
      GetLocalTime(&lt);
      CTime t(lt);
      CString strFileName = t.Format(__T("%d%m%y%H%M%SOTALog.log"));

      m_LogFile.Open(strFileName, CFile::modeCreate|CFile::modeReadWrite);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnLogClearlog
*
*  DESCRIPTION
*      Clear log handler
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnLogClearlog()
{
   // TODO: Add your command handler code here
   m_LogListBox.ResetContent();
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnUpdateAutoLog
*
*  DESCRIPTION
*      Auto logging menu update handler
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnUpdateAutoLog(CCmdUI* pCmdUI)
{
   if(m_AutoLog == TRUE)
   {
      pCmdUI->SetCheck(TRUE);
   }
   else
   {
      pCmdUI->SetCheck(FALSE);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnInitMenuPopup
*
*  DESCRIPTION
*      Init menu handler for pop up menu
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
   CCmdUI CmdUI;
   UINT newIndex;

   // Skip system menu from processing
   if( FALSE != bSysMenu )
      return;

   CmdUI.m_nIndexMax = pPopupMenu->GetMenuItemCount();
   for( newIndex = 0; newIndex < CmdUI.m_nIndexMax; ++newIndex )
   {
      CmdUI.m_nIndex = newIndex;
      CmdUI.m_nID = pPopupMenu->GetMenuItemID(newIndex);
      CmdUI.m_pMenu = pPopupMenu;

      CmdUI.DoUpdate( this, TRUE );
   }
} 

/*----------------------------------------------------------------------------*
*  NAME
*      OnDeviceChange
*
*  DESCRIPTION
*      On Device chnage event handler
*---------------------------------------------------------------------------*/
LRESULT COTAUpdateToolDlg::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
    // A USB device has been removed.
    if((wParam == DBT_DEVICEREMOVECOMPLETE) || (wParam == DBT_DEVTYP_DEVNODE))
    {
        // If the application itself is not re-initializing Synergy, then, 
        // handle the event.
       if(m_IsStartingUp == FALSE)
       {
          if(m_pRadioInterface->IsRadioAvailable() == FALSE)
          {
             // close the main window
             SendMessage(WM_CLOSE);
          }
       }
    }
    return 0;
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnPowerStateChange
*
*  DESCRIPTION
*      When the system power change event occuers
*---------------------------------------------------------------------------*/
LRESULT COTAUpdateToolDlg::OnPowerStateChange(WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
        // The system is about to go to a low power state(sleep/hibernation).
        case PBT_APMSUSPEND:
        {
           m_IsStartingUp = TRUE;
           m_pRadioInterface->BleDeInit();
        }
        break;

        // System is returning from a low power state(sleep/hibernation).
        case PBT_APMRESUMESUSPEND: // A PBT_APMSUSPEND was received earlier.
        case PBT_APMRESUMECRITICAL: // A PBT_APMSUSPEND wasn't received earlier.
        {        
            // start up - show delay message
            // Wait for the driver to complete it's initialization procedure. 
            // It's been observed that if the initialization request is sent 
            // immediately, BSOD is seen.
            Sleep(2000);

            // Reload the Synergy stack.
            if(m_pRadioInterface->BleInit(m_hWnd, CsrBleClientMessage, m_SynergyLog) != RADIO_INIT_SUCCESS)
            {
                SendMessage(WM_CLOSE);
            }
        }
        break;

        default:
            // Nothing to do.
        break;
    }
    return 0;
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnEnUpdateSecretkeyEdit
*
*  DESCRIPTION
*      Handler for the secret key entered by user
*---------------------------------------------------------------------------*/
void COTAUpdateToolDlg::OnEnUpdateSecretkeyEdit()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CDialog::OnInitDialog()
   // function to send the EM_SETEVENTMASK message to the control
   // with the ENM_UPDATE flag ORed into the lParam mask.

   // TODO:  Add your control notification handler code here
    m_bHandleKeyUpdateMsg =  !m_bHandleKeyUpdateMsg;
    if(m_bHandleKeyUpdateMsg)
    {

       // Check whether the newly entered text is a hexadecimal digit.
       CString newText;
       CEdit* pKeyTextBox = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);

       // Get the newly entered text.
       pKeyTextBox->GetWindowText(newText);

       // Check whether the text entered is a valid hexadecimal number. If not, 
       // write the old text back to the edit box.
       if(CheckAndUpdateHexNumber(newText))
       {
          pKeyTextBox->SetWindowText(newText);
          m_keyText = newText;
       }
       else
       {
          pKeyTextBox->SetWindowText(m_keyText);
       }

       // Move the cursor to the end of the edit box.
       pKeyTextBox->GetWindowText(newText);
       int strSize = newText.GetLength();
       pKeyTextBox->SetSel(strSize, strSize);
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      CheckAndUpdateHexNumber
*
*  DESCRIPTION
*      Check the updated the HEX value in the encrytion key entered by user
*---------------------------------------------------------------------------*/
BOOL COTAUpdateToolDlg::CheckAndUpdateHexNumber(CString& text)
{
    BOOL bValidHex = FALSE;
    BOOL bPrepend0x = FALSE;
    // CString.GetLength() returns the number of bytes used by the string. 
    // Since unicode is being used, each character will be occupying 2 bytes.
    WORD nCharInString = text.GetLength();
    WORD i;

    // If the string length is 1 and the first character is not 0, then prepend 
    // 0x to the number.
    if(nCharInString == 1)
    {
        if(IS_VALID_HEX(text[0]))
        {
            bValidHex = TRUE;

            if(text[0] != ASCII_VALUE_0)
            {
                bPrepend0x = TRUE;
            }
        }
    }
    else if(nCharInString == 2)
    {
        // Check whether the string is '0x'/'0X'.
        if(text[0] == ASCII_VALUE_0 && IS_CHAR_X(text[1]))
        {
            bValidHex = TRUE;
        }

        else // Check whether both the letters are valid hex numbers.
        {
            WORD nNumbers = 0;
            for(i = 0; i < nCharInString; i++)
            {
                if(IS_VALID_HEX(text[i]))
                {
                    nNumbers++;
                }
            }
            // If both the letters are valid hex numbers, prepend 0x to the 
            // string.
            if(nNumbers == nCharInString)
            {
                bValidHex = TRUE;
                bPrepend0x = TRUE;
            }
        }
    }

    else // Number of characters in the string is greater than 2.
    {
        // Check whether the first 2 characters are '0x'/'0X'.
        if(text[0] == ASCII_VALUE_0 && IS_CHAR_X(text[1]))
        {
            // It might be a case of copy, paste from some other place. So, 
            // ensure that the maximum limit length is adhrered to.
            if(nCharInString <= KEY_TEXT_BOX_LENGTH)
            {
                // Check whether the remaining of the characters are numbers.
                WORD nNumbers = 0;
                WORD lettersToCompare = nCharInString - 2;
                for(i = 0; i < lettersToCompare; i++)
                {
                    if(IS_VALID_HEX(text[i+2]))
                    {
                        nNumbers++;
                    }
                }
                if(nNumbers == lettersToCompare)
                {
                    bValidHex = TRUE;
                }
            }
        }

        else // The first 2 characters are not 0x/0X.
        {
            // It might be a case of copy, paste from some other place. So, 
            // ensure that the maximum limit length is adhrered to.
            if(nCharInString <= KEY_TEXT_LENGTH)
            {
                WORD nNumbers = 0;
                for(i = 0; i < nCharInString; i++)
                {
                    if(IS_VALID_HEX(text[i]))
                    {
                        nNumbers++;
                    }
                }
                if(nNumbers == nCharInString)
                {
                    bPrepend0x = TRUE;
                    bValidHex = TRUE;
                }
            }
        }
    }
    return bValidHex;
}

/*----------------------------------------------------------------------------*
*  NAME
*      PreTranslateMessage
*
*  DESCRIPTION
*      Default PretranslateMessage hadnler
*---------------------------------------------------------------------------*/
BOOL COTAUpdateToolDlg::PreTranslateMessage(MSG* pMsg)
{
   if(pMsg->message == WM_KEYDOWN)
   {
      UINT asc = pMsg->wParam;
      switch(asc)
      {
      case VK_ESCAPE:
      case VK_RETURN:
         {
            return FALSE;
         } break;
      }
   }
   return CDialog::PreTranslateMessage(pMsg);
}
void COTAUpdateToolDlg::OnEnChangeSecretkeyEdit()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CDialog::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   // TODO:  Add your control notification handler code here
}

void COTAUpdateToolDlg::OnBnClickedCheckAuth()
{
   // TODO: Add your control notification handler code here
  m_AuthEnabled = !m_AuthEnabled;

  CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
  CButton* pGenerateKey = (CButton*)GetDlgItem(IDC_GENERATEKEY_BTN);
  if(m_AuthEnabled == TRUE)
  {
     pGenerateKey->EnableWindow(TRUE);
     pEdit->EnableWindow(TRUE);
  }
  else
  {
     pGenerateKey->EnableWindow(FALSE);
     pEdit->EnableWindow(FALSE);
  }
}

void COTAUpdateToolDlg::OnBnClickedGeneratekeyBtn()
{
   // TODO: Add your control notification handler code here
   BYTE randNumbers[16];
   for(WORD i = 0; i < 16; i++)
   {
      randNumbers[i] = rand();
   }

   CString strKey("");
   // Get the network key
   for(int i = 0; i < 16; ++i)
   {
      CString strTemp;
      strTemp.Format(__T("%02x"), randNumbers[i]);
      strKey.Append(strTemp);
   }

   CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SECRETKEY_EDIT);
   pEdit->SetWindowText(strKey);

   m_keyText = strKey;
}

void COTAUpdateToolDlg::OnBnClickedEnableMitm()
{
	m_MitmEnabled = !m_MitmEnabled;
	m_pRadioInterface->SetMITMFlag(m_MitmEnabled);
}

void  COTAUpdateToolDlg::HandleUserPromptDestroy(int userValue, BtnPressedId button)
{
    /* User prompt dialog has been destroyed. The object will be deleted
     * in CUserPromptDlg::PostNcDestroy(). Set the pointer to NULL.
     */
    pUserPrompt = NULL;

	switch(usrDlgCurrentUsage)
    {
        case UserDialogUsageMitmPasskeyDisplay:
        {
            lastPressedBtn = button;
        }
        break;
    }
}

