
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
KERNEL_OBJ_FILES=kernel.c.o \
                 vga.c.o \
                 paging.c.o \
                 heap.c.o \
                 multitask.c.o \
                 framebuffer.c.o \
                 framebuffer_console.c.o \
                 idt.c.o \
                 exceptions.c.o \
                 rtc.c.o \
                 pit.c.o \
                 pcspkr.c.o \
                 keyboard.c.o

# TODO: Remove ASM_OBJS
ASM_OBJ_FILES=kernel/boot.asm.o \
              kernel/io.asm.o \
              kernel/gdt.asm.o \
              kernel/idt.asm.o \
              kernel/paging.asm.o \
              kernel/multitask.asm.o \
              kernel/rand.asm.o \
              kernel/util.asm.o \
              libk/math.asm.o

APP_OBJ_FILES=sh/sh.c.o \
              piano/piano.c.o \
              minesweeper/minesweeper.c.o \
              5x5/5x5.c.o \
              mandelbrot/mandelbrot.c.o

# Libk is the libc version used by the kernel. We will link the final kernel
# binary with these objects.
LIBK_OBJ_FILES=string.c.o \
               stdlib.c.o \
               stdio.c.o \
               ctype.c.o \
               time.c.o \
               curses.c.o \
               math.c.o

# Paths for the sysroot
SYSROOT=./sysroot
SYSROOT_INCLUDE_DIR=$(SYSROOT)/usr/include
SYSROOT_BOOT_DIR=$(SYSROOT)/boot
SYSROOT_KERNEL=$(SYSROOT_BOOT_DIR)/$(KERNEL_BIN)

# Paths for moving the headers to the sysroot. For example:
# (src/libk/include/*.h -> sysroot/usr/include/*.h)
KERNEL_INCLUDE_DIR=src/kernel/include
LIBK_INCLUDE_DIR=src/libk/include

#-------------------------------------------------------------------------------

KERNEL_OBJS=$(addprefix obj/kernel/, $(KERNEL_OBJ_FILES))
ASM_OBJS=$(addprefix obj/, $(ASM_OBJ_FILES))
APP_OBJS=$(addprefix obj/apps/, $(APP_OBJ_FILES))
LIBK_OBJS=$(addprefix obj/libk/, $(LIBK_OBJ_FILES))

SRC_HEADERS=$(wildcard $(KERNEL_INCLUDE_DIR)/*/*.h) $(wildcard $(LIBK_INCLUDE_DIR)/*.h)
SYSROOT_HEADERS=$(patsubst $(LIBK_INCLUDE_DIR)/%, $(SYSROOT_INCLUDE_DIR)/%, \
                $(patsubst $(KERNEL_INCLUDE_DIR)/%, $(SYSROOT_INCLUDE_DIR)/%, $(SRC_HEADERS)))

# For replacing "(GITHASH)" with the last commit in the bootloader entry.
# Comment these lines if you just want the os name.
PERCENT:=%
COMMIT_CMD=git branch -v --format="$(PERCENT)(objectname:short)$(PERCENT)(HEAD)" | grep "*$$" | tr -d "*"
COMMIT_SHA1=($(shell $(COMMIT_CMD)))

ifeq ($(SSE_SUPPORT), true)
CFLAGS += -msse -msse2 -DENABLE_SSE
ASM_FLAGS += -D ENABLE_SSE
endif
