/*! \file macros.h
 *
 *  \brief Commonly used macros
 *
 *  This file should be #included by most C or C++ source files, whether
 *  it is being included in the firmware or host-side code.
 *
 *  Copyright (c) Cambridge Silicon Radio Ltd. 2009-2011
*/

#ifndef MACROS_H
#define MACROS_H


/* Boolean values (never test for equality with TRUE) */
#ifndef FALSE
#define FALSE           (0)
#endif

#ifndef TRUE
#define TRUE            (1)
#endif


/* Null pointer (also in <stddef.h>, <stdio.h>, <stdlib.h> and <string.h>) */
#ifndef NULL
#define NULL            (0)
#endif


/* Inlined minimum, maximum and absolute (may evaluate expressions twice) */
#ifndef max
#define max(a,b)        (((a) < (b)) ? (b) : (a))
#endif

#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif

#ifndef abs
#define abs(a)          (((a) < 0) ? -(a) : (a))
#endif

/* Below MIN & MAX macros are used by ATT code which has BC as the baseline */
#ifndef MAX
#define MAX(a,b) max(a,b)
#endif

#ifndef MIN
#define MIN(a,b) min(a,b)
#endif


/* Loop forever */
#ifndef forever
#define forever         for(;;)
#endif

/* Shut lint up */
#ifndef UNUSED
#define UNUSED(x)       (void) x
#endif


/* The "inline" keyword is only valid in C++ (or with gcc) */
#if !defined(__cplusplus) && !defined(__GNUC__)
#ifndef inline
#define inline
#endif
#endif


/* Function attributes are only supported by GCC */
#ifdef __GNUC__
#define GCC_ATTRIBUTE(x) __attribute__ ((x))
#else
#define GCC_ATTRIBUTE(x)
#endif
#define NORETURN GCC_ATTRIBUTE(__noreturn__)


/* Compile-time assertions: force the compilation to abort if the
 * expression is false.  Unlike #if, this can test enums.
 */
#ifndef COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(expr, msg) struct compile_time_assert_ ## msg { \
    int compile_time_assert_ ## msg [1 - (!(expr))*2]; \
}
#endif

#endif
