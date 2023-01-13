
%macro EXC_WRAPPER 1
    global exc_%1
    exc_%1:
        push    %1                  ; Push the parameter
        call    handle_exception    ; Call the function from exceptions.c
        iretd                       ; Return doubleword interrupt (32bit)
%endmacro

section .text
    align 8

; src/kernel/exceptions.c
extern handle_exception


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

