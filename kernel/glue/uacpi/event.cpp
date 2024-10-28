#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

uacpi_handle uacpi_kernel_create_event()
{
	return nullptr;
}

void uacpi_kernel_free_event(uacpi_handle)
{
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle, uacpi_u16)
{
	return true;
}

void uacpi_kernel_signal_event(uacpi_handle)
{
}

void uacpi_kernel_reset_event(uacpi_handle)
{
}