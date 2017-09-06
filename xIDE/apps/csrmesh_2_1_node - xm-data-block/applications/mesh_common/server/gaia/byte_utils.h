/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file byte_utils.h
 * \brief utilities used for frame parsing and creation
 */
#ifndef __BYTE_UTILS_H__
#define __BYTE_UTILS_H__

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <bt_event_types.h> /* Type definitions for Bluetooth events */
#include <status.h>         /* System wide status codes */

#define BYTES_TO_WORDS(_b_)     ((_b_ >> 1) + (_b_ % 2))


/*============================================================================*
 *  Public Data Types
 *============================================================================*/

#define HIGH(v)     ((v) >> 8)
#define LOW(v)      ((v) & 0x00FF)


/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/*! \brief Insert an 8bit value in to an array
 *
 * \param[out] dst destination buffer
 * \param[in] byteIndex offset in destination buffer
 * \param[in] val 8bit value to insert
 * \returns Number of octets inserted
 */
extern uint16 SetUint8InArray(uint8 *dst, uint16 byteIndex, uint8 val);

/*! \brief Insert an 16bit value in to an array
 *
 * \param[out] dst destination buffer
 * \param[in] byteIndex offset in destination buffer
 * \param[in] val 16bit value to insert
 * \returns Number of octets inserted
 */
extern uint16 SetUint16InArray(uint8 *dst, uint16 byteIndex, uint16 val);

/*! \brief Insert an 32bit value in to an array
 *
 * \param[out] dst destination buffer
 * \param[in] byteIndex offset in destination buffer
 * \param[in] val 32bit value to insert
 * \returns Number of octets inserted
 */
extern uint16 SetUint32InArray(uint8 *dst, uint16 byteIndex, uint32 val);

/*! \brief Extract an 8bit value from an array
 *
 * \param[in] src source buffer
 * \returns Number of octets extracted
 */
extern uint8 GetUint8FromArray(const uint8 *src);

/*! \brief Extract a 16bit value from an array
 *
 * \param[in] src source buffer
 * \returns Number of octets extracted
 */
extern uint16 GetUint16FromArray(const uint8 *src);

/*! \brief Extract a 32bit value from an array
 *
 * \param[in] src source buffer
 * \returns Number of octets extracted
 */
extern uint32 GetUint32FromArray(const uint8 *src);

/*! \brief Swap the order of pairs of bytes in a buffer. If
 * it is not even, then the function will do nothing.
 *
 *  NOTE: The buffer is expected to be an even number of bytes
 * \param[out] count size of buffer
 * \param[in] source source buffer
 * \param[out] destination destination buffer
 */
extern void SwapBytes(uint16 count, uint8* source, uint8* destination);

/*! \brief Retrieves the first N bytes from data stream (Big Endian)
 *
 *  This function retrieves the first N bytes from data stream (Big Endian)
 * \param[out] dst Destination buffer
 * \param[in] dst_index Start index for destination buffer
 * \param[in] src Source buffer
 * \param[in] src_index Start index for destination buffer
 * \param[in] transfer_length Number of bytes to transfer
 * \returns Nothing
 *
 */
extern void GetNBytesFromStream(uint16* dst, uint16 dst_index,
                                         uint8* src, uint16 src_index ,
                                         uint16 transfer_length);

/*! \brief Sets the first N bytes of data stream (Big Endian)
 *
 *  This function sets the first N bytes of data stream (Big Endian)
 * \param[out] dst Destination buffer
 * \param[in] dst_index Start index for destination buffer
 * \param[in] src Source buffer
 * \param[in] src_index Start index for destination buffer
 * \param[in] transfer_length Number of bytes to transfer
 * \returns Nothing
 *
 */
extern void SetNBytesInStream(uint8* dst, uint16 dst_index,
                                       uint16* src, uint16 src_index,
                                       uint16 transfer_length);

/*! \brief BufReadUint16() implementation in big-endian
 *
 *  Similar to the library function BufReadUint16() except returns in
 *  big-endian format instead of little endian format
 * \param[in] buf Double ponter to data to read
 * \returns Value read
 *
 */
extern uint16 BufReadUint16BE(uint8 **buf);

#endif /* __BYTE_UTILS_H__ */
