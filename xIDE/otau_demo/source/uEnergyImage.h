/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      uEnergyImage.h
 *
 *  DESCRIPTION
 *      This class handles the device image that is to be transfered as part
 *      of the device update.
 *
 ******************************************************************************/

#ifndef __IMAGE_H
#define __IMAGE_H

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "stdafx.h"
#include "csconfig.h"

/*============================================================================*
 *  Public Class Definitions
 *============================================================================*/
class ImageFileHandler
{
public:
   ImageFileHandler();

   virtual ~ImageFileHandler() 
   {
      if(mImgFile != NULL)
      {
         fclose(mImgFile);
         mImgFile = NULL;
      }
   }
    /*============================================================================*
     *  Public Definitions
     *============================================================================*/
    static const unsigned short IMAGE_NORMAL_CHUNK_SIZE_BYTES = 20;
    static const unsigned short IMAGE_MAX_LINE_LENGTH         = 100;
    static const unsigned short IMAGE_DATA_LENGTH_IN_CHARS    = 4;
    static const unsigned short IMAGE_ADDRESS_LENGTH_IN_CHARS = 6;
    static const unsigned short IMAGE_BT_ADDRESS_LEN_IN_CHARS = 12;
    static const unsigned short IMAGE_XTRIM_LENGTH_IN_CHARS   = 4;
    static const unsigned short IMAGE_IDENTITY_ROOT_LEN_IN_CHARS   = 32;
    static const unsigned short IMAGE_ENCRYPTION_ROOT_LEN_IN_CHARS = 32;

    /*============================================================================*
     *  Public Function Prototypes
     *============================================================================*/

    // The file name of the image to be transferred
    bool SetImageFileName(CString fileName);

    BOOL ValidateImageFile();

    // The file to be open for download
    bool OpenImageFile();

    bool CloseImageFile();

    // The BT address to incorporate into the image
    void SetImageBtAddress(BYTE *address);

    // The crystal trim to incorporate into the image
    void SetImageXtalTrim(BYTE trim);

    void SetImageIdentityRoot(LPBYTE idroot);

    void SetImageEncryptionRoot(LPBYTE enroot);

    // Merge into the image the supplied address and crystal trim
    BOOL MergeKeys(void);

    // Get the next chunk of image data
    BYTE GetNextChunk(BYTE chunkSize, BYTE *data);

    // Reset the data transfer records
    void ResetTransfer(void);

    // Calculate the percent-complete transfer state
    double GetTransferProgress(void);

private:

    /*============================================================================*
     *  Private Data
     *============================================================================*/
    CHAR mAsciiFileName[MAX_PATH];
    CHAR mTempFileName[MAX_PATH];
    CHAR mDeviceAddress[IMAGE_BT_ADDRESS_LEN_IN_CHARS+1];
    CHAR mXtalTrim[IMAGE_XTRIM_LENGTH_IN_CHARS+1];
    CHAR mIdentityRoot[IMAGE_IDENTITY_ROOT_LEN_IN_CHARS+1];
    CHAR mEncryptionRoot[IMAGE_ENCRYPTION_ROOT_LEN_IN_CHARS+1];
    BOOL mDeviceAddressValid;
    BOOL mXtalTrimValid;
    BOOL mIdRootValid;
    BOOL mEnRootValid;

    FILE *mImgFile;              // The image file to transfer
    DWORD mImgFileSize;          // The image file size
    DWORD mPacketCount;          // The number of packets sent

    BOOL keysUpdated;
  
    /*============================================================================*
     *  Private Function Implementations
     *============================================================================*/
    DWORD readFileLength(FILE *fp)
    {
        char currentAddress[IMAGE_ADDRESS_LENGTH_IN_CHARS];
        DWORD fileLength = 0;
        char c;
        int  i;

        //
        // Try to determine the length of the specified file
        //

        // Goto end of file
        fseek(fp, 0, SEEK_END);

        // Word back until the first '@' is found
        
        for(i = -1; ;i--)
        {
            fseek(fp, i, SEEK_CUR);

            c = fgetc(fp);

            if(c == '@')
            {
                //read the string
                
                fread(currentAddress, 1, IMAGE_ADDRESS_LENGTH_IN_CHARS, fp);

                //convert the string to hex value
                fileLength  = (DWORD)strtol(currentAddress, NULL, 16) + 4;

                //break the loop
                break;
            }
        }

        return fileLength;
    }

    	void ConvertLittleEndianOrderedWordsToASCIIArray(const BYTE *pWordsBuf, BYTE wordCount, CHAR* pCharArray);
};


#endif /* __IMAGE_H */