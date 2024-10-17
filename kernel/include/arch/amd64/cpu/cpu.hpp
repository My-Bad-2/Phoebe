#ifndef CPU_CPU_HPP
#define CPU_CPU_HPP 1

#include <stdint.h>
#include <cpu/registers.h>

#define PAT_FORCE_UNCACHABLE 0x0ul
#define PAT_WRITE_COMBINING 0x1ul
#define PAT_WRITE_THROUGH 0x4ul
#define PAT_WRITE_PROTECT 0x5ul
#define PAT_WRITE_BACK 0x6ul
#define PAT_UNCACHABLE 0x7ul

#define DEFAULT_PAT                                                                \
	((PAT_UNCACHABLE << 56) | (PAT_WRITE_BACK << 48) | (PAT_WRITE_PROTECT << 40) | \
	 (PAT_WRITE_THROUGH << 32) | (PAT_WRITE_COMBINING << 24) | (PAT_FORCE_UNCACHABLE << 16))

#define RFBM (static_cast<uint64_t>(-1))
#define RFBM_LOW (static_cast<uint32_t>(RFBM))
#define RFBM_HIGH (static_cast<uint32_t>(RFBM >> 32))

namespace cpu
{
inline void invalidate_page(uintptr_t address)
{
	asm volatile("invlpg (%0)" ::"r"(address));
}

inline uint64_t read_cr0()
{
	uint64_t value = 0;
	asm volatile("mov %%cr0, %0" : "=r"(value)::"memory");
	return value;
}

inline uint64_t read_cr2()
{
	uint64_t value = 0;
	asm volatile("mov %%cr2, %0" : "=r"(value)::"memory");
	return value;
}

inline uint64_t read_cr3()
{
	uint64_t value = 0;
	asm volatile("mov %%cr3, %0" : "=r"(value)::"memory");
	return value;
}

inline uint64_t read_cr4()
{
	uint64_t value = 0;
	asm volatile("mov %%cr4, %0" : "=r"(value)::"memory");
	return value;
}

inline void write_cr0(uint64_t value)
{
	asm volatile("mov %0, %%cr0" ::"r"(value) : "memory");
}

inline void write_cr2(uint64_t value)
{
	asm volatile("mov %0, %%cr2" ::"r"(value) : "memory");
}

inline void write_cr3(uint64_t value)
{
	asm volatile("mov %0, %%cr3" ::"r"(value) : "memory");
}

inline void write_cr4(uint64_t value)
{
	asm volatile("mov %0, %%cr4" ::"r"(value) : "memory");
}

inline uint64_t read_msr(uint32_t msr)
{
	uint32_t edx, eax;
	asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr) : "memory");
	return (static_cast<uint64_t>(edx) << 32) | eax;
}

inline void write_msr(uint32_t msr, uint64_t value)
{
	uint32_t edx = value >> 32;
	uint32_t eax = static_cast<uint32_t>(value);
	asm volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr) : "memory");
}

inline void enable_pat()
{
	write_msr(MSR_PAT, DEFAULT_PAT);
}

inline void fxsave(uint8_t* region)
{
	asm volatile("fxsaveq (%0)" ::"r"(region) : "memory");
}

inline void xsave(uint8_t* region)
{
	asm volatile("xsaveq (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

inline void xsaveopt(uint8_t* region)
{
	asm volatile("xsaveopt64 (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

inline void xrstor(uint8_t* region)
{
	asm volatile("xrstorq (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

inline void fxrstor(uint8_t* region)
{
	asm volatile("fxrstorq (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

void initialize();
} // namespace cpu

#endif // CPU_CPU_HPP