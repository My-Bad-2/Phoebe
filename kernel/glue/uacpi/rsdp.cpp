#include <uacpi/types.h>
#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <memory/memory.hpp>
#include <kernel.h>
#include <logger.h>

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr* out_rdsp_address)
{
	uintptr_t rsdp_address = reinterpret_cast<uintptr_t>(rsdp_request.response->address);
	*out_rdsp_address = memory::from_higher_half(rsdp_address);

	return UACPI_STATUS_OK;
}
