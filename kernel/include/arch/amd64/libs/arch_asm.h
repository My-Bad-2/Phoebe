#ifndef LIBS_ARCH_ASM_H
#define LIBS_ARCH_ASM_H 1

#ifdef __ASSEMBLER__
// clang-format off

.macro push_reg reg
pushq \reg
.cfi_adjust_cfa_offset 8
.cfi_rel_offset \reg, 0
.endm

.macro pop_reg reg
popq \reg
.cfi_adjust_cfa_offset -8
.cfi_same_value \reg
.endm

.macro push_value value
pushq \value
.cfi_adjust_cfa_offset 8
.endm

.macro pop_value value
popq \value
.cfi_adjust_cfa_offset -8
.endm

// Adding/Subtracting from the stack pointer (sp)

.macro sub_from_sp value
sub $\value, %rsp
.cfi_adjust_cfa_offset \value
.endm

.macro add_to_sp value
add $\value, %rsp
.cfi_adjust_cfa_offset -\value
.endm

#define ALL_CFI_SAME_VALUE \
    .cfi_same_value %rax ; \
    .cfi_same_value %rbx ; \
    .cfi_same_value %rcx ; \
    .cfi_same_value %rdx ; \
    .cfi_same_value %rsi ; \
    .cfi_same_value %rdi ; \
    .cfi_same_value %rbp ; \
    .cfi_same_value %r8  ; \
    .cfi_same_value %r9  ; \
    .cfi_same_value %r10 ; \
    .cfi_same_value %r11 ; \
    .cfi_same_value %r12 ; \
    .cfi_same_value %r13 ; \
    .cfi_same_value %r14 ; \
    .cfi_same_value %r15

// Treat all previous frame registers as not being restorable.

#define ALL_CFI_UNDEFINED \
    .cfi_undefined %rax ; \
    .cfi_undefined %rbx ; \
    .cfi_undefined %rcx ; \
    .cfi_undefined %rdx ; \
    .cfi_undefined %rsi ; \
    .cfi_undefined %rdi ; \
    .cfi_undefined %rbp ; \
    .cfi_undefined %r8  ; \
    .cfi_undefined %r9  ; \
    .cfi_undefined %r10 ; \
    .cfi_undefined %r11 ; \
    .cfi_undefined %r12 ; \
    .cfi_undefined %r13 ; \
    .cfi_undefined %r14 ; \
    .cfi_undefined %r15

#define RET_AND_SPECULATION_POSTFENCE \
    ret; \
    int3

#define JMP_AND_SPECULATION_POSTFENCE(x) \
    jmp x; \
    int3

// Writes %rax to the given MSR, which should be the bare constant.
// Clobbers %rcx and %rdx.
.macro wrmsr64 msr
    movl $\msr, %ecx
    movq %rax, %rdx
    shr $32, %rdx
    wrmsr
.endm

// Reads the given MSR, which should be the bare constant, into %rax.
// Clobbers %rcx and %rdx.
.macro rdmsr64 msr
    mov $\msr, %ecx
    rdmsr
    shl $32, %rdx
    or %rdx, %rax
.endm

#endif // __ASSEMBLER__

#endif // LIBS_ARCH_ASM_H