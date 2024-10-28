#include <uacpi/types.h>
#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <memory/memory.hpp>
#include <memory/virtual.hpp>
#include <memory/heap.hpp>

void* uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len)
{
	uintptr_t physical_addr = memory::align_down(addr, PAGE_SIZE);
	size_t count = memory::div_roundup((addr - physical_addr) + len, PAGE_SIZE);

	void* virt_addr = memory::virtual_allocate_at(physical_addr, count, MAP_WRITE | MAP_READ);

	return reinterpret_cast<uint8_t*>(virt_addr) + (addr - physical_addr);
}

void uacpi_kernel_unmap(void* addr, uacpi_size len)
{
	uintptr_t address = reinterpret_cast<uintptr_t>(addr);
	uintptr_t virt_base = memory::align_down(address, PAGE_SIZE);
	size_t count = memory::div_roundup((address - virt_base) + len, PAGE_SIZE);

	memory::virtual_free_at(reinterpret_cast<void*>(virt_base), count);
}

void* uacpi_kernel_alloc(uacpi_size size)
{
	return memory::heap_malloc(size);
}

void* uacpi_kernel_calloc(uacpi_size count, uacpi_size size)
{
	return memory::heap_calloc(count, size);
}

void uacpi_kernel_free(void* mem)
{
	memory::heap_free(mem);
}

uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle) {
    *out_handle = reinterpret_cast<uacpi_handle>(base);
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) {}