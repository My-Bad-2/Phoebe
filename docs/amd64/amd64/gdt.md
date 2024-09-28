# Global Descriptor Table

The Global Descriptor Table (GDT) is a binary data structure specific to IA-32 and x86-64 architecture. It contains entries telling the CPU about memory segment.

## Global Descriptor Table Register (GDTR)

The GDT is pointed to by the value in the GDTR  register. This is loaded using the `lgdt` assembly instruction, who argument is a pointer to a GDT Descriptor structure.

| Bits    | Type   |
| ------- | ------ |
| 79 - 16 | Offset |
| 15 - 0  | Size   |

**Size:** The size of the table in bytes minus 1. The maximum size of the GDT can be upto 0x10000 bytes. Further, no GDT can have a size of 0 bytes.

**Offset:** The linear address of the GDT (not physical).

## Table

The entries in the GDT are 8 bytes long and form a table:

| Address          | Content     |
| ---------------- | ----------- |
| GDTR Offset + 0  | Null        |
| GDTR Offset + 8  | Kernel Code |
| GDTR Offset + 16 | Kernel Data |
| GDTR Offset + 24 | User Data   |
| GDTR Offset + 32 | User Code   |

## Segment Descriptor

Each entry in the table has a complex structure:

| offset  | Type        |
| ------- | ----------- |
| 63 - 56 | Base        |
| 55 - 52 | Flags       |
| 51 - 48 | Limit       |
| 47 - 40 | Access byte |
| 39 - 32 | Base        |
| 31 - 16 | Base        |
| 15 - 0  | Limit       |

### Access byte

| Bits  | Type                         |
| ----- | ---------------------------- |
| 7     | Present                      |
| 6 - 5 | Descriptor Privilege Level   |
| 4     | Descriptor Type              |
| 3     | Executable                   |
| 2     | Direction bit/Conforming bit |
| 1     | Readable/Writable Bit        |
| 0     | Accessed Bit                 |

### Flags

| Bits | Type                |
| ---- | ------------------- |
| 3    | Granularity flag    |
| 2    | Size Flag           |
| 1    | Long-mode code flag |
| 0    | Reserved            |

## Loading GDT and Reloading System segments

The `lgdt` instructions are used to load the base address and the limit of the GDT in the CPU. Whatever changes are made in the GDT has no effect on the CPu until the new Segment Selectors are loaded into Segment Registers. For most of these registers, the process is as simple as using `mov` instructions, but changing the CS register requires code resembling a jump or call to elsewhere, as this is the only way its value is meant to be changed.

In order to load the GDT Table in the CPU and reload segment Registers. 

```assembly
load_gdt:
    lgdtq (%rdi)
    movw $KERNEL_DATA_SELECTOR, %ax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    movl %eax, %ss
    lea 1f(%rip), %rax
    pushq $KERNEL_CODE_SELECTOR
    pushq %rax
    lretq
1:
    retq
```