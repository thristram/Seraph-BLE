/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      CUserPromptDlg.cpp
 *
 *  DESCRIPTION
 *      Defines the class behaviors for CUserPromptDlg Dialog window.
 *
 ******************************************************************************/

// CUserPromptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CUserPromptDlg.h"
#include "uEnergyOTAUpdateToolDlg.h"

// CUserPromptDlg dialog

IMPLEMENT_DYNAMIC(CUserPromptDlg, CDialog)

CUserPromptDlg::CUserPromptDlg(UINT headingStringId, UINT messageStringId, 
                               CString editBoxText,
                               LOGFONT editBoxFont, bool editBoxReadOnly,
                               bool bShowOkBtn /* = TRUE */,
                               bool bShowCancelBtn /* = TRUE */,
                               CWnd* pParent /* = NULL */)
    : CDialog(CUserPromptDlg::IDD, pParent)
{
    // Load the strings from the Resource string IDs.
    m_heading.LoadString(headingStringId);
    m_message.LoadString(messageStringId);
    m_editBoxmsg = editBoxText;
    m_editBoxTextFont.CreateFontIndirect(&editBoxFont);
    m_bEditBoxReadOnly = editBoxReadOnly;
    m_bShowOkBtn = bShowOkBtn;
    m_bShowCancelBtn = bShowCancelBtn;
    m_bDeselect = true;
}

CUserPromptDlg::~CUserPromptDlg()
{
    m_editBoxTextFont.DeleteObject();
}

void CUserPromptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUserPromptDlg, CDialog)
    ON_BN_CLICKED(IDCANCEL, &CUserPromptDlg::OnBnClickedCancel)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CUserPromptDlg::OnOK()
{
    /* Tell the mainframe that the dialog box is going to be closed. Also,
     * pass the value entered by the user.
     */
	COTAUpdateToolDlg* pUpdateTool = DYNAMIC_DOWNCAST(COTAUpdateToolDlg, GetOwner());
	pUpdateTool->HandleUserPromptDestroy(GetEditBoxTextAsNumber(), BtnPressedOk);
    pUpdateTool->EndModalState();

    CDialog::OnOK();
}

BOOL CUserPromptDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the heading of the dialog box.
    SetWindowText(m_heading);

    // Set the message to be displayed to the user.
    CStatic* msgCtrl = (CStatic*)GetDlgItem(IDC_USER_PROMPT_TEXT);
    if(msgCtrl)
    {
        msgCtrl->SetWindowText(m_message);
    }

    CEdit* usrEntryCtrl = (CEdit*)GetDlgItem(IDC_USER_ENTRY);
    if(usrEntryCtrl)
    {
        usrEntryCtrl->SetReadOnly(m_bEditBoxReadOnly);
        usrEntryCtrl->SetFont(&m_editBoxTextFont);
        usrEntryCtrl->SetWindowText(m_editBoxmsg);
    }

    if(!m_bShowOkBtn)
    {
        CStatic* okBtn = (CStatic*)GetDlgItem(IDOK);
        okBtn->ShowWindow(SW_HIDE);
    }

    if(!m_bShowCancelBtn)
    {
        CStatic* cancelBtn = (CStatic*)GetDlgItem(IDCANCEL);
        cancelBtn->ShowWindow(SW_HIDE);
    }

    return TRUE;
}

void CUserPromptDlg::OnBnClickedCancel()
{
    /* Tell the mainframe that the dialog box is going to be closed. Also,
     * pass the value entered by the user.
     */
	COTAUpdateToolDlg* pUpdateTool = DYNAMIC_DOWNCAST(COTAUpdateToolDlg, GetOwner());
	pUpdateTool->EndModalState();
	pUpdateTool->HandleUserPromptDestroy(GetEditBoxTextAsNumber(), BtnPressedCancel);	

    
    CDialog::OnCancel();
}

void CUserPromptDlg::OnPaint()
{
    // The SetFont() in OnInitDialog() is resulting in the text in the edit 
    // control being selected and caret being shown at the end of the box. 
    // This behaviour is not documented anywhere. So, as a work around deselect
    // the text in the edit box.
    if(m_bDeselect)
    {
        CEdit* usrEntryCtrl = (CEdit*)GetDlgItem(IDC_USER_ENTRY);
        usrEntryCtrl->SetSel(-1, -1);
        usrEntryCtrl->HideCaret();
        m_bDeselect = FALSE;
    }
}

void CUserPromptDlg::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}

void CUserPromptDlg::SetEditBoxText(CString text)
{
    CEdit* usrEntryCtrl = (CEdit*)GetDlgItem(IDC_USER_ENTRY);
    if(usrEntryCtrl)
    {
        usrEntryCtrl->SetWindowText(text);
        usrEntryCtrl->SetFont(&m_editBoxTextFont);
    }
}

int CUserPromptDlg::GetEditBoxTextAsNumber()
{
    int userValue = -1;
    CEdit* pValue = (CEdit*)GetDlgItem(IDC_USER_ENTRY);
    CString strLoc ;
    pValue->GetWindowText(strLoc);

    int length = strLoc.GetLength();
    if(length)
    {
        WCHAR pBuffer[256];
        memset(pBuffer, 0, 256);
        wcscpy_s(pBuffer, 256, strLoc.GetBuffer(length + 1));
        __int64 tempValue = _wtoi64(pBuffer);
        if(tempValue > 0 && tempValue < 65536)
        {
            userValue = (int)tempValue;
        }
    }

    return userValue;
}
