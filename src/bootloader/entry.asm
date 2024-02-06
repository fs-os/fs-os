;------------------------------------------------------------------------------
; Entry point of our bootloader. The BIOS will look for the 0xAA55 signature in
; bytes 511-512, and loads us into 0x7C00.
;------------------------------------------------------------------------------

bits 16

; Specify base address where the BIOS will load us. We need to use NASM's
; built-in "linker" because we are generating a raw binary, and we can't link
; with `ld'.
org 0x7C00

section .text

; Entry point of the bootloader. The Boot Record must start with a short jump to
; the real entry point followed by a NOP.
global _start
_start:
    jmp     short bootloader_entry
    nop

;-------------------------------------------------------------------------------

; Now the BIOS Parameter Block (BPB)
bpb_oem:                    db "fsosboot"   ; 3 (Shouldn't matter, 8 bytes)
bpb_bytes_per_sector:       dw 512          ; 11
bpb_sectors_per_cluster:    db 1            ; 13
bpb_reserved_sectors:       dw 1            ; 14 (Includes boot sector)
bpb_fat_count:              db 2            ; 16
bpb_dir_entries_count:      dw 0xE0         ; 17
bpb_total_sectors:          dw 2880         ; 19 (1.44MiB / 512bps = 2880)
bpb_media_descriptor_type:  db 0xF0         ; 21 (3.5" floppy disk)
bpb_sectors_per_fat:        dw 9            ; 22
bpb_sectors_per_track:      dw 18           ; 24
bpb_heads:                  dw 2            ; 26
bpb_hidden_sectors:         dd 0            ; 28
bpb_large_sector_count:     dd 0            ; 32 (Set if entry 19 is zero)

; Next, the Extended Boot Record (EBR)
ebr_drive_number:           db 0            ; 36 (0 for floppy, 0x80 for HDDs)
ebr_reserved:               db 0            ; 37
ebr_signature:              db 0x28         ; 38 (0x28 or 0x29)
ebr_volume_id:              db "FSOS"       ; 39 (Shouldn't matter, 4 bytes)
ebr_volume_label:           db "Bootloader " ; 43 (Shouldn't matter, 11 bytes)
ebr_system_id:              db "FAT12   "   ; 54 (8 bytes)

;-------------------------------------------------------------------------------

bootloader_entry:
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
    test    al, al          ; Did we reach the end of the string?
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

; No `.data' section because the string also needs to be inside the first 512
; bytes, and we need to add the padding. Also note the position of the string
; inside the file. After the file is placed into 0x7C00, the BIOS will jump to
; the first instruction, so the entry point needs to be first.
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
