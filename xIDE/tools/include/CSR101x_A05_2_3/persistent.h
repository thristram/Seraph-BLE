/*! \file persistent.h
 *  \brief Application services for the persistent memory store
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd 2009-2011.
 */

#ifndef __PERSISTENT_H__
#define __PERSISTENT_H__

/*! \addtogroup PM
 *
 *  \brief The CSR1000 has a small amount of internal RAM that is capable
 *  of retaining its contents for a short while after power is removed
 *  (approximately one minute). This region can be used to hold temporary
 *  configuration information for short periods of power-loss, for example that
 *  would occur while a device battery was being replaced. The firmware makes
 *  available a subset of this RAM available for application use, and provides
 *  an interface for reading, writing, erasing, and validating the contents of
 *  that region. On power-up, the application can use the validation to find
 *  out if the persistent memory contents appear to be valid.
 *
 * @{
 */
/*----------------------------------------------------------------------------*
 *      PersistentMemIsValid
 */
/*! \brief  Allows the user application to determine if the persistent memory appears valid
 *
 * NOTE: As the persistence is controlled by the slow discharge of a capacitor
 * the exact period the persistent data remains valid is not specified. Also,
 * the exact nature of corruption on the RAM around the point the capacitor
 * charge fully dissipates cannot be specified. "Validity" therefore cannot be
 * guaranteed.
 *
 * \returns  TRUE if the application region appears to hold valid data.
 */
/*---------------------------------------------------------------------------*/
extern bool PersistentMemIsValid (void);

/*----------------------------------------------------------------------------*
 *      PersistentMemRead
 */
/*! \brief  Read the contents of the application persistent memory region.
 *
 * Copy the contents of the application persistent memory region into the buffer
 * supplied by the application. The application must specify how big the buffer is;
 * if the buffer is larger than the region then only the contents of the region
 * will be returned. The actual number of words copied is returned by the function.
 * NOTE: This function will always return the contents of the region, even if those
 * contents do not appear to be valid. PersistentMemIsValid() should be used
 * to check validity.
 *
 * \param  buffer  A pointer to the the buffer supplied by the application
 * \param  num_words  Size of the buffer
 *
 * \returns  The number of words copied to the buffer.
 */
/*---------------------------------------------------------------------------*/
extern uint8 PersistentMemRead(uint16* buffer, uint8 num_words);

/*----------------------------------------------------------------------------*
 *      PersistentMemWrite
 */
/*! \brief  Write to the application persistent memory region.
 *
 * Copy the contents of the buffer supplied by the application into the
 * application persistent memory region. The application must specify how big
 * the buffer is; if the buffer is larger than the region then only enough data
 * to fill the region will be copied. The actual number of words copied is
 * returned by the function. After the data has been copied to the region,
 * the firmware will calculate validation information, which will be used by
 * PersistentMemIsValid() to determine whether or not the region is valid.
 *
 * \param  buffer  A pointer to the the buffer supplied by the application
 * \param  num_words  Size of the buffer
 *
 * \returns  The number of words copied from the buffer.
 */
/*---------------------------------------------------------------------------*/
extern uint8 PersistentMemWrite(uint16* buffer, uint8 num_words);

/*----------------------------------------------------------------------------*
 *      PersistentMemErase
 */
/*! \brief  Clear the contents of the application persistent memory region.
 *
 * Clear the contents of the application persistent memory region and indicate
 * that the application is no longer using it. The region will be cleared and
 * invalidated.
 *
 * \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void PersistentMemErase(void);

/*----------------------------------------------------------------------------*
 *      PersistentMemGetSize
 */
/*! \brief  Return the size of the application persistent memory region.
 *
 * Return the size of the application persistent memory region. This represents
 * the maximum number of words that can be stored in the region, although the
 * application can store less if needed.
 *
 * \returns  Size of memory region in words.
 */
/*---------------------------------------------------------------------------*/
extern uint8 PersistentMemGetSize(void);

/*! @} */
#endif /* __PERSISTENT_H__ */
