#include <cpu/smp.hpp>
#include <cpu/arch_smp.hpp>
#include <libs/vector.hpp>
#include "arch.hpp"
#include "kernel.h"
#include "lock.hpp"
#include "logger.h"

namespace cpu
{
namespace smp
{
PlatformCpuData* cpu_datas = nullptr;

void cpu_entry(limine_smp_info* cpu)
{
	lock::mutex lock;

	{
		lock::ScopedLock guard(lock);
		initialize_cpu(cpu);

		log_debug("CPU %lu is up.", get_cpu_data()->id);
		get_cpu_data()->is_up = true;
	}

	if(get_cpu_data()->local_apic_id != smp_request.response->bsp_lapic_id)
	{
		log_debug("Hello");
	}
}

void initialize_bsp()
{
	cpu_datas = new PlatformCpuData[smp_request.response->cpu_count];

	for(size_t i = 0; i < smp_request.response->cpu_count; i++)
	{
		limine_smp_info* smp_info = smp_request.response->cpus[i];

		if(get_apic_id(smp_info) != smp_request.response->bsp_lapic_id)
		{
			continue;
		}

		cpu_datas[i].local_apic_id = get_apic_id(smp_info);
		cpu_datas[i].id = i;
		smp_info->extra_argument = reinterpret_cast<uintptr_t>(&cpu_datas[i]);

		initialize_base_cpu(smp_info);
	}
}

void initialize()
{
	for(size_t i = 0; i < smp_request.response->cpu_count; i++)
	{
		limine_smp_info* smp_info = smp_request.response->cpus[i];
		smp_info->extra_argument = reinterpret_cast<uintptr_t>(&cpu_datas[i]);
		cpu_datas[i].id = i;

		if(cpu_datas[i].local_apic_id != smp_request.response->bsp_lapic_id)
		{
			smp_info->goto_address = cpu_entry;

			while(!cpu_datas[i].is_up)
			{
				arch::halt();
			}
		}
		else
		{
			cpu_entry(smp_info);
		}
	}
}
} // namespace smp
} // namespace cpu