/*!
   Copyright (c) 2009 - 2013 Qualcomm Technologies International, Ltd.
   All Rights Reserved.
   Qualcomm Technologies International, Ltd. Confidential and Proprietary.

\file   crypt.h
*/
#ifndef __CRYPT_H__
#define __CRYPT_H__

/*! \addtogroup SM
 * @{
 */

/* \brief The crypto library provides access to the AES encryption hardware. This
 * allows applications to take advantage of hardware accelerated AES encryption.
 */

/*! \brief AES encrypt a block of data
 *
 * Performs an in-place encryption of supplied plain-text data. The mode of
 * operation is Electronic Code Book, and only one block is encrypted at a time.
 * Data and key should be stored as word-wise little-endian.
 *      Example:
 *
 *      The key 0x000102030405060708090a0b0c0d0e0f (MSB -> LSB) is stored
 *
 *      uint16 key[] = {0x0e0f, 0x0c0d, 0x0a0b, 0x0809, 0x0607, 0x0405, 0x0203, 0x0001};

 *
 * \param      key  pointer to 8 word (128 bit) encryption key
 * \param[in]  data pointer to 8 word (128 bit) data block to be encrypted
 * \param[out] data pointer to 8 word (128 bit) encrypted output
 */
extern void SMEncryptRawAes(uint16 *key, uint16 *data);
/*! @} */

#endif /* __CRYPT_H__ */

