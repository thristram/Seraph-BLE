/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      KeyEdit.h
 *
 *  DESCRIPTION
 *      This class is a subclass of the CEdit class and handles the 
 *      authentication key validation.
 *
 ******************************************************************************/
#pragma once


// CKeyEdit

class CKeyEdit : public CEdit
{
	DECLARE_DYNAMIC(CKeyEdit)

public:
	CKeyEdit();
	virtual ~CKeyEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   BOOL PreTranslateMessage(MSG* pMsg);
   afx_msg void OnEnKillfocus();
};


