#include <stdio.h>
#include <limits.h>

extern int vsnprintf_internal(FILE*, const char*, va_list);

int vsnprintf(char* buffer, size_t maxlen, const char* __restrict format, va_list arg)
{
	FILE stream = {
		.flags = 0,
		.buffer_size = maxlen,
		.position = 0,
		.buffer = buffer,
		.write_func = NULL,
	};

	return vsnprintf_internal(&stream, format, arg);
}

int vsprintf(char* buffer, const char* __restrict format, va_list arg)
{
	return vsnprintf(buffer, ULONG_MAX, format, arg);
}

int vfprintf(FILE* stream, const char* __restrict format, va_list arg)
{
	return vsnprintf_internal(stream, format, arg);
}

int vprintf(const char* __restrict format, va_list arg)
{
	return vfprintf(stdout, format, arg);
}