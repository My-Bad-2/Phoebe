#ifndef CPU_LAPIC_HPP
#define CPU_LAPIC_HPP 1

#include <errno.h>
#include <stdint.h>
#include <stddef.h>

#include <cpu/ioapic.hpp>

#define INVALID_APIC_ID 0xffffffff
#define APIC_PHYS_BASE 0xfee00000
#define APIC_BASE_BSP (1u << 8)
#define APIC_BASE_X2APIC_ENABLE (1u << 10)
#define APIC_BASE_XAPIC_ENABLE (1u << 11)
#define NUM_ISA_IRQS 16

// LVT Timer bitmasks
#define LVT_TIMER_VECTOR_MASK 0x000000ff
#define LVT_TIMER_MODE_MASK 0x00060000
#define LVT_TIMER_MODE_ONESHOT (0u << 17)
#define LVT_TIMER_MODE_PERIODIC (1u << 17)
#define LVT_TIMER_MODE_TSC_DEADLINE (2u << 17)
#define LVT_TIMER_MODE_RESERVED (3u << 17)
#define LVT_MASKED (1u << 16)

namespace cpu
{
namespace apic
{
enum TimerModes
{
	TIMER_ONESHOT = 0,
	TIMER_PERIODIC,
	TIMER_TSC_DEADLINE,
};

void initialize_lapic_virt();
void initialize_lapic();

uint8_t local_id();
uint8_t bsp_id();

void send_ipi(uint8_t vector, uint32_t dst_apic_id, interrupt_delivery_mode delivery_mode);
void send_self_ipi(uint8_t vector, interrupt_delivery_mode delivery_mode);
void issue_eoi();

error_t timer_set_oneshot(uint32_t count, uint8_t divisor, bool masked);

void timer_set_tsc_deadline(uint64_t deadline);
uint32_t timer_current_count();

uint64_t get_apic_freq();

void timer_mask();
void timer_unmask();
void timer_stop();

void initialize_timer_tsc_deadline();

void pmi_mask();
void pmi_unmask();

bool is_x2apic_enabled();

void set_timer(uint8_t vector, size_t ticks, TimerModes mode);
} // namespace apic
} // namespace cpu

#endif // CPU_LAPIC_HPP