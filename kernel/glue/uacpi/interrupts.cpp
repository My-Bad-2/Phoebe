#include <uacpi/types.h>
#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <drivers/interrupts.hpp>

uacpi_status uacpi_kernel_install_interrupt_handler(uacpi_u32 irq, uacpi_interrupt_handler handle,
													uacpi_handle ctx, uacpi_handle* out_irq_handle)
{
	auto [handler, vector] = drivers::interrupts::allocate_handler(irq);
	handler.set([handle, ctx](Iframe*) {
		handle(ctx);
	});

	drivers::interrupts::clear_interrupt_mask(vector);

	*reinterpret_cast<size_t*>(out_irq_handle) = vector;
	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_uninstall_interrupt_handler(uacpi_interrupt_handler handle,
													  uacpi_handle irq_handle)
{
	size_t vector = *reinterpret_cast<size_t*>(irq_handle);
	drivers::interrupts::set_interrupt_mask(vector);

	drivers::interrupts::InterruptHandler& handler = drivers::interrupts::get_handler(vector);
	handler.reset();

	return UACPI_STATUS_OK;
}