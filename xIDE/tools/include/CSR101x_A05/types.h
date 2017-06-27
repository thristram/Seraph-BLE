/*! \file types.h
 *
 *  \brief Commonly used typedefs
 *
 *  This file should be #included by every C or C++ source file, whether
 *  it is being included in the firmware or host-side code.
 *
 * Copyright (c) 2009 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef TYPES_H
#define TYPES_H

/* All currently supported platforms define the basic types to be
 * compatible sizes (although note that (u)int8 is actually 16-bit on the
 * Xap). Conditional compilation may be required to support additional
 * platforms. 
 */

#if defined(_WIN32)
typedef unsigned char uint8;    /*!< unsigned byte (8-bits) */
typedef unsigned short uint16;  /*!< unsigned word (16-bits) */
typedef unsigned int uint32;    /*!< unsigned long (32-bits) */
typedef uint32 uint24;          /*!< unsigned long (24-bits) */

typedef signed char     int8;
typedef signed short    int16;
typedef signed long     int32;
#else
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef uint32          uint24;

typedef signed char     int8;
typedef signed short    int16;
typedef signed long     int32;
#endif

#if !defined(_WIN32)
#ifndef MATLAB
typedef unsigned        bool;
#define FALSE 0
#define TRUE 1
#endif
#endif

#include "macros.h"

/* Include system-wide status code definitions */
#include "status.h"
#endif
