/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      DlgProxy.h
 *
 *  DESCRIPTION
 *      header file
 *
 ******************************************************************************/
#pragma once

class COTAUpdateToolDlg;


// COTAUpdateToolDlgAutoProxy command target

class COTAUpdateToolDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(COTAUpdateToolDlgAutoProxy)

	COTAUpdateToolDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	COTAUpdateToolDlg* m_pDialog;

// Operations
public:

// Overrides
	public:
	virtual void OnFinalRelease();

// Implementation
protected:
	virtual ~COTAUpdateToolDlgAutoProxy();

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(COTAUpdateToolDlgAutoProxy)

	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

