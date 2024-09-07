#ifndef _STRING_H
#define _STRING_H 1

#include <sys/defs.h>

#define __need_size_t
#define __need_NULL
#include <stddef.h>

__CDECLS_BEGIN

void* memcpy(void* __restrict __dest, const void* __restrict __src, size_t __n) __NONNULL((1, 2));
void* memmove(void* __dest, const void* __src, size_t __n) __NONNULL((1, 2));
void* memset(void* __s, int __c, size_t __n) __NONNULL((1));
int memcmp(const void* __s1, const void* __s2, size_t __n) __PURE __NONNULL((1, 2));

char* strcpy(char* __restrict __dest, const char* __restrict __src) __NONNULL((1, 2));
char* strncpy(char* __restrict __dest, const char* __restrict __src, size_t __n) __NONNULL((1, 2));

char* strcat(char* __restrict __dest, const char* __restrict __src) __NONNULL((1, 2));
char* strncat(char* __restrict __dest, const char* __restrict __src, size_t __n) __NONNULL((1, 2));

int strcmp(const char* __s1, const char* __s2) __PURE __NONNULL((1, 2));
int strncmp(const char* __s1, const char* __s2, size_t __n) __PURE __NONNULL((1, 2));

size_t strnlen(const char* __string, size_t __maxlen) __PURE __NONNULL((1));
size_t strlen(const char* __string) __PURE __NONNULL((1));

__CDECLS_END

#endif // _STRING_H