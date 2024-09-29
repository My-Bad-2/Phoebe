#include <libs/asm.h>
#include <cpu/registers.h>

.macro isr.dispatch.define name
    .pushsection .text.isr.dispatch, "ax", %progbits
        .ifeq isr.current
            .balign 64
        .endif
        .function \name, cfi=custom, nosection=nosection
            .cfi_signal_frame
            isr.current.has_error = isr.current == 8 || (isr.current >= 10 && isr.current <= 14) || isr.current == 17
            .cfi_def_cfa %rsp, (8 * (5 + isr.current.has_error))
            .cfi_offset %rip, -(5 * 8)
            ALL_CFI_SAME_VALUE

            clac
            .if !isr.current.has_error
                push_value $0
            .endif
            push_value $isr.current
            JMP_AND_SPECULATION_POSTFENCE(interrupt_common)
        .end_function
    .popsection

    .quad \name
.endm

.macro isr.dispatch.define.next
    isr.dispatch.define isr.dispatch.\@
    isr.current = isr.current + 1
.endm

.pushsection .rodata.isr_table, "a", %progbits
.balign 8
.label isr_table, global, object
    isr.current = 0
    .rept 256
        isr.dispatch.define.next
    .endr
.size isr_table, . - isr_table
.popsection

.extern exception_handler
.extern nmi_handler
.function interrupt_common, global, align=64, cfi=custom
    .cfi_signal_frame
    .cfi_def_cfa %rsp, 7 * 8
    .cfi_offset %rip, -(5 * 8)

    // Mark each reg as having the same value as from the "calling frame"
    ALL_CFI_SAME_VALUE

    // Clear the direction flag
    cld

    // Save general purpose registers.
    push_reg %r15
    push_reg %r14
    push_reg %r13
    push_reg %r12
    push_reg %r11
    push_reg %r10
    push_reg %r9
    push_reg %r8
    push_reg %rax
    push_reg %rcx
    push_reg %rdx
    push_reg %rbx
    push_reg %rbp
    push_reg %rsi
    push_reg %rdi

    // Zero general-purpose registers to constrain speculative execution
    xorl %eax, %eax
    xorl %ebx, %ebx
    xorl %ecx, %ecx
    xorl %edx, %edx
    xorl %esi, %esi
    xorl %edi, %edi
    xorq %r8, %r8
    xorq %r9, %r9
    xorq %r10, %r10
    xorq %r11, %r11
    xorq %r12, %r12
    xorq %r13, %r13
    xorq %r14, %r14
    xorq %r15, %r15

    // Pass the Iframe in %rdi
    movq %rsp, %rdi

    // Was this an Non-maskable interrupt?s
    cmpq $2, IFRAME_OFFSET_VECTOR(%rsp)
    je .Lnmi

    // Check to see if we came from user space
    testb $3, IFRAME_OFFSET_CS(%rsp)
    jz 1f

    // We don't need the stack frame for user fault
    xorl %ebp, %ebp
    swapgs
1:
    call exception_handler

    // Check to see if we came from user space
    testb $3, IFRAME_OFFSET_CS(%rsp)
    jz .Lcommon_return
    swapgs

.Lcommon_return:
    // Restore general purpose registers.
    pop_reg %rdi
    pop_reg %rsi
    pop_reg %rbp
    pop_reg %rbx
    pop_reg %rdx
    pop_reg %rcx
    pop_reg %rax
    pop_reg %r8
    pop_reg %r9
    pop_reg %r10
    pop_reg %r11
    pop_reg %r12
    pop_reg %r13
    pop_reg %r14
    pop_reg %r15

    // Drop vector number and error code.
    add_to_sp 16

    iretq

.Lnmi:
    rdmsr64 MSR_GS_BASE
    mov %rax, %rbx

    // Compute the %rsp-relative offset of the stack base.
    .Lstack_base_from_rsp = INTERRUPT_STACK_SIZE - IFRAME_SIZE
    // The NMI stack is embedded in the percpu struct.
    .Lpercpu_from_rsp = .Lstack_base_from_rsp + PERCPU_INTERRUPT_STACKS_NMI_OFFSET
    mov %rsp, %rax
    sub $.Lpercpu_from_rsp, %rax

    rdmsr64 MSR_GS_BASE
    call nmi_handler

    mov %rbx, %rax
    wrmsr64 MSR_GS_BASE

    JMP_AND_SPECULATION_POSTFENCE(.Lcommon_return)
.end_function

.function load_idt, global
    lidt (%rdi)
    RET_AND_SPECULATION_POSTFENCE
.end_function