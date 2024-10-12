#ifndef CPU_PIC_HPP
#define CPU_PIC_HPP 1

#include <stdint.h>

namespace cpu
{
namespace interrupts
{
void pic_send_eoi(uint8_t __irq);
void pic_set_mask(uint8_t __irq);
void pic_clear_mask(uint8_t __irq);

void pic_initialize(int __offset1, int __offset2);
void pic_disable();

uint16_t pic_get_interrupt_req_reg();
uint16_t pic_get_in_service_reg();
} // namespace interrupts
} // namespace cpu

#endif // CPU_PIC_HPP