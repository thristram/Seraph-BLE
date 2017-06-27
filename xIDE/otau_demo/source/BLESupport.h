/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BLESupport.h
 *
 *  DESCRIPTION
 *      Assorted support routines.
 *
 ******************************************************************************/

#ifndef __SUPPORT_H
#define __SUPPORT_H

#include "uEnergyHost.h"

#define SIZE_OF_IDENTITY_ROOT_IN_WORDS          (8)
#define SIZE_OF_ENCRYPTION_ROOT_IN_WORDS        (8)
#define SIZE_OF_BLUETOOTH_ADDR_IN_BYTES         (6)

/******************************************************************************
 * addressToString 
 * Bluetooth address-to-string converter
 */
void addressToString(BYTE address[SIZE_OF_BLUETOOTH_ADDR_IN_BYTES], 
                     CString *strAddress);

/******************************************************************************
 * typedAddressToString 
 * Typed-bluetooth address-to-string converter
 */
void typedAddressToString(CSR_BLE_BLUETOOTH_ADDRESS *address, CString *strAddress);

void IdentityRootToString(unsigned short ir[SIZE_OF_IDENTITY_ROOT_IN_WORDS], 
                          CString *str);
void EncryptionRootToString(unsigned short er[SIZE_OF_ENCRYPTION_ROOT_IN_WORDS], 
                            CString *str);
#endif /* __SUPPORT_H */