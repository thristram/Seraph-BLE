/****************************************************************************/
/*
 * Copyright 2010 - 2016 Qualcomm Technologies International, Ltd.
 */
/*! \file
 *
 *  \brief build identifier functions
 *
 *  Holds automatically set strings and version number identifying the
 *  (released) build.
 *
 */
/*! \cond ENABLE_FW_INTERNAL*/
/*! \details
 *  This file needs to support old id.h for baldrick/robinson releases.
 *  #ifdef CHIP_BASE_FLASHHEART has been used to handle this.  This is required because
 *  the release build process copies id.h from main to the release branch. This should
 *  be cleaned up by B-175619.
 *
 */
/*! \endcond */
/****************************************************************************/

#ifndef	__ID_H__
#define	__ID_H__
 
#include "types.h"

/*! \addtogroup BID
 *
 *  \brief Information about application build, firmware build, and Bluetooth
 *         version supported.
 * @{
 */


/*==========================================================================*
    Public Definitions
*===========================================================================*/

/*! \brief Firmware abbreviated ID string.
 *
 * Not currently used by the FW but retained to keep build system happy.
*/
#define BUILD_IDENTIFIER_STRING "bdkSDK2_6_1_275_16..."

/*! \brief Firmware full ID string.
 *
 * This is stored in ROM constant data and can retrieved by various means
 * (e.g. direct access via SLT, UCI command, etc.) Non-release builds will use
 * a generic "Unknown_TIMESTAMP" string rather than a formal label like below.
*/
#define BUILD_IDENTIFIER_STRING_FULL "bdk_SDK_2_6_1_275_________1606221138"

/*! \brief Firmware build id.
*/
#define BUILD_IDENTIFIER_INTEGER_BASE 1525


/*==========================================================================*
    Public Function Prototypes
*===========================================================================*/

/*! \cond ENABLE_FW_INTERNAL */
#ifndef CHIP_BASE_ROBINSON
/*! \endcond */

/****************************************************************************/
/*! \brief Get build identifier of ROM part of firmware.
 *
 *  \returns Build Identifier of ROM
 ****************************************************************************/
extern uint16 IdGetRomBuild( void );

/****************************************************************************/
/*! \brief Set build identifier of Application.  (Used by UCI)
 *  \param[in] id  Unique id to identify an application build.
 ****************************************************************************/
extern void IdSetAppBuild( uint16 id );

/****************************************************************************/
/*! \brief Set location of application's build identifier string.  (Used by UCI)
 *  \param[in] id_str  Unique string to identify an application build.
 *  \param[in] str_len Length of string passed in.
 *  \note Parameter id_str should point to a string that will exist permanently in
 *        memory.
 ****************************************************************************/
extern void IdSetAppString( const char *id_str, uint16 str_len );

/****************************************************************************/
/*! \brief Supported version of Bluetooth specification.
           Refer to Bluetooth assigned numbers for Link Layer version values.
 *
 *  \returns Link Layer version
 ****************************************************************************/
uint16 IdGetLlVerBtle( void );

/*! \cond ENABLE_FW_INTERNAL */
#else /* Legacy definitions for CSR101x */
/*! \endcond */
/*! \cond CHIP_BASE_ROBINSON */

/* Manually set string embedded in the code. */
extern const char build_identifier_string[];

/* Manually set string embedded in the code. */
extern const char build_identifier_string_full[];

#ifdef ROM_BUILD
/* Manually set string embedded in the RAM code. */
extern const char ram_build_identifier_string[];
#endif /* ROM_BUILD */

/*! \brief string embedded in the code when build string unknown.
*/
#define BUILD_IDENTIFIER_STRING_UNKNOWN "Unknown"
#define BUILD_IDENTIFIER_STRING_FULL_UNKNOWN "Unknown"

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

/****************************************************************************/
/*! \brief Access functions for the patched FW and APP build id's and strings
 */
extern uint16 IdGetPatchedFwBuild( void );
extern void   IdSetPatchedFwBuild( uint16 id );
extern uint16 IdGetAppBuild( void );
extern void   IdSetAppBuild( uint16 id );
extern uint16 IdGetPatchedFwString( const uint16 **id );
extern uint16 IdGetAppString( const uint16 **id );

/*! \endcond */
/*! \cond ENABLE_FW_INTERNAL */
#endif  /* CHIP_BASE_ROBINSON */
/*! \endcond */

/*! @} */

#endif  /* __ID_H__ */
