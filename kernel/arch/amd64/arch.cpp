#include <arch.hpp>
#include <cpu/cpu.hpp>

namespace arch
{
void initialize()
{
	cpu::initialize();
}
} // namespace arch