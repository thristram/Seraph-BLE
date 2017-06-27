/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      aes.h
 *
 *  DESCRIPTION
 *      This Class implements the interface with the functionality
 *      provided by the file csraes.dll.
 *
 ******************************************************************************/
#ifndef _AES_H
#define _AES_H

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <windows.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "csraes.h"

/*============================================================================*
 *  Public Class Definitions
 *============================================================================*/
class Aes
{
public:
    /*============================================================================*
     *  Public Function Prototypes
     *============================================================================*/

    /* Check that the DLL is present in the current working directory */
    static bool CheckForDll(void);

    /* Process (encrypt) the challenge key supplied by the remote device */
    static void ProcessChallenge(unsigned char challengeKey[AES_KEY_LENGTH_BYTES]);

    /* Call this function when the application is to close */
    static void OnShutdown(void);

    static void SetSharedSecret(LPBYTE secretKey);

private:

    
    /*============================================================================*
     *  Private Function Prototypes
     *============================================================================*/
    typedef void (__cdecl *challengeFuncPtr)(unsigned char*, const unsigned char*);

    /* The handle for the AES DLL */
    static HINSTANCE dllHandle;

    /* A pointer to the encryption function supplied by the DLL */
    static challengeFuncPtr processChallenge;

};

#endif /* _AES_H */
