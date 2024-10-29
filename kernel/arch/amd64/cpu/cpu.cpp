#include "cpu/lapic.hpp"
#include <arch.hpp>

#include <cpu/gdt.hpp>
#include <cpu/cpu.hpp>
#include <cpu/idt.hpp>
#include <cpu/fpu.hpp>
#include <cpu/ioapic.hpp>

namespace cpu
{
void initialize()
{
	disable_interrupts();

	gdt::initialize();
	interrupts::initialize();
	fpu::initialize();

	enable_interrupts();
}

void late_initialize()
{
	disable_interrupts();
	
	apic::initialize_ioapic();

	apic::initialize_lapic_virt();
	apic::initialize_lapic();
	
	enable_interrupts();
}
} // namespace cpu