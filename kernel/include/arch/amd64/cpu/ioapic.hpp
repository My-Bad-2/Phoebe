#include <cstdint>
#ifndef CPU_IOAPIC_HPP
	#define CPU_IOAPIC_HPP 1

	#include <drivers/interrupts.hpp>

	#define NUM_ISA_IRQS 16

	#define IO_APIC_IOREGSEL 0x00
	#define IO_APIC_IOWIN 0x10

	#define IO_APIC_REG_ID 0x00
	#define IO_APIC_REG_VER 0x01

namespace cpu
{
namespace apic
{
enum interrupt_delivery_mode
{
	DELIVERY_MODE_FIXED = 0,
	DELIVERY_MODE_LOWEST_PRI = 1,
	DELIVERY_MODE_SMI = 2,
	DELIVERY_MODE_NMI = 4,
	DELIVERY_MODE_INIT = 5,
	DELIVERY_MODE_STARTUP = 6,
	DELIVERY_MODE_EXT_INT = 7,
};

enum interrupt_dst_mode
{
	DST_MODE_PHYSICAL = 0,
	DST_MODE_LOGICAL = 1,
};

// Global system interrupts in the range [start, end).
struct GsiRange
{
	uint32_t start;
	uint32_t end;
};

struct IoApicDescriptor
{
	uint8_t apic_id;
	uint32_t global_irq_base;
	uintptr_t paddr;
};

struct IoApicIsaOverride
{
	uint8_t isa_irq;
	bool remapped;
	int trigger_mode;
	int polarity;
	uint32_t global_irq;
};

void initialize_ioapic();
bool is_valid_irq(uint32_t global_irq);

void set_irq_mask(uint32_t global_irq);
void clear_irq_mask(uint32_t global_irq);

void configure_irq(uint32_t global_irq, int trigger_mode, int polarity,
				   interrupt_delivery_mode delivery_mode, bool mask, interrupt_dst_mode dest_mode,
				   uint8_t dest, uint8_t vector);
void fetch_irq_config(uint32_t global_irq, int* trigger_mode, int* polarity);

void configure_irq_vector(uint32_t global_irq, uint8_t vector);
uint8_t fetch_irq_vector(uint32_t global_irq);

void set_isa_irq_mask(uint8_t isa_irq);
void clear_isa_irq_mask(uint8_t isa_irq);

void configure_isa_irq(uint8_t isa_irq, interrupt_delivery_mode delivery_mode, bool mask,
					   interrupt_dst_mode dest_mode, uint8_t dest, uint8_t vector);
void io_apic_eoi(uint32_t global_irq, uint8_t vector);

uint32_t isa_to_global(uint8_t isa_irq);
GsiRange get_gsi_range();

void save_io_apic();
void restore_io_apic();

bool io_apic_initialized();
} // namespace apic
} // namespace cpu

#endif // CPU_IOAPIC_HPP