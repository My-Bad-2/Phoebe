#include <drivers/drivers.hpp>

namespace drivers
{
extern void arch_initialize();

void initialize()
{
	arch_initialize();
}
} // namespace drivers