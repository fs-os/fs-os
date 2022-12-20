/* https://wiki.osdev.org/Bare_Bones#Linking_the_Kernel */

/* Entry point for boot.asm */
ENTRY(_start)

SECTIONS {
    /*
     * Start sections at 1MiB, a common place for kernels to be loaded by the
     * bootloader
     */
    . = 1M;

    /*
     * First the multiboot header from boot.asm (magic values for multiboot, etc.),
     * next the .text section
     */
    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data */
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack from boot.asm */
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
}

