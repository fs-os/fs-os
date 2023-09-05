

section .data
    title_fmt  db "Dumping %ld items of size %ld from stack: {", 0xA, 0x0
    hex_fmt    db "  0x%lX", 0xA, 0x0
    close_br   db "}", 0xA, 0x0

section .text
    extern printf:function

; void asm_dump_stack(uint32_t count, size_t size);
; Prints "count" elements of size "size" from the stack in hex format. Pseudo:
;   asm_dump_stack(N, S):
;       eax_total = N * S;
;       edx_size  = S;
;       ecx_iter = 20;
;       while (eax_total != 0):
;           print(stack[ecx_iter]);
;           ecx_iter  += edx_size;
;           eax_total -= edx_size;
global asm_dump_stack:function
dump_stack:
    push    ebp                 ; Save stack frame
    mov     ebp, esp

    mov     edx, [ebp + 12]     ; Second arg, size of elements
    mov     eax, [ebp + 8]      ; Size of total elements to print:
    mul     edx                 ;   first_arg (eax) *= second_arg (esp + 12)

    mov     edx, [ebp + 12]     ; Second arg, size of elements

    push    eax                 ; Registers that should be preserved by the caller
    push    edx

    push    dword [ebp + 12]
    push    dword [ebp + 8]
    push    dword title_fmt
    call    printf
    add     esp, 12             ; Remove 3 dwords we just pushed

    pop     edx                 ; Restore caller registers
    pop     eax

    mov     ecx, 16             ; Real start of stack:
                                ;   ebp + ret_addr + first_arg + second_arg
.loop:
    test    eax, eax            ; if (!eax)  // We have no bytes left to print
    jz      .done               ;   break;

    push    eax                 ; Registers that should be preserved by the caller
    push    ecx                 ; We use +12 bellow because we push 3 dwords
    push    edx

    push    dword [esp + ecx + 12]      ; printf(" 0x%lX\n", stack[ecx]);
    push    dword hex_fmt
    call    printf
    add     esp, 8                      ; Remove 2 dwords from printf

    pop     edx                 ; Restore caller registers
    pop     ecx
    pop     eax

    add     ecx, edx            ; ecx += size;  // stack pos, 2nd arg
    sub     eax, edx            ; eax -= size;  // bytes left to print, 2nd arg

    jmp     .loop               ; continue;

.done:
    push    dword close_br
    call    printf
    add     esp, 4

    mov     esp, ebp        ; Restore stack frame
    pop     ebp

    xor     eax, eax        ; Return 0
    ret

