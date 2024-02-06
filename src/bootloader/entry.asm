;------------------------------------------------------------------------------
; Entry point of our bootloader. The BIOS will look for the 0xAA55 signature in
; bytes 511-512, and loads us into 0x7C00.
;------------------------------------------------------------------------------

bits 16

; Specify base address where the BIOS will load us.
; TODO: Replace with linker script?
org 0x7C00

section .text

global _start
_start:
    ; Start by setting up the Data and Extra segments. We need to use an
    ; intermediate register to write to them.
    mov     ax, 0
    mov     ds, ax
    mov     es, ax

    ; We will also set up the Stack segment. Since the BIOS loaded us at address
    ; 0x7C00, and the stack grows downwards, we can use the current address as
    ; the bottom of the stack (The highest address).
    mov     ss, ax
    mov     sp, 0x7C00

    mov     si, msg_boot
    call    bios_puts

.halt:
    ; For now, halt the system
    ; TODO: Jump to kernel
    hlt
    jmp     .halt

;-------------------------------------------------------------------------------

; void bios_puts(const char* si);
bios_puts:
    push    ax
    push    bx
    push    si

.loop:
    lodsb                   ; Load byte from SI into AL, and increment SI
    test    al, al
    jz      .done

    mov     ah, 0xE         ; BIOS code for writing char
    mov     bh, 0x0         ; Page number: 0
    int     0x10            ; BIOS video interrupt

    jmp     .loop

.done:
    pop     si
    pop     bx
    pop     ax
    ret

;-------------------------------------------------------------------------------

; The string also needs to be inside the first 512 bytes.
msg_boot: db "Hello, world.", 13, 10, 0 ; "\r\n\0"

;-------------------------------------------------------------------------------

; Make sure we have enough space for the BIOS signature. See comment bellow.
%if ($-$$) > 510
%error "Binary is too large for boot sector (+512 bytes)."
%endif

; Fill the rest of the binary up to 510 with zeros. We subtract the address of
; the current instruction ($) from the address of the start of the current
; section ($$). Note that acording to our linker script (cfg/bootloader.ld), the
; first section will be the current one (.text).
times 510 - ($ - $$) db 0x00

; The BIOS will look for the 0xAA55 signature in bytes 511-512. Note the
; endianness.
db 0x55, 0xAA
