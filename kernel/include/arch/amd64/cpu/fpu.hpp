#ifndef CPU_FPU_HPP
#define CPU_FPU_HPP 1

#include <stdint.h>
#include <stddef.h>

namespace cpu
{
namespace fpu
{
void initialize();
void* allocate_fpu_buffer();
void free_fpu_buffer(void* __buffer);

void save(uint8_t* __region);
} // namespace fpu
} // namespace cpu

#endif // CPU_FPU_HPP