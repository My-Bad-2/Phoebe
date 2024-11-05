#include <assert.h>

#include <arch.hpp>

#include <cpu/cpu.hpp>
#include <cpu/features.h>
#include <cpu/idt.hpp>
#include <cpu/lapic.hpp>

#include <memory/virtual.hpp>

#include <drivers/timers.hpp>

#include <libs/mmio.hpp>

// local apic registers
// set as an offset into the mmio region here
// x2APIC msr offsets are these >> 4
#define LAPIC_REG_ID (0x020)
#define LAPIC_REG_VERSION (0x030)
#define LAPIC_REG_TASK_PRIORITY (0x080)
#define LAPIC_REG_PROCESSOR_PRIORITY (0x0A0)
#define LAPIC_REG_EOI (0x0B0)
#define LAPIC_REG_LOGICAL_DST (0x0D0)
#define LAPIC_REG_SPURIOUS_IRQ (0x0F0)
#define LAPIC_REG_IN_SERVICE(x) (0x100 + ((x) << 4))
#define LAPIC_REG_TRIGGER_MODE(x) (0x180 + ((x) << 4))
#define LAPIC_REG_IRQ_REQUEST(x) (0x200 + ((x) << 4))
#define LAPIC_REG_ERROR_STATUS (0x280)
#define LAPIC_REG_LVT_CMCI (0x2F0)
#define LAPIC_REG_IRQ_CMD_LOW (0x300)
#define LAPIC_REG_IRQ_CMD_HIGH (0x310)
#define LAPIC_REG_LVT_TIMER (0x320)
#define LAPIC_REG_LVT_THERMAL (0x330)
#define LAPIC_REG_LVT_PERF (0x340)
#define LAPIC_REG_LVT_LINT0 (0x350)
#define LAPIC_REG_LVT_LINT1 (0x360)
#define LAPIC_REG_LVT_ERROR (0x370)
#define LAPIC_REG_INIT_COUNT (0x380)
#define LAPIC_REG_CURRENT_COUNT (0x390)
#define LAPIC_REG_DIVIDE_CONF (0x3E0)

#define LAPIC_X2APIC_MSR_BASE (0x800)
#define LAPIC_X2APIC_MSR_ICR (0x830)
#define LAPIC_X2APIC_MSR_SELF_IPI (0x83f)

// Spurious IRQ bitmasks
#define SVR_APIC_ENABLE (1 << 8)
#define SVR_SPURIOUS_VECTOR(x) (x)

// Interrupt Command bitmasks
#define ICR_VECTOR(x) (x)
#define ICR_DELIVERY_PENDING (1 << 12)
#define ICR_LEVEL_ASSERT (1 << 14)
#define ICR_DST(x) (((uint32_t)(x)) << 24)
#define ICR_DST_BROADCAST ICR_DST(0xff)
#define ICR_DELIVERY_MODE(x) (((uint32_t)(x)) << 8)
#define ICR_DST_SHORTHAND(x) (((uint32_t)(x)) << 18)
#define ICR_DST_SELF ICR_DST_SHORTHAND(1)
#define ICR_DST_ALL ICR_DST_SHORTHAND(2)
#define ICR_DST_ALL_MINUS_SELF ICR_DST_SHORTHAND(3)

#define X2_ICR_DST(x) ((uint64_t)(x) << 32)
#define X2_ICR_BROADCAST ((uint64_t)(0xffffffff) << 32)

// Common LVT bitmasks
#define LVT_VECTOR(x) (x)
#define LVT_DELIVERY_MODE(x) (((uint32_t)(x)) << 8)
#define LVT_DELIVERY_PENDING (1 << 12)

namespace cpu
{
namespace apic
{
bool x2apic_enabled = false;
void* lapic_virt_base = nullptr;

uint8_t bsp_lapic_id = 0;
bool bsp_lapic_id_valid = false;

uint32_t read_reg(size_t offset)
{
	if(x2apic_enabled)
	{
		return read_msr(LAPIC_X2APIC_MSR_BASE + static_cast<uint32_t>(offset >> 4));
	}
	else
	{
		return mmio_in<uint32_t>(reinterpret_cast<uintptr_t>(lapic_virt_base) + offset);
	}
}

void write_reg(size_t offset, uint32_t val)
{
	if(x2apic_enabled)
	{
		write_msr(LAPIC_X2APIC_MSR_BASE + static_cast<uint32_t>(offset >> 4), val);
	}
	else
	{
		mmio_out<uint32_t>(reinterpret_cast<uintptr_t>(lapic_virt_base) + offset, val);
	}
}

void reg_or(size_t offset, uint32_t bits)
{
	write_reg(offset, read_reg(offset) | bits);
}

void reg_and(size_t offset, uint32_t bits)
{
	write_reg(offset, read_reg(offset) & bits);
}

void intialize_apic_error()
{
	write_reg(LAPIC_REG_LVT_ERROR, LVT_VECTOR(INTERRUPT_APIC_ERROR));
	write_reg(LAPIC_REG_ERROR_STATUS, 0);

	auto& handler = drivers::interrupts::get_handler(INTERRUPT_APIC_ERROR);
	handler.reserved = true;
	handler.vector = INTERRUPT_APIC_ERROR;

	handler.set([](Iframe*) {
		write_reg(LAPIC_REG_ERROR_STATUS, 0);
		log_panic("APIC error detected: %u", read_reg(LAPIC_REG_ERROR_STATUS));
		log_panik("APIC error!");
	});
}

void initialize_apic_pmi()
{
	write_reg(LAPIC_REG_LVT_PERF, LVT_VECTOR(INTERRUPT_APIC_PMI) | LVT_MASKED);

	auto& handler = drivers::interrupts::get_handler(INTERRUPT_APIC_PMI);
	handler.reserved = true;
	handler.vector = INTERRUPT_APIC_PMI;

	handler.set([](Iframe*) {
		log_error("Implement APIC PMI handler!");
	});
}

void initialize_timer_tsc_deadline()
{
	assert(test_feature(FEATURE_TSC_DEADLINE));

	write_reg(LAPIC_REG_LVT_TIMER, LVT_VECTOR(INTERRUPT_APIC_TIMER) | LVT_TIMER_MODE_TSC_DEADLINE);

	asm volatile("mfence" ::: "memory");
}

void initialize_apic_timer()
{
	write_reg(LAPIC_REG_LVT_TIMER, LVT_VECTOR(INTERRUPT_APIC_TIMER) | LVT_MASKED);

	if(test_feature(FEATURE_TSC_DEADLINE))
	{
		initialize_timer_tsc_deadline();
	}

	auto& handler = drivers::interrupts::get_handler(INTERRUPT_APIC_TIMER);
	handler.reserved = true;
	handler.vector = INTERRUPT_APIC_TIMER;

	handler.set([](Iframe*) {
		drivers::timers::tick();
	});
}

bool is_x2apic_enabled()
{
	return x2apic_enabled;
}

void initialize_lapic_virt()
{
	if(!x2apic_enabled)
	{
		assert(lapic_virt_base == nullptr);

		lapic_virt_base =
			memory::virtual_allocate_at(APIC_PHYS_BASE, 1, MAP_READ | MAP_WRITE | MAP_MMIO);

		assert(lapic_virt_base != nullptr);
	}
}

void initialize_lapic()
{
	uint64_t reg = read_msr(MSR_APIC_BASE);

	if(reg & APIC_BASE_BSP)
	{
		if(test_feature(FEATURE_X2APIC))
		{
			log_debug("x2APIC enabled!");
			x2apic_enabled = true;
		}
	}

	reg |= APIC_BASE_XAPIC_ENABLE;
	reg |= x2apic_enabled ? APIC_BASE_X2APIC_ENABLE : 0;

	write_msr(MSR_APIC_BASE, reg);

	if(reg & APIC_BASE_BSP)
	{
		uint8_t id = local_id();

		bsp_lapic_id = id;
		bsp_lapic_id_valid = true;
	}

	uint32_t svr = SVR_SPURIOUS_VECTOR(INTERRUPT_APIC_SPURIOUS) | SVR_APIC_ENABLE;
	write_reg(LAPIC_REG_SPURIOUS_IRQ, svr);

	intialize_apic_error();
	initialize_apic_timer();
	initialize_apic_pmi();
}

uint8_t local_id()
{
	uint32_t id = read_reg(LAPIC_REG_ID);

	if(!x2apic_enabled)
	{
		id >>= 24;
	}

	return static_cast<uint8_t>(id);
}

uint8_t bsp_id()
{
	assert(bsp_lapic_id_valid);
	return bsp_lapic_id;
}

inline void wait_for_ipi_send()
{
	while(read_reg(LAPIC_REG_IRQ_CMD_LOW) & ICR_DELIVERY_PENDING)
	{
	}
}

void send_ipi(uint8_t vector, uint32_t dest_apic_id, interrupt_delivery_mode delivery_mode)
{
	uint32_t request = ICR_LEVEL_ASSERT | ICR_DELIVERY_MODE(delivery_mode) | ICR_VECTOR(vector);

	if(x2apic_enabled)
	{
		return write_msr(LAPIC_X2APIC_MSR_ICR, X2_ICR_DST(dest_apic_id) | request);
	}

	disable_interrupts();

	write_reg(LAPIC_REG_IRQ_CMD_HIGH, ICR_DST(dest_apic_id));
	write_reg(LAPIC_REG_IRQ_CMD_LOW, request);

	wait_for_ipi_send();

	enable_interrupts();
}

void send_self_ipi(uint8_t vector, interrupt_delivery_mode delivery_mode)
{
	uint32_t request = ICR_LEVEL_ASSERT | ICR_DELIVERY_MODE(delivery_mode) | ICR_VECTOR(vector);
	request |= ICR_DST_SELF;

	if(x2apic_enabled)
	{
		return write_msr(LAPIC_X2APIC_MSR_SELF_IPI, vector);
	}

	disable_interrupts();

	write_reg(LAPIC_REG_IRQ_CMD_LOW, request);

	wait_for_ipi_send();

	enable_interrupts();
}

void issue_eoi()
{
	write_reg(LAPIC_REG_EOI, 0);
}

error_t set_timer_divide_value(uint8_t val)
{
	uint32_t new_value = 0;

	switch(val)
	{
		case 1:
			new_value = 0xb;
			break;
		case 2:
			new_value = 0x0;
			break;
		case 4:
			new_value = 0x1;
			break;
		case 8:
			new_value = 0x2;
			break;
		case 16:
			new_value = 0x3;
			break;
		case 32:
			new_value = 0x8;
			break;
		case 64:
			new_value = 0x9;
			break;
		case 128:
			new_value = 0xa;
			break;
		default:
			return SYSTEM_ERR_INVALID_ARGS;
	}

	write_reg(LAPIC_REG_DIVIDE_CONF, new_value);
	return SYSTEM_OK;
}

uint64_t get_apic_freq()
{
	CpuidLeaf leaf = {};
	read_cpuid(&leaf, CPUID_TSC, 0);

	return leaf.values[2] / read_reg(LAPIC_REG_DIVIDE_CONF);
}

void timer_mask()
{
	disable_interrupts();
	reg_or(LAPIC_REG_LVT_TIMER, LVT_MASKED);
	enable_interrupts();
}

void timer_unmask()
{
	disable_interrupts();
	reg_and(LAPIC_REG_LVT_TIMER, ~LVT_MASKED);
	enable_interrupts();
}

void timer_stop()
{
	disable_interrupts();

	write_reg(LAPIC_REG_INIT_COUNT, 0);

	if(test_feature(FEATURE_TSC_DEADLINE))
	{
		write_msr(MSR_TSC_DEADLINE, 0);
	}
}

error_t timer_set_oneshot(uint32_t count, uint8_t divisor, bool masked)
{
	error_t status = SYSTEM_OK;

	uint32_t timer_config = LVT_VECTOR(INTERRUPT_APIC_TIMER) | LVT_TIMER_MODE_ONESHOT;

	if(masked)
	{
		timer_config |= LVT_MASKED;
	}

	disable_interrupts();

	status = set_timer_divide_value(divisor);

	if(status != SYSTEM_OK)
	{
		return status;
	}

	write_reg(LAPIC_REG_LVT_TIMER, timer_config);
	write_reg(LAPIC_REG_INIT_COUNT, count);

	enable_interrupts();

	return SYSTEM_OK;
}

void timer_set_tsc_deadline(uint64_t deadline)
{
	assert(test_feature(FEATURE_TSC_DEADLINE));
	write_msr(MSR_TSC_DEADLINE, deadline);
}

uint32_t timer_current_count()
{
	return read_reg(LAPIC_REG_CURRENT_COUNT);
}

void pmi_mask()
{
	disable_interrupts();
	reg_or(LAPIC_REG_LVT_PERF, LVT_MASKED);
	enable_interrupts();
}

void pmi_unmask()
{
	disable_interrupts();
	reg_and(LAPIC_REG_LVT_PERF, ~LVT_MASKED);
	enable_interrupts();
}

void set_timer(uint8_t vector, size_t ticks, TimerModes mode)
{
	size_t timer_config = read_reg(LAPIC_REG_LVT_TIMER);
	timer_config &=
		~(LVT_TIMER_MODE_ONESHOT | LVT_TIMER_MODE_TSC_DEADLINE | LVT_TIMER_MODE_PERIODIC);

	switch(mode)
	{
		case TIMER_ONESHOT:
			timer_config |= LVT_TIMER_MODE_ONESHOT;
			break;
		case TIMER_PERIODIC:
			timer_config |= LVT_TIMER_MODE_PERIODIC;
			break;
		case TIMER_TSC_DEADLINE:
			timer_config |= LVT_TIMER_MODE_TSC_DEADLINE;
			break;
	}

	timer_config &= 0xffffff00;
	timer_config |= LVT_VECTOR(vector);

	write_reg(LAPIC_REG_LVT_TIMER, timer_config);
	write_reg(LAPIC_REG_INIT_COUNT, ticks ? ticks : 1);
	reg_and(LAPIC_REG_LVT_TIMER, ~LVT_MASKED);
}
} // namespace apic
} // namespace cpu