
%include "structs.asm"      ; ctx_t

section .bss
    global mt_current_task
    mt_current_task: resd 1

    first_ctx:              ; Reserve the bytes, filled in mt_init
        istruc ctx_t
            at ctx_t.next,  resd 1
            at ctx_t.prev,  resd 1
            at ctx_t.stack, resd 1
            at ctx_t.esp,   resd 1
            at ctx_t.cr3,   resd 1
            at ctx_t.state, resd 1
            at ctx_t.name,  resd 1
        iend
    ctx_t_end:

section .data
    first_task_name db 'kernel_main', 0x0
    ctx_t_sz dd ctx_t_end - first_ctx       ; TODO: use ctx_t_size

section .text
    extern stack_bottom         ; src/kernel/boot.asm
    extern malloc:function      ; src/libk/stdlib.c
    extern free:function        ; src/libk/stdlib.c

; void mt_init(void);
; Initialize multitasking. Creates the first task for the kernel.
global mt_init:function
mt_init:
    ; .next and .next of first task is itself.
    mov     [first_ctx + ctx_t.next], dword first_ctx
    mov     [first_ctx + ctx_t.prev], dword first_ctx

    ; Bottom of the main stack. Declared in src/kernel/boot.asm
    mov     [first_ctx + ctx_t.stack], dword stack_bottom

    ; Current esp
    mov     [first_ctx + ctx_t.esp],  dword esp

    ; Current cr3 (paging should be enabled)
    mov     eax, cr3
    mov     [first_ctx + ctx_t.cr3], eax

    ; TODO: State
    mov     [first_ctx + ctx_t.state], dword 0x00000000

    ; "kernel_main"
    mov     [first_ctx + ctx_t.name],  dword first_task_name

    ; Address of the struct we just filled
    mov     [mt_current_task], dword first_ctx

    ret

; Ctx* mt_newtask(const char* name, void* entry);
; Creates a new task named "name", and with the entry point "entry". Returns the
; ptr to the new task.
global mt_newtask:function
mt_newtask:
    push    ebp
    mov     ebp, esp

    push    dword [ctx_t_sz]    ; Size of the ctx_t struct
    call    malloc              ; Allocate a ctx_t struct on the heap
    add     esp, 4              ; Remove dword we just pushed

    mov     ecx, [esp + 12]     ; Second arg, entry point for task
    mov     edx, [esp + 8]      ; First arg, task name. edx will be overwritten
                                ; by new allocated stack.

    mov     [eax + ctx_t.name], edx ; Program name (char*), first arg
    mov     [eax + ctx_t.state], dword 0x00000000 ; TODO: State

    mov     edx, cr3
    mov     [eax + ctx_t.cr3], edx  ; TODO: For now save current cr3 for new
                                    ; tasks

    ; Insert new task next to the current one in the list.
    ; First, move the current task's address to the new task's "prev" pointer,
    ; then move the current task's "next" pointer to the new task's "next"
    ; pointer, and overwrite the current task's "next" with the pointer of this
    ; new task.
    mov     edx, [mt_current_task]
    mov     [eax + ctx_t.prev], edx         ; new.prev = &cur
    mov     edx, [edx + ctx_t.next]
    mov     [eax + ctx_t.next], edx         ; new.next = cur.next
    mov     [edx + ctx_t.next], eax         ; cur.next = &new

    push    eax             ; Push eax (allocated Ctx*) because of next malloc
    push    ecx             ; Push second arg because caller must preserve

    push    dword 16384     ; 16KiB stack for the new task
    call    malloc
    mov     edx, eax        ; Save new stack address to edx
    add     esp, 4          ; Remove dword we just pushed

    pop     ecx             ; Restore second arg
    pop     eax             ; Restore old Ctx* from first malloc

    mov     [eax + ctx_t.stack], edx    ; Address of stack we just allocated.
                                        ; Stored so we can free it once the task
                                        ; ends.

    add     edx, 16384          ; Now edx points to the end of the allocated
                                ; memory, which is the bottom of the stack in
                                ; x86 (pushed items are in lower addresses).

    ; Fill new allocated stack for new task. From bottom to top, needed by
    ; mt_switch and System V ABI:
    ;   - eip (return address of the task/entry point)
    ;   - edi
    ;   - esi
    ;   - ebp
    ;   - ebx
    mov     [edx], ecx                  ; *stack_ptr = eip;   // Entry point
    sub     edx, 4                      ; stack_ptr--;
    mov     [edx], dword 0x00000000     ; *stack_ptr = edi;
    sub     edx, 4                      ; stack_ptr--;
    mov     [edx], dword 0x00000000     ; *stack_ptr = esi;
    sub     edx, 4                      ; stack_ptr--;
    mov     [edx], dword 0x00000000     ; *stack_ptr = ebp;
    sub     edx, 4                      ; stack_ptr--;
    mov     [edx], dword 0x00000000     ; *stack_ptr = ebx;

    mov     [eax + ctx_t.esp], edx      ; Save the address at the top of the
                                        ; allocated stack

    mov     esp, ebp
    pop     ebp

    ret

; void mt_switch(Ctx* next);
; Switch to task "next".
global mt_switch:function
mt_switch:
    cli             ; Clear interrupts

    push    edi     ; edi will be the current task
    push    esi     ; esi will be the first argument (new ctx)
    push    ebp     ; ebp and ebx are unused in mt_swtich, we still need to save
    push    ebx     ; them because the called function should preserve them
                    ; acording to the System V ABI.

    ; Dereference the label to get the address stored in the pointer:
    ;   mt_current_task     = &mt_current_task = 0x10050 (addr of label)
    ;   [mt_current_task]   = mt_current_task  = 0x51AB3 (addr of struct)
    ;   [[mt_current_task]] = *mt_current_task = struct[0] (dereference ptr)
    ; edi is now a void* pointing to the current Ctx struct. Because we are
    ; switching tasks, we need to store the current esp in the old task's ctx.
    ; We dereference edi and save esp in (*mt_current_stack).esp
    mov     edi, [mt_current_task]
    mov     [edi + ctx_t.esp], esp

    mov     esi, [esp + 5 * 4]      ; First argument. We pushed 4 elements +
                                    ; return address, of size 4 (dword).
    mov     [mt_current_task], esi  ; Save the function argument as the current
                                    ; ctx

    mov     esp, [esi + ctx_t.esp]  ; Load all fields from next task
    mov     eax, [esi + ctx_t.cr3]  ; Save new cr3 to eax for comparing
    mov     ecx, cr3                ; Save old cr3 to ecx for comparing
    ; TODO: tss.esp0

    cmp     eax, ecx                ; If new and old cr3 match, don't load
    je      .pd_loaded
    mov     cr3, eax                ; If the don't match, load cr3 from new task

.pd_loaded:
    pop     ebx     ; Restore registers from caller function
    pop     ebp
    pop     esi
    pop     edi

    sti             ; Enable interrupts again

    ret

; void mt_endtask(Ctx* task);
; Frees the stack and ends the task passed as parameter. The task should not be
; the current working task.
global mt_endtask:function
mt_endtask:
    push    ebp
    mov     ebp, esp

    mov     eax, [esp + 8]              ; eax is the first arg, Ctx*

    ; First, remove the Ctx* from the linked list. Make the previous item's
    ; "next" point to the "next" pointer of the task we want to free, and then
    ; make the next item's "prev" point to the "prev" pointer of the task we
    ; want to free:
    ;
    ; [ prev task ] -> [ task to free ] -> [ next task ]
    ;      ^   |                              ^   |
    ;      |   |-------------(1st)------------|   |
    ;      |-----------------(2nd)----------------|

    mov     ecx, [eax + ctx_t.prev]     ; ecx is the previous task in the list
    mov     edx, [eax + ctx_t.next]     ; edx is the next task in the list

    mov     [ecx + ctx_t.next], edx     ; arg->prev->next = arg->next
    mov     [edx + ctx_t.prev], ecx     ; arg->next->prev = arg->prev

    ; Free the stack we allocated for the task, and then free the Ctx struct as
    ; well

    push    eax         ; Preserve eax and push it to use it as arg for the 2nd
                        ; free call (first free the stack, then free the Ctx
                        ; struct)

    push    dword [eax + ctx_t.stack]   ; Address of the allocated stack
    call    free                        ; Free the task's stack
    add     esp, 4                      ; Remove dword we just pushed

    call    free                        ; Free the Ctx struct itself, function
                                        ; arg we pushed before freeing the stack.

    mov     esp, ebp
    pop     ebp

    ret

; Ctx* mt_gettask(void);
; Returns a pointer to the current task context struct being used.
global mt_gettask:function
mt_gettask:
    mov     eax, [mt_current_task]  ; Dereference to get the pointer
    ret

