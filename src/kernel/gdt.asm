
; For more information, see:
;   https://wiki.osdev.org/GDT_Tutorial
;   https://www.youtube.com/watch?v=Wh5nPn2U_1w

%include "tss_struc.asm"    ; tss_t, gdt_entry_t

section .text

; C struct in: src/kernel/include/kernel/multitask.h
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

; -----------------------------------------------------------------------------------

gdt_start:
    .null_descriptor:           ; First segment is the null descriptor, where the
        dd      0x00000000      ; base, limit, access bytes, and flags are set to 0
        dd      0x00000000      ; We declare 2 double words (2 * 32 bits)
    .kernel_code:
        dw      0xffff          ; Define the first 16 bits of the limit
        dw      0x0000          ; First 24 bits of the base (16 + 8)
        db      0x00
        db      10011010b       ; ppt flags (4 bits) + type flags (4 bits). Note 1*
        db      11001111b       ; Other flags + last 4 bits of the limit. Note 2*
        db      0x00            ; Last 8 bits of the base
    .kernel_data:
        dw      0xffff          ; Same limit as code
        dw      0x0000          ; Same base as code
        db      0x00
        db      10010010b       ; Same ppt flags as code, but new type. Note 3*
        db      11001111b       ; Same "Other flags", same limit as code
        db      0x00            ; Same base as code
    .user_code:
        dw      0xffff          ; Same limit and base as kernel (start to end)
        dw      0x0000
        db      0x00
        db      11111010b       ; Same as kenel but in ring 3 (11 -> ring 3)
        db      11001111b       ; Same as kernel
        db      0x00
    .user_data:
        dw      0xffff
        dw      0x0000
        db      0x00
        db      10010010b       ; Same as kernel data but ring 3 (11 -> ring 3)
        db      11001111b
        db      0x00
    .tss:
        istruc gdt_entry_t                          ; See src/kernel/tss_struc.asm
            at gdt_entry_t.limit0,  dw 0x0000       ; First 16 bits of limit
            at gdt_entry_t.base0,   dw 0x0000       ; First 16 bits of base
            at gdt_entry_t.base1,   db 0x00         ; Mid 8 bits of base
            at gdt_entry_t.flags,   db 10001001b    ; ppt flags + type flags
            at gdt_entry_t.limit1,  db 0x00         ; no other flags + last 4 bits
                                                    ; of limit
            at gdt_entry_t.base2,   db 0x00         ; Last 8 bits of the base
        iend
gdt_end:

gdt_descriptor:
    dw      gdt_end - gdt_start - 1     ; Size of the gdt, word
    dd      gdt_start                   ; Pointer to the gdt

KERNEL_CODE_SEG equ gdt_start.kernel_code - gdt_start   ; Constants for descriptor
KERNEL_DATA_SEG equ gdt_start.kernel_data - gdt_start   ; offsets.
USER_CODE_SEG   equ gdt_start.user_code   - gdt_start   ; User segs currently unused.
USER_DATA_SEG   equ gdt_start.user_data   - gdt_start

; Note 1: The first 4 bits of that byte correspond to the "Present", "Privilege" and
;         "Type" flags. The first Present byte is 1 if the segment is used. The
;         Privilege field is 2 bits (00) and represents the ring (in our case ring
;         0). The last bit of this first half is the type bit. Should be 1 if code or
;         0 if data. In that case code.
;         The last 4 bits of the byte, are the type flags. First bit is the "Code"
;         flag, which indicates if there is code in that segment. In that case 1.
;         Next is the "Conforming" flag. It is 1 if we are able to execute this code
;         from lower privilege segments. Because it's the highest privilege segment,
;         we set it to 1. Then is the "Readable" bit, which is 1 if we can read
;         constants from that segment. And the last bit is the "Accessed" bit, which
;         is managed by the CPU so we set it to 0.
; Note 2: The first half of that byte corresponds to "Other flags". First bit
;         corresponds to "Granularity", which indicates if the limit is expressed in
;         bytes or in 4k byte regions. In our case 1. Next bit is the "32 bit" flag,
;         which is 1 if we are 32 bits. Last 2 bits are for 64bits, so we set them to
;         0.
;         The last 4 bits of the byte are last 4 bits of the limit, which in our case
;         is 1: 0xffff + f (0b1111) = 0xfffff
; Note 3: The last 4 bits change from the code segment, the first bit is not "Code",
;         so 0. Because the type is data, the second flag is now "Direction". If 1,
;         the segment grows downwards (0 because we don't want that). The 3rd bit,
;         instead of "Readable", is now the "Writable" bit. We set it to 1 because
;         we can write to it.

; -----------------------------------------------------------------------------------

; Called from _start (src/kernel/boot.asm)
global gdt_init:function
gdt_init:
    cli                         ; Disable interrupts
    push    eax
    jmp     .fill_tss_done      ; TODO: Fill tss

.fill_tss:
    mov     eax, TSS_SIZE
    mov     [gdt_start.tss + gdt_entry_t.limit0], ax    ; First 16 bits of tss limit

    mov     eax, tss_start
    mov     [gdt_start.tss + gdt_entry_t.base0], ax     ; First 16 bits of tss base

    mov     eax, tss_start
    shr     eax, 8
    mov     [gdt_start.tss + gdt_entry_t.base1], ax     ; Mid 8 bits of tss base

    ; (flags are known at compile time)

    mov     eax, TSS_SIZE
    shr     eax, 16
    mov     [gdt_start.tss + gdt_entry_t.limit1], al    ; Last 4 bits of limit (+ no
                                                        ; flags)

    mov     eax, tss_start
    shr     eax, 16
    mov     [gdt_start.tss + gdt_entry_t.base2], ax     ; Last 8 bits of tss base

.fill_tss_done:
    lgdt    [gdt_descriptor]    ; Load the gdt descriptor, containing the gdt size
                                ; and the pointer to the gdt itself
    mov     eax, cr0            ; We need to change the last bit of cr0 to 1, but we
    or      eax, 1              ; need to move it to eax first and then restore the
    mov     cr0, eax            ; modified register to cr0. We are in 32bit mode now

    pop     eax

    jmp     KERNEL_CODE_SEG:gdt_done    ; Now we do a far jump (jump to another
                                        ; segment) with the "gdt_done" offset. Just
                                        ; return to _start at src/kernel/boot.asm

gdt_done:
    ret

; Tss* tss_getptr(void);
global tss_getptr:function
tss_getptr:
    mov     eax, tss_start
    ret

