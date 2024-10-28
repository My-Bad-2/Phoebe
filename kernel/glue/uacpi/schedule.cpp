#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

uacpi_status uacpi_kernel_schedule_work(uacpi_work_type, uacpi_work_handler, uacpi_handle ctx)
{
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_wait_for_work_completion()
{
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_thread_id uacpi_kernel_get_thread_id()
{
	return 0;
}