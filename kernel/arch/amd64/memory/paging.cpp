#include <logger.h>
#include <string.h>

#include <mmu.hpp>
#include <memory/memory.hpp>
#include <memory/paging.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>

#include <cpu/cpu.hpp>
#include <cpu/features.h>

#define GET_PML_ENTRY(virtual_address, offset) (((virtual_address) >> offset) & 0x1fful)

namespace memory
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

bool PageTableEntry::is_large()
{
	return this->get_flags(PAGE_FLAG_SIZE_EXTENSION);
}

bool PageTableEntry::is_valid()
{
	return this->get_flags(PAGE_FLAG_PRESENT);
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

PageMap::~PageMap()
{
	this->destroy_level(this->top_lvl_, 0, 256, is_paging_mode_max() ? 5 : 4);
}

void PageMap::initialize(bool kernel_pagemap)
{
	this->top_lvl_ = static_cast<PageTable*>(to_higher_half(physical_allocate()));
	memset(this->top_lvl_, 0, sizeof(PageTable));

	if(kernel_pagemap)
	{
		virt::pml3_translation = test_feature(FEATURE_HUGE_PAGE);

		for(size_t i = 256; i < 512; i++)
		{
			this->get_next_lvl(this->top_lvl_->entries[i], true);
		}

		cpu::enable_pat();
	}
	else
	{
		for(size_t i = 256; i < 512; i++)
		{
			this->top_lvl_->entries[i] = base_pagemap.top_lvl_->entries[i];
		}
	}
}

void* PageMap::get_next_lvl(PageTableEntry& entry, bool allocate, uintptr_t virtual_address,
							size_t old_page_size, size_t page_size)
{
	void* ret = nullptr;

	if(entry.is_valid())
	{
		if(entry.is_large() && (old_page_size != static_cast<size_t>(-1)))
		{
			size_t old_flags = this->vmm_flags(entry.get_flags(), old_page_size > PAGE_SIZE);
			uintptr_t old_phys_address = entry.get_address();
			uintptr_t old_virt_address = virtual_address & ~(old_page_size - 1);

			if(old_phys_address & (old_page_size - 1))
			{
				log_panik("Unexpected Page Table Entry Address: %p",
						  reinterpret_cast<void*>(old_phys_address));
			}

			ret = memory::physical_allocate();
			entry.set_address(reinterpret_cast<uintptr_t>(ret));
			entry.set_flags(NEW_PAGE_FLAGS);

			for(size_t i = 0; i < old_page_size; i += page_size)
			{
				this->map_page(old_virt_address + i, old_phys_address + i,
							   old_flags | get_page_size_flags(page_size));
			}
		}
		else
		{
			ret = reinterpret_cast<void*>(entry.get_address());
		}
	}
	else if(allocate)
	{
		ret = memory::physical_allocate();
		entry.set_address(reinterpret_cast<uintptr_t>(ret));
		entry.set_flags(NEW_PAGE_FLAGS);
	}

	return memory::to_higher_half(ret);
}

PageTableEntry* PageMap::virtual_to_entry(uintptr_t virtual_address, bool allocate,
										  size_t page_size, bool check_large)
{
	const std::size_t pml5_entry = GET_PML_ENTRY(virtual_address, 48);
	const std::size_t pml4_entry = GET_PML_ENTRY(virtual_address, 39);
	const std::size_t pdp_entry = GET_PML_ENTRY(virtual_address, 30);
	const std::size_t pd_entry = GET_PML_ENTRY(virtual_address, 21);
	const std::size_t pt_entry = GET_PML_ENTRY(virtual_address, 12);

	if(this->top_lvl_ == nullptr)
	{
		return nullptr;
	}

	PageTable* pml4 = this->top_lvl_;

	if(is_paging_mode_max())
	{
		pml4 = static_cast<PageTable*>(
			this->get_next_lvl(this->top_lvl_->entries[pml5_entry], allocate));
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

uintptr_t PageMap::virtual_to_physical(uintptr_t virtual_address, size_t flags)
{
	lock::ScopedLock guard(this->lock_);

	const size_t page_size = flag_to_page_size(flags);
	PageTableEntry* pml_entry = this->virtual_to_entry(virtual_address, false, page_size, true);

	if((pml_entry == nullptr) || !pml_entry->get_flags(PAGE_FLAG_PRESENT))
	{
		return uintptr_t(-1);
	}

	return pml_entry->get_address() + (virtual_address % page_size);
}

size_t PageMap::vmm_flags(size_t flags, bool large_pages)
{
	const uint64_t patbit = (large_pages ? (1 << 7) : (1 << 12));
	size_t ret = 0;

	if(flags & PAGE_FLAG_PRESENT)
	{
		ret |= MAP_READ;
	}

	if(flags & PAGE_FLAG_WRITABLE)
	{
		ret |= MAP_WRITE;
	}

	if(!(flags & PAGE_FLAG_NO_EXECUTE))
	{
		ret |= MAP_EXEC;
	}

	if(flags & PAGE_FLAG_USER_ACCESSIBLE)
	{
		ret |= MAP_USER;
	}

	if(flags & PAGE_FLAG_GLOBAL)
	{
		ret |= MAP_GLOBAL;
	}

#define MATCH_FLAGS(flags, match) (((flags) & (match)) == (match))
	if(MATCH_FLAGS(flags, patbit | PAGE_FLAG_NO_CACHE | PAGE_FLAG_WRITE_THROUGH))
	{
		ret |= MAP_NO_CACHE;
	}
	else if(MATCH_FLAGS(flags, patbit | PAGE_FLAG_NO_CACHE))
	{
		ret |= MAP_WRITE_BACK;
	}
	else if(MATCH_FLAGS(flags, patbit | PAGE_FLAG_WRITE_THROUGH))
	{
		ret |= MAP_PROTECTED;
	}
	else if(MATCH_FLAGS(flags, patbit))
	{
		ret |= MAP_WRITE_THROUGH;
	}
	else if(MATCH_FLAGS(flags, PAGE_FLAG_NO_CACHE | PAGE_FLAG_WRITE_THROUGH))
	{
		ret |= MAP_WRITE_COMBINING;
	}
	else if(MATCH_FLAGS(flags, PAGE_FLAG_NO_CACHE))
	{
		ret |= MAP_MMIO;
	}
#undef MATCH_FLAGS

	return ret;
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

	lock::ScopedLock guard(this->lock_);

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

error_t PageMap::unmap_page(uintptr_t virtual_address, size_t flags)
{
	auto unmap_one = [this](uintptr_t virtual_address, size_t page_size) {
		PageTableEntry* pml_entry = this->virtual_to_entry(virtual_address, false, page_size, true);

		if(pml_entry == nullptr)
		{
			log_error("Could not get pagemap entry for address 0x%.16lx", virtual_address);
			return SYSTEM_ERR_ADDRESS_UNREACHABLE;
		}

		pml_entry->reset();
		cpu::invalidate_page(virtual_address);

		return SYSTEM_OK;
	};

	lock::ScopedLock guard(this->lock_);
	const size_t page_size = flag_to_page_size(flags);

	if((page_size == PAGE_SIZE_1GiB) && !virt::pml3_translation)
	{
		for(size_t i = 0; i < PAGE_SIZE_1GiB; i += PAGE_SIZE_2MiB)
		{
			if(unmap_one(virtual_address + i, PAGE_SIZE_2MiB) != SYSTEM_OK)
			{
				return SYSTEM_ERR_ADDRESS_UNREACHABLE;
			}
		}

		return SYSTEM_OK;
	}

	return unmap_one(virtual_address, page_size);
}

error_t PageMap::setflags_page(uintptr_t virtual_address, size_t flags)
{
	lock::ScopedLock guard(this->lock_);

	const size_t page_size = flag_to_page_size(flags);
	const size_t parsed_flags = this->parse_flags(flags);
	PageTableEntry* pml_entry = this->virtual_to_entry(virtual_address, true, page_size, true);

	if(pml_entry == nullptr)
	{
		log_error("Could not get pagemap entry for address 0x%.16lx", virtual_address);
		return SYSTEM_ERR_ADDRESS_UNREACHABLE;
	}

	uintptr_t physical_address = pml_entry->get_address();

	pml_entry->reset();
	pml_entry->set_address(physical_address);
	pml_entry->set_flags(parsed_flags);

	return SYSTEM_OK;
}

error_t PageMap::remap_page(uintptr_t old_virtual_address, uintptr_t new_virtual_address,
							size_t flags)
{
	uintptr_t physical_address = this->virtual_to_physical(old_virtual_address, flags);
	this->unmap_page(old_virtual_address, flags);
	return this->map_page(new_virtual_address, physical_address, flags);
}

error_t PageMap::map_pages(uintptr_t virtual_address, uintptr_t physical_address, size_t size,
						   size_t flags)
{
	const size_t page_size = flag_to_page_size(flags);

	for(size_t i = 0; i < size; i += page_size)
	{
		if(this->map_page(virtual_address + i, physical_address + i, flags) != SYSTEM_OK)
		{
			this->unmap_pages(virtual_address, i - page_size, flags);
			return SYSTEM_ERR_ADDRESS_UNREACHABLE;
		}
	}

	return SYSTEM_OK;
}

error_t PageMap::unmap_pages(uintptr_t virtual_address, size_t size, size_t flags)
{
	const size_t page_size = flag_to_page_size(flags);

	for(size_t i = 0; i < size; i += page_size)
	{
		if(this->unmap_page(virtual_address + i, flags) != SYSTEM_OK)
		{
			return SYSTEM_ERR_ADDRESS_UNREACHABLE;
		}
	}

	return SYSTEM_OK;
}

error_t PageMap::setflags_pages(uintptr_t virtual_address, size_t size, size_t flags)
{
	const size_t page_size = flag_to_page_size(flags);

	for(size_t i = 0; i < size; i += page_size)
	{
		if(this->setflags_page(virtual_address + i, flags) != SYSTEM_OK)
		{
			return SYSTEM_ERR_ADDRESS_UNREACHABLE;
		}
	}

	return SYSTEM_OK;
}

error_t PageMap::remap_pages(uintptr_t old_virtual_address, uintptr_t new_virtual_address,
							 size_t size, size_t flags)
{
	const size_t page_size = flag_to_page_size(flags);

	for(size_t i = 0; i < size; i += page_size)
	{
		if(this->remap_page(old_virtual_address + i, new_virtual_address + i, flags) != SYSTEM_OK)
		{
			return SYSTEM_ERR_ADDRESS_UNREACHABLE;
		}
	}

	return SYSTEM_OK;
}

void PageMap::load()
{
	cpu::write_cr3(from_higher_half(reinterpret_cast<uintptr_t>(this->top_lvl_)));
}

void PageMap::save()
{
	this->top_lvl_ = reinterpret_cast<PageTable*>(to_higher_half(cpu::read_cr3()));
}

void PageMap::destroy_level(PageTable* pml, int start, int end, int level)
{
	if((level == 0) || (pml == nullptr))
	{
		return;
	}

	for(size_t i = start; i < end; i++)
	{
		PageTable* next = static_cast<PageTable*>(this->get_next_lvl(pml->entries[i], false));

		if(next == nullptr)
		{
			continue;
		}

		destroy_level(next, 0, 512, level - 1);
	}

	physical_free(from_higher_half(this->top_lvl_));
}
} // namespace memory