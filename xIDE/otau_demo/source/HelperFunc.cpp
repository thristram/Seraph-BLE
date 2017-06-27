/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      HelperFunc.cpp
 *
 *  DESCRIPTION
 *      This class implements helper functions for BLE.
 *
 ******************************************************************************/
/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "stdafx.h"
#include "GenericTypes.h"
#include "HelperFunc.h"

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      Init16BitUUID
 *
 *  DESCRIPTION
 *      CBleRadio constructor
 *---------------------------------------------------------------------------*/
void Init16BitUUID(CSR_BLE_UUID& uuid, UUID16Bit uuid16)
{
    memset(&uuid, 0, sizeof(uuid));
    uuid.lengthInBytes = CSR_BT_UUID16_SIZE_IN_BYTES;
    uuid.uuid16 = uuid16;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      Init128BitUUID
 *
 *  DESCRIPTION
 *      CBleRadio constructor
 *---------------------------------------------------------------------------*/
void Init128BitUUID(CSR_BLE_UUID& uuid, LPBYTE uuid128)
{
    memset(&uuid, 0, sizeof(uuid));
    uuid.lengthInBytes = CSR_BT_UUID128_SIZE_IN_BYTES;
    for(int i = 0; i < uuid.lengthInBytes; i++)
    {
        uuid.uuid128[i] = uuid128[i];
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      Init128BitUUID
 *
 *  DESCRIPTION
 *      CBleRadio constructor
 *---------------------------------------------------------------------------*/
void Init128BitUUID(CSR_BLE_UUID& uuid,  ULONGLONG uuidMSB, ULONGLONG uuidLSB)
{
    memset(&uuid, 0, sizeof(uuid));
    uuid.lengthInBytes = CSR_BT_UUID128_SIZE_IN_BYTES;
    
    int count = 0; 
    for(int i = 0; i < uuid.lengthInBytes; i++)
    {
        if(i <= 7)
        {
           // The LSB's are set first
            uuid.uuid128[i] = ((uuidLSB >> (count*8)) & 0xFF);
        }
        else
        {
            // The MSB's are set here
            uuid.uuid128[i] = ((uuidMSB >> (count*8)) & 0xFF);
        }
        count ++;
        if(i == 7)
        {
           count = 0;
        }
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      Match128BitUUID
 *
 *  DESCRIPTION
 *      CBleRadio constructor
 *---------------------------------------------------------------------------*/
BOOL Match128BitUUID(CSR_BLE_UUID& uuidLeft, CSR_BLE_UUID& uuidRight)
{
   if((uuidLeft.lengthInBytes == uuidRight.lengthInBytes) &&
        (uuidLeft.lengthInBytes == CSR_BT_UUID128_SIZE_IN_BYTES))
    {
        for(int i = uuidLeft.lengthInBytes-1; i >= 0; i--)
        {
            if(uuidLeft.uuid128[i] != uuidRight.uuid128[i])
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      Match128BitUUID
 *
 *  DESCRIPTION
 *      CBleRadio constructor
 *---------------------------------------------------------------------------*/
BOOL Match128BitUUID(CSR_BLE_UUID& uuid, ULONGLONG uuidMSB, ULONGLONG uuidLSB)
{
    if(uuid.lengthInBytes != CSR_BT_UUID128_SIZE_IN_BYTES)
        return FALSE;
    int count = 7; 

    for(int i = uuid.lengthInBytes-1; i >= 0; i--)
    {
        // The LSB's are compared here
        if(i <= 7)
        {
            if(uuid.uuid128[i] != ((uuidLSB >> (count*8)) & 0xFF))
            {
                return FALSE;
            }
        }
        else
        {
             // The MSB's are compared here
            if(uuid.uuid128[i] != ((uuidMSB >> (count*8)) & 0xFF))
            {
                return FALSE;
            }
        }
        count--;
        // Now compare the MSB bits hence reset the count to 0
        if(i == 8)
        {
           count = 7;
        }

    }
    return TRUE;
}