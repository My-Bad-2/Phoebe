#ifndef KERNEL_H
#define KERNEL_H 1

#include <limine.h>
#include <sys/defs.h>
#include <stdbool.h>

__CDECLS_BEGIN
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_kernel_address_request kernel_address_request;
extern volatile struct limine_kernel_file_request kernel_file_request;
extern volatile struct limine_paging_mode_request paging_mode_request;

bool is_paging_mode_max(void);
__CDECLS_END

#endif // KERNEL_H