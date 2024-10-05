#ifndef MEMORY_PAGING_HPP
#define MEMORY_PAGING_HPP 1

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/defs.h>

#include <utility>

#define PAGE_ADDR_MASK 0x000ffffffffff000

namespace memory
{
size_t page_size(size_t __flags = 0);
size_t get_page_size_flags(size_t __page_size);
std::pair<size_t, size_t> required_page_size(size_t __size);

struct PageTableEntry
{
	uintptr_t val = 0;

	constexpr void reset()
	{
		this->val = 0;
	}

	constexpr void set_flags(size_t __flags)
	{
		uintptr_t temp = this->val;
		temp |= __flags;
		this->val = temp;
	}

	constexpr void clear_flags(size_t __flags)
	{
		uintptr_t temp = this->val;
		temp &= ~__flags;
		this->val = temp;
	}

	constexpr bool get_flags(size_t __flags)
	{
		return (this->val & __flags) ? true : false;
	}

	constexpr size_t get_flags()
	{
		return this->val & ~PAGE_ADDR_MASK;
	}

	constexpr uintptr_t get_address()
	{
		return this->val & PAGE_ADDR_MASK;
	}

	constexpr void set_address(uintptr_t __address)
	{
		auto temp = this->val;
		temp &= ~PAGE_ADDR_MASK;
		temp |= __address;
		this->val = temp;
	}

	bool is_valid();
	bool is_large();
};

struct PageTable
{
	PageTableEntry entries[512];
} __ALIGNED(0x1000);

class PageMap
{
  public:
	PageMap() : top_lvl_(nullptr)
	{
	}

	~PageMap();

	constexpr PageTable* get_table() const
	{
		return this->top_lvl_;
	}

	void initialize(bool __kernel_pagemap = false);

	void* get_next_lvl(PageTableEntry& __entry, bool __allocate, uintptr_t __virtual_address = -1,
					   size_t __old_page_size = -1, size_t __page_size = -1);
	PageTableEntry* virtual_to_entry(uintptr_t __virtual_address, bool __allocate,
									 size_t __page_size, bool __check_large);
	uintptr_t virtual_to_physical(uintptr_t __virtual_address, size_t __flags = 0);

	error_t map_page(uintptr_t __virtual_address, uintptr_t __physical_address, size_t __flags);
	error_t unmap_page(uintptr_t __virtual_address, size_t __flags = 0);
	error_t setflags_page(uintptr_t __virtual_address, size_t __flags);
	error_t remap_page(uintptr_t __old_virtual_address, uintptr_t __new_virtual_address,
					   size_t __flags);

	error_t map_pages(uintptr_t __virtual_address, uintptr_t __physical_address, size_t __size,
					  size_t __flags);
	error_t unmap_pages(uintptr_t __virtual_address, size_t __size, size_t __flags = 0);
	error_t setflags_pages(uintptr_t __virtual_address, size_t __size, size_t __flags);
	error_t remap_pages(uintptr_t __old_virtual_address, uintptr_t __new_virtual_address,
						size_t __size, size_t __flags);

	void load();
	void save();

  private:
	size_t vmm_flags(size_t __flags, bool __large_pages);
	size_t parse_flags(size_t __flags);
	void destroy_level(PageTable* __pml, int __start, int __end, int __level);

	PageTable* top_lvl_ = nullptr;
};

extern PageMap base_pagemap;

PageMap* get_current_pagemap();
} // namespace memory

#endif // MEMORY_PAGING_HPP