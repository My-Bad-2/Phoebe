#include <arch.hpp>
#include <cpu/gdt.hpp>
#include <cpu/cpu.hpp>

namespace cpu
{
void initialize()
{
	disable_interrupts();
	gdt::initialize();
	enable_interrupts();
}
} // namespace cpu