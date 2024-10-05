#include <errno.h>
#include <string.h>
#include <logger.h>

#include <algorithm>

#include <memory/physical.hpp>
#include <memory/memory.hpp>

#include <libs/bitmap.hpp>

namespace memory
{
namespace physical
{
static PhysicalMemoryStats stats = {};
static Bitmap bitmap = {};

error_t alloc_page(void* phys)
{
	const size_t page = reinterpret_cast<uintptr_t>(phys) / PAGE_SIZE;

	if(bitmap[page] == BITMAP_USED)
	{
		return SYSTEM_ERR_ADDRESS_IN_USE;
	}
	else
	{
		bitmap[page] = BITMAP_USED;
		stats.used_pages++;
	}

	return SYSTEM_OK;
}

error_t free_page(void* phys)
{
	const size_t page = reinterpret_cast<uintptr_t>(phys) / PAGE_SIZE;

	if(bitmap[page] == BITMAP_FREE)
	{
		return SYSTEM_ERR_ADDRESS_IN_USE;
	}
	else
	{
		bitmap[page] = BITMAP_FREE;
		stats.used_pages--;
	}

	return SYSTEM_OK;
}

error_t alloc_pages(void* phys, size_t count)
{
	int status = SYSTEM_OK;
	uintptr_t addr = reinterpret_cast<uintptr_t>(phys);

	for(size_t i = 0; i < count; i++)
	{
		status = alloc_page(reinterpret_cast<void*>(addr));

		if(status != SYSTEM_OK)
		{
			return status;
		}

		addr += PAGE_SIZE;
	}

	return status;
}

error_t free_pages(void* phys, size_t count)
{
	int status = SYSTEM_OK;
	uintptr_t addr = reinterpret_cast<uintptr_t>(phys);

	for(size_t i = 0; i < count; i++)
	{
		status = free_page(reinterpret_cast<void*>(addr));

		if(status != SYSTEM_OK)
		{
			return status;
		}

		addr += PAGE_SIZE;
	}

	return status;
}

void reserve_page(void*)
{
	// Somehow made the execution a lot slower and wrote to framebuffer
	// so, don't track reserved memory allocation.
	// But What about reclaiming memory? You don't. For now atleast.

	// const size_t page = reinterpret_cast<uintptr_t>(phys) / PAGE_SIZE;
	//
	// bitmap[page] = BITMAP_USED;
	stats.reserved_pages++;
}

void unlock_page(void* phys)
{
	const size_t page = reinterpret_cast<uintptr_t>(phys) / PAGE_SIZE;

	bitmap[page] = BITMAP_FREE;
	stats.usable_pages++;
}

void reserve_pages(void* phys, size_t count)
{
	uintptr_t addr = reinterpret_cast<uintptr_t>(phys);

	for(size_t i = 0; i < count; i++)
	{
		reserve_page(reinterpret_cast<void*>(addr));
		addr += PAGE_SIZE;
	}
}

void unlock_pages(void* phys, size_t count)
{
	uintptr_t addr = reinterpret_cast<uintptr_t>(phys);

	for(size_t i = 0; i < count; i++)
	{
		unlock_page(reinterpret_cast<void*>(addr));
		addr += PAGE_SIZE;
	}
}

bool page_is_used(void* phys)
{
	const uintptr_t page = reinterpret_cast<uintptr_t>(phys) / PAGE_SIZE;
	return (bitmap[page] & BITMAP_USED);
}
} // namespace physical

void physical_initialize()
{
	using namespace physical;

	const size_t memmap_count = memmap_request.response->entry_count;
	limine_memmap_entry** memmaps = memmap_request.response->entries;

	log_begin_intialization("Physical Memory Manager");

	for(size_t i = 0; i < memmap_count; i++)
	{
		uintptr_t limit = memmaps[i]->base + memmaps[i]->length;
		stats.highest_physical_addr = std::max(stats.highest_physical_addr, limit);

		switch(memmaps[i]->type)
		{
			case LIMINE_MEMMAP_USABLE:
				stats.highest_usable_addr = std::max(stats.highest_usable_addr, limit);
				stats.lowest_usable_addr = std::min(stats.lowest_usable_addr, limit);
				break;
		}
	}

	// Memory Range [0, 0x1000) is guaranteed to be unusable by the bootloader.
	if(stats.lowest_usable_addr == 0)
	{
		stats.lowest_usable_addr = 0x1000;
	}

	stats.highest_page = div_roundup(stats.highest_usable_addr, PAGE_SIZE);

	size_t bitmap_entries = stats.highest_page;
	size_t bitmap_size = align_up(bitmap_entries / 8, PAGE_SIZE);
	bitmap_entries = bitmap_size * 8;

	for(size_t i = 0; i < memmap_count; i++)
	{
		if(memmaps[i]->type != LIMINE_MEMMAP_USABLE)
		{
			continue;
		}

		if(memmaps[i]->length >= bitmap_size)
		{
			bitmap.initialize(reinterpret_cast<uint8_t*>(to_higher_half(memmaps[i]->base)),
							  bitmap_entries);
			memset(bitmap.buffer(), 0xff, bitmap_entries);

			memmaps[i]->length -= bitmap_size;
			memmaps[i]->base += bitmap_size;

			stats.used_pages += div_roundup(bitmap_size, PAGE_SIZE);
			break;
		}
	}

	for(size_t i = 0; i < memmap_count; i++)
	{
		if(memmaps[i]->type != LIMINE_MEMMAP_USABLE)
		{
			reserve_pages(reinterpret_cast<void*>(memmaps[i]->base),
						  memmaps[i]->length / PAGE_SIZE);
		}
		else
		{
			unlock_pages(reinterpret_cast<void*>(memmaps[i]->base), memmaps[i]->length / PAGE_SIZE);
		}
	}

	log_end_intialization();

	log_debug("Bitmap size = %lu (%lu KiB)", bitmap_size, bitmap_size / 1024);
	log_debug("Lowest usable address = 0x%.16lx", stats.lowest_usable_addr);
	log_debug("Highest usable address = 0x%.16lx", stats.highest_usable_addr);
	log_debug("Highest physical address = 0x%.16lx", stats.highest_physical_addr);

	log_debug("Total usable memory = %lu pages (%lu MiB)", stats.usable_pages,
			  (stats.usable_pages * PAGE_SIZE) / (1024 * 1024));
	log_debug("Kernel-reserved memory = %lu pages (%lu MiB)", stats.used_pages,
			  (stats.used_pages * PAGE_SIZE) / (1024 * 1024));
	log_debug("Hardware-reserved memory = %lu pages (%lu MiB)", stats.reserved_pages,
			  (stats.reserved_pages * PAGE_SIZE) / (1024 * 1024));
}

void* physical_allocate()
{
	using namespace physical;
	uintptr_t addr = 0;

	for(addr = stats.lowest_usable_addr; addr < stats.highest_usable_addr; addr += PAGE_SIZE)
	{
		if(!page_is_used(reinterpret_cast<void*>(addr)))
		{
			alloc_page(reinterpret_cast<void*>(addr));
			memset(to_higher_half(reinterpret_cast<void*>(addr)), 0, PAGE_SIZE);

			return reinterpret_cast<void*>(addr);
		}
	}

	return nullptr;
}

void physical_free(void* ptr)
{
	using namespace physical;
	const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

	if(addr <= stats.highest_usable_addr)
	{
		return;
	}

	free_page(ptr);
}

void* physical_allocate(size_t count, int flags = 0)
{
	using namespace physical;

	// Allocation constraints
	uintptr_t start = stats.lowest_usable_addr;
	uintptr_t end = 0;
	uintptr_t addr = 0;

	// Flag only applicable when physical memory is larger than 32-bit address space
	if((flags & PHYS_LOWER_ALLOCATION) && (stats.highest_usable_addr > UINT32_MAX))
	{
		// last page of the 32-bit address space
		end = align_down(UINT32_MAX, PAGE_SIZE);
	}
	else
	{
		end = stats.highest_usable_addr - (count * PAGE_SIZE);
	}

	do
	{
		// Iterate until a free physical page is found.
		for(addr = start; addr < (start + (count * PAGE_SIZE)); addr += PAGE_SIZE)
		{
			if(page_is_used(reinterpret_cast<void*>(addr)))
			{
				break;
			}
		}

		if(addr >= (start + (count * PAGE_SIZE)))
		{
			if(alloc_pages(reinterpret_cast<void*>(start), count) == SYSTEM_OK)
			{
				memset(to_higher_half(reinterpret_cast<void*>(start)), 0, count * PAGE_SIZE);
				return reinterpret_cast<void*>(start);
			}

			return nullptr;
		}
		else
		{
			start += PAGE_SIZE;
		}
	} while(start < end);

	return nullptr;
}

void physical_free(void* ptr, size_t count)
{
	using namespace physical;
	const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

	if(addr <= stats.highest_usable_addr)
	{
		return;
	}

	free_pages(ptr, count);
}

void physical_get_status(PhysicalMemoryStats* dest)
{
	memcpy(dest, &physical::stats, sizeof(PhysicalMemoryStats));
}
} // namespace memory