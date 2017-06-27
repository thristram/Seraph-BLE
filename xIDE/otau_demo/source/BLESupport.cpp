/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BLESupport.cpp
 *
 *  DESCRIPTION
 *      Assorted support routines.
 *
 ******************************************************************************/
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "BLESupport.h"


/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      addressToString
 *
 *  DESCRIPTION
 *      Bluetooth address-to-string converter
 *---------------------------------------------------------------------------*/
void addressToString(BYTE address[SIZE_OF_BLUETOOTH_ADDR_IN_BYTES], 
                     CString *strAddress)
{
    //temp string variable
    CString devAddress;
    devAddress.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
       address[5], address[4],
       address[3], address[2], 
       address[1], address[0]);

    //concatinate the two strings
    *strAddress += devAddress;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      typedAddressToString
 *
 *  DESCRIPTION
 *      Bluetooth address to string converter
 *---------------------------------------------------------------------------*/
void typedAddressToString(CSR_BLE_BLUETOOTH_ADDRESS *address, CString *strAddress )
{
    //temp string variable
    CString devAddress;

    //convert bdaddr to string format
    devAddress.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
                      ((address->nAp >> 8) & 0xFF), (address->nAp & 0xFF),
                       address->uAp,
                      ((address->lAp >> 16) & 0xFF), ((address->lAp >> 8) & 0xFF), (address->lAp & 0xFF));

    //concatinate the two strings
    *strAddress += devAddress;
}

    
/*----------------------------------------------------------------------------*
 *  NAME
 *      IdentityRootToString
 *
 *  DESCRIPTION
 *      Identity Root-to-string converter
 *---------------------------------------------------------------------------*/
void IdentityRootToString(unsigned short ir[SIZE_OF_IDENTITY_ROOT_IN_WORDS], 
                          CString *str)
{
	CString hex;
	int     word;

	for (word=0;word<8;word++)
	{
		hex.Format(_T("%04x"), ir[word]);
		*str += hex;
	}
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      EncryptionRootToString
 *
 *  DESCRIPTION
 *      Encryption Root-to-string converter
 *---------------------------------------------------------------------------*/
void EncryptionRootToString(unsigned short er[SIZE_OF_ENCRYPTION_ROOT_IN_WORDS], 
                            CString *str)
{
	CString hex;
	int     word;

	for (word=0;word<8;word++)
	{
		hex.Format(_T("%04x"), er[word]);
		*str += hex;
	}
}
