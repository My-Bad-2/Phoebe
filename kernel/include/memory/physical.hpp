#ifndef MEMORY_PHYSICAL_HPP
#define MEMORY_PHYSICAL_HPP 1

#include <stdint.h>
#include <stddef.h>

namespace memory
{
struct PhysicalMemoryStats
{
	uintptr_t highest_physical_addr;
	uintptr_t lowest_usable_addr;
	uintptr_t highest_usable_addr;

	size_t total_pages;
	size_t usable_pages;
	size_t used_pages;
	size_t free_pages;
};

void physical_initialize();
void physical_get_status(PhysicalMemoryStats* __status);

void* physical_allocate(size_t __count = 1);
void physical_free(void* __ptr, size_t __count = 1);

template<typename T = void*>
inline T physical_allocate(size_t __count = 1)
{
	return reinterpret_cast<T>(physical_allocate(__count));
}
} // namespace memory

#endif // MEMORY_PHYSICAL_HPP