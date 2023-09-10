
/* Entry point for boot.asm */
ENTRY(_start)

SECTIONS {
    /* Start sections at 1MiB, a common place for kernels to be loaded by the
     * bootloader */
    . = 1M;

    /* First the multiboot header from boot.asm (magic values for multiboot,
     * etc.), next the .text section.
     * _text_start is used to know the physical address of each section from C
     * code. We need to align it to 4KiB as well. See the wiki for more info. */
    . = ALIGN(4K);
    _text_start = .;
    .text BLOCK(4K) : {
        *(.multiboot)
        *(.text)
    }
    . = ALIGN(4K);
    _text_end = .;

    /* Read-only data, like string literals */
    _rodata_start = .;
    .rodata BLOCK(4K) : {
        *(.rodata*)
    }
    . = ALIGN(4K);
    _rodata_end = .;

    /* Read-write data (initialized) */
    _data_start = .;
    .data BLOCK(4K) : {
        *(.data)
    }
    . = ALIGN(4K);
    _data_end = .;

    /* Read-write data (uninitialized) and stack from boot.asm */
    _bss_start = .;
    .bss BLOCK(4K) : {
        *(COMMON)
        *(.bss)
    }
    . = ALIGN(4K);
    _bss_end = .;
}
