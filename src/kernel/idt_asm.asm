
%macro EXC_WRAPPER 1
    global exc_%1:function
    exc_%1:
        push    %1                  ; Push the parameter
        call    handle_exception    ; Call the function from exceptions.c
        iretd                       ; Return doubleword interrupt (32bit)
%endmacro

bits 32

section .text
    align 8
    extern handle_exception     ; src/kernel/exceptions.c
    extern pit_dec              ; src/kernel/idt.c
    extern kb_handler           ; src/kernel/keyboard.c

; void idt_load(void* idt_desc)
global idt_load:function
idt_load:
    push    eax

    mov     eax, [esp + 8]      ; First arg, pointer to the idt descriptor. (pushed
                                ; eax (4) + return addr (4))
    lidt    [eax]

    pop     eax
    ret

; exc_X: call the exception handler with the specified IRQ. Used as ISR offsets for
; the idt.
EXC_WRAPPER 0
EXC_WRAPPER 1
EXC_WRAPPER 2
EXC_WRAPPER 3
EXC_WRAPPER 4
EXC_WRAPPER 5
EXC_WRAPPER 6
EXC_WRAPPER 7
EXC_WRAPPER 8
EXC_WRAPPER 10
EXC_WRAPPER 11
EXC_WRAPPER 12
EXC_WRAPPER 13
EXC_WRAPPER 14
EXC_WRAPPER 15
EXC_WRAPPER 16
EXC_WRAPPER 17
EXC_WRAPPER 18
EXC_WRAPPER 19
EXC_WRAPPER 20
EXC_WRAPPER 30

; void irq_pit(void)
; First IRQ we remapped to 0x20
global irq_pit:function
irq_pit:
    pusha
    call    pit_dec     ; Decrement the static counter from src/kernel/idt.c
    popa
    iretd

global irq_kb:function
irq_kb:
    pusha
    cld                 ; Clear direction flag. The flag is used to specify the
                        ; string order for operations that use the 'edi' and 'esi'
                        ; registers. Tells the CPU that it should increase or
                        ; decrease the pointer for strings.
    call    kb_handler  ; src/kernel/keyboard.c
    popa
    iretd

; void irq_default_master(void)
; Ignore all IRQs we didn't add from master PIC
global irq_default_master:function
irq_default_master:
    mov     al, 0x20
	out     0x20, al
    iretd

; void irq_default_slave(void)
; Ignore all IRQs we didn't add from slave PIC
global irq_default_slave:function
irq_default_slave:
	mov     al, 0x20
	out     0xa0, al
	out     0x20, al
    iretd

