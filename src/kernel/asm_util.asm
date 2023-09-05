

section .data
    open_br  db '{', 0x0                ; No newline because we will call puts
    hex_fmt  db '  0x%lX', 0xA, 0x0
    close_br db '}', 0x0

section .text
    extern putchar:function
    extern printf:function
    extern puts:function

; void dump_stack(uint32_t count, uint32_t size);
; Prints "count" elements of size "size" from the stack in hex format.
global dump_stack:function
dump_stack:
    push    ebx                 ; Used for popping stuff we dont need

    push    ebp                 ; Save stack frame
    mov     ebp, esp

    mov     ecx, 20             ; Real start of stack: 
                                ;   ebx + ebp + ret_addr + first_arg + second_arg
    mov     edx, [esp + 16]     ; Second arg, size of elements
    mov     eax, [esp + 12]     ; Size of total elements to print:
    mul     edx                 ;   first_arg (eax) *= second_arg (esp + 16)
                                ; If we want to print 6 elements of 4 bytes, size is
                                ; 24 bytes and for each element printed we will
                                ; subtract 4 until we have no size left
    mov     edx, [esp + 16]     ; Second arg, size of elements

    push    eax                 ; Registers that should be preserved by the caller
    push    ecx
    push    edx

    push    dword open_br
    call    puts
    add     esp, 4              ; Remove char* we just pushed

    pop     edx                 ; Restore caller registers
    pop     ecx
    pop     eax

.loop:
    cmp     eax, 0              ; if (!eax)     // We have no bytes left to print
    jz      .done               ;   break;

    push    eax                 ; Registers that should be preserved by the caller
    push    ecx                 ; +12 because we push 3 dwords
    push    edx

    push    dword [esp + ecx + 12]   ; printf(" 0x%lX\n", stack[ecx]);
    push    dword hex_fmt
    call    printf
    pop     ebx                 ; Pop args of printf
    pop     ebx

    pop     edx                 ; Restore caller registers
    pop     ecx
    pop     eax

    add     ecx, edx            ; ecx += size;  // stack pos, 2nd arg
    sub     eax, edx            ; eax -= size;  // bytes left to print, 2nd arg

    jmp     .loop               ; continue;

.done:
    push    eax                 ; Registers that should be preserved by the caller
    push    ecx
    push    edx
    push    dword close_br
    call    puts
    pop     ebx
    pop     edx                 ; Restore caller registers
    pop     ecx
    pop     eax

    mov     esp, ebp        ; Restore stack frame
    pop     ebp

    pop     ebx

    mov     eax, 0
    ret

