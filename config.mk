
# For compiling the bootstrap assembly
ASM=nasm
ASM_FLAGS=-f elf32 -isrc/kernel -isrc/kernel/include/kernel

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
KERNEL_OBJS=obj/kernel/kernel.c.o obj/kernel/vga.c.o obj/kernel/paging.c.o obj/kernel/heap.c.o obj/kernel/multitask.c.o obj/kernel/framebuffer.c.o obj/kernel/framebuffer_console.c.o obj/kernel/idt.c.o obj/kernel/exceptions.c.o obj/kernel/rtc.c.o obj/kernel/pit.c.o obj/kernel/pcspkr.c.o obj/kernel/keyboard.c.o
ASM_OBJS=obj/kernel/boot.asm.o obj/kernel/io.asm.o obj/kernel/gdt.asm.o obj/kernel/idt.asm.o obj/kernel/paging.asm.o obj/kernel/multitask.asm.o obj/kernel/rand.asm.o obj/kernel/asm_util.asm.o

# List of object files containing the app functions. For now built into the kernel
# until we have a proper userspace.
# sh means src/apps/sh/sh.c will be compiled to obj/apps/sh.c.o
APP_OBJS=obj/apps/sh/sh.c.o obj/apps/piano/piano.c.o obj/apps/minesweeper/minesweeper.c.o

# Libk is the libc version (with some changes) that the kernel uses for building. We
# don't need a static lib, because we can just link the kernel with these objs
# instead.
LIBK_OBJS=obj/libk/string.c.o obj/libk/stdlib.c.o obj/libk/stdio.c.o obj/libk/ctype.c.o obj/libk/time.c.o obj/libk/curses.c.o

# List of object files of our standard library, and the final static library
LIBC_OBJS=obj/libc/string.c.o obj/libc/stdlib.c.o obj/libc/stdio.c.o obj/libk/ctype.c.o obj/libc/time.c.o obj/libc/curses.c.o
LIBC=obj/libc.a

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

