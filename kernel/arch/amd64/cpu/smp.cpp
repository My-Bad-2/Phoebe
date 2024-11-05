#include "cpu/fpu.hpp"
#include "memory/paging.hpp"
#include <cpu/gdt.hpp>
#include <cpu/idt.hpp>
#include <cpu/smp.hpp>
#include <cpu/arch_smp.hpp>
#include <cpu/cpu.hpp>
#include <cpu/lapic.hpp>
#include <libs/vector.hpp>

namespace cpu
{
namespace smp
{
extern std::vector<PlatformCpuData> cpu_datas;

PlatformCpuData* get_cpu_data()
{
	uintptr_t cpu_data;
	asm volatile("mov %%gs:0, %0" : "=r"(cpu_data));
	return &cpu_datas[cpu_data];
}

void initialize_base_cpu(limine_smp_info* cpu)
{
	PlatformCpuData* cpu_data = reinterpret_cast<PlatformCpuData*>(cpu->extra_argument);

	cpu_data->self = cpu_data;
	cpu_data->gdt = new gdt::GdtTable;
	cpu_data->idt = new interrupts::IdtTable;
	cpu_data->tss = new gdt::Tss;

	cpu_data->tss->initialize();

	gdt::initialize(cpu_data->gdt, cpu_data->tss);
	interrupts::initialize(cpu_data->idt);

	cpu::set_kernel_gs_base(cpu->extra_argument);
	cpu::set_gs_base(cpu->extra_argument);

	apic::initialize_lapic_virt();
	apic::initialize_lapic();
}

void initialize_cpu(limine_smp_info* cpu)
{
	PlatformCpuData* cpu_data = reinterpret_cast<PlatformCpuData*>(cpu->extra_argument);
	cpu_data->local_apic_id = get_apic_id(cpu);

	if(cpu_data->local_apic_id != apic::bsp_id())
	{
		cpu::enable_pat();
		memory::base_pagemap.load();

		cpu_data->self = cpu_data;
		cpu_data->gdt = new gdt::GdtTable;
		cpu_data->idt = new interrupts::IdtTable;
		cpu_data->tss = new gdt::Tss;

		cpu_data->tss->initialize();

		gdt::initialize(cpu_data->gdt, cpu_data->tss);
		interrupts::initialize(cpu_data->idt);

		cpu::set_kernel_gs_base(cpu->extra_argument);
		cpu::set_gs_base(cpu->extra_argument);
	}

	fpu::initialize_sse();
	swapgs();

	if(cpu_data->local_apic_id != apic::bsp_id())
	{
		apic::initialize_lapic();
	}
}
} // namespace smp
} // namespace cpu