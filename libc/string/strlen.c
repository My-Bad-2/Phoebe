#include <string.h>

size_t strlen(const char* s)
{
	const char* end = s;

	while(*end != '\0')
	{
		++end;
	}

	return end - s;
}

size_t strnlen(const char* string, size_t maxlen)
{
	const char* end = string;
	size_t length = 0;

	while((*end != '\0') && (length <= maxlen))
	{
		end++;
		length++;
	}

	return length;
}