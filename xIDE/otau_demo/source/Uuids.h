/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uuids.h
 *
 *  DESCRIPTION
 *      This class holds the UUIDs for the OTAU Service and Characteristics
 *
 ******************************************************************************/

#ifndef __OTA_UUIDS_H
#define __OTA_UUIDS_H

class Uuids
{
public:
    static  const int UUID_INVALID = 0x0000;
    // The UUID identifying the GATT service
    static const int UUID_GATT_SERVICE = 0x1801;
    static const int UUID_CLIENT_CHAR_CONFIG = 0x2902;
    static const int UUID_BATTERY_SERVICE = 0x180F;
    static const int UUID_BATTERY_LEVEL_CHAR = 0x2A19;

    // The UUID identifying a client configuration characteristic
    static const int UUID_CLIENT_CHAR_CFG = 0x2902;
    // The UUID identifying the Service Changed characteristic
    static const int UUID_SERVICE_CHANGED = 0x2a05;

    // OTA-update UUIDs
    static const BYTE OtaUpgradeBootloaderServiceUuid[];
    static const BYTE OtaVersionCharUuid[];  // shared
    static const BYTE OtaTransCtrlCharUuid[];// exclusive 
    
    static const BYTE OtaUpgradeApplicationServiceUuid[];
    static const BYTE OtaReadCSKeyCharUuid[]; // exclusive 

    static const BYTE OtaCurAppCharUuid[];    // shared
    static const BYTE OtaDataTransCharUuid[]; // shared
    static const BYTE OtaReadCSBlockCharUuid[]; // exclusive
};

#endif /* __OTA_UUIDS_H */