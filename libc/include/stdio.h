#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/defs.h>
#include <sys/file.h>
#include <stdarg.h>
#include <stddef.h>

#define BUFSIZE 8192
#define EOI (-1)

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int fprintf(FILE* __restrict __stream, const char* __restrict __format, ...) __NONNULL((1));
int printf(const char* __restrict __format, ...);
int sprintf(char* __restrict __buffer, const char* __restrict __format, ...);
int snprintf(char* __restrict __buffer, size_t __maxlen, const char* __restrict __format, ...)
	__PRINTFLIKE(3, 4);

int vfprintf(FILE* __restrict __stream, const char* __restrict __format, va_list __arg)
	__NONNULL((1));
int vprintf(const char* __restrict __format, va_list __arg);
int vsprintf(char* __restrict __buffer, const char* __restrict __format, va_list __arg);
int vsnprintf(char* __restrict __buffer, size_t __maxlen, const char* __restrict __format,
			  va_list __arg) __PRINTFLIKE(3, 0);

int putchar(int __c);
int putc(int __c, FILE* __stream) __NONNULL((2));

int fputs(const char* __restrict __s, FILE* __restrict __stream) __NONNULL((2));
int puts(const char* __s);

#endif // _STDIO_H