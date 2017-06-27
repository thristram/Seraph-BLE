/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      csconfig.cpp
 *
 *  DESCRIPTION
 *      The CS Key configuration related function implementation is provided
 *      in the application.
 ******************************************************************************/

/*============================================================================*
*  Local Header Files
*============================================================================*/
#include "stdafx.h"
#include "csconfig.h"
#include "Imagecrc.h"

/*============================================================================*
*  Private Data
*============================================================================*/

/*============================================================================*
*  Public Function Implementations
*============================================================================*/
ImageUpdate::ImageUpdate() 
{
   mNumBlocks = 0;
   mKeyBlockLenInWords = 0;
   mKeyBlockCrcLine = NULL;
   memset(mThisLine, 0, sizeof(mThisLine));
}

/*----------------------------------------------------------------------------*
*  NAME
*      MergeCsKeys
*
*  DESCRIPTION
*      This function merges the CS Keys read from the existing image into 
*      the new image to be uploaded.
*---------------------------------------------------------------------------*/
BOOL ImageUpdate::MergeCsKeys(FILE *fp, CHAR *bdaddr, CHAR *xtalfreqtrim,
                              CHAR *identity_root, CHAR *encryption_root)
{
   WORD CsBlockCrc;
   WORD ControlHdrCrc;

   /* Reset to the start of the file */
   fseek(fp, 0, SEEK_SET);

   /* Read the number of header blocks in this image */
   mNumBlocks = getNumberBlocks(fp);

   if(mNumBlocks == 0)
   {
      return FALSE;
   }

   if(bdaddr != NULL)
   {
      mergeBdAddr(fp, bdaddr);

      if(xtalfreqtrim != NULL)
      {
         mergeXtalTrim(fp, xtalfreqtrim);
      }

      //merge the identity root
      if(identity_root)
      {
         mergeIdentityRoot(fp, identity_root);
      }

      ////merge the encryption root
      if(encryption_root)
      {
         mergeEncryptionRoot(fp, encryption_root);
      }

      CsBlockCrc = calculateCsBlkHdCrc(fp);

      updateCsBlockCrc(fp, CsBlockCrc);

      ControlHdrCrc = calculateControlHdCrc(fp);

      updateControlHdrCrc(fp, ControlHdrCrc);
   }

   /* Reset to the start of the file */
   fseek(fp, 0, SEEK_SET);

   return TRUE;
}


/*----------------------------------------------------------------------------*
*  NAME
*      findStartOfData
*
*  DESCRIPTION
*      finds the location for start of data in the image file.
*---------------------------------------------------------------------------*/

void ImageUpdate::findStartOfData(FILE *fp)
{
   INT ch;

   do { 
      ch = fgetc(fp);
   } while((ch != '\r') && (ch != '\n'));

   ungetc(ch, fp);

   fseek(fp, -WORD_LENGTH_IN_CHARS, SEEK_CUR);
}

/*----------------------------------------------------------------------------*
*  NAME
*      isCommentLine
*
*  DESCRIPTION
*      Verifies if it a commented line
*---------------------------------------------------------------------------*/
BOOL ImageUpdate::isCommentLine(FILE *fp)
{
   fpos_t pos;

   /* store the start of line pos */
   fgetpos (fp,&pos);

   /* read in the line */
   fgets(mThisLine, MAX_LINE_LENGTH, fp);

   /* set back to the start of line pos */
   fsetpos (fp,&pos);

   return (strchr(mThisLine, '/') != NULL);
}

/*----------------------------------------------------------------------------*
*  NAME
*      moveFpToStartOfLine
*
*  DESCRIPTION
*      moves the file pointer to the start of the line
*---------------------------------------------------------------------------*/

void ImageUpdate::moveFpToStartOfLine(FILE *fp, unsigned short lineIndex)
{
   unsigned short lineCount;

   lineCount = 0;
   while(lineCount < lineIndex)
   {
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      /* don't count commented lines */
      if(strchr(mThisLine, '/'))
      {
         continue;
      }

      lineCount++;
   }

   /* Check that this line is not a comment */
   while(isCommentLine(fp))
   {
      /* read in and discard the line */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      getNumberBlocks
*
*  DESCRIPTION
*      Calculates the no of blocks 
*---------------------------------------------------------------------------*/

BYTE ImageUpdate::getNumberBlocks(FILE *fp)
{
   CHAR asciiWord[WORD_LENGTH_IN_CHARS+1];
   CHAR *ptr;
   WORD numBlocks = 0;

   /* Skip lines until the block-count line is reached */
   moveFpToStartOfLine(fp, CTRL_HEADER_BLOCK_LINE);

   fgets(mThisLine, MAX_LINE_LENGTH, fp);

   /* This line has the format:
   * @000002 0700
   */
   /* Skip over the address by looking for the space character */
   if((ptr = strrchr(mThisLine, ' ')) != NULL)
   {
      /* copy the entire word */
      memcpy(asciiWord, (ptr + 1), WORD_LENGTH_IN_CHARS);
      /* ensure the string is terminated */
      asciiWord[WORD_LENGTH_IN_CHARS] = 0;

      /* convert the string to hex value */
      numBlocks = (WORD)strtol(asciiWord, NULL, IS_HEX);

      /* isolate the block count, which is in the top byte */
      numBlocks >>= 8;
      numBlocks &= 0xff;
   }

   return (BYTE)numBlocks;
}


/*----------------------------------------------------------------------------*
*  NAME
*      recordCskeyBlockSize
*
*  DESCRIPTION
*      calculates the cskey block size in the image file
*---------------------------------------------------------------------------*/

void ImageUpdate::recordCskeyBlockSize(FILE *fp)
{
   CHAR asciiWord[WORD_LENGTH_IN_CHARS+1];
   CHAR *ptr;

   fgets(mThisLine, MAX_LINE_LENGTH, fp);

   /* This line has the format:
   * @000002 0700
   */
   /* Skip over the address by looking for the space character */
   if((ptr = strrchr(mThisLine, ' ')) != NULL)
   {
      /* copy the entire word */
      memcpy(asciiWord, (ptr + 1), WORD_LENGTH_IN_CHARS);
      /* ensure the string is terminated */
      asciiWord[WORD_LENGTH_IN_CHARS] = 0;

      /* convert the string to hex value */
      mKeyBlockLenInWords = ((BYTE)strtol(asciiWord, NULL, IS_HEX) / 2);
   }
}


/*----------------------------------------------------------------------------*
*  NAME
*      mergeXtalTrim
*
*  DESCRIPTION
*      Merges the crystal trim to the image file
*---------------------------------------------------------------------------*/

void ImageUpdate::mergeXtalTrim(FILE *fp, char *xtalfreqtrim)
{ 
   /* Merge the xtal frequency trim.
   * This function assumes that the file pointer is already in the correct 
   * place.
   */

   /* locate the start of the data word in this line */
   findStartOfData(fp);

   /* write the new crystal freq trim */
   fwrite(xtalfreqtrim, 1, WORD_LENGTH_IN_CHARS, fp);
}

/*----------------------------------------------------------------------------*
*  NAME
*      mergeIdentityRoot
*
*  DESCRIPTION
*      Merges the identity root to the image file
*---------------------------------------------------------------------------*/

void ImageUpdate::mergeIdentityRoot(FILE *fp, char *identity_root)
{  
   WORD linecount;
   fpos_t pos;
   WORD iRootLineOffset;
   BYTE index = 0;

   iRootLineOffset = HEADER_LEN_IN_WORDS + (mNumBlocks * BLOCK_LEN_IN_WORDS) + 30;

   /* Set the file pointer back to the file start */
   fseek(fp, 0, SEEK_SET);

   /* Move to the start of the CS-key block header */
   moveFpToStartOfLine(fp, HEADER_LEN_IN_WORDS);
   /* We want the section length, two words further on */
   moveFpToStartOfLine(fp, 2);
   /* Record this location */
   recordCskeyBlockSize(fp);
   /* The next line is the block CRC */
   fgetpos(fp, &mKeyBlockCrcLine);

   /* Move to the BT address location */
   fseek(fp, 0, SEEK_SET);
   moveFpToStartOfLine(fp, iRootLineOffset);

   /* merge three parts of bdaddr (NAP, UAP and LAP) */
   for(linecount = 0; linecount < IDENTITY_ROOT_LEN_IN_WORDS; linecount++)
   {
      /* store the start of line pos */
      fgetpos(fp, &pos);

      /* go the data part of the line */
      findStartOfData(fp);

      fwrite(&identity_root[index], 1, WORD_LENGTH_IN_CHARS, fp);
      index += 4;

      /* set the file pointer back to the start of line position */
      fsetpos(fp, &pos);

      /* now jump over the line just written */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      mergeEncryptionRoot
*
*  DESCRIPTION
*      Merges the encryption root to the image file
*---------------------------------------------------------------------------*/
void ImageUpdate::mergeEncryptionRoot(FILE *fp, char *encryption_root)
{  
   WORD linecount = 0;
   fpos_t pos;
   BYTE index = 0;

   /* merge three parts of bdaddr (NAP, UAP and LAP) */
   for(linecount = 0; linecount < ENCRYPTION_ROOT_LEN_IN_WORDS; linecount++)
   {
      /* store the start of line pos */
      fgetpos(fp, &pos);

      /* go the data part of the line */
      findStartOfData(fp);

      fwrite(&encryption_root[index], 1, WORD_LENGTH_IN_CHARS, fp);
      index += 4;

      /* set the file pointer back to the start of line position */
      fsetpos(fp, &pos);

      /* now jump over the line just written */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      updateControlHdrCrc
*
*  DESCRIPTION
*      Updates the CRC of the Control header 
*---------------------------------------------------------------------------*/

void ImageUpdate::updateControlHdrCrc(FILE *fp, WORD ControlHdrCrc)
{
   CHAR crcStr[(WORD_LENGTH_IN_CHARS + 1)] = {0};

   /* Skip any comment lines */
   while(isCommentLine(fp))
   {
      /* read in and discard the line */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);
   }

   /* locate the start of the data word */
   findStartOfData(fp);

   sprintf_s(crcStr, (WORD_LENGTH_IN_CHARS + 1), "%04X", ControlHdrCrc);
  
   /* write block CRC  */
   fwrite(crcStr, 1, WORD_LENGTH_IN_CHARS, fp);
}

/*----------------------------------------------------------------------------*
*  NAME
*      readControlHdrCrc
*
*  DESCRIPTION
*      Reads the CRC of the Control header 
*---------------------------------------------------------------------------*/

WORD ImageUpdate::readControlHdrCrc(FILE *fp)
{
   CHAR crcStr[(WORD_LENGTH_IN_CHARS + 1)] = {0};
   WORD ControlHdrCrc;

   /* Skip any comment lines */
   while(isCommentLine(fp))
   {
      /* read in and discard the line */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);
   }

   /* locate the start of the data word */
   findStartOfData(fp);

   /* write cs block CRC  */
   fread(crcStr, 1, WORD_LENGTH_IN_CHARS, fp);
   ControlHdrCrc  = (WORD) strtol(crcStr, NULL, 16);
   return ControlHdrCrc;
}

/*----------------------------------------------------------------------------*
*  NAME
*      updateCsBlockCrc
*
*  DESCRIPTION
*      Updates the CRC of the CS block
*---------------------------------------------------------------------------*/

void ImageUpdate::updateCsBlockCrc(FILE *fp, WORD CsBlockCrc)
{  
   CHAR crcStr[WORD_LENGTH_IN_CHARS + 1] = {0};

   /* set back to the start of line pos */
   fsetpos (fp, &mKeyBlockCrcLine);

   /* locate the start of the data word */
   findStartOfData(fp);

   sprintf_s(crcStr, (WORD_LENGTH_IN_CHARS + 1), "%04X", CsBlockCrc);
   //_itoa_s(CsBlockCrc, crcStr, (WORD_LENGTH_IN_CHARS + 1), IS_HEX);

   /* write cs block CRC  */
   fwrite(crcStr, 1, WORD_LENGTH_IN_CHARS, fp);

   /* set the file back to initial position */
   fseek(fp, 0, SEEK_SET);
}

/*----------------------------------------------------------------------------*
*  NAME
*      readCsBlockCrc
*
*  DESCRIPTION
*      Reads the CRC of the CS block
*---------------------------------------------------------------------------*/

WORD ImageUpdate::readCsBlockCrc(FILE *fp)
{  
   CHAR crcStr[WORD_LENGTH_IN_CHARS + 1] = {0};
   WORD CsBlockHdrCrc;

   /* set back to the start of line pos */
   fsetpos (fp, &mKeyBlockCrcLine);

   /* locate the start of the data word */
   findStartOfData(fp);

   /* write cs block CRC  */
   fread(crcStr, 1, WORD_LENGTH_IN_CHARS, fp);

   /* set the file back to initial position */
   fseek(fp, 0, SEEK_SET);
   CsBlockHdrCrc = (WORD) strtol(crcStr, NULL, 16);
   
   return CsBlockHdrCrc;
}

/*----------------------------------------------------------------------------*
*  NAME
*      calculateControlHdCrc
*
*  DESCRIPTION
*      Calculates the control header CRC
*---------------------------------------------------------------------------*/

WORD ImageUpdate::calculateControlHdCrc(FILE *fp)
{
   CHAR word[5] = {0};
   WORD thisword = 0;
   fpos_t pos;
   BYTE linecount = 0;
   WORD headersLen = HEADER_LEN_IN_WORDS + (mNumBlocks * BLOCK_LEN_IN_WORDS);
   BlockCrc crcCalc;

   moveFpToStartOfLine(fp, 1);

   linecount = 1;
   while(linecount < headersLen)
   {
      /* Check that this line is not a comment */
      if(isCommentLine(fp))
      {
         /* read in and discard the line */
         fgets(mThisLine, MAX_LINE_LENGTH, fp);

         continue;
      }

      /* store the start of line pos */
      fgetpos(fp, &pos);

      /* locate the start of the data word */
      findStartOfData(fp);

      fread(word, 1, WORD_LENGTH_IN_CHARS, fp);
      word[WORD_LENGTH_IN_CHARS] = 0; // ensure termination

      /* convert the string to hex value */
      thisword = (WORD)strtol(word, NULL, IS_HEX);

      /* add the value to the CRC count */
      crcCalc.crcAddByte((BYTE)(thisword & 0xff));
      crcCalc.crcAddByte((BYTE)(thisword >> 8));

      /* move onto the next line */
      fsetpos(fp,&pos);
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      linecount++;
   }

   /* set the file back to initial position */
   fseek(fp, 0, SEEK_SET);

   return crcCalc.crcRead();
}

/*----------------------------------------------------------------------------*
*  NAME
*      getNextDataBlk
*
*  DESCRIPTION
*      Gets the next data block
*---------------------------------------------------------------------------*/
WORD ImageUpdate::getNextDataBlk(FILE* fp, fpos_t &pos)
{
   CHAR word[5] = {0};
   WORD thisword = 0;

   fgetpos(fp, &pos);

   /* locate the start of the data word */
   findStartOfData(fp);

   fread(word, 1, WORD_LENGTH_IN_CHARS, fp);
   word[WORD_LENGTH_IN_CHARS] = 0; // ensure termination

   /* convert the string to hex value */
   thisword = (WORD)strtol(word, NULL, IS_HEX);
   return thisword;
}

/*----------------------------------------------------------------------------*
*  NAME
*      readDataBlock
*
*  DESCRIPTION
*      reads and udpates the specified data block header . 
*---------------------------------------------------------------------------*/

void ImageUpdate::readDataBlock(FILE *fp, int index , BLOCK_HEADER_INFO_T *blockData)
{
   CHAR word[5] = {0};
   WORD thisword = 0;
   fpos_t pos;
   BYTE linecount = 0;
   WORD headersStart = HEADER_LEN_IN_WORDS + (index * BLOCK_LEN_IN_WORDS);
   WORD headersLen = BLOCK_LEN_IN_WORDS;
   
   moveFpToStartOfLine(fp, headersStart);

   linecount = 1;
   while(linecount < headersLen)
   {
      /* Check that this line is not a comment */
      if(isCommentLine(fp))
      {
         /* read in and discard the line */
         fgets(mThisLine, MAX_LINE_LENGTH, fp);

         continue;
      }

      /* store the start of line pos */
      blockData->offset = getNextDataBlk(fp, pos);

       /* now jump over the line just written */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      
      blockData->destination = getNextDataBlk(fp, pos);;

       /* now jump over the line just written */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      
      WORD dataLen = getNextDataBlk(fp, pos);
      blockData->length = (dataLen / 2);

       /* now jump over the line just written */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      blockData->crc = getNextDataBlk(fp, pos);;

      break;
   }

   /* set the file back to initial position */
   fseek(fp, 0, SEEK_SET);
}

/*----------------------------------------------------------------------------*
*  NAME
*      mergeBdAddr
*
*  DESCRIPTION
*      Merges the bluetooth address to the file
*---------------------------------------------------------------------------*/

void ImageUpdate::mergeBdAddr(FILE *fp, char *bdaddr)
{  
   WORD linecount;
   fpos_t pos;
   WORD addrLineOffset;

   BYTE addrpos = strlen(bdaddr);

   /* This function assumes the following:
   * 1. that the location of the device BT address is immediately
   *    after the header blocks.
   * 2. that the CS-key data is in the first header block
   */
   addrLineOffset = HEADER_LEN_IN_WORDS + (mNumBlocks * BLOCK_LEN_IN_WORDS) + 1;

   /* Set the file pointer back to the file start */
   fseek(fp, 0, SEEK_SET);

   /* Move to the start of the CS-key block header */
   moveFpToStartOfLine(fp, HEADER_LEN_IN_WORDS);
   /* We want the section length, two words further on */
   moveFpToStartOfLine(fp, 2);
   /* Record this location */
   recordCskeyBlockSize(fp);
   /* The next line is the block CRC */
   fgetpos(fp, &mKeyBlockCrcLine);

   /* Move to the BT address location */
   fseek(fp, 0, SEEK_SET);
   moveFpToStartOfLine(fp, addrLineOffset);

   /* merge three parts of bdaddr (NAP, UAP and LAP) */
   for(linecount = 0; linecount < BDADDR_LEN_IN_WORDS; linecount++)
   {
      /* store the start of line pos */
      fgetpos(fp, &pos);

      /* go the data part of the line */
      findStartOfData(fp);

      /* merge this part of the address into the line */
      addrpos = addrpos - WORD_LENGTH_IN_CHARS;
      fwrite(&bdaddr[addrpos], 1, WORD_LENGTH_IN_CHARS, fp);

      /* set the file pointer back to the start of line position */
      fsetpos(fp, &pos);

      /* now jump over the line just written */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);
   }
}

/*----------------------------------------------------------------------------*
*  NAME
*      calculateCsBlkHdCrc
*
*  DESCRIPTION
*      Calcuates the Config store Block header CRC
*---------------------------------------------------------------------------*/

WORD ImageUpdate::calculateCsBlkHdCrc(FILE *fp)
{
   WORD linecount = 0;
   CHAR word[WORD_LENGTH_IN_CHARS + 1];
   WORD CsBlkOffset = HEADER_LEN_IN_WORDS + (mNumBlocks * BLOCK_LEN_IN_WORDS);
   WORD thisword;
   fpos_t pos;
   BlockCrc crcCalc;

   /* set the file back to initial position */
   fseek(fp, 0, SEEK_SET);

   /* skip lines until blk header offset */
   moveFpToStartOfLine(fp, CsBlkOffset);

   linecount = 0;
   while(linecount < mKeyBlockLenInWords)
   {
      /* store the start of line pos */
      fgetpos (fp,&pos);

      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      /* ignore the comments */
      if(strchr(mThisLine, '/'))
      {
         continue;
      }

      /* set back to the start of line pos */
      fsetpos(fp,&pos);

      /* locate the start of the data word */
      findStartOfData(fp);

      fread(word, 1, WORD_LENGTH_IN_CHARS, fp);
      word[WORD_LENGTH_IN_CHARS] = 0; // ensure string termination

      /* convert the string to hex value */
      thisword = (WORD)strtol(word, NULL, IS_HEX);

      crcCalc.crcAddByte((BYTE)(thisword & 0xff));
      crcCalc.crcAddByte((BYTE)(thisword >> 8));

      /*set back to the start of line pos */
      fsetpos(fp, &pos);

      /* skip the current line */
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      linecount++;
   }

   return crcCalc.crcRead();
}

/*----------------------------------------------------------------------------*
*  NAME
*      checkCrc
*
*  DESCRIPTION
*      Checks the CRC of the specified data block. 
*---------------------------------------------------------------------------*/

bool ImageUpdate::checkCrc(FILE *fp, BLOCK_HEADER_INFO_T *blockData, WORD offset)
{
   CHAR word[5] = {0};
   WORD thisword = 0;
   fpos_t pos;
   WORD linecount = 0;
   WORD headersLen = (HEADER_LEN_IN_WORDS + (mNumBlocks * BLOCK_LEN_IN_WORDS)) + offset;
   BlockCrc crcCalc;
   WORD dataLen = blockData->length;

   /* Reset to the start of the file */
   fseek(fp, 0, SEEK_SET);

   moveFpToStartOfLine(fp, headersLen);

   linecount = 1;
   while(linecount <= dataLen)
   {
      /* Check that this line is not a comment */
      if(isCommentLine(fp))
      {
         /* read in and discard the line */
         fgets(mThisLine, MAX_LINE_LENGTH, fp);

         continue;
      }

      /* store the start of line pos */
      fgetpos(fp, &pos);

      /* locate the start of the data word */
      findStartOfData(fp);

      fread(word, 1, WORD_LENGTH_IN_CHARS, fp);
      word[WORD_LENGTH_IN_CHARS] = 0; // ensure termination

      /* convert the string to hex value */
      thisword = (WORD)strtol(word, NULL, IS_HEX);

      /* add the value to the CRC count */
      crcCalc.crcAddByte((BYTE)(thisword & 0xff));
      crcCalc.crcAddByte((BYTE)(thisword >> 8));

      /* move onto the next line */
      fsetpos(fp,&pos);
      fgets(mThisLine, MAX_LINE_LENGTH, fp);

      linecount++;
   }

   WORD crc  = crcCalc.crcRead();
   return ( crc == blockData->crc);
}

/*----------------------------------------------------------------------------*
*  NAME
*      ValidateImageFile
*
*  DESCRIPTION
*      Validates the image file and data blocks to be valid. 
*---------------------------------------------------------------------------*/

BOOL ImageUpdate::ValidateImageFile(FILE *fp)
{
   WORD CsBlockCrc;
   WORD ControlHdrCrc;

   WORD newCsBlockCrc;
   WORD newControlHdrCrc;

   /* Reset to the start of the file */
   fseek(fp, 0, SEEK_SET);

   /* Read the number of header blocks in this image */
   mNumBlocks = getNumberBlocks(fp);

   if(mNumBlocks == 0)
   {
      return FALSE;
   }

   /* Set the file pointer back to the file start */
   fseek(fp, 0, SEEK_SET);

   /* Move to the start of the CS-key block header */
   moveFpToStartOfLine(fp, HEADER_LEN_IN_WORDS);
   /* We want the section length, two words further on */
   moveFpToStartOfLine(fp, 2);
   /* Record this location */
   recordCskeyBlockSize(fp);
   /* The next line is the block CRC */
   fgetpos(fp, &mKeyBlockCrcLine);

   /* Move to the start location */
   fseek(fp, 0, SEEK_SET);

   newCsBlockCrc = calculateCsBlkHdCrc(fp);

   CsBlockCrc = readCsBlockCrc(fp);

   newControlHdrCrc = calculateControlHdCrc(fp);

   ControlHdrCrc = readControlHdrCrc(fp);
  
   if(newCsBlockCrc != CsBlockCrc || newControlHdrCrc != ControlHdrCrc)
   {
      /* Reset to the start of the file */
      fseek(fp, 0, SEEK_SET);
      return FALSE;
   }

   /* Reset to the start of the file */
   fseek(fp, 0, SEEK_SET);

   if(mNumBlocks > 0)
   {
      blockData = new BLOCK_HEADER_INFO_T[mNumBlocks];
      for(int i = 0; i < mNumBlocks; ++i)
      {
         readDataBlock(fp, i, &blockData[i]);
      }

      int offset = 0;
      for(int i = 0; i < mNumBlocks; ++i)
      {
         bool success = checkCrc(fp, &blockData[i], offset);
         if(success == false)
         {
            fseek(fp, 0, SEEK_SET);
            return FALSE;
         }
         offset += blockData[i].length;
      }
   }

   /* Reset to the start of the file */
   fseek(fp, 0, SEEK_SET);
   return TRUE;
}