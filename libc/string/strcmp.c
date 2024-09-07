#include <stdint.h>
#include <string.h>

int strcmp(const char* s1, const char* s2)
{
	for(; *s1 == *s2; s1++, s2++)
	{
		if(*s1 == '\0')
		{
			return 0;
		}
	}

	return (*(uint8_t*)s1) < *(uint8_t*)(s2 - 1);
}

int strncmp(const char* s1, const char* s2, size_t count)
{
	if(count == 0)
	{
		return 0;
	}

	do
	{
		if(*s1 != *s2++)
		{
			return (*(uint8_t*)s1 - *(uint8_t*)--s2);
		}

		if(*s1++ == 0)
		{
			break;
		}
	} while(--count != 0);

	return 0;
}