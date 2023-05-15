section .text
    global sqrt, sin, cos, tan, cot

sqrt:
    push ebp
    mov ebp, esp
    sub esp, 8

    movsd xmm0, qword [ebp + 8]
    sqrtsd xmm0, xmm0
    movsd qword [ebp - 8], xmm0
    fld qword [ebp - 8]

    leave
    ret

sin:
    push ebp
    mov ebp, esp
    sub esp, 8

    fld qword [ebp + 8]
    fsin
    fstp qword [ebp - 8]
    fld qword [ebp - 8]

    leave
    ret

cos:
    push ebp
    mov ebp, esp
    sub esp, 8

    fld qword [ebp + 8]
    fcos
    fstp qword [ebp + 8]
    fld qword [ebp - 8]

    leave
    ret

tan:
    push ebp
    mov ebp, esp
    sub esp, 8

    fld qword [ebp + 8]
    fsincos
    fdivp st1, st0
    fstp qword [ebp + 8]

    leave
    ret

cot:
    push ebp
    mov ebp, esp
    sub esp, 8

    fld qword [ebp + 8]
    fsincos
    fdivrp st1, st0
    fstp qword [ebp + 8]

    leave
    ret
