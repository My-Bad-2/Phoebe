#include <stdio.h>
#include <string.h>

int fputs(const char* restrict s, FILE* restrict stream)
{
	size_t length = strlen(s);

	for(size_t i = 0; i < length; i++)
	{
		if(!(putc(s[i], stream)))
		{
			return -1;
		}
	}

	return length;
}

int puts(const char* s)
{
	int ret = fputs(s, stdout);
	putc('\n', stdout);

	return ret;
}