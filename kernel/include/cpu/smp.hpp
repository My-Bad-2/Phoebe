#ifndef CPU_SMP_HPP
#define CPU_SMP_HPP 1

#include <stdint.h>
#include <stddef.h>

namespace cpu
{
namespace smp
{
struct PlatformCpuData;

void initialize_bsp();
void initialize();

PlatformCpuData* get_cpu_data();
} // namespace smp
} // namespace cpu

#endif // CPU_SMP_HPP