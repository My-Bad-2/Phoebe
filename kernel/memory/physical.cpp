#include <string.h>
#include <logger.h>

#include <algorithm>

#include <memory/physical.hpp>
#include <memory/memory.hpp>

#include <libs/bitmap.hpp>
#include <lock.hpp>

namespace memory
{
PhysicalMemoryStats phys_stats = {};
Bitmap phys_bitmap = {};
lock::mutex phys_lock = {};
size_t bitmap_last_index = 0;

void physical_initialize()
{
	const size_t memmap_count = memmap_request.response->entry_count;
	limine_memmap_entry** memmaps = memmap_request.response->entries;

	log_begin_intialization("Physical Memory Manager");

	for(size_t i = 0; i < memmap_count; i++)
	{
		uintptr_t limit = memmaps[i]->base + memmaps[i]->length;
		phys_stats.highest_physical_addr = std::max(phys_stats.highest_physical_addr, limit);

		switch(memmaps[i]->type)
		{
			case LIMINE_MEMMAP_USABLE:
				phys_stats.highest_usable_addr = std::max(phys_stats.highest_usable_addr, limit);
				phys_stats.lowest_usable_addr = std::min(phys_stats.lowest_usable_addr, limit);

				// Guaranteed to be page-aligned
				phys_stats.usable_pages += (memmaps[i]->length / PAGE_SIZE);
				break;
			case LIMINE_MEMMAP_KERNEL_AND_MODULES:
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
				break;
			default:
				continue;
		}

		// Guaranteed to be page-aligned
		phys_stats.total_pages += (memmaps[i]->length / PAGE_SIZE);
	}

	// Memory Range [0, 0x1000] is guaranteed to be unusable by the bootloader.
	if(phys_stats.lowest_usable_addr == 0)
	{
		phys_stats.lowest_usable_addr = 0x1000;
	}

	size_t bitmap_entries = phys_stats.highest_usable_addr / PAGE_SIZE;
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
			phys_bitmap.initialize(reinterpret_cast<uint8_t*>(to_higher_half(memmaps[i]->base)),
								   bitmap_entries);
			memset(phys_bitmap.buffer(), 0xff, bitmap_entries);

			memmaps[i]->length -= bitmap_size;
			memmaps[i]->base += bitmap_size;

			phys_stats.used_pages += div_roundup(bitmap_size, PAGE_SIZE);
			break;
		}
	}

	for(size_t i = 0; i < memmap_count; i++)
	{
		if(memmaps[i]->type != LIMINE_MEMMAP_USABLE)
		{
			continue;
		}

		for(uintptr_t j = 0; j < memmaps[i]->length; j += PAGE_SIZE)
		{
			phys_bitmap[(memmaps[i]->base + j) / PAGE_SIZE] = BITMAP_FREE;
		}
	}

	phys_stats.free_pages = phys_stats.usable_pages - phys_stats.used_pages;

	log_end_intialization();

	log_debug("Bitmap size = %lu (%lu KiB)", bitmap_size, bitmap_size / 1024);
	log_debug("Lowest usable address = 0x%.16lx", phys_stats.lowest_usable_addr);
	log_debug("Highest usable address = 0x%.16lx", phys_stats.highest_usable_addr);
	log_debug("Highest physical address = 0x%.16lx", phys_stats.highest_physical_addr);

	log_debug("Total usable memory = %lu pages(%lu MiB)", phys_stats.usable_pages,
			  (phys_stats.usable_pages * PAGE_SIZE) / (1024 * 1024));
	log_debug("Used memory = %lu pages (%lu MiB)", phys_stats.used_pages,
			  (phys_stats.used_pages * PAGE_SIZE) / (1024 * 1024));
	log_debug("Free memory = %lu pages (%lu MiB)", phys_stats.free_pages,
			  (phys_stats.free_pages * PAGE_SIZE) / (1024 * 1024));
}

void physical_get_status(PhysicalMemoryStats* dest)
{
	memcpy(dest, &phys_stats, sizeof(PhysicalMemoryStats));
}

void* physical_allocate(size_t count)
{
	if(count == 0)
	{
		return nullptr;
	}

	lock::ScopedLock guard(phys_lock);

	auto allocate_internal = [count](size_t limit) -> void* {
		size_t i = 0;

		while(bitmap_last_index < limit)
		{
			if(phys_bitmap[bitmap_last_index++] == BITMAP_FREE)
			{
				if(++i == count)
				{
					size_t page = bitmap_last_index - count;

					for(size_t j = page; j < bitmap_last_index; j++)
					{
						phys_bitmap[j] = BITMAP_USED;
					}

					return reinterpret_cast<void*>(page * PAGE_SIZE);
				}
			}
			else
			{
				i = 0;
			}
		}

		return nullptr;
	};

	size_t index = bitmap_last_index;
	void* ret = allocate_internal(phys_stats.highest_usable_addr / PAGE_SIZE);

	if(ret == nullptr)
	{
		bitmap_last_index = 0;
		ret = allocate_internal(index);

		// Try swapping pages
		if(ret == nullptr)
		{
			log_panik("Out of Phyiscal Memory!");
		}
	}

	memset(to_higher_half(ret), 0, count * PAGE_SIZE);
	phys_stats.used_pages += count;
	phys_stats.free_pages -= count;

	return ret;
}

void physical_free(void* ptr, size_t count)
{
	if(ptr == nullptr)
	{
		return;
	}

	lock::ScopedLock guard(phys_lock);

	size_t page = reinterpret_cast<uintptr_t>(ptr) / PAGE_SIZE;

	for(size_t i = page; i < (page + count); i++)
	{
		phys_bitmap[i] = BITMAP_FREE;
	}

	phys_stats.used_pages -= count;
	phys_stats.free_pages += count;
}
} // namespace memory