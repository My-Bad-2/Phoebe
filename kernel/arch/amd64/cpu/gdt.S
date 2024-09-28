#include <libs/asm.h>

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define TSS_SELECTOR 0x28

.section .text

.function load_gdt, scope=global, align=64
    lgdtq (%rdi)
    movw $KERNEL_DATA_SELECTOR, %ax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    movl %eax, %ss
    lea 1f(%rip), %rax
    push_value $KERNEL_CODE_SELECTOR
    push_reg %rax
    lretq
1:
    retq
.end_function

.function load_tss, scope=global
    movw $TSS_SELECTOR, %ax
    ltrw %ax
    retq
.end_function