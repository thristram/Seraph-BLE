/*! \file mem.h
 * \brief Services of the memory library
 *
 *  This file contains headers for the highly optimised
 *  memory-handling functions (mostly in assembler), plus the string,
 *  character and bit-handling functions that have ended up here by
 *  default.
 *
 * Copyright (c) 2011 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __MEM_H__
#define __MEM_H__

#include "types.h"

/*! \addtogroup MEM
 *
 *  \brief The CSR µEnergy SDK includes an optimised set of C standard library
 *  functions for copying, setting, and comparing memory blocks and strings. It
 *  also includes functions to convert between "packed" and "unpacked" arrays of
 *  bytes - features specific to the 16-bit XAP processor.
 *
 * @{
 */
/*----------------------------------------------------------------------------*
 *  MemCopyPack
 */
/*! \brief  Copy memory, turning 2 * uint8 into uint16
 *
 *  Like MemCopy but the source is a uint8 array and the destination
 *  is a uint16 array. The first word of the uint16 array is built
 *  from the first 2 words of the uint8 array (LSB first).  If the
 *  uint8's contain set bits in there msb's these are masked away.
 *
 *  \param  dest   Pointer to the destination buffer.
 *  \param  source Pointer to the source buffer.
 *  \param  length Size of the source buffer.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void MemCopyPack(uint16 *dest, const uint8 *source, uint16 length);

/*----------------------------------------------------------------------------*
 *  MemCopyUnPack
 */
/*! \brief  Copy memory, turning uint16 into 2 * uint8
 *
 *  Like MemCopy but the source is a uint16 array and the destination
 *  is a uint8 array. The first word of the uint16 array is split
 *  into the first 2 words of the uint8 array (LSB first).
 *
 * \param  dest   Pointer to the destination buffer.
 * \param  source Pointer to the source buffer.
 * \param  length Size of the destination buffer.
 *
 *
 * \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void MemCopyUnPack(uint8 *dest, const uint16 *source, uint16 length);

/*----------------------------------------------------------------------------*
 *  MemCopy
 */
/*! \brief  Copy memory, 16bit word(s) to 16 bit word(s)
 *
 *   This is implemented inline by the GCC code generator rather than as a
 *   function call. If for some reason the address of MemCopy must be taken
 *   then a wrapper around the builtin function must be written:
\code
void *memCopy(void* dest, const void* source, uint16 length)
{
    return MemCopy(dest,source,length);
}
\endcode
 *
 *  \param  dest   Pointer to the destination buffer.
 *  \param  source Pointer to the source buffer.
 *  \param  length Size of the destination buffer.
 *
 *  \return Original pointer to the destination buffer
 */
/*---------------------------------------------------------------------------*/
extern void *MemCopy(void *dest, const void *source, uint16 length);
#define MemCopy __builtin_memcpy /* use GCC intrinsic block copy */

/*----------------------------------------------------------------------------*
 *  MemSet
 */
/*! \brief  Fill memory with a specified 16 bit word
 *
 *  \param  dest   Pointer to the destination buffer.
 *  \param  value  Value to be written.
 *  \param  length Size of the destination buffer.
 *
 *  \return Original pointer to the destination buffer
 */
/*---------------------------------------------------------------------------*/
extern void *MemSet(void *dest, uint16 value, uint16 length);
#define MemSet __builtin_memset /* use GCC intrinsic block fill */

/*----------------------------------------------------------------------------*
 *  MemChr
 */
/*! \brief  Find the first occurrence of 'value' in an array of given size
 *
 *  \param  buff   Pointer to the array.
 *  \param  value  Value to be written.
 *  \param  length Size of the array.
 *
 *  \return A pointer to the character as found in memory, or NULL if not found.
 */
/*---------------------------------------------------------------------------*/
extern void *MemChr(const void *buff, int value, uint16 length);

/*----------------------------------------------------------------------------*
 *  MemCmp
 */
/*! \brief  Compare two arrays of memory
 *
 *  \param  buff1  Pointer to the 1st array.
 *  \param  buff2  Pointer to the 2nd array.
 *  \param  length Size of the array.
 *
 *  \return -1 if the first array is "less than" the second in the usual
 * string comparison sense, +1 if the first is "greater than" the
 * second, and 0 if they are equal.
 */
/*---------------------------------------------------------------------------*/
extern int MemCmp(const void *buff1, const void *buff2, uint16 length);

/*----------------------------------------------------------------------------*
 *  StrLen
 */
/*! \brief  Get the length of a string
 *
 *  \param  string Pointer to the character array.
 *
 *  \return  The number of characters in the string.
 */
/*---------------------------------------------------------------------------*/
extern uint16 StrLen(const char *string);

/*----------------------------------------------------------------------------*
 *  StrChr
 */
/*! \brief  Locate a value in a string
 *
 *  \param  string Pointer to the character array.
 *  \param  value  Search character.
 *
 *  \return A pointer to the character located in the string, or NULL if not found.
 */
/*---------------------------------------------------------------------------*/
extern char *StrChr(const char *string, int value);

/*----------------------------------------------------------------------------*
 *  StrNCopy
 */
/*! \brief Copy a limited number of characters between strings.
 *
 *  \param  dest   Pointer to the destination buffer.
 *  \param  source Pointer to the source buffer.
 *  \param  length Size of the destination buffer.
 *
 *  \return Original pointer to the destination buffer
*/
/*---------------------------------------------------------------------------*/
extern char *StrNCopy(char *dest, const char *source, uint16 length);

/*----------------------------------------------------------------------------*
 *  IsDigit
 */
/*! \brief Is the character presented a decimal digit?
 *
 *  \param  value Value to be tested.
 *
 *  \return Non-zero if the value is an ASCII digit.
*/
/*---------------------------------------------------------------------------*/
extern int IsDigit(int value);

/*----------------------------------------------------------------------------*
 *  IsSpace
 */
/*! \brief Is the character presented whitespace?
 *
 *  \param  value Value to be tested.
 *
 *  \return Non-zero if the value is an ASCII whitespace character.
*/
/*---------------------------------------------------------------------------*/
extern int IsSpace(int value);

/*----------------------------------------------------------------------------*
 *  IsUpper
 */
/*! \brief Is the character presented an uppercase roman letter?
 *
 *  \param  value Value to be tested.
 *
 *  \return Non-zero if the value is an ASCII uppercase roman character.
*/
/*---------------------------------------------------------------------------*/
extern int IsUpper(int value);

/*----------------------------------------------------------------------------*
 *  ToLower
 */
/*! \brief Convert a character to its lowercase equivalent
 *
 *  \param  value Value to be converted.
 *
 *  \return The lowercase version of the character presented if it was
 *  uppercase, otherwise the character presented.
*/
/*---------------------------------------------------------------------------*/
extern int ToLower(int value);

/*----------------------------------------------------------------------------*
 *  CountSetBits32
 */
/*! \brief Counts number of set bits in a 32 bit word.
 *
 *  \param  value Value to be tested.
 *
 *  \return Number of bits equal to 1
*/
/*---------------------------------------------------------------------------*/
extern uint16 CountSetBits32(uint32 value);

/*----------------------------------------------------------------------------*
 *  CountTransitions32
 */
/*! \brief Counts bit transitions in a 32 bit word.
 *
 *  \param  value Value to be tested.
 *
 *  \return Number of transitions between 0 and 1
*/
/*---------------------------------------------------------------------------*/
extern uint16 CountTransitions32(uint32 value);

/*! @} */
#endif  /* __MEM_H__ */
