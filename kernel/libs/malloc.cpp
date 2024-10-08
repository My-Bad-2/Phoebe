#include <memory/heap.hpp>
#include <stdlib.h>

void* malloc(size_t size)
{
	return memory::heap_malloc(size);
}

void* calloc(size_t nmemb, size_t size)
{
	return memory::heap_calloc(nmemb, size);
}

void* realloc(void* ptr, size_t new_size)
{
	return memory::heap_realloc(ptr, new_size);
}

void free(void* ptr)
{
	memory::heap_free(ptr);
}