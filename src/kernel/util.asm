
; EDX argument for RDMSR and WRMSR. See Vol. 4, search for "IA32_DEBUGCTL".
%define IA32_DEBUGCTL 0x1D9

section .data
    title_fmt  db "Dumping %ld items of size %ld from stack: {", 0xA, 0x0
    hex_fmt    db "  0x%lX", 0xA, 0x0
    close_br   db "}", 0xA, 0x0

section .text
    extern printf:function

; void asm_dump_stack(uint32_t count, size_t size);
; Function prototype in kernel/util.h; pseudo:
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

; void asm_enable_debug(uint32_t on_branch);
; NOTE: This is not really usable since the ISR calls itself. See util.h
global asm_enable_debug:function
asm_enable_debug:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp + 8]
    test    eax, eax            ; if (!on_branch)
    jz      .enable_trapflag    ;   skip_msr_bits();

    ; If `on_branch` is not zero, enable branch/interrupt/exception traces and
    ; breakpoints on branch change. See:
    ;   - Single step on branches: Vol. 3, Chapter 17.4
    mov     ecx, IA32_DEBUGCTL      ; Argument for RDMSR and WRMSR: 0x1D9
    rdmsr                   ; Read MSR into EDX:EAX. See Vol. 3, Chapter 17.4.1
    or      eax, 1 << 0     ; LBR (last branch/interrupt/exception) flag
    or      eax, 1 << 1     ; BTF (single-step on branches) flag
    wrmsr                   ; Write changed MSR register

.enable_trapflag:
    ; Enable the Trap Flag (TF) bit in EFLAGS. See:
    ;   - pushfd and popfd: Vol. 1 Chapter 5.1.11
    ;   - Trap Flag: Vol. 1, Chapter 3.4.3.3 (and figure 3-8)
    pushfd                          ; Push EFLAGS to the stack
    or      dword [esp], 1 << 8     ; Set Trap Flag bit in EFLAGS
    popfd                           ; Pop into EFLAGS from the stack

    mov     esp, ebp
    pop     ebp
    ret

; void asm_disable_debug(uint32_t on_branch);
global asm_disable_debug:function
asm_disable_debug:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp + 8]
    test    eax, eax                    ; if (!on_branch)
    jz      .disable_trapflag           ;   skip_msr_bits();

    mov     ecx, IA32_DEBUGCTL          ; Argument for RDMSR and WRMSR: 0x1D9
    rdmsr                   ; Read MSR into EDX:EAX. See Vol. 3, Chapter 17.4.1
    and     eax, ~(1 << 0)  ; Unset LBR (last branch/interrupt/exception) flag
    and     eax, ~(1 << 1)  ; Unset BTF (single-step on branches) flag
    wrmsr                   ; Write changed MSR register

.disable_trapflag:
    pushfd                              ; Push EFLAGS to the stack
    and     dword [esp], ~(1 << 8)      ; Unset Trap Flag bit in EFLAGS
    popfd                               ; Pop into EFLAGS from the stack

    mov     esp, ebp
    pop     ebp
    ret

; bool is_sse_supported(void);
global is_sse_supported:function
is_sse_supported:
    push    ebx                 ; Store registers used by CPUID (Except return)
    push    ecx
    push    edx

    ; Check for SSE1
    mov     eax, 0x1            ; Request function 1 of CPUID
    cpuid
    test    edx, 1 << 25        ; CPUID.1:EDX.SSE[bit 25] == 1?
    jnz     .check_sse2
    mov     eax, 0
    jmp     .done

.check_sse2:
    ; Check for SSE2
    test    edx, 1 << 26        ; CPUID.1:EDX.SSE[bit 26] == 1?
    jnz     .enabled            ; If 1, return true
    mov     eax, 0              ; Not enabled, return false
    jmp     .done

.enabled:
    mov     eax, 1              ; Return true

.done:
    pop     edx                 ; Restore registers used by CPUID
    pop     ecx
    pop     ebx
    ret

; bool is_msr_supported(void);
global is_msr_supported:function
is_msr_supported:
    push    ebx                 ; Store registers used by CPUID (Except return)
    push    ecx
    push    edx

    ; See Vol.2, WRMSR description
    mov     eax, 0x1            ; Request function 1 of CPUID
    cpuid
    test    edx, 1 << 5         ; CPUID.1:EDX.MSR[bit 5] == 1?
    jnz     .enabled            ; If 1, return true
    mov     eax, 0              ; Not enabled, return false
    jmp     .done

.enabled:
    mov     eax, 1

.done:
    pop     edx                 ; Restore registers used by CPUID
    pop     ecx
    pop     ebx
    ret

; bool is_tsc_supported(void);
global is_tsc_supported:function
is_tsc_supported:
    push    ebx                 ; Store registers used by CPUID (Except return)
    push    ecx
    push    edx

    ; See Vol.3, Chapter 17.17
    mov     eax, 0x1            ; Request function 1 of CPUID
    cpuid
    test    edx, 1 << 4         ; CPUID.1:EDX.TSC[bit 4] == 1?
    jnz     .enabled            ; If 1, return true
    mov     eax, 0              ; Not enabled, return false
    jmp     .done

.enabled:
    mov     eax, 1

.done:
    pop     edx                 ; Restore registers used by CPUID
    pop     ecx
    pop     ebx
    ret
