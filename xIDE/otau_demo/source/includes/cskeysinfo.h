/**********************************************************************
 *
 *  cskeysinfo.h
 *
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *
 *  Handles uEnergy keys information.
 *
 ***********************************************************************/

#ifndef CSKEYSINFO_H
#define CSKEYSINFO_H

//disable warnings on dll interface
#pragma warning (disable : 4251)

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CSKEYINFO_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CSKEYINFO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CSKEYINFO_EXPORTS
#define CSKEYINFO_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define CSKEYINFO_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif

#include "uecsresultenum.h"
#include "enginefw_interface.h"

#include <string>
#include <vector>

class CSKEYINFO_API CCsKeysInfo
{
public:
    /// Holds information about each key.
    struct CSKEYINFO_API CsKeyStruct
    {
        std::string name;             ///< The name of the key.
        std::string label;            ///< A simple label for the key.
        std::string description;      ///< A longer description (often several sentences) for the key.
        std::vector<uint16> defaults; ///< A vector of the default values
        uint16 offset;                ///< The offset into the data set.
        uint16 length;                ///< The number of 16-bit words used to make up the key (as declared in the keys database).
        uint16 min;                   ///< The minimum value allowed.
        uint16 max;                   ///< The maximum value allowed.
        uint16 id;                    ///< The ID of the key.
    };

    /// Class used for internal exception handling in LoadXML.
    class CsKeysException : public std::exception
    {
    public:
        inline CsKeysException(UeCsResultEnum aErrorCode)
            : mErrorCode(aErrorCode)
        {
        }

        inline UeCsResultEnum GetErrorCode()
        {
            return mErrorCode;
        }

    private:
        UeCsResultEnum mErrorCode;
    };

    CCsKeysInfo();
    ~CCsKeysInfo();

    ///
    /// The CS version, about which, subsequent operations will apply.
    /// The XML database contains the data for many different CS versions. When the 
    /// XML database is loaded, all versions are read in. However, the methods to
    /// extract data about any given key are only applicable for one specific version.
    /// Use this method to specify which version to use for all subsequent calls to
    /// this class.
    /// @param[in] aReqVersion The requested version number.
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum SetInterestedVersion(uint16 aReqVersion);

    ///
    /// Get the number of keys available for the currently selected version.
    /// @return Total number of keys available
    ///
    uint16 NumKeys() const;

    ///
    /// Determine the ID of the key from the index value.
    /// @param[in] aKeyIndex The index value.
    /// @param[out] aKeyId The ID value of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyIdFromIndex(uint16 aKeyIndex, uint16& aKeyId) const;

    ///
    /// Determine the minimum permissible value of a key for a given ID.
    /// If the minimum has not been declared in the database for the key,
    /// the value presented is zero.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aMinValue The minimum permissible value of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyMinValue(uint16 aKeyId, uint16& aMinValue) const;

    ///
    /// Determine the maximum permissible value of a key for a given ID.
    /// If the maximum has not been declared in the database for the key,
    /// the value presented is the highest possible integer value.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aMaxValue The maximum permissible value of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyMaxValue(uint16 aKeyId, uint16& aMaxValue) const;

    ///
    /// Determine the ID of the key from the name.
    /// @param[in] aName The name of the key.
    /// @param[out] aKeyId The ID value of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyIdFromName(const std::string& aName, uint16& aKeyId) const;

    ///
    /// Determine the name of the key with the specified ID.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aName The name of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyName(uint16 aKeyId, std::string& aName) const;

    ///
    /// Determine the description of the key with the specified ID.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aDescription The description of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyDescription(uint16 aKeyId, std::string& aDescription) const;

    ///
    /// Determine the default values of the key with the specified ID.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aKeyData A vector to place the default values.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyDefaultValue(uint16 aKeyId, std::vector<uint16>& aKeyData) const;

    ///
    /// Determine the label of the key with the specified ID.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aLabel The label of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyLabel(uint16 aKeyId, std::string& aLabel) const;

    ///
    /// Determine the length of the value of the key with the specified ID.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aLength The length of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyLength(uint16 aKeyId, uint16& aLength) const;

    ///
    /// Determine the details of the key from the index value.
    /// @param[in] aKeyIndex The index value.
    /// @param[out] aDetails The details of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyDetailsFromIndex(uint16 aKeyIndex, CsKeyStruct& aDetails) const;

    ///
    /// Determine the details of the key with the specified ID.
    /// @param[in] aKeyId The ID value of the key.
    /// @param[out] aDetails The details of the key.
    /// @see SetInterestedVersion().
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum KeyDetailsFromId(uint16 aKeyId, CsKeyStruct& aDetails) const;

    ///
    /// Loads the key definition from the XML file.
    /// @param[in] aXmlFile The XML file to load.
    /// If this is not empty, use this as the XML filename to read. Otherwise, use the
    /// file specified in the CSCONFIGFILE_ENV_NAME envirnonment variable. If this is
    /// empty, use the file specified by ::DEFAULT_CSKEY_XML_FILE.
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum LoadDatabase(char * aXmlFile);

private:
     /// A string containing the name of the environment variable to use to choose the XML file.
    static const std::string CSCONFIGFILE_ENV_NAME;
    /// The name of the XML keys database that has been successfully loaded.
    std::string mXmlFilenameLoaded;

    /// The default XML keys database file to load if the filename is not specified.
    static const std::string DEFAULT_CSKEY_XML_FILE;

    /// The error text to display if a key cannot be found in the XML database
    static const std::string ERROR_TEXT_KEY_NOT_FOUND;

    /// The build ID version that is currently "in effect".
    uint16 mCurrentlySelectedBuildId;

    /// The list of build IDs in the XML file.
    std::vector<uint16> mXmlBuildIds;

    /// Holds all the key info.
    std::vector< std::vector<CsKeyStruct> > mKeyInfo;

    ///
    /// Internal method to load the XML file (once the filename has been determined).
    /// @param[in] aFilename The filename to use.
    /// @return A value from the enum ::UeCsResultEnum.
    ///
    UeCsResultEnum LoadXML(std::string aFilename);
};

#endif

