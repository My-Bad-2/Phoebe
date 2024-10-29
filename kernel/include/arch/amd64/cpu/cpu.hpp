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
/**
 * @brief Invalidates a specific page in the TLB (Translation Lookaside Buffer).
 *
 * @param address The virtual address of the page to invalidate.
 */
inline void invalidate_page(uintptr_t address)
{
	asm volatile("invlpg (%0)" ::"r"(address));
}

/**
 * @brief Reads the value of control register CR0.
 *
 * @return The current value of CR0.
 */
inline uint64_t read_cr0()
{
	uint64_t value = 0;
	asm volatile("mov %%cr0, %0" : "=r"(value)::"memory");
	return value;
}

/**
 * @brief Reads the value of control register CR2, which stores the page-fault linear address.
 *
 * @return The current value of CR2.
 */
inline uint64_t read_cr2()
{
	uint64_t value = 0;
	asm volatile("mov %%cr2, %0" : "=r"(value)::"memory");
	return value;
}

/**
 * @brief Reads the value of control register CR3, which stores the page directory base.
 *
 * @return The current value of CR3.
 */
inline uint64_t read_cr3()
{
	uint64_t value = 0;
	asm volatile("mov %%cr3, %0" : "=r"(value)::"memory");
	return value;
}

/**
 * @brief Reads the value of control register CR4.
 *
 * @return The current value of CR4.
 */
inline uint64_t read_cr4()
{
	uint64_t value = 0;
	asm volatile("mov %%cr4, %0" : "=r"(value)::"memory");
	return value;
}

/**
 * @brief Writes a value to control register CR0.
 *
 * @param value The value to write to CR0.
 */
inline void write_cr0(uint64_t value)
{
	asm volatile("mov %0, %%cr0" ::"r"(value) : "memory");
}

/**
 * @brief Writes a value to control register CR2.
 *
 * @param value The value to write to CR2.
 */
inline void write_cr2(uint64_t value)
{
	asm volatile("mov %0, %%cr2" ::"r"(value) : "memory");
}

/**
 * @brief Writes a value to control register CR3.
 *
 * @param value The value to write to CR3.
 */
inline void write_cr3(uint64_t value)
{
	asm volatile("mov %0, %%cr3" ::"r"(value) : "memory");
}

/**
 * @brief Writes a value to control register CR4.
 *
 * @param value The value to write to CR4.
 */
inline void write_cr4(uint64_t value)
{
	asm volatile("mov %0, %%cr4" ::"r"(value) : "memory");
}

/**
 * @brief Reads the value of the specified Model-Specific Register (MSR).
 *
 * @param msr The MSR identifier to read.
 * @return The current value of the specified MSR.
 */
inline uint64_t read_msr(uint32_t msr)
{
	uint32_t edx, eax;
	asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr) : "memory");
	return (static_cast<uint64_t>(edx) << 32) | eax;
}

/**
 * @brief Writes a value to the specified Model-Specific Register (MSR).
 *
 * @param msr The MSR identifier to write to.
 * @param value The value to write into the MSR.
 */
inline void write_msr(uint32_t msr, uint64_t value)
{
	uint32_t edx = value >> 32;
	uint32_t eax = static_cast<uint32_t>(value);
	asm volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr) : "memory");
}

/**
 * @brief Enables Page Attribute Table (PAT) by writing default PAT values to the MSR_PAT register.
 */
inline void enable_pat()
{
	write_msr(MSR_PAT, DEFAULT_PAT);
}

/**
 * @brief Saves the FPU state into the specified memory region.
 *
 * @param region Pointer to a memory region to store the FPU state.
 */
inline void fxsave(uint8_t* region)
{
	asm volatile("fxsaveq (%0)" ::"r"(region) : "memory");
}

/**
 * @brief Saves the extended processor state to the specified memory region.
 *
 * @param region Pointer to a memory region to store the extended processor state.
 */
inline void xsave(uint8_t* region)
{
	asm volatile("xsaveq (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

/**
 * @brief Optimized save of the extended processor state to the specified memory region.
 *
 * @param region Pointer to a memory region to store the extended processor state.
 */
inline void xsaveopt(uint8_t* region)
{
	asm volatile("xsaveopt64 (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

/**
 * @brief Restores the extended processor state from the specified memory region.
 *
 * @param region Pointer to a memory region containing the saved extended processor state.
 */
inline void xrstor(uint8_t* region)
{
	asm volatile("xrstorq (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

/**
 * @brief Restores the FPU state from the specified memory region.
 *
 * @param region Pointer to a memory region containing the saved FPU state.
 */
inline void fxrstor(uint8_t* region)
{
	asm volatile("fxrstorq (%0)" ::"r"(region), "a"(RFBM_LOW), "d"(RFBM_HIGH) : "memory");
}

/**
 * @brief Reads the base address of the kernel GS segment.
 *
 * @return The current kernel GS base address.
 */
inline uintptr_t kernel_gs_base()
{
	return read_msr(MSR_KERNEL_GS_BASE);
}

/**
 * @brief Reads the base address of the FS segment.
 *
 * @return The current FS base address.
 */
inline uintptr_t fs_base()
{
	return read_msr(MSR_FS_BASE);
}

/**
 * @brief Reads the base address of the GS segment.
 *
 * @return The current GS base address.
 */
inline uintptr_t gs_base()
{
	return read_msr(MSR_GS_BASE);
}

/**
 * @brief Sets the base address of the FS segment.
 *
 * @param addr The new base address for the FS segment.
 */
inline void set_fs_base(uintptr_t addr)
{
	write_msr(MSR_FS_BASE, addr);
}

/**
 * @brief Sets the base address of the GS segment.
 *
 * @param addr The new base address for the GS segment.
 */
inline void set_gs_base(uintptr_t addr)
{
	write_msr(MSR_GS_BASE, addr);
}

/**
 * @brief Sets the base address of the kernel GS segment.
 *
 * @param addr The new base address for the kernel GS segment.
 */
inline void set_kernel_gs_base(uintptr_t addr)
{
	write_msr(MSR_KERNEL_GS_BASE, addr);
}

void initialize();
void late_initialize();
} // namespace cpu

#endif // CPU_CPU_HPP