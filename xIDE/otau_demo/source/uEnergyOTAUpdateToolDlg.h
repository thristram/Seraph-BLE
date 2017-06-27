/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uEnergyOTAUpdateToolDlg.h
 *
 *  DESCRIPTION
 *      The main header file for the uEnergyOTAUpdateTool Dialog window
 *
 ******************************************************************************/


#pragma once

#include "OtaInterface.h"
#include "loglistbox.h"
#include "CUserPromptDlg.h"
#include "keyedit.h"


class COTAUpdateToolDlgAutoProxy;
#define KEY_TEXT_LENGTH          32
// Length of the LTK edit box = 32 numbers + '0x' = 34.
#define KEY_TEXT_BOX_LENGTH      34

#define ASCII_VALUE_0            48
#define ASCII_VALUE_9            57
#define ASCII_VALUE_A            65
#define ASCII_VALUE_F            70
#define ASCII_VALUE_a            97
#define ASCII_VALUE_f            102
#define ASCII_VALUE_X            88
#define ASCII_VALUE_x            120

#define IS_VALID_HEX(n)          ((n >= ASCII_VALUE_0 && n <= ASCII_VALUE_9) ||\
                                 (n >= ASCII_VALUE_A && n <= ASCII_VALUE_F) ||\
                                 (n >= ASCII_VALUE_a && n <= ASCII_VALUE_f))

#define IS_CHAR_X(x)             (x == ASCII_VALUE_x || x == ASCII_VALUE_X)

#define MAX_MITM_ATTEMPTS        0x03

enum UserDialogUsage
{
   UserDialogUsageNone,
   UserDialogUsageMitmPasskeyDisplay
};

// COTAUpdateToolDlg dialog
class COTAUpdateToolDlg : public CDialog
{
	DECLARE_DYNAMIC(COTAUpdateToolDlg);
	friend class COTAUpdateToolDlgAutoProxy;

// Construction
public:
	COTAUpdateToolDlg(CString strRegPath, CWnd* pParent = NULL);	// standard constructor
	virtual ~COTAUpdateToolDlg();

// Dialog Data
	enum { IDD = IDD_UENERGYOTAUPDATETOOL_DIALOG };

	void HandleUserPromptDestroy(int, BtnPressedId);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	COTAUpdateToolDlgAutoProxy* m_pAutoProxy;
	HICON             m_hIcon;
   COTAInterface*    m_pRadioInterface;
   CString           m_PathName;
   BOOL              m_AutoLog;
   BOOL              m_SynergyLog;
   UINT_PTR          connectionTimer;
   BOOL              m_bHandleKeyUpdateMsg;
   HANDLE            singleInstanceMutex;
   CString           m_RegPath;
   BOOL              m_AuthEnabled;
   BOOL              m_MitmEnabled;
   
   // The time the image transfer started
   CTime             mStartTime;

   // Record whether the image transfer start time is recorded
   bool              mStartTimeRecorded;

   // The type of OTA mode supported by the connected device
   int               mRemoteDeviceMode;

   // The AES encryption DLL is available
   bool              mAesAvailable;

   BOOL              m_DownLoadInProgress;

   CStdioFile        m_LogFile;
   BOOL              m_IsStartingUp;

   CString           m_keyText;
   
   BtnPressedId      lastPressedBtn;

   UserDialogUsage   usrDlgCurrentUsage;

	BOOL CanExit();
	// Generated message map functions
	virtual BOOL OnInitDialog();
   BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
   LRESULT OnBleMessage(WPARAM wParam, LPARAM lParam);
   LRESULT OnGetDefID(WPARAM wp, LPARAM lp) ;
   LRESULT OnDialogMessage(WPARAM w_param, LPARAM l_param);
   afx_msg void OnBnClickedBrowseImage();
   afx_msg void OnBnClickedButtonDeviceScan();
   afx_msg void OnBnClickedImageDownload();
   afx_msg void OnBnClickedRemoveDevice();
   afx_msg void OnBnClickedAbout();
   afx_msg void OnLvnItemChangedDevList(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnLogAutoLog();
   afx_msg void OnLogClearlog();
   afx_msg void OnUpdateAutoLog(CCmdUI* pCmdUI);
   afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
   afx_msg LRESULT OnDeviceChange(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnPowerStateChange(WPARAM wParam, LPARAM lParam);
   afx_msg void OnEnUpdateSecretkeyEdit();
   afx_msg void OnEnChangeSecretkeyEdit();
   afx_msg void OnBnClickedCheckAuth();
   afx_msg void OnBnClickedGeneratekeyBtn();
   afx_msg void OnBnClickedEnableMitm();

private:

   // Sets the length of the horizontal text for horizontal scroll
   void SetTextExtent(CString strText);

   /* Handles device bonded message */
   void HandleDeviceBonded(LPVOID Msg);
   /* Handles download status updates */
   void HandleDownloadStatus(LPVOID MsgData);
   /* Handles device state updates */
   void HandleDeviceConnStatus(CSR_BLE_BT_ADDRESS *btAddr);
   /* Handles set OTA mode response */ 
   void HandleSetOtaModeRsp(LPVOID Msg);
   /* Handles set current app response */ 
   void HandleSetCurAppRsp(LPVOID Msg);
   /* Handles read current app position response */ 
   void HandleReadCurAppRsp(LPVOID Msg);
   /* Notifies the current mode of the application */
   void HandleRemoteApplicationMode(LPVOID MsgData);
   /* Handles the challenge response sequence in the application */
   void HandleChallengeResponseStatus(LPVOID Msg);
   /* Handles the bootloader  version response in the application */
   void HandleVersionResponse(LPVOID Msg);
   /* Handles the start transfer sequence in the application */
   void HandleStartTransfer();
   /* Handles device debonded message */
   void HandleDeviceDebonded(LPVOID Msg);

   void ToggleScan();
   void StopScan();
   void ResetDownloadState(void);
   void DisableControls();
   void OnTimer(UINT_PTR nIDEvent);
   bool IsItemPresent(CString *DevAddr);
   CString GetImgFileName();
   bool IsSetCurApp();
   void ClearCurAppSelection(void);
   void ReceiveOtaUpdateUIMsg(LPARAM l_param);
   void StringToBdAddr(TCHAR *StrAddr, CSR_BLE_BLUETOOTH_ADDRESS *BluetoothAddress);
   void ResetConnection();
   void ClearDeviceList();

   BOOL CheckAndUpdateHexNumber(CString& text);
   void OnExit(BOOL bCancel = FALSE);
   
	DECLARE_MESSAGE_MAP()
public:
   CLogListBox m_LogListBox;
   CKeyEdit m_SharedKeyEdit;   
   CUserPromptDlg*     pUserPrompt;   
};
