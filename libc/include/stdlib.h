#ifndef _STDLIB_H
#define _STDLIB_H 1

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>
#include <sys/defs.h>

__CDECLS_BEGIN

typedef struct
{
	int quot; // Quotient
	int rem; // Remainder
} div_t;

typedef struct
{
	long int quot; // Quotient
	long int rem; // Remainder
} ldiv_t;

typedef struct
{
	long long int quot; // Quotient
	long long int rem; // Remainder
} lldiv_t;

long int strtol(const char* __restrict __nptr, char** __restrict __endptr, int __base)
	__NONNULL((1));
unsigned long int strtoul(const char* __restrict __nptr, char** __restrict __endptr, int __base)
	__NONNULL((1));

long long int strtoll(const char* __restrict __nptr, char** __restrict __endptr, int __base)
	__NONNULL((1));
unsigned long long int strtoull(const char* __restrict __nptr, char** __restrict __endptr,
								int __base) __NONNULL((1));

int atoi(const char* __nptr) __PURE __NONNULL((1));
long atol(const char* __nptr) __PURE __NONNULL((1));
long atoll(const char* __nptr) __PURE __NONNULL((1));

int abs(int __x) __CONST;
long int labs(long int __x) __CONST;
long long int llabs(long long int __x) __CONST;

div_t div(int __numer, int __denom) __CONST;
ldiv_t ldiv(long int __numer, long int __denom) __CONST;
lldiv_t lldiv(long long int __numer, long long int __denom) __CONST;

__CDECLS_END

#endif // _STDLIB_H