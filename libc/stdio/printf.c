#include <stdio.h>

int snprintf(char* buffer, size_t maxlen, const char* restrict format, ...)
{
	va_list args = {};
	va_start(args, format);
	int ret = vsnprintf(buffer, maxlen, format, args);
	va_end(args);

	return ret;
}

int sprintf(char* buffer, const char* restrict format, ...)
{
	va_list args = {};
	va_start(args, format);
	int ret = vsprintf(buffer, format, args);
	va_end(args);

	return ret;
}

int fprintf(FILE* stream, const char* restrict format, ...)
{
	va_list args = {};
	va_start(args, format);
	int ret = vfprintf(stream, format, args);
	va_end(args);

	return ret;
}

int printf(const char* restrict format, ...)
{
	va_list args = {};
	va_start(args, format);
	int ret = vprintf(format, args);
	va_end(args);

	return ret;
}