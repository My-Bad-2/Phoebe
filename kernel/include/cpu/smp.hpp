#ifndef CPU_SMP_HPP
#define CPU_SMP_HPP 1

#include <stdint.h>
#include <stddef.h>

namespace cpu
{
namespace smp
{
void initialize_bsp();
void initialize();
} // namespace smp
} // namespace cpu

#endif // CPU_SMP_HPP