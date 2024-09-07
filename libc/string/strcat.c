#include <string.h>

char* strcat(char* __restrict dest, const char* __restrict src)
{
	const size_t dest_len = strlen(dest);
	const size_t src_len = strlen(src);

	memcpy(dest + dest_len, src, src_len);
	dest[dest_len + src_len] = '\0';

	return dest;
}

char* strncat(char* __restrict dest, const char* __restrict src, size_t count)
{
	const size_t dest_len = strlen(dest);
	const size_t src_len = strnlen(src, count);

	const size_t copy_len = src_len < count ? src_len : count;

	memcpy(dest + dest_len, src, copy_len);
	dest[dest_len + copy_len] = '\0';

	return dest;
}