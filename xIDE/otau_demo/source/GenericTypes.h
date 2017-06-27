/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      GenericTypes.h
 *
 *  DESCRIPTION
 *      This class handles some generic functions and interfaces for BLE 
 *      application.
 *
 ******************************************************************************/

#pragma once
#include "uEnergyHost.h"
#include <map>
#include <vector>

struct UUIDComparer
{
    bool operator()(const CSR_BLE_UUID & Left, const CSR_BLE_UUID & Right) const
    {
        // comparison logic goes here
        if(Left.lengthInBytes == Right.lengthInBytes)
        {
            switch(Left.lengthInBytes)
            {
            case CSR_BT_UUID16_SIZE_IN_BYTES:
                {
                    if(Left.uuid16 > Right.uuid16)
                        return true;
                    else 
                        return false;
                }
                break;
            case CSR_BT_UUID128_SIZE_IN_BYTES:
                {
                    for(int i = Left.lengthInBytes-1; i >= 0; i--)
                     {
                        if(Left.uuid128[i] > Right.uuid128[i])
                        {
                            return true;
                        }
                        else if(Left.uuid128[i] < Right.uuid128[i])
                        {
                           return false;
                        }
                     }
                    return false;
                }
                break;
            default:
               break;
            }
        }
        else
        {
            if(Left.lengthInBytes > Right.lengthInBytes)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }
};


typedef WORD UUID16Bit;
typedef WORD Handle;
typedef WORD ConfigHandle;
typedef std::map<UUID16Bit, Handle> uuidHandlePair;
typedef std::map<UUID16Bit, ConfigHandle> uuidConfigHandlePair;

typedef std::map<CSR_BLE_UUID, Handle, UUIDComparer> uuidHandlePairT;
typedef std::map<CSR_BLE_UUID, ConfigHandle, UUIDComparer> uuidConfigHandlePairT;


// service chacteristics
typedef struct _BleCharacteristic
{
    CSR_BLE_UUID uuid;
    WORD valueHandle;
    WORD cconfigHandle;
    WORD declHandle;
    BYTE permissions;

    std::vector<CSR_BLE_CHARACTERISTIC_DSC> descriptors; 

}BleCharacteristic;

typedef std::multimap<CSR_BLE_UUID, BleCharacteristic, UUIDComparer> UUIDCharacteristicPair;

// device services
typedef struct _BleService
{
    WORD startHandle;
    WORD endHandle;
    CSR_BLE_UUID uuid;
    UUIDCharacteristicPair   chars;

}BleService;

typedef std::map<CSR_BLE_UUID, BleService, UUIDComparer>UUIDServicePair;