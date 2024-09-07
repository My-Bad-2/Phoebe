#include <stdint.h>
#include <string.h>
#include "memcopy.h"

void* memcpy(void* __restrict dest, const void* __restrict src, size_t length)
{
	uintptr_t destp = (uintptr_t)dest;
	uintptr_t srcp = (uintptr_t)src;

	// Use word copy if there are too many bytes to copy.
	if(length >= OP_T_THRES)
	{
		// Copy just a few bytes to make `destp` aligned
		length -= (-destp) % OPSIZ;
		BYTE_COPY_FWD(destp, srcp, (-destp) % OPSIZ);

		// Copy from `srcp` to `destp` taking advantage of the known alignment of `destp`.
		WORD_COPY_FWD(destp, srcp, length, length);
	}

	// Byte memory operations to copy a few bytes.
	BYTE_COPY_FWD(destp, srcp, length);

	return dest;
}