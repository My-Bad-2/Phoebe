#include <string.h>
#include <lock.hpp>
#include <logger.h>

#include <libs/bitmap.hpp>

#include <memory/physical.hpp>
#include <memory/virtual.hpp>
#include <memory/heap.hpp>
#include <memory/memory.hpp>

#include <bit>

#define BUDDY_CPP_MANGLED
#include "buddy_alloc.h"

namespace memory
{
void* heap_arena = nullptr;
buddy* buddy = nullptr;
lock::mutex heap_lock = {};

void heap_init()
{
	PhysicalMemoryStats stats = {};

	log_begin_intialization("Kernel Heap");

	physical_get_status(&stats);

	// one-sixteen of total memory should be enough for Heap
	// Round off it to the nearest power of 2.
	size_t arena_size = std::bit_ceil(stats.total_pages / 16);
	heap_arena = virtual_allocate(arena_size);

	buddy = buddy_embed(reinterpret_cast<uint8_t*>(heap_arena), arena_size * PAGE_SIZE);

	assert(buddy != nullptr);

	log_end_intialization();
}

void* heap_malloc(size_t size)
{
	if(size == 0)
	{
		return nullptr;
	}

	lock::ScopedLock guard(heap_lock);
	return buddy_malloc(buddy, size);
}

void* heap_calloc(size_t nmemb, size_t size)
{
	if((size == 0) || (nmemb == 0))
	{
		return nullptr;
	}

	lock::ScopedLock guard(heap_lock);
	return buddy_calloc(buddy, nmemb, size);
}

void* heap_realloc(void* ptr, size_t size)
{
	if((ptr == nullptr) || (size == 0))
	{
		return nullptr;
	}

	lock::ScopedLock guard(heap_lock);
	return buddy_realloc(buddy, ptr, size, false);
}

void heap_free(void* ptr)
{
	if(ptr == nullptr)
	{
		return;
	}

	lock::ScopedLock guard(heap_lock);
	buddy_free(buddy, ptr);
}
} // namespace memory