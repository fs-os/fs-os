section .text
    global sqrt, sin, cos, tan, cot

sqrt:
    push ebp
    mov ebp, esp

    movsd xmm0, qword [ebp + 8]
    sqrtsd xmm0, xmm0
    movsd qword [ebp + 8], xmm0

    pop ebp
    ret

sin:
    push ebp
    mov ebp, esp

    fld qword [ebp + 8]
    fsin
    fstp qword [ebp + 8]

    pop ebp
    ret

cos:
    push ebp
    mov ebp, esp

    fld qword [ebp + 8]
    fcos
    fstp qword [ebp + 8]

    pop ebp
    ret

tan:
    push ebp
    mov ebp, esp

    fld qword [ebp + 8]
    fsincos
    fdivp st1, st0
    fstp qword [ebp + 8]

    pop ebp
    ret

cot:
    push ebp
    mov ebp, esp

    fld qword [ebp + 8]
    fsincos
    fdivrp st1, st0
    fstp qword [ebp + 8]

    pop ebp
    ret
