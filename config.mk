
# For compiling the bootstrap assembly
ASM=nasm
ASM_FLAGS=-f elf32

# i686 cross-compiler. See https://github.com/fs-os/cross-compiler
CC=/usr/local/cross/bin/i686-elf-gcc

# Global cflags. Other commands use specific ones not yet in the config
CFLAGS=-Wall -Wextra -O2 -masm=intel

# Cross-compiled ar for creating the static library (LIBC)
AR=/usr/local/cross/bin/i686-elf-ar

# Kernel binary and iso filenames
KERNEL_BIN=fs-os.bin
ISO=$(KERNEL_BIN:.bin=.iso)

# List of object files to be linked with the kernel. Same for asm_objs but with
# different compilation method.
KERNEL_OBJS=obj/kernel/kernel.o obj/kernel/vga.o obj/kernel/alloc.o obj/kernel/framebuffer.o obj/kernel/framebuffer_console.o obj/kernel/idt.o obj/kernel/exceptions.o obj/kernel/rtc.o
ASM_OBJS=obj/kernel/boot.o obj/kernel/io.o obj/kernel/gdt.o obj/kernel/idt_asm.o

# List of object files of our standard library, and the final static library
LIBC_OBJS=obj/libc/string.o obj/libc/stdlib.o obj/libc/stdio.o
LIBC=obj/libc.a

# Libk is the libc version (with some changes) that the kernel uses for building. We
# don't need a static lib, because we can just link the kernel with these objs
# instead.
LIBK_OBJS=obj/libk/string.o obj/libk/stdlib.o obj/libk/stdio.o

# sysroot paths
SYSROOT=./sysroot
SYSROOT_INCLUDEDIR=usr/include
SYSROOT_LIBDIR=usr/lib
SYSROOT_BOOTDIR=boot

SYSROOT_KERNEL=$(SYSROOT)/$(SYSROOT_BOOTDIR)/$(KERNEL_BIN)

# Paths for moving the headers to the sysroot. For example:
# (src/libc/include/*.h -> sysroot/usr/include/*.h)
KERNEL_INCLUDES=src/kernel/include
LIBC_INCLUDES=src/libc/include

# For replacing "(GITHASH)" with the last commit in the grub entry. Comment these
# lines if you just want the os name.
PERCENT:=%
COMMIT_CMD=git branch -v --format="$(PERCENT)(objectname:short)$(PERCENT)(HEAD)" | grep "*$$" | tr -d "*"
COMMIT_SHA1=($(shell $(COMMIT_CMD)))

