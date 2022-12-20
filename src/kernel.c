
/*
 * For more information see:
 *   https://wiki.osdev.org/Bare_Bones#Implementing_the_Kernel
 */

#include <stddef.h>
#include <stdint.h>

#include "lib/stdio.h"
#include "vga.h"

#if defined(__linux__)
#error "You are not using a cross compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#if !defined(__i386__)
#error "You are not using a ix86-elf compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

/* kernel_main: Called by boot.asm */
void kernel_main() {
    term_init();

    term_sprint("Hello, wellcome to the Free and Simple Operative System!\n"
                "This project is still being developed. For more information, see:\n"
                "https://github.com/fs-os/fs-os");
}

