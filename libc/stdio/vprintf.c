#include <stdio.h>

extern int vsnprintf_internal(FILE*, const char*, va_list);

int vsnprintf(char* buffer, size_t maxlen, const char* restrict format, va_list args)
{
	FILE stream = {
		.flags = 0,
		.buffer_size = maxlen,
		.position = 0,
		.buffer = buffer,
		.write_func = NULL,
	};

	return vsnprintf_internal(&stream, format, args);
}

int vsprintf(char* buffer, const char* restrict format, va_list args)
{
	return vsnprintf(buffer, BUFSIZE, format, args);
}

int vfprintf(FILE* stream, const char* restrict format, va_list args)
{
	return vsnprintf_internal(stream, format, args);
}

int vprintf(const char* restrict format, va_list args)
{
	return vfprintf(stdout, format, args);
}