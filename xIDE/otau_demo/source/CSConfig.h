/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      csconfig.h
 *
 *  DESCRIPTION
 *       This module updates the device image file with the supplied BT address
 *       and device crystal trim, updating the header block CRCs on the way.
 *      
 ******************************************************************************/

#ifndef _CSCONFIG_H
#define _CSCONFIG_H

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <windows.h>
#include <stdio.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "ImageCRC.h"

/*============================================================================*
 *  Public Class Definitions
 *============================================================================*/
/* This is the structure of the data held in memory by this module */
    /* This is the structure of each block header in the image file */
    typedef struct {
       WORD offset;
       WORD destination;
       WORD length;
       WORD crc;
    } BLOCK_HEADER_INFO_T;

class ImageUpdate
{
public:
    /*============================================================================*
     *  Public Function Prototypes
     *============================================================================*/

   ImageUpdate();
    /* Update the supplied image file with the specified BT address and
     * crystal trim.
     */
    BOOL MergeCsKeys(FILE *fp, CHAR *bdaddr, CHAR *xtalfreqtrim,
                              CHAR *identity_root, CHAR *encryption_root);

    BOOL ValidateImageFile(FILE *fp);

private:
    /*============================================================================*
    *  Private Definitions
    *============================================================================*/
    static const unsigned short MAX_LINE_LENGTH        = 100;
    static const unsigned short WORD_LENGTH_IN_CHARS   = 4;
    static const unsigned short BDADDR_LEN_IN_WORDS    = 3;
    static const unsigned short IDENTITY_ROOT_LEN_IN_WORDS    = 8;
    static const unsigned short ENCRYPTION_ROOT_LEN_IN_WORDS  = 8;

    static const unsigned short CTRL_HEADER_CRC_LINE   = 0;
    static const unsigned short CTRL_HEADER_BLOCK_LINE = 1;
    static const unsigned short CTRL_HEADER_SCL_LINE   = 2;
    static const unsigned short HEADER_LEN_IN_WORDS    = 3;

    static const unsigned short BLOCK_OFFSET_LINE      = 0;
    static const unsigned short BLOCK_DESTINATION_LINE = 1;
    static const unsigned short BLOCK_LENGTH_LINE      = 2;
    static const unsigned short BLOCK_CRC_LINE         = 3;
    static const unsigned short BLOCK_LEN_IN_WORDS     = 4;

    static const unsigned short IS_HEX = 16;

    /*============================================================================*
     *  Private Data
     *============================================================================*/
    BYTE   mNumBlocks;
    BYTE   mKeyBlockLenInWords;
    fpos_t mKeyBlockCrcLine;
    CHAR mThisLine[MAX_LINE_LENGTH];

    BLOCK_HEADER_INFO_T *blockData;
    /*============================================================================*
     *  Private Function Implementations
     *============================================================================*/
    void findStartOfData(FILE *fp);
    BOOL isCommentLine(FILE *fp);
    void moveFpToStartOfLine(FILE *fp, unsigned short lineIndex);
    BYTE getNumberBlocks(FILE *fp);
    void recordCskeyBlockSize(FILE *fp);
    void mergeXtalTrim(FILE *fp, char *xtalfreqtrim);
    void mergeIdentityRoot(FILE *fp, char *identity_root);
    void mergeEncryptionRoot(FILE *fp, char *encryption_root);
    void updateControlHdrCrc(FILE *fp, WORD ControlHdrCrc);
    void updateCsBlockCrc(FILE *fp, WORD CsBlockCrc);
    WORD calculateControlHdCrc(FILE *fp);
    void mergeBdAddr(FILE *fp, char *bdaddr);
    WORD calculateCsBlkHdCrc(FILE *fp);
    WORD getNextDataBlk(FILE* fp, fpos_t &pos);
    bool checkCrc(FILE *fp, BLOCK_HEADER_INFO_T *blockData, WORD offset);
    void readDataBlock(FILE *fp, int index , BLOCK_HEADER_INFO_T *blockData);
    WORD readControlHdrCrc(FILE *fp);
    WORD readCsBlockCrc(FILE *fp);

    
};

#endif /* _CSCONFIG_H */