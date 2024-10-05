#include <logger.h>

#include <cpu/gdt.hpp>

#define GDT_ENTRY_COUNT 5
#define GDT_ACCESSED (1 << 0)
#define GDT_READWRITE (1 << 1)
#define GDT_CONFORMING (1 << 2)
#define GDT_EXECUTABLE (1 << 3)
#define GDT_SEGMENT (1 << 4)
#define GDT_USER (3 << 5)
#define GDT_PRESENT (1 << 7)

#define GDT_LONG_MODE_GRANULARITY (1 << 1)
#define GDT_DB (1 << 2)
#define GDT_GRANULARITY (1 << 3)

#define GDT_KERNEL_CODE (1)
#define GDT_KERNEL_DATA (2)
#define GDT_USER_DATA (3)
#define GDT_USER_CODE (4)

#define GDT_RING_3 (3)

#define GDT_DATA_SEGMENT (GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE)
#define GDT_CODE_SEGMENT (GDT_DATA_SEGMENT | GDT_EXECUTABLE)

extern "C" void load_gdt(cpu::gdt::GdtRegister*);
extern "C" void load_tss();

namespace cpu
{
namespace gdt
{
GdtTable gdt_table = {};
Tss tss = {};

void GdtSegment::create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t access)
{
	this->limit = static_cast<uint16_t>(limit & 0xffff);
	this->base_low = static_cast<uint16_t>(base & 0xffff);
	this->base_mid = static_cast<uint16_t>((base >> 16) & 0xff);
	this->access = access;
	this->limit_high = (limit >> 16) & 0x0f;
	this->granularity = granularity;
	this->base_high = static_cast<uint8_t>((base >> 24) & 0xff);
}

void TssSegment::create_entry(void* tss)
{
	const uintptr_t base = reinterpret_cast<uintptr_t>(tss);

	this->limit = static_cast<uint16_t>(sizeof(Tss) - 1);
	this->base_low = static_cast<uint16_t>(base & 0xffff);
	this->base_mid = static_cast<uint8_t>((base >> 16) & 0xff);
	this->flags_low = 0x89;
	this->flags_high = 0x00;
	this->base_high = static_cast<uint8_t>((base >> 24) & 0xff);
	this->base_upper = static_cast<uint32_t>((base >> 32) & 0xffffffff);
	this->reserved = 0;
}

error_t initialize()
{
	log_begin_intialization("Global Descriptor Table");

	gdt_table[0].create_entry(0, 0, 0, 0);
	gdt_table[GDT_KERNEL_CODE].create_entry(GDT_LONG_MODE_GRANULARITY | GDT_GRANULARITY,
											GDT_CODE_SEGMENT);
	gdt_table[GDT_KERNEL_DATA].create_entry(GDT_DB | GDT_GRANULARITY, GDT_DATA_SEGMENT);
	gdt_table[GDT_USER_DATA].create_entry(GDT_DB | GDT_GRANULARITY, GDT_DATA_SEGMENT | GDT_USER);
	gdt_table[GDT_USER_CODE].create_entry(GDT_LONG_MODE_GRANULARITY | GDT_GRANULARITY,
										  GDT_CODE_SEGMENT | GDT_USER);

	gdt_table.tss.create_entry(&tss);

	GdtRegister gdtr = {
		sizeof(GdtTable) - 1,
		reinterpret_cast<uintptr_t>(&gdt_table),
	};

	load_gdt(&gdtr);
	load_tss();

	log_end_intialization();

	return SYSTEM_OK;
}
} // namespace gdt
} // namespace cpu