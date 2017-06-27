/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uiMsg.h
 *
 *  DESCRIPTION
 *      Provides a standardised interface for sending messages to the UI module
 *
 ******************************************************************************/

#ifndef _UIMSG_H
#define _UIMSG_H

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "MessageDefs.h"

class UiMsg
{
public:
    /******************************************************************************
     * SetMessageFunction 
     * Use this to register the message handler with this class 
     */
    static void SetMessageFunction(CWnd* pParent)
    {
        mParent = pParent;
    }

    /******************************************************************************
     * SendMsgToUI 
     * This method sends the message to UI 
     */
    static void SendMsgToUI(WORD msgId, LPVOID data)
    {
        UPDATE_UI_MSG_T *OtaMsg = new UPDATE_UI_MSG_T;

        OtaMsg->MsgId = msgId;
        OtaMsg->MsgData = data;

        mParent->SendMessage(DIALOG_MESSAGE_ID, OTA_MSG, (LPARAM)OtaMsg);
    }

    /******************************************************************************
     * LogMessage 
     * This method sends the message to the logging window 
     */
    static void LogMessage(CString& logText, MSG_TYPE_T type)
    {
        LOGGING_MSG_T *logMsg = new LOGGING_MSG_T;

        logMsg->logMessage = logText;
        logMsg->msgType = type;

        mParent->SendMessage(DIALOG_MESSAGE_ID, LOGGING_MSG, (LPARAM)logMsg);
    }

private:    
    /*============================================================================*
     *  Private Data
     *============================================================================*/

    static CWnd* mParent;
};

#endif /* _UIMSG_H */