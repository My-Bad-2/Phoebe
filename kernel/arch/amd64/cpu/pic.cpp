#include <cpu/pic.hpp>
#include <arch.hpp>

#define PIC1 0x20
#define PIC2 0xa0

#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)

#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20

#define ICW1_ICW4 0x01 /* Indicates that ICW4 will be present */
#define ICW1_SINGLE 0x02 /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08 /* Level triggered (edge) mode */
#define ICW1_INIT 0x10 /* Initialization - required! */

#define ICW4_8086 0x01 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10 /* Special fully nested (not) */

#define PIC_READ_IRR 0x0a /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b /* OCW3 irq service next CMD read */

namespace cpu
{
namespace interrupts
{
inline uint16_t pic_get_irq_reg(uint8_t ocw3)
{
	arch::outp(PIC1_COMMAND, ocw3);
	arch::outp(PIC2_COMMAND, ocw3);

	return (arch::inp<uint8_t>(PIC2_COMMAND) << 8) | arch::inp<uint8_t>(PIC1_COMMAND);
}

void pic_send_eoi(uint8_t irq)
{
	if(irq >= 8)
	{
		arch::outp<uint8_t>(PIC2_COMMAND, PIC_EOI);
	}

	arch::outp<uint8_t>(PIC1_COMMAND, PIC_EOI);
}

void pic_set_mask(uint8_t irq)
{
	uint16_t port = PIC1_DATA;
	uint8_t value = 0;

	if(irq >= 8)
	{
		port = PIC2_DATA;
		irq -= 8;
	}

	value = arch::inp<uint8_t>(port) | (1 << irq);
	arch::outp(port, value);
}

void pic_clear_mask(uint8_t irq)
{
	uint16_t port = PIC1_DATA;
	uint8_t value = 0;

	if(irq >= 8)
	{
		port = PIC2_DATA;
		irq -= 8;
	}

	value = arch::inp<uint8_t>(port) & ~(1 << irq);
	arch::outp(port, value);
}

void pic_initialize(int offset1, int offset2)
{
	uint8_t mask1 = arch::inp<uint8_t>(PIC1_DATA);
	uint8_t mask2 = arch::inp<uint8_t>(PIC2_DATA);

	arch::outp<uint8_t>(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	arch::io_wait();
	arch::outp<uint8_t>(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	arch::io_wait();

	arch::outp<uint8_t>(PIC1_DATA, offset1);
	arch::io_wait();
	arch::outp<uint8_t>(PIC2_DATA, offset2);
	arch::io_wait();

	arch::outp<uint8_t>(PIC1_DATA, 4);
	arch::io_wait();
	arch::outp<uint8_t>(PIC2_DATA, 2);
	arch::io_wait();

	arch::outp<uint8_t>(PIC1_DATA, ICW4_8086);
	arch::io_wait();
	arch::outp<uint8_t>(PIC2_DATA, ICW4_8086);
	arch::io_wait();

	arch::outp<uint8_t>(PIC1_DATA, mask1);
	arch::outp<uint8_t>(PIC2_DATA, mask2);
}

void pic_disable()
{
	arch::outp<uint8_t>(PIC1_DATA, 0xff);
	arch::outp<uint8_t>(PIC2_DATA, 0xff);
}

uint16_t pic_get_interrupt_req_reg()
{
	return pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_in_service_reg()
{
	return pic_get_irq_reg(PIC_READ_ISR);
}
} // namespace interrupts
} // namespace cpu