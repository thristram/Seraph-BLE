/*! \file buf_utils.h
 *  \brief Functions for reading and writing a little-endian byte buffer.
 *
 *  All the following functions take a pointer to the buffer pointer
 *  as their first argument, and update the pointer to move past the
 *  bytes read or written.
 *
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __BUF_UTILS_H__
#define __BUF_UTILS_H__

#include "types.h"

/*============================================================================*
Public Function Implementations
*============================================================================*/

/*----------------------------------------------------------------------------*
 *  BufReadUint8
 */
/*! \brief Read a byte from the buffer and advance the buffer pointer.
 *
 *  \param  buf  A pointer to the buffer pointer.
 *
 *  \returns  The byte read.
 */
/*---------------------------------------------------------------------------*/
extern uint8 BufReadUint8(uint8 **buf);

/*----------------------------------------------------------------------------*
 *  BufReadUint16
 */
/*! \brief Read a little-endian 16-bit word from the buffer and
 *      advance the buffer pointer.
 *
 *  \param  buf  A pointer to the buffer pointer.
 *
 *  \returns  The word read.
 */
/*---------------------------------------------------------------------------*/
extern uint16 BufReadUint16(uint8 **buf);

/*----------------------------------------------------------------------------*
 *  BufReadUint32
 */
/*! \brief Read a little-endian 32-bit word from the buffer and
 *      advance the buffer pointer.
 *
 *  \param  buf  A pointer to the buffer pointer.
 *
 *  \returns  The word read.
 */
/*---------------------------------------------------------------------------*/
extern uint32 BufReadUint32(uint8 **buf);

/*----------------------------------------------------------------------------*
 *  BufWriteUint8
 */
/*! \brief Write a byte to the buffer and advance the buffer pointer.
 *
 *  \param  buf  A pointer to the buffer pointer.
 *
 *  \param  val  The byte to write.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void BufWriteUint8(uint8 **buf, uint8 val);

/*----------------------------------------------------------------------------*
 *  BufWriteUint16
 */
/*! \brief Write a 16-bit word to the buffer in little-endian byte
 *      order and advance the buffer pointer.
 *
 *  \param  buf  A pointer to the buffer pointer.
 *
 *  \param  val  The word to write.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void BufWriteUint16(uint8 **buf, uint16 val);

/*----------------------------------------------------------------------------*
 *  BufWriteUint32
 */
/*! \brief Write a 32-bit word to the buffer in little-endian byte
 *      order and advance the buffer pointer.
 *
 *  \param  buf  A pointer to the buffer pointer.
 *
 *  \param  val  A pointer to the word to write.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void BufWriteUint32(uint8 **buf, uint32 *p_val);

#endif /* __BUF_UTILS_H__ */

