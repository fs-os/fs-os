
bits 32

section .text
    global check_rdseed:function
    global check_rdrand:function
    global cpu_rand:function

; bool check_rdseed(void);
; Check if the CPU supports the "rdseed" instruction. For more information, see:
;   https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits
check_rdseed:
    push    ebx         ; Save the registers used by cpuid
    push    ecx
    push    edx

    mov     eax, 7      ; Request function 7
    mov     ecx, 0      ; Request subfunction 0
    cpuid
    shr     ebx, 18     ; (ebx >> 18) Acording to the CPUID table, bit 18 (19th)
                        ;             is 1 if the RDSEED instruction is
                        ;             supported by the CPU.
    and     ebx, 1      ; (ebx & 1) We only care about that 19th bit we just
                        ; shifted

    mov     eax, ebx    ; Save the value to eax for returning

    pop     edx         ; Restore the registers used by cpuid
    pop     ecx
    pop     ebx

    ret

; bool check_rdrand(void);
; Check if the CPU supports the "rdrand" instruction. For more information, see:
;   https://en.wikipedia.org/wiki/CPUID#EAX=7,_ECX=0:_Extended_Features
check_rdrand:
    push    ebx         ; Save the registers used by cpuid
    push    ecx
    push    edx

    mov     eax, 1      ; Request function 7
    mov     ecx, 0      ; Request subfunction 0
    cpuid
    shr     ecx, 30     ; (ecx >> 30) Acording to the CPUID table, bit 30 (31st)
                        ;             is 1 if the RDRAND instruction is
                        ;             supported by the CPU.
    and     ecx, 1      ; (ecx & 1) We only care about that 31st bit we just
                        ; shifted

    mov     eax, ecx    ; Save the value to eax for returning

    pop     edx         ; Restore the registers used by cpuid
    pop     ecx
    pop     ebx

    ret

; uint32_t cpu_rand(void);
; Returns a 32 bit random integer generated using rdseed or rdrand (fallback).
; Returns 0 if not supported.
cpu_rand:
    push ebx
    mov     ebx, 50         ; Max retries
    xor     eax, eax

.rdseed:
    call    check_rdseed
    jz      .rdrand

.rdseed_loop:
    rdseed  eax
    jc      .done           ; Carry flag is set, success. Return eax

    dec      ebx            ; ebx--;
    cmp      ebx, 0         ; if (ebx == 0)
    jz      .done           ;   break;         // Failed, return 0
    jmp     .rdseed_loop    ; continue;

.rdrand:                    ; Fallback
    call    check_rdrand
    jz      .done

.rdrand_loop:
    rdrand  eax
    jc      .done           ; Carry flag is set, success. Return eax

    dec      ebx            ; ebx--;
    cmp      ebx, 0         ; if (ebx == 0)
    jz      .done           ;   break;         // Failed, return 0
    jmp     .rdrand_loop    ; continue;

.done:
    pop ebx
    ret

