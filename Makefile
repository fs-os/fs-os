
ASM=nasm
ASM_FLAGS=-f elf32

.PHONY: clean

obj/boot.o: src/boot.asm
	@mkdir -p obj/
	$(ASM) $(ASM_FLAGS) $< -o $@

clean:
	rm -f obj/*.o
