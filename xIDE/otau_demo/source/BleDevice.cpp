/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BleDevice.cpp
 *
 *  DESCRIPTION
 *      This Class implements the interface with the functionality
 *      for BleDevice.
 *
 ******************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "uuids.h"
#include "BleDevice.h"
#include "HelperFunc.h"


/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
*  NAME
*      BTAddressToString
*
*  DESCRIPTION
*      Converts the CSR_BLE_BLUETOOTH_ADDRESS address to string
*---------------------------------------------------------------------------*/
CString BleDevice::BTAddressToString(CSR_BLE_BLUETOOTH_ADDRESS address)
{
    CString devAddress;
    devAddress.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
        (address.nAp >> 8) & 0xFF, address.nAp & 0xFF,
        address.uAp,
        (address.lAp) >> 16 & 0xFF, (address.lAp >> 8) & 0xFF, address.lAp & 0xFF);

    return devAddress;
}

/*----------------------------------------------------------------------------*
*  NAME
*      BTAddressToString
*
*  DESCRIPTION
*      Compares the CSR_BLE_BLUETOOTH_ADDRESS address 
*---------------------------------------------------------------------------*/
bool BleDevice::operator ==(const CSR_BLE_BLUETOOTH_ADDRESS devAddress)
{
    return (address.type == devAddress.type) && (address.lAp == devAddress.lAp)
          && (address.nAp == devAddress.nAp) && (address.uAp == devAddress.uAp);
}

/*----------------------------------------------------------------------------*
*  NAME
*      AddServices
*
*  DESCRIPTION
*      Adds the services found during discovery to the device database.
*---------------------------------------------------------------------------*/
void BleDevice::AddServices(int nServices, PCSR_BLE_SERVICE srv)
{
    services.clear();

    for(int i = 0; i < nServices; i++)
    {
        BleService bleService;

        bleService.uuid = srv[i].uuid;
        bleService.startHandle = srv[i].startHandle;
        bleService.endHandle = srv[i].endHandle;

        for(int j = 0; j < srv[i].nCharacteristics; j++)
        {
           BleCharacteristic bleChr;

           bleChr.uuid = srv[i].characteristics[j].uuid;
           bleChr.permissions = srv[i].characteristics[j].properties;
           bleChr.valueHandle = srv[i].characteristics[j].handle;
           bleChr.declHandle = srv[i].characteristics[j].declHandle;
           bleChr.cconfigHandle = 0x0000;
           bleChr.descriptors.clear();

           for(int k = 0; k < srv[i].characteristics[j].nDescriptors; k++)
           {
              if((srv[i].characteristics[j].descriptors[k].uuid.lengthInBytes == CSR_BT_UUID16_SIZE_IN_BYTES) && 
                 (srv[i].characteristics[j].descriptors[k].uuid.uuid16 == Uuids::UUID_CLIENT_CHAR_CONFIG))
              {
                 // extract client configuration handle
                 bleChr.cconfigHandle = srv[i].characteristics[j].descriptors[k].handle;
              }
              else
              {
                 // add to the array of non-supported descriptors
                 bleChr.descriptors.push_back(srv[i].characteristics[j].descriptors[k]);
              }
           }
           bleService.chars.insert(std::pair<CSR_BLE_UUID, BleCharacteristic>(bleChr.uuid, bleChr));
        }

		services.insert(std::pair<CSR_BLE_UUID, BleService>(bleService.uuid, bleService));
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      FindCharHandle
*
*  DESCRIPTION
*      Find the Characteristic handle fo the specific service characteristic. 
*      The function can also find the client configuration desc. of the char.
*---------------------------------------------------------------------------*/
WORD BleDevice::FindCharHandle(CSR_BLE_UUID servUuid, CSR_BLE_UUID charUuid, 
                               bool configHandle /* = false*/)
{
	UUIDServicePair::iterator i = services.find(servUuid);

	if(i != services.end())
	{
		return FindCharHandle(i, charUuid, configHandle);
	}

    return HANDLE_UNKNOWN;
}

/*----------------------------------------------------------------------------*
*  NAME
*      FindCharHandle
*
*  DESCRIPTION
*      Find the Characteristic handle fo the specific service characteristic. 
*      The function can also find the client configuration desc. of the char.
*---------------------------------------------------------------------------*/
WORD BleDevice::FindCharHandle(UUIDServicePair::iterator i, CSR_BLE_UUID charUuid, 
                               bool configHandle /* = false*/)
{
	UUIDCharacteristicPair::iterator j = (*i).second.chars.find(charUuid);
    if(j != (*i).second.chars.end())
    {
        if(configHandle)
            return (*j).second.cconfigHandle;
        else
            return (*j).second.valueHandle;
    }

    return HANDLE_UNKNOWN;
}

/*----------------------------------------------------------------------------*
*  NAME
*      FindCharHandle
*
*  DESCRIPTION
*      Find the Characteristic handle fo the specific service characteristic. 
*      The function can also find the client configuration desc. of the char.
*-----------------------------------------------------------------------------*/
WORD BleDevice::FindCharHandle(UUIDServicePair::iterator i, UUID16Bit charUuid, 
                    bool configHandle /*= false*/)
{
   CSR_BLE_UUID uuid;
   Init16BitUUID(uuid, charUuid);
   return FindCharHandle(i, uuid,configHandle);
}


/*----------------------------------------------------------------------------*
*  NAME
*      GetService
*
*  DESCRIPTION
*      Gets the service information corersponding to the UUID
*-----------------------------------------------------------------------------*/
BOOL BleDevice::GetService(CSR_BLE_UUID svcUuid, BleService &bleService)
{
   UUIDServicePair::iterator it1 = services.find(svcUuid);
   if(it1 != services.end())
   {
      bleService = it1->second;
      return TRUE;
   }
   return FALSE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      GetPermission
*
*  DESCRIPTION
*      Gets the permissions field of the characteristic data 
*-----------------------------------------------------------------------------*/
BOOL BleDevice::GetPermission(CSR_BLE_UUID svcUuid, CSR_BLE_UUID charUuid, WORD &perm)
   {
      perm = 0;
      BleService service;
      if(GetService(svcUuid, service) == TRUE)
      {
         std::pair<UUIDCharacteristicPair::iterator, UUIDCharacteristicPair::iterator> char_pairs;
         char_pairs = service.chars.equal_range(charUuid);
         for(UUIDCharacteristicPair::iterator k = char_pairs.first; k != char_pairs.second; k++)
         {
            perm = (*k).second.permissions;
            break;
         }
         return TRUE;
      }
      return FALSE;
   }