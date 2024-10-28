#include <uacpi/types.h>
#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <drivers/timers.hpp>

uacpi_u64 uacpi_kernel_get_ticks()
{
	return drivers::timers::get_time() / 10000;
}

void uacpi_kernel_stall(uacpi_u8)
{
	return drivers::timers::sleep(1);
}

void uacpi_kernel_sleep(uacpi_u64 msec)
{
	drivers::timers::sleep(msec);
}