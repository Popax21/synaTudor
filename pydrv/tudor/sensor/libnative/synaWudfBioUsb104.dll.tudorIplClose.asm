%use masm
default rel
section .note.GNU-stack noalloc noexec nowrite progbits

global EXTR_tudorIplClose

section .text
EXTR_tudorIplClose:
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x48
    mov dword ptr [rsp + 0x30],0x0
    mov qword ptr [rsp + 0x38],0x0
    cmp qword ptr [glb180179f90],0x0
    jz .lbl18008729e
    lea r8,[glb1800db738]
    lea rdx,[glb18016e138]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funEnter
    .lbl18008729e: cmp qword ptr [rsp + 0x50],0x0
    jnz .lbl1800872ff
    .lbl1800872a6: cmp qword ptr [glb180179f90],0x0
    jz .lbl1800872f7
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x2
    test eax,eax
    jz .lbl1800872f7
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x2
    ja .lbl1800872f7
    lea rax,[glb18016e140]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0xa4aba2b3
    lea r8,[glb18016e160]
    mov edx,0x2
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl1800872f7: xor eax,eax
    test eax,eax
    jnz .lbl1800872a6
    jmp .lbl180087372
    .lbl1800872ff: mov rax,qword ptr [rsp + 0x50]
    mov qword ptr [rsp + 0x38],rax
    mov rax,qword ptr [rsp + 0x38]
    cmp qword ptr [rax + 0x28],0x0
    jz .lbl180087330
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,qword ptr [rax + 0x28]
    call EXTR_free wrt ..plt
    mov rax,qword ptr [rsp + 0x38]
    mov qword ptr [rax + 0x28],0x0
    .lbl180087330: mov rax,qword ptr [rsp + 0x38]
    cmp qword ptr [rax + 0x30],0x0
    jz .lbl180087357
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,qword ptr [rax + 0x30]
    call EXTR_free wrt ..plt
    mov rax,qword ptr [rsp + 0x38]
    mov qword ptr [rax + 0x30],0x0
    .lbl180087357: cmp qword ptr [rsp + 0x38],0x0
    jz .lbl180087372
    mov rcx,qword ptr [rsp + 0x38]
    call EXTR_free wrt ..plt
    mov qword ptr [rsp + 0x38],0x0
    .lbl180087372: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087396
    lea r8,[glb1800db738]
    lea rdx,[glb18016e168]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funLeave
    .lbl180087396: mov eax,dword ptr [rsp + 0x30]
    add rsp,0x48
    ret

section .data

glb18016e138: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e168: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e140: db 67, 111, 110, 116, 101, 120, 116, 32, 97, 108, 114, 101, 97, 100, 121, 32, 114, 101, 108, 101, 97, 115, 101, 100, 0
glb18016e160: db 115, 121, 110, 97, 76, 105, 98, 0
glb180179f90: db 8 dup 0
glb1800db738: db 116, 117, 100, 111, 114, 73, 112, 108, 67, 108, 111, 115, 101, 0

section .text
EXTR_funEnter:
    ret

section .text
EXTR_funLeave:
    ret

section .text
EXTR_log:
    ret

section .text
extern EXTR_free

