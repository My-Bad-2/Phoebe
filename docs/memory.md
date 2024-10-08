# Memory Management

Memory Management is a form of resource management applied to system memory. The essential requirement of memory management is to provide ways to dynamically allocate portions of memory and free it for reuse.

The Memory Management system is divided into 3 parts,
1) Physical Memory Management
2) Virtual Memory Management
3) Dynamic Memory Management (Heap)

## Address Spaces

Many platforms use a **Memory Management Unit (MMU)** to handle translation between the virtual and physical address spaces. Having multiple address spaces allows each task to have its own memory space to work in.

### Physical Address Space

The physical address Space is the direct memory address used to access the a real location in RAM. It is the most efficient, but not the most secure way of accessing memory.

### Virtual Address Space

Memory Management Units allows virtual addresses to be used. A virtual address can be mapped to any physical address. This provides each process to have its own address space.

### Physical Memory Management

The purpose of a physical memory manager is to split up the systems physical address space into block-sized chunks of memory (4096 bytes per chunk) and provide a method to allocate and release them.

A Bitmap is used to keep track of these blocks, every bit in the bitmap represents one block of physical memory.

### Virtual Memory Manager

Virtual Memory is a special memory Addressing Scheme implemented by both the hardware and kernel. It allows non-contigous physical memory to act as if it was contigous memory.

#### Translation Lookaside Buffer (TLB)
