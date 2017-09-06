/*! \file types.h
 *
 *  \brief Commonly used typedefs
 *
 *  This file should be #included by every C or C++ source file, whether
 *  it is being included in the firmware or host-side code.
 *
 * Copyright (c) 2009 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <types.h>

typedef struct{
  uint8 fover: 1;
  uint8 frequest: 1;
  uint8 count: 14;  
}TIME16;

typedef union{
  uint8 word;
  TIME16 time;
}UTIME16;

struct FLAG
{
  uint8  bit15: 1;
  uint8  bit14: 1;
  uint8  bit13: 1;
  uint8  bit12: 1;
  uint8  bit11: 1;
  uint8  bit10: 1;
  uint8  bit9: 1;
  uint8  bit8: 1;
  uint8  bit7: 1;
  uint8  bit6: 1;
  uint8  bit5: 1;
  uint8  bit4: 1;
  uint8  bit3: 1;
  uint8  bit2: 1;
  uint8  bit1: 1;
  uint8  bit0: 1;
};

union FLAGS
{
   uint8 byte;
   struct FLAG bit;
};

typedef struct{
  unsigned char b7: 1;
  unsigned char b6: 1;
  unsigned char b5: 1;
  unsigned char b4: 1;
  unsigned char b3: 1;
  unsigned char b2: 1;
  unsigned char b1: 1;
  unsigned char b0: 1;            
}BYTE_FIELD;

typedef union{
  unsigned char byte;
  BYTE_FIELD bit;
}TYPE_BYTE;

#endif
