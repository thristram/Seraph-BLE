/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      KeyEdit.cpp
 *
 *  DESCRIPTION
 *      Implements a custom control for edit control for the 
 *      keys to be entered for authentication.
 *
 ******************************************************************************/


#include "stdafx.h"
#include "resource.h"
#include "KeyEdit.h"
#include "uEnergyOTAUpdateToolDlg.h"

// CKeyEdit

IMPLEMENT_DYNAMIC(CKeyEdit, CEdit)

CKeyEdit::CKeyEdit()
{

}

CKeyEdit::~CKeyEdit()
{
}


BEGIN_MESSAGE_MAP(CKeyEdit, CEdit)
   //ON_WM_KEYDOWN()
   ON_CONTROL_REFLECT(EN_KILLFOCUS, &CKeyEdit::OnEnKillfocus)
END_MESSAGE_MAP()



// CKeyEdit message handlers


/*----------------------------------------------------------------------------*
*  NAME
*      OnKeyDown
*
*  DESCRIPTION
*      Handler for key down message
*---------------------------------------------------------------------------*/
void CKeyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: Add your message handler code here and/or call default
   
   CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*----------------------------------------------------------------------------*
*  NAME
*      PreTranslateMessage
*
*  DESCRIPTION
*      Handler for messages
*---------------------------------------------------------------------------*/
BOOL CKeyEdit::PreTranslateMessage(MSG* pMsg)
{
   return CEdit::PreTranslateMessage(pMsg);
}

/*----------------------------------------------------------------------------*
*  NAME
*      OnEnKillfocus
*
*  DESCRIPTION
*      Handler when the edit control looses focus.
*---------------------------------------------------------------------------*/
void CKeyEdit::OnEnKillfocus()
{
   // TODO: Add your control notification handler code here
   CString strText;
   GetWindowText(strText);

   if(strText.GetLength() < KEY_TEXT_LENGTH)
   {
      CString logMsg, title;
      title.LoadString(IDS_APP_TITLE);
      logMsg.LoadString(IDS_AUTH_VAL);
      MessageBox(logMsg, title);

      SetFocus();
      int len = strText.GetLength();
      SetSel(0, len);
   }
}
