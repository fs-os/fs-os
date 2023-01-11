
bits 32

section .data
    global load_idt:function
    global asm_cli:function
    global asm_sti:function

; void load_idt(void* idt_desc)
load_idt:
    push    ebp
    mov     ebp, esp

    lidt    [esp + 8]       ; First arg, pointer to the idt descriptor. (ebp (4) +
                            ; return addr (4))

    mov     esp, ebp
    pop     ebp
    ret

; void asm_cli(void)
asm_cli:
    cli
    ret

; void asm_sti(void)
asm_sti:
    sti
    ret

