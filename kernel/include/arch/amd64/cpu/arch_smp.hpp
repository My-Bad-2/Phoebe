#ifndef CPU_ARCH_SMP_HPP
#define CPU_ARCH_SMP_HPP 1

#include <stdint.h>
#include <stddef.h>

#include <cpu/gdt.hpp>
#include <cpu/idt.hpp>

#include <kernel.h>

namespace cpu
{
namespace smp
{
struct ApicContext
{
	size_t apic_ticks_per_ms = 0;
};

struct PlatformCpuData
{
	size_t id;
	PlatformCpuData* self;
	int local_apic_id;

	gdt::GdtTable* gdt;
	interrupts::IdtTable* idt;
	gdt::Tss* tss;

	void* irq_stack;
	void* nmi_stack;

	bool is_up;
};

PlatformCpuData* get_cpu_data();

void initialize_base_cpu(limine_smp_info* cpu);
void initialize_cpu(limine_smp_info* cpu);

constexpr size_t get_apic_id(limine_smp_info* cpu)
{
	return cpu->lapic_id;
}
} // namespace smp
} // namespace cpu

#endif // CPU_ARCH_SMP_HPP