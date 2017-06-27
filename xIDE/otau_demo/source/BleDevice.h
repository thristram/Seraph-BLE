/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      BleDevice.h
 *
 *  DESCRIPTION
 *      This Class defines the interface for the BleDevice class which includes
 *      information of a BLE device that is discovered and connected.
 *
 ******************************************************************************/
#pragma once

/*============================================================================*
 *  Standard Header Files
 *============================================================================*/
#include <map>
#include <vector>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "GenericTypes.h"

/*=============================================================================
*  Public definitions
*============================================================================*/
#define HANDLE_UNKNOWN                                               0x0000


/*============================================================================*
 *  Public Class Definitions
 *============================================================================*/

class BleDevice 
{

public:
   // CTor
   BleDevice(CSR_BLE_BLUETOOTH_ADDRESS devAddress, int devRssi, CString devName, 
      BYTE devFlags, bool pair = false)
        : name(devName), rssi(devRssi), flags(devFlags), txLevel(InvalidTxLevel),
        connMin(InvalidConnectionInterval), connMax(InvalidConnectionInterval),
        connectionHandle(0), latency(0), timeOut(0), connected(false),
        connMinPreferred(InvalidConnectionInterval), 
        connMaxPreferred(InvalidConnectionInterval),
        paired(pair)
    {
        address.type = devAddress.type;
        address.lAp = devAddress.lAp;
        address.nAp = devAddress.nAp;
        address.uAp = devAddress.uAp;
    }

   /* Returns the friendly name of the device. If name is empty 
      returns the Bluetooth address*/
    CString GetFriendlyName()
    {
        if(name.IsEmpty())
            return GetAddressAsString();
        return name;
    }

    /* Returns the name of the device*/
    CString GetName()
    {
        return name;
    }

    /* Sets the name of the device*/
    void SetName(CString newName)
    {
        name = newName;
    }

    /* Gets the RSSI of the device*/
    int GetRssi()
    {
        return rssi;
    }

    /* Sets the RSSI of the device*/
    void SetRssi(int newRssi)
    {
        rssi = newRssi;
    }

    /* Sets the Bluetooth address of the device*/
    CSR_BLE_BLUETOOTH_ADDRESS GetAddress()
    {
        return address;
    }

    /* Returns the the address of the device in string format*/
    CString GetAddressAsString()
    {
        return BTAddressToString(address);
    }

    /* Returns the device flags*/
    BYTE GetFlags()
    {
        return flags;
    }

    /* Sets the flags of the device*/
    void SetFlags(BYTE newFlags)
    {
        flags = newFlags;
    }

    /* Sets the TX Level of the device*/
    void SetTxLevel(int level)
    {
        txLevel = level;
    }

    /* Gets the TX Level of the device*/
    int GetTxLevel()
    {
        return txLevel;
    }
    
    /* Sets the Connection parameter of the device*/
    void SetConnectionParameters(WORD min, WORD max, WORD lat, WORD tOut)
    {
        connMin = min;
        connMax = max;
        latency = lat;
        timeOut = tOut;
    }

    /* Gets the minimum connection interval of the device*/
    WORD GetConnectionMinInterval()
    {
        return connMin;
    }

    /* Gets the maximum connection interval of the device*/
    WORD GetConnectionMaxInterval()
    {
        return connMax;
    }

    /* Gets the latency of the device*/
    WORD GetLatency()
    {
        return latency;
    }

    /* Gets the supervision timeout for the device*/
    WORD GetTimeout()
    {
        return timeOut;
    }

    /* Sets the preferred connection interval for the device*/
    void SetPreferredConnIntervals(WORD min, WORD max)
    {
        connMinPreferred = min;
        connMaxPreferred = max;
    }

    /* Gets the preferred minimum connection interval of the device*/
    WORD GetPreferredConnMinInterval()
    {
        return connMinPreferred;
    }

    /* Gets the preferred maximum connection interval of the device*/
    WORD GetPreferredConnMaxInterval()
    {
        return connMaxPreferred;
    }

    /* Sets the connection handle of the device*/
    void SetConnectionHandle(DWORD conHandle)
    {
        connectionHandle = conHandle;
    }

    /* Gets the connection handle of the device*/
    DWORD GetConnectionHandle()
    {
        return connectionHandle;
    }

    /* overloaded == operator for bluetooth address comparison*/
    bool operator ==(const CSR_BLE_BLUETOOTH_ADDRESS devAddress);

    /* Interface to add services to the device database*/
    void AddServices(int nServices, PCSR_BLE_SERVICE srv);

    /* Set the state of the device to connected */
    void SetConnected(bool connect = true)
    {
        connected = connect;
    }

    /* Returns the paired state of the device*/
    bool IsConnected()
    {
        return connected;
    }

    /* Sets the paired state of the device*/
    void SetPaired(bool pair = true)
    {
        paired = pair;
    }

    /* Gets the paired state of the device*/    
    bool IsPaired()
    {
        return paired;
    }

    /* Sets the appearance of the device*/
    void SetAppearance(WORD app)
    {
        appearance = app;
    }

    /* Gets the appearance of the device*/
    WORD GetAppearance()
    {
        return appearance;
    }

    // Gets the address of the device in string format
    static CString BTAddressToString(CSR_BLE_BLUETOOTH_ADDRESS address);

    // Gets the character based on character and service uuid
    WORD FindCharHandle(CSR_BLE_UUID servUuid, CSR_BLE_UUID charUuid, bool configHandle = false);

    // Gets the character based on character and service uuid
    static WORD FindCharHandle(UUIDServicePair::iterator i, UUID16Bit charUuid, bool configHandle = false);

    // Gets the character based on character and service uuid
    static WORD FindCharHandle(UUIDServicePair::iterator i, CSR_BLE_UUID charUuid, bool configHandle = false);

    // Gets the services supported by the device
    UUIDServicePair& GetServices()
    {
        return services;
    }

    // Gets the service data corresponding to the service UUID 
   BOOL GetService(CSR_BLE_UUID svcUuid, BleService &bleService);

   // Gets the permissions field of the characteristic data 
   BOOL GetPermission(CSR_BLE_UUID svcUuid, CSR_BLE_UUID charUuid, WORD &perm);

    void ClearServices()
    {
        services.clear();
    }

private:
    // device address
    CSR_BLE_BLUETOOTH_ADDRESS address;

    // RSSI
    int rssi;

    // device name
    CString name;

    // device flags
    BYTE flags;

    // Appearance value of the device
    WORD appearance;

    // TX level
    int txLevel;

    // preferred connection interval
    WORD connMinPreferred;
    WORD connMaxPreferred;

    // connection interval
    WORD connMin;
    WORD connMax;

    // slave latency
    WORD latency;

    // supervision timeout
    WORD timeOut;

    // BLE connection handle
    DWORD connectionHandle;

    UUIDServicePair services;

    // is initialized
    bool connected;

    // is paired
    bool paired;

    // supported BLE profiles
    DWORD profiles;
public:
   static const int InvalidRssi = 0xFFFF;
   static const WORD InvalidConnectionInterval = 0x0000;
   static const int InvalidTxLevel = 0xFFFF;

};