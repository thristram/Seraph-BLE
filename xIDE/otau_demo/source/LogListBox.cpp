/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      LogListBox.cpp
 *
 *  DESCRIPTION
 *      implementation file for the custom list box.
 *
 ******************************************************************************/
//

#include "stdafx.h"
#include "uEnergyOTAUpdateTool.h"
#include "LogListBox.h"


// CLogListBox

IMPLEMENT_DYNCREATE(CLogListBox, CListBox)

CLogListBox::CLogListBox()
{
}

CLogListBox::~CLogListBox()
{
}


BEGIN_MESSAGE_MAP(CLogListBox, CListBox)
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_LOG_AUTOLOG, &CLogListBox::OnLogAutolog)
    ON_UPDATE_COMMAND_UI(ID_LOG_AUTOLOG, &CLogListBox::OnUpdateLogAutolog)
END_MESSAGE_MAP()



// CLogListBox message handlers
void CLogListBox::OnContextMenu(CWnd* pWnd, CPoint point)
{
   CMenu popupMenu;
   popupMenu.LoadMenu(IDR_LOG_MENU);

   CMenu* deviceMenu = popupMenu.GetSubMenu(0);
   ASSERT(deviceMenu);

   if(!deviceMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_HORPOSANIMATION, point.x, point.y, GetOwner()))
      ATLTRACE("Error showing context menu %d\r\n", GetLastError());

   return;
}


void CLogListBox::OnLogAutolog()
{
   // TODO: Add your command handler code here
}

void CLogListBox::OnUpdateLogAutolog(CCmdUI *pCmdUI)
{
   // TODO: Add your command update UI handler code here
}
