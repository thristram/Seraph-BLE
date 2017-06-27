
#ifndef __STDLIB_H

#define __STDLIB_H

#include <stddef.h>

#ifndef NULL
#define NULL ((void *)0)
#endif /* ndef NULL */

void *malloc(size_t);
void free(void *);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);

int atoi(const char *);

void exit(int);

#endif
