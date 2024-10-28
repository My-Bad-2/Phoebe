#include "drivers/acpi.hpp"
#include <drivers/drivers.hpp>

namespace drivers
{
extern void arch_initialize();

void early_initialize() {
	arch_initialize();
}

void initialize()
{
	arch_initialize();
	acpi::initialize();
}
} // namespace drivers