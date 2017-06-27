/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      HelperFunc.h
 *
 *  DESCRIPTION
 *      This class handles some generic functions and interfaces for BLE 
 *      application.
 *
 ******************************************************************************/

#pragma once

#include "GenericTypes.h"

// Initializes the uuid object with a 16 bit uuid
void Init16BitUUID(CSR_BLE_UUID& uuid, UUID16Bit uuid16);

// Initializes the uuid object with a 128 bit uuid
void Init128BitUUID(CSR_BLE_UUID& uuid, LPBYTE uuid128);

// Initializes the uuid object with a 128 bit uuid
void Init128BitUUID(CSR_BLE_UUID& uuid,  ULONGLONG uuidMSB, ULONGLONG uuidLSB);

// compares the uuid objects 
BOOL Match128BitUUID(CSR_BLE_UUID& uuidLeft, CSR_BLE_UUID& uuidRight);

// compares the uuid objects
BOOL Match128BitUUID(CSR_BLE_UUID& uuid, ULONGLONG uuidMSB, ULONGLONG uuidLSB);

