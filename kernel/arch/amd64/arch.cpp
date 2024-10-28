#include <arch.hpp>
#include <cpu/cpu.hpp>

namespace arch
{
bool interrupt_status() {
	uint64_t rflags = 0;

	asm volatile("pushfq");
	asm volatile("pop %0" : "=r"(rflags));

	return rflags & FLAGS_IF;
}

void initialize()
{
	cpu::initialize();
}
} // namespace arch