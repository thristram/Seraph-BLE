/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uEnergyOTAUpdateTool.h
 *
 *  DESCRIPTION
 *      The main header file for the uEnergyOTAUpdateTool application
 *
 ******************************************************************************/


#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COTAUpdateToolApp:
// See uEnergyOTAUpdateTool.cpp for the implementation of this class
//

class COTAUpdateToolApp : public CWinAppEx
{
public:
	COTAUpdateToolApp();

// Overrides
	public:
	virtual BOOL InitInstance();
   bool GetVersionInfo(TCHAR* filename, TCHAR* request, CString& version);

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern COTAUpdateToolApp theApp;