# AMD64 Specific Features

x86-64 (also known as x64, x86_64, AMD64, and Intel 64) is a 64-bit version of the x86 instruction set, first announced in 1999. It introduced two new modes of operation, 64-bit mode and compatiblity mode, along with a new 4-level paging mode.

With 64-bit mode and the new paging mode, it supports vastly larger amounts of virtual memory and physical memory than was possible on its 32-bit predecessors, allowing programs to store larger amounts of data in memory.s86-64 also expands general-purpose registers to 64-bit, and expands the number of them from 8 to 16, and provides numerous other enhancements.

## Features
### 64-bit integer capability

All general-purpose registers (GPRs) are expanded from 32 bits to 64 bits, and all arithmetic and logical operations, memory-to-register and register-to-memory operations, etc., can operate directly on 64-bit integers. Pushes and pops on the stack default to 8-byte strides, and pointers are 8 bytes wide.

### Additional registers

In addition to increasing the size of the general-purpose registers, the number of named general-purpose registers is increased from eight (i.e. eax, ecx, edx, ebx, esp, ebp, esi, edi) in x86 to 16 (i.e. rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15). It is therefore possible to keep more local variables in registers rather than on the stack, and to let registers hold frequently accessed constants; arguments for small and fast subroutines may also be passed in registers to a greater extent.

### Additional XMM (SSE) registers

Similarly, the number of 128-bit XMM registers (used for Streaming SIMD instructions) is also increased from 8 to 16.

### Larger virtual address space

The AMD64 architecture defines a 64-bit virtual address format, of which the low-order 48 bits are used in current implementations.  This allows up to 256 TiB (248 bytes) of virtual address space. The architecture definition allows this limit to be raised in future implementations to the full 64 bits, extending the virtual address space to 16 EiB (264 bytes).

### Larger physical address space

The original implementation of the AMD64 architecture implemented 40-bit physical addresses and so could address up to 1 TiB (240 bytes) of RAM.  Current implementations of the AMD64 architecture extend this to 48-bit physical addresses and therefore can address up to 256 TiB (248 bytes) of RAM. The architecture permits extending this to 52 bits in the future, this would allow addressing of up to 4 PiB of RAM.

### Instruction pointer relative data access

Instructions can now reference data relative to the instruction pointer (RIP register). This makes position-independent code, as is often used in shared libraries and code loaded at run time, more efficient.

### No-Execute bit

The No-Execute bit or NX bit (bit 63 of the page table entry) allows the operating system to specify which pages of virtual address space can contain executable code and which cannot.

## IO Operations

### OUTB

Sends a 8/17/32-bit value on a I/O location. The `a` modifier enforces val to be placed in the `eax` register before the asm command is issued and `Nd` allows for one-byte constant values to be assembled as constants, freeing the `edx` register for other cases.

Arguments:

`port` : I/O port to write to.

`val`  : Value to write to the I/O port.

```c++
template<std::unsigned_integral T>
	requires(sizeof(T) <= sizeof(uint32_t))
inline void outp(uint16_t port, T val)
{
	if constexpr(std::same_as<T, uint8_t>)
	{
		asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint16_t>)
	{
		asm volatile("outw %0, %1" ::"a"(val), "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint32_t>)
	{
		asm volatile("outl %0, %1" ::"a"(val), "Nd"(port));
	}
}
```

### INB

Recieves a 8/16/32-bit value from an I/O location.

Arguments:

`port` : I/O port to write to.


```c++
template<std::unsigned_integral T>
	requires(sizeof(T) <= sizeof(uint32_t))
inline T inp(uint16_t port)
{
	T val = T(0);

	if constexpr(std::same_as<T, uint8_t>)
	{
		asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint16_t>)
	{
		asm volatile("inw %1, %0" : "=a"(val) : "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint32_t>)
	{
		asm volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
	}

	return val;
}
```

## References

1. [x86-64 - Wikipedia](https://en.wikipedia.org/wiki/X86-64)
2. [x86-64 - OSDEV wiki](https://wiki.osdev.org/X86-64)
3. 2. [Inline Assembly - OSDEV wiki](https://wiki.osdev.org/Inline_Assembly)
4. [Inline Assembly Examples - OSDEV wiki](https://wiki.osdev.org/Inline_Assembly/Examples)