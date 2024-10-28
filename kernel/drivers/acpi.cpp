#include <logger.h>
#include <drivers/acpi.hpp>
#include <kernel.h>

#include <uacpi/uacpi.h>
#include <uacpi/utilities.h>
#include <uacpi/types.h>
#include <uacpi/event.h>

#define MADT_SIGNATURE "APIC"

namespace drivers
{
namespace acpi
{
acpi_madt* madt_header = nullptr;

void initialize_madt()
{
	uacpi_table out_table = {};

	if(uacpi_table_find_by_signature(MADT_SIGNATURE, &out_table) != UACPI_STATUS_OK)
	{
		return;
	}

	// log_debug("%p", madt_header);
	madt_header = reinterpret_cast<acpi_madt*>(out_table.virt_addr);
}

void get_io_apic(std::vector<acpi_madt_ioapic>& ioapics)
{
	uintptr_t start = reinterpret_cast<uintptr_t>(madt_header->entries);
	uintptr_t end = reinterpret_cast<uintptr_t>(madt_header) + madt_header->hdr.length;

	acpi_entry_hdr* madt = reinterpret_cast<acpi_entry_hdr*>(start);

	for(uintptr_t entry = start; entry < end;
		entry += madt->length, madt = reinterpret_cast<acpi_entry_hdr*>(entry))
	{
		if(madt->type == ACPI_MADT_ENTRY_TYPE_IOAPIC)
		{
			ioapics.push_back(*reinterpret_cast<acpi_madt_ioapic*>(entry));
		}
	}

	ioapics.shrink_to_fit();
}

void get_interrupt_overrides(std::vector<acpi_madt_interrupt_source_override>& overrides)
{
	uintptr_t start = reinterpret_cast<uintptr_t>(madt_header->entries);
	uintptr_t end = reinterpret_cast<uintptr_t>(madt_header) + madt_header->hdr.length;

	acpi_entry_hdr* madt = reinterpret_cast<acpi_entry_hdr*>(start);

	for(uintptr_t entry = start; entry < end;
		entry += madt->length, madt = reinterpret_cast<acpi_entry_hdr*>(entry))
	{
		if(madt->type == ACPI_MADT_ENTRY_TYPE_INTERRUPT_SOURCE_OVERRIDE)
		{
			overrides.push_back(*reinterpret_cast<acpi_madt_interrupt_source_override*>(entry));
		}
	}

	overrides.shrink_to_fit();
}

void get_local_apic(std::vector<acpi_madt_lapic>& lapics)
{
	uintptr_t start = reinterpret_cast<uintptr_t>(madt_header->entries);
	uintptr_t end = reinterpret_cast<uintptr_t>(madt_header) + madt_header->hdr.length;

	acpi_entry_hdr* madt = reinterpret_cast<acpi_entry_hdr*>(start);

	for(uintptr_t entry = start; entry < end;
		entry += madt->length, madt = reinterpret_cast<acpi_entry_hdr*>(entry))
	{
		if(madt->type == ACPI_MADT_ENTRY_TYPE_LAPIC)
		{
			lapics.push_back(*reinterpret_cast<acpi_madt_lapic*>(entry));
		}
	}

	lapics.shrink_to_fit();
}

void initialize()
{
	log_begin_intialization("Advanced Configuration and Power Interface");

	uacpi_status ret = uacpi_initialize(0);

	if(uacpi_unlikely_error(ret))
	{
		log_error("uacpi_initialize error: %s", uacpi_status_to_string(ret));
	}

	initialize_madt();

	log_end_intialization();
}
} // namespace acpi
} // namespace drivers