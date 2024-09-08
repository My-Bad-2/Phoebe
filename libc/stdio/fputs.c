#include <stdio.h>
#include <string.h>
#include <errno.h>

int fputs(const char* __restrict s, FILE* __restrict stream)
{
	size_t length = strlen(s);

	for(size_t i = 0; i < length; i++)
	{
		if(putc(s[i], stream) != SYSTEM_OK)
		{
			return SYSTEM_ERR_INTERNAL;
		}
	}

	return SYSTEM_OK;
}

int puts(const char* s)
{
	int ret = fputs(s, stdout);
	putc('\n', stdout);

	return ret;
}