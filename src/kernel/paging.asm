
section .text

; void paging_load(uint32_t* page_dir);
global paging_load:function
paging_load:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp + 8]      ; First arg: page dir ptr
    mov     cr3, eax            ; Move to cr3

    mov     esp, ebp
    pop     ebp
    ret

; void paging_enable(void);
global paging_enable:function
paging_enable:
    push    ebp
    mov     ebp, esp

    ; Enable paging, see Vol. 3, Chapter 4.3 and Vol.3, Chapter 2.5
    mov     eax, cr0            ; We can't work directly with cr0
    or      eax, (1 << 31)      ; Set paging flag (bit 31):
    or      eax, (1 << 0)       ; Set protected mode flag (bit 1). Should be on.
    mov     cr0, eax            ; And move back to cr0

    mov     esp, ebp
    pop     ebp
    ret

