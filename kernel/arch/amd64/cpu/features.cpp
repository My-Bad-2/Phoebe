#include <cpu/features.h>

error_t read_cpuid(struct CpuidLeaf* leaf, const uint32_t leaf_num, const uint32_t subleaf_num)
{
	static uint32_t cpuid_max = 0;

	if(cpuid_max == 0)
	{
		asm volatile("cpuid" : "=a"(cpuid_max) : "a"(leaf_num & 0x80000000) : "ebx", "ecx", "edx");
	}

	if(leaf_num > cpuid_max)
	{
		*leaf = {};
		return SYSTEM_ERR_INVALID_ARGS;
	}

	asm volatile("cpuid"
				 : "=a"(leaf->values[0]), "=b"(leaf->values[1]), "=c"(leaf->values[2]),
				   "=d"(leaf->values[3])
				 : "a"(leaf_num), "c"(subleaf_num));

	return SYSTEM_OK;
}

bool test_feature(const struct CpuidBit bit)
{
	if((bit.word > 3) || (bit.bit > 31))
	{
		return false;
	}

	struct CpuidLeaf leaf = {};
	error_t ret = read_cpuid(&leaf, bit.leaf, 0);

	if(ret != SYSTEM_OK)
	{
		return false;
	}

	return !!((1u << bit.bit) & leaf.values[bit.word]);
}