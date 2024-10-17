#include <arch.hpp>

#include <cpu/gdt.hpp>
#include <cpu/cpu.hpp>
#include <cpu/idt.hpp>
#include <cpu/fpu.hpp>

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
} // namespace cpu