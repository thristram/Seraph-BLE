/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uEnergyOTAUpdateTool.cpp
 *
 *  DESCRIPTION
 *      Defines the class behaviors for the application.
 *
 ******************************************************************************/
#include "stdafx.h"
#include "uEnergyOTAUpdateTool.h"
#include "uEnergyOTAUpdateToolDlg.h"
#include "uiMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment( lib, "uEnergyHost" )

// COTAUpdateToolApp

BEGIN_MESSAGE_MAP(COTAUpdateToolApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COTAUpdateToolApp construction

COTAUpdateToolApp::COTAUpdateToolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only COTAUpdateToolApp object

COTAUpdateToolApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xF81E03AE, 0x5CAC, 0x48DB, { 0x8F, 0x61, 0xE7, 0xE, 0xAB, 0x1B, 0x60, 0x8 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// COTAUpdateToolApp initialization


bool COTAUpdateToolApp::GetVersionInfo(TCHAR* filename, TCHAR* request, CString& version)
{
    CString strTmp;

    DWORD handle;         //didn't actually use (dummy var)
    int infoSize = (int) GetFileVersionInfoSize(filename, &handle);
    if(infoSize == 0)
    {
        AfxMessageBox(_T("Function <GetFileVersionInfoSize> unsuccessful!"));
        version.Format(_T("ERROR"));
        return false;
    }

    LPVOID pBlock;
    pBlock = new BYTE[infoSize];

    int bResult = GetFileVersionInfo(filename, handle, infoSize, pBlock);
    if(bResult == 0)
    {
        AfxMessageBox(_T("Function <GetFileVersionInfo> unsuccessful!"));
        delete [] pBlock;
        version.Format(_T("ERROR"));
        return false;
    }

    // Structure used to store enumerated languages and code pages.
    struct LANGANDCODEPAGE {
             WORD wLanguage;
             WORD wCodePage;
    }*lpTranslate;

    LPVOID lpBuffer;
    UINT dwBytes;
    int i, langNumber;

    // Read the list of languages and code pages.
    bResult = VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &dwBytes);
    if(bResult == 0)
    {
        AfxMessageBox(_T("Function <VerQueryValue> for Translation unsuccessful!"));
        delete [] pBlock;
        version.Format(_T("ERROR"));
        return false;
    }

    langNumber = dwBytes/sizeof(struct LANGANDCODEPAGE);         //langNumber always must be equal 1 (in my program)
    if(langNumber != 1)
    {
        strTmp.Format(_T("Error! Languages number: %d."), langNumber);
        AfxMessageBox(strTmp);
        delete [] pBlock;
        version.Format(_T("ERROR"));
        return false;
    }

    // Read the file description for each language and code page.
    for(i=0; i<langNumber; i++)
    {
        strTmp.Format(_T("\\StringFileInfo\\%04x%04x\\%s"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage, request);

        // Retrieve file description for language and code page "i". 
        bResult = VerQueryValue(pBlock, strTmp, &lpBuffer, &dwBytes);
        if(bResult == 0)
        {
            AfxMessageBox(_T("Function <VerQueryValue> for StringFileInfo for %d th language unsuccessful!"), i);
            delete [] pBlock;
            version.Format(_T("ERROR"));
            return false;
        }
    }

    version = (TCHAR*)lpBuffer;
    delete [] pBlock;
    version.Replace(_T(", "), _T("."));
    return true;
}


BOOL COTAUpdateToolApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
      CString title, message;
      title.LoadString(IDS_APP_TITLE);
      message.LoadString(IDP_OLE_INIT_FAILED);
		MessageBox(NULL, message, title, MB_OK);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Cambridge Silicon Radio"));
	// Parse command line for automation or reg/unreg switches.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register class factories via CoRegisterClassObject().
		COleTemplateServer::RegisterAll();
	}
	// App was launched with /Unregserver or /Unregister switch.  Remove
	// entries from the registry.
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		COleObjectFactory::UpdateRegistryAll(FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
		return FALSE;
	}
	// App was launched standalone or with other switches (e.g. /Register
	// or /Regserver).  Update registry entries, including typelibrary.
	else
	{
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
			return FALSE;
	}

   TCHAR appPath[MAX_PATH];
   GetModuleFileName(NULL, appPath, MAX_PATH);
   // We need to get the file version.
   TCHAR stringName[] = _T("FileVersion");
   CString version;

   if(GetVersionInfo(appPath, stringName, version))
   {

      // Copy the present value of 'm_pszProfileName'.
      CString newProfileName;
      //newProfileName.Format(_T("%s"), m_pszProfileName);
      newProfileName += _T("\\");
      newProfileName += version;

      //First free the string allocated by MFC at CWinApp startup.
      //The string is allocated before InitInstance is called.
      //free((void*)m_pszProfileName);

      //The CWinApp destructor will free the memory.
      //m_pszProfileName=_tcsdup(newProfileName);
   }

	COTAUpdateToolDlg dlg(version);
	m_pMainWnd = &dlg;
   UiMsg::SetMessageFunction(m_pMainWnd);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
