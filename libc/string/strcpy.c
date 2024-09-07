#include <string.h>

char* strcpy(char* __restrict dest, const char* __restrict src)
{
	const size_t length = strlen(src);

	memcpy(dest, src, length + 1);

	return dest;
}

char* strncpy(char* __restrict dest, const char* __restrict src, size_t count)
{
	const size_t src_len = strnlen(src, count);

	if(src_len < count)
	{
		memcpy(dest, src, src_len);
		memset(dest + src_len, 0, count - src_len);
	}
	else
	{
		memcpy(dest, src, count);
	}

	return dest;
}