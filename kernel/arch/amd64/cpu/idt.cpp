#include <libs/trace.h>
#include <cpu/idt.hpp>
#include <cpu/gdt.hpp>
#include <cpu/registers.h>
#include <logger.h>

#define TYPE_ATTRIBUTE_PRESENT (1 << 7)
#define TYPE_ATTRIBUTE_DPL(x) (x << 5)

extern "C" void* isr_table[];
extern "C" void load_idt(cpu::interrupts::IdtRegister*);

namespace cpu
{
namespace interrupts
{
IdtTable idt_table = {};

inline uint8_t idt_attribute(uint8_t type, uint8_t dpl)
{
	uint8_t ret = TYPE_ATTRIBUTE_PRESENT;
	ret |= TYPE_ATTRIBUTE_DPL(dpl);
	ret |= type;

	return ret;
}

void IdtSegment::create_entry(void* handler, uint8_t ist, uint8_t type, uint8_t dpl,
							  uint16_t selector)
{
	const uintptr_t base = reinterpret_cast<uintptr_t>(handler);

	this->base_low = static_cast<uint16_t>(base & 0xffff);
	this->selector = selector;
	this->ist = ist;
	this->type_attributes = idt_attribute(type, dpl);
	this->base_mid = static_cast<uint16_t>((base >> 16) & 0xffff);
	this->base_high = static_cast<uint32_t>((base >> 32) & 0xffffffff);
	this->reserved = 0;
}

void dump_interrupt_frame(Iframe* iframe)
{
	log_panic("CS : 0x%.16lx RIP: 0x%.16lx EFL: 0x%.16lx", iframe->cs, iframe->ip, iframe->flags);
	log_panic("RAX: 0x%.16lx RBX: 0x%.16lx RCX: 0x%.16lx", iframe->rax, iframe->rbx, iframe->rcx);
	log_panic("RDX: 0x%.16lx RSI: 0x%.16lx RBP: 0x%.16lx", iframe->rdx, iframe->rsi, iframe->rbp);
	log_panic("RSP: 0x%.16lx R8 : 0x%.16lx R9 : 0x%.16lx", iframe->rsi, iframe->r8, iframe->r9);
	log_panic("R10: 0x%.16lx R11: 0x%.16lx R12: 0x%.16lx", iframe->r10, iframe->r11, iframe->r12);
	log_panic("R13: 0x%.16lx R14: 0x%.16lx R15: 0x%.16lx", iframe->r13, iframe->r14, iframe->r15);
	log_panic("EC : 0x%.16lx USP: 0x%.16lx USS: 0x%.16lx", iframe->err_code, iframe->user_sp,
			  iframe->user_ss);
}

void exception_handler(Iframe* iframe)
{
	dump_stacktrace();

	dump_interrupt_frame(iframe);
	log_panik("Unhandled Exception 0x%lx!", iframe->vector);
}

error_t initialize()
{
	uint8_t type = IDT_INTERRUPT_GATE;
	uint8_t dpl = IDT_DPL0;

	log_begin_intialization("Interrupt Descriptor Table");

	for(int vector = 0; vector < MAX_IDT_ENTRIES; vector++)
	{
		switch(vector)
		{
			case EXCEPTION_BREAKPOINT:
				dpl = IDT_DPL3;
			default:
				dpl = IDT_DPL0;
		}

		idt_table[vector].create_entry(isr_table[vector], 0, type, dpl, KERNEL_CODE_SELECTOR);
	}

	IdtRegister idtr = {
		sizeof(IdtTable) - 1,
		reinterpret_cast<uint64_t>(&idt_table),
	};

	load_idt(&idtr);

	log_end_intialization();

	return SYSTEM_OK;
}
} // namespace interrupts
} // namespace cpu

extern "C"
{
	void exception_handler(Iframe* iframe)
	{
		if(iframe->vector < 0x20)
		{
			cpu::interrupts::exception_handler(iframe);
		}

		log_panik("Interrupt 0x%.16lx triggered!", iframe->vector);
	}

	void nmi_handler(Iframe* iframe)
	{
		log_panik("nmi_handler()  called!");
	}
}