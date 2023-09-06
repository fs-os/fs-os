
%include "structs.asm"      ; ctx_t

section .bss
    global mt_current_task
    mt_current_task: resd 1

    first_ctx:              ; Reserve the bytes, filled in mt_init
        istruc ctx_t
            at ctx_t.next,   resd 1
            at ctx_t.prev,   resd 1
            at ctx_t.stack,  resd 1
            at ctx_t.esp,    resd 1
            at ctx_t.cr3,    resd 1
            at ctx_t.fxdata, resd 1
            at ctx_t.name,   resd 1
        iend

    ; 512 bytes needed by fxsave. Reserved here instead of heap.
    align 16
    first_fxdata: resb 512      ; For .fxdata member of first_ctx (kernel_main)

section .data
    first_task_name db 'kernel_main', 0x0

section .text
    extern stack_bottom         ; src/kernel/boot.asm
    extern memcpy:function      ; src/libk/string.c
    extern heap_alloc:function  ; src/kernel/heap.c
    extern heap_calloc:function ; src/kernel/heap.c
    extern free:function        ; src/libk/stdlib.c

; void mt_init(void);
; Initialize multitasking. Creates the first task for the kernel.
global mt_init:function
mt_init:
    push    ebp
    mov     ebp, esp

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

    ; 512 bytes aligned to 16 bytes in .bss for fxsave. See mt_newtask
    mov     [first_ctx + ctx_t.fxdata], dword first_fxdata

    ; "kernel_main"
    mov     [first_ctx + ctx_t.name],  dword first_task_name

    ; Address of the struct we just filled
    mov     [mt_current_task], dword first_ctx

    mov     esp, ebp
    pop     ebp
    ret

; Ctx* mt_newtask(const char* name, void* entry);
; Creates a new task named "name", and with the entry point "entry". Returns the
; ptr to the new task.
global mt_newtask:function
mt_newtask:
    push    ebp
    mov     ebp, esp

    ; NOTE: General register usage:
    ;   - eax: Pointer to the ctx_t we allocate at the start.
    ;   - ecx: Temporary register for values with short life.
    ;   - edx: Pointer to the allocated stack, or to the allocated fxdata.

    push    dword 8             ; Align to 8 bytes
    push    dword ctx_t_size    ; Size of the ctx_t struct
    call    heap_alloc          ; Allocate a ctx_t struct on the heap
    add     esp, 8              ; Remove 2 dwords we just pushed

    mov     ecx, [ebp + 8]          ; First argument
    mov     [eax + ctx_t.name], ecx ; Program name (char*), first arg

    mov     ecx, cr3
    mov     [eax + ctx_t.cr3], ecx  ; Use same CR3 as caller (parent)

    ; Insert new task next to the current one in the list.
    ;   1. Move the current task's address (edx) to the new task's (eax) "prev"
    ;      pointer.
    ;   2. Move the current task's "next" pointer (ecx) to the new task's "next"
    ;      pointer.
    ;   3. Overwrite the current task's "next" with the pointer of this new
    ;      task.
    ;   4. Overwrite the "prev" pointer of the current task's "next" (ecx) with
    ;      the new task's address (eax).
    ;   5. Restore ecx.
    ;
    ;      [cur_task] -> [new_task] -> [cur_task.next]
    ;         | ^         | ^  ^ |         ^ |
    ;         | |---(1)---| |  | |---(2)---| |
    ;         |-----(3)-----|  |-----(4)-----|
    ;
    mov     edx, [mt_current_task]          ; edx = &cur
    mov     ecx, [edx + ctx_t.next]         ; ecx = cur.next
    mov     [eax + ctx_t.prev], edx         ; new.prev = &cur
    mov     [eax + ctx_t.next], ecx         ; new.next = cur.next
    mov     [edx + ctx_t.next], eax         ; cur.next = &new
    mov     [ecx + ctx_t.prev], eax         ; cur.next.prev = &new

    push    eax             ; Preserve eax (allocated Ctx*)

    push    dword 16        ; Align to 16 bytes
    push    dword 0x4000    ; 16KiB stack for the new task
    call    heap_alloc
    mov     edx, eax        ; Save new stack address to edx
    add     esp, 8          ; Remove 2 dwords we just pushed

    pop     eax             ; Restore old Ctx* from first malloc

    ; Address of stack we just allocated. Store so we can free it in mt_endtask
    mov     [eax + ctx_t.stack], edx

    ; Now edx points to the end of the allocated memory, which is the bottom of
    ; the stack in x86 (pushed items are in lower addresses). We subtract 4 to
    ; not overflow. See issue #12
    add     edx, 0x4000 - 4

    ; Fill new allocated stack for new task. From bottom to top, needed by
    ; mt_switch and System V ABI:
    ;   - eip (return address of the task/entry point)
    ;   - edi
    ;   - esi
    ;   - ebp
    ;   - ebx
    mov     ecx, [ebp + 12]             ; tmp = second_arg;
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

    ; Allocate 512 bytes needed by the fxsave instruction for preserving the FPU
    ; and SSE registers
    push    eax

    push    dword 16                        ; Need to be 16-bit aligned
    push    dword 1                         ; sizeof(byte)
    push    dword 512                       ; 512 bytes for fxsave
    call    heap_calloc
    mov     edx, eax                        ; Save allocated bytes to edx
    add     esp, 12                         ; Remove 3 dwords we just pushed

    pop     eax
    mov     [eax + ctx_t.fxdata], edx       ; Save in ctx struct

    ; Initialize the fxdata we just allocated by:
    ;   - Setting edx.fcw to 0x037F (See Intel manual Vol. 1, Chapter 8.1.5)
    ;   - Masking bits 7..12 of the MXCSR register (Vol. 1, Figure 10-3)
    ;   - Setting the default MXCSR_MASK (Vol. 1, Chapter 11.6.6)
    mov     [edx + fpu_data_t.fcw],   dword 0b0000001101111111
    mov     [edx + fpu_data_t.mxcsr], dword 0b0001111110000000
    mov     [edx + fpu_data_t.mxcsr_mask], dword 0x0000FFFF

    ; Exit the mt_newtask function
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
    push    ebp     ; ebp and ebx are unused in mt_switch, we still need to save
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

    ; Save the SSE, MMX and FPU registers. fxsave requires a 16byte-aligned
    ; address to 512 bytes. Although this "dereference" looks weird, it's the
    ; same as lgdt (src/kernel/gdt.asm)
    mov     eax, [edi + ctx_t.fxdata]
    fxsave  [eax]

    ; Now we are done storing the current task, we can switch to the next task.
    ; We store the first argument in esi. We pushed 4 elements + 1 return
    ; address, of size 4 (dword). We set it as the current ctx.
    mov     esi, [esp + 5 * 4]
    mov     [mt_current_task], esi

    mov     eax, [esi + ctx_t.fxdata]   ; Restore SSE/MMX/FPU registers saved by
    fxrstor [eax]                       ; fxsave on ctx_t.fxdata

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

    ; Free the stack, fxdata and Ctx struct we allocated for the task. First we
    ; preserve eax in edx because the first "free" (for the stack) will overwrite it
    ; when returning.
    push    eax

    push    dword [eax + ctx_t.stack]   ; Address of the allocated stack
    call    free                        ; Free the task's stack
    add     esp, 4                      ; Remove stack dword we just pushed

    pop     eax                         ; Restore eax since it was overwritten
    mov     edx, [eax + ctx_t.fxdata]   ; Save fxdata address in edx
    push    eax                         ; Push eax (Ctx*) again

    push    edx                         ; Free 512 bytes of fxdata
    call    free
    add     esp, 4                      ; Remove fxdata dword we just pushed

    call    free        ; Free the Ctx struct itself, eax we pushed before
                        ; freeing fxdata

    mov     esp, ebp
    pop     ebp

    ret

; Ctx* mt_gettask(void);
; Returns a pointer to the current task context struct being used.
global mt_gettask:function
mt_gettask:
    mov     eax, [mt_current_task]  ; Dereference to get the pointer
    ret

; void mt_get_fpu_data(fpu_data_t* p);
; Fill fpu_data_t argument with data from fxsave
global mt_get_fpu_data:function
mt_get_fpu_data:
    push    ebx
    push    ebp
    mov     ebp, esp

    mov     ebx, [ebp + 12]     ; 1st arg = ebp + ebx + ret

    push    dword 16            ; Need to be 16-bit aligned
    push    dword 1             ; sizeof(byte)
    push    dword 512           ; 512 bytes for fxsave
    call    heap_calloc
    add     esp, 12             ; Remove 3 dwords we just pushed

    fxsave  [eax]               ; Fill 512 bytes we just allocated

    push    eax                 ; Preserve and use as parameter for free bellow

    push    dword fpu_data_t_size   ; Size
    push    eax                     ; Source
    push    ebx                     ; Destination
    call    memcpy
    add     esp, 12                 ; Remove 3 dwords we just pushed

    call    free                ; Free the 512 bytes we allocated for fxsave
    add     esp, 4              ; Remove eax (Pushed before call to memcpy)

    mov     esp, ebp
    pop     ebp
    pop     ebx
    ret
