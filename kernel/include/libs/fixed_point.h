#ifndef LIBS_FIXED_POINT_H
#define LIBS_FIXED_POINT_H 1

#include <stdint.h>

struct fp_32_64
{
	uint32_t l0; /* unshifted value */
	uint32_t l32; /* value shifted left 32 bits (or bit -1 to -32) */
	uint32_t l64; /* value shifted left 64 bits (or bit -33 to -64) */
};

static inline void fp_32_64_div_32_32(struct fp_32_64* result, uint32_t dividend, uint32_t divisor)
{
	uint64_t tmp;
	uint32_t rem;

	tmp = ((uint64_t)dividend << 32) / divisor;
	rem = (uint32_t)(((uint64_t)dividend << 32) % divisor);
	result->l0 = (uint32_t)(tmp >> 32);
	result->l32 = (uint32_t)tmp;
	tmp = ((uint64_t)rem << 32) / divisor;
	result->l64 = (uint32_t)tmp;
}

static inline uint64_t mul_u32_u32(uint32_t a, uint32_t b, int a_shift, int b_shift)
{
	uint64_t ret = (uint64_t)a * b;
	return ret;
}

static inline uint64_t u64_mul_u32_fp32_64(uint32_t a, struct fp_32_64 b)
{
	uint64_t tmp;
	uint64_t res_0;
	uint64_t res_l32;
	uint32_t res_l32_32;
	uint64_t ret;

	res_0 = mul_u32_u32(a, b.l0, 0, 0);
	tmp = mul_u32_u32(a, b.l32, 0, -32);
	res_0 += tmp >> 32;
	res_l32 = (uint32_t)tmp;
	res_l32 += mul_u32_u32(a, b.l64, 0, -64) >> 32; /* Improve rounding accuracy */
	res_0 += res_l32 >> 32;
	res_l32_32 = (uint32_t)res_l32;
	ret = res_0 + (res_l32_32 >> 31); /* Round to nearest integer */

	return ret;
}

static inline uint32_t u32_mul_u64_fp32_64(uint64_t a, struct fp_32_64 b)
{
	uint32_t a_r32 = (uint32_t)(a >> 32);
	uint32_t a_0 = (uint32_t)a;
	uint64_t res_l32;
	uint32_t ret;

	/* mul_u32_u32(a_r32, b.l0, 32, 0) does not affect result */
	res_l32 = mul_u32_u32(a_0, b.l0, 0, 0) << 32;
	res_l32 += mul_u32_u32(a_r32, b.l32, 32, -32) << 32;
	res_l32 += mul_u32_u32(a_0, b.l32, 0, -32);
	res_l32 += mul_u32_u32(a_r32, b.l64, 32, -64);
	res_l32 += mul_u32_u32(a_0, b.l64, 0, -64) >> 32; /* Improve rounding accuracy */
	ret = (uint32_t)((res_l32 >> 32) + ((uint32_t)res_l32 >> 31)); /* Round to nearest integer */

	return ret;
}

static inline uint64_t u64_mul_u64_fp32_64(uint64_t a, struct fp_32_64 b)
{
	uint32_t a_r32 = (uint32_t)(a >> 32);
	uint32_t a_0 = (uint32_t)a;
	uint64_t res_0;
	uint64_t res_l32;
	uint32_t res_l32_32;
	uint64_t tmp;
	uint64_t ret;

	tmp = mul_u32_u32(a_r32, b.l0, 32, 0);
	res_0 = tmp << 32;
	tmp = mul_u32_u32(a_0, b.l0, 0, 0);
	res_0 += tmp;
	tmp = mul_u32_u32(a_r32, b.l32, 32, -32);
	res_0 += tmp;
	tmp = mul_u32_u32(a_0, b.l32, 0, -32);
	res_0 += tmp >> 32;
	res_l32 = (uint32_t)tmp;
	tmp = mul_u32_u32(a_r32, b.l64, 32, -64);
	res_0 += tmp >> 32;
	res_l32 += (uint32_t)tmp;
	tmp = mul_u32_u32(a_0, b.l64, 0, -64); /* Improve rounding accuracy */
	res_l32 += tmp >> 32;
	res_0 += res_l32 >> 32;
	res_l32_32 = (uint32_t)(res_l32);
	ret = res_0 + (res_l32_32 >> 31); /* Round to nearest integer */

	return ret;
}

#endif // LIBS_FIXED_POINT_H