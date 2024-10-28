#include <uacpi/types.h>
#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <lock.hpp>

uacpi_handle uacpi_kernel_create_mutex()
{
	return reinterpret_cast<uacpi_handle>(new lock::mutex);
}

void uacpi_kernel_free_mutex(uacpi_handle handle)
{
	delete reinterpret_cast<lock::mutex*>(handle);
}

uacpi_bool uacpi_kernel_acquire_mutex(uacpi_handle handle, uacpi_u16 timeout)
{
	lock::mutex* mutex = reinterpret_cast<lock::mutex*>(handle);

	if(timeout == 0xffff)
	{
		mutex->lock();
		return true;
	}

	return mutex->try_lock(timeout);
}

void uacpi_kernel_release_mutex(uacpi_handle handle)
{
	reinterpret_cast<lock::mutex*>(handle)->unlock();
}

uacpi_handle uacpi_kernel_create_spinlock()
{
	return reinterpret_cast<void*>(new lock::InterruptLock);
}

void uacpi_kernel_free_spinlock(uacpi_handle handle)
{
	delete reinterpret_cast<lock::InterruptLock*>(handle);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle handle)
{
	uacpi_cpu_flags flags = arch::interrupt_status();
	reinterpret_cast<lock::InterruptLock*>(handle)->lock();
	return flags;
}

void uacpi_kernel_unlock_spinlock(uacpi_handle handle, uacpi_cpu_flags)
{
	reinterpret_cast<lock::InterruptLock*>(handle)->unlock();
}