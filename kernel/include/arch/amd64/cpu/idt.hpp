#ifndef CPU_IDT_HPP
#define CPU_IDT_HPP 1

#include <array>
#include <errno.h>
#include <stdint.h>
#include <sys/defs.h>

#define EXCEPTION_DIVIDE_BY_0 (0)
#define EXCEPTION_DEBUG (1)
#define EXCEPTION_NON_MASKABLE_INTERRUPT (2)
#define EXCEPTION_BREAKPOINT (3)
#define EXCEPTION_OVERFLOW (4)
#define EXCEPTION_BOUND_RANGE (5)
#define EXCEPTION_INVALID_OPCODE (6)
#define EXCEPTION_DEVICE_NA (7)
#define EXCEPTION_DOUBLE_FAULT (8)
#define EXCEPTION_INVALID_TSS (10)
#define EXCEPTION_SEGMENT_NOT_PRESENT (11)
#define EXCEPTION_STACK_SEGMENT_FAULT (12)
#define EXCEPTION_GENERAL_PROTECTION_FAULT (13)
#define EXCEPTION_PAGE_FAULT (14)
#define EXCEPTION_x87_FLOATING_POINT (16)
#define EXCEPTION_ALIGNMENT_CHECK (17)
#define EXCEPTION_MACHINE_CHECK (18)
#define EXCEPTION_SIMD_FLOATING_POINT (19)
#define EXCEPTION_VIRTUALIZATION (20)
#define EXCEPTION_CONTROL_PROTECTION (21)
#define EXCEPTION_HYPERVISOR_INJECTION (28)
#define EXCEPTION_VMM_COMMUNICATION (29)
#define EXCEPTION_SECURITY (30)

#define PLATFORM_INTERRUPT_BASE (32)
#define PLATFORM_MAX (239)

#define IDT_INTERRUPT_GATE 0xe
#define IDT_TRAP_GATE 0xf

#define IDT_DPL0 (0)
#define IDT_DPL3 (3)

#define MAX_IDT_ENTRIES (256)

namespace cpu
{
namespace interrupts
{
struct IdtSegment
{
	uint16_t base_low;
	uint16_t selector;
	uint8_t ist;
	uint8_t type_attributes;
	uint16_t base_mid;
	uint32_t base_high;
	uint32_t reserved;

	void create_entry(void* __handler, uint8_t __ist, uint8_t __type, uint8_t __dpl,
					  uint16_t __selector);
} __PACKED;

struct IdtTable
{
	std::array<IdtSegment, MAX_IDT_ENTRIES> entries;

	IdtSegment& operator[](std::size_t __index)
	{
		return this->entries[__index];
	}
};

struct IdtRegister
{
	uint16_t limit;
	uint64_t base;
} __PACKED;

error_t initialize();
} // namespace interrupts
} // namespace cpu

#endif // CPU_IDT_HPP