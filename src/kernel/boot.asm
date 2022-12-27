;----------------------------------------------
; https://wiki.osdev.org/Bare_Bones_with_NASM
; Compile using:
;   nasm -f elf32 boot.asm
;----------------------------------------------

MB_ALIGN    equ 1 << 0                  ; Align loaded modules on page boundaries
MB_MEMINFO  equ 1 << 1                  ; Provide memory map
MB_FLAGS    equ MB_ALIGN | MB_MEMINFO   ; Multiboot flag field
MB_MAGIC    equ 0x1BADB002              ; Magic number for the bootloader
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)  ; Checksum of above to prove we are multiboot

; Declare the multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will search
; for this signature in the first 8KiB of the kernel file, aligned at a 32-bit
; boundary.
;
; More information on alignment:
;   https://stackoverflow.com/questions/19608845/understanding-assembly-mips-align-and-memory-addressing
section .multiboot
    align 4
    dd      MB_MAGIC
    dd      MB_FLAGS
    dd      MB_CHECKSUM

; The multiboot standard does not the fine the vale of esp, and it's up to the kernel
; to provide a stack. This allocates room for a small stack by creating a symbol at 
; the bottom of the stack, allocating 16384 bytes and creating a symbol at the top.
; Keep in mind that the stack grows downwards in x86. The stack is in its own section
; because in nasm, .bss is marked as nobits which means the kernel file is smaller,
; because it does not contain an uninitialized stack.
; From https://www.nasm.us/doc/nasmdoc8.html
;   > nobits defines the section to be one with no explicit contents given, such as a
;   > BSS section.
; The stack must be 16 byte aligned acording to the System V ABI standard and de-facto
; extensions. The compiler will assume the stack is properly aligned and failure to
; align the stack will result in undefined behavior.
section .bss
    align   16
stack_bottom:       ; Items pushed to the stack will get 'closer' to this label (in x86)
    resb    16384   ; 16 KiB
stack_top:

; Declare the kernel entry point.
; TODO: What is :funtion (...) for?
section .text
    global _start:function (_start.end - _start)        ; Size of the _start section

_start:
    ; The bootloader loaded us into 32bit protected mode on a x86 machine. Interrupts
    ; are disabled. Paging is disabled. The processor state is as defined in the
    ; multiboot standard. The kernel has full control over the CPU. The kernel can
    ; only make use of hardware features and code it provides as part of itself.
    ; There is no stdio.h or anything like that. There are no security restrictions,
    ; safeguards or debugging mechanisms. Only what the kernel provides.

    ; To setup the stack, we move esp to the 'top' label we declared earlier, because
    ; the stack grows to a lower memory address in x86 assembly.
    mov     esp, stack_top

    ; This is a good place to initialize crucial processor state before the high-level
    ; kernel is entered. The processor is not fully initialized yet. We are missing
    ; features such as:
    ;   - Floating point instructions
    ;   - Instruction set extensions (Instruction Set Extensions are additional
    ;     instructions that can increase performance when the same operations are
    ;     performed on multiple data objects.)
    ;   - GDT (Global Descriptor Table)
    ;   - Paging
    ;   - C++ features such as global constructors and exceptions

    ; Push ebx, containing the multiboot information structure pointer, which we will
    ; pass to main as argument. See src/kernel/include/kernel/multiboot.h
    push    ebx

    ; Enter a high level kernel. The ABI requires the stack to be 16byte aligned at
    ; the time of the call instruction (Because it pushes the return address to the
    ; stack; 4 bytes). We aligned the stack to 16bytes when reserving the stack and
    ; we have pushed a multiple of 16 (0 bytes).
    extern  kernel_main
    call    kernel_main

    ; We should not reach here. If the system has nothing more to do, put the
    ; computer into an infinite loop:
    ;   - Disable interrupts with cli (clear interrupt enable in eflags). This is not
    ;     necesary here because they are already disabled by the bootloader. Keep in
    ;     mind that we might enable them later and return from kernel_main (which is
    ;     sort of nonsensical to do).
    ;   - Wait for the next interrupt to arrive with hlt (Halt instruction). Since
    ;     interrupts are disabled, will lock up the computer.
    ;   - Jump to the hlt instruction in case it wakes up from a non-maskable
    ;     interrupt or due to system management mode.
    cli

.hang:
    hlt
    jmp     .hang

.end:

