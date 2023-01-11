
bits 32

section .data
    global idt_load:function
    global asm_cli:function
    global asm_sti:function

; void idt_load(void* idt_desc)
idt_load:
    push    eax

    mov     eax, [esp + 8]      ; First arg, pointer to the idt descriptor. (pushed
                                ; eax (4) + return addr (4))
    lidt    [eax]

    pop     eax
    ret

; void asm_cli(void)
asm_cli:
    cli
    ret

; void asm_sti(void)
asm_sti:
    sti
    ret
