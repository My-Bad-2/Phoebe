#include <kernel.h>
#include <stddef.h>

__SECTION(".limine_requests_start_marker") __USED static volatile LIMINE_REQUESTS_START_MARKER;
__SECTION(".limine_requests") __USED static volatile LIMINE_BASE_REVISION(2);

__SECTION(".limine_requests")
volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
	.response = NULL,
};

__SECTION(".limine_requests")
volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0,
	.response = NULL,
};

__SECTION(".limine_requests")
volatile struct limine_kernel_address_request kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0,
	.response = NULL,
};

__SECTION(".limine_requests")
volatile struct limine_kernel_file_request kernel_file_request = {
	.id = LIMINE_KERNEL_FILE_REQUEST,
	.revision = 0,
	.response = NULL,
};

__SECTION(".limine_requests")
volatile struct limine_paging_mode_request paging_mode_request = {
	.id = LIMINE_PAGING_MODE_REQUEST,
	.revision = 0,
	.response = NULL,
#if defined(__x86_64__)
	.mode = LIMINE_PAGING_MODE_X86_64_5LVL,
	.max_mode = LIMINE_PAGING_MODE_X86_64_5LVL,
#endif
	.min_mode = LIMINE_PAGING_MODE_MIN,
};

__SECTION(".limine_requests")
volatile struct limine_stack_size_request stack_size_request = {
	.id = LIMINE_STACK_SIZE_REQUEST,
	.revision = 0,
	.response = NULL,
	.stack_size = 0x200000ul,
};

__SECTION(".limine_requests")
volatile struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0,
	.response = NULL,
};

__SECTION(".limine_requests_end_marker") __USED static volatile LIMINE_REQUESTS_END_MARKER;

bool is_paging_mode_max()
{
	return paging_mode_request.response->mode != LIMINE_PAGING_MODE_MIN;
}