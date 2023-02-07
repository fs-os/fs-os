
section .text
    global load_page_dir:function
    global enable_paging:function

; void load_page_dir(uint32_t* page_dir);
load_page_dir:
    push    ebp
    mov     ebp, esp

    mov     eax, [esp + 8]      ; First arg: page dir ptr
    mov     cr3, eax            ; Move to cr3

    mov     esp, ebp
    pop     ebp
    ret

; void enable_paging(void);
enable_paging:
    push    ebp
    mov     ebp, esp

    mov     eax, cr0            ; We can't work directly with cr0
    or      eax, 0x80000000     ; Set the bit in eax
    mov     cr0, eax            ; And move back to cr0

    mov     esp, ebp
    pop     ebp
    ret

