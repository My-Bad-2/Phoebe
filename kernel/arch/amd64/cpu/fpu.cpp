#include "memory/memory.hpp"
#include <stdlib.h>
#include <logger.h>

#include <cpu/cpu.hpp>
#include <cpu/fpu.hpp>
#include <cpu/features.h>
#include <cpu/registers.h>

#include <libs/function.hpp>

#include <memory/virtual.hpp>

namespace cpu
{
namespace fpu
{
uint8_t* __ALIGNED(16) fpu_init_states = nullptr;

struct FpuFeatures
{
	bool with_fpu;
	bool with_sse;
	bool with_sse2;
	bool with_sse3;
	bool with_ssse3;
	bool with_sse4_1;
	bool with_sse4_2;
	bool with_fxsave;
	bool with_xsave;
	bool with_avx;
	bool with_avx2;

	bool with_avx512_f;
	bool with_avx512_cd;
	bool with_avx512_er;
	bool with_avx512_pf;

	bool with_avx512_vl;
	bool with_avx512_dq;
	bool with_avx512_bw;

	bool with_avx512_ifma;
	bool with_avx512_vbmi;

	bool with_avx512_4vnniw;
	bool with_avx512_4fmaps;

	bool with_avx512_vpopvntdq;

	bool with_avx512_vnni;
	bool with_avx512_vbmi2;
	bool with_avx512_bitalg;

	bool with_xsaveopt;
	bool with_xsavec;
	bool with_xsaves;

	size_t storage_size;
} fpu_features;

void* allocate_fpu_buffer()
{
	const size_t fpu_pages = memory::div_roundup(fpu_features.storage_size, PAGE_SIZE);
	return memory::virtual_allocate(fpu_pages);
}

void free_fpu_buffer(void* buffer)
{
	const size_t fpu_pages = memory::div_roundup(fpu_features.storage_size, PAGE_SIZE);
	memory::virtual_free(buffer, fpu_pages);
}

void initialize_sse()
{
	uintptr_t cr0 = read_cr0();

	// enable x87-fpu
	cr0 &= ~CR0_EM;
	// enable internal x87 fpu error reporting
	cr0 |= CR0_NE;
	// control interaction of WAIT/FWAIT instructions with TS flag
	cr0 |= CR0_MP;

	write_cr0(cr0);

	// Initialize x87-fpu
	uint16_t fpu_control_word = 0;

	asm volatile("finit");
	asm volatile("fstcw %0" : "=m"(fpu_control_word));

	// Mask all exceptions
	// From: http://www.website.masmforum.com/tutorials/fptute/
	fpu_control_word |= 0x3f;

	asm volatile("fldcw %0" ::"m"(fpu_control_word));

	// Initialize SSE
	uint32_t cr4 = read_cr4();

	// Support SIMD FPU Exceptions
	cr4 |= CR4_OSXMMEXPT;
	// Support fxsave and fxrstor instructions
	cr4 |= CR4_OSFXSR;

	write_cr4(cr4);

	// Control and status information for SSE
	uint32_t mxcsr = 0;
	asm volatile("stmxcsr %0" : "=m"(mxcsr));

	// Mask all exceptions
	mxcsr = (0x3f << 7);

	asm volatile("ldmxcsr %0" ::"m"(mxcsr));

	// XSAVE enabled by OS
	if(fpu_features.with_xsave)
	{
		// Enable xsave and processor extended states
		write_cr4(read_cr4() | CR4_OSXSAVE);
	}

	// Save FPU initial state.
	save(fpu_init_states);

	// Allows saving x87 task context upon a task switch only
	// after x87 instruction is used.
	write_cr0(read_cr0() | CR0_TS);
}

void initialize()
{
	fpu_features.with_fpu = test_feature(FEATURE_FPU);

	fpu_features.with_sse = test_feature(FEATURE_SSE);
	fpu_features.with_sse2 = test_feature(FEATURE_SSE2);
	fpu_features.with_sse3 = test_feature(FEATURE_SSE3);
	fpu_features.with_ssse3 = test_feature(FEATURE_SSSE3);
	fpu_features.with_sse4_1 = test_feature(FEATURE_SSE4_1);
	fpu_features.with_sse4_2 = test_feature(FEATURE_SSE4_2);

	fpu_features.with_fxsave = test_feature(FEATURE_FXSR);
	fpu_features.with_xsave = test_feature(FEATURE_XSAVE);

	fpu_features.with_avx = test_feature(FEATURE_AVX);
	fpu_features.with_avx2 = test_feature(FEATURE_AVX2);

	fpu_features.with_avx512_f = test_feature(FEATURE_AVX512F);
	fpu_features.with_avx512_cd = test_feature(FEATURE_AVX512CD);
	fpu_features.with_avx512_er = test_feature(FEATURE_AVX512ER);
	fpu_features.with_avx512_pf = test_feature(FEATURE_AVX512PF);

	fpu_features.with_avx512_vl = test_feature(FEATURE_AVX512VL);
	fpu_features.with_avx512_dq = test_feature(FEATURE_AVX512DQ);
	fpu_features.with_avx512_bw = test_feature(FEATURE_AVX512BW);

	fpu_features.with_avx512_ifma = test_feature(FEATURE_AVX512IFMA);
	fpu_features.with_avx512_vbmi = test_feature(FEATURE_AVX512VBMI);

	fpu_features.with_avx512_4vnniw = test_feature(FEATURE_AVX512QVNNIW);
	fpu_features.with_avx512_4fmaps = test_feature(FEATURE_AVX512QFMA);

	fpu_features.with_avx512_vpopvntdq = test_feature(FEATURE_AVX512VPDQ);
	fpu_features.with_avx512_vnni = test_feature(FEATURE_AVX512VNNI);
	fpu_features.with_avx512_vbmi2 = test_feature(FEATURE_AVX512VBMI2);
	fpu_features.with_avx512_bitalg = test_feature(FEATURE_AVX512BITALG);

	if(!(fpu_features.with_fpu || fpu_features.with_sse || fpu_features.with_fxsave))
	{
		return;
	}

	fpu_features.with_xsaveopt = false;
	fpu_features.with_xsavec = false;
	fpu_features.with_xsaves = false;

	if(fpu_features.with_xsave)
	{
		log_debug("XSAVE detected!");

		fpu_features.with_xsaveopt = test_feature(FEATURE_XSAVEOPT);
		fpu_features.with_xsavec = test_feature(FEATURE_XSAVEC);
		fpu_features.with_xsaves = test_feature(FEATURE_XSAVES);

		CpuidLeaf leaf = {};

		read_cpuid(&leaf, CPUID_XSAVE, 0);

		log_debug("XSAVE size = %u", leaf.values[2]);

		fpu_features.storage_size = leaf.values[2];
	}
	else
	{
		fpu_features.storage_size = 512;
	}

	fpu_init_states = static_cast<uint8_t*>(calloc(fpu_features.storage_size, sizeof(uint8_t)));

	log_begin_intialization("Streaming SIMD Extensions");

	initialize_sse();

	log_end_intialization();
}

void save(uint8_t* region)
{
	if(fpu_features.with_xsave)
	{
		if(fpu_features.with_xsaveopt)
		{
			xsaveopt(region);
		}
		else
		{
			xsave(region);
		}
	}
	else
	{
		fxsave(region);
	}
}
} // namespace fpu
} // namespace cpu