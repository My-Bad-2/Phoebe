#include <stdint.h>
#include <string.h>
#include "memcopy.h"

void* memset(void* dest, int c, size_t length)
{
	uintptr_t destp = (uintptr_t)dest;

	if(length >= 8)
	{
		size_t len = 0;
		op_t word = 0;

		word = (uint8_t)c;
		word |= word << 8;
		word |= word << 16;

		if(OPSIZ > 4)
		{
			// Do the shift in two dteps to avoid warning if long has 32 bits
			word |= (word << 16) << 16;
		}

		while(destp % OPSIZ != 0)
		{
			((uint8_t*)destp)[0] = c;
			destp++;
			len--;
		}

		// Write 8 `op_t` per iteration until less than 8 `op_t` remain.
		len = length / (OPSIZ * 8);

		while(len > 0)
		{
			((op_t*)destp)[0] = word;
			((op_t*)destp)[1] = word;
			((op_t*)destp)[2] = word;
			((op_t*)destp)[3] = word;
			((op_t*)destp)[4] = word;
			((op_t*)destp)[5] = word;
			((op_t*)destp)[6] = word;
			((op_t*)destp)[7] = word;

			destp += 8 * OPSIZ;
			len--;
		}

		length %= OPSIZ * 8;

		// Write 1 `op_t` per iteration
		len = length / OPSIZ;

		while(len > 0)
		{
			((op_t*)destp)[0] = word;
			destp += OPSIZ;
			len--;
		}

		length %= OPSIZ;
	}

	// Write the last few bytes
	while(length > 0)
	{
		((uint8_t*)destp)[0] = c;
		destp++;
		length--;
	}

	return dest;
}