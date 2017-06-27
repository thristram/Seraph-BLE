/*! \file id.h
 *  \brief build identifier functions
 *
 *  Holds automatically set strings and version number identifying the
 *  (released) build.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd 2009-2011.
 */

#ifndef	__ID_H__
#define	__ID_H__
 
#include "types.h"

/*! \addtogroup BID
 * @{
 */
/* Manually set string embedded in the code. */
extern const char build_identifier_string[];

/* Manually set string embedded in the code. */
extern const char build_identifier_string_full[];

#ifdef ROM_BUILD
/* Manually set string embedded in the RAM code. */
extern const char ram_build_identifier_string[];
#endif /* ROM_BUILD */

/*! \brief Abbreviated string embedded in the code.
*/
#define BUILD_IDENTIFIER_STRING "bdkSDK2_3_0_136_14..."
#define BUILD_IDENTIFIER_STRING_UNKNOWN "Unknown"

/*! \brief Full string embedded in the code.
*/
#define BUILD_IDENTIFIER_STRING_FULL "bdk_SDK_2_3_0_136_1401211458 2014-01-21"
#define BUILD_IDENTIFIER_STRING_FULL_UNKNOWN "Unknown"

/* Numeric identifier embedded in the code. */
#define BUILD_IDENTIFIER_INTEGER_BASE 920

#if defined(BLE_RELEASE)
#define BUILD_IDENTIFIER_INTEGER BUILD_IDENTIFIER_INTEGER_BASE
#else
#define BUILD_IDENTIFIER_INTEGER (BUILD_IDENTIFIER_INTEGER_BASE | 0x8000)
#endif

/*! \brief Supported version of Bluetooth specification */
uint16 IdGetLlVerBtle(void);


/*----------------------------------------------------------------------------*
 *  IdGetBuild
 */
/*! \brief Get build identifier.
 *
 *  \return An identifier for the current build stored in ROM.
 */
/*---------------------------------------------------------------------------*/
extern uint16 IdGetBuild( void );

#ifdef ROM_BUILD
/****************************************************************************
NAME
	IdGetRomBuild  -  get build identifier of ROM
*/

extern uint16 IdGetRomBuild(void);

#endif /* ROM_BUILD */

/*! @} */
#endif	/* __ID_H__ */
