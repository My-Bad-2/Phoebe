#include <utility>
#ifndef CPU_PAGING_HPP
	#define CPU_PAGING_HPP 1

	#include <errno.h>
	#include <stdint.h>
	#include <stddef.h>

namespace cpu
{
	#ifdef __x86_64__
constexpr uintptr_t page_addr_mask = 0x000ffffffffff000;
	#else
constexpr uintptr_t page_addr_mask = 0;
	#endif

size_t page_size(size_t flags = 0);
size_t get_page_size_flags(size_t page_size);
std::pair<size_t, size_t> required_page_size(size_t size);

struct PageTableEntry
{
	uintptr_t val = 0;

	constexpr void reset()
	{
		this->val = 0;
	}

	constexpr void set_flags(size_t flags)
	{
		uintptr_t temp = this->val;
		temp |= flags;
		this->val = temp;
	}

	constexpr void clear_flags(size_t flags)
	{
		uintptr_t temp = this->val;
		temp &= ~flags;
		this->val = temp;
	}

	constexpr bool get_flags(size_t flags)
	{
		return (this->val & flags) ? true : false;
	}

	constexpr size_t get_flags()
	{
		return this->val & ~page_addr_mask;
	}

	constexpr uintptr_t get_address()
	{
		return this->val & page_addr_mask;
	}

	constexpr void set_address(uintptr_t address)
	{
		auto temp = this->val;
		temp &= ~page_addr_mask;
		temp |= address;
		this->val = temp;
	}

	bool is_valid();
	bool is_large();
};

struct PageTable;

class PageMap
{
  public:
	PageMap();
	~PageMap();

	constexpr PageTable* get_table() const
	{
		return this->top_lvl;
	}

	void* get_next_lvl(PageTableEntry& entry, bool allocate, uintptr_t virtual_address = -1,
					   size_t old_page_size = -1, size_t page_size = -1);
	PageTableEntry* virtual_to_entry(uintptr_t virtual_address, bool allocate, size_t page_size,
									 bool check_large);

	error_t map_page(uintptr_t virtual_address, uintptr_t physical_address, size_t flags);
	error_t unmap_page(uintptr_t virtual_address, size_t flags);
	error_t set_flags_page(uintptr_t virtual_address, size_t flags);
	error_t remap_page(uintptr_t old_virtual_address, uintptr_t new_virtual_address, size_t flags);

	error_t map_pages(uintptr_t virtual_address, uintptr_t physical_address, size_t size,
					  size_t flags);
	error_t unmap_pages(uintptr_t virtual_address, size_t size, size_t flags);
	error_t set_flags_pages(uintptr_t virtual_address, size_t size, size_t flags);
	error_t remap_pages(uintptr_t old_virtual_address, uintptr_t new_virtual_address, size_t size,
						size_t flags);

	void load();
	void save();

  private:
	size_t parse_flags(size_t flags);

	PageTable* top_lvl = nullptr;
};
} // namespace cpu

#endif // CPU_PAGING_HPP