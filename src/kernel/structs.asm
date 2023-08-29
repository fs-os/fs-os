
; Included by:
;   src/kernel/gdt.asm
;   src/kernel/multitask.asm

%ifndef STRUCTS_ASM
%define STRUCTS_ASM

struc tss_t
    .link:      resw 1
    .pad0:      resw 1

    .esp0:      resd 1
    .ss0:       resw 1
    .pad1:      resw 1
    .esp1:      resd 1
    .ss1:       resw 1
    .pad2:      resw 1
    .esp2:      resd 1
    .ss2:       resw 1
    .pad3:      resw 1

    .cr3:       resd 1
    .eip:       resd 1
    .eflags:    resd 1

    .eax:       resd 1
    .ecx:       resd 1
    .edx:       resd 1
    .ebx:       resd 1
    .esp:       resd 1
    .ebp:       resd 1
    .esi:       resd 1
    .edi:       resd 1

    .es:        resw 1
    .pad4:      resw 1
    .cs:        resw 1
    .pad5:      resw 1
    .ss:        resw 1
    .pad6:      resw 1
    .ds:        resw 1
    .pad7:      resw 1
    .fs:        resw 1
    .pad8:      resw 1
    .gs:        resw 1
    .pad9:      resw 1

    .ldtr:      resw 1
    .pad10:     resw 1
    .pad11:     resw 1
    .iobp:      resw 1
    .ssp:       resd 1
endstruc

; Used for overwritting the tss entry of the gdt
struc gdt_entry_t
    .limit0:    resw 1          ; First 16 bits of limit
    .base0:     resw 1          ; First 16 bits of base
    .base1:     resb 1          ; Mid 8 bits of base
    .flags:     resb 1          ; ppt + type flags
    .limit1:    resb 1          ; other flags (none) + last 4 bits of limit
    .base2:     resb 1          ; Last 8 bits of base
endstruc

; See src/kernel/include/kernel/multitask.h
struc ctx_t
    .next:      resd 1          ; Pointer to next task
    .prev:      resd 1          ; Pointer to previous task
    .stack:     resd 1          ; Address of the allocated stack. Used for free()
    .esp:       resd 1          ; Top of the current task's stack
    .cr3:       resd 1          ; cr3 register for the current task (virtual
                                ; address space/page directory)
    .fxdata:    resd 1          ; 512 bytes needed for fxsave to store fpu/sse
                                ; registers
    .name:      resd 1          ; char* to the task name
endstruc

%endif ; STRUCTS_ASM
