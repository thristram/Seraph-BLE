/**********************************************************************
 *
 *  uecsresultenum.h
 *
 *  Copyright 2015 Qualcomm Technologies International, Ltd. 
 *
 *  Handles uEnergy result codes.
 *
 ***********************************************************************/

#ifndef UECSRESULTENUM_H
#define UECSRESULTENUM_H

enum UeCsResultEnum
{
    /// The operation completed successfully.
    UE_CS_RES_OK,

    /// An internal error occurred; the application is not expected to recover
    /// from this.
    UE_CS_RES_INTERNAL_ERROR,

    /// An invalid argument was passed in to a method; usually (but not always)
    /// indicative of a programming error.
    UE_CS_RES_INVALID_ARGUMENT,

    /// The requested information is not available; usually (but not always)
    /// indicative of a programming sequence error.
    UE_CS_RES_INFO_NOT_AVAILABLE,

    /// The CRC on the device is invalid; almost always indicative of a corrupt
    /// image stored on a device or the device missing completely.
    UE_CS_RES_IMAGE_CRC_INVALID,

    /// The requested key does not appear in the current version.
    UE_CS_RES_KEY_UNKNOWN,

    /// The attributes of the key specified (length, range, etc.) are invalid.
    UE_CS_RES_KEY_INVALID,

    /// The version requested does not appear in the XML database.
    UE_CS_RES_VERSION_UNSUPPORTED,

    /// The versions/instances of the images being manipulated are incompatible
    /// with each other.
    UE_CS_RES_VERSION_MISMATCH,

    /// The file has an invalid extension.
    UE_CS_RES_FILE_WRONG_TYPE,

    /// The file does not exist.
    UE_CS_RES_FILE_NOT_FOUND,

    /// The file exists and has a valid extension, but the contents of the file
    /// do not match with the extension.
    UE_CS_RES_FILE_FOUND_BUT_WRONG_FORMAT,

    /// The file could not be read.
    UE_CS_RES_FILE_NOT_READABLE,

    /// The file could not be written.
    UE_CS_RES_FILE_NOT_WRITEABLE,

    /// The device is not supported; either the requisite hardware is not installed
    /// or the chip is of the wrong type.
    UE_CS_RES_DEVICE_NOT_SUPPORTED,

    /// The device could not be read from.
    UE_CS_RES_DEVICE_WRITE_FAILED,

    /// The device could not be written to.
    UE_CS_RES_DEVICE_READ_FAILED
};

#endif
