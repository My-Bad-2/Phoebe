#ifndef MEMORY_PHYSICAL_HPP
#define MEMORY_PHYSICAL_HPP 1

#include <stdint.h>
#include <stddef.h>

#define PHYS_LOWER_ALLOCATION 0x01

namespace memory
{
struct PhysicalMemoryStats
{
	uintptr_t highest_physical_addr;
	// 0-0x1000 is guaranteed to be non-usable in limine
	uintptr_t lowest_usable_addr = 0x1000;
	uintptr_t highest_usable_addr;

	size_t highest_page;
	size_t usable_pages;
	size_t used_pages;
	size_t reserved_pages;
};

void physical_initialize();
void physical_get_status(PhysicalMemoryStats* __status);

void* physical_allocate();
void* physical_allocate(size_t __count, int __flags);

void physical_free(void* __ptr);
void physical_free(void* __ptr, size_t __count);
} // namespace memory

#endif // MEMORY_PHYSICAL_HPP