/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      ImageCRC.h
 *
 *  DESCRIPTION
 *      This module calculates the CRC used in the application image header 
 *      blocks.
 *
 ******************************************************************************/
#ifndef __IMAGECRC_H
#define __IMAGECRC_H

/*============================================================================*
 *  Public Class Definitions
 *============================================================================*/

class BlockCrc
{
public:
    BlockCrc(void);

    /* Reset the CRC */
    void crcReset(void);

    /* Update the CRC with the supplied byte */
    void crcAddByte(BYTE byte);

    /* Read the final CRC */
    WORD crcRead(void);

private:

    /*============================================================================*
     *  Private Definitions
     *============================================================================*/
    #define INITIAL_REMAINDER       0x0000U
    #define FINAL_XOR_VALUE         0x0000U

    /*============================================================================*
     *  Private Data
     *============================================================================*/

    /* The current CRC result */
    WORD mCrcRemainder;

    /* The value look-up table */
    static const WORD crcLookupTable[];

    /*============================================================================*
     *  Private Function Implementations
     *============================================================================*/

    static WORD reflect(WORD original, BYTE nBits)
    {
        WORD  reflection;
        WORD  reflectedBit;
        BYTE  bitCount;

        /* Generate the initial reflection and reflected bit */
        reflection = 0x0UL ;
        reflectedBit = 1UL << ( nBits - 1U ) ;

        /* Reflect the data about the center bit. */
        for ( bitCount = 0U; bitCount < nBits; bitCount++ )
        {
            /* If the LSB is set, set the reflection of it */
            if ( original & 0x01UL )
            {
                reflection |= reflectedBit;
            }

            /* Next bit */
            original >>= 1U;
            reflectedBit >>= 1U;
        }

        return ( reflection );
    }
};

#endif /* __IMAGECRC_H */