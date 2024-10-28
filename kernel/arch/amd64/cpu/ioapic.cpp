#include "drivers/acpi.hpp"
#include "drivers/interrupts.hpp"
#include "libs/mmio.hpp"
#include "lock.hpp"
#include "logger.h"
#include "memory/memory.hpp"
#include "memory/virtual.hpp"
#include "uacpi/acpi.h"
#include <array>
#include <assert.h>
#include <cpu/ioapic.hpp>
#include <libs/vectors.hpp>
#include <cpu/idt.hpp>
#include <limits>

#define IO_APIC_IND(base) ((volatile uint32_t*)(((uint8_t*)(base)) + IO_APIC_IOREGSEL))
#define IO_APIC_DAT(base) ((volatile uint32_t*)(((uint8_t*)(base)) + IO_APIC_IOWIN))
#define IO_APIC_EOIR(base) ((volatile uint32_t*)(((uint8_t*)(base)) + 0x40))

// The minimum address space required past the base address
#define IO_APIC_WINDOW_SIZE 0x44

// The minimum version that supported the EOIR
#define IO_APIC_EOIR_MIN_VERSION 0x20

// IO APIC register offsets
#define IO_APIC_REG_RTE(idx) (0x10 + 2 * (idx))

// Macros for extracting data from REG_ID
#define IO_APIC_ID_ID(v) (((v) >> 24) & 0xf)

// Macros for extracting data from REG_VER
#define IO_APIC_VER_MAX_REDIR_ENTRY(v) (((v) >> 16) & 0xff)
#define IO_APIC_VER_VERSION(v) ((v) & 0xff)

// Macros for writing REG_RTE entries
#define IO_APIC_RTE_DST(v) (((uint64_t)(v)) << 56)
#define IO_APIC_RTE_EXTENDED_DST_ID(v) (((uint64_t)((v) & 0xf)) << 48)
#define IO_APIC_RTE_MASKED (1ULL << 16)
#define IO_APIC_RTE_TRIGGER_MODE(tm) (((uint64_t)(tm)) << 15)
#define IO_APIC_RTE_POLARITY(p) (((uint64_t)(p)) << 13)
#define IO_APIC_RTE_DST_MODE(dm) (((uint64_t)(dm)) << 11)
#define IO_APIC_RTE_DELIVERY_MODE(dm) ((((uint64_t)(dm)) & 0x7) << 8)
#define IO_APIC_RTE_VECTOR(x) (((uint64_t)(x)) & 0xff)
#define IO_APIC_RTE_MASK IO_APIC_RTE_VECTOR(0xff)

// Macros for reading REG_RTE entries
#define IO_APIC_RTE_REMOTE_IRR (1ULL << 14)
#define IO_APIC_RTE_DELIVERY_STATUS (1ULL << 12)
#define IO_APIC_RTE_GET_POLARITY(r) ((int)(((r) >> 13) & 0x1))
#define IO_APIC_RTE_GET_TRIGGER_MODE(r) ((int)(((r) >> 15) & 0x1))
#define IO_APIC_RTE_GET_VECTOR(r) ((uint8_t)((r) & 0xFF))

#define IO_APIC_NUM_REDIRECTIONS 120

namespace cpu
{
namespace apic
{
struct IoApic
{
	IoApicDescriptor desc;
	void* virtual_addr;
	uint8_t version;
	uint8_t max_redirection_entry;
	std::array<uint64_t, IO_APIC_NUM_REDIRECTIONS> saved_rtes;
};

lock::InterruptLock io_apic_lock;
std::array<IoApicIsaOverride, NUM_ISA_IRQS> isa_overrides;
std::vector<IoApic> io_apics;

IoApic* resolve_global_irq_unsafe(uint32_t irq)
{
	for(size_t i = 0; i < io_apics.size(); ++i)
	{
		uint32_t start = io_apics[i].desc.global_irq_base;
		uint32_t end = start + io_apics[i].max_redirection_entry;

		if(start <= irq && irq <= end)
		{
			return &io_apics[i];
		}
	}

	return nullptr;
}

IoApic* resolve_global_irq(uint32_t irq)
{
	IoApic* ret = resolve_global_irq_unsafe(irq);

	if(ret == nullptr)
	{
		log_panic("Could not resolve global irq %u", irq);
		log_panik("Could not resolve irq");
	}

	return ret;
}

uint32_t read_reg(IoApic* io_apic, uint8_t reg)
{
	// lock::ScopedLock guard(io_apic_lock);
	assert(io_apic != nullptr);

	mmio_out(IO_APIC_IND(io_apic->virtual_addr), reg);
	uint32_t val = mmio_in<uint32_t>(IO_APIC_DAT(io_apic->virtual_addr));

	return val;
}

void write_reg(IoApic* io_apic, uint8_t reg, uint32_t val)
{
	assert(io_apic != nullptr);

	mmio_out(IO_APIC_IND(io_apic->virtual_addr), reg);
	mmio_out(IO_APIC_DAT(io_apic->virtual_addr), val);
}

uint64_t read_redirection_entry(IoApic* io_apic, uint32_t global_irq)
{
	lock::ScopedLock guard(io_apic_lock);

	assert(global_irq >= io_apic->desc.global_irq_base);
	const uint32_t offset = global_irq - io_apic->desc.global_irq_base;
	assert(offset <= io_apic->max_redirection_entry);

	const uint8_t reg_id = static_cast<uint8_t>(IO_APIC_REG_RTE(offset));

	uint64_t result = 0;
	result |= read_reg(io_apic, reg_id);
	result |= static_cast<uint64_t>(read_reg(io_apic, static_cast<uint8_t>(reg_id + 1))) << 32;

	return result;
}

void write_redirection_entry(IoApic* io_apic, uint32_t global_irq, uint64_t value)
{
	lock::ScopedLock guard(io_apic_lock);

	assert(global_irq >= io_apic->desc.global_irq_base);
	const uint32_t offset = global_irq - io_apic->desc.global_irq_base;
	assert(offset <= io_apic->max_redirection_entry);

	const uint8_t reg_id = static_cast<uint8_t>(IO_APIC_REG_RTE(offset));
	write_reg(io_apic, reg_id, static_cast<uint32_t>(value));
	write_reg(io_apic, static_cast<uint8_t>(reg_id + 1), static_cast<uint32_t>(value >> 32));
}

bool is_valid_irq(uint32_t global_irq)
{
	return resolve_global_irq_unsafe(global_irq) != nullptr;
}

void io_apic_eoi(uint32_t global_irq, uint8_t vector)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	assert(io_apic->version >= IO_APIC_EOIR_MIN_VERSION);

	mmio_out(IO_APIC_EOIR(io_apic->virtual_addr), vector);
}

void set_irq_mask(uint32_t global_irq)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	uint64_t reg = read_redirection_entry(io_apic, global_irq);

	reg |= IO_APIC_RTE_MASKED;

	write_redirection_entry(io_apic, global_irq, reg);
}

void clear_irq_mask(uint32_t global_irq)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	uint64_t reg = read_redirection_entry(io_apic, global_irq);

	assert((IO_APIC_RTE_GET_VECTOR(reg) >= PLATFORM_INTERRUPT_BASE) &&
		   (IO_APIC_RTE_VECTOR(reg) <= PLATFORM_MAX));
	reg &= ~IO_APIC_RTE_MASKED;

	write_redirection_entry(io_apic, global_irq, reg);
}

void configure_irq(uint32_t global_irq, int trigger_mode, int polarity,
				   interrupt_delivery_mode delivery_mode, bool mask, interrupt_dst_mode dest_mode,
				   uint8_t dest, uint8_t vector)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	if(((delivery_mode == DELIVERY_MODE_FIXED) || (delivery_mode == DELIVERY_MODE_LOWEST_PRI)) &&
	   ((vector < PLATFORM_INTERRUPT_BASE) || (vector > PLATFORM_MAX)))
	{
		mask = true;
	}

	uint64_t reg = 0;
	reg |= IO_APIC_RTE_TRIGGER_MODE(trigger_mode);
	reg |= IO_APIC_RTE_POLARITY(polarity);
	reg |= IO_APIC_RTE_DELIVERY_MODE(delivery_mode);
	reg |= IO_APIC_RTE_DST_MODE(dest_mode);
	reg |= IO_APIC_RTE_DST(dest);
	reg |= IO_APIC_RTE_VECTOR(vector);

	if(mask)
	{
		reg |= IO_APIC_RTE_MASKED;
	}

	write_redirection_entry(io_apic, global_irq, reg);
}

void fetch_irq_config(uint32_t global_irq, int* trigger_mode, int* polarity)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	uint64_t reg = read_redirection_entry(io_apic, global_irq);

	if(trigger_mode)
	{
		*trigger_mode = IO_APIC_RTE_GET_TRIGGER_MODE(reg);
	}

	if(polarity)
	{
		*polarity = IO_APIC_RTE_GET_POLARITY(reg);
	}
}

void configure_irq_vector(uint32_t global_irq, uint8_t vector)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	uint64_t reg = read_redirection_entry(io_apic, global_irq);

	if((vector < PLATFORM_INTERRUPT_BASE) || (vector > PLATFORM_MAX))
	{
		reg |= IO_APIC_RTE_MASKED;
	}

	reg &= ~IO_APIC_RTE_MASK;
	reg |= IO_APIC_RTE_VECTOR(vector);

	write_redirection_entry(io_apic, global_irq, reg);
}

uint8_t fetch_irq_vector(uint32_t global_irq)
{
	IoApic* io_apic = resolve_global_irq(global_irq);

	lock::ScopedLock guard(io_apic_lock);

	uint64_t reg = read_redirection_entry(io_apic, global_irq);
	return IO_APIC_RTE_GET_VECTOR(reg);
}

void set_isa_irq_mask(uint8_t isa_irq)
{
	assert(isa_irq < NUM_ISA_IRQS);

	uint32_t global_irq = isa_irq;

	if(isa_overrides[isa_irq].remapped)
	{
		global_irq = isa_overrides[isa_irq].global_irq;
	}

	set_irq_mask(global_irq);
}

void clear_isa_irq_mask(uint8_t isa_irq)
{
	assert(isa_irq < NUM_ISA_IRQS);

	uint32_t global_irq = isa_irq;

	if(isa_overrides[isa_irq].remapped)
	{
		global_irq = isa_overrides[isa_irq].global_irq;
	}

	clear_irq_mask(global_irq);
}

void configure_isa_irq(uint8_t isa_irq, interrupt_delivery_mode delivery_mode, bool mask,
					   interrupt_dst_mode dest_mode, uint8_t dest, uint8_t vector)
{
	assert(isa_irq < NUM_ISA_IRQS);
	uint32_t global_irq = isa_irq;

	int trigger_mode = TRIGGER_MODE_EDGE;
	int polarity = POLARITY_ACTIVE_HIGH;

	if(isa_overrides[isa_irq].remapped)
	{
		global_irq = isa_overrides[isa_irq].global_irq;
		trigger_mode = isa_overrides[isa_irq].trigger_mode;
		polarity = isa_overrides[isa_irq].polarity;
	}

	configure_irq(global_irq, trigger_mode, polarity, delivery_mode, mask, dest_mode, dest, vector);
}

uint32_t isa_to_global(uint8_t isa_irq)
{
	assert(isa_irq < NUM_ISA_IRQS);

	if(isa_overrides[isa_irq].remapped)
	{
		return isa_overrides[isa_irq].global_irq;
	}

	return isa_irq;
}

void save_io_apic()
{
	lock::ScopedLock guard(io_apic_lock);

	for(size_t i = 0; i < io_apics.size(); i++)
	{
		IoApic* apic = &io_apics[i];

		for(uint8_t j = 0; j <= apic->max_redirection_entry; ++j)
		{
			uint32_t global_irq = apic->desc.global_irq_base + j;
			apic->saved_rtes[j] = read_redirection_entry(apic, global_irq);
		}
	}
}

void restore_io_apic()
{
	lock::ScopedLock guard(io_apic_lock);

	for(size_t i = 0; i < io_apics.size(); i++)
	{
		IoApic* apic = &io_apics[i];

		for(uint8_t j = 0; j <= apic->max_redirection_entry; ++j)
		{
			uint32_t global_irq = apic->desc.global_irq_base + j;
			write_redirection_entry(apic, global_irq, apic->saved_rtes[j]);
		}
	}
}

GsiRange get_gsi_range()
{
	assert(!io_apics.empty());

	lock::ScopedLock guard(io_apic_lock);
	GsiRange range = {std::numeric_limits<uint32_t>::max(), 0};

	for(const IoApic& apic: io_apics)
	{
		range.start = std::min(range.start, apic.desc.global_irq_base);
		range.end = std::max(range.end, apic.desc.global_irq_base + apic.max_redirection_entry);
	}

	return range;
}

IoApicIsaOverride parse_isa_overrides(const acpi_madt_interrupt_source_override* record)
{
	if(record->bus != 0)
	{
		log_panik("Invalid bus for IO APIC interrupt override.");
	}

	const uint32_t flags = record->flags;

	const int polarity = [flags]() {
		uint32_t polarity = flags & ACPI_MADT_POLARITY_MASK;

		switch(polarity)
		{
			case ACPI_MADT_POLARITY_CONFORMING:
			case ACPI_MADT_POLARITY_ACTIVE_HIGH:
				return POLARITY_ACTIVE_HIGH;
			case ACPI_MADT_POLARITY_ACTIVE_LOW:
				return POLARITY_ACTIVE_LOW;
			default:
				log_panic("Unknown IRQ polarity in override: %u", polarity);
				log_panik("Unknown Polarity");
		}

		return 0;
	}();

	const int trigger_mode = [flags]() {
		uint32_t trigger = flags & ACPI_MADT_TRIGGERING_MASK;

		switch(trigger)
		{
			case ACPI_MADT_TRIGGERING_CONFORMING:
			case ACPI_MADT_TRIGGERING_EDGE:
				return TRIGGER_MODE_EDGE;
			case ACPI_MADT_TRIGGERING_LEVEL:
				return TRIGGER_MODE_LEVEL;
			default:
				log_panic("Unknown IRQ trigger in override: %u", trigger);
				log_panik("Unknown trigger");
		}

		return 0;
	}();

	return {
		.isa_irq = record->source,
		.remapped = true,
		.trigger_mode = trigger_mode,
		.polarity = polarity,
		.global_irq = record->gsi,
	};
}

void initialize_ioapic()
{
	assert(io_apics.empty());

	std::vector<acpi_madt_ioapic> ioapics;
	std::vector<acpi_madt_interrupt_source_override> overrides;

	drivers::acpi::get_io_apic(ioapics);
	drivers::acpi::get_interrupt_overrides(overrides);

	std::vector<IoApicDescriptor> descriptors;

	for(const auto& apic: ioapics)
	{
		descriptors.push_back({
			apic.id,
			apic.gsi_base,
			apic.address,
		});
	}

	std::vector<IoApicIsaOverride> isa_override;

	for(const auto& override: overrides)
	{
		isa_override.push_back(parse_isa_overrides(&override));
	}

	descriptors.shrink_to_fit();
	isa_override.shrink_to_fit();

	size_t desc_size = descriptors.size();

	for(size_t i = 0; i < desc_size; i++)
	{
		io_apics.push_back({});
		io_apics[i].desc = descriptors[i];
	}

	for(size_t i = 0; i < desc_size; i++)
	{
		IoApic* apic = &io_apics[i];
		const uintptr_t phys_addr = apic->desc.paddr;
		void* virtual_addr = nullptr;

		const uintptr_t phys_base = memory::align_down(phys_addr, PAGE_SIZE);

		assert(phys_addr + IO_APIC_WINDOW_SIZE <= phys_base + PAGE_SIZE);

		// Check if a previous IO APIC shared the same page as this one
		for(size_t j = 0; j < i; j++)
		{
			if(memory::align_down(io_apics[j].desc.paddr, PAGE_SIZE) == phys_base)
			{
				virtual_addr = reinterpret_cast<void*>(memory::align_down(
					reinterpret_cast<uintptr_t>(io_apics[j].virtual_addr), PAGE_SIZE));
				break;
			}
		}

		if(virtual_addr == nullptr)
		{
			virtual_addr =
				memory::virtual_allocate_at(phys_base, 1, MAP_READ | MAP_WRITE | MAP_MMIO);
		}

		// Account for offset
		virtual_addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virtual_addr) +
											   phys_addr - phys_base);

		// Populate the rest of the descriptor
		apic->virtual_addr = virtual_addr;

		// lock::ScopedLock guard(io_apic_lock);

		uint32_t version = read_reg(apic, IO_APIC_REG_VER);
		apic->version = IO_APIC_VER_VERSION(version);
		apic->max_redirection_entry = IO_APIC_VER_MAX_REDIR_ENTRY(version);

		log_debug("Found an IO APIC at phys %p, virt %p, ver %08x",
				  reinterpret_cast<void*>(phys_addr), virtual_addr, version);

		if(apic->max_redirection_entry > IO_APIC_NUM_REDIRECTIONS - 1)
		{
			log_debug("IO APIC supports more redirections than kernel: %08x", version);
			apic->max_redirection_entry = IO_APIC_NUM_REDIRECTIONS - 1;
		}

		// Cleanout the redirection entries
		for(int j = 0; j <= apic->max_redirection_entry; j++)
		{
			write_redirection_entry(apic, j + apic->desc.global_irq_base, IO_APIC_RTE_MASKED);
		}
	}

	for(int i = 0; i < isa_override.size(); i++)
	{
		uint8_t isa_irq = isa_override[i].isa_irq;
		assert(isa_irq < NUM_ISA_IRQS);
		isa_overrides[isa_irq] = isa_override[i];
		log_debug("ISA IRQ override for ISA IRQ %u, mapping to %u", isa_irq,
				  isa_overrides[i].global_irq);
	}
}
} // namespace apic
} // namespace cpu