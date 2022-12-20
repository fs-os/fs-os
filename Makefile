
ASM=nasm
ASM_FLAGS=-f elf32

# See https://github.com/fs-os/cross-compiler
CC=/usr/local/cross/bin/i686-elf-gcc
CFLAGS=-Wall -Wextra

BIN=fs-os.bin
ISO=$(BIN:.bin=.iso)

LIB=obj/lib/stdio.o

.PHONY: clean all qemu

# -----------------------------------------------------------------------

all: $(ISO)

$(ISO): $(BIN)
	mkdir -p iso/boot/grub/
	cp $(BIN) iso/boot/$(BIN)
	cp cfg/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

# We will use the same compiler for linking. We need to make a separate target for
# the kernel, boot and lib, because they have different compilation methods.
$(BIN): cfg/linker.ld obj/kernel.o obj/boot.o $(LIB)
	$(CC) -T cfg/linker.ld -o $@ -O2 -ffreestanding -nostdlib obj/kernel.o obj/boot.o $(LIB) -lgcc

obj/kernel.o: src/kernel.c
	@mkdir -p obj/
	$(CC) $(CFLAGS) -O2 -ffreestanding -std=gnu99 -c -o $@ $<

obj/boot.o: src/boot.asm
	@mkdir -p obj/
	$(ASM) $(ASM_FLAGS) $< -o $@

# Not sure if it's a good idea to use -ffreestanding here
$(LIB): obj/lib/%.o : src/lib/%.c
	@mkdir -p obj/lib/
	$(CC) $(FLAGS) -ffreestanding -c -o $@ $<

# -----------------------------------------------------------------------

# Alterative: qemu-system-i386 -kernel fs-os.bin
qemu: $(ISO)
	qemu-system-i386 -cdrom $<

clean:
	rm -f obj/*.o $(BIN) $(ISO)
	rm -rf iso
