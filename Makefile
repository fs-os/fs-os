
ASM=nasm
ASM_FLAGS=-f elf32
# See https://github.com/fs-os/cross-compiler
CC=/usr/local/cross/bin/i686-elf-gcc
# Global cflags
CFLAGS=-Wall -Wextra

.PHONY: clean all

all: fs-os.bin

# We will use the same compiler for linking
fs-os.bin: obj/kernel.o obj/boot.o src/linker.ld
	$(CC) -T src/linker.ld -o $@ -O2 -ffreestanding -nostdlib obj/kernel.o obj/boot.o -lgcc

obj/kernel.o: src/kernel.c
	@mkdir -p obj/
	$(CC) $(CFLAGS) -O2 -ffreestanding -std=gnu99 $< -c -o $@

obj/boot.o: src/boot.asm
	@mkdir -p obj/
	$(ASM) $(ASM_FLAGS) $< -o $@

clean:
	rm -f obj/*.o
