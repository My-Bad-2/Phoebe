#ifndef MEMORY_HEAP_HPP
#define MEMORY_HEAP_HPP 1

#include <stdint.h>
#include <stddef.h>

namespace memory
{
void heap_initialize();

void* heap_malloc(size_t __size);
void* heap_calloc(size_t __nmemb, size_t __size);
void* heap_realloc(void* __ptr, size_t __new_size);
void heap_free(void* __ptr);
} // namespace memory

#endif // MEMORY_HEAP_HPP