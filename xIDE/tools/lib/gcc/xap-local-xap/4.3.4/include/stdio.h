
#ifndef __STDIO_H

#define __STDIO_H

#include <stdarg.h>

int printf(const char *, ...);
int vprintf(const char *, va_list arg);
int sprintf(char *, const char *, ...);
int vsprintf(char *, const char *, va_list arg);
int putchar(int c);
int puts(const char *s);

#endif
