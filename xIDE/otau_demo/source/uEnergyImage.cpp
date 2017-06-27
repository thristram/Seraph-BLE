/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uEnergyImage.cpp
 *
 *  DESCRIPTION
 *      This class handles the device image that is to be transfered as part
 *      of the device update.
 *
 ******************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "uEnergyImage.h"
#include "uimsg.h"
#include "resource.h"

/*============================================================================*
 *  Private Data
 *============================================================================*/

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
*  NAME
*      ImageFileHandler CTOR
*
*  DESCRIPTION
*      ImageFileHandler constructor
*---------------------------------------------------------------------------*/
ImageFileHandler::ImageFileHandler()
{
   mImgFile = NULL;
   mDeviceAddressValid = FALSE;
   mXtalTrimValid = FALSE;
   mIdRootValid = FALSE;
   mEnRootValid = FALSE;

   mImgFileSize = 0;          // The image file size
   mPacketCount = 0;          // The number of packets sent

   strcpy_s(mTempFileName, MAX_PATH, "OTATmp.Img");
   CHAR tmpPath[MAX_PATH];
   if(GetTempPathA(MAX_PATH, tmpPath) > 0)
   {
      strcpy_s(mTempFileName, MAX_PATH, tmpPath);
      strcat_s(mTempFileName, MAX_PATH, "OTATmp.Img");
   }
   

   keysUpdated = FALSE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      ResetTransfer
*
*  DESCRIPTION
*      Reset the data transfer records.
*---------------------------------------------------------------------------*/
void ImageFileHandler::ResetTransfer(void)
{
    if(mImgFile)
    {
        fseek(mImgFile, 0, SEEK_SET);
    }

    mPacketCount = 0;
    keysUpdated = FALSE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetImageFileName
*
*  DESCRIPTION
*      The file name of the image to be transferred.
*---------------------------------------------------------------------------*/
bool ImageFileHandler::SetImageFileName(CString wideFileName)
{
    // Convert the supplied CString into an ASCII file name
    WideCharToMultiByte(CP_ACP, // ANSI Code Page
                        0, // No special handling of unmapped chars
                        wideFileName, // wide-character string to be converted
                        wideFileName.GetLength()+1,
                        mAsciiFileName, 
                        MAX_PATH,
                        NULL, NULL ); // Unrepresented char replacement - Use Default

    return true;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetImageFileName
*
*  DESCRIPTION
*      The file name of the image to be transferred.
*---------------------------------------------------------------------------*/
bool ImageFileHandler::OpenImageFile()
{
    bool fileOpened = false;
    if(mImgFile != NULL)
    {
        fclose(mImgFile);
        mImgFile = NULL;
    }

    FILE *imgFile = NULL;              // The image file to transfer
    // Try to open the specified file
    if(0 == fopen_s(&imgFile, mAsciiFileName, "rb+"))
    {
       fclose(imgFile);
       //if(fopen_s(&mImgFile, "OTATemp.img", "w+")) == 0)
       DeleteFileA(mTempFileName);
       CopyFileA(mAsciiFileName, mTempFileName, FALSE);

       if(0 == fopen_s(&mImgFile, mTempFileName, "rb+"))
       {
          fileOpened = true;

          // Try to determine the length of the specified file
          mImgFileSize = readFileLength(mImgFile);

          ResetTransfer();
       }
    }
    return (fileOpened && (mImgFileSize > 0));
}

/*----------------------------------------------------------------------------*
*  NAME
*      CloseImageFile
*
*  DESCRIPTION
*      Closes the image file handle
*---------------------------------------------------------------------------*/
bool ImageFileHandler::CloseImageFile()
{
   if(mImgFile != NULL)
   {
      fclose(mImgFile);
      mImgFile = NULL;
      DeleteFileA(mTempFileName);
   }

   return true;
}

/*----------------------------------------------------------------------------*
*  NAME
*      GetNextChunk
*
*  DESCRIPTION
*      This method sends image file to the remote device.
*---------------------------------------------------------------------------*/
BYTE ImageFileHandler::GetNextChunk(BYTE chunkSize, BYTE *data)
{
    char thisLine[IMAGE_MAX_LINE_LENGTH] = {0};
    char dataInAscii[IMAGE_DATA_LENGTH_IN_CHARS] = {0};
    char *whitespace;
    BYTE actualChunkSize = 0;
    WORD dataInHex;
    int  counter;

    // copy maximum "chunkSize" bytes of image data
    for(counter = 0; counter < chunkSize; counter += 2) 
    {
        // read next line, ignoring commented lines
        if(fgets(thisLine, IMAGE_MAX_LINE_LENGTH, mImgFile) == NULL)
        {
            break;
        }

        // Ignore any commented lines in the file
        if(strncmp(thisLine, "//", 2) == 0)
        {
            continue;
        }

        // thisLine should contain something line this:
        // @001122 8465
        // Address, data

        // Find the white space separating address and data
        whitespace = strrchr(thisLine, ' ');
        if(whitespace == NULL)
        {
            // Cannot process this line
            break;
        }

        // copy the data (as a string)
        memcpy(dataInAscii, (whitespace + 1), IMAGE_DATA_LENGTH_IN_CHARS);

        // convert string to hex value
        dataInHex = (WORD)strtol(dataInAscii, NULL, 16);

        //add to Tx Buffer
        data[actualChunkSize++] = (BYTE)(dataInHex & 0xFF);
        data[actualChunkSize++] = (BYTE)((dataInHex >> 8 ) & 0xFF);
    }

    if(actualChunkSize)
    {
        mPacketCount++;
    }

    return actualChunkSize;
}

/*----------------------------------------------------------------------------*
*  NAME
*      GetTransferProgress
*
*  DESCRIPTION
*      Calculate the percent-complete transfer state.
*---------------------------------------------------------------------------*/
double ImageFileHandler::GetTransferProgress(void)
{
    return ((double)(((double)(mPacketCount) * 20)/(double)mImgFileSize) * (double)100); 
}


/*----------------------------------------------------------------------------*
*  NAME
*      SetImageBtAddress
*
*  DESCRIPTION
*      The BT address to incorporate into the image.
*---------------------------------------------------------------------------*/
void ImageFileHandler::SetImageBtAddress(BYTE *address)
{
    BYTE character = 0;
    short byte;

    for(byte = (IMAGE_BT_ADDRESS_LEN_IN_CHARS / 2)-1; byte >= 0 ; byte--)
    {
        mDeviceAddress[character++] = address[byte] >> 4;
        mDeviceAddress[character++] = address[byte] & 0xf;
    }

    for(character = 0; character < IMAGE_BT_ADDRESS_LEN_IN_CHARS; character++)
    {
        if(mDeviceAddress[character] <= 9)
        {
            mDeviceAddress[character] += '0';
        }
        else
        {
            mDeviceAddress[character] += ('a' - 0xa);
        }
    }
    mDeviceAddress[IMAGE_BT_ADDRESS_LEN_IN_CHARS] = 0;

    mDeviceAddressValid = TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetImageXtalTrim
*
*  DESCRIPTION
*      The crystal trim to incorporate into the image.
*---------------------------------------------------------------------------*/
void ImageFileHandler::SetImageXtalTrim(BYTE trim)
{
    sprintf_s(mXtalTrim, IMAGE_XTRIM_LENGTH_IN_CHARS+1, "%04X", trim);
    mXtalTrimValid = TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      ConvertLittleEndianOrderedWordsToASCIIArray
*
*  DESCRIPTION
*      Convert a BYTE array of words (16-bit) stored in little endian byte order
*      to ASCII string array. The character array should be atleast 4 times the 
*      number of words (because each 16-bit word contains 4 hex digits
*---------------------------------------------------------------------------*/
void ImageFileHandler::ConvertLittleEndianOrderedWordsToASCIIArray(
                  const BYTE *pWordsBuf, 
                  BYTE wordCount, 
                  CHAR* pCharArray)
{
    BYTE character = 0;
    BYTE word;

    for(word = 0; word < wordCount; ++word)
    {
		BYTE lByte = *(pWordsBuf);
		BYTE hByte = *(pWordsBuf+1);
        pCharArray[character++] = hByte >> 4;
        pCharArray[character++] = hByte & 0xf;
        pCharArray[character++] = lByte >> 4;
        pCharArray[character++] = lByte & 0xf;
		pWordsBuf += 2;
    }

	/* Each word contains 4 characters */
    for(character = 0; character < (wordCount * 4); character++)
    {
        if(pCharArray[character] <= 9)
        {
            pCharArray[character] += '0';
        }
        else
        {
            pCharArray[character] += ('a' - 0xa);
        }
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetImageIdentityRoot
*
*  DESCRIPTION
*      Function to set the identity root
*---------------------------------------------------------------------------*/
void ImageFileHandler::SetImageIdentityRoot(LPBYTE identity_root)
{
   ConvertLittleEndianOrderedWordsToASCIIArray( identity_root,
		IMAGE_IDENTITY_ROOT_LEN_IN_CHARS/4, mIdentityRoot );

    mIdentityRoot[IMAGE_IDENTITY_ROOT_LEN_IN_CHARS] = 0;
    mIdRootValid = TRUE;
}

/*----------------------------------------------------------------------------*
*  NAME
*      SetImageEncryptionRoot 
*
*  DESCRIPTION
*      Function to set the encryption root
*---------------------------------------------------------------------------*/
void ImageFileHandler::SetImageEncryptionRoot(LPBYTE encryption_root)
{
    ConvertLittleEndianOrderedWordsToASCIIArray( encryption_root,
		IMAGE_ENCRYPTION_ROOT_LEN_IN_CHARS/4, mEncryptionRoot );

    mEncryptionRoot[IMAGE_ENCRYPTION_ROOT_LEN_IN_CHARS] = 0;
    mEnRootValid = TRUE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      MergeKeys
*
*  DESCRIPTION
*       Merge into the image the supplied address, crystal trim, identity root
*       and encryption root.
*---------------------------------------------------------------------------*/
BOOL ImageFileHandler::MergeKeys(void)
{
    char *deviceAddress = NULL;
    char *xtalTrim = NULL;
    char *identity_root = NULL;
    char *encryption_root = NULL;
    BOOL retval = FALSE;
    ImageUpdate csConfig;

    if(mImgFile)
    {
       CString strLoad;
       strLoad.FormatMessage(IDS_OTA_MERGE_KEYS, mAsciiFileName);
       UiMsg::LogMessage(strLoad, INFO_TYPE);

        retval = TRUE;

        if(mDeviceAddressValid || mXtalTrimValid)
        {
            if(mDeviceAddressValid)
            {
                deviceAddress = mDeviceAddress;
            }

            if(mXtalTrimValid)
            {
                xtalTrim = mXtalTrim;
            }

             if(mIdRootValid)
            {
              identity_root = mIdentityRoot;
            }

            if(mEnRootValid)
            {
              encryption_root = mEncryptionRoot;
            }

            csConfig.MergeCsKeys(mImgFile, deviceAddress, xtalTrim, identity_root, encryption_root);

            fseek(mImgFile, 0, SEEK_SET);
        }
    }

    return retval;
}

/*----------------------------------------------------------------------------*
*  NAME
*      ValidateImageFile
*
*  DESCRIPTION
*       Verifies the validity of the specified image file.
*---------------------------------------------------------------------------*/
BOOL ImageFileHandler::ValidateImageFile()
{
   ImageUpdate csConfig;
   BOOL retVal = csConfig.ValidateImageFile(mImgFile);

   fseek(mImgFile, 0, SEEK_SET);

   return retVal;
}