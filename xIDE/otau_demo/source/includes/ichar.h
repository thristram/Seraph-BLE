/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *
 *  FILE
 *      ichar.h
 *
 *  DESCRIPTION
 *      ichar functions
 *
 ******************************************************************************/

#ifndef UNICODE__ICHAR_H
#define UNICODE__ICHAR_H


/* The UNICODE__ICHAR_WIDE define selects wide character support */
/* Allow _UNICODE as an alternative for Windows platforms */

#ifdef _UNICODE
#define UNICODE__ICHAR_WIDE
#endif
#define UNICODE__ICHAR_SUPPORT_WIDE

/* Macro to select appropriate function name based on compilation options */

#ifdef UNICODE__ICHAR_WIDE
#define ICHAR_SELECT(a,w) w
#else
#define ICHAR_SELECT(a,w) a
#endif

/* Include appropriate header files */

#include <stdio.h>
#include <stdarg.h>
#if !defined(_WINCE) && !defined(_WIN32_WCE)
#  include <time.h>
#  include <string.h>
#  include <ctype.h>
#  include <stdlib.h>
#  include <locale.h>
#  ifdef __cplusplus
#    include <iosfwd>
#    include <iostream>
#    include <sstream>
#  endif
#endif /* _WINCE */
#if defined(__APPLE__) || defined(_WINCE)
    #include <stdlib.h>
#else
    #include <wchar.h>
#endif

#ifdef __cplusplus
#  include <string>
#  if !defined(WIN32) && !defined(_WINCE) && !defined(_WIN32_WCE)
typedef std::basic_string <wchar_t> wstring;
#  endif
#endif



/* Define the appropriate character type */

#ifdef UNICODE__ICHAR_WIDE
typedef wchar_t         ichar;
typedef wint_t          iint;
#define ICHAR_MIN       WCHAR_MIN
#define ICHAR_MAX       WCHAR_MAX
#define IEOF            WEOF
#else
typedef char            ichar;
typedef int             iint;
#define ICHAR_MIN       CHAR_MIN
#define ICHAR_MAX       CHAR_MAX
#define IEOF            EOF
#endif


/* Allow character and string literals to be specified appropriately */

#ifdef UNICODE__ICHAR_WIDE
#define II(x)           _II(x)
#define _II(x)          L ## x
#else
#define II(x)           x
#endif


/* Equivalents of standard C library functions */

#ifdef  __cplusplus
extern "C" {
#endif

/* String manipulation */
ichar *istrchr(const ichar *, iint);
size_t istrcspn(const ichar *, const ichar *);
ichar *istrcat(ichar *, const ichar *);
ichar *istrncat(ichar *, const ichar *, size_t);
ichar *istrcpy(ichar *, const ichar *);
ichar *istrncpy(ichar *, const ichar *, size_t);
ichar *istrpbrk(const ichar *, const ichar *);
ichar *istrrchr(const ichar *, iint);
size_t istrspn(const ichar *, const ichar *);
ichar *istrstr(const ichar *, const ichar *);
ichar *istrtok(ichar *, const ichar *);
int istrcmp(const ichar *, const ichar *);
int istricmp(const ichar *, const ichar *);
int istrncmp(const ichar *, const ichar *, size_t);
int istrnicmp(const ichar *string1, const ichar *string2, size_t count);
size_t istrlen(const ichar *);
ichar *istrdup(const ichar *);
#if !defined _WINCE && !defined _WIN32_WCE
int istrcoll(const ichar *, const ichar *);
size_t istrxfrm(ichar *, const ichar *, size_t);
#endif

/* Character manipulation */
int iisalnum(iint);
int iisalpha(iint);
int iisdigit(iint);
int iisgraph(iint);
int iislower(iint);
int iisprint(iint);
int iispunct(iint);
int iisspace(iint);
int iisupper(iint);
int iisascii(iint);
int iiscntrl(iint);
int iisxdigit(iint);
int itoupper(iint);
int itolower(iint);

/* Formatted I/O */
int iprintf(const ichar *, ...);
int ifprintf(FILE *, const ichar *, ...);
int isprintf(ichar *, size_t, const ichar *, ...);
int ivprintf(const ichar *, va_list);
int ivfprintf(FILE *, const ichar *, va_list);
int ivsprintf(ichar *, size_t, const ichar *, va_list);
/*
    The standard library does not include va_arg versions of the formatted
    input functions:
        int iscanf(const ichar *, ...);
        int ifscanf(FILE *, const ichar *, ...);
        int isscanf(const ichar *, const ichar *, ...);
*/

/* Unformatted I/O */
iint ifgetc(FILE *);
ichar *ifgets(ichar *, int, FILE *);
iint ifputc(iint, FILE *);
iint ifputs(const ichar *, FILE *);
iint igetc(FILE *);
iint igetchar(void);
/* ichar *igets(ichar *); Use ifgset()*/
iint iputc(iint, FILE *);
iint iputchar(iint);
iint iputs(const ichar *);
iint iungetc(iint, FILE *);

/* String conversion */
double istrtod(const ichar *, ichar **);
long istrtol(const ichar *, ichar **, int);
unsigned long istrtoul(const ichar *, ichar **, int);
int iatoi(const ichar *);
long iatol(const ichar *);
double iatof(const ichar *);

/* Command execution */
#if !defined _WINCE && !defined _WIN32_WCE
int isystem(const ichar *);
ichar *igetenv(const ichar *);
#endif

/* Time processing */
#if !defined _WINCE && !defined _WIN32_WCE
ichar *iasctime(const struct tm *);
ichar *ictime(const time_t *);
size_t istrftime(ichar *, size_t, const ichar *, const struct tm *);
#endif

/* File handling */
FILE *ifopen(const ichar *, const ichar *);
FILE *ifreopen(const ichar *, const ichar *, FILE *);
#if !defined _WINCE && !defined _WIN32_WCE
void iperror(const ichar *);
/* ichar *itmpnam(ichar *); Use imkstemp() */
int iremove(const ichar *);
int irename(const ichar *, const ichar *);
#endif

/* Locale handling */
#if !defined _WINCE && !defined _WIN32_WCE
ichar *isetlocale(int, const ichar *);
#endif

#ifdef  __cplusplus
}
#endif


/* A few C++ library additions (implemented in inarrow.cpp) */

#if defined(__cplusplus)

/* A string type for C++ */
#ifdef UNICODE__ICHAR_WIDE
typedef std::wstring    istring;
#else
typedef std::string     istring;
#endif

/* stream types */
#ifdef UNICODE__ICHAR_WIDE
typedef std::wostream    iiostream;
typedef std::wistream    iiistream;
typedef std::wofstream   iiofstream;
typedef std::wifstream   iiifstream;
#else
typedef std::ostream     iiostream;
typedef std::istream     iiistream;
typedef std::ofstream    iiofstream;
typedef std::ifstream    iiifstream;
#endif

/* A string stream types for C++. Prefix with ii due to clash */
#ifdef UNICODE__ICHAR_WIDE
typedef std::wistringstream iiistringstream;
typedef std::wostringstream iiostringstream;
typedef std::wstringstream  iistringstream;
#else
typedef std::istringstream  iiistringstream;
typedef std::ostringstream  iiostringstream;
typedef std::stringstream   iistringstream;
#endif

/* String conversions */
std::string inarrow(const std::string &);
istring icoerce(const std::string &);
#ifdef UNICODE__ICHAR_SUPPORT_WIDE
std::wstring iwiden(const std::string &);
std::string inarrow(const std::wstring &);
istring icoerce(const std::wstring &);
std::wstring iwiden(const std::wstring &);
#endif

/* Filename conversions */
std::string inarrow_filename(const std::string &);
istring icoerce_filename(const std::string &);
#ifdef UNICODE__ICHAR_SUPPORT_WIDE
std::wstring iwiden_filename(const std::string &);
std::string inarrow_filename(const std::wstring &);
istring icoerce_filename(const std::wstring &);
std::wstring iwiden_filename(const std::wstring &);
#endif

/* Standard I/O streams */
#ifdef UNICODE__ICHAR_WIDE
#define icin            std::wcin
#define icout           std::wcout
#define icerr           std::wcerr
#define iclog           std::wclog
#else
#define icin            std::cin
#define icout           std::cout
#define icerr           std::cerr
#define iclog           std::clog
#endif

#endif

/* C wrappers around the above C++ functions */
#ifdef __cplusplus
extern "C"
{
#endif
char *iinarrowdup(const ichar *string);
ichar *iccoercedup(const char *string);
#ifdef __cplusplus
}
#endif

#endif
