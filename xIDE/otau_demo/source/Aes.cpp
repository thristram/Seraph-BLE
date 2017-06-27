/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      Aes.cpp
 *
 *  DESCRIPTION
 *      This Class implements the interface with the functionality
 *      provided by the file csraes.dll.
 *
 ******************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "aes.h"


/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#define AES_DLL_NAME    L"csraes.dll"


/*============================================================================*
 *  Private Data
 *============================================================================*/
unsigned char authenticationKey[AES_KEY_LENGTH_BYTES] = {0};

/* The handle for the AES DLL */
HINSTANCE Aes::dllHandle = NULL;

/* A pointer to the encryption function supplied by the DLL */
Aes::challengeFuncPtr Aes::processChallenge = NULL;

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/******************************************************************************
 * CheckForDll 
 * Check that the DLL is present in the current working directory
 *****************************************************************************/
bool Aes::CheckForDll(void)
{
    dllHandle = LoadLibrary(AES_DLL_NAME);

    if(dllHandle)
    {
        processChallenge = (challengeFuncPtr)GetProcAddress(dllHandle, (LPCSTR)1);
    }

    return ((dllHandle != NULL) && (processChallenge != NULL));
}

/******************************************************************************
 * ProcessChallenge 
 * Process (encrypt) the challenge key supplied by the remote device
 *****************************************************************************/
void Aes::ProcessChallenge(unsigned char challengeKey[AES_KEY_LENGTH_BYTES])
{
    if(processChallenge)
    {
        processChallenge(challengeKey, authenticationKey);
    }
}

/******************************************************************************
 * SetSharedSecret 
 * This function sets the shared secret key
 *****************************************************************************/
void Aes::SetSharedSecret(LPBYTE secretKey)
{
   memcpy(authenticationKey, secretKey, AES_KEY_LENGTH_BYTES);
}

/******************************************************************************
 * OnShutdown 
 * Call this function when the application is to close
 *****************************************************************************/
void Aes::OnShutdown(void)
{
    if(dllHandle)
    {
        FreeLibrary(dllHandle);
    }
}