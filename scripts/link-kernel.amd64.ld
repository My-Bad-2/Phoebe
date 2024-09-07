OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(kernel_main)

PHDRS
{
    limine_requests PT_LOAD     FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
    text            PT_LOAD     FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata          PT_LOAD     FLAGS((1 << 2)) ;            /* Read only */
    data            PT_LOAD     FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
    dynamic         PT_DYNAMIC  FLAGS((1 << 1) | (1 << 2)) ; /* Dynamic PHDR for relocations */
}

SECTIONS
{
    . = 0xffffffff80000000;

    .text : {
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .init_array : {
        crti.o(.init_array)
        KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .init_array.*)))
        KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .init_array))
        crtn.o(.init_array)
    }

    .fini_array : {
        crti.o(.fini_array)
        KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .fini_array.*)))
        KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .fini_array))
        crtn.o(.fini_array)
    }

    . += CONSTANT(MAXPAGESIZE);
    
    .data : {
        *(.data .data.*)

        KEEP (*(.limine_requests_start_marker))
        KEEP (*(.limine_requests))
        KEEP (*(.limine_requests_end_marker))
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
        *(.interp)
    }
}