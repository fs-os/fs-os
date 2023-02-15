
%include "structs.asm"      ; ctx_t

section .data
    first_task_name: db 'kernel_main', 0x0

section .bss
    global mt_current_task
    mt_current_task: resd 1

    first_ctx:              ; Reserve the bytes, filled in mt_init
        istruc ctx_t
            at ctx_t.next,  resd 1
            at ctx_t.esp,   resd 1
            at ctx_t.cr3,   resd 1
            at ctx_t.state, resd 1
            at ctx_t.name,  resd 1
        iend

section .text

; void mt_init(void);
; Initialize multitasking. Creates the first task for the kernel.
global mt_init:function
mt_init:
    mov     [first_ctx + ctx_t.next],   dword mt_init    ; .next of first task is
                                                         ; itself
    mov     [first_ctx + ctx_t.esp],    dword esp        ; Current esp

    ; Current cr3 (paging should be enabled)
    mov     eax, cr3
    mov     [first_ctx + ctx_t.cr3],    eax

    mov     [first_ctx + ctx_t.state],  dword 0x00000000 ; TODO
    mov     [first_ctx + ctx_t.name],   dword first_task_name  ; "kernel_main"

    mov     [mt_current_task], dword first_ctx  ; Address of the struct we just
                                                ; filled

    ret

; void mt_switch(Ctx* next);
; Switch to task "next".
global mt_switch:function
mt_switch:
    cli             ; Clear interrupts

    push    edi
    push    esi

    ; Dereference the label to get the address stored in the pointer:
    ;   mt_current_task     = &mt_current_task = 0x10050 (addr of label)
    ;   [mt_current_task]   = mt_current_task  = 0x51AB3 (addr of struct)
    ;   [[mt_current_task]] = *mt_current_task = struct[0] (dereference ptr)
    ; edi is now a void* pointing to the current Ctx struct. Because we are switching
    ; tasks, we need to store the current esp in the old task's ctx. We dereference
    ; edi and save esp in (*mt_current_stack).esp
    mov     edi, [mt_current_task]
    mov     [edi + ctx_t.esp], esp

    mov     esi, [esp + 3 * 4]      ; First argument. We pushed 2 elements + return
                                    ; address, of size 4 (dword).
    mov     [mt_current_task], esi  ; Save the function argument as the current ctx

    mov     esp, [esi + ctx_t.esp]  ; Load all fields from next task
    mov     eax, [esi + ctx_t.cr3]  ; Save new cr3 to eax for comparing
    mov     ecx, cr3                ; Save old cr3 to eax for comparing
    ; TODO: tss.esp0

    cmp     eax, ecx                ; If new and old cr3 match, don't load
    je      .pd_loaded
    mov     cr3, eax                ; If the don't match, load cr3 from new task

.pd_loaded:
    pop     esi
    pop     edi

    sti                             ; Enable interrupts again

    ret                             ; Return

