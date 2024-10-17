#ifndef CPU_FPU_HPP
#define CPU_FPU_HPP 1

#include <stdint.h>

namespace cpu
{
namespace fpu
{
void initialize();

void save(uint8_t* __region);
} // namespace fpu
} // namespace cpu

#endif // CPU_FPU_HPP