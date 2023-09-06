
%macro EXC_WRAPPER 1
    global exc_%1:function
    exc_%1:
        cli                         ; Clear interrupts

        ; The CPU pushes some values to the stack before calling the ISR for the
        ; exception that just ocurred. Since it's already the last pushed value,
        ; we can simply push the first parameter of handle_exception (the
        ; exception ID). For more info, see:
        ;   https://wiki.osdev.org/Interrupt_Service_Routines#x86
        push    %1
        call    handle_exception    ; Call the function from exceptions.c
        add     esp, 4              ; Remove dword we just pushed from stack

        sti                         ; Re-enable interrupts
        iretd                       ; Return from 32 bit interrupt
%endmacro

; We need to different macros because some exceptions push an extra "Error code"
; value to the stack. For more info, see: https://wiki.osdev.org/Exceptions
%macro EXC_WRAPPER_ERR 1
    global exc_%1:function
    exc_%1:
        cli                         ; Clear interrupts

        ; In this macro, we need to add 4 to ESP to "pop" the error code that
        ; the CPU pushed before calling the ISR.
        add     esp, 4

        ; Now the stack is pointing to the EIP value (2nd arg of
        ; handle_exception). We can push the first argument and call it.
        push    %1
        call    handle_exception    ; Call the function from exceptions.c
        add     esp, 4              ; Remove dword we just pushed from stack

        sti                         ; Re-enable interrupts
        iretd                       ; Return from 32 bit interrupt
%endmacro

;-------------------------------------------------------------------------------

bits 32
section .bss
    ; This is how GCC stores 64 bit integers in 32 bits. See:
    ; https://github.com/8dcc/c-stuff/blob/main/Misc/64bit-vars-in-32bit-pc.c
    align 16                    ; Align to 16 bytes for SSE
    tsc_lo: resd 1              ; uint64_t tsc used in exc_debug() bellow
    tsc_hi: resd 1

section .text
    align 8
    extern handle_exception     ; src/kernel/exceptions.c
    extern handle_debug         ; src/kernel/exceptions.c
    extern pit_inc              ; src/kernel/idt.c
    extern kb_handler           ; src/kernel/keyboard.c

; void idt_load(void* idt_desc)
global idt_load:function
idt_load:
    push    eax

    mov     eax, [esp + 8]      ; First arg, pointer to the idt descriptor.
                                ; (pushed eax (4) + return addr (4))
    lidt    [eax]

    pop     eax
    ret

; exc_X: call the exception handler with the specified IRQ. Used as ISR offsets
; for the idt. We use different macros depending on the exception number because
; the CPU pushes an extra error code argument before calling the ISR.
EXC_WRAPPER     0
; EXC_DEBUG is managed bellow
EXC_WRAPPER     2
EXC_WRAPPER     3
EXC_WRAPPER     4
EXC_WRAPPER     5
EXC_WRAPPER     6
EXC_WRAPPER     7
EXC_WRAPPER_ERR 8
EXC_WRAPPER_ERR 10
EXC_WRAPPER_ERR 11
EXC_WRAPPER_ERR 12
EXC_WRAPPER_ERR 13
EXC_WRAPPER_ERR 14
EXC_WRAPPER     15  ; Reserved
EXC_WRAPPER     16
EXC_WRAPPER_ERR 17
EXC_WRAPPER     18
EXC_WRAPPER     19
EXC_WRAPPER     20
EXC_WRAPPER_ERR 30

global exc_debug:function
exc_debug:
    cli                         ; Clear interrupts

    ; We need to disable the Trap Flag so we don't debug the next print and call
    ; ourselves. See asm_enable_debug in src/kernel/util.asm
    ; NOTE: We don't re-enable it before returning to not call ourselves again
    pushfd                              ; Push EFLAGS to the stack
    and     dword [esp], ~(1 << 8)      ; Unset Trap Flag bit in EFLAGS
    popfd                               ; Pop into EFLAGS from the stack

%ifndef DEBUG
    push    dword 0x00000000    ; Pass NULL if !DEBUG
%else
    ; Read Time-Stamp Counter (TSC) to EDX:EAX
    rdtsc

    mov     dword [tsc_lo], eax ; Store the lower part of first argument, TSC
    mov     dword [tsc_hi], edx ; Store the upper part of uint64_t
    push    tsc_lo              ; Push first argument, pointer to uint64_t
%endif ; DEBUG

    ; The CPU pushed EIP (2nd arg of handle_debug) before calling the ISR, and
    ; we just pushed the tsc ptr. We need to pass a ptr for SSE alignment.
    call    handle_debug        ; Call C function with 2 args
    add     esp, 4              ; Remove dword we just pushed

    sti                         ; Re-enable interrupts
    iretd                       ; Return from 32 bit interrupt

; void irq_pit(void)
; First IRQ we remapped to 0x20. Calls the pit_inc C function, located in:
; src/kernel/pit.c
global irq_pit:function
irq_pit:
    pusha
    call    pit_inc     ; Increment the static counter from src/kernel/idt.c
    popa
    iretd

; void irq_kb(void)
; Second IRQ we remapped to 0x21. Calls the kb_handler C function, located in:
; src/kernel/keyboard.c
global irq_kb:function
irq_kb:
    pusha
    cld                 ; Clear direction flag. The flag is used to specify the
                        ; string order for operations that use the 'edi' and
                        ; 'esi' registers. Tells the CPU that it should increase
                        ; or decrease the pointer for strings.
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

