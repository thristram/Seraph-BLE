/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      CUserPromptDlg.h
 *
 *  DESCRIPTION
 *      The main header file for the CUserPromptDlg Dialog window
 *
 ******************************************************************************/

#pragma once

#include "Resource.h"
// CUserPromptDlg dialog

enum BtnPressedId
    {
        BtnPressedNone,
        BtnPressedCancel,
        BtnPressedOk
    };


class CUserPromptDlg : public CDialog
{
    DECLARE_DYNAMIC(CUserPromptDlg)

public:
    CUserPromptDlg(UINT headingStringId, UINT messageStringId,
                   CString editBoxText, LOGFONT editBoxFont,
                   bool editBoxReadOnly, bool bShowOkBtn = TRUE,
                   bool bShowCancelBtn = TRUE, CWnd* pParent = NULL);
    virtual ~CUserPromptDlg();

    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnPaint();
    void SetEditBoxText(CString text);
    int GetEditBoxTextAsNumber();

// Dialog Data
    enum { IDD = IDD_PROMPT_USER };

protected:

    CString m_heading;
    CString m_message;
    CString m_editBoxmsg;
    CFont m_editBoxTextFont;
    bool m_bEditBoxReadOnly;
    bool m_bShowOkBtn;
    bool m_bShowCancelBtn;
    bool m_bDeselect;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();

    DECLARE_MESSAGE_MAP()
};
