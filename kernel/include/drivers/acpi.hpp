#ifndef DRIVERS_ACPI_HPP
#define DRIVERS_ACPI_HPP 1

#include <uacpi/tables.h>
#include <uacpi/acpi.h>

#include <libs/vector.hpp>

namespace drivers
{
namespace acpi
{
void get_io_apic(std::vector<acpi_madt_ioapic>& __ioapics);
void get_interrupt_overrides(std::vector<acpi_madt_interrupt_source_override>& __overrides);
void get_local_apic(std::vector<acpi_madt_lapic>& __lapics);

acpi_fadt* get_fadt();
bool legacy_pic();

void initialize();
void initialize_madt();
} // namespace acpi
} // namespace drivers

#endif // DRIVERS_ACPI_HPP