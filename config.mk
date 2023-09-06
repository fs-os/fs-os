
# Set to false to disable SSE/SSE2 support (experimental)
SSE_SUPPORT=true

# Assembler
ASM=nasm
ASM_FLAGS=-f elf32 -isrc/kernel -isrc/kernel/include/kernel

# i686 cross-compiler. See https://github.com/fs-os/cross-compiler
CC=/usr/local/cross/bin/i686-elf-gcc
CFLAGS=-Wall -Wextra -O2 -masm=intel

# Kernel binary and iso filenames
KERNEL_BIN=fs-os.bin
ISO=$(KERNEL_BIN:.bin=.iso)

# List of object files to be linked with the kernel. Same for asm_objs but with
# different compilation method.
KERNEL_OBJS=obj/kernel/kernel.c.o \
			obj/kernel/vga.c.o \
			obj/kernel/paging.c.o \
			obj/kernel/heap.c.o \
			obj/kernel/multitask.c.o \
			obj/kernel/framebuffer.c.o \
			obj/kernel/framebuffer_console.c.o \
			obj/kernel/idt.c.o \
			obj/kernel/exceptions.c.o \
			obj/kernel/rtc.c.o \
			obj/kernel/pit.c.o \
			obj/kernel/pcspkr.c.o \
			obj/kernel/keyboard.c.o

ASM_OBJS=obj/kernel/boot.asm.o \
		 obj/kernel/io.asm.o \
		 obj/kernel/gdt.asm.o \
		 obj/kernel/idt.asm.o \
		 obj/kernel/paging.asm.o \
		 obj/kernel/multitask.asm.o \
		 obj/kernel/rand.asm.o \
		 obj/kernel/util.asm.o

# List of object files containing the app functions. Built into the kernel.
APP_OBJS=obj/apps/sh/sh.c.o obj/apps/piano/piano.c.o obj/apps/minesweeper/minesweeper.c.o obj/apps/5x5/5x5.c.o

# Libk is the libc version used by the kernel. We will link the final kernel
# binary with these objects.
LIBK_OBJS=obj/libk/string.c.o obj/libk/stdlib.c.o obj/libk/stdio.c.o obj/libk/ctype.c.o obj/libk/time.c.o obj/libk/curses.c.o obj/libk/math.c.o obj/libk/math.asm.o

# Paths for the sysroot
SYSROOT=./sysroot
SYSROOT_INCLUDEDIR=$(SYSROOT)/usr/include
SYSROOT_BOOTDIR=$(SYSROOT)/boot
SYSROOT_KERNEL=$(SYSROOT_BOOTDIR)/$(KERNEL_BIN)

# Paths for moving the headers to the sysroot. For example:
# (src/libk/include/*.h -> sysroot/usr/include/*.h)
KERNEL_INCLUDES=src/kernel/include
LIBK_INCLUDES=src/libk/include

#-------------------------------------------------------------------------------

# For replacing "(GITHASH)" with the last commit in the bootloader entry.
# Comment these lines if you just want the os name.
PERCENT:=%
COMMIT_CMD=git branch -v --format="$(PERCENT)(objectname:short)$(PERCENT)(HEAD)" | grep "*$$" | tr -d "*"
COMMIT_SHA1=($(shell $(COMMIT_CMD)))

ifeq ($(SSE_SUPPORT), true)
CFLAGS += -msse -msse2 -DENABLE_SSE
ASM_FLAGS += -D ENABLE_SSE
endif
