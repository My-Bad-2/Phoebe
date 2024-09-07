#include <stdint.h>
#include <string.h>
#include "memcopy.h"

void* memmove(void* dest, const void* src, size_t length)
{
	uintptr_t destp = (uintptr_t)dest;
	uintptr_t srcp = (uintptr_t)src;

	if(destp - srcp >= length)
	{
		dest = memcpy(dest, src, length);
	}
	else
	{
		srcp += length;
		destp += length;

		if(length >= OP_T_THRES)
		{
			length -= destp % OPSIZ;
			BYTE_COPY_BWD(destp, srcp, destp % OPSIZ);
			WORD_COPY_BWD(destp, srcp, length, length);
		}

		BYTE_COPY_BWD(destp, srcp, length);
	}

	return dest;
}