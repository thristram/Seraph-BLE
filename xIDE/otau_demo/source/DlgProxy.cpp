/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      DlgProxy.cpp
 *
 *  DESCRIPTION
 *      implementation file
 *
 ******************************************************************************/
#include "stdafx.h"
#include "uEnergyOTAUpdateTool.h"
#include "DlgProxy.h"
#include "uEnergyOTAUpdateToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COTAUpdateToolDlgAutoProxy

IMPLEMENT_DYNCREATE(COTAUpdateToolDlgAutoProxy, CCmdTarget)

COTAUpdateToolDlgAutoProxy::COTAUpdateToolDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(COTAUpdateToolDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(COTAUpdateToolDlg)))
		{
			m_pDialog = reinterpret_cast<COTAUpdateToolDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

COTAUpdateToolDlgAutoProxy::~COTAUpdateToolDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void COTAUpdateToolDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(COTAUpdateToolDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COTAUpdateToolDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Note: we add support for IID_IuEnergyOTAUpdateTool to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {AEE4B565-A36E-4F96-B5C7-60643E21FCD3}
static const IID IID_IuEnergyOTAUpdateTool =
{ 0xAEE4B565, 0xA36E, 0x4F96, { 0xB5, 0xC7, 0x60, 0x64, 0x3E, 0x21, 0xFC, 0xD3 } };

BEGIN_INTERFACE_MAP(COTAUpdateToolDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(COTAUpdateToolDlgAutoProxy, IID_IuEnergyOTAUpdateTool, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {D6E0F2B4-8530-471F-B869-86D34F48C074}
IMPLEMENT_OLECREATE2(COTAUpdateToolDlgAutoProxy, "uEnergyOTAUpdateTool.Application", 0xd6e0f2b4, 0x8530, 0x471f, 0xb8, 0x69, 0x86, 0xd3, 0x4f, 0x48, 0xc0, 0x74)


// COTAUpdateToolDlgAutoProxy message handlers
