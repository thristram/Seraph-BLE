
#ifndef __STRING_H

#define __STRING_H

#include <stddef.h>

char *strcat(char *, const char *);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);

int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);

char *strchr(const char *, int);
char *strrchr(const char *, int);

void *memchr(const void *, int, size_t);

size_t strlen(const char *);

void *memset(void *, int, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
int memcmp(const void *x, const void *y, size_t n);

size_t strspn(const char *, const char *);
size_t strcspn(const char *, const char *);

char *strpbrk(const char *, const char *);

#endif

