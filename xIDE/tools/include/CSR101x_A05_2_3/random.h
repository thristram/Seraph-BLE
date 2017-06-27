/*!
 *  \file   random.h
 *  \brief  Generators for pseudo-random data sequences
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

/*! \addtogroup RND
 * @{
 */
/*!
 *  \brief      Obtain a 16-bit random number
 *  \returns    A 16-bit pseudo-random number
 *
 *  Obtain a 16-bit random number from the hardware's random number generator.
 */
extern uint16 Random16(void);

/*!
 *  \brief      Obtain a 32-bit random number
 *  \returns    A 32-bit pseudo-random number
 *  
 *  Obtain a 32-bit random number by calling Random16() twice.
 */
#define Random32()    ((((uint32)Random16()) << 16) | Random16())
 
/*!
 *  \brief      Generate prbs data using two feed back bits
 *  \returns    Void
 *  \param     feed_back_max    See ITU-T 0.150 for details.
 *  \param     feed_back_min
 *  \param     data             Pointer to the buffer where the numbers will be placed.
 *  \param     size             How many numbers to generate. Limits 0 to 2**15-1.
 *
 *  Generates prbs data for a maximum of 2**15 -1 states as per ITU-T 0.150
 *  standard using 2 feedback bits.
 *
 *  If you wish to use default values for (feed_back_max, feed_back_min), use
 *  RandomGenPrbs9 or 15.
 */
extern void RandomGenPrbs(uint8 feed_back_max,
                          uint8 feed_back_min,
                          uint8 *data,
                          uint16 size);

/*!
 *  \brief  Generate PRBS9 data
 *  \param     data             Pointer to the buffer where the numbers will be placed.
 *  \param     size             How many numbers to generate. Limits 0 to 2**15-1.
 *
 * Implemented as a call to RandomGenPrbs with (feed_back_max, feed_back_min) = (8, 4).
 */
#define RandomGenPrbs9(data, length) RandomGenPrbs(8, 4, data, length)

/*!
 *  \brief  Generate PRBS15 data
 *  \param     data             Pointer to the buffer where the numbers will be placed.
 *  \param     size             How many numbers to generate. Limits 0 to 2**15-1.
 *
 * Implemented as a call to RandomGenPrbs with (feed_back_max, feed_back_min) = (14, 13).
 */
#define RandomGenPrbs15(data, length) RandomGenPrbs(14, 13, data, length)

/*! @} */
#endif /* __RANDOM_H__ */
