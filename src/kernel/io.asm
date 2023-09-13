; I/O port functions.
; For more info on registers, see:
;   https://www.felixcloutier.com/x86/in

section .text:

; uint8_t io_inb(uint16_t port)
global io_inb
io_inb:
    push    ebp
    mov     ebp, esp

    ; First arg, port (uint16_t)
    ; [esp + 8] -> esp + ebp we just pushed (4b) + return address (4b) = first
    ;              arg of the function, last pushed to the stack by the caller
    mov     edx, [esp + 8]

    ; Copy to al because we want the byte
    xor     eax, eax
    in      al, dx

    pop     ebp
    ret

; uint32_t io_inl(uint16_t port)
global io_inl
io_inl:
    push    ebp
    mov     ebp, esp

    ; First arg, port (uint16_t)
    mov     edx, [esp + 8]

    ; Copy to eax because we want the dword
    in      eax, dx

    pop     ebp
    ret

; void io_outb(uint16_t port, uint8_t data)
global io_outb
io_outb:
    push    ebp
    mov     ebp, esp

    ; First arg, port (uint16_t)
    mov     edx, [esp + 8]

    ; Second arg, data (uint8_t)
    ; [esp + 12] -> first arg + second arg (4 bytes even if the arg is 16 bits)
    mov     eax, [esp + 12]

    ; Copy from al because we want the byte
    out     dx, al

    pop     ebp
    ret

; void io_outl(uint16_t port, uint32_t data)
global io_outl
io_outl:
    push    ebp
    mov     ebp, esp

    ; First arg, port (uint16_t)
    mov     edx, [esp + 8]

    ; [esp + 12] -> first arg + second arg (4 bytes even if the arg is 16 bits)
    mov     eax, [esp + 12]

    ; Copy from eax because we want the dword
    out     dx, eax

    pop     ebp
    ret

