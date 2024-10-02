#include "memory/memory.hpp"
#include <errno.h>
#include <logger.h>
#include <cpu/paging.hpp>
#include <memory/physical.hpp>
#include <mmu.hpp>
#include <array>
#include <kernel.h>
#include <memory/virtual.hpp>
#include <utility>

#define GET_PML_ENTRY(virtual_address, offset) ((virtual_address & (0x1ffUL << offset)) >> offset)

namespace cpu
{
namespace virt
{
static bool pml3_translation = false;

constexpr size_t parse_cache(size_t flags, size_t page_size)
{
	std::size_t patbit = (page_size > PAGE_SIZE) ? (1 << 12) : (1 << 7);
	std::size_t ret = 0;

	if(flags & MAP_MMIO)
	{
		ret |= PAGE_FLAG_NO_CACHE;
	}
	else if(flags & MAP_WRITE_COMBINING)
	{
		ret |= PAGE_FLAG_NO_CACHE | PAGE_FLAG_WRITE_THROUGH;
	}
	else if(flags & MAP_WRITE_THROUGH)
	{
		ret |= patbit;
	}
	else if(flags & MAP_PROTECTED)
	{
		ret |= patbit | PAGE_FLAG_WRITE_THROUGH;
	}
	else if(flags & MAP_WRITE_BACK)
	{
		ret |= patbit | PAGE_FLAG_NO_CACHE;
	}
	else if(flags & MAP_NO_CACHE)
	{
		ret |= patbit | PAGE_FLAG_NO_CACHE | PAGE_FLAG_WRITE_THROUGH;
	}

	return ret;
}

size_t parse_flags(size_t flags)
{
	size_t ret = 0;

	if((flags & MAP_PAGE_1GB) || (flags & MAP_PAGE_2MB))
	{
		ret |= PAGE_FLAG_SIZE_EXTENSION;
	}

	if(flags & MAP_READ)
	{
		ret |= PAGE_FLAG_PRESENT;
	}

	if(flags & MAP_WRITE)
	{
		ret |= PAGE_FLAG_WRITABLE;
	}

	if(flags & MAP_USER)
	{
		ret |= PAGE_FLAG_USER_ACCESSIBLE;
	}

	if(!(flags & MAP_EXEC))
	{
		ret |= PAGE_FLAG_NO_EXECUTE;
	}

	if(flags & MAP_GLOBAL)
	{
		ret |= PAGE_FLAG_GLOBAL;
	}

	return ret;
}
} // namespace virt

struct PageTable
{
	std::array<PageTableEntry, 512> entries;
} __ALIGNED(0x1000);

bool PageTableEntry::is_large()
{
	return this->get_flags(PAGE_FLAG_SIZE_EXTENSION);
}

bool PageTableEntry::is_valid()
{
	return this->get_flags(PAGE_FLAG_PRESENT);
}

size_t get_page_size_flags(size_t page_size)
{
	if(page_size == PAGE_SIZE)
	{
		return 0;
	}

	return PAGE_FLAG_SIZE_EXTENSION;
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
		return std::make_pair(PAGE_SIZE_1GiB, PAGE_FLAG_SIZE_EXTENSION);
	}
	else if(size >= PAGE_SIZE_2MiB)
	{
		return std::make_pair(PAGE_SIZE_2MiB, PAGE_FLAG_SIZE_EXTENSION);
	}

	return std::make_pair(PAGE_SIZE, 0);
}

constexpr size_t flag_to_page_size(size_t flags)
{
	if(flags & MAP_PAGE_1GB)
	{
		return PAGE_SIZE_1GiB;
	}
	else if(flags & MAP_PAGE_2MB)
	{
		return PAGE_SIZE_2MiB;
	}

	return PAGE_SIZE;
}

void* PageMap::get_next_lvl(PageTableEntry& entry, bool allocate, uintptr_t virtual_address,
							size_t old_page_size, size_t page_size)
{
	void* ret = nullptr;

	if(entry.is_valid())
	{
		if(entry.is_large() && old_page_size != -1)
		{
			size_t old_flags = entry.get_flags();
			uintptr_t old_phys_address = entry.get_address();
			uintptr_t old_virt_address = virtual_address & ~(old_page_size - 1);

			if(old_phys_address & (old_page_size - 1))
			{
				log_panik("Unexpected Page Table Entry Address: %p",
						  reinterpret_cast<void*>(old_phys_address));
			}

			ret = memory::to_higher_half(memory::physical_allocate());
			entry.set_address(memory::from_higher_half(reinterpret_cast<uintptr_t>(ret)));
			entry.set_flags(NEW_PAGE_FLAGS);

			for(size_t i = 0; i < old_page_size; i += page_size)
			{
				this->map_page(old_virt_address + i, old_phys_address + i,
							   old_flags | get_page_size_flags(page_size));
			}
		}
		else
		{
			ret = memory::to_higher_half(reinterpret_cast<void*>(entry.get_address()));
		}
	}
	else if(allocate)
	{
		ret = memory::to_higher_half(memory::physical_allocate());
		entry.set_address(reinterpret_cast<uintptr_t>(ret));
		entry.set_flags(NEW_PAGE_FLAGS);
	}

	return ret;
}

PageTableEntry* PageMap::virtual_to_entry(uintptr_t virtual_address, bool allocate,
										  size_t page_size, bool check_large)
{
	const std::size_t pml5_entry = GET_PML_ENTRY(virtual_address, 48);
	const std::size_t pml4_entry = GET_PML_ENTRY(virtual_address, 39);
	const std::size_t pdp_entry = GET_PML_ENTRY(virtual_address, 30);
	const std::size_t pd_entry = GET_PML_ENTRY(virtual_address, 21);
	const std::size_t pt_entry = GET_PML_ENTRY(virtual_address, 12);

	if(this->top_lvl == nullptr)
	{
		return nullptr;
	}

	PageTable* pml4 = this->top_lvl;

	if(is_paging_mode_max())
	{
		pml4 = static_cast<PageTable*>(
			this->get_next_lvl(this->top_lvl->entries[pml5_entry], allocate));
	}

	if(pml4 == nullptr)
	{
		return nullptr;
	}

	PageTable* pdp =
		static_cast<PageTable*>(this->get_next_lvl(pml4->entries[pml4_entry], allocate));

	if(pdp == nullptr)
	{
		return nullptr;
	}

	if((page_size == PAGE_SIZE_1GiB) || (check_large && pdp->entries[pdp_entry].is_large()))
	{
		return &pdp->entries[pdp_entry];
	}

	PageTable* pd = static_cast<PageTable*>(this->get_next_lvl(
		pdp->entries[pdp_entry], allocate, virtual_address, PAGE_SIZE_1GiB, page_size));

	if(pd == nullptr)
	{
		return nullptr;
	}

	if((page_size == PAGE_SIZE_2MiB) || (check_large && pdp->entries[pd_entry].is_large()))
	{
		return &pd->entries[pd_entry];
	}

	PageTable* pt = static_cast<PageTable*>(this->get_next_lvl(
		pd->entries[pd_entry], allocate, virtual_address, PAGE_SIZE_2MiB, page_size));

	if(pt == nullptr)
	{
		return nullptr;
	}

	return &pt->entries[pt_entry];
}

size_t PageMap::parse_flags(size_t flags)
{
	const size_t page_size = flag_to_page_size(flags);
	return virt::parse_cache(flags, page_size) | virt::parse_flags(flags);
}

error_t PageMap::map_page(uintptr_t virtual_address, uintptr_t physical_address, size_t flags)
{
	auto map_one = [this](uintptr_t virtual_address, uintptr_t physical_address, size_t flags,
						  size_t page_size) {
		PageTableEntry* pml_entry = this->virtual_to_entry(virtual_address, true, page_size, false);

		if(pml_entry == nullptr)
		{
			log_error("Could not get pagemap entry for address 0x%.16lx", virtual_address);
			return SYSTEM_ERR_ADDRESS_UNREACHABLE;
		}

		pml_entry->reset();
		pml_entry->set_address(physical_address);
		pml_entry->set_flags(flags);

		return SYSTEM_OK;
	};

	const size_t page_size = flag_to_page_size(flags);
	const size_t parsed_flags = this->parse_flags(flags);

	if((page_size == PAGE_SIZE_1GiB) && !virt::pml3_translation)
	{
		for(size_t i = 0; i < PAGE_SIZE_1GiB; i += PAGE_SIZE_2MiB)
		{
			if(map_one(virtual_address + i, physical_address + i, parsed_flags, PAGE_SIZE_2MiB) !=
			   SYSTEM_OK)
			{
				return SYSTEM_ERR_ADDRESS_UNREACHABLE;
			}
		}

		return SYSTEM_OK;
	}

	return map_one(virtual_address, physical_address, parsed_flags, page_size);
}

error_t PageMap::unmap_page(uintptr_t virtual_address, size_t flags) {
	auto unmap_one = [this](uintptr_t virtual_address, size_t page_size) {};
}
} // namespace cpu