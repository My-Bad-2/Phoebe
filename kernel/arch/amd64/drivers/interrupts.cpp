#include <assert.h>

#include <cpu/pic.hpp>
#include <cpu/idt.hpp>
#include <drivers/interrupts.hpp>

namespace drivers
{
namespace interrupts
{
std::array<InterruptHandler, MAX_IDT_ENTRIES> handlers;

std::pair<InterruptHandler&, int> allocate_handler(int vector)
{
	if(vector < PLATFORM_INTERRUPT_BASE)
	{
		vector += PLATFORM_INTERRUPT_BASE;
	}

	for(int i = vector; i < MAX_IDT_ENTRIES; i++)
	{
		if(!handlers[i].handler && !handlers[i].reserved)
		{
			handlers[i].reserved = true;
			handlers[i].vector = i;

			return {handlers[i], i};
		}
	}

	log_panik("Out of Interrupt handlers");
	__UNREACHABLE();
}

InterruptHandler& get_handler(int vector)
{
	return handlers[vector];
}

void set_interrupt_mask(int vector)
{
	assert((vector >= PLATFORM_INTERRUPT_BASE) && (vector <= (PLATFORM_INTERRUPT_BASE + 16)));
	cpu::interrupts::pic_set_mask(vector - PLATFORM_INTERRUPT_BASE);
}

void clear_interrupt_mask(int vector)
{
	assert((vector >= PLATFORM_INTERRUPT_BASE) && (vector <= (PLATFORM_INTERRUPT_BASE + 16)));
	cpu::interrupts::pic_clear_mask(vector - PLATFORM_INTERRUPT_BASE);
}

void issue_eoi(int vector)
{
	assert((vector >= PLATFORM_INTERRUPT_BASE) && (vector <= (PLATFORM_INTERRUPT_BASE + 16)));
	cpu::interrupts::pic_send_eoi(vector - PLATFORM_INTERRUPT_BASE);
}
} // namespace interrupts
} // namespace drivers