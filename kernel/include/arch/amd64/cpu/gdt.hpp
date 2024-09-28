#ifndef CPU_GDT_HPP
#define CPU_GDT_HPP

#include <sys/defs.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <array>

#define NULL_SELECTOR 0x00
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define USER_DATA_SELECTOR 0x18
#define USER_CODE_SELECTOR 0x20
#define TSS_SELECTOR 0x28

namespace cpu
{
namespace gdt
{
struct GdtSegment
{
	uint16_t limit;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t limit_high : 4;
	uint8_t granularity : 4;
	uint8_t base_high;

	void create_entry(uint32_t __base, uint32_t __limit, uint8_t __granularity, uint8_t __access);

	uint64_t raw();

	inline void create_entry(uint8_t __granularity, uint8_t __access)
	{
		return create_entry(0, 0xffffffff, __granularity, __access);
	}
} __PACKED;

struct TssSegment
{
	uint16_t limit;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t flags_low;
	uint8_t flags_high;
	uint8_t base_high;
	uint32_t base_upper;
	uint32_t reserved;

	void create_entry(void* __tss);
} __PACKED;

struct Tss
{
	uint32_t reserved0;
	std::array<uint64_t, 3> rsp;
	uint64_t reserved1;
	std::array<uint64_t, 7> ist;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iopb_offset;
} __PACKED;

struct GdtTable
{
	std::array<GdtSegment, 5> table;
	TssSegment tss;

	GdtSegment& operator[](size_t __index)
	{
		return this->table[__index];
	}
} __PACKED;

static_assert(sizeof(GdtTable) != 0, "No GDT can have a size of 0 bytes.");
static_assert(sizeof(GdtTable) <= 0x10000, "Maximum number GDT Entries exceeded.");

struct GdtRegister
{
	uint16_t limit;
	uint64_t base;
} __PACKED;

error_t initialize();
} // namespace gdt
} // namespace cpu

#endif // CPU_GDT_HPP