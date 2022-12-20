
ASM=nasm
ASM_FLAGS=-f elf32
# See https://github.com/fs-os/cross-compiler
CC=/usr/local/cross/bin/i686-elf-gcc
# Global cflags
CFLAGS=-Wall -Wextra
BIN=fs-os.bin
ISO=$(BIN:.bin=.iso)

.PHONY: clean all

all: $(ISO)

$(ISO): $(BIN)
	@echo TODO

# We will use the same compiler for linking
$(BIN): obj/kernel.o obj/boot.o cfg/linker.ld
	$(CC) -T cfg/linker.ld -o $@ -O2 -ffreestanding -nostdlib obj/kernel.o obj/boot.o -lgcc

obj/kernel.o: src/kernel.c
	@mkdir -p obj/
	$(CC) $(CFLAGS) -O2 -ffreestanding -std=gnu99 $< -c -o $@

obj/boot.o: src/boot.asm
	@mkdir -p obj/
	$(ASM) $(ASM_FLAGS) $< -o $@

clean:
	rm -f obj/*.o $(BIN) $(ISO)
