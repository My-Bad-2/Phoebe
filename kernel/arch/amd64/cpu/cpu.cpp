#include "cpu/lapic.hpp"
#include "cpu/smp.hpp"
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

	fpu::initialize();
	smp::initialize_bsp();

	enable_interrupts();
}

void late_initialize()
{
	disable_interrupts();

	apic::initialize_ioapic();
	smp::initialize();

	enable_interrupts();
}
} // namespace cpu