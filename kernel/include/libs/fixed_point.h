#ifndef LIBS_FIXED_POINT_H
#define LIBS_FIXED_POINT_H 1

#include <stdint.h>

/**
 * @brief Structure representing a fixed-point number with components
 * shifted by different amounts.
 *
 * This structure holds three 32-bit components of a fixed-point number:
 * - `l0`: Unshifted value.
 * - `l32`: Value shifted left by 32 bits (representing bits -1 to -32).
 * - `l64`: Value shifted left by 64 bits (representing bits -33 to -64).
 */
struct fp_32_64
{
	uint32_t l0; //< Unshifted value.
	uint32_t l32; //< Value shifted left by 32 bits.
	uint32_t l64; //< Value shifted left by 64 bits.
};

/**
 * @brief Divides a 32-bit dividend by a 32-bit divisor and stores the result in fixed-point form.
 *
 * This function computes the division of `dividend` by `divisor`, storing the result across
 * the fields of `result`. The quotient is divided into three parts:
 * - `l0` holds the high 32 bits of the quotient.
 * - `l32` holds the low 32 bits of the quotient.
 * - `l64` holds a finer result based on the remainder of the first division.
 *
 * @param result Pointer to the `fp_32_64` structure where the result will be stored.
 * @param dividend The 32-bit dividend.
 * @param divisor The 32-bit divisor.
 */
static inline void fp_32_64_div_32_32(struct fp_32_64* result, uint32_t dividend, uint32_t divisor)
{
	uint64_t tmp;
	uint32_t rem;

	// Perform division with the dividend shifted left by 32 bits
	tmp = ((uint64_t)dividend << 32) / divisor;
	rem = (uint32_t)(((uint64_t)dividend << 32) % divisor);

	// Store the result in the `fp_32_64` structure
	result->l0 = (uint32_t)(tmp >> 32); // High 32 bits of the quotient
	result->l32 = (uint32_t)tmp; // Low 32 bits of the quotient

	// Calculate `l64` for finer precision based on the remainder
	tmp = ((uint64_t)rem << 32) / divisor;
	result->l64 = (uint32_t)tmp;
}

/**
 * @brief Multiplies two 32-bit unsigned integers with optional bit shifts.
 *
 * This function multiplies two 32-bit values `a` and `b`. The `a_shift` and `b_shift`
 * parameters provide optional bit shifts to be applied to `a` and `b`, respectively.
 *
 * @param a The first 32-bit unsigned integer.
 * @param b The second 32-bit unsigned integer.
 * @param a_shift Bit shift applied to `a` before multiplication.
 * @param b_shift Bit shift applied to `b` before multiplication.
 * @return The 64-bit product of the (possibly shifted) values.
 */
static inline uint64_t mul_u32_u32(uint32_t a, uint32_t b, int a_shift, int b_shift)
{
	uint64_t ret = (uint64_t)a * b; // Multiply without shifts as shifts are not currently applied
	return ret;
}

/**
 * @brief Multiplies a 32-bit integer by a fixed-point number.
 *
 * This function multiplies a 32-bit integer `a` by a fixed-point `fp_32_64` structure `b`
 * to compute an approximate 64-bit result.
 *
 * @param a The 32-bit integer.
 * @param b The fixed-point structure.
 * @return The 64-bit product.
 */
static inline uint64_t u64_mul_u32_fp32_64(uint32_t a, struct fp_32_64 b)
{
	uint64_t tmp;
	uint64_t res_0;
	uint64_t res_l32;
	uint32_t res_l32_32;
	uint64_t ret;

	res_0 = mul_u32_u32(a, b.l0, 0, 0);
	tmp = mul_u32_u32(a, b.l32, 0, -32);
	res_0 += tmp >> 32; // Accumulate high part of tmp
	res_l32 = (uint32_t)tmp;
	res_l32 += mul_u32_u32(a, b.l64, 0, -64) >> 32; // Improve rounding accuracy
	res_0 += res_l32 >> 32;
	res_l32_32 = (uint32_t)res_l32;
	ret = res_0 + (res_l32_32 >> 31); // Round to the nearest integer

	return ret;
}

/**
 * @brief Multiplies a 64-bit integer by a fixed-point number, returning a 32-bit result.
 *
 * This function multiplies a 64-bit integer `a` by a fixed-point `fp_32_64` structure `b`
 * and computes a 32-bit approximate result.
 *
 * @param a The 64-bit integer.
 * @param b The fixed-point structure.
 * @return The 32-bit product.
 */
static inline uint32_t u32_mul_u64_fp32_64(uint64_t a, struct fp_32_64 b)
{
	uint32_t a_r32 = (uint32_t)(a >> 32); // High 32 bits of `a`
	uint32_t a_0 = (uint32_t)a; // Low 32 bits of `a`
	uint64_t res_l32;
	uint32_t ret;

	res_l32 = mul_u32_u32(a_0, b.l0, 0, 0) << 32;
	res_l32 += mul_u32_u32(a_r32, b.l32, 32, -32) << 32;
	res_l32 += mul_u32_u32(a_0, b.l32, 0, -32);
	res_l32 += mul_u32_u32(a_r32, b.l64, 32, -64);
	res_l32 += mul_u32_u32(a_0, b.l64, 0, -64) >> 32; // Improve rounding accuracy
	ret = (uint32_t)((res_l32 >> 32) + ((uint32_t)res_l32 >> 31)); // Round to nearest integer

	return ret;
}

/**
 * @brief Multiplies a 64-bit integer by a fixed-point number, returning a 64-bit result.
 *
 * This function multiplies a 64-bit integer `a` by a fixed-point `fp_32_64` structure `b`
 * to compute an approximate 64-bit result.
 *
 * @param a The 64-bit integer.
 * @param b The fixed-point structure.
 * @return The 64-bit product.
 */
static inline uint64_t u64_mul_u64_fp32_64(uint64_t a, struct fp_32_64 b)
{
	uint32_t a_r32 = (uint32_t)(a >> 32); // High 32 bits of `a`
	uint32_t a_0 = (uint32_t)a; // Low 32 bits of `a`
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
	tmp = mul_u32_u32(a_0, b.l64, 0, -64); // Improve rounding accuracy
	res_l32 += tmp >> 32;
	res_0 += res_l32 >> 32;
	res_l32_32 = (uint32_t)(res_l32);
	ret = res_0 + (res_l32_32 >> 31); // Round to the nearest integer

	return ret;
}

#endif // LIBS_FIXED_POINT_H