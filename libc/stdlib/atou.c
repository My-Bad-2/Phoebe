#include <ctype.h>
#include <stdlib.h>

unsigned long atou(const char* nptr)
{
	const char* end = nptr;
	size_t val = 0;

	while(isdigit(*end))
	{
		val = (val * 10) + (size_t)(*(end++) - '0');
	}

	return val;
}