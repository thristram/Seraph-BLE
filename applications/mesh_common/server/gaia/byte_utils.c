/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file byte_utils.c
 * \brief utilities used for frame parsing and creation
 */
#include "byte_utils.h"


/*-----------------------------------------------------------------------------
 *  NAME
 *      SetUint8InArray
 *
 *  DESCRIPTION
 *      Inserts an 8bit value in to an array.
 *
 *  RETURNS:
 *      Number of octets converted
 *
 *----------------------------------------------------------------------------*/
extern uint16 SetUint8InArray(uint8 *dst, uint16 byteIndex, uint8 val)
{
    dst[byteIndex] = val;

    return 1;
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      SetUint16InArray
 *
 *  DESCRIPTION
 *      Inserts a 16bit value in to an array.
 *
 *  RETURNS:
 *      Number of octets converted
 *
 *----------------------------------------------------------------------------*/
extern uint16 SetUint16InArray(uint8 *dst, uint16 byteIndex, uint16 val)
{
    dst[byteIndex] = val >> 8;
    dst[byteIndex+1] = val;

    return 2;
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      SetUint32InArray
 *
 *  DESCRIPTION
 *      Inserts a 32bit value in to an array.
 *
 *  RETURNS:
 *      Number of octets converted
 *
 *----------------------------------------------------------------------------*/
extern uint16 SetUint32InArray(uint8 *dst, uint16 byteIndex, uint32 val)
{
    dst[byteIndex] = val >> 24;
    dst[byteIndex+1] = val >> 16;
    dst[byteIndex+2] = val >> 8;
    dst[byteIndex+3] = val;

    return 4;
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GetUint8FromArray
 *
 *  DESCRIPTION
 *      Extracts an 8bit value from an array
 *
 *  RETURNS:
 *      Number of octets extracted
 *
 *----------------------------------------------------------------------------*/
extern uint8 GetUint8FromArray(const uint8 *src)
{
    return src[0];
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GetUint16FromArray
 *
 *  DESCRIPTION
 *      Extracts a 16bit value from an array
 *
 *  RETURNS:
 *      Number of octets extracted
 *
 *----------------------------------------------------------------------------*/
extern uint16 GetUint16FromArray(const uint8 *src)
{
    uint16 val = 0;

    val = (uint16)src[0] << 8;
    val |= src[1];

    return val;
}

/*-----------------------------------------------------------------------------
 *  NAME
 *      GetUint32FromArray
 *
 *  DESCRIPTION
 *      Extracts a 32bit value from an array
 *
 *  RETURNS:
 *      Number of octets extracted
 *
 *----------------------------------------------------------------------------*/
extern uint32 GetUint32FromArray(const uint8 *src)
{
    uint32 val = 0;

    val = ((uint32)src[0] & 0xff)<< 24;
    val |= ((uint32)src[1] & 0xff)<< 16;
    val |= ((uint32)src[2] & 0xff) << 8 ;
    val |= ((uint32)src[3] & 0xff) ;

    return val;
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      SwapBytes
 *
 *  DESCRIPTION
 *       Swap the order of pairs of bytes in a buffer. If it is not even,
 *       then the function will do nothing.
 *
 *  RETURNS:
 *      Number of octets extracted
 *
 *----------------------------------------------------------------------------*/
extern void SwapBytes(uint16 count, uint8* source, uint8* destination)
{
    uint8 i;

    /* even counts only */
    if ( count % 2 == 1 )
        return;

    for(i = 0 ; i < count; i+=2)
    {
        destination[i+1] = source[i];
        destination[i] = source[i+1];
    }
}


/*-----------------------------------------------------------------------------
 *  NAME
 *      ByteUtilsGetNBytesFromStream
 *
 *  DESCRIPTION
 *      Retrieves the first N bytes from data stream (Big Endian)
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void GetNBytesFromStream(    uint16* dst, uint16 dst_index,
                                    uint8* src, uint16 src_index ,
                                    uint16 transfer_length)
{
    uint16 count;

    for(count = transfer_length ; count > 0; count = count-2)
    {
        dst[dst_index]  =  (src [src_index + count - 1]) |
                           (src [src_index + count - 2] << 8);
        dst_index++;
    }

}

/*-----------------------------------------------------------------------------
 *  NAME
 *      ByteUtilsSetNBytesInStream
 *
 *  DESCRIPTION
 *      Sets the first N bytes of data stream (Big Endian)
 *
 *  RETURNS:
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void SetNBytesInStream(  uint8* dst, uint16 dst_index,
                                uint16* src, uint16 src_index ,
                                uint16 transfer_length)
{
    uint16 count;

    for(count = transfer_length ; count > 0; count -=2)
    {
        dst[dst_index++] = HIGH(src [src_index + (count/2 ) - 1]) ;
        dst[dst_index++] = LOW(src [src_index + (count/2 ) - 1]);
    }

}


/*-----------------------------------------------------------------------------
 *  NAME
 *      BufReadUint16BE
 *
 *  DESCRIPTION
 *      Similar to the library function BufReadUint16() except reads data in
 *      big-endian format instead of little endian format
 *
 *  RETURNS:
 *      uint16 value from the first two bytes of the buffer. Advances the
 *      pointer to buffer.
 *
 *----------------------------------------------------------------------------*/
extern uint16 BufReadUint16BE(uint8 **buf)
{
    uint8 *p = *buf;
    uint16 r;
    r = *p++ << 8;
    r |= *p++;
    *buf = p;
    return r;
}
