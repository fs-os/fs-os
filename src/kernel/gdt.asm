; For more information, see the wiki:
; https://github.com/fs-os/fs-os/wiki/Kernel-and-bootloader#gdt

%include "structs.asm"      ; tss_t, gdt_entry_t

section .text

; ASM struct in: src/kernel/structs.asm
; C struct in:   src/kernel/include/kernel/multitask.h
tss_start:
    istruc tss_t
        at tss_t.link,      dw 0x0000
        at tss_t.pad0,      dw 0x0000

        at tss_t.esp0,      dd 0x00000000
        at tss_t.ss0,       dw 0x0000
        at tss_t.pad1,      dw 0x0000
        at tss_t.esp1,      dd 0x00000000
        at tss_t.ss1,       dw 0x0000
        at tss_t.pad2,      dw 0x0000
        at tss_t.esp2,      dd 0x00000000
        at tss_t.ss2,       dw 0x0000
        at tss_t.pad3,      dw 0x0000

        at tss_t.cr3,       dd 0x00000000
        at tss_t.eip,       dd 0x00000000
        at tss_t.eflags,    dd 0x00000000

        at tss_t.eax,       dd 0x00000000
        at tss_t.ecx,       dd 0x00000000
        at tss_t.edx,       dd 0x00000000
        at tss_t.ebx,       dd 0x00000000
        at tss_t.esp,       dd 0x00000000
        at tss_t.ebp,       dd 0x00000000
        at tss_t.esi,       dd 0x00000000
        at tss_t.edi,       dd 0x00000000

        at tss_t.es,        dw 0x0000
        at tss_t.pad4,      dw 0x0000
        at tss_t.cs,        dw 0x0000
        at tss_t.pad5,      dw 0x0000
        at tss_t.ss,        dw 0x0000
        at tss_t.pad6,      dw 0x0000
        at tss_t.ds,        dw 0x0000
        at tss_t.pad7,      dw 0x0000
        at tss_t.fs,        dw 0x0000
        at tss_t.pad8,      dw 0x0000
        at tss_t.gs,        dw 0x0000
        at tss_t.pad9,      dw 0x0000

        at tss_t.ldtr,      dw 0x0000
        at tss_t.pad10,     dw 0x0000
        at tss_t.pad11,     dw 0x0000
        at tss_t.iobp,      dw 0x0000
        at tss_t.ssp,       dd 0x00000000
    iend
tss_end:

TSS_SIZE    equ tss_end - tss_start

; ------------------------------------------------------------------------------

; For more information about each entry, see the fs-os wiki
gdt_start:
    .null_descriptor:           ; First segment is the null descriptor, where
        dd      0x00000000      ; the base, limit, access bytes, and flags are 0
        dd      0x00000000      ; We declare 2 double words (2 * 32 bits)
    .kernel_code:
        istruc gdt_entry_t                          ; See src/kernel/structs.asm
            at gdt_entry_t.limit0,  dw 0xffff       ; First 16 bits of the limit
            at gdt_entry_t.base0,   dw 0x0000       ; First 24 bits of the base
            at gdt_entry_t.base1,   db 0x00         ; Mid 8 bits of base
            at gdt_entry_t.flags,   db 10011010b    ; ppt flags + type flags
            at gdt_entry_t.limit1,  db 11001111b    ; Other flags, end of limit
            at gdt_entry_t.base2,   db 0x00         ; Last 8 bits of the base
        iend
    .kernel_data:
        istruc gdt_entry_t
            at gdt_entry_t.limit0,  dw 0xffff       ; Same limit & base as code
            at gdt_entry_t.base0,   dw 0x0000
            at gdt_entry_t.base1,   db 0x00
            at gdt_entry_t.flags,   db 10010010b    ; Change type flag
            at gdt_entry_t.limit1,  db 11001111b
            at gdt_entry_t.base2,   db 0x00
        iend
    .tss:
        istruc gdt_entry_t
            at gdt_entry_t.limit0,  dw 0x0000       ; First 16 bits of limit
            at gdt_entry_t.base0,   dw 0x0000       ; First 16 bits of base
            at gdt_entry_t.base1,   db 0x00         ; Mid 8 bits of base
            at gdt_entry_t.flags,   db 10001001b    ; ppt flags + type flags
            at gdt_entry_t.limit1,  db 0x00         ; no other flags + last 4
                                                    ; bits of limit.
            at gdt_entry_t.base2,   db 0x00         ; Last 8 bits of the base
        iend
gdt_end:

gdt_descriptor:
    dw      gdt_end - gdt_start - 1     ; Size of the gdt, word
    dd      gdt_start                   ; Pointer to the gdt

KERNEL_CODE_SEG equ gdt_start.kernel_code - gdt_start   ; Constants for
KERNEL_DATA_SEG equ gdt_start.kernel_data - gdt_start   ; descriptor offsets.

; ------------------------------------------------------------------------------

; Called from _start (src/kernel/boot.asm)
global gdt_init:function
gdt_init:
    cli                         ; Disable interrupts
    push    eax
    jmp     .fill_tss_done      ; TODO: Fill tss

.fill_tss:
    mov     eax, TSS_SIZE
    mov     [gdt_start.tss + gdt_entry_t.limit0], ax    ; First 16 bits of tss
                                                        ; limit.

    mov     eax, tss_start
    mov     [gdt_start.tss + gdt_entry_t.base0], ax     ; First 16 bits of tss
                                                        ; base.

    mov     eax, tss_start
    shr     eax, 8
    mov     [gdt_start.tss + gdt_entry_t.base1], ax     ; Mid 8 bits of tss base

    ; (flags are known at compile time)

    mov     eax, TSS_SIZE
    shr     eax, 16
    mov     [gdt_start.tss + gdt_entry_t.limit1], al    ; Last 4 bits of limit
                                                        ; (+ no flags)

    mov     eax, tss_start
    shr     eax, 16
    mov     [gdt_start.tss + gdt_entry_t.base2], ax     ; Last 8 bits of tss
                                                        ; base.

.fill_tss_done:
    lgdt    [gdt_descriptor]    ; Load the gdt descriptor, containing the gdt
                                ; size and the pointer to the gdt itself
    mov     eax, cr0            ; We need to change the last bit of cr0 to 1,
    or      eax, 1              ; but we need to move it to eax first and then
    mov     cr0, eax            ; restore the modified register to cr0. We are
                                ; in 32bit mode now.

    pop     eax

    jmp     KERNEL_CODE_SEG:gdt_done    ; Now we do a far jump (jump to another
                                        ; segment) with the "gdt_done" offset.
                                        ; Just return to _start at
                                        ; src/kernel/boot.asm

gdt_done:
    ret

; Tss* tss_getptr(void);
global tss_getptr:function
tss_getptr:
    mov     eax, tss_start
    ret

