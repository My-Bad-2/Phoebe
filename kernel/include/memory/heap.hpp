#ifndef MEMORY_HEAP_HPP
#define MEMORY_HEAP_HPP 1

#include <stdint.h>
#include <stddef.h>

namespace memory
{
void heap_init();

void* heap_malloc(size_t __size);
void heap_free(void* __ptr);
} // namespace memory

#endif // MEMORY_HEAP_HPP