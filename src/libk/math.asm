section .text

; double sqrt(double x);
global fabs:function
fabs:
    push    ebp
    mov     ebp, esp
    sub     esp, 8                      ; Reserve 8 bytes on the stack

    emms                                ; Reset FPU. See Vol.1, Chapter 9.4.8
    fld     qword [ebp + 8]             ; Push to FPU stack
    fabs                                ; st(0) = fabs(st(0));

    mov     esp, ebp
    pop     ebp
    ret                                 ; Return double in st0, acording to ABI

; double sqrt(double x);
global sqrt:function
sqrt:
    push    ebp
    mov     ebp, esp
    sub     esp, 8                      ; Reserve 8 bytes on the stack

    emms                                ; Reset FPU. See Vol.1, Chapter 9.4.8
    movsd   xmm0, qword [ebp + 8]       ; Move double argument to xmm0
    sqrtsd  xmm0, xmm0                  ; xmm0 = sqrt(xmm0);
    movsd   qword [ebp - 8], xmm0       ; Move xmm0 to reserved bytes
    fld     qword [ebp - 8]             ; Push reserved bytes to FPU stack

    mov     esp, ebp
    pop     ebp
    ret

; double sin(double x);
global sin:function
sin:
    push    ebp
    mov     ebp, esp
    sub     esp, 8

    emms
    fld     qword [ebp + 8]
    fsin                                ; st(0) = sin(st(0));

    mov     esp, ebp
    pop     ebp
    ret

; double cos(double x);
global cos:function
cos:
    push    ebp
    mov     ebp, esp
    sub     esp, 8

    emms
    fld     qword [ebp + 8]
    fcos                                ; st(0) = cos(st(0));

    mov     esp, ebp
    pop     ebp
    ret

; double tan(double x);
global tan:function
tan:
    push    ebp
    mov     ebp, esp
    sub     esp, 8

    emms
    fld     qword [ebp + 8]
    fsincos                             ; st1 = cos(st0); st0 = sin(st0);
    fdivp   st1, st0                    ; st1 /= st0; (sin/cos)

    mov     esp, ebp
    pop     ebp
    ret

; double cot(double x);
global cot:function
cot:
    push    ebp
    mov     ebp, esp
    sub     esp, 8

    emms
    fld     qword [ebp + 8]
    fsincos                             ; st1 = cos(st0); st0 = sin(st0);
    fdivrp  st1, st0                    ; st0 = st1 / st0; (cos/sin)

    mov     esp, ebp
    pop     ebp
    ret
