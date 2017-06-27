/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *
 *  FILE
 *      csraes.h
 *
 *  DESCRIPTION
 *      This is the header file for the aesdll.dll.
 *      The file csraes.dll is available from the CSR Support web-site.
 *
 ******************************************************************************/
#ifndef _CSR_AES_H
#define _CSR_AES_H

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* The length (in bytes) of the AES challenge and secret keys */
#define AES_KEY_LENGTH_BYTES    16
/* The length (in words) of the AES challenge and secret keys */
#define AES_KEY_LENGTH_WORDS    (AES_KEY_LENGTH_BYTES >> 1)

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* Encrypt the supplied challengeKey using the shared key. The result is returned
 * in the challengeKey.
 */
extern __declspec(dllexport) 
    void aes_challenge_response(unsigned char challengeKey[AES_KEY_LENGTH_BYTES], 
                                const unsigned char sharedSecretKey[AES_KEY_LENGTH_BYTES]);
#endif /* _CSR_AES_H */
