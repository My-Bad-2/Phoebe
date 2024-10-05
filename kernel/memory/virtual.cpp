#include <logger.h>
#include <assert.h>

#include <memory/physical.hpp>
#include <memory/memory.hpp>
#include <memory/paging.hpp>
#include <memory/virtual.hpp>

namespace memory
{
PageMap base_pagemap = PageMap();

void virtual_initialize()
{
	const size_t memmap_count = memmap_request.response->entry_count;
	limine_memmap_entry** memmaps = memmap_request.response->entries;

	log_begin_intialization("Virtual Memory Manager");

	base_pagemap.initialize(true);

	auto [page_size, flags] = required_page_size(PAGE_SIZE_1GiB * 4);

	for(uintptr_t i = 0; i < (PAGE_SIZE_1GiB * 4); i += page_size)
	{
		assert(!base_pagemap.map_page(to_higher_half(i), i,
									  MAP_READ | MAP_WRITE | flags | MAP_WRITE_BACK));
	}

	// Map if any memory map entry is above 4 GiB.
	for(size_t i = 0; i < memmap_count; i++)
	{
		limine_memmap_entry* entry = memmaps[i];

		uintptr_t base = align_down(entry->base, PAGE_SIZE);
		uintptr_t top = align_up(entry->base + entry->length, PAGE_SIZE);

		if(top < (PAGE_SIZE_1GiB * 4))
		{
			continue;
		}

		size_t cache = MAP_WRITE_BACK;

		if(entry->type == LIMINE_MEMMAP_FRAMEBUFFER)
		{
			cache = MAP_WRITE_COMBINING;
		}

		size_t size = top - base;
		auto [page_size, flags] = required_page_size(size);

		size_t aligned_size = align_down(size, page_size);
		size_t difference = size - aligned_size;

		for(uintptr_t j = base; j < (base + aligned_size); j += page_size)
		{
			if(j < (PAGE_SIZE_1GiB * 4))
			{
				continue;
			}

			assert(
				!base_pagemap.map_page(to_higher_half(j), j, MAP_READ | MAP_WRITE | flags | cache));
		}

		base += aligned_size;

		for(uintptr_t j = base; j < (base + difference); j += PAGE_SIZE)
		{
			if(j < (PAGE_SIZE_1GiB * 4))
			{
				continue;
			}

			assert(!base_pagemap.map_page(to_higher_half(j), j, MAP_READ | MAP_WRITE | cache));
		}
	}

	const limine_file* kernel_file = kernel_file_request.response->kernel_file;
	const uintptr_t virtual_base = kernel_address_request.response->virtual_base;
	const uintptr_t physical_base = kernel_address_request.response->physical_base;

	// Map the kernel
	for(size_t i = 0; i < kernel_file->size; i += PAGE_SIZE)
	{
		assert(!base_pagemap.map_page(virtual_base + i, physical_base + i,
									  MAP_WRITE | MAP_READ | MAP_EXEC | MAP_WRITE_BACK));
	}

	base_pagemap.load();

	log_end_intialization();

	log_debug("Kernel Virtual Base = 0x%.16lx", virtual_base);
	log_debug("Kernel Physical Base = 0x%.16lx", physical_base);
	log_debug("Kernel Size = %lu", kernel_file->size);
}

PageMap* get_current_pagemap()
{
	return &base_pagemap;
}

size_t get_page_size_flags(size_t page_size)
{
	if(page_size == PAGE_SIZE)
	{
		return 0;
	}
	else if(page_size == PAGE_SIZE_2MiB)
	{
		return MAP_PAGE_2MB;
	}

	return MAP_PAGE_1GB;
}

size_t get_page_size(size_t size)
{
	if(size >= PAGE_SIZE_1GiB)
	{
		return PAGE_SIZE_1GiB;
	}
	else if(size >= PAGE_SIZE_2MiB)
	{
		return PAGE_SIZE_2MiB;
	}

	return PAGE_SIZE;
}

std::pair<size_t, size_t> required_page_size(size_t size)
{
	if(size >= PAGE_SIZE_1GiB)
	{
		return std::make_pair(PAGE_SIZE_1GiB, MAP_PAGE_1GB);
	}
	else if(size >= PAGE_SIZE_2MiB)
	{
		return std::make_pair(PAGE_SIZE_2MiB, MAP_PAGE_2MB);
	}

	return std::make_pair(PAGE_SIZE, 0);
}

void* virtual_allocate(uintptr_t base, uintptr_t limit, size_t count, size_t flags)
{
	base = align_down(base, PAGE_SIZE);
	limit = align_down(limit, PAGE_SIZE);

	const size_t size = count * PAGE_SIZE;
	uintptr_t start = base;
	uintptr_t end = limit - (count * PAGE_SIZE);
	uintptr_t address = 0;
	PageMap* pagemap = get_current_pagemap();

	auto [page_size, page_flag] = required_page_size(size);

	do
	{
		for(address = start; address < (start + size); address += page_size)
		{
			if(pagemap->virtual_to_entry(address, false, page_size, true)->is_valid())
			{
				// Found a valid page
				break;
			}
		}

		if(address >= (start + size))
		{
			for(size_t i = 0; i < size; i += page_size)
			{
				void* physical_address = physical_allocate();
				if(pagemap->map_page(start + i, reinterpret_cast<uintptr_t>(physical_address),
									 flags) != SYSTEM_OK)
				{
					return nullptr;
				}
			}

			return reinterpret_cast<void*>(start);
		}

		start += PAGE_SIZE;
	} while(start < end);

	return nullptr;
}

void* virtual_allocate(size_t count, size_t flags)
{
	PhysicalMemoryStats stats = {};
	physical_get_status(&stats);

	const uintptr_t virtual_base = to_higher_half(stats.highest_physical_addr);
	return virtual_allocate(virtual_base, 0xffff8fffffffffff, count, flags);
}

void virtual_free(void* ptr, size_t count)
{
	uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
	PageMap* pagemap = get_current_pagemap();

	address = align_down(address, PAGE_SIZE);

	for(size_t i = 0; i < count; i++)
	{
		uintptr_t physical_address = pagemap->virtual_to_physical(address);

		if(physical_address == static_cast<size_t>(-1))
		{
			return;
		}

		pagemap->unmap_page(address);
		physical_free(reinterpret_cast<void*>(physical_address));
	}
}
} // namespace memory