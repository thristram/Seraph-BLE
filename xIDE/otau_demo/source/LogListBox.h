/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      LogListBox.h
 *
 *  DESCRIPTION
 *      This class is a subclass of the CListBox class and handles the 
 *      menu handling of the listbox.
 *
 ******************************************************************************/
#pragma once


// CLogListBox

class CLogListBox : public CListBox
{
	DECLARE_DYNCREATE(CLogListBox)

public:
	CLogListBox();
	virtual ~CLogListBox();

   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void OnUpdateAutoLog(CCmdUI* pCmdUI);

protected:
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnLogAutolog();
   afx_msg void OnUpdateLogAutolog(CCmdUI *pCmdUI);
};


