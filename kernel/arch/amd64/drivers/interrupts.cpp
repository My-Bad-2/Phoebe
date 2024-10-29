#include <cpu/lapic.hpp>
#include <assert.h>

#include <cpu/pic.hpp>
#include <cpu/idt.hpp>
#include <cpu/ioapic.hpp>

#include <drivers/interrupts.hpp>
#include <drivers/acpi.hpp>

namespace drivers
{
namespace interrupts
{
std::array<InterruptHandler, MAX_IDT_ENTRIES> handlers;

std::pair<InterruptHandler&, int> allocate_handler(int vector)
{
	using namespace cpu::apic;

	if(vector < PLATFORM_INTERRUPT_BASE)
	{
		vector += PLATFORM_INTERRUPT_BASE;
	}

	if(acpi::legacy_pic())
	{
		if((vector >= PLATFORM_INTERRUPT_BASE) && (vector <= PLATFORM_INTERRUPT_BASE + 15) &&
		   !handlers[vector].reserved)
		{
			handlers[vector].reserved = true;
			handlers[vector].vector = vector;

			// Configure isa irqs for ioapic
			if(io_apic_initialized())
			{
				configure_isa_irq(vector - PLATFORM_INTERRUPT_BASE, DELIVERY_MODE_FIXED, false,
								  DST_MODE_PHYSICAL, 0, vector);
			}

			return {handlers[vector], vector};
		}
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
	using namespace cpu;
	assert((vector >= PLATFORM_INTERRUPT_BASE) && (vector <= (PLATFORM_INTERRUPT_BASE + 16)));

	if(acpi::legacy_pic() && apic::io_apic_initialized())
	{
		apic::set_irq_mask(vector);
	}
	else
	{
		cpu::interrupts::pic_set_mask(vector - PLATFORM_INTERRUPT_BASE);
	}
}

void clear_interrupt_mask(int vector)
{
	using namespace cpu;
	assert((vector >= PLATFORM_INTERRUPT_BASE) && (vector <= (PLATFORM_INTERRUPT_BASE + 16)));

	if(acpi::legacy_pic() && apic::io_apic_initialized())
	{
		apic::clear_irq_mask(vector);
	}
	else
	{
		cpu::interrupts::pic_clear_mask(vector - PLATFORM_INTERRUPT_BASE);
	}
}

void issue_eoi(int vector)
{
	assert((vector >= PLATFORM_INTERRUPT_BASE));

	if(cpu::apic::io_apic_initialized())
	{
		cpu::apic::issue_eoi();
	}
	else
	{
		cpu::interrupts::pic_send_eoi(vector - PLATFORM_INTERRUPT_BASE);
	}
}
} // namespace interrupts
} // namespace drivers