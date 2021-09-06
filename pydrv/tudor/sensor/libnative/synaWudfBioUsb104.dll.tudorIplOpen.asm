%use masm
default rel
section .note.GNU-stack noalloc noexec nowrite progbits

global EXTR_tudorIplOpen

section .text
EXTR_tudorIplOpen:
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x88
    mov dword ptr [rsp + 0x30],0x0
    mov qword ptr [rsp + 0x38],0x0
    mov dword ptr [rsp + 0x60],0x0
    mov dword ptr [rsp + 0x64],0x0
    mov dword ptr [rsp + 0x68],0x0
    mov dword ptr [rsp + 0x44],0xffffffff
    mov dword ptr [rsp + 0x40],0x0
    cmp qword ptr [glb180179f90],0x0
    jz .lbl180086c8e
    lea r8,[glb1800db728]
    lea rdx,[glb18016e050]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funEnter
    .lbl180086c8e: cmp qword ptr [rsp + 0x90],0x0
    jnz .lbl180086cfd
    .lbl180086c99: cmp qword ptr [glb180179f90],0x0
    jz .lbl180086cea
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180086cea
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180086cea
    lea rax,[glb18016e058]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x38fb9d6b
    lea r8,[glb18016e068]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180086cea: xor eax,eax
    test eax,eax
    jnz .lbl180086c99
    mov dword ptr [rsp + 0x30],0x6f
    jmp .lbl18008721b
    .lbl180086cfd: cmp qword ptr [rsp + 0x98],0x0
    jnz .lbl180086d6c
    .lbl180086d08: cmp qword ptr [glb180179f90],0x0
    jz .lbl180086d59
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180086d59
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180086d59
    lea rax,[glb18016e070]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0xf595325b
    lea r8,[glb18016e080]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180086d59: xor eax,eax
    test eax,eax
    jnz .lbl180086d08
    mov dword ptr [rsp + 0x30],0x6f
    jmp .lbl18008721b
    .lbl180086d6c: mov ecx,0x40
    call EXTR_malloc wrt ..plt
    mov qword ptr [rsp + 0x38],rax
    cmp qword ptr [rsp + 0x38],0x0
    jnz .lbl180086df5
    cmp dword ptr [rsp + 0x30],0x0
    jz .lbl180086d91
    cmp dword ptr [rsp + 0x30],0x6e
    jnz .lbl180086d99
    .lbl180086d91: mov dword ptr [rsp + 0x30],0x25a
    .lbl180086d99: cmp qword ptr [glb180179f90],0x0
    jz .lbl180086dea
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180086dea
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180086dea
    lea rax,[glb18016e088]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x7fbdc693
    lea r8,[glb18016e0a8]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180086dea: xor eax,eax
    test eax,eax
    jnz .lbl180086d99
    jmp .lbl18008721b
    .lbl180086df5: mov r8d,0x40
    xor edx,edx
    mov rcx,qword ptr [rsp + 0x38]
    call EXTR_memset2
    mov rax,qword ptr [rsp + 0x98]
    mov rcx,qword ptr [rsp + 0x38]
    mov r8d,0x12
    mov rdx,rax
    call EXTR_memcpy wrt ..plt
    mov rax,qword ptr [rsp + 0x98]
    movzx eax,word ptr [rax + 0x10]
    mov rcx,qword ptr [rsp + 0x38]
    mov dword ptr [rcx + 0x14],eax
    mov rax,qword ptr [rsp + 0x98]
    movzx eax,word ptr [rax + 0x8]
    mov rcx,qword ptr [rsp + 0x38]
    mov dword ptr [rcx + 0x18],eax
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,qword ptr [rsp + 0x38]
    mov eax,dword ptr [rax + 0x14]
    imul eax,dword ptr [rcx + 0x18]
    mov eax,eax
    shl rax,1
    mov dword ptr [rsp + 0x60],eax
    mov ecx,dword ptr [rsp + 0x60]
    call EXTR_malloc wrt ..plt
    mov rcx,qword ptr [rsp + 0x38]
    mov qword ptr [rcx + 0x28],rax
    mov rax,qword ptr [rsp + 0x38]
    cmp qword ptr [rax + 0x28],0x0
    jnz .lbl180086ef4
    cmp dword ptr [rsp + 0x30],0x0
    jz .lbl180086e90
    cmp dword ptr [rsp + 0x30],0x6e
    jnz .lbl180086e98
    .lbl180086e90: mov dword ptr [rsp + 0x30],0x25a
    .lbl180086e98: cmp qword ptr [glb180179f90],0x0
    jz .lbl180086ee9
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180086ee9
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180086ee9
    lea rax,[glb18016e0b0]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x7fbdc693
    lea r8,[glb18016e0d0]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180086ee9: xor eax,eax
    test eax,eax
    jnz .lbl180086e98
    jmp .lbl18008721b
    .lbl180086ef4: mov qword ptr [rsp + 0x70],0x0
    mov rax,qword ptr [rsp + 0x98]
    movzx eax,byte ptr [rax + 0x1c]
    cmp eax,0x4
    jnz .lbl180086f26
    mov dword ptr [rsp + 0x6c],0x3
    mov eax,0x16b
    mov rcx,qword ptr [rsp + 0x38]
    mov word ptr [rcx + 0x38],ax
    jmp .lbl180086f8a
    .lbl180086f26: cmp qword ptr [glb180179f90],0x0
    jz .lbl180086f77
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x4
    test eax,eax
    jz .lbl180086f77
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x3
    ja .lbl180086f77
    lea rax,[glb18016e0d8]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0xeedbde43
    lea r8,[glb18016e100]
    mov edx,0x3
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180086f77: xor eax,eax
    test eax,eax
    jnz .lbl180086f26
    mov dword ptr [rsp + 0x30],0x6f
    jmp .lbl18008721b
    .lbl180086f8a: call EXTR_const_0x300
    mov dword ptr [rsp + 0x48],eax
    mov ecx,dword ptr [rsp + 0x48]
    call EXTR_malloc wrt ..plt
    mov qword ptr [rsp + 0x70],rax
    cmp qword ptr [rsp + 0x70],0x0
    jnz .lbl18008701b
    cmp dword ptr [rsp + 0x30],0x0
    jz .lbl180086fb7
    cmp dword ptr [rsp + 0x30],0x6e
    jnz .lbl180086fbf
    .lbl180086fb7: mov dword ptr [rsp + 0x30],0x25a
    .lbl180086fbf: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087010
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180087010
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180087010
    lea rax,[glb18016e108]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x7fbdc693
    lea r8,[glb18016e128]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087010: xor eax,eax
    test eax,eax
    jnz .lbl180086fbf
    jmp .lbl18008721b
    .lbl18008701b: mov r9,qword ptr [rsp + 0x70]
    mov rax,qword ptr [rsp + 0x38]
    mov r8d,dword ptr [rax + 0x14]
    mov rax,qword ptr [rsp + 0x38]
    mov edx,dword ptr [rax + 0x18]
    mov ecx,dword ptr [rsp + 0x6c]
    call EXTR_FUN_180001830
    mov rcx,qword ptr [rsp + 0x38]
    mov qword ptr [rcx + 0x30],rax
    mov rax,qword ptr [rsp + 0x98]
    mov eax,dword ptr [rax + 0x14]
    mov dword ptr [rsp + 0x64],eax
    mov rax,qword ptr [rsp + 0x98]
    mov eax,dword ptr [rax + 0x18]
    mov dword ptr [rsp + 0x68],eax
    call EXTR_const_0x44
    mov dword ptr [rsp + 0x48],eax
    mov dword ptr [rsp + 0x40],0x0
    jmp .lbl18008707e
    .lbl180087074: mov eax,dword ptr [rsp + 0x40]
    inc eax
    mov dword ptr [rsp + 0x40],eax
    .lbl18008707e: movsxd rax,dword ptr [rsp + 0x40]
    cmp rax,0x2
    jnc .lbl1800870e2
    movsxd rax,dword ptr [rsp + 0x40]
    imul rax,rax,0x50
    lea rcx,[glb18016dfb0]
    mov edx,dword ptr [rsp + 0x64]
    cmp dword ptr [rcx + rax*0x1],edx
    jnz .lbl1800870e0
    movsxd rax,dword ptr [rsp + 0x40]
    imul rax,rax,0x50
    lea rcx,[glb18016dfb0]
    mov edx,dword ptr [rsp + 0x68]
    cmp dword ptr [rcx + rax*0x1 + 0x4],edx
    jnz .lbl1800870e0
    movsxd rax,dword ptr [rsp + 0x40]
    imul rax,rax,0x50
    lea rcx,[glb18016dfb0]
    mov edx,dword ptr [rsp + 0x48]
    cmp dword ptr [rcx + rax*0x1 + 0x8],edx
    jnz .lbl1800870e0
    mov eax,dword ptr [rsp + 0x40]
    mov dword ptr [rsp + 0x44],eax
    jmp .lbl1800870e2
    .lbl1800870e0: jmp .lbl180087074
    .lbl1800870e2: cmp dword ptr [rsp + 0x44],0x0
    jl .lbl180087175
    movsxd rax,dword ptr [rsp + 0x44]
    imul rax,rax,0x50
    lea rcx,[glb18016dfb0]
    lea rax,[rcx + rax*0x1 + 0x1e]
    mov qword ptr [rsp + 0x50],rax
    mov rax,qword ptr [rsp + 0x50]
    mov rcx,qword ptr [rsp + 0x38]
    movzx ecx,word ptr [rcx + 0x14]
    mov word ptr [rax],cx
    mov rax,qword ptr [rsp + 0x50]
    add rax,0x2
    mov qword ptr [rsp + 0x50],rax
    mov rax,qword ptr [rsp + 0x50]
    mov rcx,qword ptr [rsp + 0x38]
    movzx ecx,word ptr [rcx + 0x18]
    mov word ptr [rax],cx
    mov rax,qword ptr [rsp + 0x50]
    add rax,0x2
    mov qword ptr [rsp + 0x50],rax
    movsxd rax,dword ptr [rsp + 0x44]
    imul rax,rax,0x50
    lea rcx,[glb18016dfb0]
    lea rax,[rcx + rax*0x1 + 0xc]
    mov r8,rax
    mov edx,0x64
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,qword ptr [rax + 0x30]
    call EXTR_FUN_180001960
    jmp .lbl18008720b
    .lbl180087175: mov rax,qword ptr [rsp + 0x98]
    cmp qword ptr [rax + 0x28],0x0
    jz .lbl18008720b
    mov rax,qword ptr [rsp + 0x98]
    mov ecx,dword ptr [rsp + 0x48]
    cmp dword ptr [rax + 0x20],ecx
    jnz .lbl18008720b
    mov rax,qword ptr [rsp + 0x98]
    mov rax,qword ptr [rax + 0x28]
    add rax,0x12
    mov qword ptr [rsp + 0x58],rax
    mov rax,qword ptr [rsp + 0x58]
    mov rcx,qword ptr [rsp + 0x38]
    movzx ecx,word ptr [rcx + 0x14]
    mov word ptr [rax],cx
    mov rax,qword ptr [rsp + 0x58]
    add rax,0x2
    mov qword ptr [rsp + 0x58],rax
    mov rax,qword ptr [rsp + 0x58]
    mov rcx,qword ptr [rsp + 0x38]
    movzx ecx,word ptr [rcx + 0x18]
    mov word ptr [rax],cx
    mov rax,qword ptr [rsp + 0x58]
    add rax,0x2
    mov qword ptr [rsp + 0x58],rax
    mov rax,qword ptr [rsp + 0x98]
    mov r8,qword ptr [rax + 0x28]
    mov edx,0x64
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,qword ptr [rax + 0x30]
    call EXTR_FUN_180001960
    .lbl18008720b: mov rax,qword ptr [rsp + 0x90]
    mov rcx,qword ptr [rsp + 0x38]
    mov qword ptr [rax],rcx
    .lbl18008721b: cmp qword ptr [glb180179f90],0x0
    jz .lbl18008723f
    lea r8,[glb1800db728]
    lea rdx,[glb18016e130]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funLeave
    .lbl18008723f: mov eax,dword ptr [rsp + 0x30]
    add rsp,0x88
    ret

section .data

glb18016e068: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e088: db 77, 101, 109, 111, 114, 121, 32, 97, 108, 108, 111, 99, 97, 116, 105, 111, 110, 32, 102, 97, 105, 108, 101, 100, 46, 10, 0
glb18016e0a8: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e108: db 77, 101, 109, 111, 114, 121, 32, 97, 108, 108, 111, 99, 97, 116, 105, 111, 110, 32, 102, 97, 105, 108, 101, 100, 46, 10, 0
glb18016e128: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e080: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e100: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e058: db 112, 104, 73, 112, 108, 32, 105, 115, 32, 78, 85, 76, 76, 0
glb18016e0d8: db 73, 80, 76, 32, 112, 114, 111, 100, 117, 99, 116, 32, 116, 121, 112, 101, 32, 105, 115, 32, 110, 111, 116, 32, 100, 101, 102, 105, 110, 101, 100, 33, 0
glb18016dfb8: db 68, 0, 0, 0, -38, 125, 0, 0, 16, 52, 2, 0, 88, 27, 0, 0, -116, 0, 0, 0, 68, 0, 86, 0, 104, 0, 3, 0, 107, 1, 64, 31, 0, 0, 0, 0, -106, 0, 4, 0, -24, 3, 60, 0, 0, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, -121, 13, 0, 0, 1, 0, 0, 0, 68, 0, 0, 0, 54, 63, 1, 0, 80, -98, 2, 0, 88, 27, 0, 0, -98, 0, 0, 0, 68, 0, 86, 0, 104, 0, 3, 0, 107, 1, 64, 31, 0, 0, 0, 0, 16, 39, 122, 0, -24, 3, -84, 13, 0, 0, 0, 0, 33, 0, 63, 0, -121, 0, -23, 0, 36, 1, 69, 1, 15, 1, -41, 0, -106, 0, 85, 0, 25, 0, 12, 0
glb18016e050: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e070: db 112, 80, 97, 114, 97, 109, 32, 105, 115, 32, 78, 85, 76, 76, 0
glb18016e0b0: db 77, 101, 109, 111, 114, 121, 32, 97, 108, 108, 111, 99, 97, 116, 105, 111, 110, 32, 102, 97, 105, 108, 101, 100, 46, 10, 0
glb18016e0d0: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016dfb0: db 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, -38, 125, 0, 0, 16, 52, 2, 0, 88, 27, 0, 0, -116, 0, 0, 0, 68, 0, 86, 0, 104, 0, 3, 0, 107, 1, 64, 31, 0, 0, 0, 0, -106, 0, 4, 0, -24, 3, 60, 0, 0, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, -121, 13, 0, 0, 1, 0, 0, 0, 68, 0, 0, 0, 54, 63, 1, 0, 80, -98, 2, 0, 88, 27, 0, 0, -98, 0, 0, 0, 68, 0, 86, 0, 104, 0, 3, 0, 107, 1, 64, 31, 0, 0, 0, 0, 16, 39, 122, 0, -24, 3, -84, 13, 0, 0, 0, 0, 33, 0, 63, 0, -121, 0, -23, 0, 36, 1, 69, 1, 15, 1, -41, 0, -106, 0, 85, 0, 25, 0, 12, 0
glb18016e130: db 115, 121, 110, 97, 76, 105, 98, 0
glb180179f90: db 8 dup 0
glb1800db728: db 116, 117, 100, 111, 114, 73, 112, 108, 79, 112, 101, 110, 0
glb18016dfb4: db 0, 0, 0, 0, 68, 0, 0, 0, -38, 125, 0, 0, 16, 52, 2, 0, 88, 27, 0, 0, -116, 0, 0, 0, 68, 0, 86, 0, 104, 0, 3, 0, 107, 1, 64, 31, 0, 0, 0, 0, -106, 0, 4, 0, -24, 3, 60, 0, 0, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, 25, 0, -121, 13, 0, 0, 1, 0, 0, 0, 68, 0, 0, 0, 54, 63, 1, 0, 80, -98, 2, 0, 88, 27, 0, 0, -98, 0, 0, 0, 68, 0, 86, 0, 104, 0, 3, 0, 107, 1, 64, 31, 0, 0, 0, 0, 16, 39, 122, 0, -24, 3, -84, 13, 0, 0, 0, 0, 33, 0, 63, 0, -121, 0, -23, 0, 36, 1, 69, 1, 15, 1, -41, 0, -106, 0, 85, 0, 25, 0, 12, 0

section .text
EXTR_const_0x44:
    mov eax,0x44
    ret

section .data


section .text
EXTR_funEnter:
    ret

section .text
extern EXTR_memcpy

section .text
extern EXTR_malloc

section .text
EXTR_FUN_180001830:
    push rbx
    sub rsp,0x20
    mov rbx,r9
    mov qword ptr [r9],0x0
    mov r9d,r8d
    mov r8d,edx
    mov edx,ecx
    mov rcx,rbx
    call EXTR_FUN_180001730
    mov rax,rbx
    add rsp,0x20
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180001730:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x20
    mov esi,r9d
    mov ebp,r8d
    mov edi,edx
    mov rbx,rcx
    call EXTR_FUN_1800016d0
    cmp edx,0x2
    mov dword ptr [rcx + 0x10],0x1b58
    mov word ptr [rcx + 0x140],dx
    jz .lbl180001782
    cmp edx,0x8
    jle .lbl180001777
    cmp edx,0xa
    jle .lbl180001782
    .lbl180001777: mov word ptr [rbx + 0x142],0x0
    jmp .lbl18000178b
    .lbl180001782: mov word ptr [rbx + 0x142],0x1
    .lbl18000178b: lea rcx,[rbx + 0x18]
    call EXTR_FUN_1800014f0
    xor r12d,r12d
    lea rcx,[rbx + 0x88]
    mov r9d,esi
    mov r8d,ebp
    mov edx,edi
    mov word ptr [rbx + 0x144],ax
    mov qword ptr [rbx + 0x60],r12
    mov qword ptr [rbx + 0x68],r12
    mov qword ptr [rbx + 0x78],r12
    call EXTR_FUN_1800010b0
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x48]
    mov qword ptr [rbx + 0x100],r12
    mov qword ptr [rbx + 0x108],r12
    mov qword ptr [rbx + 0x110],r12
    mov qword ptr [rbx + 0x118],r12
    mov qword ptr [rbx + 0x120],r12
    mov qword ptr [rbx + 0x138],r12
    mov qword ptr [rbx + 0x130],r12
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop r12
    ret

section .data


section .text
EXTR_FUN_1800016d0:
    mov word ptr [rcx + 0x140],0x0
    mov word ptr [rcx + 0x142],0x0
    mov word ptr [rcx + 0x144],0x0
    mov word ptr [rcx + 0x146],0x0
    mov dword ptr [glb18017a2e0],0x898
    mov dword ptr [glb18017a2e4],0x258
    mov dword ptr [glb18017a2e8],0x258
    mov dword ptr [glb18017a2ec],0x3e8
    mov dword ptr [glb18017a2f0],0xfffffc18
    ret

section .data

glb18017a2e8: db 20 dup 0
glb18017a2ec: db 20 dup 0
glb18017a2e0: db 20 dup 0
glb18017a2f0: db 20 dup 0
glb18017a2e4: db 20 dup 0

section .text
EXTR_FUN_1800014f0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x20
    mov esi,edx
    xor edx,edx
    mov edi,r8d
    lea r8d,[rdx + 0x44]
    mov ebx,r9d
    mov rbp,rcx
    xor r12w,r12w
    call EXTR_memset wrt ..plt
    mov rcx,rbp
    call EXTR_FUN_180001480
    mov eax,0x2
    mov word ptr [rbp + 0x16],si
    cmp ebx,eax
    mov word ptr [rbp + 0x10],0x44
    cmovl bx,ax
    cmp edi,eax
    cmovl di,ax
    add esi,-0x1
    mov word ptr [rbp + 0x12],bx
    cmp esi,0xa
    mov word ptr [rbp + 0x14],di
    ja .lbl180001656
    lea rdx,[EXTR_FUN_1800014f0]
    movsxd rax,esi
    mov ecx,dword ptr [rdx + rax*0x4 + (.jt18000156f - EXTR_FUN_1800014f0)]
    add rcx,rdx
    jmp rcx
    .lbl180001571: mov r12w,0x100
    jmp .lbl180001652
    .lbl18000157b: or dword ptr [rbp + 0xc],0x1e
    mov r12w,0x200
    mov word ptr [rbp + 0x1c],0x4
    mov word ptr [rbp + 0x20],0xc8
    mov word ptr [rbp + 0x22],0x226
    mov word ptr [rbp + 0x24],0x3e8
    jmp .lbl180001656
    .lbl1800015a1: or dword ptr [rbp + 0xc],0x8
    mov word ptr [rbp + 0x1c],0x4
    mov word ptr [rbp + 0x20],0x3e8
    mov word ptr [rbp + 0x22],0x320
    jmp .lbl180001656
    .lbl1800015bc: mov r12w,0x2bc
    mov word ptr [rbp + 0x1c],0x6
    mov word ptr [rbp + 0x20],0x12c
    mov word ptr [rbp + 0x22],0x64
    mov word ptr [rbp + 0x24],0x578
    jmp .lbl18000164c
    .lbl1800015db: mov word ptr [rbp + 0x18],0x1f4
    .lbl1800015e1: or dword ptr [rbp + 0xc],0x293
    mov word ptr [rbp + 0x1c],0x5
    mov word ptr [rbp + 0x24],0x1b58
    mov word ptr [rbp + 0x20],0x32c8
    mov word ptr [rbp + 0x22],0x4e20
    jmp .lbl180001656
    .lbl180001602: or dword ptr [rbp + 0xc],0x9e
    mov r12w,0x2bc
    mov word ptr [rbp + 0x1c],0x6
    mov word ptr [rbp + 0x1a],0x7d0
    mov word ptr [rbp + 0x22],0x3e8
    mov word ptr [rbp + 0x24],0x1388
    mov word ptr [rbp + 0x20],0x12c
    mov word ptr [rbp + 0x1e],0x0
    jmp .lbl180001656
    .lbl180001634: mov word ptr [rbp + 0x1c],0x0
    mov word ptr [rbp + 0x20],0x3e8
    mov word ptr [rbp + 0x22],0x14a
    mov word ptr [rbp + 0x24],0x1f40
    .lbl18000164c: mov word ptr [rbp + 0x1e],0x0
    .lbl180001652: or dword ptr [rbp + 0xc],0x1a
    .lbl180001656: movzx edx,r12w
    mov rcx,rbp
    call EXTR_FUN_1800012e0
    mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x48]
    mov ax,r12w
    add rsp,0x20
    pop r12
    ret

section .data
    .jt18000156f dq (.lbl180001571 - EXTR_FUN_1800014f0), (.lbl180001656 - EXTR_FUN_1800014f0), (.lbl18000157b - EXTR_FUN_1800014f0), (.lbl1800015a1 - EXTR_FUN_1800014f0), (.lbl1800015bc - EXTR_FUN_1800014f0), (.lbl1800015db - EXTR_FUN_1800014f0), (.lbl1800015e1 - EXTR_FUN_1800014f0), (.lbl180001602 - EXTR_FUN_1800014f0), (.lbl180001656 - EXTR_FUN_1800014f0), (.lbl180001656 - EXTR_FUN_1800014f0), (.lbl180001634 - EXTR_FUN_1800014f0)


section .text
EXTR_FUN_180001480:
    xor edx,edx
    mov dword ptr [rcx + 0x8],0x1b58
    mov dword ptr [rcx + 0xc],0x0
    mov word ptr [rcx + 0x12],0x1
    mov word ptr [rcx + 0x14],0x1
    mov word ptr [rcx + 0x16],0x0
    mov word ptr [rcx + 0x18],0x16b
    mov word ptr [rcx + 0x1a],0x1f40
    mov word ptr [rcx + 0x1c],0x2
    mov word ptr [rcx + 0x1e],0x0
    mov word ptr [rcx + 0x20],0x7fff
    mov word ptr [rcx + 0x22],0x1c2
    mov word ptr [rcx + 0x24],0x14d
    mov word ptr [rcx + 0x26],0xdac
    mov word ptr [rcx + 0x28],0x0
    lea r8d,[rdx + 0x1a]
    add rcx,0x2a
    jmp EXTR_memset wrt ..plt

section .data


section .text
extern EXTR_memset

section .text
EXTR_FUN_1800012e0:
    movzx eax,word ptr [rcx + 0x16]
    cmp eax,0x6
    jl .lbl180001385
    cmp eax,0x7
    jle .lbl180001336
    cmp eax,0xb
    jnz .lbl180001385
    mov word ptr [rcx + 0x2c],0xa
    mov word ptr [rcx + 0x2e],0x64
    mov word ptr [rcx + 0x30],0x64
    mov word ptr [rcx + 0x32],0x64
    mov word ptr [rcx + 0x34],0xa
    mov word ptr [rcx + 0x36],0x0
    mov word ptr [rcx + 0x38],0x0
    mov word ptr [rcx + 0x3a],0x0
    mov word ptr [rcx + 0x3c],0x0
    jmp .lbl180001459
    .lbl180001336: mov word ptr [rcx + 0x2c],0xf
    mov word ptr [rcx + 0x2e],0x19
    mov word ptr [rcx + 0x30],0x19
    mov word ptr [rcx + 0x32],0x19
    mov word ptr [rcx + 0x34],0x19
    mov word ptr [rcx + 0x36],0x19
    mov word ptr [rcx + 0x38],0x19
    mov word ptr [rcx + 0x3a],0x19
    mov word ptr [rcx + 0x3c],0x14
    mov word ptr [rcx + 0x3e],0xf
    mov word ptr [rcx + 0x40],0x5
    mov word ptr [rcx + 0x42],0x0
    mov word ptr [rcx + 0x2a],0x0
    ret
    .lbl180001385: test edx,edx
    jnz .lbl1800013a4
    mov rax,0x19001900190019
    mov qword ptr [rcx + 0x2a],rax
    mov qword ptr [rcx + 0x32],rax
    mov qword ptr [rcx + 0x3a],rax
    mov word ptr [rcx + 0x42],ax
    ret
    .lbl1800013a4: cmp edx,0x180
    jg .lbl1800013fb
    mov word ptr [rcx + 0x2c],0x21
    mov word ptr [rcx + 0x2e],0x3f
    mov word ptr [rcx + 0x30],0x87
    mov word ptr [rcx + 0x32],0xe9
    mov word ptr [rcx + 0x34],0x124
    mov word ptr [rcx + 0x36],0x145
    mov word ptr [rcx + 0x38],0x10f
    mov word ptr [rcx + 0x3a],0xd7
    mov word ptr [rcx + 0x3c],0x96
    mov word ptr [rcx + 0x3e],0x55
    mov word ptr [rcx + 0x40],0x19
    mov word ptr [rcx + 0x42],0xc
    mov word ptr [rcx + 0x2a],0x0
    ret
    .lbl1800013fb: cmp edx,0x260
    mov word ptr [rcx + 0x2c],0x50
    mov word ptr [rcx + 0x30],0x14c
    mov word ptr [rcx + 0x32],0x2fa
    mov word ptr [rcx + 0x34],0x28b
    jg .lbl18000143b
    mov word ptr [rcx + 0x2e],0x64
    mov word ptr [rcx + 0x36],0x200
    mov word ptr [rcx + 0x38],0x15e
    mov word ptr [rcx + 0x3a],0xf9
    mov word ptr [rcx + 0x3c],0x0
    jmp .lbl180001459
    .lbl18000143b: mov word ptr [rcx + 0x2e],0xc8
    mov word ptr [rcx + 0x36],0x1c2
    mov word ptr [rcx + 0x38],0x190
    mov word ptr [rcx + 0x3a],0x12c
    mov word ptr [rcx + 0x3c],0xc8
    .lbl180001459: mov word ptr [rcx + 0x3e],0x0
    mov word ptr [rcx + 0x40],0x0
    mov word ptr [rcx + 0x42],0x0
    mov word ptr [rcx + 0x2a],0x0
    ret

section .data


section .text
EXTR_FUN_1800010b0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    mov edi,edx
    xor edx,edx
    mov esi,r8d
    lea r8d,[rdx + 0x78]
    mov ebp,r9d
    mov rbx,rcx
    call EXTR_memset wrt ..plt
    mov rcx,rbx
    call EXTR_FUN_180001000
    mov eax,0x2
    mov word ptr [rbx + 0x10],0x78
    cmp esi,eax
    mov word ptr [rbx + 0x1a],di
    cmovl esi,eax
    cmp ebp,eax
    cmovl ebp,eax
    cmp edi,eax
    mov word ptr [rbx + 0x14],si
    mov word ptr [rbx + 0x12],bp
    jz .lbl1800011d6
    cmp edi,0x8
    jle .lbl180001288
    cmp edi,0xa
    jg .lbl180001288
    mov word ptr [rbx + 0x16],bp
    mov word ptr [rbx + 0x18],si
    mov word ptr [rbx + 0x22],0xafc8
    mov word ptr [rbx + 0x26],0xffff
    mov word ptr [rbx + 0x2a],0x320
    mov word ptr [rbx + 0x2c],0x64
    mov word ptr [rbx + 0x28],0xffff
    mov word ptr [rbx + 0x2e],0xffff
    mov word ptr [rbx + 0x36],0x7
    mov word ptr [rbx + 0x38],ax
    mov word ptr [rbx + 0x3a],0x7fff
    mov word ptr [rbx + 0x3c],0x3
    mov word ptr [rbx + 0x3e],0x104
    mov word ptr [rbx + 0x48],0x1c
    mov word ptr [rbx + 0x4c],0x3c
    mov word ptr [rbx + 0x4e],0x4c
    mov word ptr [rbx + 0x50],0x50
    mov word ptr [rbx + 0x52],0x57
    mov word ptr [rbx + 0x54],0x64
    mov word ptr [rbx + 0x56],0x64
    mov word ptr [rbx + 0x58],0x6b
    mov word ptr [rbx + 0x64],0x6e
    mov word ptr [rbx + 0x66],0x6e
    mov word ptr [rbx + 0x68],0x6e
    mov word ptr [rbx + 0x6a],0x6e
    mov word ptr [rbx + 0x6c],0x67
    mov word ptr [rbx + 0x6e],0x64
    mov word ptr [rbx + 0x70],0x5b
    mov word ptr [rbx + 0x72],0x46
    mov word ptr [rbx + 0x74],0x46
    mov word ptr [rbx + 0x76],0x32
    jmp .lbl180001258
    .lbl1800011d6: mov eax,ebp
    mov word ptr [rbx + 0x48],0x3c
    mov word ptr [rbx + 0x4c],0x50
    cdq
    mov word ptr [rbx + 0x4e],0x50
    mov word ptr [rbx + 0x50],0x64
    sub eax,edx
    mov word ptr [rbx + 0x52],0x64
    mov word ptr [rbx + 0x54],0x6e
    sar eax,1
    mov word ptr [rbx + 0x56],0x6e
    mov word ptr [rbx + 0x58],0x6e
    mov word ptr [rbx + 0x16],ax
    mov eax,esi
    mov word ptr [rbx + 0x64],0x64
    cdq
    mov word ptr [rbx + 0x66],0x64
    mov word ptr [rbx + 0x68],0x46
    sub eax,edx
    mov word ptr [rbx + 0x6a],0x46
    mov word ptr [rbx + 0x6c],0x28
    sar eax,1
    mov word ptr [rbx + 0x6e],0x28
    mov word ptr [rbx + 0x70],0x14
    mov word ptr [rbx + 0x72],0x14
    mov word ptr [rbx + 0x74],0x0
    mov word ptr [rbx + 0x76],0x0
    mov word ptr [rbx + 0x18],ax
    .lbl180001258: mov word ptr [rbx + 0x62],0x6e
    mov word ptr [rbx + 0x60],0x6e
    mov word ptr [rbx + 0x5e],0x6e
    mov word ptr [rbx + 0x5c],0x6e
    mov word ptr [rbx + 0x5a],0x6e
    mov word ptr [rbx + 0x4a],0x3c
    mov word ptr [rbx + 0x46],0x0
    mov word ptr [rbx + 0x44],0x0
    .lbl180001288: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180001000:
    xor edx,edx
    mov dword ptr [rcx + 0x8],0x1b58
    mov word ptr [rcx + 0x12],0x1
    mov word ptr [rcx + 0x14],0x1
    mov word ptr [rcx + 0x16],0x1
    mov word ptr [rcx + 0x18],0x1
    mov dword ptr [rcx + 0xc],0xfe
    mov word ptr [rcx + 0x1c],0x0
    mov word ptr [rcx + 0x1e],0x1
    mov word ptr [rcx + 0x20],0x1f4
    mov word ptr [rcx + 0x22],0xfa0
    mov word ptr [rcx + 0x24],0x1f4
    mov word ptr [rcx + 0x26],0xfff
    mov word ptr [rcx + 0x2a],0x190
    mov word ptr [rcx + 0x2c],0x5a
    mov word ptr [rcx + 0x2e],0xfff
    mov word ptr [rcx + 0x30],0x1
    mov word ptr [rcx + 0x32],0x3
    mov word ptr [rcx + 0x28],0xfa0
    mov word ptr [rcx + 0x34],0xffff
    mov word ptr [rcx + 0x36],0x7
    mov word ptr [rcx + 0x38],0x2
    mov word ptr [rcx + 0x3a],0xfff
    mov word ptr [rcx + 0x3c],0x3
    mov word ptr [rcx + 0x3e],0x190
    mov word ptr [rcx + 0x40],0x19
    mov word ptr [rcx + 0x42],0x0
    lea r8d,[rdx + 0x34]
    add rcx,0x44
    jmp EXTR_memset wrt ..plt

section .data


section .text
EXTR_funLeave:
    ret

section .text
EXTR_const_0x300:
    mov eax,0x300
    ret

section .data


section .text
EXTR_log:
    ret

section .text
EXTR_FUN_180001960:
    push r12
    push r13
    sub rsp,0x38
    mov r13,r8
    xor r8d,r8d
    cmp edx,0x3e9
    mov r12,rcx
    jg .lbl180001cb0
    cmp edx,0x3e9
    jz .lbl180001c95
    add edx,-0x64
    cmp edx,0x77
    ja .lbl180001d4d
    movsxd rax,edx
    lea rdx,[EXTR_FUN_180001960]
    movzx eax,byte ptr [rdx + rax*0x1 + 0x1db4]
    mov ecx,dword ptr [rdx + rax*0x4 + (.jt1800019b1 - EXTR_FUN_180001960)]
    add rcx,rdx
    jmp rcx
    .lbl1800019b3: lea rcx,[r12 + 0x42]
    mov r8d,0x1a
    mov rdx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl1800019ce: lea rdx,[r12 + 0x42]
    mov r8d,0x1a
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl1800019e9: movzx r8d,word ptr [r12 + 0x2c]
    movzx eax,word ptr [r12 + 0x2a]
    mov rdx,qword ptr [r12 + 0x78]
    imul r8d,eax
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001a0e: mov qword ptr [r12 + 0x60],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001a1c: mov qword ptr [r12 + 0x68],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001a2a: lea rcx,[r12 + 0xcc]
    mov r8d,0x34
    mov rdx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001a48: lea rdx,[r12 + 0xcc]
    mov r8d,0x34
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001a66: movzx r8d,word ptr [r12 + 0xa0]
    movzx eax,word ptr [r12 + 0x9e]
    mov rdx,qword ptr [r12 + 0x118]
    imul r8d,eax
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001a94: movzx r8d,word ptr [r12 + 0xa0]
    movzx eax,word ptr [r12 + 0x9e]
    mov rdx,qword ptr [r12 + 0x120]
    imul r8d,eax
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001ac2: mov qword ptr [r12 + 0x100],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001ad3: mov qword ptr [r12 + 0x108],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001ae4: mov qword ptr [r12 + 0x110],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001af5: lea rdx,[r12 + 0x1f4]
    mov r8d,0xe4
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001b13: mov rdx,qword ptr [r12 + 0x2f8]
    mov r8d,0xe4
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001b31: mov qword ptr [r12 + 0x130],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001b42: mov qword ptr [r12 + 0x138],r13
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001b53: movzx eax,word ptr [r12 + 0x9c]
    movzx ecx,word ptr [r12 + 0x9a]
    mov rdx,qword ptr [r12 + 0x100]
    imul ecx,eax
    mov qword ptr [rsp + 0x50],rbx
    lea ebx,[rcx + rcx*0x1]
    movsxd rax,ecx
    mov qword ptr [rsp + 0x58],rbp
    mov rcx,r13
    mov r8d,ebx
    mov qword ptr [rsp + 0x60],rsi
    mov qword ptr [rsp + 0x30],rdi
    lea rdi,[r13 + rax*0x2]
    call EXTR_memcpy wrt ..plt
    mov r8d,ebx
    xor edx,edx
    mov rcx,rdi
    call EXTR_memset wrt ..plt
    mov rax,qword ptr [r12]
    mov rbp,qword ptr [r12 + 0x100]
    mov qword ptr [rsp + 0x28],rax
    lea rdx,[r12 + 0x148]
    lea rcx,[r12 + 0x18]
    xor r9d,r9d
    mov r8,r13
    mov word ptr [rsp + 0x20],0x190
    mov qword ptr [r12 + 0x100],rdi
    call EXTR_FUN_18000a7f0
    mov eax,dword ptr [r12 + 0x94]
    mov ebx,eax
    and eax,0xffffffbf
    lea rdx,[r12 + 0x148]
    mov dword ptr [r12 + 0x94],eax
    mov rax,qword ptr [r12]
    lea rcx,[r12 + 0x18]
    shr ebx,0x6
    xor r9d,r9d
    mov r8,r13
    mov qword ptr [rsp + 0x20],rax
    and ebx,0x1
    call EXTR_FUN_18000b2c0
    mov rdi,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x60]
    shl ebx,0x6
    xor ebx,dword ptr [r12 + 0x94]
    mov qword ptr [r12 + 0x100],rbp
    mov rbp,qword ptr [rsp + 0x58]
    and ebx,0x40
    xor dword ptr [r12 + 0x94],ebx
    mov rbx,qword ptr [rsp + 0x50]
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001c56: movzx ecx,word ptr [r12 + 0x142]
    test ecx,ecx
    jz .lbl180001c73
    cmp ecx,0x1
    jnz .lbl180001c79
    movzx r8d,word ptr [r12 + 0xca]
    jmp .lbl180001c79
    .lbl180001c73: movzx r8d,word ptr [r12 + 0x40]
    .lbl180001c79: mov rcx,qword ptr [r12 + 0x2f0]
    shl r8d,0x2
    mov rdx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001c95: lea rdx,[r12 + 0x18]
    mov r8d,0x44
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001cb0: sub edx,0x3ea
    jz .lbl180001d20
    sub edx,0x1
    jz .lbl180001d02
    cmp edx,0x1
    jnz .lbl180001d4d
    .lbl180001cc6: lea rcx,[r12 + 0x88]
    mov r8d,0x78
    mov rdx,r13
    call EXTR_memcpy wrt ..plt
    movzx r11d,word ptr [r12 + 0xa2]
    mov word ptr [r12 + 0x142],0x1
    mov word ptr [r12 + 0x140],r11w
    add rsp,0x38
    pop r13
    pop r12
    ret
    .lbl180001d02: lea rdx,[r12 + 0x88]
    mov r8d,0x78
    mov rcx,r13
    add rsp,0x38
    pop r13
    pop r12
    jmp EXTR_memcpy wrt ..plt
    .lbl180001d20: lea rcx,[r12 + 0x18]
    mov r8d,0x44
    mov rdx,r13
    call EXTR_memcpy wrt ..plt
    movzx r11d,word ptr [r12 + 0x2e]
    mov word ptr [r12 + 0x142],0x0
    mov word ptr [r12 + 0x140],r11w
    .lbl180001d4d: add rsp,0x38
    pop r13
    pop r12
    ret

section .data
    .jt1800019b1 dq (.lbl180001d20 - EXTR_FUN_180001960), (.lbl180001c95 - EXTR_FUN_180001960), (.lbl1800019b3 - EXTR_FUN_180001960), (.lbl1800019ce - EXTR_FUN_180001960), (.lbl1800019e9 - EXTR_FUN_180001960), (.lbl180001a0e - EXTR_FUN_180001960), (.lbl180001a1c - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001cc6 - EXTR_FUN_180001960), (.lbl180001d02 - EXTR_FUN_180001960), (.lbl180001a2a - EXTR_FUN_180001960), (.lbl180001a48 - EXTR_FUN_180001960), (.lbl180001a66 - EXTR_FUN_180001960), (.lbl180001a94 - EXTR_FUN_180001960), (.lbl180001ac2 - EXTR_FUN_180001960), (.lbl180001ad3 - EXTR_FUN_180001960), (.lbl180001ae4 - EXTR_FUN_180001960), (.lbl180001c56 - EXTR_FUN_180001960), (.lbl180001b31 - EXTR_FUN_180001960), (.lbl180001b42 - EXTR_FUN_180001960), (.lbl180001b53 - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001d4d - EXTR_FUN_180001960), (.lbl180001af5 - EXTR_FUN_180001960), (.lbl180001b13 - EXTR_FUN_180001960)

glb180001db4: db 0

section .text
extern EXTR_memcpy

section .text
EXTR_FUN_18000a7f0:
    mov word ptr [rsp + 0x20],r9w
    mov qword ptr [rsp + 0x10],rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0xe8
    movzx r10d,word ptr [rcx + 0x82]
    movzx r11d,word ptr [rcx + 0x84]
    mov r14,rdx
    mov rdx,qword ptr [rsp + 0x158]
    mov rsi,rcx
    xor edi,edi
    movzx r15d,word ptr [rsi + 0x8c]
    mov rbx,r8
    movzx eax,r9w
    movzx r9d,word ptr [rcx + 0x86]
    movzx ecx,word ptr [rcx + 0x88]
    mov r8d,r11d
    imul r8d,r10d
    mov dword ptr [rsp + 0x64],ecx
    movsxd rbp,r8d
    mov dword ptr [rsp + 0x80],r10d
    imul ecx,r9d
    movsxd r12,ecx
    mov dword ptr [rsp + 0x74],ecx
    mov dword ptr [rsp + 0x140],r11d
    add r12,rdx
    test rbx,rbx
    mov rdx,qword ptr [rsi + 0x108]
    mov qword ptr [rsp + 0x90],rdx
    mov rdx,qword ptr [rsi + 0x100]
    lea rcx,[r12 + rbp*0x2]
    lea rcx,[rcx + rbp*0x2]
    mov qword ptr [rsp + 0xa8],rdx
    movzx edx,word ptr [rsi + 0x90]
    mov qword ptr [rsp + 0x78],rcx
    lea rcx,[rcx + rbp*0x2]
    mov dword ptr [rsp + 0xb0],edx
    movzx edx,word ptr [rsi + 0x92]
    mov qword ptr [rsp + 0xb8],rcx
    lea rcx,[rcx + rbp*0x2]
    mov dword ptr [rsp + 0xc0],edx
    movzx edx,word ptr [rsi + 0x94]
    mov qword ptr [rsp + 0x98],rcx
    lea rcx,[rcx + rbp*0x2]
    mov dword ptr [rsp + 0xa4],edx
    movzx edx,word ptr [rsi + 0x96]
    mov qword ptr [rsp + 0xd0],rcx
    lea rcx,[r12 + rbp*0x2]
    mov dword ptr [rsp + 0x6c],r8d
    mov word ptr [rsp + 0x130],dx
    mov dword ptr [rsp + 0x70],r9d
    mov dword ptr [rsp + 0x8c],edi
    mov r13d,edi
    mov dword ptr [rsp + 0x88],edi
    mov dword ptr [rsp + 0x84],edi
    mov dword ptr [rsp + 0x60],edi
    mov qword ptr [rsp + 0xd8],rcx
    mov word ptr [r14],ax
    mov word ptr [r14 + 0x2],0x64
    mov byte ptr [r14 + 0x4],dil
    mov byte ptr [r14 + 0x5],dil
    mov byte ptr [r14 + 0x6],dil
    mov word ptr [r14 + 0x8],di
    mov word ptr [r14 + 0xa],di
    mov dword ptr [r14 + 0xc],edi
    mov word ptr [r14 + 0x10],di
    mov word ptr [r14 + 0x12],di
    mov word ptr [r14 + 0x14],di
    jz .lbl18000b2a3
    test r10d,r10d
    jz .lbl18000b2a3
    test r11d,r11d
    jz .lbl18000b2a3
    cmp r10d,0xffff
    jg .lbl18000b2a3
    cmp r11d,0xffff
    jg .lbl18000b2a3
    cmp word ptr [rsi + 0x8a],0x2
    jnz .lbl18000a9e4
    lea rdx,[r11 + -0x1]
    mov r8,r10
    imul r8,r11
    imul rdx,r10
    movzx ecx,word ptr [rbx + r8*0x2 + -0x4]
    movzx eax,word ptr [rbx + rdx*0x2 + -0x4]
    add ecx,eax
    movzx eax,word ptr [rbx + rdx*0x2 + -0x2]
    add ecx,eax
    mov eax,0x55555556
    imul ecx
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov word ptr [rbx + r8*0x2 + -0x2],dx
    mov dx,word ptr [rsp + 0x130]
    .lbl18000a9e4: mov eax,dword ptr [rsp + 0x64]
    mov qword ptr [rsp + 0x38],r12
    mov word ptr [rsp + 0x30],dx
    mov dword ptr [rsp + 0x28],r9d
    mov r9,qword ptr [rsp + 0x158]
    mov r8d,r10d
    mov edx,r11d
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_1800078a0
    cmp word ptr [rsp + 0x148],di
    jbe .lbl18000aa25
    add word ptr [rsp + 0x130],0xfff6
    .lbl18000aa25: mov eax,dword ptr [rsp + 0x6c]
    mov rdx,qword ptr [rsi + 0xe8]
    mov rcx,r12
    add eax,eax
    mov r8d,eax
    mov dword ptr [rsp + 0x68],eax
    call EXTR_memcpy wrt ..plt
    mov r8d,dword ptr [rsp + 0x68]
    mov rdx,qword ptr [rsi + 0xf0]
    lea rcx,[r12 + rbp*0x2]
    call EXTR_memcpy wrt ..plt
    mov r8d,dword ptr [rsp + 0x68]
    mov rdx,qword ptr [rsi + 0xf8]
    mov rcx,qword ptr [rsp + 0x78]
    call EXTR_memcpy wrt ..plt
    cmp word ptr [rsi + 0x8a],0x2
    jnz .lbl18000ab1f
    movsxd r11,dword ptr [rsp + 0x140]
    movsxd r9,dword ptr [rsp + 0x80]
    lea r10,[r12 + rbp*0x2]
    lea r8,[r11 + -0x1]
    imul r8,r9
    imul r9,r11
    movzx eax,word ptr [r12 + r9*0x2 + -0x4]
    movzx ecx,word ptr [r12 + r8*0x2 + -0x2]
    add ecx,eax
    movzx eax,word ptr [r12 + r8*0x2 + -0x4]
    add ecx,eax
    mov eax,0x55555556
    imul ecx
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov word ptr [r12 + r9*0x2 + -0x2],dx
    movzx eax,word ptr [r10 + r9*0x2 + -0x4]
    movzx ecx,word ptr [r10 + r8*0x2 + -0x2]
    add ecx,eax
    movzx eax,word ptr [r10 + r8*0x2 + -0x4]
    add ecx,eax
    mov eax,0x55555556
    imul ecx
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov word ptr [r10 + r9*0x2 + -0x2],dx
    mov r10,qword ptr [rsp + 0x78]
    movzx eax,word ptr [r10 + r9*0x2 + -0x4]
    movzx ecx,word ptr [r10 + r8*0x2 + -0x2]
    add ecx,eax
    movzx eax,word ptr [r10 + r8*0x2 + -0x4]
    add ecx,eax
    mov eax,0x55555556
    imul ecx
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov word ptr [r10 + r9*0x2 + -0x2],dx
    jmp .lbl18000ab2c
    .lbl18000ab1f: mov r11d,dword ptr [rsp + 0x140]
    mov r10,qword ptr [rsp + 0x78]
    .lbl18000ab2c: test r15w,r15w
    jz .lbl18000ab4e
    test rbp,rbp
    mov rax,rdi
    jle .lbl18000ab4e
    nop
    nop
    .lbl18000ab40: add word ptr [rbx + rax*0x2],r15w
    add rax,0x1
    cmp rax,rbp
    jl .lbl18000ab40
    .lbl18000ab4e: test rbp,rbp
    mov rdx,rdi
    jle .lbl18000ab70
    nop
    nop
    nop
    .lbl18000ab60: movzx ecx,word ptr [rbx + rdx*0x2]
    add rdx,0x1
    add r13d,ecx
    cmp rdx,rbp
    jl .lbl18000ab60
    .lbl18000ab70: movzx ecx,word ptr [rsi + 0x8a]
    xor edx,edx
    mov eax,r13d
    div dword ptr [rsp + 0x6c]
    cmp ecx,0x2
    lea r15,[r12 + rbp*0x2]
    mov r13d,eax
    mov dword ptr [rsp + 0xa0],eax
    jz .lbl18000abe2
    add ecx,-0x9
    cmp ecx,0x1
    ja .lbl18000ac85
    test rbp,rbp
    jle .lbl18000acf8
    mov r9,qword ptr [rsp + 0xb8]
    mov r10,r12
    mov rdx,r15
    sub r9,r12
    sub r10,r15
    mov r8,rbp
    nop
    .lbl18000abc0: movzx eax,word ptr [rdx]
    lea rcx,[r10 + rdx*0x1]
    add rdx,0x2
    mov word ptr [rcx + r9*0x1],ax
    movzx eax,word ptr [rcx]
    sub word ptr [rdx + -0x2],ax
    sub r8,0x1
    jnz .lbl18000abc0
    jmp .lbl18000ac85
    .lbl18000abe2: test rbp,rbp
    jle .lbl18000acf8
    movzx r11d,word ptr [rsp + 0x150]
    mov r14w,word ptr [rsp + 0x148]
    lea rax,[r10 + rbp*0x2]
    mov r13,r10
    sub r15,r12
    mov r8,r12
    sub r13,r12
    sub rax,r12
    mov r10,rbp
    mov qword ptr [rsp + 0xc8],rax
    nop
    nop
    .lbl18000ac20: test r14w,r14w
    movzx r9d,word ptr [r8]
    jz .lbl18000ac33
    movzx eax,word ptr [r13 + r8*0x1]
    sub r9d,eax
    .lbl18000ac33: movzx ecx,word ptr [r15 + r8*0x1]
    mov eax,0x51eb851f
    add r8,0x2
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov rax,qword ptr [rsp + 0xc8]
    add dx,r9w
    sub r10,0x1
    mov word ptr [r8 + rax*0x1 + -0x2],dx
    jnz .lbl18000ac20
    mov r14,qword ptr [rsp + 0x138]
    mov r11d,dword ptr [rsp + 0x140]
    mov r13d,dword ptr [rsp + 0xa0]
    lea r15,[r12 + rbp*0x2]
    .lbl18000ac85: test rbp,rbp
    jle .lbl18000acf8
    mov rax,qword ptr [rsp + 0xb8]
    mov r9,r12
    mov r10,rbx
    sub r9,rax
    mov r8,rbp
    mov r11d,edi
    mov rdx,rax
    mov r14d,edi
    sub r10,rax
    nop
    nop
    .lbl18000acb0: movzx ecx,word ptr [rdx]
    movzx eax,word ptr [r9 + rdx*0x1]
    add r14d,eax
    add edi,ecx
    cmp cx,word ptr [r10 + rdx*0x1]
    jnc .lbl18000acc8
    add r11d,0x1
    .lbl18000acc8: add rdx,0x2
    sub r8,0x1
    jnz .lbl18000acb0
    mov dword ptr [rsp + 0x84],edi
    mov dword ptr [rsp + 0x88],r14d
    mov r14,qword ptr [rsp + 0x138]
    mov dword ptr [rsp + 0x60],r11d
    mov r11d,dword ptr [rsp + 0x140]
    xor edi,edi
    .lbl18000acf8: mov ecx,dword ptr [rsp + 0x6c]
    mov eax,dword ptr [rsp + 0x88]
    xor edx,edx
    div ecx
    mov word ptr [r14 + 0x10],ax
    mov eax,dword ptr [rsp + 0x84]
    cdq
    idiv ecx
    mov r8d,eax
    mov eax,dword ptr [rsp + 0x60]
    imul eax,eax,0x64
    cdq
    idiv ecx
    test r8d,r8d
    mov word ptr [r14 + 0x8],ax
    jle .lbl18000ad3b
    mov eax,r13d
    imul eax,eax,0x64
    cdq
    idiv r8d
    mov dword ptr [r14 + 0xc],eax
    .lbl18000ad3b: mov word ptr [r14 + 0x12],r13w
    movzx ecx,word ptr [rsi + 0x8a]
    sub ecx,0x9
    jz .lbl18000ade3
    cmp ecx,0x1
    mov r9,r12
    mov edx,r11d
    mov rcx,rbx
    jz .lbl18000ad9a
    mov r15d,dword ptr [rsp + 0x80]
    mov eax,0xfff
    mov dword ptr [rsp + 0x30],eax
    mov dword ptr [rsp + 0x60],eax
    mov ax,word ptr [rsp + 0x148]
    mov word ptr [rsp + 0x28],ax
    mov rax,qword ptr [rsp + 0x78]
    mov r8d,r15d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180006ef0
    mov r13d,eax
    jmp .lbl18000aef1
    .lbl18000ad9a: mov ax,word ptr [rsp + 0x148]
    mov dword ptr [rsp + 0x40],0xffff
    mov dword ptr [rsp + 0x38],r8d
    mov word ptr [rsp + 0x30],ax
    mov rax,qword ptr [rsp + 0x78]
    mov dword ptr [rsp + 0x60],0xffff
    mov qword ptr [rsp + 0x28],rax
    mov qword ptr [rsp + 0x20],r15
    mov r15d,dword ptr [rsp + 0x80]
    mov r8d,r15d
    call EXTR_FUN_180006cb0
    mov r13d,eax
    jmp .lbl18000aef1
    .lbl18000ade3: mov r8d,dword ptr [rsp + 0x68]
    mov rcx,qword ptr [rsp + 0x98]
    mov rdx,rbx
    mov dword ptr [rsp + 0x60],0xffff
    call EXTR_memcpy wrt ..plt
    mov r15d,dword ptr [rsp + 0x80]
    mov r13d,dword ptr [rsp + 0x140]
    mov r9,qword ptr [rsp + 0xd0]
    mov rcx,qword ptr [rsp + 0x98]
    mov r8d,r15d
    mov edx,r13d
    call EXTR_FUN_1800085d0
    mov ax,word ptr [rsp + 0x148]
    mov rcx,qword ptr [rsp + 0x98]
    mov dword ptr [rsp + 0x30],0xffff
    mov word ptr [rsp + 0x28],ax
    mov rax,qword ptr [rsp + 0x78]
    lea r9,[r12 + rbp*0x2]
    mov r8d,r15d
    mov edx,r13d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180006ef0
    mov r8d,dword ptr [rsp + 0x68]
    mov rcx,qword ptr [rsp + 0x78]
    mov rdx,r12
    call EXTR_memcpy wrt ..plt
    mov ax,word ptr [rsp + 0x148]
    mov dword ptr [rsp + 0x30],0xffff
    mov word ptr [rsp + 0x28],ax
    mov rax,qword ptr [rsp + 0x78]
    lea r9,[r12 + rbp*0x2]
    mov r8d,r15d
    mov edx,r13d
    mov rcx,rax
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180006ef0
    mov ax,word ptr [rsp + 0x148]
    mov rcx,qword ptr [rsp + 0x98]
    mov dword ptr [rsp + 0x30],0xffff
    mov word ptr [rsp + 0x28],ax
    mov rax,qword ptr [rsp + 0x78]
    mov r8d,r15d
    mov edx,r13d
    mov r9,rax
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180006ef0
    mov r8d,dword ptr [rsp + 0x68]
    mov rdx,qword ptr [rsp + 0x98]
    mov rcx,rbx
    mov r13d,eax
    call EXTR_memcpy wrt ..plt
    .lbl18000aef1: test r13d,r13d
    jnz .lbl18000b29e
    cmp rbp,0x2
    mov edx,edi
    mov r8d,edi
    mov r10d,edi
    mov rcx,rdi
    jl .lbl18000af27
    lea r9,[rbp + -0x1]
    nop
    .lbl18000af10: movzx eax,word ptr [rbx + rcx*0x2]
    add rcx,0x2
    add edx,eax
    movzx eax,word ptr [rbx + rcx*0x2 + -0x2]
    add r8d,eax
    cmp rcx,r9
    jl .lbl18000af10
    .lbl18000af27: cmp rcx,rbp
    jge .lbl18000af31
    movzx r10d,word ptr [rbx + rcx*0x2]
    .lbl18000af31: lea eax,[rdx + r8*0x1]
    xor edx,edx
    mov r13d,0x1
    add eax,r10d
    mov r8,rdi
    div dword ptr [rsp + 0x6c]
    test eax,eax
    mov r9d,eax
    cmovz r9d,r13d
    test rbp,rbp
    jle .lbl18000af85
    mov r10d,dword ptr [rsp + 0xa0]
    nop
    .lbl18000af60: movzx eax,word ptr [rbx + r8*0x2]
    xor edx,edx
    mov ecx,0xffff
    imul eax,r10d
    div r9d
    cmp eax,ecx
    cmovc ecx,eax
    mov word ptr [rbx + r8*0x2],cx
    add r8,r13
    cmp r8,rbp
    jl .lbl18000af60
    .lbl18000af85: mov eax,dword ptr [rsp + 0x60]
    mov ebp,dword ptr [rsp + 0x140]
    mov r9d,dword ptr [rsp + 0x64]
    mov dword ptr [rsp + 0x30],eax
    mov rax,qword ptr [rsp + 0xb8]
    mov r8d,r15d
    mov qword ptr [rsp + 0x28],rax
    mov eax,dword ptr [rsp + 0x70]
    mov edx,ebp
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180007260
    mov r8d,r15d
    mov edx,ebp
    mov word ptr [r14 + 0xa],ax
    test byte ptr [rsi + 0x7c],r13b
    mov rcx,rbx
    jz .lbl18000afe5
    mov qword ptr [rsp + 0x30],r12
    mov r9d,r13d
    mov dword ptr [rsp + 0x28],edi
    mov dword ptr [rsp + 0x20],edi
    call EXTR_FUN_18000a260
    jmp .lbl18000b05a
    .lbl18000afe5: mov r13,qword ptr [rsp + 0xd8]
    movzx r9d,word ptr [rsi + 0x8e]
    lea rax,[rsp + 0x8c]
    mov qword ptr [rsp + 0x50],r13
    mov qword ptr [rsp + 0x48],rax
    mov eax,dword ptr [rsp + 0x6c]
    mov dword ptr [rsp + 0x40],eax
    mov eax,dword ptr [rsp + 0xa4]
    mov qword ptr [rsp + 0x38],r12
    mov dword ptr [rsp + 0x30],eax
    mov eax,dword ptr [rsp + 0xc0]
    mov dword ptr [rsp + 0x28],eax
    mov eax,dword ptr [rsp + 0xb0]
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_18000a0b0
    mov eax,dword ptr [rsp + 0x8c]
    mov r9,r12
    mov r8d,r15d
    mov edx,ebp
    mov rcx,rbx
    mov qword ptr [rsp + 0x28],r13
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180007b10
    .lbl18000b05a: mov r9,qword ptr [rsp + 0x90]
    mov r13d,eax
    mov ax,word ptr [rsp + 0x130]
    mov qword ptr [rsp + 0x38],r12
    mov word ptr [rsp + 0x30],ax
    mov eax,dword ptr [rsp + 0x70]
    mov dword ptr [rsp + 0x28],eax
    mov eax,dword ptr [rsp + 0x64]
    mov r8d,r15d
    mov edx,ebp
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_1800078a0
    movsxd rax,dword ptr [rsp + 0x74]
    test eax,eax
    mov r9,rax
    jle .lbl18000b0da
    mov rax,qword ptr [rsp + 0x90]
    mov r14,qword ptr [rsp + 0x158]
    mov rcx,rax
    sub r14,rax
    nop
    nop
    nop
    .lbl18000b0c0: movzx eax,byte ptr [r14 + rcx*0x1]
    add rcx,0x1
    and byte ptr [rcx + -0x1],al
    sub r9,0x1
    jnz .lbl18000b0c0
    mov r14,qword ptr [rsp + 0x138]
    .lbl18000b0da: cmp word ptr [rsi + 0x8a],0x2
    jnz .lbl18000b106
    mov eax,dword ptr [rsp + 0x70]
    mov r9d,dword ptr [rsp + 0x64]
    mov r8d,r15d
    mov edx,ebp
    mov rcx,rbx
    mov qword ptr [rsp + 0x28],r12
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180007d70
    mov r13d,eax
    .lbl18000b106: mov r15d,dword ptr [rsp + 0x70]
    mov ebp,dword ptr [rsp + 0x64]
    movzx r9d,word ptr [rsi + 0xb0]
    mov r8d,r15d
    mov edx,ebp
    mov rcx,rbx
    mov qword ptr [rsp + 0x20],r12
    call EXTR_FUN_1800082c0
    mov word ptr [r14 + 0x14],ax
    cmp word ptr [rsi + 0x8a],0xa
    jnz .lbl18000b148
    mov r9,r12
    mov r8d,r15d
    mov edx,ebp
    mov rcx,rbx
    call EXTR_FUN_1800085d0
    .lbl18000b148: test byte ptr [rsi + 0x7c],0x2
    jz .lbl18000b15e
    mov r8,r12
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_FUN_1800155d0
    jmp .lbl18000b172
    .lbl18000b15e: mov r8d,dword ptr [rsp + 0x74]
    mov rcx,qword ptr [rsp + 0xa8]
    mov dl,0x1
    call EXTR_memset wrt ..plt
    .lbl18000b172: cmp word ptr [rsi + 0x8a],0x9
    jnz .lbl18000b18c
    mov r9,r12
    mov r8d,r15d
    mov edx,ebp
    mov rcx,rbx
    call EXTR_FUN_1800085d0
    .lbl18000b18c: movzx eax,word ptr [rsi + 0x9e]
    movzx ecx,word ptr [r14 + 0xa]
    mov r9,qword ptr [rsp + 0x90]
    mov r8d,dword ptr [rsp + 0x64]
    mov word ptr [rsp + 0x40],ax
    movzx eax,word ptr [rsi + 0x9c]
    mov word ptr [rsp + 0x38],ax
    movzx eax,word ptr [rsi + 0x9a]
    mov edx,r15d
    mov word ptr [rsp + 0x30],ax
    movzx eax,word ptr [rsi + 0x98]
    mov ebp,edi
    mov word ptr [rsp + 0x28],ax
    mov dword ptr [rsp + 0x20],ecx
    mov rcx,rbx
    mov r12d,edi
    call EXTR_FUN_180008830
    mov rdx,qword ptr [rsp + 0x90]
    or word ptr [r14],ax
    movsxd rax,dword ptr [rsp + 0x74]
    mov rcx,rdi
    cmovz rdx,qword ptr [rsp + 0xa8]
    cmp rax,0x2
    mov r15,rax
    jl .lbl18000b231
    lea r8,[rax + -0x1]
    nop
    .lbl18000b210: cmp byte ptr [rcx + rdx*0x1],0x0
    mov eax,edi
    setnz al
    add ebp,eax
    cmp byte ptr [rcx + rdx*0x1 + 0x1],0x0
    mov eax,edi
    setnz al
    add rcx,0x2
    add r12d,eax
    cmp rcx,r8
    jl .lbl18000b210
    .lbl18000b231: cmp rcx,r15
    jge .lbl18000b23e
    cmp byte ptr [rcx + rdx*0x1],0x0
    setnz dil
    .lbl18000b23e: xor edx,edx
    lea eax,[rbp + r12*0x1]
    add eax,edi
    mov edi,dword ptr [rsp + 0x74]
    lea r8d,[rdi + rdi*0x1]
    imul eax,eax,0x64
    div edi
    mov rdx,rbx
    mov word ptr [r14 + 0x2],ax
    mov rcx,qword ptr [rsi + 0x110]
    call EXTR_memcpy wrt ..plt
    cmp word ptr [rsi + 0x8a],0x2
    jnz .lbl18000b29e
    mov rdx,qword ptr [rsp + 0x90]
    lea rcx,[rbx + r15*0x2]
    mov r8d,edi
    call EXTR_memcpy wrt ..plt
    mov rdx,qword ptr [rsp + 0xa8]
    lea r11,[rbx + r15*0x2]
    lea rcx,[r15 + r11*0x1]
    mov r8d,edi
    call EXTR_memcpy wrt ..plt
    .lbl18000b29e: mov eax,r13d
    jmp .lbl18000b2a8
    .lbl18000b2a3: mov eax,0x1
    .lbl18000b2a8: add rsp,0xe8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_18000a0b0:
    mov r11,rsp
    mov dword ptr [r11 + 0x20],r9d
    mov dword ptr [rsp + 0x10],edx
    push rbp
    push rsi
    push r14
    sub rsp,0x60
    lea eax,[r9 + r9*0x1 + 0x1]
    movsxd rsi,r8d
    mov r14,rcx
    imul eax,eax
    test rcx,rcx
    mov dword ptr [rsp + 0x80],eax
    jz .lbl18000a248
    cmp qword ptr [rsp + 0xb8],0x0
    jz .lbl18000a248
    mov rbp,qword ptr [rsp + 0xc8]
    test rbp,rbp
    jz .lbl18000a248
    cmp dword ptr [rsp + 0xc0],0x0
    jz .lbl18000a248
    mov qword ptr [r11 + 0x18],rbx
    mov qword ptr [r11 + -0x20],rdi
    mov qword ptr [r11 + -0x28],r12
    xor r12d,r12d
    test edx,edx
    mov qword ptr [r11 + -0x30],r13
    mov qword ptr [r11 + -0x38],r15
    mov dword ptr [rbp],0x0
    jle .lbl18000a221
    mov r15d,dword ptr [rsp + 0xb0]
    nop
    .lbl18000a140: xor ebx,ebx
    test esi,esi
    jle .lbl18000a214
    movsxd r13,r12d
    imul r13,rsi
    .lbl18000a151: movsxd rax,ebx
    add rax,r13
    movzx edi,word ptr [r14 + rax*0x2]
    cmp edi,dword ptr [rsp + 0xa0]
    jl .lbl18000a172
    cmp edi,dword ptr [rsp + 0xa8]
    jle .lbl18000a209
    .lbl18000a172: mov dword ptr [rsp + 0x30],r9d
    mov r9,qword ptr [rsp + 0xd0]
    mov r8d,edx
    mov edx,esi
    mov rcx,r14
    mov dword ptr [rsp + 0x28],ebx
    mov dword ptr [rsp + 0x20],r12d
    call EXTR_FUN_180007460
    mov edx,dword ptr [rsp + 0x80]
    mov rcx,qword ptr [rsp + 0xd0]
    call EXTR_FUN_180007570
    sub edi,eax
    mov eax,r15d
    neg eax
    cmp edi,eax
    jl .lbl18000a1b9
    cmp edi,r15d
    jle .lbl18000a1fa
    .lbl18000a1b9: mov eax,dword ptr [rsp + 0xc0]
    movsxd rdx,dword ptr [rbp]
    add eax,-0x1
    cmp edx,eax
    jg .lbl18000a221
    mov r9d,dword ptr [rsp + 0x98]
    mov rax,rdx
    mov rdx,qword ptr [rsp + 0xb8]
    movzx ecx,si
    imul cx,r12w
    add cx,bx
    mov word ptr [rdx + rax*0x2],cx
    add dword ptr [rbp],0x1
    mov edx,dword ptr [rsp + 0x88]
    jmp .lbl18000a209
    .lbl18000a1fa: mov edx,dword ptr [rsp + 0x88]
    mov r9d,dword ptr [rsp + 0x98]
    .lbl18000a209: add ebx,0x1
    cmp ebx,esi
    jl .lbl18000a151
    .lbl18000a214: add r12d,0x1
    cmp r12d,edx
    jl .lbl18000a140
    .lbl18000a221: mov r13,qword ptr [rsp + 0x48]
    mov r12,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    mov rbx,qword ptr [rsp + 0x90]
    mov r15,qword ptr [rsp + 0x40]
    xor eax,eax
    add rsp,0x60
    pop r14
    pop rsi
    pop rbp
    ret
    .lbl18000a248: xor eax,eax
    add rsp,0x60
    pop r14
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180007460:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x10
    mov eax,dword ptr [rsp + 0x68]
    movsxd r10,dword ptr [rsp + 0x70]
    movsxd r11,dword ptr [rsp + 0x60]
    mov r14,r9
    mov r9d,eax
    add eax,r10d
    sub r9d,r10d
    xor r13d,r13d
    movsxd rbp,r8d
    cmp r9d,eax
    movsxd rsi,edx
    mov r15,rcx
    mov r8,r11
    movsxd rbx,r9d
    mov dword ptr [rsp + 0x68],eax
    jg .lbl180007548
    sub r8,r10
    mov edx,r11d
    sub edx,r10d
    mov qword ptr [rsp],r8
    add r11d,r10d
    mov dword ptr [rsp + 0x70],edx
    mov rcx,r8
    nop
    .lbl1800074d0: cmp edx,r11d
    mov r8d,edx
    mov r10,rcx
    jg .lbl18000753b
    nop
    nop
    .lbl1800074e0: test r9d,r9d
    jns .lbl1800074e9
    xor edx,edx
    jmp .lbl1800074f5
    .lbl1800074e9: cmp r9d,esi
    lea rdx,[rsi + -0x1]
    jge .lbl1800074f5
    mov rdx,rbx
    .lbl1800074f5: test r8d,r8d
    jns .lbl1800074fe
    xor ecx,ecx
    jmp .lbl18000750a
    .lbl1800074fe: cmp r8d,ebp
    lea rcx,[rbp + -0x1]
    jge .lbl18000750a
    mov rcx,r10
    .lbl18000750a: mov rax,rsi
    add r8d,0x1
    add r13,0x1
    add r10,0x1
    imul rax,rcx
    add rax,rdx
    cmp r8d,r11d
    movzx eax,word ptr [r15 + rax*0x2]
    mov dword ptr [r14 + r13*0x4 + -0x4],eax
    jle .lbl1800074e0
    mov eax,dword ptr [rsp + 0x68]
    mov edx,dword ptr [rsp + 0x70]
    mov rcx,qword ptr [rsp]
    .lbl18000753b: add r9d,0x1
    add rbx,0x1
    cmp r9d,eax
    jle .lbl1800074d0
    .lbl180007548: mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x10
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180007570:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    lea esi,[rdx + -0x1]
    xor r12d,r12d
    mov r9,rcx
    mov eax,esi
    movsxd rdi,esi
    mov rbx,r12
    cdq
    sub eax,edx
    sar eax,1
    test esi,esi
    movsxd rbp,eax
    jle .lbl1800076c5
    nop
    nop
    nop
    .lbl1800075b0: lea r10d,[r12 + 0x1]
    cmp esi,r10d
    jz .lbl1800076aa
    mov ecx,dword ptr [r9 + rdi*0x4]
    lea eax,[rsi + r12*0x1]
    cdq
    sub eax,edx
    sar eax,1
    movsxd r11,eax
    mov r8d,dword ptr [r9 + r11*0x4]
    cmp r8d,ecx
    jle .lbl1800075df
    mov dword ptr [r9 + r11*0x4],ecx
    mov dword ptr [r9 + rdi*0x4],r8d
    .lbl1800075df: mov ecx,dword ptr [r9 + rbx*0x4]
    mov eax,dword ptr [r9 + rdi*0x4]
    cmp ecx,eax
    jle .lbl1800075f3
    mov dword ptr [r9 + rbx*0x4],eax
    mov dword ptr [r9 + rdi*0x4],ecx
    .lbl1800075f3: mov ecx,dword ptr [r9 + r11*0x4]
    mov eax,dword ptr [r9 + rbx*0x4]
    cmp ecx,eax
    jle .lbl180007607
    mov dword ptr [r9 + r11*0x4],eax
    mov dword ptr [r9 + rbx*0x4],ecx
    .lbl180007607: cmp r11d,r10d
    jle .lbl18000761e
    mov eax,dword ptr [r9 + rbx*0x4 + 0x4]
    mov ecx,dword ptr [r9 + r11*0x4]
    mov dword ptr [r9 + r11*0x4],eax
    mov dword ptr [r9 + rbx*0x4 + 0x4],ecx
    .lbl18000761e: lea r8,[rbx + 0x1]
    mov r11d,esi
    mov rdx,rdi
    cmp r8,rdi
    jge .lbl180007675
    nop
    .lbl180007630: mov eax,dword ptr [r9 + rbx*0x4]
    nop
    nop
    nop
    .lbl180007640: add r8,0x1
    add r10d,0x1
    cmp eax,dword ptr [r9 + r8*0x4]
    jg .lbl180007640
    nop
    .lbl180007650: sub rdx,0x1
    sub r11d,0x1
    cmp dword ptr [r9 + rdx*0x4],eax
    jg .lbl180007650
    cmp r8,rdx
    jge .lbl180007675
    mov eax,dword ptr [r9 + rdx*0x4]
    mov ecx,dword ptr [r9 + r8*0x4]
    mov dword ptr [r9 + r8*0x4],eax
    mov dword ptr [r9 + rdx*0x4],ecx
    jmp .lbl180007630
    .lbl180007675: cmp rbx,rdx
    jz .lbl18000768a
    mov eax,dword ptr [r9 + rdx*0x4]
    mov ecx,dword ptr [r9 + rbx*0x4]
    mov dword ptr [r9 + rbx*0x4],eax
    mov dword ptr [r9 + rdx*0x4],ecx
    .lbl18000768a: cmp rdx,rbp
    jg .lbl180007697
    mov r12d,r10d
    mov rbx,r8
    jl .lbl18000769f
    .lbl180007697: lea esi,[r11 + -0x1]
    lea rdi,[rdx + -0x1]
    .lbl18000769f: cmp rbx,rdi
    jl .lbl1800075b0
    jmp .lbl1800076c5
    .lbl1800076aa: movsxd rcx,esi
    movsxd rdx,r12d
    mov r10d,dword ptr [r9 + rdx*0x4]
    mov r8d,dword ptr [r9 + rcx*0x4]
    cmp r10d,r8d
    jle .lbl1800076c5
    mov dword ptr [r9 + rdx*0x4],r8d
    mov dword ptr [r9 + rcx*0x4],r10d
    .lbl1800076c5: mov eax,dword ptr [r9 + rbp*0x4]
    mov rbx,qword ptr [rsp + 0x10]
    mov rsi,qword ptr [rsp + 0x20]
    mov rbp,qword ptr [rsp + 0x18]
    mov rdi,qword ptr [rsp + 0x28]
    pop r12
    ret

section .data


section .text
EXTR_FUN_180008830:
    mov rax,rsp
    mov qword ptr [rax + 0x20],r9
    mov qword ptr [rax + 0x8],rcx
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov qword ptr [rax + -0x20],rbx
    mov qword ptr [rax + -0x28],rbp
    mov qword ptr [rax + -0x30],rsi
    mov qword ptr [rax + -0x38],rdi
    mov qword ptr [rax + -0x40],r12
    mov eax,edx
    xor r14d,r14d
    mov r15d,0x10000000
    mov qword ptr [rsp + 0x10],0x0
    imul eax,r8d
    test r8d,r8d
    mov dword ptr [rsp],eax
    movsxd rax,r8d
    mov dword ptr [rsp + 0x80],r14d
    mov r13d,r15d
    mov dword ptr [rsp + 0x78],r15d
    mov qword ptr [rsp + 0x8],rax
    movsxd rbp,edx
    jle .lbl180008955
    xor ebx,ebx
    lea r12,[rbp + rbp*0x1]
    mov rdi,rcx
    mov r15,rbx
    mov rsi,rax
    nop
    nop
    .lbl1800088b0: xor r9d,r9d
    xor r10d,r10d
    xor r8d,r8d
    xor r11d,r11d
    cmp rbp,0x2
    jl .lbl1800088fc
    lea rdx,[rbp + -0x2]
    mov rcx,rdi
    shr rdx,1
    add rdx,0x1
    lea r11,[rdx + rdx*0x1]
    nop
    nop
    nop
    .lbl1800088e0: movzx eax,word ptr [rcx]
    add rcx,0x4
    add r9d,eax
    movzx eax,word ptr [rcx + -0x2]
    add r10d,eax
    sub rdx,0x1
    jnz .lbl1800088e0
    mov rcx,qword ptr [rsp + 0x70]
    .lbl1800088fc: cmp r11,rbp
    jge .lbl18000890a
    lea rax,[rbx + r11*0x1]
    movzx r8d,word ptr [rcx + rax*0x2]
    .lbl18000890a: lea eax,[r10 + r9*0x1]
    add r8d,eax
    mov eax,r8d
    add r15,rax
    cmp r8d,r14d
    jbe .lbl180008921
    mov r14d,r8d
    jmp .lbl180008928
    .lbl180008921: cmp r8d,r13d
    cmovc r13d,r8d
    .lbl180008928: add rbx,rbp
    add rdi,r12
    sub rsi,0x1
    jnz .lbl1800088b0
    mov rax,qword ptr [rsp + 0x8]
    mov qword ptr [rsp + 0x10],r15
    mov dword ptr [rsp + 0x80],r14d
    mov r15d,0x10000000
    mov dword ptr [rsp + 0x78],r13d
    .lbl180008955: xor r14d,r14d
    xor ebx,ebx
    test rbp,rbp
    jle .lbl180008a0d
    mov r12,rcx
    lea r13,[rcx + rbp*0x2]
    nop
    nop
    .lbl180008970: xor r10d,r10d
    xor r11d,r11d
    xor ecx,ecx
    xor esi,esi
    cmp rax,0x2
    jl .lbl1800089bf
    lea rdx,[rax + -0x2]
    lea rdi,[rbp*0x4]
    mov r8,r12
    shr rdx,1
    mov r9,r13
    add rdx,0x1
    lea rsi,[rdx + rdx*0x1]
    nop
    .lbl1800089a0: movzx eax,word ptr [r8]
    add r8,rdi
    add r10d,eax
    movzx eax,word ptr [r9]
    add r9,rdi
    add r11d,eax
    sub rdx,0x1
    jnz .lbl1800089a0
    mov rax,qword ptr [rsp + 0x8]
    .lbl1800089bf: cmp rsi,rax
    jge .lbl1800089d7
    mov rcx,qword ptr [rsp + 0x70]
    mov rax,rbp
    imul rax,rsi
    add rax,rbx
    movzx ecx,word ptr [rcx + rax*0x2]
    .lbl1800089d7: lea eax,[r11 + r10*0x1]
    add ecx,eax
    cmp ecx,r14d
    jbe .lbl1800089e7
    mov r14d,ecx
    jmp .lbl1800089ee
    .lbl1800089e7: cmp ecx,r15d
    cmovc r15d,ecx
    .lbl1800089ee: mov rax,qword ptr [rsp + 0x8]
    add rbx,0x1
    add r12,0x2
    add r13,0x2
    cmp rbx,rbp
    jl .lbl180008970
    mov r13d,dword ptr [rsp + 0x78]
    .lbl180008a0d: movsxd rcx,dword ptr [rsp]
    mov r12,qword ptr [rsp + 0x28]
    mov rdi,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    mov rbp,qword ptr [rsp + 0x40]
    mov rbx,qword ptr [rsp + 0x48]
    xor r8d,r8d
    xor eax,eax
    test rcx,rcx
    jle .lbl180008a53
    mov rdx,qword ptr [rsp + 0x88]
    nop
    .lbl180008a40: cmp byte ptr [rax + rdx*0x1],0x0
    jnz .lbl180008a4a
    add r8d,0x1
    .lbl180008a4a: add rax,0x1
    cmp rax,rcx
    jl .lbl180008a40
    .lbl180008a53: mov rax,qword ptr [rsp + 0x10]
    xor edx,edx
    div rcx
    cmp ax,word ptr [rsp + 0x98]
    ja .lbl180008acc
    mov ecx,dword ptr [rsp + 0x80]
    movzx r9d,word ptr [rsp + 0xb0]
    xor edx,edx
    mov eax,ecx
    add ecx,0x1
    sub eax,r13d
    imul eax,eax,0x64
    div ecx
    cmp eax,r9d
    ja .lbl180008acc
    xor edx,edx
    mov eax,r14d
    lea ecx,[r14 + 0x1]
    sub eax,r15d
    imul eax,eax,0x64
    div ecx
    cmp eax,r9d
    ja .lbl180008acc
    movzx ecx,word ptr [rsp + 0xa0]
    cmp r8d,ecx
    jg .lbl180008acc
    movzx ecx,word ptr [rsp + 0xa8]
    cmp dword ptr [rsp + 0x90],ecx
    jg .lbl180008acc
    xor eax,eax
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    ret
    .lbl180008acc: mov eax,0x1
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    ret

section .data


section .text
EXTR_FUN_180007d70:
    mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x8],rcx
    push r13
    push r14
    sub rsp,0x58
    mov r14d,dword ptr [rsp + 0x90]
    movsxd r13,edx
    mov eax,r8d
    cdq
    mov r11,rcx
    idiv r14d
    test r11,r11
    mov ecx,eax
    mov dword ptr [rsp + 0x10],eax
    jz .lbl1800082ab
    test r8d,r8d
    jz .lbl1800082ab
    test r13d,r13d
    jz .lbl1800082ab
    cmp eax,0x2
    jl .lbl1800082ab
    mov qword ptr [rsp + 0x78],rbx
    mov qword ptr [rsp + 0x50],rbp
    mov qword ptr [rsp + 0x48],rsi
    mov qword ptr [rsp + 0x40],rdi
    xor edi,edi
    test r8d,r8d
    mov qword ptr [rsp + 0x38],r12
    mov qword ptr [rsp + 0x30],r15
    movsxd r10,r8d
    jle .lbl180007fbe
    mov eax,0x1
    mov dword ptr [rsp],eax
    nop
    nop
    .lbl180007e00: test r13d,r13d
    jle .lbl180007f97
    lea ecx,[rax + -0x2]
    mov rax,qword ptr [rsp + 0x98]
    mov rbp,r11
    xor r15d,r15d
    mov ebx,0x1
    sub rbp,rax
    mov dword ptr [rsp + 0x8],ecx
    lea r14,[rax + rdi*0x2]
    mov qword ptr [rsp + 0x18],r13
    mov qword ptr [rsp + 0x20],rbp
    mov r8,rbx
    .lbl180007e37: test ecx,ecx
    js .lbl180007e41
    lea rdx,[rdi + -0x1]
    jmp .lbl180007e43
    .lbl180007e41: xor edx,edx
    .lbl180007e43: lea r12d,[rbx + -0x2]
    test r12d,r12d
    js .lbl180007e52
    lea rcx,[r8 + -0x2]
    jmp .lbl180007e54
    .lbl180007e52: xor ecx,ecx
    .lbl180007e54: mov rax,r10
    imul rax,rcx
    add rax,rdx
    movzx eax,word ptr [r11 + rax*0x2]
    lea ecx,[rax + rax*0x8]
    lea rax,[rdx + r15*0x1]
    movzx eax,word ptr [r11 + rax*0x2]
    movsxd r9,ecx
    imul eax,eax,0xf
    cdqe
    add r9,rax
    cmp ebx,r13d
    jge .lbl180007e84
    mov rcx,r8
    jmp .lbl180007e88
    .lbl180007e84: lea rcx,[r13 + -0x1]
    .lbl180007e88: mov rax,r10
    imul rax,rcx
    add rax,rdx
    movzx eax,word ptr [r11 + rax*0x2]
    lea eax,[rax + rax*0x8]
    cdqe
    add r9,rax
    test r12d,r12d
    js .lbl180007eaa
    lea rcx,[r8 + -0x2]
    jmp .lbl180007eac
    .lbl180007eaa: xor ecx,ecx
    .lbl180007eac: mov rax,r10
    imul rax,rcx
    add rax,rdi
    movzx eax,word ptr [r11 + rax*0x2]
    imul eax,eax,0xf
    movsxd rdx,eax
    movzx eax,word ptr [r14 + rbp*0x1]
    imul eax,eax,0x19
    movsxd rbp,eax
    add rbp,r9
    add rbp,rdx
    cmp ebx,r13d
    jge .lbl180007edc
    mov rcx,r8
    jmp .lbl180007ee0
    .lbl180007edc: lea rcx,[r13 + -0x1]
    .lbl180007ee0: mov rax,r10
    imul rax,rcx
    add rax,rdi
    movzx eax,word ptr [r11 + rax*0x2]
    imul eax,eax,0xf
    cdqe
    add rbp,rax
    mov eax,dword ptr [rsp]
    cmp eax,dword ptr [rsp + 0x80]
    jge .lbl180007f09
    lea r9,[rdi + 0x1]
    jmp .lbl180007f0d
    .lbl180007f09: lea r9,[r10 + -0x1]
    .lbl180007f0d: test r12d,r12d
    js .lbl180007f18
    lea rcx,[r8 + -0x2]
    jmp .lbl180007f1a
    .lbl180007f18: xor ecx,ecx
    .lbl180007f1a: mov rax,r10
    imul rax,rcx
    add rax,r9
    movzx eax,word ptr [r11 + rax*0x2]
    lea ecx,[rax + rax*0x8]
    lea rax,[r9 + r15*0x1]
    movzx eax,word ptr [r11 + rax*0x2]
    movsxd rdx,ecx
    imul eax,eax,0xf
    cdqe
    add rax,rbp
    add rdx,rax
    cmp ebx,r13d
    jge .lbl180007f4d
    mov rcx,r8
    jmp .lbl180007f51
    .lbl180007f4d: lea rcx,[r13 + -0x1]
    .lbl180007f51: mov rbp,qword ptr [rsp + 0x20]
    mov rax,r10
    add ebx,0x1
    add r8,0x1
    add r15,r10
    imul rax,rcx
    add rax,r9
    movzx eax,word ptr [r11 + rax*0x2]
    lea ecx,[rax + rax*0x8]
    lea rax,[r10 + r10*0x1]
    add rcx,rdx
    sar rcx,0x7
    mov word ptr [r14],cx
    mov ecx,dword ptr [rsp + 0x8]
    add r14,rax
    sub qword ptr [rsp + 0x18],0x1
    jnz .lbl180007e37
    mov eax,dword ptr [rsp]
    .lbl180007f97: add eax,0x1
    add rdi,0x1
    cmp rdi,r10
    mov dword ptr [rsp],eax
    jl .lbl180007e00
    mov ecx,dword ptr [rsp + 0x10]
    mov r9d,dword ptr [rsp + 0x88]
    mov r14d,dword ptr [rsp + 0x90]
    .lbl180007fbe: sub ecx,0x2
    jz .lbl18000818b
    cmp ecx,0x1
    jnz .lbl18000828d
    test r14d,r14d
    jle .lbl18000828d
    mov eax,r14d
    xor ebx,ebx
    xor r15d,r15d
    mov qword ptr [rsp],rax
    .lbl180007fe5: test r9d,r9d
    jle .lbl180008167
    xor r9d,r9d
    movsxd rcx,r14d
    mov ebp,0x2
    add rcx,rcx
    mov qword ptr [rsp + 0x18],r11
    mov r11d,dword ptr [rsp + 0x80]
    mov qword ptr [rsp + 0x20],rcx
    mov ecx,dword ptr [rsp + 0x88]
    lea eax,[r15 + 0x1]
    mov qword ptr [rsp + 0x10],r9
    mov r12,rbp
    mov qword ptr [rsp + 0x8],rcx
    .lbl180008026: cmp eax,r11d
    jge .lbl180008031
    lea r14,[rbx + 0x1]
    jmp .lbl180008035
    .lbl180008031: lea r14,[r10 + -0x1]
    .lbl180008035: lea eax,[rbp + -0x1]
    cmp eax,r13d
    jge .lbl180008044
    lea rcx,[r12 + -0x1]
    jmp .lbl180008048
    .lbl180008044: lea rcx,[r13 + -0x1]
    .lbl180008048: lea eax,[r15 + 0x2]
    cmp eax,r11d
    jge .lbl180008057
    lea rdi,[rbx + 0x2]
    jmp .lbl18000805b
    .lbl180008057: lea rdi,[r10 + -0x1]
    .lbl18000805b: cmp ebp,r13d
    jge .lbl180008065
    mov rax,r12
    jmp .lbl180008069
    .lbl180008065: lea rax,[r13 + -0x1]
    .lbl180008069: mov rdx,r10
    mov r8,r10
    add ebp,0x3
    add r12,0x3
    imul rdx,rcx
    mov rcx,qword ptr [rsp + 0x98]
    imul r8,rax
    lea rax,[r9 + r14*0x1]
    movzx r9d,word ptr [rcx + rax*0x2]
    lea rax,[rdx + rbx*0x1]
    movzx ecx,word ptr [rcx + rax*0x2]
    lea rax,[r8 + rbx*0x1]
    add r9d,ecx
    mov rcx,qword ptr [rsp + 0x98]
    movzx ecx,word ptr [rcx + rax*0x2]
    lea rax,[rdx + rdi*0x1]
    add r9d,ecx
    mov rcx,qword ptr [rsp + 0x98]
    movzx ecx,word ptr [rcx + rax*0x2]
    lea rax,[r8 + rdi*0x1]
    add r9d,ecx
    mov rcx,qword ptr [rsp + 0x98]
    movzx ecx,word ptr [rcx + rax*0x2]
    lea rax,[rdx + r14*0x1]
    mov rdx,qword ptr [rsp + 0x98]
    add r9d,ecx
    movzx ecx,word ptr [rdx + rax*0x2]
    lea rax,[r8 + r14*0x1]
    mov r8,qword ptr [rsp + 0x10]
    add r9d,ecx
    movzx ecx,word ptr [rdx + rax*0x2]
    add r9d,ecx
    lea rax,[rbx + r8*0x1]
    movzx ecx,word ptr [rdx + rax*0x2]
    lea rax,[r8 + rdi*0x1]
    add r9d,ecx
    movzx ecx,word ptr [rdx + rax*0x2]
    mov eax,0x38e38e39
    add r9d,ecx
    mov rcx,qword ptr [rsp + 0x18]
    imul r9d
    sar edx,1
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    lea rax,[r10 + r10*0x2]
    add r8,rax
    mov word ptr [rcx],dx
    add rcx,qword ptr [rsp + 0x20]
    sub qword ptr [rsp + 0x8],0x1
    mov qword ptr [rsp + 0x10],r8
    lea eax,[r15 + 0x1]
    mov qword ptr [rsp + 0x18],rcx
    mov r9,r8
    jnz .lbl180008026
    mov r11,qword ptr [rsp + 0x70]
    mov rax,qword ptr [rsp]
    mov r9d,dword ptr [rsp + 0x88]
    mov r14d,dword ptr [rsp + 0x90]
    .lbl180008167: add r11,0x2
    add r15d,0x3
    add rbx,0x3
    sub rax,0x1
    mov qword ptr [rsp + 0x70],r11
    mov qword ptr [rsp],rax
    jnz .lbl180007fe5
    jmp .lbl18000828d
    .lbl18000818b: test r14d,r14d
    jle .lbl18000828d
    xor ebx,ebx
    mov eax,r14d
    lea r15d,[rbx + 0x1]
    mov qword ptr [rsp + 0x10],rax
    .lbl1800081a2: test r9d,r9d
    jle .lbl180008276
    movsxd rax,r14d
    xor edi,edi
    mov ebp,0x1
    add rax,rax
    lea r14,[rbx + r11*0x1]
    mov r11d,dword ptr [rsp + 0x80]
    mov qword ptr [rsp + 0x20],rax
    mov eax,r9d
    mov r12,rbp
    mov qword ptr [rsp + 0x18],rax
    .lbl1800081d4: cmp r15d,r11d
    jge .lbl1800081df
    lea r9,[rbx + 0x1]
    jmp .lbl1800081e3
    .lbl1800081df: lea r9,[r10 + -0x1]
    .lbl1800081e3: cmp ebp,r13d
    jge .lbl1800081ed
    mov rax,r12
    jmp .lbl1800081f1
    .lbl1800081ed: lea rax,[r13 + -0x1]
    .lbl1800081f1: mov rdx,qword ptr [rsp + 0x98]
    mov r8,r10
    add ebp,0x2
    add r12,0x2
    imul r8,rax
    lea rax,[rbx + rdi*0x1]
    lea rcx,[r8 + r9*0x1]
    movzx eax,word ptr [rdx + rax*0x2]
    movzx edx,word ptr [rdx + rcx*0x2]
    lea rcx,[r8 + rbx*0x1]
    mov r8,qword ptr [rsp + 0x98]
    add eax,edx
    movzx edx,word ptr [r8 + rcx*0x2]
    lea rcx,[rdi + r9*0x1]
    add eax,edx
    movzx edx,word ptr [r8 + rcx*0x2]
    add eax,edx
    cdq
    and edx,0x3
    add eax,edx
    sar eax,0x2
    mov word ptr [r14],ax
    add r14,qword ptr [rsp + 0x20]
    lea rax,[r10 + r10*0x1]
    add rdi,rax
    sub qword ptr [rsp + 0x18],0x1
    jnz .lbl1800081d4
    mov r11,qword ptr [rsp + 0x70]
    mov rax,qword ptr [rsp + 0x10]
    mov r9d,dword ptr [rsp + 0x88]
    mov r14d,dword ptr [rsp + 0x90]
    .lbl180008276: add r15d,0x2
    add rbx,0x2
    sub rax,0x1
    mov qword ptr [rsp + 0x10],rax
    jnz .lbl1800081a2
    .lbl18000828d: mov r15,qword ptr [rsp + 0x30]
    mov r12,qword ptr [rsp + 0x38]
    mov rdi,qword ptr [rsp + 0x40]
    mov rsi,qword ptr [rsp + 0x48]
    mov rbp,qword ptr [rsp + 0x50]
    mov rbx,qword ptr [rsp + 0x78]
    .lbl1800082ab: xor eax,eax
    add rsp,0x58
    pop r14
    pop r13
    ret

section .data


section .text
EXTR_FUN_180006cb0:
    mov qword ptr [rsp + 0x20],r9
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r15
    sub rsp,0x10
    xor r11d,r11d
    mov r15d,edx
    mov rsi,r9
    mov rbx,rcx
    mov r10d,r11d
    mov edi,r11d
    imul r15d,r8d
    cmp dword ptr [rsp + 0x78],r11d
    lea ebp,[r11 + 0x64]
    jg .lbl180006cf3
    lea eax,[rbp + -0x63]
    add rsp,0x10
    pop r15
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret
    .lbl180006cf3: mov r8,qword ptr [rsp + 0x60]
    mov qword ptr [rsp + 0x50],r12
    movsxd r12,r15d
    cmp r12,0x2
    mov qword ptr [rsp + 0x8],r13
    mov edx,r11d
    mov r9d,r11d
    mov rcx,r11
    mov qword ptr [rsp],r14
    jl .lbl180006d3e
    lea rbx,[r12 + -0x1]
    nop
    .lbl180006d20: movzx eax,word ptr [r8 + rcx*0x2]
    add rcx,0x2
    add edx,eax
    movzx eax,word ptr [r8 + rcx*0x2 + -0x2]
    add r9d,eax
    cmp rcx,rbx
    jl .lbl180006d20
    mov rbx,qword ptr [rsp + 0x40]
    .lbl180006d3e: cmp rcx,r12
    jge .lbl180006d48
    movzx r11d,word ptr [r8 + rcx*0x2]
    .lbl180006d48: mov r14,qword ptr [rsp + 0x68]
    lea eax,[r9 + rdx*0x1]
    add eax,r11d
    cdq
    idiv r15d
    movzx edx,word ptr [rsp + 0x70]
    mov r13d,eax
    mov dword ptr [rsp + 0x48],eax
    sar r13d,0x2
    test dx,dx
    jz .lbl180006dbf
    test r15d,r15d
    jle .lbl180006e39
    mov r11,rsi
    mov esi,dword ptr [rsp + 0x80]
    mov rdx,r14
    sub r11,r14
    sub rbx,r14
    mov r9,r12
    nop
    .lbl180006d90: movzx eax,word ptr [rdx]
    movzx ecx,word ptr [r11 + rdx*0x1]
    sub ecx,eax
    movzx eax,word ptr [rbx + rdx*0x1]
    sub eax,ecx
    add edi,ecx
    test eax,eax
    jg .lbl180006dae
    cmp ecx,esi
    jge .lbl180006dae
    add r10d,0x1
    .lbl180006dae: add rdx,0x2
    sub r9,0x1
    jnz .lbl180006d90
    mov rsi,qword ptr [rsp + 0x58]
    jmp .lbl180006e02
    .lbl180006dbf: test r15d,r15d
    jle .lbl180006e39
    mov r11,rsi
    mov rcx,rbx
    mov r9,r12
    sub r11,rbx
    mov ebx,dword ptr [rsp + 0x80]
    nop
    nop
    nop
    .lbl180006de0: movzx edx,word ptr [r11 + rcx*0x1]
    movzx eax,word ptr [rcx]
    sub eax,edx
    add edi,edx
    test eax,eax
    jg .lbl180006df8
    cmp edx,ebx
    jge .lbl180006df8
    add r10d,0x1
    .lbl180006df8: add rcx,0x2
    sub r9,0x1
    jnz .lbl180006de0
    .lbl180006e02: test r10d,r10d
    jle .lbl180006e2d
    mov eax,edi
    cdq
    idiv r15d
    imul eax,eax,0x64
    cdq
    idiv dword ptr [rsp + 0x78]
    cmp eax,0xa
    mov ebp,eax
    jl .lbl180006e21
    cmp eax,0x64
    jle .lbl180006e34
    .lbl180006e21: mov dx,word ptr [rsp + 0x70]
    mov ebp,0x64
    jmp .lbl180006e39
    .lbl180006e2d: mov dx,word ptr [rsp + 0x70]
    jmp .lbl180006e39
    .lbl180006e34: mov dx,word ptr [rsp + 0x70]
    .lbl180006e39: test r15d,r15d
    jle .lbl180006ec8
    mov r11,qword ptr [rsp + 0x40]
    mov ebx,dword ptr [rsp + 0x48]
    sub rsi,r8
    sub r14,r8
    mov edi,0xffff
    mov r10d,0x1
    sub r11,r8
    nop
    .lbl180006e60: movzx eax,word ptr [r8]
    movzx ecx,word ptr [rsi + r8*0x1]
    mov r9d,r13d
    cmp eax,r13d
    cmovg r9d,eax
    test dx,dx
    jz .lbl180006e7f
    movzx eax,word ptr [r14 + r8*0x1]
    sub ecx,eax
    .lbl180006e7f: cmp ebp,0x64
    jge .lbl180006e9a
    imul ecx,ebp
    mov eax,0x51eb851f
    imul ecx
    mov ecx,edx
    sar ecx,0x5
    mov eax,ecx
    shr eax,0x1f
    add ecx,eax
    .lbl180006e9a: movzx eax,word ptr [r11 + r8*0x1]
    sub eax,ecx
    cmp eax,edi
    cmovg eax,edi
    cmp eax,r10d
    cmovl eax,r10d
    add r8,0x2
    imul eax,ebx
    cdq
    idiv r9d
    sub r12,r10
    mov dx,word ptr [rsp + 0x70]
    mov word ptr [r11 + r8*0x1 + -0x2],ax
    jnz .lbl180006e60
    .lbl180006ec8: mov r14,qword ptr [rsp]
    mov r13,qword ptr [rsp + 0x8]
    mov r12,qword ptr [rsp + 0x50]
    xor eax,eax
    add rsp,0x10
    pop r15
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180007260:
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r14
    push r15
    sub rsp,0x40
    mov r12d,dword ptr [rsp + 0xa0]
    mov rsi,qword ptr [rsp + 0xa8]
    mov r10d,edx
    imul r10d,r8d
    mov r15d,edx
    mov eax,r9d
    xor ebx,ebx
    mov r14d,r8d
    mov edi,r9d
    mov rbp,rcx
    imul eax,r12d
    mov dword ptr [rsp + 0x24],eax
    mov eax,r8d
    movsxd r8,r10d
    cdq
    idiv r12d
    xor edx,edx
    mov r11d,eax
    xor eax,eax
    test r10d,r10d
    jle .lbl1800072d0
    nop
    .lbl1800072c0: movzx ecx,word ptr [rsi + rdx*0x2]
    add rdx,0x1
    add rax,rcx
    cmp rdx,r8
    jl .lbl1800072c0
    .lbl1800072d0: cqo
    idiv r8
    mov rcx,rax
    call EXTR_FUN_180006c80
    shl ax,0x2
    test edi,edi
    movzx eax,ax
    mov dword ptr [rsp + 0x90],eax
    jle .lbl18000743f
    mov rcx,rdi
    mov qword ptr [rsp + 0x38],r13
    xor r13d,r13d
    mov qword ptr [rsp + 0x28],rcx
    .lbl180007303: test r12d,r12d
    jle .lbl180007428
    mov ecx,r15d
    mov r10d,r14d
    mov r15d,r11d
    sub ecx,r11d
    sub r10d,r11d
    mov r9d,r12d
    imul r15d,r11d
    add ecx,0x1
    add r10d,0x1
    xor r8d,r8d
    mov dword ptr [rsp + 0x98],ecx
    mov dword ptr [rsp + 0x20],r10d
    nop
    nop
    .lbl180007340: cmp r13d,ecx
    mov eax,r10d
    mov edx,ecx
    cmovl edx,r13d
    cmp r8d,r10d
    cmovl eax,r8d
    imul edx,r14d
    add edx,eax
    cmp r11d,0x1
    movsxd rdi,edx
    movzx r10d,word ptr [rsi + rdi*0x2]
    jle .lbl1800073d7
    lea ecx,[rdx + 0x1]
    add edx,r14d
    movsxd r12,ecx
    lea eax,[rdx + 0x1]
    movsxd rbp,eax
    movsxd rax,edx
    movzx ecx,word ptr [rsi + rax*0x2]
    movzx eax,word ptr [rsi + r12*0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + rbp*0x2]
    add r10d,eax
    add r10d,ecx
    cmp r11d,0x2
    jle .lbl1800073cf
    lea ecx,[rdx + r14*0x1]
    lea eax,[rcx + 0x1]
    movsxd rdx,eax
    movsxd rax,ecx
    movzx ecx,word ptr [rsi + rax*0x2]
    movzx eax,word ptr [rsi + rdx*0x2 + 0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + r12*0x2 + 0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + rbp*0x2 + 0x2]
    mov rbp,qword ptr [rsp + 0x80]
    add ecx,eax
    movzx eax,word ptr [rsi + rdx*0x2]
    add r10d,eax
    add r10d,ecx
    jmp .lbl1800073d7
    .lbl1800073cf: mov rbp,qword ptr [rsp + 0x80]
    .lbl1800073d7: mov eax,r10d
    cdq
    idiv r15d
    add eax,dword ptr [rsp + 0x90]
    cmp eax,dword ptr [rsp + 0xb0]
    jge .lbl1800073fa
    movzx ecx,word ptr [rbp + rdi*0x2]
    cmp eax,ecx
    jge .lbl1800073fa
    add ebx,0x1
    .lbl1800073fa: mov ecx,dword ptr [rsp + 0x98]
    mov r10d,dword ptr [rsp + 0x20]
    add r8d,r11d
    sub r9,0x1
    jnz .lbl180007340
    mov rcx,qword ptr [rsp + 0x28]
    mov r12d,dword ptr [rsp + 0xa0]
    mov r15d,dword ptr [rsp + 0x88]
    .lbl180007428: add r13d,r11d
    sub rcx,0x1
    mov qword ptr [rsp + 0x28],rcx
    jnz .lbl180007303
    mov r13,qword ptr [rsp + 0x38]
    .lbl18000743f: imul ebx,ebx,0x64
    mov eax,ebx
    cdq
    idiv dword ptr [rsp + 0x24]
    add rsp,0x40
    pop r15
    pop r14
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180006c80:
    mov r8d,ecx
    xor eax,eax
    mov edx,0x8000
    mov ecx,0xf
    nop
    .lbl180006c90: lea r9d,[rdx + rax*0x2]
    shl r9d,cl
    add ecx,-0x1
    cmp r8d,r9d
    jc .lbl180006ca4
    add eax,edx
    sub r8d,r9d
    .lbl180006ca4: shr edx,1
    jnz .lbl180006c90
    ret

section .data


section .text
EXTR_FUN_180007b10:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    sub rsp,0x60
    mov r14,qword ptr [rsp + 0xc8]
    movsxd rbx,dword ptr [rsp + 0xc0]
    mov r12d,r8d
    mov r8d,edx
    mov rsi,r9
    mov edi,edx
    mov rbp,rcx
    imul r8d,r12d
    movsxd r13,r8d
    add r13,r14
    test ebx,ebx
    mov qword ptr [rsp + 0x48],r13
    jle .lbl180007d58
    xor edx,edx
    mov rcx,r14
    call EXTR_memset wrt ..plt
    xor ecx,ecx
    test ebx,ebx
    mov rdx,rbx
    mov qword ptr [rsp + 0x40],rbx
    jle .lbl180007b92
    nop
    nop
    .lbl180007b80: movzx eax,word ptr [rsi + rcx*0x2]
    add rcx,0x1
    cmp rcx,rbx
    mov byte ptr [rax + r14*0x1],0x1
    jl .lbl180007b80
    .lbl180007b92: test ebx,ebx
    jle .lbl180007d2f
    mov rax,rsi
    lea r9d,[rdi + -0x1]
    mov qword ptr [rsp + 0x58],r15
    sub rax,r13
    lea ecx,[r12 + -0x1]
    mov qword ptr [rsp + 0x28],r13
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x30],rbx
    mov rsi,r13
    mov qword ptr [rsp + 0x38],rax
    nop
    nop
    nop
    .lbl180007bd0: movzx eax,word ptr [rax + rsi*0x1]
    cdq
    idiv r12d
    test edx,edx
    mov r14d,edx
    mov dword ptr [rsp + 0xa8],edx
    mov r13d,eax
    mov dword ptr [rsp + 0xc0],eax
    jnz .lbl180007bf5
    xor r8d,r8d
    jmp .lbl180007bf9
    .lbl180007bf5: lea r8d,[rdx + -0x1]
    .lbl180007bf9: cmp edx,ecx
    jz .lbl180007c00
    add edx,0x1
    .lbl180007c00: test eax,eax
    jnz .lbl180007c08
    xor ecx,ecx
    jmp .lbl180007c0b
    .lbl180007c08: lea ecx,[rax + -0x1]
    .lbl180007c0b: cmp eax,r9d
    jz .lbl180007c13
    add eax,0x1
    .lbl180007c13: movsxd rdi,r8d
    movsxd r15,edx
    xor r10d,r10d
    xor r9d,r9d
    cmp rdi,r15
    movsxd rbp,ecx
    movsxd r12,eax
    jg .lbl180007ccf
    mov rsi,qword ptr [rsp + 0xa0]
    movsxd r14,dword ptr [rsp + 0xb0]
    mov r13,qword ptr [rsp + 0xc8]
    .lbl180007c46: cmp rbp,r12
    jg .lbl180007c8c
    mov rcx,r14
    lea rbx,[r14 + r14*0x1]
    imul rcx,rbp
    add rcx,rdi
    lea rdx,[rcx + r13*0x1]
    lea r8,[rsi + rcx*0x2]
    mov rcx,r12
    sub rcx,rbp
    add rcx,0x1
    nop
    nop
    .lbl180007c70: cmp byte ptr [rdx],0x0
    jnz .lbl180007c80
    movzx eax,word ptr [r8]
    add r10,rax
    add r9d,0x1
    .lbl180007c80: add r8,rbx
    add rdx,r14
    sub rcx,0x1
    jnz .lbl180007c70
    .lbl180007c8c: add rdi,0x1
    cmp rdi,r15
    jle .lbl180007c46
    test r9d,r9d
    mov r13d,dword ptr [rsp + 0xc0]
    mov r14d,dword ptr [rsp + 0xa8]
    mov rsi,qword ptr [rsp + 0x28]
    jz .lbl180007ccf
    mov rbp,qword ptr [rsp + 0xa0]
    mov r12d,dword ptr [rsp + 0xb0]
    mov rax,r10
    cqo
    movsxd rcx,r9d
    idiv rcx
    mov word ptr [rsi],ax
    jmp .lbl180007cf4
    .lbl180007ccf: movsxd r12,dword ptr [rsp + 0xb0]
    mov rbp,qword ptr [rsp + 0xa0]
    movsxd rax,r14d
    movsxd rcx,r13d
    imul rcx,r12
    add rcx,rax
    movzx eax,word ptr [rbp + rcx*0x2]
    mov word ptr [rsi],ax
    .lbl180007cf4: mov rax,qword ptr [rsp + 0x38]
    mov r9d,dword ptr [rsp + 0x20]
    add rsi,0x2
    sub qword ptr [rsp + 0x30],0x1
    mov qword ptr [rsp + 0x28],rsi
    lea ecx,[r12 + -0x1]
    jnz .lbl180007bd0
    mov rsi,qword ptr [rsp + 0xb8]
    mov rdx,qword ptr [rsp + 0x40]
    mov r13,qword ptr [rsp + 0x48]
    mov r15,qword ptr [rsp + 0x58]
    .lbl180007d2f: test rdx,rdx
    jle .lbl180007d58
    sub r13,rsi
    nop
    nop
    nop
    .lbl180007d40: movzx ecx,word ptr [rsi]
    movzx eax,word ptr [r13 + rsi*0x1]
    add rsi,0x2
    sub rdx,0x1
    mov word ptr [rbp + rcx*0x2],ax
    jnz .lbl180007d40
    .lbl180007d58: xor eax,eax
    add rsp,0x60
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_1800155d0:
    push rbp
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov rbp,rdx
    mov r12,r8
    mov rdx,rcx
    movzx r15d,word ptr [rbp + 0x86]
    movzx r14d,word ptr [rbp + 0x88]
    movzx r10d,word ptr [rbp + 0xa6]
    mov eax,r15d
    imul eax,r14d
    movsxd r9,eax
    movzx eax,word ptr [rbp + 0x8a]
    lea r8,[r8 + r9*0x2]
    add eax,-0x9
    cmp eax,0x1
    mov qword ptr [rsp + 0x88],r8
    ja .lbl180015666
    test r9,r9
    jle .lbl180015659
    mov rcx,r12
    sub rdx,r12
    mov r8,r9
    nop
    nop
    nop
    nop
    .lbl180015640: movzx eax,word ptr [rdx + rcx*0x1]
    add rcx,0x2
    shr ax,1
    sub r8,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl180015640
    lea r8,[r12 + r9*0x2]
    .lbl180015659: movzx r13d,word ptr [rbp + 0xaa]
    shr r13d,1
    jmp .lbl18001569e
    .lbl180015666: test r9,r9
    jle .lbl180015696
    mov rcx,r12
    sub rdx,r12
    mov r8,r9
    nop
    nop
    nop
    .lbl180015680: movzx eax,word ptr [rcx + rdx*0x1]
    add rcx,0x2
    sub r8,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl180015680
    lea r8,[r12 + r9*0x2]
    .lbl180015696: movzx r13d,word ptr [rbp + 0xaa]
    .lbl18001569e: movzx ecx,r10w
    mov qword ptr [rsp + 0x90],rsi
    sub ecx,0x7
    jz .lbl180015740
    cmp ecx,0x1
    jz .lbl1800156ea
    xor esi,esi
    test r9,r9
    jle .lbl180015836
    nop
    nop
    nop
    nop
    .lbl1800156d0: mov rax,qword ptr [rbp + 0x100]
    add rsi,0x1
    cmp rsi,r9
    mov byte ptr [rsi + rax*0x1 + -0x1],0x1
    jl .lbl1800156d0
    jmp .lbl180015836
    .lbl1800156ea: movzx ecx,word ptr [rbp + 0xae]
    mov rax,qword ptr [rbp + 0x100]
    movzx r10d,word ptr [rbp + 0xac]
    movzx r11d,word ptr [rbp + 0xa8]
    movzx r9d,word ptr [rbp + 0x8a]
    mov qword ptr [rsp + 0x48],r8
    mov qword ptr [rsp + 0x40],rax
    mov dword ptr [rsp + 0x38],ecx
    mov dword ptr [rsp + 0x30],r10d
    mov rcx,r12
    mov r8d,r15d
    mov edx,r14d
    mov dword ptr [rsp + 0x28],r13d
    mov dword ptr [rsp + 0x20],r11d
    call EXTR_FUN_180014340
    jmp .lbl180015836
    .lbl180015740: mov qword ptr [rsp + 0x80],rbx
    mov qword ptr [rsp + 0x98],rdi
    mov eax,r14d
    mov ecx,r15d
    xor esi,esi
    mov ebx,r14d
    mov edi,r15d
    and ebx,0xfffffffc
    mov r9d,r15d
    sub eax,ebx
    and edi,0xfffffffc
    mov r8d,r14d
    mov dword ptr [rsp + 0x38],eax
    sub ecx,edi
    mov dword ptr [rsp + 0x30],esi
    mov dword ptr [rsp + 0x28],ecx
    mov rcx,r12
    mov rdx,r12
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_180003fb0
    movzx ecx,word ptr [rbp + 0xa8]
    movzx edx,word ptr [rbp + 0xae]
    mov rax,qword ptr [rsp + 0x88]
    movzx r9d,word ptr [rbp + 0xac]
    mov qword ptr [rsp + 0x48],rax
    mov rax,qword ptr [rbp + 0x100]
    mov r8d,edi
    mov qword ptr [rsp + 0x40],rax
    mov dword ptr [rsp + 0x38],ecx
    mov dword ptr [rsp + 0x30],edx
    mov edx,ebx
    mov rcx,r12
    mov dword ptr [rsp + 0x28],r13d
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_180014df0
    mov rdx,qword ptr [rbp + 0x100]
    mov r8d,ebx
    mov rcx,r12
    imul r8d,edi
    call EXTR_memcpy wrt ..plt
    mov rcx,qword ptr [rbp + 0x100]
    mov dword ptr [rsp + 0x48],0x1
    mov byte ptr [rsp + 0x40],sil
    sub r14d,ebx
    sub r15d,edi
    mov dword ptr [rsp + 0x38],r14d
    mov dword ptr [rsp + 0x30],esi
    mov r9d,edi
    mov r8d,ebx
    mov rdx,r12
    mov dword ptr [rsp + 0x28],r15d
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_180003d50
    mov rdi,qword ptr [rsp + 0x98]
    mov rbx,qword ptr [rsp + 0x80]
    .lbl180015836: mov rsi,qword ptr [rsp + 0x90]
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180014df0:
    mov r11,rsp
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x8a8
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x890],rax
    mov r14,qword ptr [rsp + 0x938]
    mov r15d,dword ptr [rsp + 0x918]
    mov r12,qword ptr [rsp + 0x930]
    mov word ptr [rsp + 0x850],0x0
    mov rbp,rcx
    xor ecx,ecx
    mov qword ptr [r11 + -0x96],rcx
    mov qword ptr [r11 + -0x8e],rcx
    mov qword ptr [r11 + -0x86],rcx
    mov qword ptr [r11 + -0x7e],rcx
    mov qword ptr [r11 + -0x76],rcx
    mov qword ptr [r11 + -0x6e],rcx
    mov qword ptr [r11 + -0x66],rcx
    mov dword ptr [r11 + -0x5e],ecx
    mov word ptr [r11 + -0x5a],cx
    lea rcx,[r11 + -0x98]
    mov r13d,r8d
    mov dword ptr [rsp + 0x44],r9d
    mov r9d,dword ptr [rsp + 0x910]
    mov qword ptr [rsp + 0x30],r14
    mov qword ptr [rsp + 0x28],rcx
    imul r13d,edx
    movsxd rbx,r13d
    xor edi,edi
    mov rcx,rbp
    mov dword ptr [rsp + 0x40],r8d
    mov dword ptr [rsp + 0x48],edx
    mov dword ptr [rsp + 0x20],r15d
    mov dword ptr [rsp + 0x4c],edi
    lea rsi,[r14 + rbx*0x4]
    call EXTR_FUN_180012420
    mov r8d,dword ptr [rsp + 0x40]
    mov edx,dword ptr [rsp + 0x48]
    lea r9,[rsp + 0x850]
    mov rcx,rbp
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_1800124f0
    lea r8d,[r13*0x4]
    mov rdx,r14
    mov rcx,rsi
    call EXTR_memcpy wrt ..plt
    mov eax,dword ptr [rsp + 0x44]
    mov r8d,dword ptr [rsp + 0x40]
    lea rdx,[rsi + rbx*0x4]
    mov ebx,dword ptr [rsp + 0x48]
    mov rcx,r14
    mov r9d,ebx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180005460
    mov edx,r13d
    mov rcx,rsi
    call EXTR_FUN_1800054c0
    mov r8d,r13d
    mov rdx,rsi
    mov rcx,r14
    call EXTR_FUN_1800054f0
    mov eax,dword ptr [rsp + 0x44]
    mov r8d,dword ptr [rsp + 0x40]
    lea r11d,[rax + rax*0x1 + -0x1]
    mov r9d,ebx
    mov rdx,rsi
    mov rcx,r14
    mov dword ptr [rsp + 0x20],r11d
    call EXTR_FUN_180005460
    mov rdx,rsi
    nop
    nop
    nop
    nop
    .lbl180014f50: mov ecx,edi
    mov eax,edi
    add rdx,0x1
    shr eax,0x6
    shr ecx,0x7
    and al,0x1
    and cl,0x1
    add cl,al
    mov eax,edi
    shr eax,0x5
    and al,0x1
    add cl,al
    mov eax,edi
    shr eax,0x4
    and al,0x1
    add cl,al
    mov eax,edi
    shr eax,0x3
    and al,0x1
    add cl,al
    mov eax,edi
    shr eax,0x2
    and al,0x1
    add cl,al
    mov eax,edi
    shr eax,1
    and al,0x1
    add cl,al
    movzx eax,dil
    add edi,0x1
    and al,0x1
    add cl,al
    cmp edi,0x100
    mov byte ptr [rdx + -0x1],cl
    jl .lbl180014f50
    test r13d,r13d
    mov rdx,r14
    jle .lbl180014fee
    mov r8,r12
    mov r9d,r13d
    nop
    nop
    nop
    .lbl180014fc0: movzx eax,byte ptr [rdx]
    add r8,0x1
    add rdx,0x4
    movzx ecx,byte ptr [rax + rsi*0x1]
    movzx eax,byte ptr [rdx + -0x3]
    add cl,byte ptr [rax + rsi*0x1]
    movzx eax,byte ptr [rdx + -0x2]
    add cl,byte ptr [rax + rsi*0x1]
    movzx eax,byte ptr [rdx + -0x1]
    add cl,byte ptr [rax + rsi*0x1]
    sub r9,0x1
    mov byte ptr [r8 + -0x1],cl
    jnz .lbl180014fc0
    .lbl180014fee: mov ebx,dword ptr [rsp + 0x928]
    cmp ebx,0x2
    jg .lbl180015003
    movzx r8d,bx
    jmp .lbl180015080
    .lbl180015003: lea rcx,[rsp + 0x50]
    xor edx,edx
    mov r8d,0x800
    call EXTR_memset wrt ..plt
    test r13d,r13d
    jle .lbl18001505c
    mov r9d,dword ptr [rsp + 0x910]
    mov rcx,r12
    mov r8d,r13d
    nop
    nop
    .lbl180015030: movsx eax,word ptr [rbp]
    movzx edx,byte ptr [rcx]
    cmp eax,r9d
    jl .lbl18001504e
    cmp eax,r15d
    jg .lbl18001504e
    cmp edx,0x200
    jge .lbl18001504e
    add dword ptr [rsp + rdx*0x4 + 0x50],0x1
    .lbl18001504e: add rcx,0x1
    add rbp,0x2
    sub r8,0x1
    jnz .lbl180015030
    .lbl18001505c: lea r8,[rsp + 0x4c]
    lea rcx,[rsp + 0x50]
    mov edx,0x200
    call EXTR_FUN_1800125c0
    mov eax,dword ptr [rsp + 0x4c]
    mov r8w,bx
    cmp ax,bx
    cmovc r8w,ax
    .lbl180015080: test r13d,r13d
    jle .lbl1800150a7
    mov rcx,r12
    mov edx,r13d
    nop
    nop
    .lbl180015090: movzx eax,byte ptr [rcx]
    cmp ax,r8w
    setnc al
    add rcx,0x1
    sub rdx,0x1
    mov byte ptr [rcx + -0x1],al
    jnz .lbl180015090
    .lbl1800150a7: mov esi,dword ptr [rsp + 0x40]
    mov edi,dword ptr [rsp + 0x48]
    mov r9,r14
    mov r8d,esi
    mov edx,edi
    mov rcx,r12
    call EXTR_FUN_180014cf0
    mov r9,r14
    mov r8d,edi
    mov edx,esi
    mov rcx,r12
    call EXTR_FUN_180015da0
    cmp ebx,0x2
    jg .lbl1800150ed
    mov eax,dword ptr [rsp + 0x44]
    mov r9d,edi
    mov r8d,esi
    mov rdx,r14
    mov rcx,r12
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180004aa0
    .lbl1800150ed: mov edx,r13d
    mov rcx,r12
    call EXTR_FUN_180004e20
    mov r9d,dword ptr [rsp + 0x920]
    mov r8d,esi
    mov edx,edi
    mov rcx,r12
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_180012f50
    mov edx,r13d
    mov rcx,r12
    call EXTR_FUN_180004e20
    xor eax,eax
    mov rcx,qword ptr [rsp + 0x890]
    xor rcx,rsp
    call EXTR_check_cookie
    add rsp,0x8a8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data

glb180161a18: db 50, -94, -33, 45, -103, 43, 0, 0

section .text
EXTR_FUN_180012f50:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov rdi,qword ptr [rsp + 0x80]
    mov eax,edx
    mov r12d,r9d
    mov r13d,r8d
    mov r14d,edx
    mov rbp,rcx
    imul eax,r8d
    test eax,eax
    movsxd rsi,eax
    lea rbx,[rdi + rsi*0x2]
    jle .lbl180012fb6
    mov r8,rdi
    mov rax,rbx
    mov rcx,rsi
    sub r8,rbx
    nop
    .lbl180012fa0: mov word ptr [r8 + rax*0x1],0x0
    mov word ptr [rax],0x0
    add rax,0x2
    sub rcx,0x1
    jnz .lbl180012fa0
    .lbl180012fb6: mov r9d,0x1
    mov r8d,r13d
    mov rcx,rbp
    call EXTR_FUN_180010450
    mov r9,rdi
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rbp
    mov byte ptr [rsp + 0x28],0x30
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180012d30
    mov r9d,0x1
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rdi
    movsxd r15,eax
    call EXTR_FUN_180010770
    mov r9d,0x1
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rbp
    call EXTR_FUN_180010550
    xor r11d,r11d
    test rsi,rsi
    mov rcx,r11
    jle .lbl180013033
    .lbl180013018: cmp word ptr [rdi + rcx*0x2],r11w
    jle .lbl18001302a
    movsx rax,word ptr [rdi + rcx*0x2]
    add word ptr [rbx + rax*0x2 + -0x2],0x1
    .lbl18001302a: add rcx,0x1
    cmp rcx,rsi
    jl .lbl180013018
    .lbl180013033: test r15d,r15d
    mov rdx,r11
    jle .lbl18001305b
    nop
    nop
    .lbl180013040: movsx eax,word ptr [rbx + rdx*0x2]
    mov ecx,r11d
    cmp eax,r12d
    setge cl
    add rdx,0x1
    cmp rdx,r15
    mov word ptr [rbx + rdx*0x2 + -0x2],cx
    jl .lbl180013040
    .lbl18001305b: test rsi,rsi
    jle .lbl180013084
    .lbl180013060: movzx eax,word ptr [rdi + r11*0x2]
    test ax,ax
    jle .lbl18001307b
    movsx rax,ax
    cmp word ptr [rbx + rax*0x2 + -0x2],0x0
    jnz .lbl18001307b
    mov byte ptr [r11 + rbp*0x1],0x0
    .lbl18001307b: add r11,0x1
    cmp r11,rsi
    jl .lbl180013060
    .lbl180013084: mov rbx,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x68]
    mov rsi,qword ptr [rsp + 0x70]
    add rsp,0x30
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180012d30:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x10],edx
    push rbx
    push rdi
    push r12
    push r14
    push r15
    sub rsp,0x70
    xor ebx,ebx
    movsxd r14,r8d
    mov edi,edx
    lea r9d,[rbx + 0x1]
    mov r8d,r14d
    mov r15,rcx
    mov dword ptr [rsp + 0xb0],ebx
    call EXTR_FUN_180010450
    mov rcx,qword ptr [rsp + 0xc0]
    mov r11d,edi
    imul r11d,r14d
    test r11d,r11d
    jle .lbl180012dab
    mov r8,qword ptr [rsp + 0xb8]
    mov rax,rcx
    mov rdx,r8
    sub rdx,rcx
    mov ecx,r11d
    nop
    nop
    .lbl180012d90: mov word ptr [rdx + rax*0x1],bx
    mov word ptr [rax],bx
    add rax,0x2
    sub rcx,0x1
    jnz .lbl180012d90
    mov rcx,qword ptr [rsp + 0xc0]
    jmp .lbl180012db3
    .lbl180012dab: mov r8,qword ptr [rsp + 0xb8]
    .lbl180012db3: mov edx,0x1
    lea eax,[rdi + -0x1]
    cmp eax,edx
    mov r12d,edx
    mov qword ptr [rsp + 0x58],rdx
    mov dword ptr [rsp + 0x50],eax
    jle .lbl180012f22
    mov qword ptr [rsp + 0xa0],rbp
    mov qword ptr [rsp + 0x68],rsi
    mov qword ptr [rsp + 0x60],r13
    lea r13d,[r14 + -0x1]
    .lbl180012de5: mov esi,0x1
    xor bx,bx
    cmp r13d,esi
    jle .lbl180012edc
    mov rax,r14
    imul rax,rdx
    lea rbp,[rax + r15*0x1 + 0x1]
    lea rdi,[r8 + rax*0x2 + 0x2]
    .lbl180012e07: cmp byte ptr [rbp],0x0
    jz .lbl180012e6f
    test bx,bx
    jz .lbl180012e1a
    mov word ptr [rdi],bx
    jmp .lbl180012eaf
    .lbl180012e1a: movzx ebx,word ptr [rdi]
    test bx,bx
    jnz .lbl180012eaf
    mov eax,dword ptr [rsp + 0xb0]
    mov r9,rcx
    mov edx,r14d
    add eax,0x1
    mov rcx,r15
    mov bx,ax
    mov dword ptr [rsp + 0xb0],eax
    movzx eax,byte ptr [rsp + 0xc8]
    mov byte ptr [rsp + 0x40],al
    mov dword ptr [rsp + 0x38],0x0
    mov word ptr [rsp + 0x30],bx
    mov dword ptr [rsp + 0x28],esi
    mov dword ptr [rsp + 0x20],r12d
    call EXTR_FUN_180010220
    mov word ptr [rdi],bx
    jmp .lbl180012eaf
    .lbl180012e6f: test bx,bx
    jz .lbl180012eaf
    cmp word ptr [rdi],0x0
    jnz .lbl180012eac
    mov dl,byte ptr [rsp + 0xc8]
    lea eax,[rsi + -0x1]
    mov r9,rcx
    mov byte ptr [rsp + 0x40],dl
    mov dword ptr [rsp + 0x38],0x1
    mov word ptr [rsp + 0x30],bx
    mov dword ptr [rsp + 0x28],eax
    mov edx,r14d
    mov rcx,r15
    mov dword ptr [rsp + 0x20],r12d
    call EXTR_FUN_180010220
    .lbl180012eac: xor bx,bx
    .lbl180012eaf: mov r8,qword ptr [rsp + 0xb8]
    mov rcx,qword ptr [rsp + 0xc0]
    add esi,0x1
    add rdi,0x2
    add rbp,0x1
    cmp esi,r13d
    jl .lbl180012e07
    mov rdx,qword ptr [rsp + 0x58]
    mov eax,dword ptr [rsp + 0x50]
    .lbl180012edc: mov r8,qword ptr [rsp + 0xb8]
    mov rcx,qword ptr [rsp + 0xc0]
    add rdx,0x1
    add r12d,0x1
    mov qword ptr [rsp + 0x58],rdx
    cmp r12d,eax
    jl .lbl180012de5
    mov ebx,dword ptr [rsp + 0xb0]
    mov edi,dword ptr [rsp + 0xa8]
    mov r13,qword ptr [rsp + 0x60]
    mov rsi,qword ptr [rsp + 0x68]
    mov rbp,qword ptr [rsp + 0xa0]
    .lbl180012f22: mov r9d,0x1
    mov r8d,r14d
    mov edx,edi
    mov rcx,r15
    call EXTR_FUN_180010550
    mov eax,ebx
    add rsp,0x70
    pop r15
    pop r14
    pop r12
    pop rdi
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180010220:
    mov r11,rsp
    mov qword ptr [r11 + 0x18],r8
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [r11 + 0x8],rcx
    push rbx
    push rbp
    push rdi
    push r12
    push r15
    sub rsp,0x40
    mov ebp,dword ptr [r11 + 0x30]
    mov r12d,dword ptr [r11 + 0x28]
    mov rax,rcx
    lea rcx,[r11 + 0x40]
    mov r15,r9
    lea r9,[r11 + 0x28]
    mov qword ptr [r11 + -0x40],rcx
    lea rcx,[r11 + 0x30]
    xor bl,bl
    mov qword ptr [r11 + -0x48],rcx
    mov rcx,rax
    call EXTR_FUN_180010110
    cmp byte ptr [rsp + 0xb0],0x31
    movzx edi,word ptr [rsp + 0xa0]
    movsxd r8,dword ptr [rsp + 0x98]
    movsxd r9,dword ptr [rsp + 0x90]
    jnz .lbl18001029e
    movsxd r10,dword ptr [rsp + 0x78]
    mov rcx,r10
    imul rcx,r9
    add rcx,r8
    mov word ptr [r15 + rcx*0x2],di
    jmp .lbl1800102a3
    .lbl18001029e: mov r10d,dword ptr [rsp + 0x78]
    .lbl1800102a3: cmp r8d,ebp
    jnz .lbl1800102b1
    cmp r9d,r12d
    jz .lbl1800103ab
    .lbl1800102b1: mov qword ptr [rsp + 0x88],rsi
    mov qword ptr [rsp + 0x38],r13
    mov qword ptr [rsp + 0x30],r14
    mov r14d,r9d
    mov r13d,r8d
    movsxd rsi,r10d
    nop
    .lbl1800102d0: mov eax,dword ptr [rsp + 0xa8]
    movsxd rcx,r9d
    lea r9,[rsp + 0x90]
    add eax,0x6
    imul rcx,rsi
    cdq
    and edx,0x7
    add eax,edx
    and eax,0x7
    sub eax,edx
    mov edx,r10d
    mov dword ptr [rsp + 0xa8],eax
    movsxd rax,r8d
    add rcx,rax
    mov rax,qword ptr [rsp + 0x80]
    mov word ptr [rax + rcx*0x2],di
    lea rcx,[rsp + 0xa8]
    mov r8,rax
    mov qword ptr [rsp + 0x28],rcx
    lea rcx,[rsp + 0x98]
    mov qword ptr [rsp + 0x20],rcx
    mov rcx,qword ptr [rsp + 0x70]
    call EXTR_FUN_180010110
    cmp byte ptr [rsp + 0xb0],0x31
    movsxd r8,dword ptr [rsp + 0x98]
    movsxd r9,dword ptr [rsp + 0x90]
    jnz .lbl180010360
    mov rcx,r9
    imul rcx,rsi
    add rcx,r8
    mov word ptr [r15 + rcx*0x2],di
    .lbl180010360: cmp r8d,ebp
    jnz .lbl180010376
    cmp r9d,r12d
    jnz .lbl180010376
    mov r10d,dword ptr [rsp + 0x78]
    mov bl,0x1
    jmp .lbl1800102d0
    .lbl180010376: test bl,bl
    mov r10d,dword ptr [rsp + 0x78]
    jz .lbl1800102d0
    cmp r8d,r13d
    jnz .lbl18001038d
    cmp r9d,r14d
    jz .lbl180010399
    .lbl18001038d: mov r10d,dword ptr [rsp + 0x78]
    xor bl,bl
    jmp .lbl1800102d0
    .lbl180010399: mov r13,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x88]
    mov r14,qword ptr [rsp + 0x30]
    .lbl1800103ab: add rsp,0x40
    pop r15
    pop r12
    pop rdi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180010110:
    mov rax,rsp
    mov qword ptr [rax + 0x8],rbx
    mov qword ptr [rax + 0x10],rbp
    mov qword ptr [rax + 0x18],rsi
    mov qword ptr [rax + 0x20],rdi
    push r12
    sub rsp,0x40
    mov rdi,qword ptr [rsp + 0x70]
    xor r10d,r10d
    mov rsi,r8
    mov r8,qword ptr [rsp + 0x78]
    mov dword ptr [rsp],r10d
    mov rbx,r9
    mov dword ptr [rax + -0x44],0x1
    mov dword ptr [rax + -0x40],0x1
    mov dword ptr [rax + -0x3c],0x1
    mov dword ptr [rax + -0x38],0x1
    mov dword ptr [rax + -0x34],r10d
    mov dword ptr [rax + -0x30],0x1
    mov dword ptr [rax + -0x2c],0xffffffff
    mov dword ptr [rax + -0x28],r10d
    mov dword ptr [rax + -0x24],0xffffffff
    mov dword ptr [rax + -0x20],0xffffffff
    mov dword ptr [rax + -0x1c],0xffffffff
    mov dword ptr [rax + -0x18],0xffffffff
    mov dword ptr [rax + -0x14],r10d
    mov dword ptr [rax + -0x10],0xffffffff
    mov dword ptr [rax + -0xc],0x1
    movsxd r12,edx
    nop
    nop
    nop
    nop
    .lbl1800101b0: movsxd rax,dword ptr [r8]
    mov edx,dword ptr [rsp + rax*0x8]
    mov r9d,dword ptr [rsp + rax*0x8 + 0x4]
    add edx,dword ptr [rbx]
    add r9d,dword ptr [rdi]
    movsxd r11,edx
    movsxd rax,r9d
    imul r11,r12
    add r11,rax
    cmp byte ptr [r11 + rcx*0x1],0x0
    jnz .lbl1800101fb
    mov word ptr [rsi + r11*0x2],0xffff
    mov eax,dword ptr [r8]
    add r10d,0x1
    add eax,0x1
    cdq
    and edx,0x7
    add eax,edx
    and eax,0x7
    sub eax,edx
    cmp r10d,0x7
    mov dword ptr [r8],eax
    jl .lbl1800101b0
    jmp .lbl180010200
    .lbl1800101fb: mov dword ptr [rbx],edx
    mov dword ptr [rdi],r9d
    .lbl180010200: mov rbx,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    mov rdi,qword ptr [rsp + 0x68]
    add rsp,0x40
    pop r12
    ret

section .data


section .text
EXTR_FUN_180010550:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x10
    movsxd r15,edx
    movsxd r14,r8d
    mov ebp,r9d
    mov eax,r14d
    mov rbx,rcx
    cdq
    sub eax,edx
    sar eax,1
    cmp r9d,eax
    cmovg ebp,eax
    mov eax,r15d
    cdq
    sub eax,edx
    sar eax,1
    cmp ebp,eax
    cmovg ebp,eax
    movsxd rsi,ebp
    lea eax,[rbp + -0x1]
    test eax,eax
    movsxd rdi,eax
    js .lbl18001068b
    mov r13d,r14d
    mov r12d,r15d
    mov r11,rsi
    sub r13d,ebp
    sub r12d,ebp
    sub r11,rdi
    .lbl1800105b6: cmp ebp,r13d
    jge .lbl180010624
    mov rcx,r15
    mov rdx,r15
    mov r8,r14
    sub rdx,rdi
    sub rcx,rsi
    lea r9,[r14 + 0x1]
    imul r8,rdi
    sub rdx,0x1
    sub rcx,0x1
    add r8,rsi
    imul r9,rsi
    mov r10d,r13d
    add r8,rbx
    add r9,rbx
    imul rdx,r14
    imul rcx,r14
    add rdx,rsi
    add rcx,rsi
    add rdx,rbx
    add rcx,rbx
    sub r10d,ebp
    .lbl180010601: movzx eax,byte ptr [r9]
    add rdx,0x1
    add r9,0x1
    mov byte ptr [r8],al
    movzx eax,byte ptr [rcx]
    add r8,0x1
    mov byte ptr [rdx + -0x1],al
    add rcx,0x1
    sub r10,0x1
    jnz .lbl180010601
    .lbl180010624: cmp ebp,r12d
    jge .lbl18001067d
    mov rcx,r14
    lea rax,[rsi + 0x1]
    imul rcx,rsi
    imul rax,r14
    lea rdx,[rcx + rdi*0x1]
    sub rcx,rsi
    add rcx,r14
    sub rax,rdi
    add rdx,rbx
    lea r10,[rcx + rbx*0x1 + -0x1]
    mov ecx,r12d
    lea r9,[rax + rbx*0x1 + -0x1]
    sub ecx,ebp
    nop
    nop
    nop
    .lbl180010660: movzx eax,byte ptr [rdx + r11*0x1]
    mov byte ptr [rdx],al
    movzx eax,byte ptr [r10]
    add rdx,r14
    mov byte ptr [r9],al
    add r9,r14
    add r10,r14
    sub rcx,0x1
    jnz .lbl180010660
    .lbl18001067d: add r11,0x1
    sub rdi,0x1
    jns .lbl1800105b6
    .lbl18001068b: test ebp,ebp
    jle .lbl180010753
    mov rdx,r15
    mov rcx,r15
    mov rax,r14
    sub rcx,rsi
    lea r9,[rdx + -0x1]
    lea r10,[r14 + 0x1]
    imul rax,rsi
    lea rdi,[rcx + -0x1]
    sub rax,rsi
    mov r15,r14
    imul r10,rsi
    imul r9,r14
    add rax,r14
    add r10,rbx
    neg r15
    imul rdi,r14
    imul rcx,r14
    add rdi,rsi
    sub rcx,rsi
    add r9,rbx
    add rdi,rbx
    lea r11,[rax + rbx*0x1 + -0x1]
    lea r12,[r14 + rbx*0x1 + -0x1]
    imul rdx,r14
    lea rbp,[rcx + rbx*0x1 + -0x1]
    mov qword ptr [rsp],rsi
    lea r13,[rdx + rbx*0x1 + -0x1]
    nop
    nop
    nop
    .lbl180010700: xor ecx,ecx
    mov rdx,r13
    mov r8,r12
    nop
    nop
    .lbl180010710: movzx eax,byte ptr [r10]
    add rcx,0x1
    sub rdx,0x1
    mov byte ptr [rbx + rcx*0x1 + -0x1],al
    movzx eax,byte ptr [r11]
    sub r8,0x1
    cmp rcx,rsi
    mov byte ptr [r8 + 0x1],al
    movzx eax,byte ptr [rdi]
    mov byte ptr [r9 + rcx*0x1 + -0x1],al
    movzx eax,byte ptr [rbp]
    mov byte ptr [rdx + 0x1],al
    jl .lbl180010710
    add r12,r14
    add rbx,r14
    add r9,r15
    add r13,r15
    sub qword ptr [rsp],0x1
    jnz .lbl180010700
    .lbl180010753: mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x10
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180010450:
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x10
    movsxd r10,edx
    mov ebp,r9d
    movsxd r12,r8d
    mov eax,r12d
    mov r11,rcx
    mov rdi,r10
    cdq
    sub eax,edx
    sar eax,1
    cmp r9d,eax
    cmovg ebp,eax
    mov eax,r10d
    cdq
    sub eax,edx
    sar eax,1
    cmp ebp,eax
    cmovg ebp,eax
    xor r9d,r9d
    test ebp,ebp
    movsxd rsi,ebp
    jle .lbl18001053a
    mov qword ptr [rsp + 0x20],rbx
    mov ebx,r12d
    sub ebx,ebp
    add r10,-0x1
    nop
    nop
    .lbl1800104b0: test rdi,rdi
    jle .lbl1800104e2
    mov rax,r12
    lea rcx,[r9 + r11*0x1]
    sub rax,r9
    lea rdx,[rax + r11*0x1 + -0x1]
    mov rax,rdi
    nop
    nop
    nop
    .lbl1800104d0: mov byte ptr [rcx],0x0
    mov byte ptr [rdx],0x0
    add rdx,r12
    add rcx,r12
    sub rax,0x1
    jnz .lbl1800104d0
    .lbl1800104e2: cmp ebp,ebx
    jge .lbl180010524
    mov rcx,r12
    mov rdx,r10
    mov r8d,ebx
    imul rdx,r12
    imul rcx,r9
    add rdx,rsi
    add rcx,rsi
    add rdx,r11
    add rcx,r11
    sub r8d,ebp
    nop
    nop
    nop
    .lbl180010510: mov byte ptr [rcx],0x0
    mov byte ptr [rdx],0x0
    add rdx,0x1
    add rcx,0x1
    sub r8,0x1
    jnz .lbl180010510
    .lbl180010524: add r9,0x1
    sub r10,0x1
    cmp r9,rsi
    jl .lbl1800104b0
    mov rbx,qword ptr [rsp + 0x20]
    .lbl18001053a: mov rbp,qword ptr [rsp + 0x28]
    mov rsi,qword ptr [rsp + 0x30]
    mov rdi,qword ptr [rsp + 0x38]
    add rsp,0x10
    pop r12
    ret

section .data


section .text
EXTR_FUN_180010770:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x18],rbp
    mov qword ptr [rsp + 0x20],rsi
    mov dword ptr [rsp + 0x10],edx
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x20
    mov edi,r9d
    mov r9d,edx
    movsxd r15,r8d
    mov eax,r15d
    mov rsi,rcx
    cdq
    sub eax,edx
    sar eax,1
    cmp edi,eax
    cmovg edi,eax
    mov eax,r9d
    cdq
    sub eax,edx
    sar eax,1
    cmp edi,eax
    cmovg edi,eax
    movsxd r14,edi
    lea eax,[rdi + -0x1]
    mov qword ptr [rsp + 0x8],r14
    test eax,eax
    movsxd rbx,eax
    js .lbl1800108d4
    mov r13d,r15d
    mov r12d,r9d
    lea rbp,[rbx + rbx*0x1]
    sub r13d,edi
    sub r12d,edi
    nop
    .lbl1800107e0: cmp edi,r13d
    jge .lbl180010855
    movsxd rcx,r9d
    mov r10d,r13d
    mov rax,rcx
    sub rcx,r14
    sub rax,rbx
    sub rcx,0x1
    sub rax,0x1
    imul rcx,r15
    imul rax,r15
    add rcx,r14
    lea r9,[rsi + rcx*0x2]
    add rax,r14
    lea r8,[rsi + rax*0x2]
    mov rax,r15
    imul rax,rbx
    add rax,r14
    lea rcx,[rsi + rax*0x2]
    lea rax,[r15 + 0x1]
    imul rax,r14
    sub r10d,edi
    lea rdx,[rsi + rax*0x2]
    nop
    .lbl180010830: movzx eax,word ptr [rdx]
    add r8,0x2
    add rdx,0x2
    mov word ptr [rcx],ax
    movzx eax,word ptr [r9]
    add rcx,0x2
    mov word ptr [r8 + -0x2],ax
    add r9,0x2
    sub r10,0x1
    jnz .lbl180010830
    .lbl180010855: cmp edi,r12d
    jge .lbl1800108c1
    lea rcx,[r14 + r14*0x1]
    mov rax,r15
    lea r9,[r15 + r15*0x1]
    lea rdx,[rcx + 0x2]
    lea r10,[r9 + 0x2]
    imul rax,r14
    add rax,rbx
    imul rdx,r15
    imul r10,r14
    add r10,rsi
    lea r11,[rsi + rax*0x2]
    mov rax,rdx
    sub rdx,rcx
    lea rcx,[rdx + rsi*0x1 + -0x2]
    sub rax,rbp
    mov edx,r12d
    sub edx,edi
    lea r8,[rax + rsi*0x1 + -0x2]
    nop
    nop
    .lbl1800108a0: movzx eax,word ptr [r10]
    add r10,r9
    mov word ptr [r11],ax
    movzx eax,word ptr [rcx]
    add r11,r9
    mov word ptr [r8],ax
    add r8,r9
    add rcx,r9
    sub rdx,0x1
    jnz .lbl1800108a0
    .lbl1800108c1: mov r9d,dword ptr [rsp + 0x58]
    sub rbp,0x2
    sub rbx,0x1
    jns .lbl1800107e0
    .lbl1800108d4: test edi,edi
    jle .lbl1800109c6
    mov r8,r15
    lea rax,[r15 + 0x1]
    movsxd rdx,r9d
    mov rcx,rdx
    lea r11,[rdx + rdx*0x1 + -0x2]
    lea rbx,[rsi + r8*0x2 + -0x2]
    imul rax,r14
    lea rbp,[rsi + rax*0x2]
    sub rcx,r14
    lea rax,[r14 + 0x1]
    imul r11,r8
    imul rax,r15
    sub rax,r14
    lea r12,[rsi + rax*0x2 + -0x2]
    lea rax,[rcx + -0x1]
    imul rcx,r15
    sub rcx,r14
    imul rax,r15
    add rax,r14
    lea r14,[rsi + rcx*0x2 + -0x2]
    lea r13,[rsi + rax*0x2]
    lea rax,[r15 + r15*0x1]
    neg r15
    mov qword ptr [rsp + 0x10],rax
    mov rax,rdx
    add r15,r15
    imul rax,r8
    add r11,rsi
    lea rdi,[rsi + rax*0x2 + -0x2]
    mov rax,qword ptr [rsp + 0x8]
    mov qword ptr [rsp],rax
    nop
    nop
    nop
    .lbl180010960: mov rcx,r11
    mov rdx,rsi
    mov r8,rdi
    mov r9,rbx
    mov r10,rax
    nop
    .lbl180010970: movzx eax,word ptr [rbp]
    sub r8,0x2
    add rdx,0x2
    mov word ptr [rdx + -0x2],ax
    movzx eax,word ptr [r12]
    add rcx,0x2
    mov word ptr [r9],ax
    movzx eax,word ptr [r13]
    sub r9,0x2
    sub r10,0x1
    mov word ptr [rcx + -0x2],ax
    movzx eax,word ptr [r14]
    mov word ptr [r8 + 0x2],ax
    jnz .lbl180010970
    mov rax,qword ptr [rsp + 0x10]
    add r11,r15
    add rdi,r15
    add rsi,rax
    add rbx,rax
    sub qword ptr [rsp],0x1
    mov rax,qword ptr [rsp + 0x8]
    jnz .lbl180010960
    .lbl1800109c6: mov rbx,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x60]
    mov rsi,qword ptr [rsp + 0x68]
    add rsp,0x20
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180012420:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x40
    mov rdi,qword ptr [rsp + 0x90]
    mov r13d,dword ptr [rsp + 0x80]
    mov ebp,r9d
    mov qword ptr [rsp + 0x30],rdi
    mov word ptr [rsp + 0x28],r13w
    mov word ptr [rsp + 0x20],r9w
    mov r9d,0x3e8
    mov ebx,r8d
    mov r12d,edx
    xor esi,esi
    call EXTR_FUN_1800036e0
    mov r9,qword ptr [rsp + 0x88]
    imul r12d,ebx
    mov r8d,r12d
    mov r11d,esi
    .lbl180012481: add esi,dword ptr [rdi]
    mov eax,0x3e0f83e1
    imul r8d
    sar edx,0x3
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    cmp esi,edx
    jle .lbl1800124c2
    mov ecx,r13d
    mov eax,0x10624dd3
    add r8d,r12d
    sub ecx,ebp
    imul ecx,r11d
    imul ecx
    sar edx,0x6
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    add dx,bp
    add r9,0x2
    mov word ptr [r9 + -0x2],dx
    .lbl1800124c2: add r11d,0x1
    add rdi,0x4
    cmp r11d,0x3e8
    jl .lbl180012481
    mov rbx,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x68]
    mov rsi,qword ptr [rsp + 0x70]
    xor eax,eax
    add rsp,0x40
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_1800036e0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    mov rbx,qword ptr [rsp + 0x60]
    mov esi,edx
    mov rbp,rcx
    xor edx,edx
    mov rcx,rbx
    mov edi,r9d
    imul esi,r8d
    lea r8d,[r9*0x4]
    call EXTR_memset wrt ..plt
    movsx r11d,word ptr [rsp + 0x50]
    movsx r9d,word ptr [rsp + 0x58]
    sub r9d,r11d
    add r9d,0x1
    jnz .lbl180003732
    lea eax,[r9 + 0x1]
    jmp .lbl18000376d
    .lbl180003732: xor r10d,r10d
    test esi,esi
    movsxd r8,esi
    mov rcx,r10
    jle .lbl18000376b
    nop
    .lbl180003740: movsx eax,word ptr [rbp + rcx*0x2]
    sub eax,r11d
    imul eax,edi
    cdq
    idiv r9d
    test eax,eax
    cmovs eax,r10d
    cmp eax,edi
    jl .lbl18000375c
    lea eax,[rdi + -0x1]
    .lbl18000375c: cdqe
    add rcx,0x1
    add dword ptr [rbx + rax*0x4],0x1
    cmp rcx,r8
    jl .lbl180003740
    .lbl18000376b: xor eax,eax
    .lbl18000376d: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_1800054c0:
    xor r8d,r8d
    test edx,edx
    movsxd r9,edx
    jle .lbl1800054e4
    nop
    nop
    .lbl1800054d0: mov eax,dword ptr [rcx + r8*0x4]
    add r8,0x1
    cmp r8,r9
    not eax
    mov dword ptr [rcx + r8*0x4 + -0x4],eax
    jl .lbl1800054d0
    .lbl1800054e4: mov rax,rcx
    ret

section .data


section .text
EXTR_FUN_1800054f0:
    test r8d,r8d
    movsxd r9,r8d
    jle .lbl180005512
    mov r8,rcx
    sub rdx,rcx
    nop
    .lbl180005500: mov eax,dword ptr [rdx + r8*0x1]
    add r8,0x4
    and dword ptr [r8 + -0x4],eax
    sub r9,0x1
    jnz .lbl180005500
    .lbl180005512: mov rax,rcx
    ret

section .data


section .text
EXTR_FUN_180005460:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x30
    mov ebx,dword ptr [rsp + 0x60]
    mov edi,r9d
    mov esi,r8d
    mov dword ptr [rsp + 0x20],ebx
    mov rbp,rdx
    call EXTR_FUN_180005380
    mov r9d,edi
    mov r8d,esi
    mov rdx,rbp
    mov rcx,rax
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180005090
    mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x30
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180005380:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    sub rsp,0x20
    mov ebp,dword ptr [rsp + 0x60]
    xor r14d,r14d
    mov r12d,r9d
    lea eax,[rbp + -0x1]
    mov r13d,r8d
    mov rsi,rdx
    test eax,eax
    mov rbx,rcx
    jle .lbl1800053f8
    add eax,-0x1
    shr eax,1
    add eax,0x1
    mov edi,eax
    lea r14d,[rax + rax*0x1]
    nop
    nop
    nop
    .lbl1800053d0: mov r9d,r12d
    mov r8d,r13d
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_FUN_180005170
    mov r9d,r12d
    mov r8d,r13d
    mov rdx,rbx
    mov rcx,rsi
    call EXTR_FUN_180005170
    sub rdi,0x1
    jnz .lbl1800053d0
    .lbl1800053f8: cmp r14d,ebp
    jge .lbl180005435
    mov edi,r13d
    imul edi,r12d
    shl edi,0x2
    sub ebp,r14d
    nop
    nop
    .lbl180005410: mov r9d,r12d
    mov r8d,r13d
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_FUN_180005170
    mov r8d,edi
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_memcpy wrt ..plt
    sub rbp,0x1
    jnz .lbl180005410
    .lbl180005435: mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    mov rax,rbx
    mov rbx,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_180005170:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    push r15
    mov eax,dword ptr [rcx + 0x4]
    movsxd r15,r8d
    mov rsi,rcx
    lea rbp,[rcx + 0x4]
    mov r14,rdx
    movsxd r12,r9d
    lea r10,[r15*0x4]
    lea r11,[r12 + -0x2]
    lea r13d,[r8 + -0x1]
    or eax,dword ptr [r10 + rcx*0x1]
    imul r11,r15
    or eax,dword ptr [rcx]
    mov dword ptr [rdx],eax
    mov eax,dword ptr [rcx + r15*0x8 + -0x4]
    or eax,dword ptr [rcx + r15*0x4 + -0x8]
    or eax,dword ptr [rcx + r15*0x4 + -0x4]
    mov rcx,r12
    mov dword ptr [rdx + r15*0x4 + -0x4],eax
    lea rdx,[r12 + -0x1]
    imul rcx,r15
    imul rdx,r15
    mov eax,dword ptr [rsi + rdx*0x4 + 0x4]
    or eax,dword ptr [rsi + rdx*0x4]
    or eax,dword ptr [rsi + r11*0x4]
    mov dword ptr [r14 + rdx*0x4],eax
    mov eax,dword ptr [rsi + rcx*0x4 + -0x8]
    or eax,dword ptr [rsi + rdx*0x4 + -0x4]
    or eax,dword ptr [rsi + rcx*0x4 + -0x4]
    cmp r13d,0x1
    mov dword ptr [r14 + rcx*0x4 + -0x4],eax
    jle .lbl18000526f
    lea r9,[r14 + rdx*0x4 + 0x4]
    lea rcx,[rsi + rdx*0x4 + 0x8]
    lea r8,[r14 + 0x4]
    lea rdx,[rsi + 0x8]
    lea r11,[rsi + r11*0x4 + 0x4]
    lea rbx,[r10 + rsi*0x1 + 0x4]
    lea edi,[r13 + -0x1]
    nop
    nop
    nop
    .lbl180005230: mov eax,dword ptr [rdx + -0x8]
    add r9,0x4
    add rbx,0x4
    or eax,dword ptr [rdx + -0x4]
    add rdx,0x4
    add r8,0x4
    or eax,dword ptr [rbx + -0x4]
    add rcx,0x4
    add r11,0x4
    or eax,dword ptr [rdx + -0x4]
    mov dword ptr [r8 + -0x4],eax
    mov eax,dword ptr [rcx + -0xc]
    or eax,dword ptr [rcx + -0x8]
    or eax,dword ptr [rcx + -0x4]
    or eax,dword ptr [r11 + -0x4]
    sub rdi,0x1
    mov dword ptr [r9 + -0x4],eax
    jnz .lbl180005230
    .lbl18000526f: add r12d,-0x1
    cmp r12d,0x1
    jle .lbl18000535c
    lea rax,[r15 + r15*0x2]
    mov rdi,r14
    lea r8,[r10 + r14*0x1]
    lea rdx,[r10 + rsi*0x1 + -0x4]
    mov r9,rsi
    lea rcx,[rsi + r15*0x8 + -0x4]
    lea r11,[rsi + rax*0x4 + -0x4]
    sub rdi,rsi
    lea ebx,[r12 + -0x1]
    nop
    nop
    nop
    nop
    .lbl1800052b0: mov eax,dword ptr [rcx + 0x4]
    or eax,dword ptr [rdx + 0x8]
    or eax,dword ptr [rdx + 0x4]
    or eax,dword ptr [r9]
    add r9,r10
    mov dword ptr [r8],eax
    mov eax,dword ptr [rcx + -0x4]
    add r8,r10
    or eax,dword ptr [r11]
    add r11,r10
    or eax,dword ptr [rcx]
    or eax,dword ptr [rdx]
    add rdx,r10
    mov dword ptr [rdi + rcx*0x1],eax
    add rcx,r10
    sub rbx,0x1
    jnz .lbl1800052b0
    cmp r12d,0x1
    jle .lbl18000535c
    lea rbx,[rsi + r15*0x4 + 0x8]
    lea rdi,[r10 + r14*0x1 + 0x4]
    add r12d,-0x1
    lea rsi,[rsi + r15*0x8 + 0x4]
    nop
    nop
    .lbl180005300: cmp r13d,0x1
    jle .lbl18000534a
    mov rdx,rdi
    mov rcx,rbx
    mov r8,rsi
    mov r9,rbp
    lea r11d,[r13 + -0x1]
    nop
    nop
    nop
    .lbl180005320: mov eax,dword ptr [rcx + -0x8]
    add rdx,0x4
    add r9,0x4
    or eax,dword ptr [rcx + -0x4]
    add r8,0x4
    add rcx,0x4
    or eax,dword ptr [r9 + -0x4]
    or eax,dword ptr [rcx + -0x4]
    or eax,dword ptr [r8 + -0x4]
    sub r11,0x1
    mov dword ptr [rdx + -0x4],eax
    jnz .lbl180005320
    .lbl18000534a: add rdi,r10
    add rbx,r10
    add rsi,r10
    add rbp,r10
    sub r12,0x1
    jnz .lbl180005300
    .lbl18000535c: mov rbx,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    mov rdi,qword ptr [rsp + 0x40]
    mov rax,r14
    pop r15
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_180005090:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    sub rsp,0x20
    mov ebp,dword ptr [rsp + 0x60]
    xor r14d,r14d
    mov r12d,r9d
    lea eax,[rbp + -0x1]
    mov r13d,r8d
    mov rsi,rdx
    test eax,eax
    mov rbx,rcx
    jle .lbl180005108
    add eax,-0x1
    shr eax,1
    add eax,0x1
    mov edi,eax
    lea r14d,[rax + rax*0x1]
    nop
    nop
    nop
    .lbl1800050e0: mov r9d,r12d
    mov r8d,r13d
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_FUN_180004e80
    mov r9d,r12d
    mov r8d,r13d
    mov rdx,rbx
    mov rcx,rsi
    call EXTR_FUN_180004e80
    sub rdi,0x1
    jnz .lbl1800050e0
    .lbl180005108: cmp r14d,ebp
    jge .lbl180005145
    mov edi,r13d
    imul edi,r12d
    shl edi,0x2
    sub ebp,r14d
    nop
    nop
    .lbl180005120: mov r9d,r12d
    mov r8d,r13d
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_FUN_180004e80
    mov r8d,edi
    mov rdx,rsi
    mov rcx,rbx
    call EXTR_memcpy wrt ..plt
    sub rbp,0x1
    jnz .lbl180005120
    .lbl180005145: mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    mov rax,rbx
    mov rbx,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_180004e80:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    push r15
    mov eax,dword ptr [rcx + 0x4]
    movsxd r14,r8d
    mov rsi,rcx
    lea rbp,[rcx + 0x4]
    mov r15,rdx
    movsxd r12,r9d
    lea r10,[r14*0x4]
    lea r11,[r12 + -0x2]
    lea r13d,[r8 + -0x1]
    and eax,dword ptr [r10 + rcx*0x1]
    imul r11,r14
    and eax,dword ptr [rcx]
    mov dword ptr [rdx],eax
    mov eax,dword ptr [rcx + r14*0x8 + -0x4]
    and eax,dword ptr [rcx + r14*0x4 + -0x8]
    and eax,dword ptr [rcx + r14*0x4 + -0x4]
    mov rcx,r12
    mov dword ptr [rdx + r14*0x4 + -0x4],eax
    lea rdx,[r12 + -0x1]
    imul rcx,r14
    imul rdx,r14
    mov eax,dword ptr [rsi + rdx*0x4 + 0x4]
    and eax,dword ptr [rsi + rdx*0x4]
    and eax,dword ptr [rsi + r11*0x4]
    mov dword ptr [r15 + rdx*0x4],eax
    mov eax,dword ptr [rsi + rcx*0x4 + -0x8]
    and eax,dword ptr [rsi + rdx*0x4 + -0x4]
    and eax,dword ptr [rsi + rcx*0x4 + -0x4]
    cmp r13d,0x1
    mov dword ptr [r15 + rcx*0x4 + -0x4],eax
    jle .lbl180004f7f
    lea r9,[r15 + rdx*0x4 + 0x4]
    lea rcx,[rsi + rdx*0x4 + 0x8]
    lea r8,[r15 + 0x4]
    lea rdx,[rsi + 0x8]
    lea r11,[rsi + r11*0x4 + 0x4]
    lea rbx,[r10 + rsi*0x1 + 0x4]
    lea edi,[r13 + -0x1]
    nop
    nop
    nop
    .lbl180004f40: mov eax,dword ptr [rdx + -0x8]
    add r9,0x4
    add rbx,0x4
    and eax,dword ptr [rdx + -0x4]
    add rdx,0x4
    add r8,0x4
    and eax,dword ptr [rbx + -0x4]
    add rcx,0x4
    add r11,0x4
    and eax,dword ptr [rdx + -0x4]
    mov dword ptr [r8 + -0x4],eax
    mov eax,dword ptr [rcx + -0xc]
    and eax,dword ptr [rcx + -0x8]
    and eax,dword ptr [rcx + -0x4]
    and eax,dword ptr [r11 + -0x4]
    sub rdi,0x1
    mov dword ptr [r9 + -0x4],eax
    jnz .lbl180004f40
    .lbl180004f7f: add r12d,-0x1
    cmp r12d,0x1
    jle .lbl18000506c
    lea rax,[r14 + r14*0x2]
    mov rdi,r15
    lea r8,[r10 + r15*0x1]
    lea rdx,[r10 + rsi*0x1 + -0x4]
    mov r9,rsi
    lea rcx,[rsi + r14*0x8 + -0x4]
    lea r11,[rsi + rax*0x4 + -0x4]
    sub rdi,rsi
    lea ebx,[r12 + -0x1]
    nop
    nop
    nop
    nop
    .lbl180004fc0: mov eax,dword ptr [rcx + 0x4]
    and eax,dword ptr [rdx + 0x8]
    and eax,dword ptr [rdx + 0x4]
    and eax,dword ptr [r9]
    add r9,r10
    mov dword ptr [r8],eax
    mov eax,dword ptr [rcx + -0x4]
    add r8,r10
    and eax,dword ptr [r11]
    add r11,r10
    and eax,dword ptr [rcx]
    and eax,dword ptr [rdx]
    add rdx,r10
    mov dword ptr [rdi + rcx*0x1],eax
    add rcx,r10
    sub rbx,0x1
    jnz .lbl180004fc0
    cmp r12d,0x1
    jle .lbl18000506c
    lea rbx,[rsi + r14*0x4 + 0x8]
    lea rdi,[r10 + r15*0x1 + 0x4]
    add r12d,-0x1
    lea rsi,[rsi + r14*0x8 + 0x4]
    nop
    nop
    .lbl180005010: cmp r13d,0x1
    jle .lbl18000505a
    mov rdx,rdi
    mov rcx,rbx
    mov r8,rsi
    mov r9,rbp
    lea r11d,[r13 + -0x1]
    nop
    nop
    nop
    .lbl180005030: mov eax,dword ptr [rcx + -0x8]
    add rdx,0x4
    add r9,0x4
    and eax,dword ptr [rcx + -0x4]
    add r8,0x4
    add rcx,0x4
    and eax,dword ptr [r9 + -0x4]
    and eax,dword ptr [rcx + -0x4]
    and eax,dword ptr [r8 + -0x4]
    sub r11,0x1
    mov dword ptr [rdx + -0x4],eax
    jnz .lbl180005030
    .lbl18000505a: add rdi,r10
    add rbx,r10
    add rsi,r10
    add rbp,r10
    sub r12,0x1
    jnz .lbl180005010
    .lbl18000506c: mov rbx,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    mov rdi,qword ptr [rsp + 0x40]
    mov rax,r15
    pop r15
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_1800124f0:
    mov qword ptr [rsp + 0x20],rdi
    push r12
    imul edx,r8d
    xor r10d,r10d
    mov r12,rcx
    test edx,edx
    movsxd rdi,edx
    jle .lbl1800125b4
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x18],rbp
    mov qword ptr [rsp + 0x20],rsi
    mov rsi,qword ptr [rsp + 0x30]
    lea rbp,[r9 + 0x4]
    nop
    nop
    nop
    .lbl180012530: movzx ebx,word ptr [r12 + r10*0x2]
    xor r9d,r9d
    mov r11,rbp
    lea r8d,[r9 + 0x2]
    nop
    .lbl180012540: xor edx,edx
    cmp bx,word ptr [r11 + -0x4]
    lea ecx,[r8 + -0x2]
    setg dl
    xor eax,eax
    shl edx,cl
    cmp bx,word ptr [r11 + -0x2]
    lea ecx,[r8 + -0x1]
    setg al
    shl eax,cl
    lea ecx,[r8 + 0x1]
    or edx,eax
    xor eax,eax
    cmp bx,word ptr [r11 + 0x2]
    setg al
    shl eax,cl
    mov ecx,r8d
    or edx,eax
    xor eax,eax
    cmp bx,word ptr [r11]
    setg al
    add r8d,0x4
    add r11,0x8
    shl eax,cl
    or edx,eax
    lea eax,[r8 + -0x2]
    or r9d,edx
    cmp eax,0x20
    jl .lbl180012540
    mov dword ptr [rsi + r10*0x4],r9d
    add r10,0x1
    cmp r10,rdi
    jl .lbl180012530
    mov rsi,qword ptr [rsp + 0x20]
    mov rbp,qword ptr [rsp + 0x18]
    mov rbx,qword ptr [rsp + 0x10]
    .lbl1800125b4: xor eax,eax
    mov rdi,qword ptr [rsp + 0x28]
    pop r12
    ret

section .data


section .text
EXTR_FUN_180014cf0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    sub rsp,0x30
    mov edi,edx
    mov byte ptr [rsp + 0x28],0x30
    mov rbp,r9
    mov r13d,r8d
    mov r14d,edx
    mov rsi,rcx
    imul edi,r8d
    movsxd rax,edi
    lea r12,[r9 + rax*0x2]
    mov qword ptr [rsp + 0x20],r12
    call EXTR_FUN_180012d30
    cmp eax,0x2
    mov ebx,eax
    jl .lbl180014dcf
    mov r9d,eax
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rbp
    mov qword ptr [rsp + 0x20],r12
    call EXTR_FUN_1800103d0
    xor r11w,r11w
    xor r8w,r8w
    cmp ebx,0x1
    mov cx,0x1
    jl .lbl180014d92
    nop
    nop
    .lbl180014d70: movsx rax,cx
    movzx edx,word ptr [r12 + rax*0x2]
    cmp r11w,dx
    jg .lbl180014d87
    movzx r11d,dx
    movzx r8d,cx
    .lbl180014d87: add cx,0x1
    movsx eax,cx
    cmp eax,ebx
    jle .lbl180014d70
    .lbl180014d92: xor dx,dx
    test edi,edi
    jle .lbl180014dbb
    nop
    nop
    .lbl180014da0: movsx rcx,dx
    cmp word ptr [rbp + rcx*0x2],r8w
    setz al
    add dx,0x1
    mov byte ptr [rcx + rsi*0x1],al
    movsx eax,dx
    cmp eax,edi
    jl .lbl180014da0
    .lbl180014dbb: mov r9d,0x1
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rsi
    call EXTR_FUN_180010550
    .lbl180014dcf: mov rbx,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    mov rdi,qword ptr [rsp + 0x68]
    xor eax,eax
    add rsp,0x30
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_1800103d0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    test r9d,r9d
    mov rdi,qword ptr [rsp + 0x50]
    mov ebp,r8d
    movsxd r8,r9d
    mov esi,edx
    mov rbx,rcx
    js .lbl18001040a
    add r8,0x1
    xor edx,edx
    mov rcx,rdi
    add r8,r8
    call EXTR_memset wrt ..plt
    .lbl18001040a: imul esi,ebp
    test esi,esi
    jle .lbl18001042c
    mov ecx,esi
    .lbl180010413: cmp word ptr [rbx],0x0
    jl .lbl180010422
    movsx rax,word ptr [rbx]
    add word ptr [rdi + rax*0x2],0x1
    .lbl180010422: add rbx,0x2
    sub rcx,0x1
    jnz .lbl180010413
    .lbl18001042c: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    xor eax,eax
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
extern EXTR_memset

section .text
EXTR_FUN_180004e20:
    xor r8d,r8d
    test edx,edx
    movsxd r9,edx
    jle .lbl180004e46
    nop
    nop
    .lbl180004e30: cmp byte ptr [r8 + rcx*0x1],0x0
    setz al
    add r8,0x1
    cmp r8,r9
    mov byte ptr [r8 + rcx*0x1 + -0x1],al
    jl .lbl180004e30
    .lbl180004e46: mov rax,rcx
    ret

section .data


section .text
EXTR_FUN_180015da0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x30
    lea eax,[r8 + r8*0x1]
    mov rbx,r9
    mov esi,r8d
    movsxd r10,eax
    mov ebp,edx
    mov r12,rcx
    lea rdi,[r9 + r10*0x8]
    call EXTR_FUN_180015a40
    mov r8,rdi
    mov edx,eax
    mov rcx,rbx
    call EXTR_FUN_180015ca0
    mov r9d,ebp
    mov r8,r12
    mov rcx,rdi
    mov edx,eax
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_180015b20
    mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    add rsp,0x30
    pop r12
    ret

section .data


section .text
EXTR_FUN_180015ca0:
    push rdi
    push r13
    push r14
    push r15
    sub rsp,0x28
    xor r15d,r15d
    xor r13d,r13d
    cmp edx,0x3
    mov r14,r8
    mov edi,edx
    mov r11,rcx
    jge .lbl180015ccd
    xor eax,eax
    add rsp,0x28
    pop r15
    pop r14
    pop r13
    pop rdi
    ret
    .lbl180015ccd: mov qword ptr [rsp + 0x50],rbx
    mov qword ptr [rsp + 0x58],rbp
    mov edx,0x1
    cmp edi,edx
    mov qword ptr [rsp + 0x60],rsi
    mov qword ptr [rsp + 0x20],r12
    jle .lbl180015d0d
    xor r8d,r8d
    mov ecx,0x8
    .lbl180015cf2: mov eax,dword ptr [r8 + r11*0x1]
    cmp dword ptr [rcx + r11*0x1],eax
    jge .lbl180015d02
    mov r13d,edx
    mov r8,rcx
    .lbl180015d02: add edx,0x1
    add rcx,0x8
    cmp edx,edi
    jl .lbl180015cf2
    .lbl180015d0d: mov ebx,r13d
    .lbl180015d10: movsxd r12,ebx
    add r15d,0x1
    add r14,0x8
    mov eax,dword ptr [r11 + r12*0x8]
    xor ebp,ebp
    mov dword ptr [r14 + -0x8],eax
    mov eax,dword ptr [r11 + r12*0x8 + 0x4]
    mov dword ptr [r14 + -0x4],eax
    lea eax,[rbx + 0x1]
    cdq
    idiv edi
    test edi,edi
    mov ebx,edx
    jle .lbl180015d64
    mov rsi,r11
    nop
    .lbl180015d40: mov rdx,qword ptr [rsi]
    mov rcx,qword ptr [r11 + r12*0x8]
    movsxd r8,ebx
    mov r8,qword ptr [r11 + r8*0x8]
    call EXTR_FUN_180015a00
    cmp eax,0x2
    cmovz ebx,ebp
    add ebp,0x1
    add rsi,0x8
    cmp ebp,edi
    jl .lbl180015d40
    .lbl180015d64: cmp ebx,r13d
    jnz .lbl180015d10
    mov r12,qword ptr [rsp + 0x20]
    mov rsi,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x58]
    mov rbx,qword ptr [rsp + 0x50]
    mov eax,r15d
    add rsp,0x28
    pop r15
    pop r14
    pop r13
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180015a00:
    mov r9,r8
    sub r8d,edx
    mov r10,rdx
    shr r10,0x20
    shr r9,0x20
    mov eax,edx
    sub eax,ecx
    sub r9d,r10d
    shr rcx,0x20
    sub r10d,ecx
    imul r9d,eax
    xor eax,eax
    imul r10d,r8d
    sub r10d,r9d
    jz .lbl180015a37
    test r10d,r10d
    setle al
    add eax,0x1
    .lbl180015a37: ret 0x0

section .data


section .text
EXTR_FUN_180015a40:
    mov qword ptr [rsp + 0x18],rbx
    mov qword ptr [rsp + 0x20],rsi
    push r12
    push r13
    push r14
    xor r10d,r10d
    xor ebx,ebx
    xor r12d,r12d
    test r8d,r8d
    mov r14d,r8d
    movsxd rsi,edx
    mov r13,rcx
    jle .lbl180015b0b
    mov qword ptr [rsp + 0x20],rbp
    mov qword ptr [rsp + 0x28],rdi
    .lbl180015a74: xor r8d,r8d
    xor ebp,ebp
    xor edx,edx
    xor r11d,r11d
    test esi,esi
    jle .lbl180015af1
    lea edi,[rsi + -0x1]
    .lbl180015a85: cmp ebp,0x1
    jnz .lbl180015a91
    cmp r8d,ebp
    jz .lbl180015af1
    jmp .lbl180015abb
    .lbl180015a91: mov rax,rsi
    imul rax,r12
    add rax,r11
    cmp byte ptr [rax + r13*0x1],0x0
    jz .lbl180015ab5
    mov ebp,0x1
    mov dword ptr [r9],edx
    mov dword ptr [r9 + 0x4],ebx
    add r9,0x8
    add r10d,ebp
    .lbl180015ab5: cmp r8d,0x1
    jz .lbl180015ae3
    .lbl180015abb: lea rcx,[r12 + 0x1]
    imul rcx,rsi
    sub rcx,r11
    cmp byte ptr [rcx + r13*0x1 + -0x1],0x0
    jz .lbl180015ae3
    mov r8d,0x1
    mov dword ptr [r9],edi
    mov dword ptr [r9 + 0x4],ebx
    add r9,0x8
    add r10d,r8d
    .lbl180015ae3: add edx,0x1
    sub edi,0x1
    add r11,0x1
    cmp edx,esi
    jl .lbl180015a85
    .lbl180015af1: add ebx,0x1
    add r12,0x1
    cmp ebx,r14d
    jl .lbl180015a74
    mov rdi,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x20]
    .lbl180015b0b: mov rbx,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    mov eax,r10d
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_180015b20:
    mov rax,rsp
    mov qword ptr [rax + 0x18],r8
    push rbx
    push rsi
    push r13
    push r14
    push r15
    sub rsp,0x40
    xor ebx,ebx
    movsxd r15,r9d
    mov r13d,edx
    cmp dword ptr [rsp + 0x90],ebx
    mov r14,rcx
    mov dword ptr [rsp + 0x20],ebx
    mov rsi,r15
    jle .lbl180015c8f
    mov qword ptr [rax + 0x8],rbp
    mov qword ptr [rax + 0x10],rdi
    mov qword ptr [rax + 0x20],r12
    mov r12,r8
    mov rax,rbx
    mov qword ptr [rsp + 0x28],rbx
    nop
    nop
    .lbl180015b70: mov dword ptr [rsp + 0x34],ebx
    xor edi,edi
    xor ebx,ebx
    test r15d,r15d
    jle .lbl180015c5a
    .lbl180015b81: mov dword ptr [rsp + 0x30],edi
    mov r8,qword ptr [rsp + 0x30]
    mov rdx,r14
    mov ecx,r13d
    call EXTR_FUN_180015850
    test eax,eax
    jnz .lbl180015bc2
    mov rdx,r14
    mov ecx,r13d
    call EXTR_FUN_180015920
    test eax,eax
    jnz .lbl180015bc2
    add edi,0x1
    mov byte ptr [r12 + rbx*0x1],al
    add rbx,0x1
    cmp edi,r15d
    jl .lbl180015b81
    mov rax,qword ptr [rsp + 0x28]
    jmp .lbl180015c5a
    .lbl180015bc2: test edi,edi
    js .lbl180015c55
    add rsi,-0x1
    lea ebp,[r15 + -0x1]
    cmp rsi,rbx
    jle .lbl180015c1b
    nop
    nop
    nop
    .lbl180015be0: mov dword ptr [rsp + 0x30],ebp
    mov r8,qword ptr [rsp + 0x30]
    mov rdx,r14
    mov ecx,r13d
    call EXTR_FUN_180015850
    test eax,eax
    jnz .lbl180015c19
    mov rdx,r14
    mov ecx,r13d
    call EXTR_FUN_180015920
    test eax,eax
    jnz .lbl180015c19
    mov byte ptr [r12 + rsi*0x1],al
    sub rsi,0x1
    sub ebp,0x1
    cmp rsi,rbx
    jg .lbl180015be0
    jmp .lbl180015c1b
    .lbl180015c19: mov edi,ebp
    .lbl180015c1b: movsxd r8,edi
    cmp rbx,r8
    jg .lbl180015c4b
    mov rcx,qword ptr [rsp + 0x28]
    sub r8,rbx
    mov dl,0x1
    add rcx,rbx
    add r8,0x1
    add rcx,qword ptr [rsp + 0x80]
    call EXTR_memset wrt ..plt
    mov rax,qword ptr [rsp + 0x28]
    mov rsi,r15
    jmp .lbl180015c5a
    .lbl180015c4b: mov rax,qword ptr [rsp + 0x28]
    mov rsi,r15
    jmp .lbl180015c5a
    .lbl180015c55: mov rax,qword ptr [rsp + 0x28]
    .lbl180015c5a: mov ebx,dword ptr [rsp + 0x20]
    add rax,rsi
    add r12,rsi
    add ebx,0x1
    mov qword ptr [rsp + 0x28],rax
    cmp ebx,dword ptr [rsp + 0x90]
    mov dword ptr [rsp + 0x20],ebx
    jl .lbl180015b70
    mov r12,qword ptr [rsp + 0x88]
    mov rdi,qword ptr [rsp + 0x78]
    mov rbp,qword ptr [rsp + 0x70]
    .lbl180015c8f: add rsp,0x40
    pop r15
    pop r14
    pop r13
    pop rsi
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180015920:
    mov qword ptr [rsp + 0x20],rsi
    mov qword ptr [rsp + 0x18],r8
    push rdi
    push r12
    lea eax,[rcx + -0x1]
    xor r11d,r11d
    test ecx,ecx
    mov rsi,rdx
    mov r12d,ecx
    movsxd rdi,eax
    jle .lbl1800159e7
    mov qword ptr [rsp + 0x18],rbx
    xor r10d,r10d
    mov qword ptr [rsp + 0x20],rbp
    mov ebp,dword ptr [rsp + 0x2c]
    shl rdi,0x3
    nop
    nop
    .lbl180015960: mov ebx,dword ptr [r10 + rsi*0x1 + 0x4]
    mov r9d,dword ptr [r10 + rsi*0x1]
    mov eax,r8d
    sub eax,dword ptr [rdi + rsi*0x1]
    mov edx,ebp
    mov ecx,r8d
    sub edx,ebx
    sub ecx,r9d
    imul edx,eax
    mov eax,ebp
    sub eax,dword ptr [rdi + rsi*0x1 + 0x4]
    imul ecx,eax
    cmp edx,ecx
    jnz .lbl1800159aa
    sub ebx,ebp
    mov eax,ebp
    sub eax,dword ptr [rdi + rsi*0x1 + 0x4]
    imul ebx,eax
    test ebx,ebx
    js .lbl1800159aa
    sub r9d,r8d
    mov eax,r8d
    sub eax,dword ptr [rdi + rsi*0x1]
    imul r9d,eax
    test r9d,r9d
    jns .lbl1800159cf
    .lbl1800159aa: add r11d,0x1
    mov rdi,r10
    add r10,0x8
    cmp r11d,r12d
    jl .lbl180015960
    mov rbx,qword ptr [rsp + 0x18]
    mov rbp,qword ptr [rsp + 0x20]
    xor eax,eax
    mov rsi,qword ptr [rsp + 0x30]
    pop r12
    pop rdi
    ret
    .lbl1800159cf: mov rbx,qword ptr [rsp + 0x18]
    mov rbp,qword ptr [rsp + 0x20]
    mov eax,0x1
    mov rsi,qword ptr [rsp + 0x30]
    pop r12
    pop rdi
    ret
    .lbl1800159e7: xor eax,eax
    mov rsi,qword ptr [rsp + 0x30]
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180015850:
    mov qword ptr [rsp + 0x18],r8
    push rbx
    push rdi
    push r12
    lea eax,[rcx + -0x1]
    xor ebx,ebx
    test ecx,ecx
    movsxd rdi,eax
    mov r12,rdx
    jle .lbl180015918
    mov qword ptr [rsp + 0x20],rbp
    mov ebp,dword ptr [rsp + 0x34]
    mov qword ptr [rsp + 0x28],rsi
    mov qword ptr [rsp + 0x38],r13
    mov r13,-0x4
    shl rdi,0x3
    lea r11,[rdx + 0x4]
    mov esi,ecx
    sub r13,rdx
    nop
    nop
    nop
    nop
    .lbl1800158a0: xor ecx,ecx
    cmp dword ptr [r11],ebp
    setge cl
    xor eax,eax
    cmp dword ptr [rdi + r12*0x1 + 0x4],ebp
    setge al
    cmp ecx,eax
    jz .lbl1800158fa
    mov edx,dword ptr [r11]
    mov r10d,dword ptr [r11 + -0x4]
    mov eax,dword ptr [rdi + r12*0x1]
    mov ecx,dword ptr [rdi + r12*0x1 + 0x4]
    sub eax,r10d
    mov r9d,ebp
    sub r9d,edx
    sub ecx,edx
    imul r10d,r10d,0x64
    imul r9d,eax
    imul r9d,r9d,0x64
    mov eax,r9d
    cdq
    idiv ecx
    lea ecx,[r10 + rax*0x1]
    mov eax,r8d
    imul eax,eax,0x64
    cmp eax,ecx
    jg .lbl1800158fa
    xor eax,eax
    test ebx,ebx
    setz al
    mov ebx,eax
    .lbl1800158fa: lea rdi,[r13 + r11*0x1]
    add r11,0x8
    sub rsi,0x1
    jnz .lbl1800158a0
    mov r13,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x20]
    .lbl180015918: mov eax,ebx
    pop r12
    pop rdi
    pop rbx
    ret

section .data


section .text
EXTR_FUN_1800125c0:
    mov qword ptr [rsp + 0x18],r8
    mov dword ptr [rsp + 0x10],edx
    push rbx
    push rsi
    push rdi
    sub rsp,0x30
    mov rdi,rcx
    mov ecx,dword ptr [rsp + 0x58]
    xor r10d,r10d
    xor r11d,r11d
    xor ebx,ebx
    xor eax,eax
    xor r9d,r9d
    cmp ecx,0x2
    mov rsi,r8
    jl .lbl180012617
    lea eax,[rcx + -0x2]
    shr eax,1
    add eax,0x1
    mov ecx,eax
    add eax,eax
    nop
    nop
    .lbl180012600: add r10d,dword ptr [rdi + r9*0x4]
    add r11d,dword ptr [rdi + r9*0x4 + 0x4]
    add r9,0x2
    sub rcx,0x1
    jnz .lbl180012600
    mov ecx,dword ptr [rsp + 0x58]
    .lbl180012617: cmp eax,ecx
    jnc .lbl18001261f
    mov ebx,dword ptr [rdi + r9*0x4]
    .lbl18001261f: lea eax,[r10 + r11*0x1]
    add ebx,eax
    jz .lbl18001264d
    test ecx,ecx
    jz .lbl18001264d
    mov r8d,dword ptr [rsp + 0x58]
    mov rcx,rdi
    .lbl180012633: mov eax,dword ptr [rcx]
    xor edx,edx
    add rcx,0x4
    shl eax,0x10
    div ebx
    sub r8,0x1
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180012633
    mov ecx,dword ptr [rsp + 0x58]
    .lbl18001264d: xor r9d,r9d
    xor r10d,r10d
    xor r8d,r8d
    test ecx,ecx
    mov dword ptr [rsi],0x0
    mov dword ptr [rsp],r9d
    mov qword ptr [rsp + 0x8],r10
    jz .lbl180012831
    mov qword ptr [rsp + 0x50],rbp
    mov qword ptr [rsp + 0x28],r12
    mov qword ptr [rsp + 0x20],r13
    mov qword ptr [rsp + 0x18],r14
    mov dword ptr [rsp + 0x68],ecx
    mov qword ptr [rsp + 0x10],r15
    nop
    nop
    .lbl180012690: xor ebx,ebx
    xor esi,esi
    xor r12d,r12d
    xor r13d,r13d
    xor r14d,r14d
    xor r9d,r9d
    xor r15d,r15d
    xor ebp,ebp
    xor edx,edx
    cmp r8d,0x2
    jl .lbl1800126f3
    lea eax,[r8 + -0x2]
    lea r10d,[rbx + 0x2]
    shr eax,1
    add eax,0x1
    mov r11d,eax
    lea ebp,[rax + rax*0x1]
    .lbl1800126c0: mov ecx,dword ptr [rdi + rdx*0x4]
    lea eax,[r10 + -0x1]
    add rdx,0x2
    add ebx,ecx
    imul eax,ecx
    add r12d,eax
    mov eax,dword ptr [rdi + rdx*0x4 + -0x4]
    add esi,eax
    imul eax,r10d
    add r10d,0x2
    add r13d,eax
    sub r11,0x1
    jnz .lbl1800126c0
    mov ecx,dword ptr [rsp + 0x58]
    mov r10,qword ptr [rsp + 0x8]
    .lbl1800126f3: cmp ebp,r8d
    jnc .lbl180012704
    mov r15d,dword ptr [rdi + rdx*0x4]
    lea r14d,[rbp + 0x1]
    imul r14d,r15d
    .lbl180012704: lea eax,[rsi + rbx*0x1]
    add r15d,eax
    jz .lbl180012720
    lea eax,[r13 + r12*0x1]
    add eax,r14d
    shl eax,0x8
    cdq
    idiv r15d
    mov r14d,eax
    jmp .lbl180012723
    .lbl180012720: xor r14d,r14d
    .lbl180012723: mov esi,dword ptr [rsp + 0x68]
    xor r11d,r11d
    xor ebx,ebx
    xor ebp,ebp
    xor r12d,r12d
    cmp esi,0x2
    mov r13d,r8d
    mov rdx,r10
    jl .lbl180012797
    mov eax,ecx
    lea r10d,[r8 + 0x2]
    sub eax,r8d
    sub eax,0x2
    shr eax,1
    add eax,0x1
    mov esi,eax
    lea r13d,[r8 + rax*0x2]
    nop
    nop
    nop
    nop
    .lbl180012760: mov ecx,dword ptr [rdi + rdx*0x4]
    lea eax,[r10 + -0x1]
    add rdx,0x2
    add r11d,ecx
    imul eax,ecx
    add ebp,eax
    mov eax,dword ptr [rdi + rdx*0x4 + -0x4]
    add ebx,eax
    imul eax,r10d
    add r10d,0x2
    add r12d,eax
    sub rsi,0x1
    jnz .lbl180012760
    mov ecx,dword ptr [rsp + 0x58]
    mov r10,qword ptr [rsp + 0x8]
    mov esi,dword ptr [rsp + 0x68]
    .lbl180012797: cmp r13d,ecx
    jnc .lbl1800127aa
    mov r9d,dword ptr [rdi + rdx*0x4]
    lea edx,[r13 + 0x1]
    imul edx,r9d
    jmp .lbl1800127ad
    .lbl1800127aa: mov edx,r9d
    .lbl1800127ad: lea eax,[rbx + r11*0x1]
    add r9d,eax
    jz .lbl1800127c5
    lea eax,[r12 + rbp*0x1]
    add eax,edx
    shl eax,0x8
    cdq
    idiv r9d
    jmp .lbl1800127c7
    .lbl1800127c5: xor eax,eax
    .lbl1800127c7: sar r9d,0x8
    sar r15d,0x8
    sub eax,r14d
    sar eax,0x8
    imul r9d,r15d
    imul r9d,eax
    imul r9d,eax
    cmp dword ptr [rsp],r9d
    jnc .lbl1800127fb
    test r8d,r8d
    mov dword ptr [rsp],r9d
    jz .lbl1800127fb
    mov rdx,qword ptr [rsp + 0x60]
    lea eax,[r8 + -0x1]
    mov dword ptr [rdx],eax
    .lbl1800127fb: add esi,-0x1
    add r10,0x1
    add r8d,0x1
    cmp r8d,ecx
    mov dword ptr [rsp + 0x68],esi
    mov qword ptr [rsp + 0x8],r10
    jc .lbl180012690
    mov r15,qword ptr [rsp + 0x10]
    mov r14,qword ptr [rsp + 0x18]
    mov r13,qword ptr [rsp + 0x20]
    mov r12,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x50]
    .lbl180012831: add rsp,0x30
    pop rdi
    pop rsi
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180004aa0:
    EXTR_FUN_180004aa0: mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rsi
    push rdi
    sub rsp,0x60
    mov eax,dword ptr [rsp + 0xa0]
    movsxd rdi,r9d
    movsxd rbx,r8d
    cmp eax,0x1
    mov r11,rdx
    mov rsi,rcx
    jle .lbl180004d48
    add eax,-0x1
    mov r9d,edi
    mov r8d,ebx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180004aa0
    mov r11,qword ptr [rsp + 0x88]
    .lbl180004af1: movzx eax,byte ptr [rsi + 0x1]
    lea r8,[rdi + -0x1]
    lea rcx,[rbx + r11*0x1]
    or al,byte ptr [rsi]
    imul r8,rbx
    mov qword ptr [rsp + 0x58],rbp
    or al,byte ptr [rbx + rsi*0x1]
    lea rbp,[rsi + 0x1]
    mov qword ptr [rsp + 0x30],rcx
    mov byte ptr [r11],al
    movzx eax,byte ptr [rbx + rsi*0x1 + -0x2]
    lea rdx,[rsi + rbx*0x2 + -0x1]
    or al,byte ptr [rdx]
    lea r9,[rbx + rsi*0x1 + -0x1]
    mov r10,rbx
    or al,byte ptr [r9]
    mov qword ptr [rsp + 0x40],rbp
    mov byte ptr [rcx + -0x1],al
    movzx eax,byte ptr [r8 + rsi*0x1 + 0x1]
    mov rcx,rdi
    or al,byte ptr [r8 + rsi*0x1]
    imul rcx,rbx
    add rdi,-0x2
    imul rdi,rbx
    or al,byte ptr [rdi + rsi*0x1]
    mov byte ptr [r8 + r11*0x1],al
    movzx eax,byte ptr [rcx + rsi*0x1 + -0x2]
    or al,byte ptr [rcx + rsi*0x1 + -0x1]
    or al,byte ptr [r8 + rsi*0x1 + -0x1]
    mov byte ptr [rcx + r11*0x1 + -0x1],al
    lea eax,[rbx + -0x1]
    cmp eax,0x1
    mov dword ptr [rsp + 0xa0],eax
    jle .lbl180004bff
    mov ebp,dword ptr [rsp + 0xa0]
    lea rbx,[rdi + rsi*0x1 + 0x1]
    mov rdi,qword ptr [rsp + 0x88]
    lea rcx,[r8 + rsi*0x1 + 0x2]
    lea rax,[r10 + rsi*0x1 + 0x1]
    lea r11,[r8 + r11*0x1 + 0x1]
    add rdi,0x1
    lea r8,[rsi + 0x2]
    add ebp,-0x1
    mov rsi,rax
    nop
    .lbl180004bb0: movzx eax,byte ptr [r8 + -0x2]
    add r11,0x1
    add rsi,0x1
    or al,byte ptr [r8 + -0x1]
    add r8,0x1
    add rdi,0x1
    or al,byte ptr [rsi + -0x1]
    add rcx,0x1
    add rbx,0x1
    or al,byte ptr [r8 + -0x1]
    mov byte ptr [rdi + -0x1],al
    movzx eax,byte ptr [rcx + -0x3]
    or al,byte ptr [rcx + -0x2]
    or al,byte ptr [rcx + -0x1]
    or al,byte ptr [rbx + -0x1]
    sub rbp,0x1
    mov byte ptr [r11 + -0x1],al
    jnz .lbl180004bb0
    mov rbp,qword ptr [rsp + 0x40]
    mov rsi,qword ptr [rsp + 0x80]
    .lbl180004bff: mov ecx,dword ptr [rsp + 0x98]
    lea edi,[rcx + -0x1]
    cmp edi,0x1
    jle .lbl180004c70
    mov rcx,qword ptr [rsp + 0x30]
    lea rax,[rsi + r10*0x2]
    mov r8,rsi
    lea r11,[r10 + rax*0x1 + -0x1]
    mov rax,qword ptr [rsp + 0x88]
    lea ebx,[rdi + -0x1]
    sub rax,rsi
    mov rbp,rax
    .lbl180004c30: movzx eax,byte ptr [rdx + 0x1]
    or al,byte ptr [r9 + 0x2]
    or al,byte ptr [r9 + 0x1]
    or al,byte ptr [r8]
    add r8,r10
    mov byte ptr [rcx],al
    movzx eax,byte ptr [rdx + -0x1]
    add rcx,r10
    or al,byte ptr [r11]
    add r11,r10
    or al,byte ptr [rdx]
    or al,byte ptr [r9]
    add r9,r10
    mov byte ptr [rdx + rbp*0x1],al
    add rdx,r10
    sub rbx,0x1
    jnz .lbl180004c30
    mov ecx,dword ptr [rsp + 0x98]
    lea rbp,[rsi + 0x1]
    .lbl180004c70: cmp edi,0x1
    jle .lbl180004d17
    mov rdi,qword ptr [rsp + 0x30]
    mov r11d,dword ptr [rsp + 0xa0]
    lea eax,[rcx + -0x1]
    add rdi,0x1
    add eax,-0x1
    lea rbx,[r10 + rsi*0x1 + 0x2]
    mov qword ptr [rsp + 0x30],rax
    lea rsi,[rsi + r10*0x2 + 0x1]
    nop
    .lbl180004ca0: cmp r11d,0x1
    jle .lbl180004cf8
    mov rdx,rdi
    mov rcx,rbx
    mov r8,rsi
    mov r9,rbp
    add r11d,-0x1
    nop
    nop
    nop
    .lbl180004cc0: movzx eax,byte ptr [rcx + -0x2]
    add rdx,0x1
    add r9,0x1
    or al,byte ptr [rcx + -0x1]
    add r8,0x1
    add rcx,0x1
    or al,byte ptr [r9 + -0x1]
    or al,byte ptr [rcx + -0x1]
    or al,byte ptr [r8 + -0x1]
    sub r11,0x1
    mov byte ptr [rdx + -0x1],al
    jnz .lbl180004cc0
    mov rax,qword ptr [rsp + 0x30]
    mov r11d,dword ptr [rsp + 0xa0]
    .lbl180004cf8: add rdi,r10
    add rbx,r10
    add rsi,r10
    add rbp,r10
    sub rax,0x1
    mov qword ptr [rsp + 0x30],rax
    jnz .lbl180004ca0
    mov rsi,qword ptr [rsp + 0x80]
    .lbl180004d17: mov r8d,dword ptr [rsp + 0x90]
    mov rdx,qword ptr [rsp + 0x88]
    mov rcx,rsi
    imul r8d,dword ptr [rsp + 0x98]
    call EXTR_memcpy wrt ..plt
    mov rbp,qword ptr [rsp + 0x58]
    mov rax,rsi
    add rsp,0x60
    pop rdi
    pop rsi
    pop rbx
    ret
    .lbl180004d48: cmp eax,0x1
    jge .lbl180004af1
    mov rax,rcx
    add rsp,0x60
    pop rdi
    pop rsi
    pop rbx
    ret

section .data


section .text
EXTR_check_cookie:
    ret

section .text
EXTR_FUN_180003fb0:
    mov qword ptr [rsp + 0x18],rbp
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    movsxd r11,dword ptr [rsp + 0x48]
    movsxd rdi,dword ptr [rsp + 0x38]
    sub r8d,dword ptr [rsp + 0x50]
    movsxd rbp,r9d
    mov r9,rcx
    mov ecx,dword ptr [rsp + 0x40]
    mov eax,ebp
    mov r13,rdx
    mov r10,r11
    sub eax,edi
    sub eax,ecx
    cmp r11d,r8d
    movsxd rdx,eax
    jge .lbl18000405c
    mov qword ptr [rsp + 0x18],rbx
    mov rax,r11
    mov ebx,ebp
    sub rax,r11
    sub ebx,ecx
    mov qword ptr [rsp + 0x20],rsi
    lea rsi,[rdx + rdx*0x1]
    imul rax,rdx
    sub r8d,r11d
    lea r9,[r9 + rax*0x2]
    mov r11d,r8d
    .lbl180004011: cmp edi,ebx
    jge .lbl180004045
    mov rax,rbp
    mov r8d,ebx
    mov rcx,r9
    imul rax,r10
    add rax,rdi
    sub r8d,edi
    lea rdx,[r13 + rax*0x2]
    nop
    .lbl180004030: movzx eax,word ptr [rdx]
    add rcx,0x2
    add rdx,0x2
    sub r8,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl180004030
    .lbl180004045: add r10,0x1
    add r9,rsi
    sub r11,0x1
    jnz .lbl180004011
    mov rsi,qword ptr [rsp + 0x20]
    mov rbx,qword ptr [rsp + 0x18]
    .lbl18000405c: mov rbp,qword ptr [rsp + 0x28]
    mov rdi,qword ptr [rsp + 0x30]
    xor eax,eax
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_180003d50:
    mov qword ptr [rsp + 0x10],rbx
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    movsxd r15,dword ptr [rsp + 0x90]
    movsxd rbp,dword ptr [rsp + 0xa0]
    mov eax,dword ptr [rsp + 0xa8]
    mov r12d,dword ptr [rsp + 0xb8]
    movsxd rsi,r8d
    movsxd r14,r9d
    lea r13d,[rsi + rbp*0x1]
    lea edi,[r14 + r15*0x1]
    mov rbx,rdx
    add edi,dword ptr [rsp + 0x98]
    add eax,r13d
    test r12d,r12d
    mov dword ptr [rsp + 0x90],eax
    jnz .lbl180003dc8
    movzx edx,byte ptr [rsp + 0xb0]
    mov r8d,eax
    imul r8d,edi
    call EXTR_memset wrt ..plt
    .lbl180003dc8: mov rax,rbp
    xor r10d,r10d
    test rsi,rsi
    movsxd r8,edi
    mov qword ptr [rsp + 0x20],rsi
    mov qword ptr [rsp + 0x28],rax
    jle .lbl180003e39
    mov r12,qword ptr [rsp + 0x70]
    mov r11,rax
    imul r11,r8
    nop
    .lbl180003df0: xor ecx,ecx
    test r14,r14
    jle .lbl180003e22
    mov rdx,r14
    lea r9,[r11 + r15*0x1]
    imul rdx,r10
    add rdx,rbx
    add r9,r12
    nop
    nop
    .lbl180003e10: movzx eax,byte ptr [rdx + rcx*0x1]
    add rcx,0x1
    cmp rcx,r14
    mov byte ptr [r9 + rcx*0x1 + -0x1],al
    jl .lbl180003e10
    .lbl180003e22: add r10,0x1
    add r11,r8
    cmp r10,rsi
    jl .lbl180003df0
    mov r12d,dword ptr [rsp + 0xb8]
    mov rax,rbp
    .lbl180003e39: cmp r12d,0x1
    jnz .lbl180003f8a
    cmp ebp,r13d
    mov rdi,r15
    jge .lbl180003ec4
    mov rdx,rax
    mov r10,rax
    lea rsi,[r14 + r15*0x1]
    sub r10,rax
    imul rdx,r8
    add rdx,qword ptr [rsp + 0x70]
    imul r10,r14
    sub r13d,ebp
    mov r11d,r13d
    nop
    nop
    .lbl180003e70: xor ecx,ecx
    test r15d,r15d
    jle .lbl180003e92
    nop
    nop
    nop
    .lbl180003e80: movzx eax,byte ptr [r10 + rbx*0x1]
    add rcx,0x1
    cmp rcx,r15
    mov byte ptr [rdx + rcx*0x1 + -0x1],al
    jl .lbl180003e80
    .lbl180003e92: cmp rsi,r8
    mov rcx,rsi
    jge .lbl180003eb3
    lea r9,[r10 + r14*0x1]
    nop
    .lbl180003ea0: movzx eax,byte ptr [r9 + rbx*0x1 + -0x1]
    add rcx,0x1
    cmp rcx,r8
    mov byte ptr [rdx + rcx*0x1 + -0x1],al
    jl .lbl180003ea0
    .lbl180003eb3: add r10,r14
    add rdx,r8
    sub r11,0x1
    jnz .lbl180003e70
    mov rsi,qword ptr [rsp + 0x20]
    .lbl180003ec4: movsxd rbp,dword ptr [rsp + 0x90]
    xor r11d,r11d
    test r8,r8
    jle .lbl180003f8a
    mov r12d,dword ptr [rsp + 0x88]
    mov rdx,qword ptr [rsp + 0x28]
    neg rdi
    add r12d,-0x1
    add rsi,rdx
    neg r15d
    .lbl180003ef2: xor eax,eax
    test r15d,r15d
    cmovg eax,r15d
    cmp eax,r12d
    jge .lbl180003f0c
    xor r9d,r9d
    test r15d,r15d
    cmovg r9,rdi
    jmp .lbl180003f10
    .lbl180003f0c: lea r9,[r14 + -0x1]
    .lbl180003f10: test rdx,rdx
    mov r13,qword ptr [rsp + 0x70]
    jle .lbl180003f30
    lea rcx,[r11 + r13*0x1]
    nop
    .lbl180003f20: movzx eax,byte ptr [r9 + rbx*0x1]
    mov byte ptr [rcx],al
    add rcx,r8
    sub rdx,0x1
    jnz .lbl180003f20
    .lbl180003f30: cmp rsi,rbp
    jge .lbl180003f70
    mov r10,qword ptr [rsp + 0x20]
    mov rcx,rsi
    mov rdx,rbp
    add r10,-0x1
    imul rcx,r8
    add rcx,r11
    add rcx,r13
    imul r10,r14
    add r10,r9
    sub rdx,rsi
    nop
    nop
    .lbl180003f60: movzx eax,byte ptr [r10 + rbx*0x1]
    mov byte ptr [rcx],al
    add rcx,r8
    sub rdx,0x1
    jnz .lbl180003f60
    .lbl180003f70: mov rdx,qword ptr [rsp + 0x28]
    add r11,0x1
    add r15d,0x1
    add rdi,0x1
    cmp r11,r8
    jl .lbl180003ef2
    .lbl180003f8a: xor eax,eax
    mov rbx,qword ptr [rsp + 0x78]
    add rsp,0x30
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180014340:
    mov qword ptr [rsp + 0x20],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x8a0
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x898],rax
    mov rax,qword ptr [rsp + 0x920]
    mov r15,qword ptr [rsp + 0x928]
    mov ebp,r9d
    xor edi,edi
    mov qword ptr [rsp + 0x68],rax
    mov eax,r8d
    mov r13,rcx
    mov r9d,r8d
    mov r11d,edx
    mov dword ptr [rsp + 0x40],edx
    mov r10d,r8d
    mov dword ptr [rsp + 0x44],r8d
    imul r9d,edx
    cmp edx,r8d
    movsxd rsi,r9d
    mov dword ptr [rsp + 0x4c],r9d
    cmovg eax,edx
    cmp ebp,0x8
    lea r14,[r15 + rsi*0x4]
    cdqe
    mov word ptr [rsp + 0x870],0x37b
    mov word ptr [rsp + 0x872],0x459
    lea rcx,[r15 + rax*0x4]
    mov word ptr [rsp + 0x874],0x37b
    mov word ptr [rsp + 0x878],0xa
    mov qword ptr [rsp + 0x58],rcx
    lea rcx,[rcx + rax*0x4]
    mov word ptr [rsp + 0x87a],0x20
    lea rdx,[rcx + rax*0x4]
    mov word ptr [rsp + 0x87c],0x41
    mov word ptr [rsp + 0x87e],0x52
    mov word ptr [rsp + 0x880],0x41
    mov word ptr [rsp + 0x882],0x20
    mov word ptr [rsp + 0x884],0xa
    mov qword ptr [rsp + 0x50],rdx
    mov word ptr [rsp + 0x888],0x1f
    mov word ptr [rsp + 0x88a],0x29
    mov word ptr [rsp + 0x88c],0x31
    mov word ptr [rsp + 0x88e],0x34
    mov word ptr [rsp + 0x890],0x31
    mov word ptr [rsp + 0x892],0x29
    mov word ptr [rsp + 0x894],0x1f
    mov dword ptr [rsp + 0x48],edi
    lea r12,[r15 + rsi*0x2]
    lea rbx,[r14 + rsi*0x4]
    mov qword ptr [rsp + 0x60],rcx
    jz .lbl180014500
    cmp ebp,0x5
    jz .lbl180014500
    cmp ebp,0x9
    jz .lbl180014500
    xor ecx,ecx
    test r9d,r9d
    jle .lbl1800144b3
    nop
    nop
    .lbl1800144a0: movsx eax,word ptr [r13 + rcx*0x2]
    add rcx,0x1
    cmp rcx,rsi
    mov dword ptr [rbx + rcx*0x4 + -0x4],eax
    jl .lbl1800144a0
    .lbl1800144b3: mov r12d,r8d
    .lbl1800144b6: cmp ebp,0x9
    jl .lbl18001462e
    cmp ebp,0xa
    jg .lbl18001462e
    xor ecx,ecx
    test rsi,rsi
    jle .lbl1800144e1
    nop
    .lbl1800144d0: mov eax,dword ptr [rbx + rcx*0x4]
    cmp eax,edi
    cmovg edi,eax
    add rcx,0x1
    cmp rcx,rsi
    jl .lbl1800144d0
    .lbl1800144e1: mov ecx,edi
    call EXTR_log2ceil
    mov edx,0xf
    mov ecx,edx
    sub ecx,eax
    test ecx,ecx
    jle .lbl18001461a
    sub dl,al
    jmp .lbl18001461c
    .lbl180014500: xor edx,edx
    xor r8d,r8d
    xor ecx,ecx
    cmp rsi,0x2
    jl .lbl180014530
    lea r9,[rsi + -0x1]
    .lbl180014511: movsx eax,word ptr [r13 + rcx*0x2]
    add rcx,0x2
    add edx,eax
    movsx eax,word ptr [r13 + rcx*0x2 + -0x2]
    add r8d,eax
    cmp rcx,r9
    jl .lbl180014511
    mov r9d,dword ptr [rsp + 0x4c]
    .lbl180014530: cmp rcx,rsi
    jge .lbl18001453b
    movsx edi,word ptr [r13 + rcx*0x2]
    .lbl18001453b: lea eax,[rdx + r8*0x1]
    mov rcx,r13
    mov r8d,r10d
    add eax,edi
    cdq
    idiv r9d
    mov r9,rbx
    mov edx,r11d
    mov edi,eax
    call EXTR_FUN_180012130
    xor r11d,r11d
    test rsi,rsi
    jle .lbl180014574
    .lbl180014560: movzx ecx,word ptr [rbx + r11*0x4]
    add r11,0x1
    cmp r11,rsi
    mov word ptr [r15 + r11*0x2 + -0x2],cx
    jl .lbl180014560
    .lbl180014574: mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],r12
    lea r9,[rsp + 0x870]
    mov rcx,r15
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x3
    call EXTR_FUN_180002820
    mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],r15
    lea r9,[rsp + 0x870]
    mov rcx,r12
    mov dword ptr [rsp + 0x28],0x3
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    mov r12d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov r8d,r12d
    mov r9d,0x1
    mov rcx,r15
    call EXTR_FUN_180010770
    xor r11d,r11d
    test rsi,rsi
    jle .lbl1800144b6
    nop
    nop
    nop
    .lbl180014600: movsx eax,word ptr [r15 + r11*0x2]
    add r11,0x1
    add eax,edi
    cmp r11,rsi
    mov dword ptr [rbx + r11*0x4 + -0x4],eax
    jl .lbl180014600
    jmp .lbl1800144b6
    .lbl18001461a: xor dl,dl
    .lbl18001461c: movzx eax,dl
    cmp eax,0x1
    jns .lbl180014629
    xor dil,dil
    jmp .lbl180014633
    .lbl180014629: lea edi,[rdx + -0x1]
    jmp .lbl180014633
    .lbl18001462e: mov dl,0xc
    mov dil,0x9
    .lbl180014633: xor eax,eax
    test rsi,rsi
    jle .lbl18001464c
    movzx ecx,dl
    nop
    .lbl180014640: shl dword ptr [rbx + rax*0x4],cl
    add rax,0x1
    cmp rax,rsi
    jl .lbl180014640
    .lbl18001464c: mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],r14
    mov ebp,0x1
    lea r9,[rsp + 0x878]
    mov r8d,r12d
    mov rcx,rbx
    mov dword ptr [rsp + 0x28],ebp
    mov dword ptr [rsp + 0x20],0x7
    call EXTR_FUN_180002f50
    mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],r15
    lea r9,[rsp + 0x878]
    mov r8d,r12d
    mov rcx,r14
    mov dword ptr [rsp + 0x28],0x7
    mov dword ptr [rsp + 0x20],ebp
    call EXTR_FUN_180002f50
    mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],r14
    lea r9,[rsp + 0x888]
    mov r8d,r12d
    mov rcx,rbx
    mov dword ptr [rsp + 0x28],ebp
    mov dword ptr [rsp + 0x20],0x7
    call EXTR_FUN_180002f50
    mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],rbx
    lea r9,[rsp + 0x888]
    mov r8d,r12d
    mov rcx,r14
    mov dword ptr [rsp + 0x28],0x7
    mov dword ptr [rsp + 0x20],ebp
    call EXTR_FUN_180002f50
    xor r11d,r11d
    test rsi,rsi
    jle .lbl18001471c
    movzx ecx,dil
    mov rdx,r15
    sub rbx,r15
    .lbl180014703: mov eax,dword ptr [rdx + rbx*0x1]
    add rdx,0x4
    sub eax,dword ptr [rdx + -0x4]
    sar eax,cl
    mov word ptr [r15 + r11*0x2],ax
    add r11,rbp
    cmp r11,rsi
    jl .lbl180014703
    .lbl18001471c: mov edi,dword ptr [rsp + 0x40]
    mov r9d,0x3
    mov r8d,r12d
    mov edx,edi
    mov rcx,r15
    call EXTR_FUN_180010770
    lea rcx,[rsp + 0x70]
    xor edx,edx
    mov r8d,0x800
    call EXTR_memset wrt ..plt
    test edi,edi
    jle .lbl1800147ab
    mov ebx,dword ptr [rsp + 0x908]
    movsxd rbp,r12d
    mov r11,r13
    add rbp,rbp
    nop
    nop
    .lbl180014760: test r12d,r12d
    jle .lbl18001479b
    mov r10,r15
    mov r8,r11
    mov r9d,r12d
    sub r10,r13
    .lbl180014771: movsx eax,word ptr [r10 + r8*0x1]
    movsx ecx,word ptr [r8]
    cdq
    sub eax,edx
    sar eax,1
    cmp ecx,ebx
    jge .lbl180014791
    cmp eax,0x1ff
    ja .lbl180014791
    cdqe
    add dword ptr [rsp + rax*0x4 + 0x70],0x1
    .lbl180014791: add r8,0x2
    sub r9,0x1
    jnz .lbl180014771
    .lbl18001479b: add r11,rbp
    sub rdi,0x1
    jnz .lbl180014760
    lea ebp,[rdi + 0x1]
    mov edi,dword ptr [rsp + 0x40]
    .lbl1800147ab: lea r8,[rsp + 0x48]
    lea rcx,[rsp + 0x70]
    mov edx,0x200
    call EXTR_FUN_1800125c0
    mov eax,dword ptr [rsp + 0x900]
    movzx r11d,word ptr [rsp + 0x48]
    mov r14,qword ptr [rsp + 0x68]
    add r11w,r11w
    cmp r11w,ax
    cmovnc r11w,ax
    xor ecx,ecx
    test rsi,rsi
    jle .lbl180014808
    movzx edx,r11w
    nop
    nop
    .lbl1800147f0: movsx eax,word ptr [r15 + rcx*0x2]
    cmp eax,edx
    setg al
    add rcx,0x1
    cmp rcx,rsi
    mov byte ptr [rcx + r14*0x1 + -0x1],al
    jl .lbl1800147f0
    .lbl180014808: mov r9d,dword ptr [rsp + 0x910]
    mov r8d,r12d
    mov edx,edi
    mov rcx,r14
    mov qword ptr [rsp + 0x28],r15
    mov dword ptr [rsp + 0x20],0x5
    call EXTR_FUN_1800130b0
    mov r9d,0x2
    mov r8d,r12d
    mov edx,edi
    mov rcx,r14
    call EXTR_FUN_180010550
    movsxd rbx,r12d
    xor esi,esi
    test rbx,rbx
    mov r11,rsi
    movsxd r12,edi
    jle .lbl18001487b
    mov rdx,r15
    .lbl180014850: test edi,edi
    mov dword ptr [rdx],esi
    jle .lbl18001486e
    lea rcx,[r11 + r14*0x1]
    mov r8,r12
    nop
    .lbl180014860: movzx eax,byte ptr [rcx]
    add rcx,rbx
    add dword ptr [rdx],eax
    sub r8,0x1
    jnz .lbl180014860
    .lbl18001486e: add r11,0x1
    add rdx,0x4
    cmp r11,rbx
    jl .lbl180014850
    .lbl18001487b: mov r11d,dword ptr [rsp + 0x44]
    mov r10,qword ptr [rsp + 0x58]
    mov edx,ebp
    lea r9d,[r11 + -0x3]
    mov dword ptr [r10],esi
    mov ecx,ebp
    cmp r9d,0x1
    jle .lbl1800148c9
    lea r8,[r10 + 0x4]
    lea rax,[r15 + 0xc]
    nop
    .lbl1800148a0: cmp dword ptr [rax + -0x8],esi
    jnz .lbl1800148bd
    cmp dword ptr [rax + -0x4],esi
    jnz .lbl1800148bd
    cmp dword ptr [rax],esi
    jnz .lbl1800148bd
    cmp dword ptr [rax + 0x4],esi
    jnz .lbl1800148bd
    mov dword ptr [r8],ecx
    add edx,0x1
    add r8,0x4
    .lbl1800148bd: add ecx,0x1
    add rax,0x4
    cmp ecx,r9d
    jl .lbl1800148a0
    .lbl1800148c9: mov r13,qword ptr [rsp + 0x60]
    movsxd rax,edx
    lea edi,[r11 + -0x1]
    add edx,0x1
    mov dword ptr [r10 + rax*0x4],edi
    cmp edx,0x2
    jle .lbl180014a6a
    lea eax,[rdx + -0x1]
    mov dword ptr [r13],esi
    mov r11d,ebp
    cmp eax,0x1
    jle .lbl180014925
    add eax,-0x1
    lea rcx,[r10 + 0x4]
    lea rdx,[r13 + 0x4]
    mov r10d,eax
    .lbl180014902: mov r8d,dword ptr [rcx]
    mov eax,dword ptr [rcx + 0x4]
    sub eax,r8d
    cmp eax,0x1
    jle .lbl18001491b
    mov dword ptr [rdx],r8d
    add r11d,0x1
    add rdx,0x4
    .lbl18001491b: add rcx,0x4
    sub r10,0x1
    jnz .lbl180014902
    .lbl180014925: movsxd rax,r11d
    cmp dword ptr [r13 + rax*0x4 + -0x4],edi
    jz .lbl180014938
    mov dword ptr [r13 + rax*0x4],edi
    add r11d,0x1
    .lbl180014938: mov rsi,qword ptr [rsp + 0x50]
    movsxd rax,r11d
    cmp rax,0x1
    jle .lbl1800149b7
    mov r10,r13
    lea r9,[r13 + 0x4]
    mov rcx,rsi
    sub r10,rsi
    lea rdi,[rax + -0x1]
    nop
    nop
    nop
    .lbl180014960: mov dword ptr [rcx],0x0
    movsxd rdx,dword ptr [r10 + rcx*0x1]
    cmp edx,dword ptr [r9]
    jg .lbl180014991
    lea r8,[r15 + rdx*0x4]
    nop
    nop
    nop
    nop
    .lbl180014980: mov eax,dword ptr [r8]
    add edx,0x1
    add r8,0x4
    add dword ptr [rcx],eax
    cmp edx,dword ptr [r9]
    jle .lbl180014980
    .lbl180014991: mov r8d,dword ptr [r9]
    sub r8d,dword ptr [r10 + rcx*0x1]
    add r8d,0x1
    test r8d,r8d
    jle .lbl1800149a9
    mov eax,dword ptr [rcx]
    cdq
    idiv r8d
    mov dword ptr [rcx],eax
    .lbl1800149a9: add r9,0x4
    add rcx,0x4
    sub rdi,0x1
    jnz .lbl180014960
    .lbl1800149b7: add r11d,-0x1
    xor r8d,r8d
    xor r9d,r9d
    xor eax,eax
    test r11d,r11d
    jle .lbl1800149e9
    mov rcx,rsi
    nop
    nop
    .lbl1800149d0: mov edx,dword ptr [rcx]
    cmp edx,r8d
    jle .lbl1800149dd
    mov r8d,edx
    mov r9d,eax
    .lbl1800149dd: add eax,0x1
    add rcx,0x4
    cmp eax,r11d
    jl .lbl1800149d0
    .lbl1800149e9: xor esi,esi
    movsxd r10,r9d
    mov eax,dword ptr [r13 + r10*0x4]
    mov r8d,esi
    sub eax,0x1
    test eax,eax
    jle .lbl180014a31
    mov rdx,r14
    .lbl180014a00: test r12,r12
    jle .lbl180014a1c
    mov rax,rdx
    mov rcx,r12
    nop
    nop
    .lbl180014a10: mov byte ptr [rax],sil
    add rax,rbx
    sub rcx,0x1
    jnz .lbl180014a10
    .lbl180014a1c: mov eax,dword ptr [r13 + r10*0x4]
    add r8d,0x1
    add rdx,0x1
    sub eax,0x1
    cmp r8d,eax
    jl .lbl180014a00
    .lbl180014a31: mov eax,dword ptr [r13 + r10*0x4 + 0x4]
    add eax,0x1
    movsxd rdx,eax
    cmp rdx,rbx
    jge .lbl180014a65
    .lbl180014a41: test r12,r12
    jle .lbl180014a5c
    lea rax,[rdx + r14*0x1]
    mov rcx,r12
    nop
    .lbl180014a50: mov byte ptr [rax],sil
    add rax,rbx
    sub rcx,0x1
    jnz .lbl180014a50
    .lbl180014a5c: add rdx,0x1
    cmp rdx,rbx
    jl .lbl180014a41
    .lbl180014a65: mov r10,qword ptr [rsp + 0x58]
    .lbl180014a6a: test r12,r12
    jle .lbl180014aad
    mov r8,r14
    mov rdx,r15
    mov r9,r12
    nop
    nop
    .lbl180014a80: test rbx,rbx
    mov dword ptr [rdx],esi
    mov rcx,rsi
    jle .lbl180014aa0
    nop
    nop
    .lbl180014a90: movzx eax,byte ptr [r8 + rcx*0x1]
    add rcx,0x1
    add dword ptr [rdx],eax
    cmp rcx,rbx
    jl .lbl180014a90
    .lbl180014aa0: add rdx,0x4
    add r8,rbx
    sub r9,0x1
    jnz .lbl180014a80
    .lbl180014aad: mov r11d,dword ptr [rsp + 0x40]
    mov dword ptr [r10],esi
    mov edx,ebp
    lea r9d,[r11 + -0x3]
    mov ecx,ebp
    cmp r9d,0x1
    jle .lbl180014afd
    lea r8,[r10 + 0x4]
    lea rax,[r15 + 0xc]
    nop
    nop
    .lbl180014ad0: cmp dword ptr [rax + -0x8],0x0
    jnz .lbl180014af1
    cmp dword ptr [rax + -0x4],0x0
    jnz .lbl180014af1
    cmp dword ptr [rax],0x0
    jnz .lbl180014af1
    cmp dword ptr [rax + 0x4],0x0
    jnz .lbl180014af1
    mov dword ptr [r8],ecx
    add edx,0x1
    add r8,0x4
    .lbl180014af1: add ecx,0x1
    add rax,0x4
    cmp ecx,r9d
    jl .lbl180014ad0
    .lbl180014afd: movsxd rax,edx
    add r11d,-0x1
    add edx,0x1
    cmp edx,0x2
    mov dword ptr [r10 + rax*0x4],r11d
    jle .lbl180014c6c
    lea eax,[rdx + -0x1]
    mov dword ptr [r13],esi
    cmp eax,0x1
    jle .lbl180014b52
    add eax,-0x1
    lea rcx,[r10 + 0x4]
    lea rdx,[r13 + 0x4]
    mov r10d,eax
    nop
    .lbl180014b30: mov r8d,dword ptr [rcx]
    mov eax,dword ptr [rcx + 0x4]
    sub eax,r8d
    cmp eax,0x1
    jle .lbl180014b48
    mov dword ptr [rdx],r8d
    add ebp,0x1
    add rdx,0x4
    .lbl180014b48: add rcx,0x4
    sub r10,0x1
    jnz .lbl180014b30
    .lbl180014b52: movsxd rax,ebp
    cmp dword ptr [r13 + rax*0x4 + -0x4],r11d
    jz .lbl180014b64
    mov dword ptr [r13 + rax*0x4],r11d
    add ebp,0x1
    .lbl180014b64: mov rdi,qword ptr [rsp + 0x50]
    movsxd r11,ebp
    cmp r11,0x1
    jle .lbl180014bd7
    mov r10,r13
    lea r9,[r13 + 0x4]
    mov rcx,rdi
    sub r10,rdi
    add r11,-0x1
    .lbl180014b83: mov dword ptr [rcx],esi
    movsxd rdx,dword ptr [r10 + rcx*0x1]
    cmp edx,dword ptr [r9]
    jg .lbl180014bb1
    lea r8,[r15 + rdx*0x4]
    nop
    nop
    nop
    nop
    .lbl180014ba0: mov eax,dword ptr [r8]
    add edx,0x1
    add r8,0x4
    add dword ptr [rcx],eax
    cmp edx,dword ptr [r9]
    jle .lbl180014ba0
    .lbl180014bb1: mov r8d,dword ptr [r9]
    sub r8d,dword ptr [r10 + rcx*0x1]
    add r8d,0x1
    test r8d,r8d
    jle .lbl180014bc9
    mov eax,dword ptr [rcx]
    cdq
    idiv r8d
    mov dword ptr [rcx],eax
    .lbl180014bc9: add r9,0x4
    add rcx,0x4
    sub r11,0x1
    jnz .lbl180014b83
    .lbl180014bd7: lea r8d,[rbp + -0x1]
    mov edx,esi
    mov r9d,esi
    test r8d,r8d
    mov eax,esi
    jle .lbl180014bfe
    .lbl180014be7: mov ecx,dword ptr [rdi]
    cmp ecx,edx
    jle .lbl180014bf2
    mov edx,ecx
    mov r9d,eax
    .lbl180014bf2: add eax,0x1
    add rdi,0x4
    cmp eax,r8d
    jl .lbl180014be7
    .lbl180014bfe: movsxd rbp,r9d
    mov eax,dword ptr [r13 + rbp*0x4]
    sub eax,0x1
    test eax,eax
    jle .lbl180014c34
    mov rdi,r14
    .lbl180014c10: test rbx,rbx
    jle .lbl180014c22
    mov r8,rbx
    xor edx,edx
    mov rcx,rdi
    call EXTR_memset wrt ..plt
    .lbl180014c22: mov eax,dword ptr [r13 + rbp*0x4]
    add esi,0x1
    add rdi,rbx
    sub eax,0x1
    cmp esi,eax
    jl .lbl180014c10
    .lbl180014c34: mov eax,dword ptr [r13 + rbp*0x4 + 0x4]
    add eax,0x1
    movsxd rcx,eax
    cmp rcx,r12
    jge .lbl180014c6c
    mov rdi,rcx
    imul rdi,rbx
    add rdi,r14
    sub r12,rcx
    .lbl180014c51: test rbx,rbx
    jle .lbl180014c63
    mov r8,rbx
    xor edx,edx
    mov rcx,rdi
    call EXTR_memset wrt ..plt
    .lbl180014c63: add rdi,rbx
    sub r12,0x1
    jnz .lbl180014c51
    .lbl180014c6c: mov edi,dword ptr [rsp + 0x40]
    mov ebx,dword ptr [rsp + 0x44]
    mov r9,r15
    mov r8d,edi
    mov edx,ebx
    mov rcx,r14
    call EXTR_FUN_180015da0
    mov esi,dword ptr [rsp + 0x4c]
    mov rcx,r14
    mov edx,esi
    call EXTR_FUN_180004e20
    mov r9d,dword ptr [rsp + 0x918]
    mov r8d,ebx
    mov edx,edi
    mov rcx,r14
    mov qword ptr [rsp + 0x28],r15
    mov dword ptr [rsp + 0x20],0x0
    call EXTR_FUN_1800130b0
    mov edx,esi
    mov rcx,r14
    call EXTR_FUN_180004e20
    mov rcx,qword ptr [rsp + 0x898]
    xor rcx,rsp
    call EXTR_check_cookie
    mov rbx,qword ptr [rsp + 0x8f8]
    add rsp,0x8a0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180002f50:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    mov dword ptr [r11 + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [r11 + 0x8],rcx
    push rbp
    push rsi
    push r14
    sub rsp,0xe0
    movsxd r10,dword ptr [rsp + 0x120]
    movsxd rbp,edx
    mov rsi,r9
    mov eax,r10d
    movsxd r14,r8d
    cdq
    and eax,0x1
    xor eax,edx
    cmp eax,edx
    jz .lbl1800036ba
    movsxd rcx,dword ptr [rsp + 0x128]
    mov eax,ecx
    cdq
    and eax,0x1
    xor eax,edx
    cmp eax,edx
    jz .lbl1800036ba
    mov qword ptr [r11 + -0x20],rbx
    mov qword ptr [r11 + -0x28],rdi
    mov qword ptr [r11 + -0x30],r12
    mov qword ptr [r11 + -0x38],r13
    xor ebx,ebx
    mov eax,r10d
    cdq
    mov qword ptr [r11 + -0x40],r15
    mov qword ptr [rsp + 0x48],rcx
    sub eax,edx
    mov r9,rbx
    mov r13,r10
    sar eax,1
    mov qword ptr [rsp + 0x98],r10
    mov edi,eax
    mov eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    test r10d,r10d
    movsxd r15,eax
    mov dword ptr [rsp + 0x70],r15d
    jle .lbl18000303f
    lea rbx,[rcx + rcx*0x1]
    mov r10,rsi
    mov r11,r13
    mov r12,rcx
    nop
    .lbl180003000: test r12,r12
    jle .lbl180003034
    mov rdx,r10
    mov r8,r12
    nop
    nop
    .lbl180003010: movsx ecx,word ptr [rdx]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    jle .lbl180003020
    movsx eax,word ptr [rdx]
    jmp .lbl180003025
    .lbl180003020: movsx eax,word ptr [rdx]
    neg eax
    .lbl180003025: cdqe
    add rdx,0x2
    add r9,rax
    sub r8,0x1
    jnz .lbl180003010
    .lbl180003034: add r10,rbx
    sub r11,0x1
    jnz .lbl180003000
    xor ebx,ebx
    .lbl18000303f: mov rax,-0x7ffffffefffffffd
    mov qword ptr [rsp],r15
    mov qword ptr [rsp + 0x50],r14
    mov r8,rbp
    mov qword ptr [rsp + 0x90],rbp
    imul r9
    lea r12,[r9 + rdx*0x1]
    mov r9,r15
    mov r15,r14
    sar r12,0x1e
    mov rax,r12
    shr rax,0x3f
    add r12,rax
    mov eax,0x1
    cmovz r12,rax
    test ebp,ebp
    mov qword ptr [rsp + 0x58],r12
    jle .lbl180003684
    mov rdx,qword ptr [rsp + 0x130]
    mov eax,edi
    lea ecx,[rdi + -0x1]
    neg eax
    lea r10,[r14*0x4]
    mov dword ptr [rsp + 0xc],ecx
    mov dword ptr [rsp + 0x8],eax
    nop
    .lbl1800030b0: test r15,r15
    mov r14,rbx
    mov qword ptr [rsp + 0x20],rbx
    jle .lbl180003659
    mov ecx,dword ptr [rsp + 0x70]
    mov rbx,r9
    mov rdi,rdx
    mov dword ptr [rsp + 0x120],ecx
    mov qword ptr [rsp + 0x60],rbx
    mov qword ptr [rsp + 0x80],rdx
    nop
    .lbl1800030e0: xor eax,eax
    test r13,r13
    mov r8,rax
    mov qword ptr [rsp + 0x18],rax
    mov r10,rax
    mov qword ptr [rsp + 0x10],rax
    jle .lbl1800035ab
    movsxd rax,ecx
    mov qword ptr [rsp + 0x68],rsi
    mov qword ptr [rsp + 0x88],rax
    mov eax,dword ptr [rsp + 0x110]
    lea edx,[rcx + rax*0x1]
    mov ecx,dword ptr [rsp + 0x8]
    lea rax,[rbx + r15*0x1]
    mov dword ptr [rsp + 0x74],edx
    xor edx,edx
    mov qword ptr [rsp + 0xa0],rax
    mov eax,dword ptr [rsp + 0xc]
    mov r13,rdx
    mov qword ptr [rsp + 0x40],rdx
    mov rdx,qword ptr [rsp + 0x98]
    mov dword ptr [rsp + 0x38],ecx
    mov dword ptr [rsp + 0x28],eax
    mov qword ptr [rsp + 0x78],rdx
    nop
    .lbl180003150: test ecx,ecx
    js .lbl180003168
    lea eax,[rbp + -0x1]
    cmp ecx,eax
    jle .lbl180003166
    lea eax,[rbp + rbp*0x1]
    sub eax,ecx
    sub eax,0x1
    jmp .lbl180003168
    .lbl180003166: mov eax,ecx
    .lbl180003168: xor ecx,ecx
    movsxd rdx,eax
    mov rbx,rcx
    mov rdi,rcx
    mov rsi,rcx
    mov rbp,rcx
    mov r11,rcx
    mov rcx,qword ptr [rsp + 0x88]
    cmp rcx,0x2
    mov qword ptr [rsp + 0x30],rdx
    jl .lbl180003228
    mov r9,qword ptr [rsp + 0x68]
    mov rax,r15
    lea r8,[rcx + -0x2]
    imul rax,rdx
    mov rdx,qword ptr [rsp + 0x100]
    sub rax,r14
    add rax,qword ptr [rsp]
    shr r8,1
    add r8,0x1
    lea r10,[rdx + rax*0x4 + -0x8]
    lea r11,[r8 + r8*0x1]
    nop
    nop
    nop
    nop
    .lbl1800031d0: movsx rcx,word ptr [r9]
    movsxd rax,dword ptr [r10 + 0x4]
    add r9,0x4
    add rbx,rcx
    sub r10,0x8
    imul rax,rcx
    movsx rcx,word ptr [r9 + -0x2]
    cqo
    add rdi,rcx
    idiv r12
    add rsi,rax
    movsxd rax,dword ptr [r10 + 0x8]
    imul rax,rcx
    cqo
    idiv r12
    add rbp,rax
    sub r8,0x1
    jnz .lbl1800031d0
    mov r8,qword ptr [rsp + 0x18]
    mov r10,qword ptr [rsp + 0x10]
    mov rdx,qword ptr [rsp + 0x30]
    mov r9,qword ptr [rsp]
    mov rcx,qword ptr [rsp + 0x88]
    .lbl180003228: cmp r11,rcx
    jge .lbl18000326d
    mov rcx,qword ptr [rsp + 0x118]
    lea rax,[r13 + r11*0x1]
    movsx rcx,word ptr [rcx + rax*0x2]
    mov rax,r15
    add r10,rcx
    imul rax,rdx
    sub rax,r11
    mov r11,qword ptr [rsp + 0x100]
    sub rax,r14
    add rax,r9
    movsxd rax,dword ptr [r11 + rax*0x4 + -0x4]
    imul rax,rcx
    cqo
    idiv r12
    add r8,rax
    jmp .lbl180003275
    .lbl18000326d: mov r11,qword ptr [rsp + 0x100]
    .lbl180003275: mov edx,dword ptr [rsp + 0x128]
    mov ecx,dword ptr [rsp + 0x74]
    mov r9,qword ptr [rsp + 0x60]
    lea rax,[rdi + rbx*0x1]
    mov ebx,0x0
    add r10,rax
    lea rax,[rsi + rbp*0x1]
    mov r12,rbx
    add r8,rax
    cmp dword ptr [rsp + 0x120],0x0
    mov eax,edx
    cmovl r9,rbx
    cmp ecx,edx
    mov r13,rbx
    cmovl eax,ecx
    mov r14,rbx
    mov r15,rbx
    movsxd rbx,eax
    mov qword ptr [rsp + 0x10],r10
    mov qword ptr [rsp + 0x18],r8
    mov rax,rbx
    sub rax,r9
    cmp rax,0x2
    jl .lbl180003378
    mov rsi,qword ptr [rsp + 0x40]
    mov rcx,qword ptr [rsp + 0x118]
    mov rdx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x20]
    mov rdi,qword ptr [rsp + 0x58]
    lea rax,[rsi + r9*0x1]
    lea r10,[rcx + rax*0x2]
    mov rax,qword ptr [rsp + 0x50]
    mov r8,rbx
    sub r8,r9
    sub r8,0x2
    imul rax,rdx
    sub rax,qword ptr [rsp]
    shr r8,1
    add rax,rbp
    add rax,r9
    add r8,0x1
    lea r9,[r9 + r8*0x2]
    lea r11,[r11 + rax*0x4]
    nop
    nop
    nop
    .lbl180003330: movsx rcx,word ptr [r10]
    movsxd rax,dword ptr [r11]
    add r11,0x8
    add r12,rcx
    add r10,0x4
    imul rax,rcx
    movsx rcx,word ptr [r10 + -0x2]
    cqo
    add r13,rcx
    idiv rdi
    add r14,rax
    movsxd rax,dword ptr [r11 + -0x4]
    imul rax,rcx
    cqo
    idiv rdi
    add r15,rax
    sub r8,0x1
    jnz .lbl180003330
    mov r8,qword ptr [rsp + 0x18]
    mov r10,qword ptr [rsp + 0x10]
    jmp .lbl180003387
    .lbl180003378: mov rdi,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x20]
    .lbl180003387: cmp r9,rbx
    jge .lbl1800033d2
    mov rdx,qword ptr [rsp + 0x100]
    mov rcx,qword ptr [rsp + 0x118]
    mov r11,qword ptr [rsp + 0x30]
    lea rax,[rsi + r9*0x1]
    movsx rcx,word ptr [rcx + rax*0x2]
    mov rax,qword ptr [rsp + 0x50]
    add r10,rcx
    imul rax,r11
    sub rax,qword ptr [rsp]
    add rax,r9
    add rax,rbp
    movsxd rax,dword ptr [rdx + rax*0x4]
    imul rax,rcx
    cqo
    idiv rdi
    add r8,rax
    jmp .lbl1800033d7
    .lbl1800033d2: mov r11,qword ptr [rsp + 0x30]
    .lbl1800033d7: mov rcx,qword ptr [rsp + 0xa0]
    xor eax,eax
    mov rbx,rax
    mov rdi,rax
    mov rsi,rax
    mov rbp,rax
    lea rax,[r13 + r12*0x1]
    mov r12,qword ptr [rsp + 0x48]
    add r10,rax
    lea rax,[r15 + r14*0x1]
    mov r9,rcx
    add r8,rax
    mov rax,r12
    mov qword ptr [rsp + 0x10],r10
    sub rax,rcx
    mov qword ptr [rsp + 0x18],r8
    cmp rax,0x2
    jl .lbl1800034c9
    mov r13,qword ptr [rsp + 0x40]
    mov r15,qword ptr [rsp + 0x50]
    mov r14,qword ptr [rsp + 0x20]
    lea rax,[r13 + rcx*0x1]
    mov rcx,qword ptr [rsp + 0x118]
    mov r8,r12
    mov r12,qword ptr [rsp + 0x58]
    lea r10,[rcx + rax*0x2]
    mov rcx,qword ptr [rsp + 0x100]
    sub r8,r9
    lea rax,[r11 + 0x2]
    sub r8,0x2
    shr r8,1
    imul rax,r15
    sub rax,r9
    sub rax,r14
    add rax,qword ptr [rsp]
    add r8,0x1
    lea r11,[rcx + rax*0x4 + -0x8]
    lea r9,[r9 + r8*0x2]
    nop
    nop
    nop
    .lbl180003480: movsx rcx,word ptr [r10]
    movsxd rax,dword ptr [r11 + 0x4]
    add r10,0x4
    add rbx,rcx
    sub r11,0x8
    imul rax,rcx
    movsx rcx,word ptr [r10 + -0x2]
    cqo
    add rdi,rcx
    idiv r12
    add rsi,rax
    movsxd rax,dword ptr [r11 + 0x8]
    imul rax,rcx
    cqo
    idiv r12
    add rbp,rax
    sub r8,0x1
    jnz .lbl180003480
    mov r8,qword ptr [rsp + 0x18]
    mov r10,qword ptr [rsp + 0x10]
    jmp .lbl1800034dd
    .lbl1800034c9: mov r12,qword ptr [rsp + 0x58]
    mov r13,qword ptr [rsp + 0x40]
    mov r14,qword ptr [rsp + 0x20]
    mov r15,qword ptr [rsp + 0x50]
    .lbl1800034dd: mov r11,qword ptr [rsp + 0x48]
    cmp r9,r11
    jge .lbl180003531
    mov rdx,qword ptr [rsp + 0x100]
    mov rcx,qword ptr [rsp + 0x118]
    lea rax,[r13 + r9*0x1]
    movsx rcx,word ptr [rcx + rax*0x2]
    mov rax,qword ptr [rsp + 0x30]
    add rax,0x2
    add r10,rcx
    imul rax,r15
    sub rax,r9
    mov r9,qword ptr [rsp]
    sub rax,r14
    add rax,r9
    movsxd rax,dword ptr [rdx + rax*0x4 + -0x4]
    imul rax,rcx
    cqo
    idiv r12
    add r8,rax
    jmp .lbl180003535
    .lbl180003531: mov r9,qword ptr [rsp]
    .lbl180003535: mov ecx,dword ptr [rsp + 0x38]
    lea rax,[rdi + rbx*0x1]
    add r13,r11
    add r10,rax
    lea rax,[rsi + rbp*0x1]
    mov ebp,dword ptr [rsp + 0x108]
    add r8,rax
    mov eax,dword ptr [rsp + 0x28]
    add ecx,0x1
    sub eax,0x1
    lea rdx,[r11 + r11*0x1]
    mov qword ptr [rsp + 0x10],r10
    add qword ptr [rsp + 0x68],rdx
    sub qword ptr [rsp + 0x78],0x1
    mov qword ptr [rsp + 0x18],r8
    mov dword ptr [rsp + 0x28],eax
    mov dword ptr [rsp + 0x38],ecx
    mov qword ptr [rsp + 0x40],r13
    jnz .lbl180003150
    mov rsi,qword ptr [rsp + 0x118]
    mov ecx,dword ptr [rsp + 0x120]
    mov rbx,qword ptr [rsp + 0x60]
    mov rdi,qword ptr [rsp + 0x80]
    mov r13,qword ptr [rsp + 0x98]
    .lbl1800035ab: mov rax,r10
    cqo
    idiv r12
    test rax,rax
    jnz .lbl1800035db
    test r8,r8
    jz .lbl1800035d3
    mov rax,-0x80000000
    test r8,r8
    mov edx,0x7fffffff
    cmovg eax,edx
    mov dword ptr [rdi],eax
    jmp .lbl180003606
    .lbl1800035d3: xor edx,edx
    mov eax,edx
    mov dword ptr [rdi],edx
    jmp .lbl180003606
    .lbl1800035db: imul r8,r12
    mov rax,r8
    cqo
    idiv r10
    mov edx,0x7fffffff
    cmp rax,rdx
    jle .lbl1800035f6
    mov rax,rdx
    jmp .lbl180003604
    .lbl1800035f6: mov rdx,-0x80000000
    cmp rax,rdx
    cmovl rax,rdx
    .lbl180003604: mov dword ptr [rdi],eax
    .lbl180003606: add r14,0x1
    sub ecx,0x1
    add rdi,0x4
    sub rbx,0x1
    cmp r14,r15
    mov dword ptr [rsp + 0x120],ecx
    mov qword ptr [rsp + 0x20],r14
    mov qword ptr [rsp + 0x80],rdi
    mov qword ptr [rsp + 0x60],rbx
    jl .lbl1800030e0
    mov eax,dword ptr [rsp + 0x8]
    mov ecx,dword ptr [rsp + 0xc]
    mov rdx,qword ptr [rsp + 0x130]
    mov r8,qword ptr [rsp + 0x90]
    lea r10,[r15*0x4]
    xor ebx,ebx
    .lbl180003659: add eax,0x1
    sub ecx,0x1
    add rdx,r10
    sub r8,0x1
    mov dword ptr [rsp + 0x8],eax
    mov dword ptr [rsp + 0xc],ecx
    mov qword ptr [rsp + 0x90],r8
    mov qword ptr [rsp + 0x130],rdx
    jnz .lbl1800030b0
    .lbl180003684: mov r15,qword ptr [rsp + 0xb8]
    mov r13,qword ptr [rsp + 0xc0]
    mov r12,qword ptr [rsp + 0xc8]
    mov rdi,qword ptr [rsp + 0xd0]
    mov rbx,qword ptr [rsp + 0xd8]
    xor eax,eax
    add rsp,0xe0
    pop r14
    pop rsi
    pop rbp
    ret
    .lbl1800036ba: mov eax,0x1
    add rsp,0xe0
    pop r14
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_log2ceil:
    test ecx,ecx
    mov eax,0x1
    jns .lbl1800023bb
    neg ecx
    .lbl1800023bb: cmp ecx,eax
    jg .lbl1800023c2
    xor eax,eax
    ret
    .lbl1800023c2: sub ecx,eax
    test ecx,0xffff0000
    jz .lbl1800023d7
    and ecx,0xffff0000
    mov eax,0x11
    .lbl1800023d7: test ecx,0xff00ff00
    jz .lbl1800023e8
    and ecx,0xff00ff00
    add eax,0x8
    .lbl1800023e8: test ecx,0xf0f0f0f0
    jz .lbl1800023f9
    and ecx,0xf0f0f0f0
    add eax,0x4
    .lbl1800023f9: test ecx,0xcccccccc
    jz .lbl18000240a
    and ecx,0xcccccccc
    add eax,0x2
    .lbl18000240a: test ecx,0xaaaaaaaa
    jz .lbl180002415
    add eax,0x1
    .lbl180002415: ret 0x0

section .data


section .text
EXTR_FUN_180002820:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    mov dword ptr [r11 + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [r11 + 0x8],rcx
    push rbp
    push rsi
    push r14
    sub rsp,0xe0
    movsxd r10,dword ptr [rsp + 0x120]
    movsxd rbp,edx
    mov rsi,r9
    mov eax,r10d
    movsxd r14,r8d
    cdq
    and eax,0x1
    xor eax,edx
    cmp eax,edx
    jz .lbl180002f37
    movsxd rcx,dword ptr [rsp + 0x128]
    mov eax,ecx
    cdq
    and eax,0x1
    xor eax,edx
    cmp eax,edx
    jz .lbl180002f37
    mov qword ptr [r11 + -0x20],rbx
    mov qword ptr [r11 + -0x28],rdi
    mov qword ptr [r11 + -0x30],r12
    mov qword ptr [r11 + -0x38],r13
    xor ebx,ebx
    mov eax,r10d
    cdq
    mov qword ptr [r11 + -0x40],r15
    mov qword ptr [rsp + 0x48],rcx
    sub eax,edx
    mov r9d,ebx
    mov r13,r10
    sar eax,1
    mov qword ptr [rsp + 0x98],r10
    mov edi,eax
    mov eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    test r10d,r10d
    movsxd r15,eax
    mov dword ptr [rsp + 0x58],r15d
    jle .lbl18000290d
    lea rbx,[rcx + rcx*0x1]
    mov r10,rsi
    mov r11,r13
    mov r12,rcx
    nop
    .lbl1800028d0: test r12,r12
    jle .lbl180002902
    mov rdx,r10
    mov r8,r12
    nop
    nop
    .lbl1800028e0: movsx ecx,word ptr [rdx]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    jle .lbl1800028f0
    movsx eax,word ptr [rdx]
    jmp .lbl1800028f5
    .lbl1800028f0: movsx eax,word ptr [rdx]
    neg eax
    .lbl1800028f5: add r9d,eax
    add rdx,0x2
    sub r8,0x1
    jnz .lbl1800028e0
    .lbl180002902: add r10,rbx
    sub r11,0x1
    jnz .lbl1800028d0
    xor ebx,ebx
    .lbl18000290d: mov eax,0x80010003
    mov qword ptr [rsp + 0x18],r15
    mov qword ptr [rsp + 0x50],r14
    mov r8,rbp
    mov qword ptr [rsp + 0x90],rbp
    imul r9d
    lea r12d,[r9 + rdx*0x1]
    mov r9,r15
    mov r15,r14
    sar r12d,0xe
    mov eax,r12d
    shr eax,0x1f
    add r12d,eax
    mov eax,0x1
    cmovz r12d,eax
    test ebp,ebp
    mov dword ptr [rsp + 0x4],r12d
    jle .lbl180002f01
    mov rdx,qword ptr [rsp + 0x130]
    mov eax,edi
    lea ecx,[rdi + -0x1]
    neg eax
    lea r10,[r14 + r14*0x1]
    mov dword ptr [rsp + 0xc],ecx
    mov dword ptr [rsp + 0x10],eax
    .lbl180002972: test r15,r15
    mov r14,rbx
    mov qword ptr [rsp + 0x38],rbx
    jle .lbl180002ed6
    mov ecx,dword ptr [rsp + 0x58]
    mov rbx,r9
    mov rdi,rdx
    mov dword ptr [rsp + 0x8],ecx
    mov qword ptr [rsp + 0x70],rbx
    mov qword ptr [rsp + 0x80],rdx
    nop
    .lbl1800029a0: xor eax,eax
    test r13,r13
    mov r8d,eax
    mov dword ptr [rsp],eax
    mov r10d,eax
    mov dword ptr [rsp + 0x120],eax
    jle .lbl180002e37
    movsxd rax,ecx
    mov qword ptr [rsp + 0x60],rsi
    mov qword ptr [rsp + 0x78],rax
    mov eax,dword ptr [rsp + 0x110]
    lea edx,[rcx + rax*0x1]
    mov ecx,dword ptr [rsp + 0x10]
    lea rax,[rbx + r15*0x1]
    mov dword ptr [rsp + 0x68],edx
    xor edx,edx
    mov qword ptr [rsp + 0xa0],rax
    mov eax,dword ptr [rsp + 0xc]
    mov r13,rdx
    mov qword ptr [rsp + 0x20],rdx
    mov rdx,qword ptr [rsp + 0x98]
    mov dword ptr [rsp + 0x28],ecx
    mov dword ptr [rsp + 0x40],eax
    mov qword ptr [rsp + 0x88],rdx
    nop
    .lbl180002a10: test ecx,ecx
    js .lbl180002a28
    lea eax,[rbp + -0x1]
    cmp ecx,eax
    jle .lbl180002a26
    lea eax,[rbp + rbp*0x1]
    sub eax,ecx
    sub eax,0x1
    jmp .lbl180002a28
    .lbl180002a26: mov eax,ecx
    .lbl180002a28: xor ecx,ecx
    movsxd rdx,eax
    mov ebx,ecx
    mov edi,ecx
    mov esi,ecx
    mov ebp,ecx
    mov r11,rcx
    mov rcx,qword ptr [rsp + 0x78]
    cmp rcx,0x2
    mov qword ptr [rsp + 0x30],rdx
    jl .lbl180002ad2
    mov r9,qword ptr [rsp + 0x60]
    mov rax,r15
    lea r8,[rcx + -0x2]
    imul rax,rdx
    mov rdx,qword ptr [rsp + 0x100]
    sub rax,r14
    add rax,qword ptr [rsp + 0x18]
    shr r8,1
    add r8,0x1
    lea r10,[rdx + rax*0x2 + -0x4]
    lea r11,[r8 + r8*0x1]
    nop
    .lbl180002a80: movsx ecx,word ptr [r9]
    movsx eax,word ptr [r10 + 0x2]
    add r9,0x4
    add ebx,ecx
    sub r10,0x4
    imul eax,ecx
    movsx ecx,word ptr [r9 + -0x2]
    cdq
    add edi,ecx
    idiv r12d
    add esi,eax
    movsx eax,word ptr [r10 + 0x4]
    imul eax,ecx
    cdq
    idiv r12d
    add ebp,eax
    sub r8,0x1
    jnz .lbl180002a80
    mov r8d,dword ptr [rsp]
    mov r10d,dword ptr [rsp + 0x120]
    mov rdx,qword ptr [rsp + 0x30]
    mov r9,qword ptr [rsp + 0x18]
    mov rcx,qword ptr [rsp + 0x78]
    .lbl180002ad2: cmp r11,rcx
    jge .lbl180002b15
    mov rcx,qword ptr [rsp + 0x118]
    lea rax,[r13 + r11*0x1]
    movsx ecx,word ptr [rcx + rax*0x2]
    mov rax,r15
    add r10d,ecx
    imul rax,rdx
    sub rax,r11
    mov r11,qword ptr [rsp + 0x100]
    sub rax,r14
    add rax,r9
    movsx eax,word ptr [r11 + rax*0x2 + -0x2]
    imul eax,ecx
    cdq
    idiv r12d
    add r8d,eax
    jmp .lbl180002b1d
    .lbl180002b15: mov r11,qword ptr [rsp + 0x100]
    .lbl180002b1d: mov edx,dword ptr [rsp + 0x128]
    mov ecx,dword ptr [rsp + 0x68]
    mov r9,qword ptr [rsp + 0x70]
    lea eax,[rdi + rbx*0x1]
    mov ebx,0x0
    add r10d,eax
    lea eax,[rbp + rsi*0x1]
    mov r12d,ebx
    add r8d,eax
    cmp dword ptr [rsp + 0x8],0x0
    mov eax,edx
    cmovl r9,rbx
    cmp ecx,edx
    mov r13d,ebx
    cmovl eax,ecx
    mov r14d,ebx
    mov r15d,ebx
    movsxd rbx,eax
    mov dword ptr [rsp + 0x120],r10d
    mov dword ptr [rsp],r8d
    mov rax,rbx
    sub rax,r9
    cmp rax,0x2
    jl .lbl180002c16
    mov rsi,qword ptr [rsp + 0x20]
    mov rcx,qword ptr [rsp + 0x118]
    mov rdx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov edi,dword ptr [rsp + 0x4]
    lea rax,[rsi + r9*0x1]
    lea r10,[rcx + rax*0x2]
    mov rax,qword ptr [rsp + 0x50]
    mov r8,rbx
    sub r8,r9
    sub r8,0x2
    imul rax,rdx
    sub rax,qword ptr [rsp + 0x18]
    shr r8,1
    add rax,rbp
    add rax,r9
    add r8,0x1
    lea r9,[r9 + r8*0x2]
    lea r11,[r11 + rax*0x2]
    nop
    nop
    .lbl180002bd0: movsx ecx,word ptr [r10]
    movsx eax,word ptr [r11]
    add r11,0x4
    add r12d,ecx
    add r10,0x4
    imul eax,ecx
    movsx ecx,word ptr [r10 + -0x2]
    cdq
    add r13d,ecx
    idiv edi
    add r14d,eax
    movsx eax,word ptr [r11 + -0x2]
    imul eax,ecx
    cdq
    idiv edi
    add r15d,eax
    sub r8,0x1
    jnz .lbl180002bd0
    mov r8d,dword ptr [rsp]
    mov r10d,dword ptr [rsp + 0x120]
    jmp .lbl180002c24
    .lbl180002c16: mov edi,dword ptr [rsp + 0x4]
    mov rsi,qword ptr [rsp + 0x20]
    mov rbp,qword ptr [rsp + 0x38]
    .lbl180002c24: cmp r9,rbx
    jge .lbl180002c6c
    mov rdx,qword ptr [rsp + 0x100]
    mov rcx,qword ptr [rsp + 0x118]
    mov r11,qword ptr [rsp + 0x30]
    lea rax,[rsi + r9*0x1]
    movsx ecx,word ptr [rcx + rax*0x2]
    mov rax,qword ptr [rsp + 0x50]
    add r10d,ecx
    imul rax,r11
    sub rax,qword ptr [rsp + 0x18]
    add rax,r9
    add rax,rbp
    movsx eax,word ptr [rdx + rax*0x2]
    imul eax,ecx
    cdq
    idiv edi
    add r8d,eax
    jmp .lbl180002c71
    .lbl180002c6c: mov r11,qword ptr [rsp + 0x30]
    .lbl180002c71: mov rcx,qword ptr [rsp + 0xa0]
    xor eax,eax
    mov ebx,eax
    mov edi,eax
    mov esi,eax
    mov ebp,eax
    lea eax,[r13 + r12*0x1]
    mov r12,qword ptr [rsp + 0x48]
    add r10d,eax
    lea eax,[r15 + r14*0x1]
    mov r9,rcx
    add r8d,eax
    mov rax,r12
    mov dword ptr [rsp + 0x120],r10d
    sub rax,rcx
    mov dword ptr [rsp],r8d
    cmp rax,0x2
    jl .lbl180002d55
    mov r13,qword ptr [rsp + 0x20]
    mov r15,qword ptr [rsp + 0x50]
    mov r14,qword ptr [rsp + 0x38]
    lea rax,[r13 + rcx*0x1]
    mov rcx,qword ptr [rsp + 0x118]
    mov r8,r12
    mov r12d,dword ptr [rsp + 0x4]
    lea r10,[rcx + rax*0x2]
    mov rcx,qword ptr [rsp + 0x100]
    sub r8,r9
    lea rax,[r11 + 0x2]
    sub r8,0x2
    shr r8,1
    imul rax,r15
    sub rax,r9
    sub rax,r14
    add rax,qword ptr [rsp + 0x18]
    add r8,0x1
    lea r11,[rcx + rax*0x2 + -0x4]
    lea r9,[r9 + r8*0x2]
    .lbl180002d10: movsx ecx,word ptr [r10]
    movsx eax,word ptr [r11 + 0x2]
    add r10,0x4
    add ebx,ecx
    sub r11,0x4
    imul eax,ecx
    movsx ecx,word ptr [r10 + -0x2]
    cdq
    add edi,ecx
    idiv r12d
    add esi,eax
    movsx eax,word ptr [r11 + 0x4]
    imul eax,ecx
    cdq
    idiv r12d
    add ebp,eax
    sub r8,0x1
    jnz .lbl180002d10
    mov r8d,dword ptr [rsp]
    mov r10d,dword ptr [rsp + 0x120]
    jmp .lbl180002d69
    .lbl180002d55: mov r12d,dword ptr [rsp + 0x4]
    mov r13,qword ptr [rsp + 0x20]
    mov r14,qword ptr [rsp + 0x38]
    mov r15,qword ptr [rsp + 0x50]
    .lbl180002d69: mov r11,qword ptr [rsp + 0x48]
    cmp r9,r11
    jge .lbl180002dbb
    mov rdx,qword ptr [rsp + 0x100]
    mov rcx,qword ptr [rsp + 0x118]
    lea rax,[r13 + r9*0x1]
    movsx ecx,word ptr [rcx + rax*0x2]
    mov rax,qword ptr [rsp + 0x30]
    add rax,0x2
    add r10d,ecx
    imul rax,r15
    sub rax,r9
    mov r9,qword ptr [rsp + 0x18]
    sub rax,r14
    add rax,r9
    movsx eax,word ptr [rdx + rax*0x2 + -0x2]
    imul eax,ecx
    cdq
    idiv r12d
    add r8d,eax
    jmp .lbl180002dc0
    .lbl180002dbb: mov r9,qword ptr [rsp + 0x18]
    .lbl180002dc0: mov ecx,dword ptr [rsp + 0x28]
    lea eax,[rdi + rbx*0x1]
    add r13,r11
    add r10d,eax
    lea eax,[rbp + rsi*0x1]
    mov ebp,dword ptr [rsp + 0x108]
    add r8d,eax
    mov eax,dword ptr [rsp + 0x40]
    add ecx,0x1
    sub eax,0x1
    lea rdx,[r11 + r11*0x1]
    mov dword ptr [rsp + 0x120],r10d
    add qword ptr [rsp + 0x60],rdx
    sub qword ptr [rsp + 0x88],0x1
    mov dword ptr [rsp],r8d
    mov dword ptr [rsp + 0x40],eax
    mov dword ptr [rsp + 0x28],ecx
    mov qword ptr [rsp + 0x20],r13
    jnz .lbl180002a10
    mov rsi,qword ptr [rsp + 0x118]
    mov ecx,dword ptr [rsp + 0x8]
    mov rbx,qword ptr [rsp + 0x70]
    mov rdi,qword ptr [rsp + 0x80]
    mov r13,qword ptr [rsp + 0x98]
    .lbl180002e37: mov eax,r10d
    cdq
    idiv r12d
    test eax,eax
    jnz .lbl180002e65
    test r8d,r8d
    jz .lbl180002e5c
    mov eax,0xffff8000
    test r8d,r8d
    mov edx,0x7fff
    cmovg eax,edx
    mov word ptr [rdi],ax
    jmp .lbl180002e8a
    .lbl180002e5c: xor edx,edx
    mov eax,edx
    mov word ptr [rdi],ax
    jmp .lbl180002e8a
    .lbl180002e65: imul r8d,r12d
    mov eax,r8d
    cdq
    idiv r10d
    mov edx,0x7fff
    cmp eax,edx
    jle .lbl180002e7d
    mov eax,edx
    jmp .lbl180002e87
    .lbl180002e7d: mov edx,0xffff8000
    cmp eax,edx
    cmovl eax,edx
    .lbl180002e87: mov word ptr [rdi],ax
    .lbl180002e8a: add r14,0x1
    sub ecx,0x1
    add rdi,0x2
    sub rbx,0x1
    cmp r14,r15
    mov dword ptr [rsp + 0x8],ecx
    mov qword ptr [rsp + 0x38],r14
    mov qword ptr [rsp + 0x80],rdi
    mov qword ptr [rsp + 0x70],rbx
    jl .lbl1800029a0
    mov eax,dword ptr [rsp + 0x10]
    mov ecx,dword ptr [rsp + 0xc]
    mov rdx,qword ptr [rsp + 0x130]
    mov r8,qword ptr [rsp + 0x90]
    lea r10,[r15 + r15*0x1]
    xor ebx,ebx
    .lbl180002ed6: add eax,0x1
    sub ecx,0x1
    add rdx,r10
    sub r8,0x1
    mov dword ptr [rsp + 0x10],eax
    mov dword ptr [rsp + 0xc],ecx
    mov qword ptr [rsp + 0x90],r8
    mov qword ptr [rsp + 0x130],rdx
    jnz .lbl180002972
    .lbl180002f01: mov r15,qword ptr [rsp + 0xb8]
    mov r13,qword ptr [rsp + 0xc0]
    mov r12,qword ptr [rsp + 0xc8]
    mov rdi,qword ptr [rsp + 0xd0]
    mov rbx,qword ptr [rsp + 0xd8]
    xor eax,eax
    add rsp,0xe0
    pop r14
    pop rsi
    pop rbp
    ret
    .lbl180002f37: mov eax,0x1
    add rsp,0xe0
    pop r14
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_1800130b0:
    mov qword ptr [rsp + 0x20],rbx
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x90
    mov r13,qword ptr [rsp + 0xf8]
    movsxd r15,edx
    mov r12d,r8d
    mov eax,r15d
    xor edx,edx
    mov edi,r9d
    imul eax,r8d
    movsxd rcx,eax
    lea ebx,[rax + rax*0x1]
    mov qword ptr [rsp + 0x68],rcx
    lea rbp,[r13 + rcx*0x2]
    mov r8d,ebx
    lea rsi,[rbp + rcx*0x2]
    lea r14,[rsi + rcx*0x2]
    mov rcx,r13
    mov qword ptr [rsp + 0x50],r14
    call EXTR_memset wrt ..plt
    mov r8d,ebx
    xor edx,edx
    mov rcx,rbp
    call EXTR_memset wrt ..plt
    mov r8d,ebx
    xor edx,edx
    mov rcx,rsi
    call EXTR_memset wrt ..plt
    mov rbx,qword ptr [rsp + 0xd0]
    mov r9d,0x1
    mov r8d,r12d
    mov edx,r15d
    mov rcx,rbx
    call EXTR_FUN_180010450
    mov r9,r13
    mov r8d,r12d
    mov edx,r15d
    mov rcx,rbx
    mov byte ptr [rsp + 0x28],0x31
    mov qword ptr [rsp + 0x20],rbp
    call EXTR_FUN_180012d30
    mov r9d,0x1
    mov r8d,r12d
    mov edx,r15d
    mov rcx,r13
    movsxd rbx,eax
    call EXTR_FUN_180010770
    mov rcx,qword ptr [rsp + 0xd0]
    mov r9d,0x1
    mov r8d,r12d
    mov edx,r15d
    call EXTR_FUN_180010550
    mov rdx,qword ptr [rsp + 0x68]
    xor r12d,r12d
    test rdx,rdx
    mov rcx,r12
    jle .lbl1800131cd
    nop
    nop
    .lbl1800131b0: cmp word ptr [r13 + rcx*0x2],r12w
    jle .lbl1800131c4
    movsx rax,word ptr [r13 + rcx*0x2]
    add word ptr [rsi + rax*0x2 + -0x2],0x1
    .lbl1800131c4: add rcx,0x1
    cmp rcx,rdx
    jl .lbl1800131b0
    .lbl1800131cd: test ebx,ebx
    mov rdx,r12
    jle .lbl1800131fa
    nop
    nop
    nop
    .lbl1800131e0: movsx eax,word ptr [rsi + rdx*0x2]
    mov ecx,r12d
    cmp eax,edi
    setge cl
    add rdx,0x1
    cmp rdx,rbx
    mov word ptr [rsi + rdx*0x2 + -0x2],cx
    jl .lbl1800131e0
    .lbl1800131fa: movsxd rdi,dword ptr [rsp + 0xf0]
    lea r8d,[rbx*0x4]
    mov rcx,r14
    test edi,edi
    jnz .lbl18001321d
    mov dl,0x1
    call EXTR_memset wrt ..plt
    jmp .lbl18001343f
    .lbl18001321d: xor edx,edx
    call EXTR_memset wrt ..plt
    test r15d,r15d
    movsxd r9,dword ptr [rsp + 0xe0]
    mov rbx,rdi
    mov rcx,r15
    mov rdx,r9
    mov qword ptr [rsp + 0x60],rdx
    mov qword ptr [rsp + 0x88],rbx
    mov qword ptr [rsp + 0x58],rcx
    jle .lbl18001343f
    mov rax,rbx
    mov r11d,edi
    mov r15d,edi
    neg rax
    neg r11d
    mov r8,rbp
    mov qword ptr [rsp + 0x70],rbp
    lea r10,[r9 + r9*0x1]
    mov dword ptr [rsp + 0x30],r11d
    mov qword ptr [rsp + 0x48],rax
    .lbl180013275: test rdx,rdx
    jle .lbl18001340a
    mov rcx,rbx
    mov r13d,edi
    mov r10d,edi
    neg rcx
    mov qword ptr [rsp + 0x38],r8
    mov qword ptr [rsp + 0x78],rdx
    mov qword ptr [rsp + 0x40],rcx
    neg r13d
    nop
    .lbl1800132a0: movzx ebx,word ptr [r8]
    sub bx,0x1
    js .lbl1800133c2
    movsx rax,bx
    mov qword ptr [rsp + 0x80],rax
    cmp word ptr [rsi + rax*0x2],0x1
    jnz .lbl1800133c2
    cmp word ptr [r14 + rax*0x2],r12w
    jnz .lbl1800133c2
    test r11d,r11d
    jns .lbl1800132dd
    mov edi,r12d
    mov rax,r12
    jmp .lbl1800132e5
    .lbl1800132dd: mov rax,qword ptr [rsp + 0x48]
    mov edi,r11d
    .lbl1800132e5: mov r14d,dword ptr [rsp + 0xd8]
    mov r11,rdx
    add r14d,-0x1
    imul r11,rax
    nop
    nop
    .lbl180013300: cmp r14d,r15d
    mov eax,r15d
    cmovl eax,r14d
    cmp edi,eax
    jg .lbl1800133b3
    test r13d,r13d
    jns .lbl18001331f
    mov edx,r12d
    mov r8,r12
    jmp .lbl180013325
    .lbl18001331f: mov edx,r13d
    mov r8,rcx
    .lbl180013325: add r9d,-0x1
    nop
    nop
    .lbl180013330: cmp r9d,r10d
    mov eax,r10d
    cmovl eax,r9d
    cmp edx,eax
    jg .lbl180013366
    lea rax,[r11 + r8*0x1]
    movzx ecx,word ptr [rbp + rax*0x2]
    sub cx,0x1
    js .lbl18001335d
    cmp cx,bx
    jz .lbl18001335d
    movsx rax,cx
    cmp word ptr [rsi + rax*0x2],0x1
    jz .lbl180013380
    .lbl18001335d: add edx,0x1
    add r8,0x1
    jmp .lbl180013330
    .lbl180013366: mov rdx,qword ptr [rsp + 0x60]
    mov rcx,qword ptr [rsp + 0x40]
    mov r9d,dword ptr [rsp + 0xe0]
    add edi,0x1
    add r11,rdx
    jmp .lbl180013300
    .lbl180013380: mov rax,qword ptr [rsp + 0x80]
    mov r14,qword ptr [rsp + 0x50]
    mov rdx,qword ptr [rsp + 0x60]
    mov r9d,dword ptr [rsp + 0xe0]
    mov word ptr [r14 + rax*0x2],0x1
    movsx rax,cx
    mov rcx,qword ptr [rsp + 0x40]
    mov word ptr [r14 + rax*0x2],0x1
    jmp .lbl1800133b8
    .lbl1800133b3: mov r14,qword ptr [rsp + 0x50]
    .lbl1800133b8: mov r11d,dword ptr [rsp + 0x30]
    mov r8,qword ptr [rsp + 0x38]
    .lbl1800133c2: add r8,0x2
    add rcx,0x1
    add r13d,0x1
    add r10d,0x1
    sub qword ptr [rsp + 0x78],0x1
    mov qword ptr [rsp + 0x38],r8
    mov qword ptr [rsp + 0x40],rcx
    jnz .lbl1800132a0
    mov rax,qword ptr [rsp + 0x48]
    mov r8,qword ptr [rsp + 0x70]
    mov edi,dword ptr [rsp + 0xf0]
    mov rcx,qword ptr [rsp + 0x58]
    mov rbx,qword ptr [rsp + 0x88]
    lea r10,[rdx + rdx*0x1]
    .lbl18001340a: add r11d,0x1
    add r8,r10
    add rax,0x1
    add r15d,0x1
    sub rcx,0x1
    mov dword ptr [rsp + 0x30],r11d
    mov qword ptr [rsp + 0x70],r8
    mov qword ptr [rsp + 0x48],rax
    mov qword ptr [rsp + 0x58],rcx
    jnz .lbl180013275
    mov r13,qword ptr [rsp + 0xf8]
    .lbl18001343f: mov rdx,qword ptr [rsp + 0x68]
    test rdx,rdx
    jle .lbl180013486
    nop
    nop
    .lbl180013450: movzx eax,word ptr [r13 + r12*0x2]
    test ax,ax
    jle .lbl18001347d
    movsx rcx,ax
    cmp word ptr [rsi + rcx*0x2 + -0x2],0x0
    jz .lbl180013470
    cmp word ptr [r14 + rcx*0x2 + -0x2],0x0
    jnz .lbl18001347d
    .lbl180013470: mov rax,qword ptr [rsp + 0xd0]
    mov byte ptr [r12 + rax*0x1],0x0
    .lbl18001347d: add r12,0x1
    cmp r12,rdx
    jl .lbl180013450
    .lbl180013486: mov rbx,qword ptr [rsp + 0xe8]
    add rsp,0x90
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180012130:
    sub rsp,0x18
    mov qword ptr [rsp + 0x10],r12
    mov qword ptr [rsp + 0x8],r13
    mov qword ptr [rsp],r14
    movsxd r14,edx
    xor r10d,r10d
    movsxd r12,r8d
    lea eax,[r14 + -0x1]
    mov r13,rcx
    cmp eax,0x1
    jle .lbl1800121f9
    mov qword ptr [rsp + 0x20],rbx
    mov qword ptr [rsp + 0x28],rbp
    add eax,-0x1
    mov qword ptr [rsp + 0x30],rsi
    mov qword ptr [rsp + 0x38],rdi
    lea ebp,[r12 + -0x1]
    mov edi,eax
    mov r11,r10
    nop
    .lbl180012180: cmp ebp,0x1
    jle .lbl1800121db
    lea rax,[r11 + 0x1]
    lea esi,[rbp + -0x1]
    imul rax,r12
    lea rbx,[r9 + rax*0x4 + 0x4]
    lea rax,[r11 + 0x2]
    imul rax,r12
    lea rdx,[r13 + rax*0x2 + 0x4]
    mov rax,r11
    imul rax,r12
    lea r8,[r13 + rax*0x2 + 0x4]
    nop
    .lbl1800121b0: movsx eax,word ptr [rdx + -0x4]
    movsx ecx,word ptr [r8 + -0x4]
    add rbx,0x4
    sub ecx,eax
    movsx eax,word ptr [r8]
    add r8,0x2
    sub ecx,eax
    movsx eax,word ptr [rdx]
    add rdx,0x2
    add ecx,eax
    sub rsi,0x1
    mov dword ptr [rbx + -0x4],ecx
    jnz .lbl1800121b0
    .lbl1800121db: add r11,0x1
    sub rdi,0x1
    jnz .lbl180012180
    mov rdi,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x28]
    mov rbx,qword ptr [rsp + 0x20]
    .lbl1800121f9: mov r13,qword ptr [rsp + 0x8]
    mov r11,r12
    mov r12,qword ptr [rsp + 0x10]
    test r11,r11
    jle .lbl180012252
    lea rax,[r14 + -0x1]
    lea r8,[r9 + r11*0x4]
    imul rax,r11
    lea rdx,[r9 + rax*0x4]
    lea rax,[r14 + -0x2]
    imul rax,r11
    lea rcx,[r9 + rax*0x4]
    nop
    nop
    nop
    .lbl180012230: mov eax,dword ptr [r8]
    add r10,0x1
    add rdx,0x4
    mov dword ptr [r9 + r10*0x4 + -0x4],eax
    mov eax,dword ptr [rcx]
    add r8,0x4
    mov dword ptr [rdx + -0x4],eax
    add rcx,0x4
    cmp r10,r11
    jl .lbl180012230
    .lbl180012252: mov rdx,r14
    mov r14,qword ptr [rsp]
    test rdx,rdx
    jle .lbl180012288
    lea r8,[r11*0x4]
    lea rcx,[r8 + r9*0x1 + -0x4]
    nop
    nop
    .lbl180012270: mov eax,dword ptr [r9 + 0x4]
    mov dword ptr [r9],eax
    mov eax,dword ptr [rcx + -0x4]
    add r9,r8
    mov dword ptr [rcx],eax
    add rcx,r8
    sub rdx,0x1
    jnz .lbl180012270
    .lbl180012288: xor eax,eax
    add rsp,0x18
    ret

section .data


section .text
EXTR_FUN_180006ef0:
    mov dword ptr [rsp + 0x18],r8d
    push rbp
    push rdi
    push r12
    push r13
    push r15
    sub rsp,0x90
    mov eax,edx
    mov r15,rcx
    xor ecx,ecx
    mov r13d,0x64
    mov rdi,r9
    mov dword ptr [rsp + 0x30],0x3
    imul eax,r8d
    test eax,eax
    mov r12d,ecx
    mov ebp,ecx
    mov dword ptr [rsp + 0xc],eax
    mov qword ptr [rsp + 0x70],r13
    mov dword ptr [rsp],ecx
    mov dword ptr [rsp + 0x34],0x8
    mov dword ptr [rsp + 0x38],0x8
    mov dword ptr [rsp + 0x3c],0x3
    mov dword ptr [rsp + 0x40],0x8
    mov dword ptr [rsp + 0x44],0xffffffed
    mov dword ptr [rsp + 0x48],0xffffffed
    mov dword ptr [rsp + 0x4c],0x8
    mov dword ptr [rsp + 0x50],0x8
    mov dword ptr [rsp + 0x54],0xffffffed
    mov dword ptr [rsp + 0x58],0xffffffed
    mov dword ptr [rsp + 0x5c],0x8
    mov dword ptr [rsp + 0x60],0x3
    mov dword ptr [rsp + 0x64],0x8
    mov dword ptr [rsp + 0x68],0x8
    mov dword ptr [rsp + 0x6c],0x3
    jnz .lbl180006fc0
    lea eax,[rcx + 0x1]
    add rsp,0x90
    pop r15
    pop r13
    pop r12
    pop rdi
    pop rbp
    ret
    .lbl180006fc0: mov qword ptr [rsp + 0xc0],rbx
    mov qword ptr [rsp + 0x88],rsi
    movzx esi,word ptr [rsp + 0xe8]
    lea eax,[rdx + -0x2]
    mov qword ptr [rsp + 0x80],r14
    mov r14,qword ptr [rsp + 0xe0]
    cmp eax,0x2
    mov r10,rcx
    mov qword ptr [rsp + 0x20],rcx
    mov r9,rcx
    mov qword ptr [rsp + 0x10],rcx
    jle .lbl180007156
    add eax,-0x2
    lea r11d,[r8 + -0x2]
    mov dword ptr [rsp + 0x8],ecx
    mov edx,eax
    mov qword ptr [rsp + 0x18],rdx
    .lbl180007016: cmp r11d,0x2
    jle .lbl180007113
    lea edx,[r11 + -0x2]
    mov dword ptr [rsp + 0xc8],ecx
    mov qword ptr [rsp + 0x28],rdx
    .lbl180007030: mov ebx,ebp
    lea r10,[rsp + 0x30]
    mov r12d,ecx
    mov r13d,0x4
    .lbl180007040: mov r9d,r12d
    mov r11d,0x4
    nop
    nop
    .lbl180007050: test si,si
    jz .lbl18000706b
    movsxd rax,r9d
    lea rdx,[rax + rax*0x1]
    movzx r8d,word ptr [rdx + rdi*0x1]
    movzx eax,word ptr [rdx + r14*0x1]
    sub r8d,eax
    jmp .lbl180007077
    .lbl18000706b: movsxd rax,r9d
    lea rdx,[rax + rax*0x1]
    movzx r8d,word ptr [rdx + rdi*0x1]
    .lbl180007077: mov ecx,dword ptr [r10]
    add r9d,0x1
    add r10,0x4
    mov eax,ecx
    imul eax,r8d
    add ebp,eax
    movzx eax,word ptr [rdx + r15*0x1]
    imul eax,ecx
    add ebx,eax
    sub r11,0x1
    jnz .lbl180007050
    mov r8d,dword ptr [rsp + 0xd0]
    add r12d,r8d
    sub r13,0x1
    jnz .lbl180007040
    cmp dword ptr [rsp + 0xf0],0xfff
    jle .lbl1800070bc
    sar ebp,1
    sar ebx,1
    .lbl1800070bc: mov r10,qword ptr [rsp + 0x20]
    mov r9,qword ptr [rsp + 0x10]
    movsxd rcx,ebp
    movsxd rax,ebx
    mov ebp,0x0
    imul rax,rcx
    imul rcx,rcx
    add r10,rcx
    mov ecx,dword ptr [rsp + 0xc8]
    add r9,rax
    add ecx,0x1
    sub qword ptr [rsp + 0x28],0x1
    mov qword ptr [rsp + 0x10],r9
    mov dword ptr [rsp + 0xc8],ecx
    mov qword ptr [rsp + 0x20],r10
    jnz .lbl180007030
    mov ecx,dword ptr [rsp + 0x8]
    mov rdx,qword ptr [rsp + 0x18]
    lea r11d,[r8 + -0x2]
    .lbl180007113: add ecx,r8d
    sub rdx,0x1
    mov qword ptr [rsp + 0x18],rdx
    mov dword ptr [rsp + 0x8],ecx
    jnz .lbl180007016
    test r10,r10
    jle .lbl18000714b
    imul r9,r9,0x64
    mov rax,r9
    cqo
    idiv r10
    mov r13,rax
    test eax,eax
    cmovs r13d,ebp
    mov ebp,dword ptr [rsp]
    mov r12d,ebp
    jmp .lbl180007156
    .lbl18000714b: mov ebp,dword ptr [rsp]
    mov r13,qword ptr [rsp + 0x70]
    mov r12d,ebp
    .lbl180007156: movsxd rbx,dword ptr [rsp + 0xc]
    test rbx,rbx
    jle .lbl1800071ba
    mov r10,r14
    mov r11,r15
    mov rcx,rdi
    sub r10,rdi
    sub r11,rdi
    mov r9,rbx
    .lbl180007172: test si,si
    jz .lbl180007183
    movzx eax,word ptr [r10 + rcx*0x1]
    movzx edx,word ptr [rcx]
    sub edx,eax
    jmp .lbl180007186
    .lbl180007183: movzx edx,word ptr [rcx]
    .lbl180007186: movzx r8d,word ptr [r11 + rcx*0x1]
    imul edx,r13d
    mov eax,0x51eb851f
    imul edx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    sub r8d,edx
    jns .lbl1800071b0
    add r12d,0x1
    cmp r8d,ebp
    cmovl ebp,r8d
    .lbl1800071b0: add rcx,0x2
    sub r9,0x1
    jnz .lbl180007172
    .lbl1800071ba: imul r12d,r12d,0x64
    mov eax,r12d
    cdq
    idiv dword ptr [rsp + 0xc]
    cmp eax,0x2
    mov eax,0x0
    cmovle ebp,eax
    test rbx,rbx
    jle .lbl180007235
    sub r14,rdi
    lea r8d,[rax + 0x1]
    mov r9d,0xffff
    sub r15,rdi
    .lbl1800071e6: test si,si
    jz .lbl1800071f7
    movzx eax,word ptr [rdi + r14*0x1]
    movzx ecx,word ptr [rdi]
    sub ecx,eax
    jmp .lbl1800071fa
    .lbl1800071f7: movzx ecx,word ptr [rdi]
    .lbl1800071fa: imul ecx,r13d
    mov eax,0x51eb851f
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    movzx eax,word ptr [rdi + r15*0x1]
    sub eax,edx
    sub eax,ebp
    cmp eax,r9d
    cmovg eax,r9d
    cmp eax,r8d
    cmovl eax,r8d
    add rdi,0x2
    sub rbx,r8
    mov word ptr [rdi + r15*0x1 + -0x2],ax
    jnz .lbl1800071e6
    .lbl180007235: mov r14,qword ptr [rsp + 0x80]
    mov rsi,qword ptr [rsp + 0x88]
    mov rbx,qword ptr [rsp + 0xc0]
    xor eax,eax
    add rsp,0x90
    pop r15
    pop r13
    pop r12
    pop rdi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_1800078a0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x10],edx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov r12d,dword ptr [rsp + 0xb8]
    mov r15d,dword ptr [rsp + 0xb0]
    mov r14d,r8d
    mov r8d,edx
    mov eax,r14d
    mov rsi,rcx
    cdq
    xor ebp,ebp
    mov rcx,r9
    mov r13,r9
    mov edi,ebp
    idiv r12d
    xor edx,edx
    mov ebx,eax
    mov eax,r8d
    imul eax,r14d
    mov dword ptr [rsp + 0x3c],eax
    mov eax,r15d
    imul eax,r12d
    mov r8d,eax
    mov dword ptr [rsp + 0x38],eax
    call EXTR_memset wrt ..plt
    test r15d,r15d
    mov rcx,rbp
    mov qword ptr [rsp + 0x40],rcx
    jle .lbl180007a7e
    mov eax,dword ptr [rsp + 0xb0]
    mov r15d,ebp
    mov qword ptr [rsp + 0x48],rax
    nop
    nop
    .lbl180007930: test r12d,r12d
    jle .lbl180007a5b
    mov eax,dword ptr [rsp + 0x98]
    movsxd r8,r12d
    mov r10d,r14d
    sub eax,ebx
    sub r10d,ebx
    mov r9d,ebp
    imul r8,rcx
    add eax,0x1
    add r10d,0x1
    mov r11d,r12d
    mov dword ptr [rsp + 0x30],eax
    mov eax,ebx
    mov dword ptr [rsp + 0x34],r10d
    mov ebp,dword ptr [rsp + 0x30]
    imul eax,ebx
    mov dword ptr [rsp + 0xa0],eax
    mov ecx,eax
    add r8,r13
    nop
    nop
    .lbl180007980: cmp r15d,ebp
    mov eax,r10d
    mov edx,ebp
    cmovl edx,r15d
    cmp r9d,r10d
    cmovl eax,r9d
    imul edx,r14d
    add edx,eax
    cmp ebx,0x1
    movsxd rax,edx
    movzx r10d,word ptr [rsi + rax*0x2]
    jle .lbl180007a0c
    lea ecx,[rdx + 0x1]
    add edx,r14d
    movsxd r13,ecx
    lea eax,[rdx + 0x1]
    movsxd r12,eax
    movsxd rax,edx
    movzx ecx,word ptr [rsi + rax*0x2]
    movzx eax,word ptr [rsi + r13*0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + r12*0x2]
    add r10d,eax
    add r10d,ecx
    cmp ebx,0x2
    jle .lbl180007a05
    lea ecx,[rdx + r14*0x1]
    lea eax,[rcx + 0x1]
    movsxd rdx,eax
    movsxd rax,ecx
    movzx ecx,word ptr [rsi + rax*0x2]
    movzx eax,word ptr [rsi + rdx*0x2 + 0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + rdx*0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + r13*0x2 + 0x2]
    add ecx,eax
    movzx eax,word ptr [rsi + r12*0x2 + 0x2]
    add r10d,eax
    add r10d,ecx
    .lbl180007a05: mov ecx,dword ptr [rsp + 0xa0]
    .lbl180007a0c: mov eax,r10d
    cdq
    idiv ecx
    movzx edx,word ptr [rsp + 0xc0]
    cmp eax,edx
    setl al
    test al,al
    mov byte ptr [r8],al
    jnz .lbl180007a29
    add edi,0x1
    .lbl180007a29: mov r10d,dword ptr [rsp + 0x34]
    add r9d,ebx
    add r8,0x1
    sub r11,0x1
    jnz .lbl180007980
    mov r12d,dword ptr [rsp + 0xb8]
    mov rcx,qword ptr [rsp + 0x40]
    mov rax,qword ptr [rsp + 0x48]
    mov r13,qword ptr [rsp + 0xa8]
    xor ebp,ebp
    .lbl180007a5b: add rcx,0x1
    add r15d,ebx
    sub rax,0x1
    mov qword ptr [rsp + 0x48],rax
    mov qword ptr [rsp + 0x40],rcx
    jnz .lbl180007930
    mov r15d,dword ptr [rsp + 0xb0]
    .lbl180007a7e: cmp edi,dword ptr [rsp + 0x38]
    jge .lbl180007ab3
    mov r9d,0xa
    mov r8d,r12d
    mov edx,r15d
    mov rcx,r13
    mov byte ptr [rsp + 0x20],0x0
    call EXTR_FUN_180007730
    mov r9,qword ptr [rsp + 0xc8]
    mov r8d,r15d
    mov edx,r12d
    mov rcx,r13
    call EXTR_FUN_180015da0
    .lbl180007ab3: test edi,edi
    jnz .lbl180007aed
    movsxd rax,dword ptr [rsp + 0x3c]
    test rax,rax
    jle .lbl180007ae2
    movzx ecx,word ptr [rsp + 0xc0]
    nop
    nop
    .lbl180007ad0: cmp word ptr [rsi + rbp*0x2],cx
    jc .lbl180007ad9
    add edi,0x1
    .lbl180007ad9: add rbp,0x1
    cmp rbp,rax
    jl .lbl180007ad0
    .lbl180007ae2: imul ebx,ebx
    xor edx,edx
    mov eax,edi
    div ebx
    jmp .lbl180007aef
    .lbl180007aed: mov eax,edi
    .lbl180007aef: mov rbx,qword ptr [rsp + 0x90]
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180007730:
    push r12
    cmp r9d,edx
    mov r12,rcx
    jge .lbl180007894
    cmp r9d,r8d
    jge .lbl180007894
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x18],rbp
    mov qword ptr [rsp + 0x20],rsi
    xor r10d,r10d
    test r8d,r8d
    movsxd rbp,edx
    movzx edx,byte ptr [rsp + 0x30]
    mov qword ptr [rsp + 0x28],rdi
    movsxd rsi,r9d
    movsxd r11,r8d
    jle .lbl1800077f9
    mov rbx,rbp
    lea rdi,[rsi + -0x1]
    sub rbx,rsi
    nop
    .lbl180007780: xor r8d,r8d
    test rdi,rdi
    mov rcx,rdi
    js .lbl1800077bc
    mov rax,r11
    mov r9,r11
    neg r9
    imul rax,rcx
    add rax,r10
    add rax,r12
    nop
    .lbl1800077a0: cmp byte ptr [rax],dl
    jnz .lbl1800077ac
    mov r8d,0x1
    jmp .lbl1800077b1
    .lbl1800077ac: test r8d,r8d
    jle .lbl1800077b3
    .lbl1800077b1: mov byte ptr [rax],dl
    .lbl1800077b3: add rax,r9
    sub rcx,0x1
    jns .lbl1800077a0
    .lbl1800077bc: xor ecx,ecx
    cmp rbx,rbp
    jge .lbl1800077f0
    mov rax,r11
    mov r8,rbp
    imul rax,rbx
    add rax,r10
    add rax,r12
    sub r8,rbx
    .lbl1800077d6: cmp byte ptr [rax],dl
    jnz .lbl1800077e1
    mov ecx,0x1
    jmp .lbl1800077e5
    .lbl1800077e1: test ecx,ecx
    jle .lbl1800077e7
    .lbl1800077e5: mov byte ptr [rax],dl
    .lbl1800077e7: add rax,r11
    sub r8,0x1
    jnz .lbl1800077d6
    .lbl1800077f0: add r10,0x1
    cmp r10,r11
    jl .lbl180007780
    .lbl1800077f9: xor r10d,r10d
    test rbp,rbp
    jle .lbl180007880
    mov rbx,r11
    lea rdi,[rsi + -0x1]
    sub rbx,rsi
    nop
    nop
    .lbl180007810: xor r8d,r8d
    test rdi,rdi
    mov rcx,rdi
    js .lbl180007842
    mov r9,r11
    imul r9,r10
    add r9,r12
    .lbl180007825: cmp byte ptr [r9 + rcx*0x1],dl
    jnz .lbl180007833
    mov r8d,0x1
    jmp .lbl180007838
    .lbl180007833: test r8d,r8d
    jle .lbl18000783c
    .lbl180007838: mov byte ptr [r9 + rcx*0x1],dl
    .lbl18000783c: sub rcx,0x1
    jns .lbl180007825
    .lbl180007842: xor r8d,r8d
    cmp rbx,r11
    mov rcx,rbx
    jge .lbl180007877
    mov r9,r11
    imul r9,r10
    add r9,r12
    .lbl180007857: cmp byte ptr [r9 + rcx*0x1],dl
    jnz .lbl180007865
    mov r8d,0x1
    jmp .lbl18000786a
    .lbl180007865: test r8d,r8d
    jle .lbl18000786e
    .lbl18000786a: mov byte ptr [r9 + rcx*0x1],dl
    .lbl18000786e: add rcx,0x1
    cmp rcx,r11
    jl .lbl180007857
    .lbl180007877: add r10,0x1
    cmp r10,rbp
    jl .lbl180007810
    .lbl180007880: mov rsi,qword ptr [rsp + 0x20]
    mov rbp,qword ptr [rsp + 0x18]
    mov rbx,qword ptr [rsp + 0x10]
    mov rdi,qword ptr [rsp + 0x28]
    .lbl180007894: pop r12
    ret

section .data


section .text
EXTR_FUN_1800085d0:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    push rbx
    push rbp
    push rsi
    push rdi
    push r13
    push r15
    sub rsp,0x28
    xor ebp,ebp
    mov rsi,r9
    movsxd rdi,edx
    mov r9d,edi
    mov r13,rcx
    mov r11d,ebp
    mov ebx,ebp
    mov r10,rbp
    mov ecx,ebp
    imul r9d,r8d
    movsxd r8,r9d
    cmp r8,0x2
    jl .lbl18000862d
    lea rdx,[r8 + -0x1]
    .lbl180008613: movzx eax,word ptr [r13 + r10*0x2]
    add r10,0x2
    add r11d,eax
    movzx eax,word ptr [r13 + r10*0x2 + -0x2]
    add ecx,eax
    cmp r10,rdx
    jl .lbl180008613
    .lbl18000862d: cmp r10,r8
    jge .lbl180008638
    movzx ebx,word ptr [r13 + r10*0x2]
    .lbl180008638: lea eax,[r11 + rcx*0x1]
    movsxd rcx,dword ptr [rsp + 0x70]
    mov qword ptr [rsp + 0x60],r12
    add eax,ebx
    mov qword ptr [rsp + 0x20],r14
    mov r15,rdi
    cdq
    mov r11,rcx
    mov qword ptr [rsp + 0x8],rdi
    idiv r9d
    test edi,edi
    mov dword ptr [rsp],eax
    jle .lbl180008726
    lea r14,[rcx + rcx*0x1]
    lea r15,[rcx*0x4]
    mov rbx,r13
    mov r12,rdi
    nop
    nop
    .lbl180008680: xor r8d,r8d
    xor r9d,r9d
    xor edi,edi
    xor r10d,r10d
    cmp r11,0x2
    jl .lbl1800086cb
    lea rdx,[r11 + -0x2]
    mov rcx,rbx
    shr rdx,1
    add rdx,0x1
    lea r10,[rdx + rdx*0x1]
    nop
    nop
    nop
    nop
    .lbl1800086b0: movzx eax,word ptr [rcx]
    add rcx,0x4
    add r8d,eax
    movzx eax,word ptr [rcx + -0x2]
    add r9d,eax
    sub rdx,0x1
    jnz .lbl1800086b0
    mov ecx,dword ptr [rsp + 0x70]
    .lbl1800086cb: cmp r10,r11
    jge .lbl1800086da
    lea rax,[r10 + rbp*0x1]
    movzx edi,word ptr [r13 + rax*0x2]
    .lbl1800086da: lea eax,[r9 + r8*0x1]
    add eax,edi
    cdq
    idiv ecx
    test r11,r11
    jle .lbl18000870c
    mov rdx,rsi
    mov r8,rbx
    mov r9,r11
    .lbl1800086f1: movzx ecx,word ptr [r8]
    add rdx,0x4
    add r8,0x2
    sub ecx,eax
    sub r9,0x1
    mov dword ptr [rdx + -0x4],ecx
    jnz .lbl1800086f1
    mov ecx,dword ptr [rsp + 0x70]
    .lbl18000870c: add rbp,r11
    add rsi,r15
    add rbx,r14
    sub r12,0x1
    jnz .lbl180008680
    mov r15,qword ptr [rsp + 0x8]
    xor ebp,ebp
    .lbl180008726: test r11,r11
    mov rdi,rbp
    jle .lbl180008818
    mov rcx,qword ptr [rsp + 0x78]
    lea r14,[r11*0x4]
    mov dword ptr [rsp + 0x70],0xffff
    lea r12,[r14 + rcx*0x1]
    mov rsi,rcx
    nop
    .lbl180008750: xor ebx,ebx
    cmp r15,0x2
    mov r8d,ebp
    mov r9d,ebp
    jl .lbl180008797
    lea rdx,[r15 + -0x2]
    lea r10,[r11*0x8]
    mov rax,rsi
    shr rdx,1
    mov rcx,r12
    add rdx,0x1
    lea rbx,[rdx + rdx*0x1]
    nop
    nop
    .lbl180008780: add r8d,dword ptr [rax]
    add r9d,dword ptr [rcx]
    add rax,r10
    add rcx,r10
    sub rdx,0x1
    jnz .lbl180008780
    mov rcx,qword ptr [rsp + 0x78]
    .lbl180008797: cmp rbx,r15
    jge .lbl1800087a9
    mov rax,r11
    imul rax,rbx
    add rax,rdi
    mov ebp,dword ptr [rcx + rax*0x4]
    .lbl1800087a9: lea eax,[r9 + r8*0x1]
    add eax,ebp
    cdq
    idiv dword ptr [rsp + 0x68]
    test r15,r15
    jle .lbl1800087f8
    mov ebx,dword ptr [rsp]
    lea r10,[r11 + r11*0x1]
    mov rdx,r13
    mov r8,rsi
    mov r9,r15
    xor ebp,ebp
    nop
    nop
    .lbl1800087d0: mov ecx,dword ptr [r8]
    sub ecx,eax
    add ecx,ebx
    cmp ecx,0xffff
    cmovg ecx,dword ptr [rsp + 0x70]
    test ecx,ecx
    cmovs ecx,ebp
    add r8,r14
    mov word ptr [rdx],cx
    add rdx,r10
    sub r9,0x1
    jnz .lbl1800087d0
    jmp .lbl1800087fa
    .lbl1800087f8: xor ebp,ebp
    .lbl1800087fa: mov rcx,qword ptr [rsp + 0x78]
    add rdi,0x1
    add r13,0x2
    add r12,0x4
    add rsi,0x4
    cmp rdi,r11
    jl .lbl180008750
    .lbl180008818: mov r14,qword ptr [rsp + 0x20]
    mov r12,qword ptr [rsp + 0x60]
    add rsp,0x28
    pop r15
    pop r13
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_18000a260:
    mov dword ptr [rsp + 0x20],r9d
    push r12
    push r13
    push r14
    sub rsp,0x80
    mov eax,edx
    mov r13,rcx
    movsxd r12,r8d
    mov r14d,edx
    mov rdx,qword ptr [rsp + 0xd0]
    imul eax,r12d
    mov dword ptr [rsp + 0xb0],eax
    cdqe
    lea rcx,[rdx + rax*0x2]
    lea eax,[r9 + r9*0x1 + 0x1]
    mov qword ptr [rsp + 0xa0],rcx
    imul eax,eax
    test r13,r13
    mov dword ptr [rsp + 0xa8],eax
    jnz .lbl18000a2c4
    lea eax,[r13 + 0x1]
    add rsp,0x80
    pop r14
    pop r13
    pop r12
    ret
    .lbl18000a2c4: mov qword ptr [rsp + 0x78],rbx
    xor ebx,ebx
    test r14d,r14d
    jle .lbl18000a3c1
    mov qword ptr [rsp + 0x70],rbp
    mov qword ptr [rsp + 0x68],rsi
    mov rax,r12
    add rax,rax
    mov qword ptr [rsp + 0x60],rdi
    mov qword ptr [rsp + 0x58],r15
    mov r15,r13
    mov qword ptr [rsp + 0x40],r13
    mov qword ptr [rsp + 0x48],rax
    nop
    nop
    .lbl18000a300: xor esi,esi
    test r12d,r12d
    jle .lbl18000a399
    mov rdi,rdx
    mov rbp,r15
    mov r15d,dword ptr [rsp + 0xa8]
    sub rdi,r13
    nop
    .lbl18000a320: mov dword ptr [rsp + 0x30],r9d
    mov r9,rcx
    mov r8d,r14d
    mov rcx,r13
    mov edx,r12d
    mov dword ptr [rsp + 0x28],esi
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180007460
    mov eax,dword ptr [rsp + 0xc8]
    mov r8,qword ptr [rsp + 0xa0]
    mov dword ptr [rsp + 0x28],eax
    mov eax,dword ptr [rsp + 0xc0]
    lea rdx,[rdi + rbp*0x1]
    mov r9d,r15d
    mov rcx,rbp
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_1800076e0
    mov rcx,qword ptr [rsp + 0xa0]
    mov r9d,dword ptr [rsp + 0xb8]
    add esi,0x1
    add rbp,0x2
    cmp esi,r12d
    jl .lbl18000a320
    mov r15,qword ptr [rsp + 0x40]
    mov rax,qword ptr [rsp + 0x48]
    mov rdx,qword ptr [rsp + 0xd0]
    .lbl18000a399: add r15,rax
    add ebx,0x1
    cmp ebx,r14d
    mov qword ptr [rsp + 0x40],r15
    jl .lbl18000a300
    mov r15,qword ptr [rsp + 0x58]
    mov rdi,qword ptr [rsp + 0x60]
    mov rsi,qword ptr [rsp + 0x68]
    mov rbp,qword ptr [rsp + 0x70]
    .lbl18000a3c1: mov r8d,dword ptr [rsp + 0xb0]
    mov rcx,r13
    add r8d,r8d
    call EXTR_memcpy wrt ..plt
    mov rbx,qword ptr [rsp + 0x78]
    xor eax,eax
    add rsp,0x80
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_1800076e0:
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x20
    mov rdi,rdx
    mov rbx,rcx
    mov edx,r9d
    mov rcx,r8
    call EXTR_FUN_180007570
    movzx edx,word ptr [rbx]
    mov ecx,eax
    sub ecx,edx
    cmp ecx,dword ptr [rsp + 0x50]
    jg .lbl180007720
    mov ecx,edx
    sub ecx,eax
    cmp ecx,dword ptr [rsp + 0x58]
    jg .lbl180007720
    mov word ptr [rdi],dx
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret
    .lbl180007720: mov word ptr [rdi],ax
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_1800082c0:
    push rbx
    push rsi
    push r12
    push r15
    sub rsp,0x58
    mov eax,r9d
    mov esi,edx
    mov r15d,r8d
    cdq
    mov r12,rcx
    sub eax,edx
    sar eax,1
    mov ebx,eax
    mov dword ptr [rsp + 0x44],eax
    jnz .lbl1800082f1
    xor ax,ax
    add rsp,0x58
    pop r15
    pop r12
    pop rsi
    pop rbx
    ret
    .lbl1800082f1: sub esi,eax
    xor edx,edx
    mov ecx,r8d
    sub ecx,eax
    mov qword ptr [rsp + 0x88],rdi
    mov dword ptr [rsp + 0x30],esi
    mov eax,ecx
    mov dword ptr [rsp + 0x2c],ecx
    div ebx
    xor edx,edx
    mov r11d,eax
    mov eax,esi
    div ebx
    imul r11d,eax
    test r11d,r11d
    mov dword ptr [rsp + 0x24],r11d
    jz .lbl180008348
    mov rdi,qword ptr [rsp + 0xa0]
    mov r8d,r11d
    xor edx,edx
    mov rcx,rdi
    add r8,r8
    call EXTR_memset wrt ..plt
    mov ecx,dword ptr [rsp + 0x2c]
    mov r11d,dword ptr [rsp + 0x24]
    jmp .lbl180008350
    .lbl180008348: mov rdi,qword ptr [rsp + 0xa0]
    .lbl180008350: xor r9d,r9d
    lea r10d,[rbx + rbx*0x1]
    mov dword ptr [rsp + 0x34],ebx
    lea eax,[r10 + 0x1]
    mov dword ptr [rsp + 0x28],r9d
    mov qword ptr [rsp + 0x48],r9
    imul eax,eax
    cmp ebx,esi
    mov dword ptr [rsp + 0x40],eax
    mov eax,ebx
    jnc .lbl1800085ac
    mov qword ptr [rsp + 0x80],rbp
    mov edx,ebx
    mov r8d,r9d
    mov qword ptr [rsp + 0x90],r13
    mov dword ptr [rsp + 0x20],r9d
    mov esi,r9d
    imul edx,r15d
    mov dword ptr [rsp + 0x38],edx
    mov edx,dword ptr [rsp + 0x30]
    mov dword ptr [rsp + 0x98],r9d
    mov r9d,dword ptr [rsp + 0x38]
    mov qword ptr [rsp + 0x50],r14
    .lbl1800083b5: cmp ebx,ecx
    jnc .lbl180008577
    lea ecx,[rax + rbx*0x1]
    mov r14d,r10d
    xor r13d,r13d
    mov dword ptr [rsp + 0x3c],ecx
    nop
    nop
    .lbl1800083d0: xor ebx,ebx
    cmp r8d,ecx
    ja .lbl18000846b
    mov edi,esi
    mov ebp,r14d
    mov esi,ecx
    sub ebp,r13d
    sub esi,r8d
    add ebp,0x1
    add esi,0x1
    nop
    .lbl1800083f0: xor r9d,r9d
    xor r10d,r10d
    cmp ebp,0x2
    mov r11d,r13d
    jl .lbl18000843c
    mov eax,r14d
    lea edx,[r13 + rdi*0x1 + 0x1]
    sub eax,r13d
    sub eax,0x1
    shr eax,1
    add eax,0x1
    mov r8d,eax
    lea r11d,[r13 + rax*0x2]
    nop
    nop
    .lbl180008420: movzx ecx,word ptr [r12 + rdx*0x2]
    lea eax,[rdx + -0x1]
    add edx,0x2
    movzx eax,word ptr [r12 + rax*0x2]
    add r10d,ecx
    add r9d,eax
    sub r8,0x1
    jnz .lbl180008420
    .lbl18000843c: cmp r11d,r14d
    ja .lbl18000844c
    lea ecx,[rdi + r11*0x1]
    movzx ecx,word ptr [r12 + rcx*0x2]
    add ebx,ecx
    .lbl18000844c: lea ecx,[r10 + r9*0x1]
    add edi,r15d
    add ebx,ecx
    sub rsi,0x1
    jnz .lbl1800083f0
    mov r8d,dword ptr [rsp + 0x20]
    mov ecx,dword ptr [rsp + 0x3c]
    mov esi,dword ptr [rsp + 0x98]
    .lbl18000846b: mov eax,ebx
    xor r11d,r11d
    xor r9d,r9d
    cdq
    xor ebx,ebx
    xor r10d,r10d
    idiv dword ptr [rsp + 0x40]
    cmp r8d,ecx
    ja .lbl1800084eb
    sub ecx,r8d
    lea ebp,[rcx + 0x1]
    .lbl180008488: cmp r13d,r14d
    ja .lbl1800084c1
    mov r8d,r14d
    lea edx,[r13 + rsi*0x1]
    sub r8d,r13d
    add r8d,0x1
    nop
    .lbl1800084a0: movzx ecx,word ptr [r12 + rdx*0x2]
    cmp ecx,eax
    jle .lbl1800084b2
    add r9d,ecx
    add r11d,0x1
    jmp .lbl1800084b8
    .lbl1800084b2: add r10d,ecx
    add ebx,0x1
    .lbl1800084b8: add edx,0x1
    sub r8,0x1
    jnz .lbl1800084a0
    .lbl1800084c1: add esi,r15d
    sub rbp,0x1
    jnz .lbl180008488
    test r11d,r11d
    jle .lbl1800084d9
    mov eax,r9d
    cdq
    idiv r11d
    mov r9d,eax
    .lbl1800084d9: test ebx,ebx
    jle .lbl1800084e6
    mov eax,r10d
    cdq
    idiv ebx
    mov r10d,eax
    .lbl1800084e6: cmp r9d,r10d
    jl .lbl180008511
    .lbl1800084eb: mov edx,dword ptr [rsp + 0x28]
    mov r11d,dword ptr [rsp + 0x24]
    cmp edx,r11d
    jnc .lbl180008524
    mov rax,qword ptr [rsp + 0x48]
    mov rdi,qword ptr [rsp + 0xa0]
    sub r9w,r10w
    mov word ptr [rdi + rax*0x2],r9w
    jmp .lbl18000852c
    .lbl180008511: mov edx,dword ptr [rsp + 0x28]
    mov r11d,dword ptr [rsp + 0x24]
    mov rdi,qword ptr [rsp + 0xa0]
    jmp .lbl18000852c
    .lbl180008524: mov rdi,qword ptr [rsp + 0xa0]
    .lbl18000852c: mov ebx,dword ptr [rsp + 0x44]
    add qword ptr [rsp + 0x48],0x1
    mov r8d,dword ptr [rsp + 0x20]
    mov ecx,dword ptr [rsp + 0x3c]
    mov esi,dword ptr [rsp + 0x98]
    add r13d,ebx
    add edx,0x1
    lea eax,[r13 + rbx*0x1]
    add r14d,ebx
    cmp eax,dword ptr [rsp + 0x2c]
    mov dword ptr [rsp + 0x28],edx
    jc .lbl1800083d0
    mov ecx,dword ptr [rsp + 0x2c]
    mov eax,dword ptr [rsp + 0x34]
    mov edx,dword ptr [rsp + 0x30]
    mov r9d,dword ptr [rsp + 0x38]
    lea r10d,[rbx + rbx*0x1]
    .lbl180008577: add eax,ebx
    add esi,r9d
    add r8d,ebx
    cmp eax,edx
    mov dword ptr [rsp + 0x34],eax
    mov dword ptr [rsp + 0x98],esi
    mov dword ptr [rsp + 0x20],r8d
    jc .lbl1800083b5
    mov r14,qword ptr [rsp + 0x50]
    mov r13,qword ptr [rsp + 0x90]
    mov rbp,qword ptr [rsp + 0x80]
    .lbl1800085ac: mov edx,r11d
    mov rcx,rdi
    mov rdi,qword ptr [rsp + 0x88]
    add rsp,0x58
    pop r15
    pop r12
    pop rsi
    pop rbx
    jmp EXTR_FUN_180002790

section .data


section .text
EXTR_FUN_180002790:
    mov qword ptr [rsp + 0x18],rbp
    push rsi
    sub rsp,0x20
    mov eax,edx
    mov esi,edx
    mov rbp,rcx
    cdq
    and eax,0x1
    xor eax,edx
    cmp eax,edx
    jz .lbl1800027cc
    mov eax,esi
    lea r8d,[rsi + -0x1]
    cdq
    sub eax,edx
    xor edx,edx
    sar eax,1
    mov r9d,eax
    call EXTR_FUN_180002710
    cwde
    mov rbp,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rsi
    ret
    .lbl1800027cc: mov eax,esi
    lea r8d,[rsi + -0x1]
    mov qword ptr [rsp + 0x30],rbx
    cdq
    mov qword ptr [rsp + 0x38],rdi
    sub eax,edx
    xor edx,edx
    sar eax,1
    mov edi,eax
    lea r9d,[rax + -0x1]
    call EXTR_FUN_180002710
    lea r8d,[rsi + -0x1]
    mov r9d,edi
    xor edx,edx
    mov rcx,rbp
    movsx ebx,ax
    call EXTR_FUN_180002710
    mov rdi,qword ptr [rsp + 0x38]
    mov rbp,qword ptr [rsp + 0x40]
    cwde
    add eax,ebx
    mov rbx,qword ptr [rsp + 0x30]
    cdq
    sub eax,edx
    sar eax,1
    cwde
    add rsp,0x20
    pop rsi
    ret

section .data


section .text
EXTR_FUN_180002710:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    cmp edx,r8d
    movsxd rsi,r9d
    mov edi,r8d
    mov ebx,edx
    mov rbp,rcx
    jz .lbl180002758
    cmp edx,r8d
    .lbl180002737: jge .lbl18000277f
    mov r8d,edi
    mov edx,ebx
    mov rcx,rbp
    call EXTR_FUN_180002600
    cmp esi,eax
    jz .lbl180002775
    jge .lbl180002751
    lea edi,[rax + -0x1]
    jmp .lbl180002754
    .lbl180002751: lea ebx,[rax + 0x1]
    .lbl180002754: cmp ebx,edi
    jnz .lbl180002737
    .lbl180002758: movsxd rax,ebx
    mov ax,word ptr [rbp + rax*0x2]
    .lbl180002760: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rdi
    ret
    .lbl180002775: mov rax,rsi
    mov ax,word ptr [rbp + rsi*0x2]
    jmp .lbl180002760
    .lbl18000277f: mov ax,0x8ad1
    jmp .lbl180002760

section .data


section .text
EXTR_FUN_180002600:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rdi
    movsxd r9,edx
    lea r11d,[r8 + 0x1]
    mov r10,rcx
    movzx ebx,word ptr [rcx + r9*0x2]
    lea rdi,[rcx + r9*0x2]
    movsxd rdx,r11d
    movsxd r8,r8d
    .lbl180002623: add r9,0x1
    cmp word ptr [r10 + r9*0x2],bx
    jg .lbl180002640
    cmp r9,r8
    jle .lbl180002623
    nop
    nop
    nop
    nop
    .lbl180002640: sub rdx,0x1
    sub r11d,0x1
    cmp word ptr [r10 + rdx*0x2],bx
    jg .lbl180002640
    cmp r9,rdx
    jge .lbl18000266a
    movzx eax,word ptr [r10 + rdx*0x2]
    movzx ecx,word ptr [r10 + r9*0x2]
    mov word ptr [r10 + r9*0x2],ax
    mov word ptr [r10 + rdx*0x2],cx
    jmp .lbl180002623
    .lbl18000266a: movzx r8d,word ptr [rdi]
    mov rbx,qword ptr [rsp + 0x8]
    movsxd rdx,r11d
    movzx ecx,word ptr [r10 + rdx*0x2]
    mov eax,r11d
    mov word ptr [rdi],cx
    mov rdi,qword ptr [rsp + 0x10]
    mov word ptr [r10 + rdx*0x2],r8w
    ret

section .data


section .text
EXTR_FUN_18000b2c0:
    mov qword ptr [rsp + 0x18],rbx
    mov qword ptr [rsp + 0x20],r9
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x70
    mov rax,qword ptr [rcx + 0x108]
    movzx ebp,word ptr [rcx + 0x86]
    movzx esi,word ptr [rcx + 0x88]
    mov r15,rcx
    mov qword ptr [rsp + 0x60],rax
    mov rax,qword ptr [rcx + 0x100]
    mov ecx,esi
    mov qword ptr [rsp + 0x50],rax
    movzx eax,word ptr [rdx]
    imul ecx,ebp
    mov dword ptr [rsp + 0xb8],ecx
    movsxd r14,ecx
    mov rcx,qword ptr [rsp + 0xd0]
    xor r12d,r12d
    test ebp,ebp
    lea rcx,[rcx + r14*0x2]
    mov rdi,r8
    mov r13,rdx
    mov qword ptr [rsp + 0x58],rcx
    mov word ptr [rsp + 0xb0],ax
    jz .lbl18000b695
    test esi,esi
    jz .lbl18000b695
    cmp ebp,0xffff
    jg .lbl18000b695
    cmp esi,0xffff
    jg .lbl18000b695
    test byte ptr [r15 + 0x7c],0x8
    jz .lbl18000b3a7
    movzx eax,word ptr [r15 + 0xa2]
    movzx r9d,word ptr [r15 + 0xa0]
    mov qword ptr [rsp + 0x30],rcx
    mov dword ptr [rsp + 0x28],eax
    mov r8d,ebp
    mov edx,esi
    mov rcx,rdi
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_18000a260
    test eax,eax
    mov r12d,eax
    jnz .lbl18000b697
    mov rcx,qword ptr [rsp + 0x58]
    mov ax,word ptr [rsp + 0xb0]
    .lbl18000b3a7: test byte ptr [r15 + 0x7c],0x10
    jz .lbl18000b3c3
    cmp ax,0x1
    jnz .lbl18000b3c3
    mov r8,rcx
    lea rcx,[r15 + 0x70]
    mov rdx,rdi
    call EXTR_FUN_180008d20
    .lbl18000b3c3: test byte ptr [r15 + 0x7c],0x20
    jz .lbl18000b443
    movzx eax,word ptr [r15 + 0x8a]
    add eax,-0x9
    cmp eax,0x1
    ja .lbl18000b3ee
    xor eax,eax
    test r14,r14
    jle .lbl18000b3ee
    .lbl18000b3e1: shr word ptr [rdi + rax*0x2],1
    add rax,0x1
    cmp rax,r14
    jl .lbl18000b3e1
    .lbl18000b3ee: mov r14,qword ptr [rsp + 0x58]
    lea rax,[r15 + 0xb4]
    lea rcx,[r13 + 0x28]
    mov qword ptr [rsp + 0x48],r14
    mov word ptr [rsp + 0x40],0x7fff
    mov dword ptr [rsp + 0x38],0x1a
    mov qword ptr [rsp + 0x30],rax
    mov dword ptr [rsp + 0x28],0x20
    mov qword ptr [rsp + 0x20],rcx
    mov rcx,rdi
    mov r9d,0x1
    mov r8d,esi
    mov edx,ebp
    call EXTR_FUN_18000a3f0
    test eax,eax
    mov r12d,eax
    jz .lbl18000b448
    jmp .lbl18000b697
    .lbl18000b443: mov r14,qword ptr [rsp + 0x58]
    .lbl18000b448: test byte ptr [r15 + 0x7c],0x40
    jz .lbl18000b518
    mov r9,qword ptr [rsp + 0xc8]
    test r9,r9
    jnz .lbl18000b46c
    mov r8d,ebp
    mov edx,esi
    xor ecx,ecx
    call EXTR_FUN_18000a7c0
    .lbl18000b46c: mov r8d,dword ptr [rsp + 0xb8]
    mov rcx,qword ptr [rsp + 0xd0]
    mov rdx,r9
    add r8d,r8d
    call EXTR_memcpy wrt ..plt
    cmp word ptr [r15 + 0x8a],0x9
    mov r9,qword ptr [rsp + 0x50]
    mov eax,0x3c
    mov ecx,0x5a
    cmovnz ax,cx
    cmp word ptr [rsp + 0xb0],0x0
    jnz .lbl18000b4da
    cmp word ptr [r13 + 0x2],ax
    jbe .lbl18000b4da
    mov rdx,qword ptr [rsp + 0xc8]
    mov rcx,qword ptr [rsp + 0xd0]
    mov r8,rdi
    mov dword ptr [rsp + 0x28],ebp
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_180009ca0
    mov byte ptr [r13 + 0x6],0x1
    .lbl18000b4da: cmp word ptr [rsp + 0xb0],0x0
    mov r8,qword ptr [rsp + 0x60]
    mov rdx,qword ptr [rsp + 0xd0]
    cmovz r8,r9
    cmp word ptr [r15 + 0x8a],0x9
    mov qword ptr [rsp + 0x30],r14
    setz al
    mov r9d,esi
    mov rcx,rdi
    mov byte ptr [rsp + 0x28],al
    mov dword ptr [rsp + 0x20],ebp
    call EXTR_FUN_180009d20
    .lbl18000b518: test byte ptr [r15 + 0x7c],0x80
    jz .lbl18000b562
    mov qword ptr [rsp + 0x40],r14
    mov dword ptr [rsp + 0x38],0x0
    mov dword ptr [rsp + 0x30],0x64
    mov r9d,0x2
    mov r8d,esi
    mov edx,ebp
    mov rcx,rdi
    mov dword ptr [rsp + 0x28],0xffff
    mov dword ptr [rsp + 0x20],0x62
    call EXTR_FUN_180008b30
    test eax,eax
    mov r12d,eax
    jnz .lbl18000b697
    .lbl18000b562: bt dword ptr [r15 + 0x7c],0x8
    jnc .lbl18000b585
    or r9w,0xffff
    mov r8d,esi
    mov edx,ebp
    mov rcx,rdi
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_180009a60
    mov r12d,eax
    .lbl18000b585: cmp word ptr [rsp + 0xb0],0x0
    jnz .lbl18000b5b0
    test byte ptr [r15 + 0x7c],0x2
    jz .lbl18000b5dd
    mov r9,qword ptr [rsp + 0x50]
    mov r8d,esi
    mov edx,ebp
    mov rcx,rdi
    call EXTR_FUN_180008cf0
    mov byte ptr [r13 + 0x4],0x1
    jmp .lbl18000b5dd
    .lbl18000b5b0: test byte ptr [r15 + 0x7c],0x4
    jz .lbl18000b5dd
    movzx eax,word ptr [r13 + 0x10]
    shr ax,1
    cmp word ptr [r13 + 0x12],ax
    jbe .lbl18000b5dd
    mov r9,qword ptr [rsp + 0x60]
    mov r8d,esi
    mov edx,ebp
    mov rcx,rdi
    call EXTR_FUN_180008cf0
    mov byte ptr [r13 + 0x5],0x1
    .lbl18000b5dd: mov r9d,0xffff
    mov r8d,ebp
    mov edx,esi
    mov rcx,rdi
    call EXTR_FUN_18000a060
    mov rcx,qword ptr [r15 + 0x118]
    test rcx,rcx
    jz .lbl18000b617
    mov r8,qword ptr [r15 + 0x100]
    mov r9d,esi
    mov rdx,rdi
    mov dword ptr [rsp + 0x20],ebp
    call EXTR_FUN_18000a4f0
    mov word ptr [r13 + 0x22],ax
    .lbl18000b617: mov rdx,qword ptr [r15 + 0x120]
    test rdx,rdx
    jz .lbl18000b690
    mov r9d,ebp
    mov r8d,esi
    mov rcx,r14
    call EXTR_FUN_18000a610
    mov r8,qword ptr [r15 + 0x100]
    mov r9d,esi
    mov rdx,rdi
    mov rcx,r14
    mov dword ptr [rsp + 0x20],ebp
    call EXTR_FUN_18000a4f0
    mov rdx,qword ptr [r15 + 0x120]
    mov r9d,ebp
    mov r8d,esi
    mov rcx,r14
    movsx ebx,ax
    call EXTR_FUN_18000a6e0
    mov r8,qword ptr [r15 + 0x100]
    mov r9d,esi
    mov rdx,rdi
    mov rcx,r14
    mov dword ptr [rsp + 0x20],ebp
    call EXTR_FUN_18000a4f0
    movsx ecx,ax
    imul ebx,ebx
    imul ecx,ecx
    add ecx,ebx
    call EXTR_FUN_180002420
    mov word ptr [r13 + 0x24],ax
    .lbl18000b690: mov eax,r12d
    jmp .lbl18000b697
    .lbl18000b695: xor eax,eax
    .lbl18000b697: mov rbx,qword ptr [rsp + 0xc0]
    add rsp,0x70
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180008d20:
    mov qword ptr [rsp + 0x18],r8
    mov qword ptr [rsp + 0x10],rdx
    push rbp
    push rsi
    push r12
    push r13
    push r14
    sub rsp,0x60
    movzx ebp,word ptr [rcx + 0x18]
    movzx r14d,word ptr [rcx + 0x16]
    mov r13,rcx
    mov eax,r14d
    lea rcx,[r8 + r14*0x4]
    xor r12d,r12d
    xor esi,esi
    mov qword ptr [rsp + 0x30],rcx
    imul eax,ebp
    test rdx,rdx
    mov dword ptr [rsp + 0x90],eax
    movsxd rax,ebp
    lea r9,[rcx + rax*0x4]
    mov qword ptr [rsp + 0x28],rax
    mov qword ptr [rsp + 0x38],r9
    jz .lbl1800090a7
    test r9,r9
    jz .lbl1800090a7
    test r8,r8
    jz .lbl1800090a7
    test rcx,rcx
    jz .lbl1800090a7
    mov qword ptr [rsp + 0x58],rbx
    mov qword ptr [rsp + 0x50],rdi
    xor edi,edi
    xor ebx,ebx
    test r14d,r14d
    mov qword ptr [rsp + 0x48],r15
    movsxd r15,r14d
    jle .lbl180009098
    mov r10,rdx
    mov r11,r15
    mov rdx,r8
    nop
    nop
    .lbl180008dc0: test rax,rax
    mov dword ptr [rdx],esi
    jle .lbl180008dee
    lea r9,[r15 + r15*0x1]
    mov rcx,r10
    mov r8,rax
    .lbl180008dd1: movzx eax,word ptr [rcx]
    add dword ptr [rdx],eax
    movzx eax,word ptr [rcx]
    add ebx,eax
    cmp eax,edi
    jbe .lbl180008de2
    movzx edi,word ptr [rcx]
    .lbl180008de2: add rcx,r9
    sub r8,0x1
    jnz .lbl180008dd1
    mov rax,rbp
    .lbl180008dee: add rdx,0x4
    add r10,0x2
    sub r11,0x1
    jnz .lbl180008dc0
    test edi,edi
    jz .lbl180009098
    mov edx,dword ptr [rsp + 0x90]
    lea r9,[rsp + 0xa8]
    lea r8d,[r11 + 0x64]
    mov ecx,ebx
    call EXTR_FUN_180008ae0
    mov r9,qword ptr [rsp + 0xa0]
    xor edx,edx
    mov r8d,eax
    mov dword ptr [rsp + 0x20],eax
    imul ebx,eax
    mov eax,ebx
    div dword ptr [rsp + 0x90]
    xor ecx,ecx
    mov dword ptr [rsp + 0x24],eax
    .lbl180008e41: mov eax,dword ptr [r9 + rcx*0x4]
    xor edx,edx
    imul eax,r8d
    div ebp
    cmp eax,r12d
    mov dword ptr [r9 + rcx*0x4],eax
    cmova r12d,eax
    add rcx,0x1
    cmp rcx,r15
    jl .lbl180008e41
    mov rbx,qword ptr [rsp + 0x28]
    test rbx,rbx
    jle .lbl180008ec3
    mov rbp,qword ptr [rsp + 0x30]
    mov r9,qword ptr [rsp + 0x98]
    lea r11,[r15 + r15*0x1]
    mov rdx,rbp
    mov r10,rbx
    nop
    nop
    nop
    nop
    .lbl180008e90: mov dword ptr [rdx],esi
    mov rcx,r9
    mov r8,r15
    nop
    nop
    .lbl180008ea0: movzx eax,word ptr [rcx]
    add rcx,0x2
    add dword ptr [rdx],eax
    sub r8,0x1
    jnz .lbl180008ea0
    add rdx,0x4
    add r9,r11
    sub r10,0x1
    jnz .lbl180008e90
    mov r8d,dword ptr [rsp + 0x20]
    jmp .lbl180008ec8
    .lbl180008ec3: mov rbp,qword ptr [rsp + 0x30]
    .lbl180008ec8: xor ecx,ecx
    test rbx,rbx
    jle .lbl180008eef
    nop
    .lbl180008ed0: xor edx,edx
    mov eax,r8d
    imul eax,dword ptr [rbp + rcx*0x4]
    div r14d
    cmp eax,esi
    mov dword ptr [rbp + rcx*0x4],eax
    cmova esi,eax
    add rcx,0x1
    cmp rcx,rbx
    jl .lbl180008ed0
    .lbl180008eef: test r12d,r12d
    jz .lbl180009098
    lea r9,[rsp + 0xa8]
    mov r8d,0x2710
    mov edx,r12d
    mov ecx,edi
    call EXTR_FUN_180008ae0
    xor edx,edx
    lea r9,[rsp + 0xa8]
    mov ebp,eax
    imul edi,eax
    mov eax,edi
    div r12d
    mov edx,esi
    mov ecx,eax
    call EXTR_FUN_180008ae0
    xor ebx,ebx
    xor r11d,r11d
    cmp qword ptr [rsp + 0x28],rbx
    mov edi,eax
    jle .lbl18000900d
    mov rsi,qword ptr [rsp + 0x98]
    mov r12,qword ptr [rsp + 0x38]
    mov qword ptr [rsp + 0xa8],rsi
    nop
    nop
    nop
    .lbl180008f60: mov r14,qword ptr [rsp + 0x30]
    mov r10,rsi
    mov rsi,qword ptr [rsp + 0xa0]
    xor r9d,r9d
    mov rcx,r12
    .lbl180008f76: movzx eax,word ptr [r10]
    test ax,ax
    jz .lbl180008fc2
    mov r8d,dword ptr [rsi + r9*0x4]
    test r8d,r8d
    jnz .lbl180008f92
    movzx eax,word ptr [r13 + 0x34]
    imul eax,ebp
    jmp .lbl180008f9d
    .lbl180008f92: movzx eax,ax
    xor edx,edx
    imul eax,ebp
    div r8d
    .lbl180008f9d: test eax,eax
    jz .lbl180008fc2
    mov r8d,dword ptr [r14 + rbx*0x4]
    test r8d,r8d
    jnz .lbl180008fb6
    movzx eax,word ptr [r13 + 0x34]
    imul eax,edi
    mov dword ptr [rcx],eax
    jmp .lbl180008fc8
    .lbl180008fb6: imul eax,edi
    xor edx,edx
    div r8d
    mov dword ptr [rcx],eax
    jmp .lbl180008fc8
    .lbl180008fc2: mov dword ptr [rcx],0x0
    .lbl180008fc8: add r11d,dword ptr [rcx]
    add r9,0x1
    add r10,0x2
    add rcx,0x4
    cmp r9,r15
    jl .lbl180008f76
    mov rsi,qword ptr [rsp + 0xa8]
    lea r14,[r15 + r15*0x1]
    lea rax,[r15*0x4]
    add rsi,r14
    add rbx,0x1
    add r12,rax
    cmp rbx,qword ptr [rsp + 0x28]
    mov qword ptr [rsp + 0xa8],rsi
    jl .lbl180008f60
    .lbl18000900d: mov rdi,qword ptr [rsp + 0x28]
    xor edx,edx
    mov eax,r11d
    div dword ptr [rsp + 0x90]
    test rdi,rdi
    mov r10d,eax
    jle .lbl180009098
    mov esi,dword ptr [rsp + 0x20]
    mov ebp,dword ptr [rsp + 0x24]
    mov r12,qword ptr [rsp + 0x98]
    mov r14,qword ptr [rsp + 0x38]
    lea r11,[r15*0x4]
    lea rbx,[r15 + r15*0x1]
    nop
    nop
    nop
    .lbl180009050: mov rcx,r12
    mov r8,r14
    mov r9,r15
    nop
    nop
    .lbl180009060: test r10d,r10d
    jnz .lbl18000906c
    mov ax,word ptr [r13 + 0x34]
    jmp .lbl18000907b
    .lbl18000906c: xor edx,edx
    mov eax,ebp
    imul eax,dword ptr [r8]
    div r10d
    xor edx,edx
    div esi
    .lbl18000907b: mov word ptr [rcx],ax
    add rcx,0x2
    add r8,0x4
    sub r9,0x1
    jnz .lbl180009060
    add r12,rbx
    add r14,r11
    sub rdi,0x1
    jnz .lbl180009050
    .lbl180009098: mov rdi,qword ptr [rsp + 0x50]
    mov rbx,qword ptr [rsp + 0x58]
    mov r15,qword ptr [rsp + 0x48]
    .lbl1800090a7: add rsp,0x60
    pop r14
    pop r13
    pop r12
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180008ae0:
    test edx,edx
    mov r11d,edx
    mov r10d,0x1
    jz .lbl180008b2b
    xor edx,edx
    mov eax,ecx
    mov dword ptr [r9],0x0
    div r11d
    cmp eax,r8d
    jnc .lbl180008b2b
    .lbl180008b00: lea r10d,[r10 + r10*0x4]
    add r10d,r10d
    cmp r10d,0xf4240
    ja .lbl180008b24
    xor edx,edx
    mov eax,r10d
    imul eax,ecx
    div r11d
    cmp eax,r8d
    jc .lbl180008b00
    mov eax,r10d
    ret
    .lbl180008b24: mov dword ptr [r9],0x1
    .lbl180008b2b: mov eax,r10d
    ret

section .data


section .text
EXTR_FUN_18000a7c0:
    mov qword ptr [rsp + 0x8],rdi
    imul edx,r8d
    mov rdi,rcx
    test edx,edx
    movsxd rcx,edx
    jle .lbl18000a7db
    mov eax,0x7fff
    rep stosw
    .lbl18000a7db: mov rdi,qword ptr [rsp + 0x8]
    ret

section .data


section .text
EXTR_FUN_180008cf0:
    imul edx,r8d
    xor eax,eax
    test edx,edx
    movsxd r8,edx
    jle .lbl180008d16
    nop
    .lbl180008d00: cmp byte ptr [rax + r9*0x1],0x0
    jnz .lbl180008d0d
    mov word ptr [rcx + rax*0x2],0x8000
    .lbl180008d0d: add rax,0x1
    cmp rax,r8
    jl .lbl180008d00
    .lbl180008d16: ret 0x0

section .data


section .text
EXTR_FUN_18000a6e0:
    mov qword ptr [rsp + 0x18],rbx
    mov qword ptr [rsp + 0x20],rbp
    push r12
    push r13
    xor r13d,r13d
    test r9d,r9d
    movsxd rbx,r8d
    mov r12,rdx
    mov rbp,rcx
    movsxd r11,r9d
    mov r10,r13
    jle .lbl18000a723
    lea r8,[rbx + -0x1]
    imul r8,r11
    add r8,rcx
    .lbl18000a710: mov byte ptr [r10 + rcx*0x1],0x80
    mov byte ptr [r8 + r10*0x1],0x80
    add r10,0x1
    cmp r10,r11
    jl .lbl18000a710
    .lbl18000a723: lea eax,[rbx + -0x1]
    cmp eax,0x1
    jle .lbl18000a7a8
    add eax,-0x1
    mov qword ptr [rsp + 0x18],rsi
    mov r10,r11
    mov ebx,eax
    mov qword ptr [rsp + 0x20],rdi
    nop
    .lbl18000a740: test r11,r11
    mov r8,r13
    jle .lbl18000a795
    mov rdi,r10
    lea rsi,[r10 + r11*0x1]
    lea r9,[r10 + rbp*0x1]
    sub rdi,r11
    add rdi,r12
    add rsi,r12
    nop
    .lbl18000a760: movzx eax,byte ptr [rsi + r8*0x1]
    movzx ecx,byte ptr [rdi + r8*0x1]
    sub eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    sub eax,-0x80
    cmp eax,0xff
    jle .lbl18000a782
    mov eax,0xff
    jmp .lbl18000a788
    .lbl18000a782: test eax,eax
    cmovs eax,r13d
    .lbl18000a788: mov byte ptr [r9 + r8*0x1],al
    add r8,0x1
    cmp r8,r11
    jl .lbl18000a760
    .lbl18000a795: add r10,r11
    sub rbx,0x1
    jnz .lbl18000a740
    mov rdi,qword ptr [rsp + 0x20]
    mov rsi,qword ptr [rsp + 0x18]
    .lbl18000a7a8: mov rbx,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x30]
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_18000a610:
    sub rsp,0x18
    test r8d,r8d
    mov r11,rcx
    jle .lbl18000a6d4
    mov qword ptr [rsp + 0x20],rbx
    mov qword ptr [rsp + 0x28],rbp
    mov qword ptr [rsp + 0x30],rsi
    mov qword ptr [rsp + 0x10],rdi
    mov qword ptr [rsp + 0x8],r12
    movsxd rbx,r9d
    mov qword ptr [rsp],r13
    lea r12d,[r9 + -0x1]
    lea rsi,[rdx + 0x2]
    xor r13d,r13d
    lea rdi,[rbx + rcx*0x1 + -0x1]
    mov ebp,r8d
    .lbl18000a653: cmp r12d,0x1
    mov byte ptr [r11],0x80
    mov byte ptr [rdi],0x80
    jle .lbl18000a6a8
    lea r8,[r11 + 0x1]
    mov r9,rsi
    lea r10d,[r12 + -0x1]
    nop
    .lbl18000a670: movzx ecx,byte ptr [r9 + -0x2]
    movzx eax,byte ptr [r9]
    sub eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    sub eax,-0x80
    cmp eax,0xff
    jle .lbl18000a691
    mov eax,0xff
    jmp .lbl18000a697
    .lbl18000a691: test eax,eax
    cmovs eax,r13d
    .lbl18000a697: mov byte ptr [r8],al
    add r8,0x1
    add r9,0x1
    sub r10,0x1
    jnz .lbl18000a670
    .lbl18000a6a8: add rdi,rbx
    add r11,rbx
    add rsi,rbx
    sub rbp,0x1
    jnz .lbl18000a653
    mov r13,qword ptr [rsp]
    mov r12,qword ptr [rsp + 0x8]
    mov rdi,qword ptr [rsp + 0x10]
    mov rsi,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x28]
    mov rbx,qword ptr [rsp + 0x20]
    .lbl18000a6d4: add rsp,0x18
    ret

section .data


section .text
EXTR_FUN_180009a60:
    mov qword ptr [rsp + 0x8],rbx
    mov word ptr [rsp + 0x20],r9w
    mov dword ptr [rsp + 0x18],r8d
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov rdi,qword ptr [rsp + 0xb0]
    mov eax,edx
    mov r13d,edx
    and eax,0x7
    mov r14d,r8d
    xor esi,esi
    sub r13d,eax
    mov eax,r8d
    mov r15d,r8d
    and eax,0x7
    add r13d,0x8
    mov ebp,edx
    sub r14d,eax
    mov r12,rcx
    add r14d,0x8
    mov eax,r14d
    mov dword ptr [rsp + 0x98],r14d
    imul eax,r13d
    add rax,rdi
    test r8d,r8d
    mov qword ptr [rsp + 0xb0],rax
    jz .lbl180009b7c
    mov r10d,r13d
    mov r14,r15
    mov r11d,esi
    mov ebx,esi
    mov r15d,0x100
    sub r10d,ebp
    .lbl180009ae7: test ebp,ebp
    jz .lbl180009b1d
    mov r8d,r11d
    mov edx,ebx
    mov r9,rbp
    .lbl180009af3: mov eax,edx
    movzx ecx,byte ptr [r12 + rax*0x2 + 0x1]
    test ecx,ecx
    jns .lbl180009b03
    mov ecx,esi
    jmp .lbl180009b0a
    .lbl180009b03: cmp ecx,r15d
    cmovg ecx,r15d
    .lbl180009b0a: mov eax,r8d
    add edx,0x1
    add r8d,0x1
    sub r9,0x1
    mov byte ptr [rax + rdi*0x1],cl
    jnz .lbl180009af3
    .lbl180009b1d: test r10d,r10d
    mov edx,esi
    jz .lbl180009b5d
    lea r9d,[rbx + rbp*0x1]
    lea r8d,[r11 + rbp*0x1]
    nop
    .lbl180009b30: mov eax,r9d
    sub eax,edx
    sub eax,0x1
    movzx ecx,byte ptr [r12 + rax*0x2 + 0x1]
    test ecx,ecx
    jns .lbl180009b46
    mov ecx,esi
    jmp .lbl180009b4d
    .lbl180009b46: cmp ecx,r15d
    cmovg ecx,r15d
    .lbl180009b4d: add edx,0x1
    mov byte ptr [r8 + rdi*0x1],cl
    add r8d,0x1
    cmp edx,r10d
    jc .lbl180009b30
    .lbl180009b5d: add ebx,ebp
    add r11d,r13d
    sub r14,0x1
    jnz .lbl180009ae7
    mov r15d,dword ptr [rsp + 0xa0]
    mov r14d,dword ptr [rsp + 0x98]
    .lbl180009b7c: mov ebx,r14d
    mov r10d,esi
    sub ebx,r15d
    jz .lbl180009bd3
    mov r11d,r13d
    imul r11d,r15d
    nop
    .lbl180009b90: test r13d,r13d
    jz .lbl180009bc7
    mov edx,r15d
    mov r8d,r11d
    mov r9d,r13d
    sub edx,r10d
    sub edx,0x1
    imul edx,r13d
    nop
    nop
    .lbl180009bb0: movzx eax,byte ptr [rdx + rdi*0x1]
    mov ecx,r8d
    add edx,0x1
    add r8d,0x1
    sub r9,0x1
    mov byte ptr [rcx + rdi*0x1],al
    jnz .lbl180009bb0
    .lbl180009bc7: add r10d,0x1
    add r11d,r13d
    cmp r10d,ebx
    jc .lbl180009b90
    .lbl180009bd3: mov rax,qword ptr [rsp + 0xb0]
    movss xmm0,dword ptr [glb18013ab20]
    xor r9d,r9d
    mov qword ptr [rsp + 0x48],rax
    mov r8d,r14d
    mov edx,r13d
    mov rcx,rdi
    movss dword ptr [rsp + 0x40],xmm0
    mov dword ptr [rsp + 0x38],0x80
    mov dword ptr [rsp + 0x30],0x8
    mov dword ptr [rsp + 0x28],0x8
    mov byte ptr [rsp + 0x20],0xff
    call EXTR_FUN_180009600
    movzx eax,word ptr [rsp + 0xa8]
    add eax,0x1
    cdq
    movzx r11d,dl
    add r11d,eax
    sar r11d,0x8
    test r15d,r15d
    jz .lbl180009c7c
    mov r10d,esi
    mov ebx,r15d
    nop
    .lbl180009c40: test ebp,ebp
    jz .lbl180009c70
    mov edx,r10d
    mov r8d,esi
    mov r9,rbp
    nop
    .lbl180009c50: mov eax,r8d
    add r8d,0x1
    movzx ecx,byte ptr [rax + rdi*0x1]
    mov eax,edx
    add edx,0x1
    imul cx,r11w
    sub r9,0x1
    mov word ptr [r12 + rax*0x2],cx
    jnz .lbl180009c50
    .lbl180009c70: add r10d,ebp
    add esi,r13d
    sub rbx,0x1
    jnz .lbl180009c40
    .lbl180009c7c: xor eax,eax
    mov rbx,qword ptr [rsp + 0x90]
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data

glb18013ab20: db 0, 0, 32, 65

section .text
EXTR_FUN_180009600:
    push rbx
    push rsi
    sub rsp,0x1e8
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x1a0],rax
    mov esi,dword ptr [rsp + 0x228]
    mov rbx,qword ptr [rsp + 0x248]
    movzx r10d,r9b
    cmp esi,0x10
    mov byte ptr [rsp + 0x50],r9b
    mov eax,edx
    mov dword ptr [rsp + 0x54],edx
    mov qword ptr [rsp + 0x68],rcx
    mov qword ptr [rsp + 0x70],rbx
    jbe .lbl180009651
    or eax,0xffffffff
    jmp .lbl180009a38
    .lbl180009651: mov qword ptr [rsp + 0x1c8],r13
    mov r13d,dword ptr [rsp + 0x230]
    cmp r13d,0x10
    jbe .lbl180009671
    mov eax,0xfffffffe
    jmp .lbl180009a30
    .lbl180009671: xor edx,edx
    mov qword ptr [rsp + 0x1e0],rbp
    div esi
    test edx,edx
    mov ebp,eax
    jz .lbl18000968d
    mov eax,0xfffffffd
    jmp .lbl180009a28
    .lbl18000968d: xor edx,edx
    mov eax,r8d
    mov qword ptr [rsp + 0x1d0],r12
    div r13d
    test edx,edx
    mov r12d,eax
    mov dword ptr [rsp + 0x58],eax
    jz .lbl1800096b2
    mov eax,0xfffffffc
    jmp .lbl180009a20
    .lbl1800096b2: movzx ecx,byte ptr [rsp + 0x220]
    cmp ecx,0x100
    jc .lbl1800096cc
    mov eax,0xfffffffb
    jmp .lbl180009a20
    .lbl1800096cc: cmp r10b,cl
    jc .lbl1800096db
    mov eax,0xfffffffa
    jmp .lbl180009a20
    .lbl1800096db: cmp esi,0x2
    jc .lbl180009a1b
    cmp r13d,0x2
    jc .lbl180009a1b
    movss xmm0,dword ptr [rsp + 0x240]
    ucomiss xmm0,dword ptr [glb18013ab1c]
    jp .lbl180009709
    jnz .lbl180009709
    xor eax,eax
    jmp .lbl180009a20
    .lbl180009709: mov qword ptr [rsp + 0x1d8],rdi
    mov qword ptr [rsp + 0x1c0],r14
    mov r14d,dword ptr [rsp + 0x238]
    mov eax,0x80
    test r14d,r14d
    mov qword ptr [rsp + 0x1b8],r15
    cmovz r14d,eax
    mov eax,r12d
    mov dword ptr [rsp + 0x238],r14d
    imul eax,ebp
    comiss xmm0,dword ptr [glb18013ab18]
    mov dword ptr [rsp + 0x78],eax
    jbe .lbl180009783
    cvtsi2ss xmm1,rax
    mov eax,r14d
    mulss xmm1,xmm0
    cvtsi2ss xmm0,rax
    mov eax,0x1
    divss xmm1,xmm0
    cvttss2si rbx,xmm1
    cmp ebx,eax
    cmovc ebx,eax
    mov qword ptr [rsp + 0x90],rbx
    mov rbx,qword ptr [rsp + 0x70]
    jmp .lbl18000978e
    .lbl180009783: mov dword ptr [rsp + 0x90],0x4000
    .lbl18000978e: movzx r8d,cl
    lea rcx,[rsp + 0xa0]
    mov r9d,r14d
    movzx edx,r10b
    call EXTR_FUN_1800092a0
    test r13d,r13d
    mov r10,qword ptr [rsp + 0x68]
    mov rdi,r10
    jz .lbl1800098dc
    mov r10d,dword ptr [rsp + 0x54]
    mov edx,r12d
    xor r12d,r12d
    lea eax,[rdx + -0x1]
    mov dword ptr [rsp + 0x5c],r12d
    imul eax,r10d
    mov qword ptr [rsp + 0x80],rax
    mov eax,esi
    mov r8,qword ptr [rsp + 0x80]
    imul eax,r14d
    mov dword ptr [rsp + 0x60],eax
    mov rax,r13
    mov ecx,dword ptr [rsp + 0x60]
    mov qword ptr [rsp + 0x88],rax
    .lbl1800097f7: test esi,esi
    jz .lbl1800098a8
    mov r13d,r12d
    mov r12,qword ptr [rsp + 0x90]
    mov r15d,esi
    nop
    .lbl180009810: mov eax,r13d
    mov r9d,edx
    mov r8d,ebp
    lea rbx,[rbx + rax*0x4]
    lea rax,[rsp + 0xa0]
    mov edx,r10d
    mov qword ptr [rsp + 0x30],rax
    mov rcx,rdi
    mov dword ptr [rsp + 0x28],r14d
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_1800092f0
    mov r8d,r12d
    mov edx,r14d
    mov rcx,rbx
    call EXTR_FUN_1800093a0
    mov eax,dword ptr [rsp + 0x78]
    movzx r8d,byte ptr [rsp + 0x220]
    movzx edx,byte ptr [rsp + 0x50]
    mov r9d,r14d
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_1800094a0
    mov edx,dword ptr [rsp + 0x58]
    mov rbx,qword ptr [rsp + 0x70]
    mov r10d,dword ptr [rsp + 0x54]
    add r13d,r14d
    add rdi,rbp
    sub r15,0x1
    jnz .lbl180009810
    mov esi,dword ptr [rsp + 0x228]
    mov r12d,dword ptr [rsp + 0x5c]
    mov rax,qword ptr [rsp + 0x88]
    mov ecx,dword ptr [rsp + 0x60]
    mov r8,qword ptr [rsp + 0x80]
    .lbl1800098a8: mov rbx,qword ptr [rsp + 0x70]
    add r12d,ecx
    add rdi,r8
    sub rax,0x1
    mov dword ptr [rsp + 0x5c],r12d
    mov qword ptr [rsp + 0x88],rax
    jnz .lbl1800097f7
    mov r13d,dword ptr [rsp + 0x230]
    mov r10,qword ptr [rsp + 0x68]
    mov r12d,dword ptr [rsp + 0x58]
    .lbl1800098dc: xor r15d,r15d
    mov dword ptr [rsp + 0x5c],r15d
    .lbl1800098e4: test r15d,r15d
    jnz .lbl1800098f4
    shr r12d,1
    xor r13d,r13d
    xor r14d,r14d
    jmp .lbl18000990d
    .lbl1800098f4: cmp r15d,r13d
    jnz .lbl180009905
    shr r12d,1
    add r13d,-0x1
    mov r14d,r13d
    jmp .lbl18000990d
    .lbl180009905: lea r13d,[r15 + -0x1]
    lea r14d,[r13 + 0x1]
    .lbl18000990d: mov r15d,dword ptr [rsp + 0x238]
    xor edi,edi
    imul r14d,esi
    imul r13d,esi
    nop
    .lbl180009920: test edi,edi
    jnz .lbl180009930
    mov ebx,ebp
    xor r8d,r8d
    shr ebx,1
    xor r9d,r9d
    jmp .lbl18000994b
    .lbl180009930: cmp edi,esi
    jnz .lbl180009941
    lea r8d,[rsi + -0x1]
    mov ebx,ebp
    mov r9d,r8d
    shr ebx,1
    jmp .lbl18000994b
    .lbl180009941: lea r8d,[rdi + -0x1]
    mov ebx,ebp
    lea r9d,[r8 + 0x1]
    .lbl18000994b: mov r11,qword ptr [rsp + 0x70]
    lea eax,[r14 + r9*0x1]
    imul eax,r15d
    lea rdx,[r11 + rax*0x4]
    lea eax,[r14 + r8*0x1]
    imul eax,r15d
    lea rcx,[r11 + rax*0x4]
    lea eax,[r13 + r9*0x1]
    imul eax,r15d
    lea r9,[r11 + rax*0x4]
    lea eax,[r13 + r8*0x1]
    imul eax,r15d
    lea r8,[r11 + rax*0x4]
    lea rax,[rsp + 0xa0]
    mov qword ptr [rsp + 0x40],rax
    mov dword ptr [rsp + 0x38],r12d
    mov dword ptr [rsp + 0x30],ebx
    mov qword ptr [rsp + 0x28],rdx
    mov edx,dword ptr [rsp + 0x54]
    mov qword ptr [rsp + 0x20],rcx
    mov rcx,r10
    call EXTR_FUN_1800090c0
    mov r10,qword ptr [rsp + 0x68]
    mov r11d,ebx
    add r10,r11
    add edi,0x1
    cmp edi,esi
    mov qword ptr [rsp + 0x68],r10
    jbe .lbl180009920
    mov r15d,dword ptr [rsp + 0x5c]
    mov r13d,dword ptr [rsp + 0x230]
    lea eax,[r12 + -0x1]
    imul eax,dword ptr [rsp + 0x54]
    mov r12d,dword ptr [rsp + 0x58]
    add r15d,0x1
    add r10,rax
    cmp r15d,r13d
    mov qword ptr [rsp + 0x68],r10
    mov dword ptr [rsp + 0x5c],r15d
    jbe .lbl1800098e4
    mov r15,qword ptr [rsp + 0x1b8]
    mov r14,qword ptr [rsp + 0x1c0]
    mov rdi,qword ptr [rsp + 0x1d8]
    xor eax,eax
    jmp .lbl180009a20
    .lbl180009a1b: mov eax,0xfffffff9
    .lbl180009a20: mov r12,qword ptr [rsp + 0x1d0]
    .lbl180009a28: mov rbp,qword ptr [rsp + 0x1e0]
    .lbl180009a30: mov r13,qword ptr [rsp + 0x1c8]
    .lbl180009a38: mov rcx,qword ptr [rsp + 0x1a0]
    xor rcx,rsp
    call EXTR_check_cookie
    add rsp,0x1e8
    pop rsi
    pop rbx
    ret

section .data

glb18013ab1c: db 0
glb18013ab18: db 0

section .text
EXTR_FUN_1800094a0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    mov qword ptr [rsp + 0x18],rdi
    xor r10d,r10d
    movzx ebx,dl
    movzx edx,r8b
    mov eax,edx
    mov rdi,rcx
    mov r11d,r10d
    sub eax,ebx
    cmp r9d,0x4
    mov rsi,r10
    cvtsi2ss xmm1,eax
    mov eax,dword ptr [rsp + 0x28]
    cvtsi2ss xmm0,rax
    divss xmm1,xmm0
    jl .lbl1800095a7
    lea eax,[r9 + -0x4]
    add rcx,0x8
    cvtsi2ss xmm2,rbx
    shr eax,0x2
    add eax,0x1
    mov r8d,eax
    lea r11d,[rax*0x4]
    lea rsi,[rax*0x4]
    nop
    nop
    nop
    .lbl180009510: add r10d,dword ptr [rcx + -0x8]
    mov eax,r10d
    cvtsi2ss xmm0,rax
    mulss xmm0,xmm1
    addss xmm0,xmm2
    cvttss2si rax,xmm0
    cmp eax,edx
    mov dword ptr [rcx + -0x8],eax
    jbe .lbl180009533
    mov dword ptr [rcx + -0x8],edx
    .lbl180009533: add r10d,dword ptr [rcx + -0x4]
    mov eax,r10d
    cvtsi2ss xmm0,rax
    mulss xmm0,xmm1
    addss xmm0,xmm2
    cvttss2si rax,xmm0
    cmp eax,edx
    mov dword ptr [rcx + -0x4],eax
    jbe .lbl180009556
    mov dword ptr [rcx + -0x4],edx
    .lbl180009556: add r10d,dword ptr [rcx]
    mov eax,r10d
    cvtsi2ss xmm0,rax
    mulss xmm0,xmm1
    addss xmm0,xmm2
    cvttss2si rax,xmm0
    cmp eax,edx
    mov dword ptr [rcx],eax
    jbe .lbl180009576
    mov dword ptr [rcx],edx
    .lbl180009576: add r10d,dword ptr [rcx + 0x4]
    mov eax,r10d
    cvtsi2ss xmm0,rax
    mulss xmm0,xmm1
    addss xmm0,xmm2
    cvttss2si rax,xmm0
    cmp eax,edx
    mov dword ptr [rcx + 0x4],eax
    jbe .lbl180009599
    mov dword ptr [rcx + 0x4],edx
    .lbl180009599: add rcx,0x10
    sub r8,0x1
    jnz .lbl180009510
    .lbl1800095a7: cmp r11d,r9d
    jnc .lbl1800095e5
    sub r9d,r11d
    lea rcx,[rdi + rsi*0x4]
    cvtsi2ss xmm2,rbx
    mov r8d,r9d
    .lbl1800095bb: add r10d,dword ptr [rcx]
    mov eax,r10d
    cvtsi2ss xmm0,rax
    mulss xmm0,xmm1
    addss xmm0,xmm2
    cvttss2si rax,xmm0
    cmp eax,edx
    mov dword ptr [rcx],eax
    jbe .lbl1800095db
    mov dword ptr [rcx],edx
    .lbl1800095db: add rcx,0x4
    sub r8,0x1
    jnz .lbl1800095bb
    .lbl1800095e5: mov rbx,qword ptr [rsp + 0x8]
    mov rsi,qword ptr [rsp + 0x10]
    mov rdi,qword ptr [rsp + 0x18]
    ret

section .data


section .text
EXTR_FUN_1800090c0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x20],r9
    mov qword ptr [rsp + 0x18],r8
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x10
    mov r11d,dword ptr [rsp + 0x80]
    mov r12d,dword ptr [rsp + 0x88]
    mov r14,r9
    mov r9d,r11d
    xor esi,esi
    sub edx,r11d
    mov r10,rcx
    imul r9d,r12d
    lea eax,[r9 + -0x1]
    test r9d,eax
    jz .lbl1800091c2
    xor ebx,ebx
    test r12d,r12d
    mov edi,r12d
    jz .lbl180009283
    mov r15,qword ptr [rsp + 0x60]
    mov r13d,edx
    mov qword ptr [rsp],r13
    nop
    nop
    nop
    .lbl180009130: xor esi,esi
    test r11d,r11d
    mov ebp,r11d
    jz .lbl1800091ab
    mov r12,qword ptr [rsp + 0x90]
    mov r13,qword ptr [rsp + 0x78]
    nop
    nop
    nop
    .lbl180009150: movzx eax,byte ptr [r10]
    add r10,0x1
    movzx r8d,byte ptr [rax + r12*0x1]
    mov ecx,dword ptr [r14 + r8*0x4]
    mov eax,dword ptr [r15 + r8*0x4]
    imul eax,ebp
    imul ecx,esi
    add eax,ecx
    mov rcx,qword ptr [rsp + 0x70]
    imul eax,edi
    mov edx,dword ptr [rcx + r8*0x4]
    mov ecx,dword ptr [r13 + r8*0x4]
    imul edx,ebp
    imul ecx,esi
    add edx,ecx
    add esi,0x1
    add ebp,-0x1
    imul edx,ebx
    add eax,edx
    xor edx,edx
    div r9d
    cmp esi,r11d
    mov byte ptr [r10 + -0x1],al
    jc .lbl180009150
    mov r12d,dword ptr [rsp + 0x88]
    mov r13,qword ptr [rsp]
    .lbl1800091ab: add ebx,0x1
    add edi,-0x1
    add r10,r13
    cmp ebx,r12d
    jc .lbl180009130
    jmp .lbl180009283
    .lbl1800091c2: shr r9d,1
    jz .lbl1800091cf
    .lbl1800091c7: add esi,0x1
    shr r9d,1
    jnz .lbl1800091c7
    .lbl1800091cf: xor ebx,ebx
    test r12d,r12d
    mov ebp,r12d
    jz .lbl180009283
    mov r13,qword ptr [rsp + 0x90]
    mov r14,qword ptr [rsp + 0x78]
    mov r15,qword ptr [rsp + 0x70]
    mov eax,edx
    mov qword ptr [rsp],rax
    .lbl1800091f5: xor r9d,r9d
    test r11d,r11d
    mov edi,r11d
    jz .lbl180009271
    mov r12,qword ptr [rsp + 0x68]
    nop
    nop
    nop
    .lbl180009210: movzx eax,byte ptr [r10]
    add r10,0x1
    movzx edx,byte ptr [rax + r13*0x1]
    mov r8d,dword ptr [r8 + rdx*0x4]
    mov eax,dword ptr [r12 + rdx*0x4]
    mov ecx,dword ptr [r15 + rdx*0x4]
    imul r8d,edi
    imul eax,r9d
    imul ecx,edi
    add r8d,eax
    mov eax,dword ptr [r14 + rdx*0x4]
    add edi,-0x1
    imul r8d,ebp
    imul eax,r9d
    add r9d,0x1
    add ecx,eax
    imul ecx,ebx
    add r8d,ecx
    mov ecx,esi
    shr r8d,cl
    cmp r9d,r11d
    mov byte ptr [r10 + -0x1],r8b
    mov r8,qword ptr [rsp + 0x60]
    jc .lbl180009210
    mov r12d,dword ptr [rsp + 0x88]
    mov rax,qword ptr [rsp]
    .lbl180009271: add ebx,0x1
    add ebp,-0x1
    add r10,rax
    cmp ebx,r12d
    jc .lbl1800091f5
    .lbl180009283: mov rbx,qword ptr [rsp + 0x50]
    add rsp,0x10
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_1800092a0:
    mov qword ptr [rsp + 0x8],rbx
    movzx ebx,dl
    movzx r11d,r8b
    xor edx,edx
    mov eax,r11d
    sub eax,ebx
    div r9d
    add al,0x1
    cmp ebx,r11d
    jg .lbl1800092e7
    lea r8,[rbx + rcx*0x1]
    mov ecx,ebx
    movzx r10d,al
    sub ecx,ebx
    sub r11d,ebx
    lea r9d,[r11 + 0x1]
    .lbl1800092d0: mov eax,ecx
    add r8,0x1
    add ecx,0x1
    cdq
    idiv r10d
    sub r9,0x1
    mov byte ptr [r8 + -0x1],al
    jnz .lbl1800092d0
    .lbl1800092e7: mov rbx,qword ptr [rsp + 0x8]
    ret

section .data


section .text
EXTR_FUN_1800092f0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x20
    mov eax,dword ptr [rsp + 0x58]
    mov rdi,qword ptr [rsp + 0x50]
    mov esi,r9d
    test eax,eax
    mov ebp,r8d
    mov r12d,edx
    mov rbx,rcx
    jz .lbl180009334
    mov r8,rax
    xor edx,edx
    mov rcx,rdi
    shl r8,0x2
    call EXTR_memset wrt ..plt
    .lbl180009334: test esi,esi
    jz .lbl18000937f
    mov r8,qword ptr [rsp + 0x60]
    mov r10,rbp
    neg ebp
    movsxd r11,ebp
    mov r9,rsi
    add r11,r12
    nop
    nop
    .lbl180009350: lea rdx,[r10 + rbx*0x1]
    cmp rbx,rdx
    jnc .lbl180009375
    nop
    nop
    .lbl180009360: movzx eax,byte ptr [rbx]
    add rbx,0x1
    movzx ecx,byte ptr [rax + r8*0x1]
    add dword ptr [rdi + rcx*0x4],0x1
    cmp rbx,rdx
    jc .lbl180009360
    .lbl180009375: sub r9,0x1
    lea rbx,[r11 + rdx*0x1]
    jnz .lbl180009350
    .lbl18000937f: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x48]
    add rsp,0x20
    pop r12
    ret

section .data


section .text
EXTR_FUN_1800093a0:
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    xor r9d,r9d
    test edx,edx
    mov r11d,r8d
    mov edi,edx
    mov rsi,rcx
    jz .lbl1800093d7
    mov r10,rcx
    mov r8,rdi
    .lbl1800093c0: mov eax,dword ptr [r10]
    sub eax,r11d
    test eax,eax
    jle .lbl1800093cd
    add r9d,eax
    .lbl1800093cd: add r10,0x4
    sub r8,0x1
    jnz .lbl1800093c0
    .lbl1800093d7: xor edx,edx
    mov eax,r9d
    mov r8d,r11d
    div edi
    sub r8d,eax
    test edi,edi
    mov ebx,eax
    jz .lbl180009420
    mov r10,rdi
    nop
    .lbl1800093f0: mov edx,dword ptr [rcx]
    cmp edx,r11d
    jbe .lbl1800093fc
    mov dword ptr [rcx],r11d
    jmp .lbl180009416
    .lbl1800093fc: cmp edx,r8d
    jbe .lbl18000940e
    mov eax,r8d
    mov dword ptr [rcx],r11d
    sub eax,edx
    add r9d,eax
    jmp .lbl180009416
    .lbl18000940e: lea eax,[rdx + rbx*0x1]
    sub r9d,ebx
    mov dword ptr [rcx],eax
    .lbl180009416: add rcx,0x4
    sub r10,0x1
    jnz .lbl1800093f0
    .lbl180009420: test r9d,r9d
    jz .lbl180009491
    mov qword ptr [rsp + 0x10],rbp
    lea r8,[rsi + rdi*0x4]
    mov ebp,0x1
    .lbl180009433: mov rbx,rsi
    .lbl180009436: cmp rbx,r8
    jnc .lbl180009487
    xor edx,edx
    mov eax,edi
    mov rcx,rbx
    div r9d
    mov r10d,eax
    cmp eax,ebp
    cmovc r10d,ebp
    nop
    .lbl180009450: test r9d,r9d
    jz .lbl18000946d
    mov eax,dword ptr [rcx]
    cmp eax,r11d
    jnc .lbl180009464
    add eax,ebp
    add r9d,-0x1
    mov dword ptr [rcx],eax
    .lbl180009464: lea rcx,[rcx + r10*0x4]
    cmp rcx,r8
    jc .lbl180009450
    .lbl18000946d: add rbx,0x4
    test r9d,r9d
    jnz .lbl180009436
    mov rbp,qword ptr [rsp + 0x10]
    mov rbx,qword ptr [rsp + 0x18]
    mov rsi,qword ptr [rsp + 0x20]
    pop rdi
    ret
    .lbl180009487: test r9d,r9d
    jnz .lbl180009433
    mov rbp,qword ptr [rsp + 0x10]
    .lbl180009491: mov rbx,qword ptr [rsp + 0x18]
    mov rsi,qword ptr [rsp + 0x20]
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180008b30:
    push rbx
    push rbp
    push r12
    sub rsp,0x20
    mov ebp,edx
    mov r12d,r9d
    mov rbx,rcx
    imul ebp,r8d
    test rcx,rcx
    jnz .lbl180008b56
    lea eax,[rcx + 0x1]
    add rsp,0x20
    pop r12
    pop rbp
    pop rbx
    ret
    .lbl180008b56: mov qword ptr [rsp + 0x40],rsi
    mov qword ptr [rsp + 0x48],rdi
    mov rdi,qword ptr [rsp + 0x80]
    xor edx,edx
    mov r8d,0x20002
    mov rcx,rdi
    mov qword ptr [rsp + 0x50],r13
    mov qword ptr [rsp + 0x58],r14
    call EXTR_memset wrt ..plt
    mov r14d,dword ptr [rsp + 0x68]
    xor r9d,r9d
    movsxd rbp,ebp
    test rbp,rbp
    mov esi,r14d
    mov r11d,r9d
    mov rcx,r9
    jle .lbl180008bd1
    nop
    nop
    .lbl180008ba0: movzx eax,word ptr [rbx + rcx*0x2]
    add word ptr [rdi + rax*0x2],0x1
    add word ptr [rdi + 0x20000],0x1
    movzx eax,word ptr [rbx + rcx*0x2]
    cmp eax,esi
    jnc .lbl180008bbd
    movzx esi,word ptr [rbx + rcx*0x2]
    .lbl180008bbd: movzx eax,word ptr [rbx + rcx*0x2]
    cmp eax,r11d
    cmova r11d,eax
    add rcx,0x1
    cmp rcx,rbp
    jl .lbl180008ba0
    .lbl180008bd1: movzx r8d,word ptr [rdi + 0x20000]
    mov eax,0x51eb851f
    mov ecx,r8d
    imul r8d,dword ptr [rsp + 0x60]
    imul ecx,r12d
    mul ecx
    mov eax,0x51eb851f
    mov ecx,esi
    shr edx,0x5
    mov r13d,edx
    mul r8d
    or r8,-0x1
    shr edx,0x5
    cmp rcx,0x10000
    mov r10,r8
    mov r12d,edx
    mov rdx,r9
    jnc .lbl180008c53
    nop
    nop
    nop
    .lbl180008c20: movzx eax,word ptr [rdi + rcx*0x2]
    add rdx,rax
    cmp rdx,r13
    jbe .lbl180008c34
    cmp r8,-0x1
    cmovz r8,rcx
    .lbl180008c34: cmp rdx,r12
    jbe .lbl180008c41
    cmp r10,-0x1
    cmovz r10,rcx
    .lbl180008c41: add rcx,0x1
    cmp rcx,0x10000
    jc .lbl180008c20
    test r10,r10
    jns .lbl180008c56
    .lbl180008c53: mov r10d,r11d
    .lbl180008c56: mov r13,qword ptr [rsp + 0x50]
    mov r11,r10
    sub r11,r8
    jz .lbl180008ccc
    imul r14d,dword ptr [rsp + 0x70]
    mov esi,dword ptr [rsp + 0x78]
    mov eax,0x51eb851f
    mul r14d
    mov eax,0xffff
    shr edx,0x5
    mov edi,edx
    lea rcx,[rdx + rsi*0x1]
    cmp rcx,rax
    cmovg rcx,rax
    test rbp,rbp
    jle .lbl180008ccc
    nop
    .lbl180008c90: movzx eax,word ptr [rbx + r9*0x2]
    cmp rax,r8
    jge .lbl180008ca3
    mov word ptr [rbx + r9*0x2],0x0
    jmp .lbl180008cc3
    .lbl180008ca3: cmp rax,r10
    jl .lbl180008caf
    mov word ptr [rbx + r9*0x2],cx
    jmp .lbl180008cc3
    .lbl180008caf: sub rax,r8
    imul rax,rdi
    cqo
    idiv r11
    add ax,si
    mov word ptr [rbx + r9*0x2],ax
    .lbl180008cc3: add r9,0x1
    cmp r9,rbp
    jl .lbl180008c90
    .lbl180008ccc: mov rdi,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x40]
    mov r14,qword ptr [rsp + 0x58]
    xor eax,eax
    add rsp,0x20
    pop r12
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180002420:
    xor edx,edx
    test ecx,ecx
    mov r8d,ecx
    mov eax,0x40000000
    jg .lbl180002431
    xor eax,eax
    ret
    .lbl180002431: mov ecx,edx
    shr edx,1
    or ecx,eax
    cmp r8d,ecx
    jc .lbl180002441
    sub r8d,ecx
    or edx,eax
    .lbl180002441: shr eax,0x2
    jnz .lbl180002431
    mov eax,edx
    ret

section .data


section .text
EXTR_FUN_180009ca0:
    push rbx
    mov eax,dword ptr [rsp + 0x30]
    mov rbx,rdx
    mov r11,rcx
    imul eax,dword ptr [rsp + 0x38]
    test rcx,rcx
    jz .lbl180009d1e
    test rdx,rdx
    jz .lbl180009d1e
    test r8,r8
    jz .lbl180009d1e
    xor r10d,r10d
    test eax,eax
    mov qword ptr [rsp + 0x10],rdi
    movsxd rdi,eax
    jle .lbl180009d19
    sub r11,r8
    sub rbx,r8
    .lbl180009cd5: test r9,r9
    movzx ecx,word ptr [r11 + r8*0x1]
    jz .lbl180009ced
    cmp byte ptr [r10 + r9*0x1],0x0
    jnz .lbl180009ced
    mov eax,0x7fff
    jmp .lbl180009cf1
    .lbl180009ced: movzx eax,word ptr [r8]
    .lbl180009cf1: imul ecx,ecx,0x31
    add r10,0x1
    add r8,0x2
    add ecx,eax
    mov eax,0x10624dd3
    lea ecx,[rcx + rcx*0x4]
    shl ecx,0x2
    mul ecx
    shr edx,0x6
    cmp r10,rdi
    mov word ptr [rbx + r8*0x1 + -0x2],dx
    jl .lbl180009cd5
    .lbl180009d19: mov rdi,qword ptr [rsp + 0x10]
    .lbl180009d1e: pop rbx
    ret

section .data


section .text
EXTR_FUN_18000a060:
    imul edx,r8d
    xor r8d,r8d
    test edx,edx
    movsxd r10,edx
    jle .lbl18000a0a4
    mov r11d,0xff
    nop
    nop
    nop
    .lbl18000a080: movzx eax,word ptr [rcx + r8*0x2]
    imul eax,eax,0xff
    cdq
    idiv r9d
    cmp eax,r11d
    cmovg eax,r11d
    add r8,0x1
    cmp r8,r10
    mov byte ptr [r8 + rcx*0x1 + -0x1],al
    jl .lbl18000a080
    .lbl18000a0a4: ret 0x0

section .data


section .text
EXTR_FUN_180009d20:
    mov qword ptr [rsp + 0x20],rbx
    mov qword ptr [rsp + 0x18],r8
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x40
    mov eax,dword ptr [rsp + 0xa0]
    mov r13,qword ptr [rsp + 0xb0]
    xor edi,edi
    mov ebx,r9d
    mov qword ptr [rsp + 0x30],rdi
    mov qword ptr [rsp + 0x38],rdi
    mov r15,rdi
    mov ebp,0xc0000000
    mov r12d,0x40000000
    imul ebx,eax
    test rcx,rcx
    movsxd rsi,ebx
    lea r14,[r13 + rsi*0x2]
    jz .lbl18000a035
    test rdx,rdx
    jz .lbl18000a035
    test eax,eax
    jle .lbl18000a035
    test r9d,r9d
    jle .lbl18000a035
    mov rdx,rcx
    add ebx,ebx
    mov rcx,r13
    mov r8d,ebx
    call EXTR_memcpy wrt ..plt
    mov rdx,qword ptr [rsp + 0x88]
    mov r8d,ebx
    mov rcx,r14
    call EXTR_memcpy wrt ..plt
    test rsi,rsi
    mov r11,rdi
    jle .lbl180009dff
    movzx edx,byte ptr [rsp + 0xa8]
    mov r8,qword ptr [rsp + 0x90]
    mov rcx,r13
    sub rcx,r14
    mov rax,r14
    .lbl180009de1: test dl,dl
    jz .lbl180009df2
    cmp byte ptr [r11 + r8*0x1],dil
    jnz .lbl180009df2
    mov word ptr [rcx + rax*0x1],di
    mov word ptr [rax],di
    .lbl180009df2: add r11,0x1
    add rax,0x2
    cmp r11,rsi
    jl .lbl180009de1
    .lbl180009dff: cmp rsi,0x2
    mov r8,rdi
    mov r9,rdi
    mov r10,rdi
    mov r11,rdi
    mov rcx,rdi
    jl .lbl180009e63
    mov rbx,r14
    lea rdx,[r13 + 0x2]
    sub rbx,r13
    lea r13,[rsi + -0x1]
    nop
    nop
    nop
    nop
    .lbl180009e30: movzx eax,word ptr [r14 + rcx*0x2]
    add rcx,0x2
    add rdx,0x4
    add r10,rax
    movzx eax,word ptr [rdx + -0x6]
    add r8,rax
    movzx eax,word ptr [rbx + rdx*0x1 + -0x4]
    add r11,rax
    movzx eax,word ptr [rdx + -0x4]
    add r9,rax
    cmp rcx,r13
    jl .lbl180009e30
    mov r13,qword ptr [rsp + 0xb0]
    .lbl180009e63: cmp rcx,rsi
    jge .lbl180009e7d
    movzx eax,word ptr [r14 + rcx*0x2]
    mov qword ptr [rsp + 0x30],rax
    movzx eax,word ptr [r13 + rcx*0x2]
    mov qword ptr [rsp + 0x38],rax
    .lbl180009e7d: lea rax,[r10 + r11*0x1]
    mov rbx,rdi
    mov rcx,rdi
    add rax,qword ptr [rsp + 0x30]
    mov qword ptr [rsp + 0x20],rdi
    mov qword ptr [rsp + 0x28],rdi
    cqo
    idiv rsi
    mov r10,rax
    lea rax,[r8 + r9*0x1]
    add rax,qword ptr [rsp + 0x38]
    cqo
    idiv rsi
    cmp rsi,0x2
    mov rdx,rdi
    mov r11,rax
    jl .lbl180009f36
    mov r9,r14
    lea r8,[r13 + 0x2]
    lea r12,[rsi + -0x1]
    sub r9,r13
    mov r13,rdi
    nop
    nop
    .lbl180009ed0: movzx ecx,word ptr [r14 + rdx*0x2]
    movzx eax,word ptr [r8 + -0x2]
    add rdx,0x2
    sub rcx,r10
    sub rax,r11
    add r8,0x4
    imul rax,rcx
    imul rcx,rcx
    add qword ptr [rsp + 0x20],rax
    movzx eax,word ptr [r8 + -0x4]
    add rbx,rcx
    movzx ecx,word ptr [r8 + r9*0x1 + -0x4]
    sub rax,r11
    sub rcx,r10
    imul rax,rcx
    imul rcx,rcx
    add qword ptr [rsp + 0x28],rax
    add r13,rcx
    cmp rdx,r12
    jl .lbl180009ed0
    mov qword ptr [rsp + 0x38],r13
    mov r13,qword ptr [rsp + 0xb0]
    mov r12d,0x40000000
    mov rcx,qword ptr [rsp + 0x38]
    .lbl180009f36: cmp rdx,rsi
    jge .lbl180009f56
    movzx r15d,word ptr [r14 + rdx*0x2]
    movzx edx,word ptr [r13 + rdx*0x2]
    sub rdx,r11
    sub r15,r10
    imul rdx,r15
    imul r15,r15
    jmp .lbl180009f59
    .lbl180009f56: mov rdx,rdi
    .lbl180009f59: lea rax,[rcx + rbx*0x1]
    mov r8d,edi
    add r15,rax
    test r15,r15
    jle .lbl180009f99
    mov rax,qword ptr [rsp + 0x20]
    mov rcx,qword ptr [rsp + 0x28]
    add rax,rcx
    add rax,rdx
    imul rax,rax,0x64
    cqo
    idiv r15
    test eax,eax
    mov r8,rax
    jns .lbl180009f8d
    mov r8d,edi
    jmp .lbl180009f99
    .lbl180009f8d: mov eax,0x190
    cmp r8d,eax
    cmovg r8d,eax
    .lbl180009f99: test rsi,rsi
    mov r9,rdi
    jle .lbl18000a035
    mov r11,qword ptr [rsp + 0x80]
    sub qword ptr [rsp + 0x88],r11
    mov r10,r11
    mov r11,qword ptr [rsp + 0x88]
    .lbl180009fc0: movzx ecx,word ptr [r11 + r10*0x1]
    mov eax,0x51eb851f
    imul ecx,r8d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    movzx eax,word ptr [r10]
    sub eax,edx
    cmp eax,ebp
    mov dword ptr [r13 + r9*0x4],eax
    cmovg ebp,eax
    cmp eax,r12d
    cmovl r12d,eax
    add r9,0x1
    add r10,0x2
    cmp r9,rsi
    jl .lbl180009fc0
    cmp ebp,r12d
    mov r11,qword ptr [rsp + 0x80]
    jle .lbl18000a035
    sub ebp,r12d
    nop
    .lbl18000a010: mov eax,dword ptr [r13 + rdi*0x4]
    xor edx,edx
    add rdi,0x1
    sub eax,r12d
    imul eax,eax,0xffff
    div ebp
    cmp rdi,rsi
    mov word ptr [r11 + rdi*0x2 + -0x2],ax
    jl .lbl18000a010
    xor eax,eax
    jmp .lbl18000a03a
    .lbl18000a035: mov eax,0x1
    .lbl18000a03a: mov rbx,qword ptr [rsp + 0x98]
    add rsp,0x40
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_18000a4f0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x20
    imul r9d,dword ptr [rsp + 0x60]
    xor r11d,r11d
    xor esi,esi
    xor ebp,ebp
    mov rbx,r8
    mov r13,rdx
    movsxd r9,r9d
    mov r10,rcx
    test r9,r9
    jle .lbl18000a57b
    mov rdx,rbx
    mov rdi,r13
    mov r8,rcx
    sub rdx,rcx
    sub rdi,rcx
    mov rcx,r9
    .lbl18000a538: test rbx,rbx
    jz .lbl18000a544
    cmp byte ptr [rdx + r8*0x1],0x0
    jbe .lbl18000a555
    .lbl18000a544: movzx eax,byte ptr [r8]
    add r11d,0x1
    add esi,eax
    movzx eax,byte ptr [rdi + r8*0x1]
    add ebp,eax
    .lbl18000a555: add r8,0x1
    sub rcx,0x1
    jnz .lbl18000a538
    test r11d,r11d
    jle .lbl18000a57b
    mov ecx,r11d
    sar ecx,1
    lea eax,[rcx + rsi*0x1]
    cdq
    idiv r11d
    mov esi,eax
    lea eax,[rcx + rbp*0x1]
    cdq
    idiv r11d
    mov ebp,eax
    .lbl18000a57b: xor r12d,r12d
    xor ecx,ecx
    xor edi,edi
    test r9,r9
    jle .lbl18000a5c9
    mov r11,rbx
    sub r11,r10
    sub r13,r10
    .lbl18000a590: test rbx,rbx
    jz .lbl18000a59c
    cmp byte ptr [r10 + r11*0x1],0x0
    jbe .lbl18000a5bf
    .lbl18000a59c: movzx r8d,byte ptr [r10 + r13*0x1]
    movzx edx,byte ptr [r10]
    sub r8d,ebp
    sub edx,esi
    mov eax,r8d
    imul r8d,r8d
    imul eax,edx
    imul edx,edx
    add ecx,edx
    add r12d,eax
    add edi,r8d
    .lbl18000a5bf: add r10,0x1
    sub r9,0x1
    jnz .lbl18000a590
    .lbl18000a5c9: call EXTR_FUN_180002420
    mov ecx,edi
    movsxd rbx,eax
    call EXTR_FUN_180002420
    test ebx,ebx
    jle .lbl18000a5f5
    test eax,eax
    jle .lbl18000a5f5
    movsxd rcx,eax
    movsxd rax,r12d
    shl rax,0xa
    imul rcx,rbx
    cqo
    idiv rcx
    jmp .lbl18000a5f7
    .lbl18000a5f5: xor eax,eax
    .lbl18000a5f7: mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x20
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_18000a3f0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r13
    sub rsp,0x40
    movsxd r13,dword ptr [rsp + 0x78]
    mov esi,r8d
    mov ebp,r9d
    test r13d,r13d
    mov ebx,edx
    mov rdi,rcx
    mov r8,r13
    jle .lbl18000a432
    mov rcx,qword ptr [rsp + 0x70]
    shl r8,0x2
    xor edx,edx
    call EXTR_memset wrt ..plt
    .lbl18000a432: test ebp,ebp
    mov rax,qword ptr [rsp + 0x98]
    mov r8d,ebx
    mov edx,esi
    mov rcx,rdi
    jz .lbl18000a474
    mov r9,qword ptr [rsp + 0x70]
    mov qword ptr [rsp + 0x38],rax
    mov eax,dword ptr [rsp + 0x88]
    mov dword ptr [rsp + 0x30],eax
    mov rax,qword ptr [rsp + 0x80]
    mov qword ptr [rsp + 0x28],rax
    mov dword ptr [rsp + 0x20],r13d
    call EXTR_FUN_180016b80
    jmp .lbl18000a491
    .lbl18000a474: mov r9,qword ptr [rsp + 0x80]
    mov qword ptr [rsp + 0x28],rax
    mov eax,dword ptr [rsp + 0x88]
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_1800169f0
    .lbl18000a491: imul ebx,esi
    test ebx,ebx
    jle .lbl18000a4c9
    movzx r8d,word ptr [rsp + 0x90]
    mov edx,ebx
    .lbl18000a4a3: movsx eax,word ptr [rdi]
    xor ecx,ecx
    add eax,r8d
    test eax,eax
    cmovg ecx,eax
    mov eax,0xffff
    cmp ecx,eax
    cmovl ax,cx
    add rdi,0x2
    sub rdx,0x1
    mov word ptr [rdi + -0x2],ax
    jnz .lbl18000a4a3
    .lbl18000a4c9: mov rbx,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    mov rdi,qword ptr [rsp + 0x68]
    xor eax,eax
    add rsp,0x40
    pop r13
    ret

section .data


section .text
EXTR_FUN_1800169f0:
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x18],rbp
    mov qword ptr [rsp + 0x8],rcx
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov rbx,rcx
    mov ecx,r8d
    mov r13,r9
    mov r15d,r8d
    mov r14d,edx
    call EXTR_round_pow2
    mov ecx,r14d
    mov r12d,eax
    call EXTR_round_pow2
    mov edi,dword ptr [rsp + 0x80]
    mov rcx,r13
    mov edx,edi
    mov ebp,eax
    call EXTR_max_log2ceil
    mov rdx,qword ptr [rsp + 0x88]
    mov r9d,r14d
    mov r8d,r15d
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180018e10
    mov rcx,qword ptr [rsp + 0x88]
    mov r9,r13
    mov r8d,r12d
    mov edx,ebp
    mov dword ptr [rsp + 0x20],edi
    mov ebx,eax
    call EXTR_FUN_180015ec0
    mov rdx,qword ptr [rsp + 0x88]
    mov rcx,qword ptr [rsp + 0x60]
    mov r9d,r14d
    mov r8d,r15d
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180019010
    mov rbx,qword ptr [rsp + 0x68]
    mov rbp,qword ptr [rsp + 0x70]
    xor eax,eax
    add rsp,0x30
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180015ec0:
    mov qword ptr [rsp + 0x20],r9
    push r12
    push r13
    push r14
    sub rsp,0x70
    mov r10d,edx
    mov eax,r8d
    mov r12,rcx
    cdq
    sub eax,edx
    sar eax,1
    mov r11d,eax
    mov dword ptr [rsp + 0xa0],eax
    lea r13d,[rax + 0x1]
    mov eax,r10d
    cdq
    mov r14d,r13d
    sub eax,edx
    imul r14d,r10d
    sar eax,1
    mov r10d,eax
    mov dword ptr [rsp + 0x24],eax
    movsxd rax,r14d
    mov qword ptr [rsp + 0x40],rax
    js .lbl1800160cc
    mov qword ptr [rsp + 0x90],rbx
    mov qword ptr [rsp + 0x68],rbp
    mov ecx,r13d
    neg ecx
    mov qword ptr [rsp + 0x60],rsi
    lea r9,[r12 + rax*0x4]
    mov dword ptr [rsp + 0x98],ecx
    mov qword ptr [rsp + 0x58],rdi
    movsxd rcx,r13d
    mov edx,dword ptr [rsp + 0x98]
    mov qword ptr [rsp + 0x50],r15
    lea eax,[r10 + 0x1]
    xor r15d,r15d
    shl rcx,0x2
    mov r8,r12
    mov qword ptr [rsp + 0x28],r12
    mov qword ptr [rsp + 0x30],r9
    mov qword ptr [rsp + 0x38],rax
    mov qword ptr [rsp + 0x48],rcx
    nop
    nop
    nop
    .lbl180015f70: xor ebx,ebx
    test r13d,r13d
    jle .lbl180016087
    mov eax,r15d
    mov rdi,r8
    mov rsi,r9
    cdq
    idiv r10d
    mov ecx,eax
    imul ecx,ecx
    xor ebp,ebp
    mov dword ptr [rsp + 0x20],ecx
    .lbl180015f93: mov eax,ebp
    cdq
    idiv r11d
    imul eax,eax
    add ecx,eax
    call EXTR_FUN_180002420
    mov edx,dword ptr [rsp + 0xb0]
    mov rcx,qword ptr [rsp + 0xa8]
    mov r8d,eax
    call EXTR_query_range
    mov ecx,dword ptr [rdi]
    mov r11d,eax
    mov eax,0x51eb851f
    shl r11d,0x2
    imul ecx,r11d
    imul ecx
    mov eax,0x51eb851f
    sar edx,0x5
    mov ecx,edx
    shr ecx,0x1f
    add edx,ecx
    mov dword ptr [rdi],edx
    mov ecx,dword ptr [rsi]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    test r15d,r15d
    mov dword ptr [rsi],edx
    jle .lbl180016041
    lea eax,[r14 + rbx*0x1]
    movsxd r8,eax
    mov eax,0x51eb851f
    mov ecx,dword ptr [r12 + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov rax,qword ptr [rsp + 0x40]
    mov dword ptr [r12 + r8*0x4],edx
    add r8,rax
    mov eax,0x51eb851f
    mov ecx,dword ptr [r12 + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov dword ptr [r12 + r8*0x4],edx
    .lbl180016041: mov ecx,dword ptr [rsp + 0x20]
    mov r11d,dword ptr [rsp + 0xa0]
    add ebx,0x1
    add ebp,0x400
    add rdi,0x4
    add rsi,0x4
    cmp ebx,r13d
    jl .lbl180015f93
    mov r8,qword ptr [rsp + 0x28]
    mov r9,qword ptr [rsp + 0x30]
    mov rcx,qword ptr [rsp + 0x48]
    mov rax,qword ptr [rsp + 0x38]
    mov edx,dword ptr [rsp + 0x98]
    mov r10d,dword ptr [rsp + 0x24]
    .lbl180016087: add r8,rcx
    add r9,rcx
    add r15d,0x400
    add r14d,edx
    sub rax,0x1
    mov qword ptr [rsp + 0x28],r8
    mov qword ptr [rsp + 0x38],rax
    mov qword ptr [rsp + 0x30],r9
    jnz .lbl180015f70
    mov r15,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x68]
    mov rbx,qword ptr [rsp + 0x90]
    .lbl1800160cc: add rsp,0x70
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_query_range:
    mov r10d,0x400
    mov eax,r8d
    mov r9,rcx
    cmp r8d,r10d
    jl .lbl180015e3a
    movsxd rax,edx
    movzx eax,word ptr [rcx + rax*0x2 + -0x2]
    ret
    .lbl180015e3a: test eax,eax
    jg .lbl180015e42
    movzx eax,word ptr [rcx]
    ret
    .lbl180015e42: lea r8d,[rdx + -0x1]
    imul r8d,eax
    mov edx,r8d
    sar edx,0xa
    mov ecx,edx
    movsxd rdx,edx
    movzx eax,word ptr [r9 + rdx*0x2 + 0x2]
    shl ecx,0xa
    sub r8d,ecx
    movzx ecx,word ptr [r9 + rdx*0x2]
    sub r10d,r8d
    imul eax,r8d
    imul r10d,ecx
    add eax,r10d
    cdq
    and edx,0x3ff
    add eax,edx
    sar eax,0xa
    ret

section .data


section .text
EXTR_max_log2ceil:
    mov r10,rcx
    xor ecx,ecx
    test edx,edx
    mov r8,rcx
    movsxd r9,edx
    jle .lbl180015eb3
    nop
    .lbl180015ea0: movzx eax,word ptr [r10 + r8*0x2]
    cmp eax,ecx
    cmovg ecx,eax
    add r8,0x1
    cmp r8,r9
    jl .lbl180015ea0
    .lbl180015eb3: jmp EXTR_log2ceil

section .data


section .text
EXTR_FUN_180018e10:
    mov qword ptr [rsp + 0x18],rbx
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov r13,rcx
    mov ecx,r9d
    mov ebp,r9d
    movsxd rsi,r8d
    mov rdi,rdx
    call EXTR_round_pow2
    mov ecx,esi
    mov r12d,eax
    mov dword ptr [rsp + 0x24],eax
    call EXTR_round_pow2
    xor edx,edx
    mov rcx,rdi
    sar eax,1
    lea ebx,[rax + 0x1]
    mov eax,ebx
    mov dword ptr [rsp + 0x28],ebx
    imul eax,r12d
    mov dword ptr [rsp + 0x20],eax
    add eax,eax
    lea r8d,[rax*0x4]
    mov dword ptr [rsp + 0x2c],eax
    call EXTR_memset wrt ..plt
    xor r15d,r15d
    test ebp,ebp
    jle .lbl180018eea
    movsxd rax,dword ptr [rsp + 0x20]
    mov rcx,qword ptr [rsp + 0x78]
    mov r15d,dword ptr [rsp + 0x88]
    movsxd r12,ebx
    mov r14,rsi
    mov rbx,r13
    add r14,r14
    lea rbp,[rcx + rax*0x4]
    shl r12,0x2
    mov r13,r15
    .lbl180018eb0: mov r9d,esi
    mov r8,rbp
    mov rdx,rdi
    mov rcx,rbx
    call EXTR_FUN_180017c80
    add rbp,r12
    add rdi,r12
    add rbx,r14
    sub r13,0x1
    jnz .lbl180018eb0
    mov r13,qword ptr [rsp + 0x70]
    mov rdi,qword ptr [rsp + 0x78]
    mov r12d,dword ptr [rsp + 0x24]
    mov ebx,dword ptr [rsp + 0x28]
    mov ebp,dword ptr [rsp + 0x88]
    .lbl180018eea: mov eax,r12d
    sub eax,ebp
    cdq
    sub eax,edx
    sar eax,1
    lea edx,[rbp + rax*0x1]
    cmp r15d,edx
    jge .lbl180018f62
    lea ecx,[rbp + -0x1]
    movsxd rbp,ebx
    sub edx,r15d
    shl rbp,0x2
    mov r12d,edx
    imul ecx,esi
    movsxd rcx,ecx
    lea r14,[r13 + rcx*0x2]
    mov ecx,ebx
    imul ecx,r15d
    movsxd rax,ecx
    lea rbx,[rdi + rax*0x4]
    mov eax,dword ptr [rsp + 0x20]
    add eax,ecx
    add r15d,edx
    movsxd rcx,eax
    lea rdi,[rdi + rcx*0x4]
    nop
    nop
    nop
    .lbl180018f40: mov r9d,esi
    mov r8,rdi
    mov rdx,rbx
    mov rcx,r14
    call EXTR_FUN_180017c80
    add rdi,rbp
    add rbx,rbp
    sub r12,0x1
    jnz .lbl180018f40
    mov r12d,dword ptr [rsp + 0x24]
    .lbl180018f62: cmp r15d,r12d
    mov r14,qword ptr [rsp + 0x78]
    jge .lbl180018fb0
    movsxd rcx,dword ptr [rsp + 0x28]
    mov rbp,rcx
    shl rbp,0x2
    imul ecx,r15d
    movsxd rax,ecx
    lea rbx,[r14 + rax*0x4]
    mov eax,dword ptr [rsp + 0x20]
    add eax,ecx
    sub r12d,r15d
    movsxd rcx,eax
    lea rdi,[r14 + rcx*0x4]
    .lbl180018f93: mov r9d,esi
    mov r8,rdi
    mov rdx,rbx
    mov rcx,r13
    call EXTR_FUN_180017c80
    add rdi,rbp
    add rbx,rbp
    sub r12,0x1
    jnz .lbl180018f93
    .lbl180018fb0: mov edx,dword ptr [rsp + 0x2c]
    mov r8d,0x1f
    mov rcx,r14
    sub r8d,dword ptr [rsp + 0x90]
    call EXTR_FUN_180017800
    mov r8d,dword ptr [rsp + 0x88]
    mov edx,esi
    mov rcx,r14
    mov ebx,eax
    call EXTR_FUN_1800180d0
    mov eax,ebx
    mov rbx,qword ptr [rsp + 0x80]
    add rsp,0x30
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_round_pow2:
    test ecx,ecx
    jns .lbl1800177c6
    neg ecx
    .lbl1800177c6: cmp ecx,0x1
    jg .lbl1800177d1
    mov eax,0x1
    ret
    .lbl1800177d1: sub ecx,0x1
    mov eax,ecx
    sar eax,1
    or ecx,eax
    mov eax,ecx
    sar eax,0x2
    or ecx,eax
    mov eax,ecx
    sar eax,0x4
    or ecx,eax
    mov eax,ecx
    sar eax,0x8
    or ecx,eax
    mov eax,ecx
    sar eax,0x10
    or eax,ecx
    add eax,0x1
    ret

section .data


section .text
EXTR_FUN_180017c80:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x20
    mov rdi,rcx
    mov ecx,r9d
    mov ebp,r9d
    mov r13,r8
    mov rsi,rdx
    call EXTR_round_pow2
    mov r12d,eax
    xor edx,edx
    mov rcx,rsi
    sar r12d,1
    add r12d,0x1
    lea ebx,[r12*0x4]
    mov r8d,ebx
    call EXTR_memset wrt ..plt
    mov r8d,ebx
    xor edx,edx
    mov rcx,r13
    call EXTR_memset wrt ..plt
    xor r11d,r11d
    cmp ebp,0x1
    jle .lbl180017d20
    mov r8,rsi
    mov r9,r13
    lea edx,[r11 + 0x1]
    sub r8,rdi
    sub r9,rdi
    mov rcx,rdi
    nop
    nop
    nop
    nop
    .lbl180017d00: movsx eax,word ptr [rcx]
    add edx,0x2
    add rcx,0x4
    mov dword ptr [r8 + rcx*0x1 + -0x4],eax
    movsx eax,word ptr [rcx + -0x2]
    add r11d,0x1
    cmp edx,ebp
    mov dword ptr [r9 + rcx*0x1 + -0x4],eax
    jl .lbl180017d00
    .lbl180017d20: lea eax,[rbp + r12*0x2]
    lea r8d,[r11 + r11*0x1]
    movsxd rcx,r11d
    cdq
    sub eax,edx
    sar eax,1
    cmp r8d,eax
    mov r9d,eax
    jge .lbl180017d82
    lea eax,[rbp + -0x1]
    lea rcx,[r13 + rcx*0x4]
    cdq
    sub eax,edx
    sar eax,1
    movsxd r10,eax
    mov eax,ebp
    cdq
    sub eax,edx
    mov rdx,rsi
    sar eax,1
    sub rdx,r13
    movsxd rbx,eax
    nop
    nop
    .lbl180017d60: movsx eax,word ptr [rdi + r10*0x4]
    add r8d,0x2
    add rcx,0x4
    mov dword ptr [rdx + rcx*0x1 + -0x4],eax
    movsx eax,word ptr [rdi + rbx*0x4 + -0x2]
    add r11d,0x1
    cmp r8d,r9d
    mov dword ptr [rcx + -0x4],eax
    jl .lbl180017d60
    .lbl180017d82: movsxd rax,r11d
    movsxd rdx,r12d
    cmp rax,rdx
    jge .lbl180017db8
    sub rsi,r13
    lea rcx,[r13 + rax*0x4]
    sub rdx,rax
    nop
    nop
    .lbl180017da0: movsx eax,word ptr [rdi]
    add rcx,0x4
    sub rdx,0x1
    mov dword ptr [rcx + rsi*0x1 + -0x4],eax
    movsx eax,word ptr [rdi + 0x2]
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180017da0
    .lbl180017db8: mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    xor eax,eax
    add rsp,0x20
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_1800180d0:
    push rbp
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov r13,rcx
    mov ecx,r8d
    mov r15d,edx
    call EXTR_log2ceil
    mov ecx,r15d
    mov edx,eax
    mov dword ptr [rsp + 0x98],eax
    call EXTR_round_pow2
    sar eax,1
    mov ecx,edx
    mov r12d,0x1
    lea ebp,[rax + 0x1]
    shl r12d,cl
    mov r14d,ebp
    mov dword ptr [rsp + 0x80],ebp
    imul r14d,r12d
    test r13,r13
    jnz .lbl180018145
    lea r8d,[r13 + 0x4]
    mov edx,r12d
    mov ecx,eax
    call EXTR_FUN_180017500
    mov ecx,r14d
    mov edx,eax
    lea rax,[rdx + rcx*0x8]
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret
    .lbl180018145: test r12d,r12d
    mov qword ptr [rsp + 0x88],rbx
    lea eax,[r14 + r14*0x1]
    movsxd rcx,eax
    mov qword ptr [rsp + 0x48],rsi
    mov qword ptr [rsp + 0x40],rdi
    lea rbx,[r13 + rcx*0x4]
    mov qword ptr [rsp + 0x30],rbx
    jle .lbl1800181a8
    movsxd rsi,ebp
    movsxd rax,r14d
    mov rbx,r13
    lea rdi,[r13 + rax*0x4]
    shl rsi,0x2
    mov ebp,r12d
    .lbl180018182: mov r8d,r15d
    mov rdx,rdi
    mov rcx,rbx
    call EXTR_FUN_180017a60
    add rdi,rsi
    add rbx,rsi
    sub rbp,0x1
    jnz .lbl180018182
    mov ebp,dword ptr [rsp + 0x80]
    mov rbx,qword ptr [rsp + 0x30]
    .lbl1800181a8: mov r9d,0x4
    mov r8d,r12d
    mov edx,ebp
    mov rcx,r13
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    movsxd r11,r14d
    mov r9d,0x4
    lea r14,[r13 + r11*0x4]
    mov r8d,r12d
    mov edx,ebp
    mov rcx,r14
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    test ebp,ebp
    jle .lbl180018229
    mov r15d,dword ptr [rsp + 0x98]
    movsxd rsi,r12d
    mov rbx,r13
    shl rsi,0x2
    mov rdi,r14
    nop
    nop
    .lbl180018200: xor r9d,r9d
    mov r8d,r15d
    mov rdx,rdi
    mov rcx,rbx
    call EXTR_FUN_180019460
    add rdi,rsi
    add rbx,rsi
    sub rbp,0x1
    jnz .lbl180018200
    mov ebp,dword ptr [rsp + 0x80]
    mov rbx,qword ptr [rsp + 0x30]
    .lbl180018229: mov r9d,0x4
    mov r8d,ebp
    mov edx,r12d
    mov rcx,r13
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    mov r9d,0x4
    mov r8d,ebp
    mov edx,r12d
    mov rcx,r14
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    mov rdi,qword ptr [rsp + 0x40]
    mov rsi,qword ptr [rsp + 0x48]
    mov rbx,qword ptr [rsp + 0x88]
    xor eax,eax
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180017a60:
    push rbx
    push rdi
    push r14
    push r15
    sub rsp,0x58
    mov r14,rcx
    mov ecx,r8d
    mov r15,rdx
    call EXTR_log2ceil
    mov ebx,0x1
    xor r9d,r9d
    lea edi,[rax + -0x1]
    mov rdx,r15
    mov ecx,edi
    mov r8d,edi
    shl ebx,cl
    mov rcx,r14
    call EXTR_FUN_180019460
    mov ecx,dword ptr [r14]
    movsxd r11,ebx
    mov dword ptr [r14 + r11*0x4],ecx
    mov ecx,dword ptr [r15]
    xor r8d,r8d
    mov eax,ebx
    mov dword ptr [rsp + 0x98],r8d
    mov dword ptr [r15 + r11*0x4],ecx
    cdq
    sub eax,edx
    sar eax,1
    mov dword ptr [rsp + 0x34],eax
    js .lbl180017c73
    mov qword ptr [rsp + 0x90],rbp
    mov qword ptr [rsp + 0x50],rsi
    mov qword ptr [rsp + 0x48],r12
    mov eax,0x9
    mov qword ptr [rsp + 0x40],r13
    lea r12,[r15 + r11*0x4]
    sub eax,edi
    mov r10d,0x2ff
    lea r9,[glb180174d70]
    mov dword ptr [rsp + 0x3c],eax
    sub r14,r15
    nop
    nop
    .lbl180017b00: mov r13d,dword ptr [r15 + r14*0x1]
    mov esi,dword ptr [r14 + r12*0x1]
    mov ebp,dword ptr [r12]
    mov ecx,eax
    mov edx,r8d
    mov dword ptr [rsp + 0x80],r13d
    shl edx,cl
    mov ecx,dword ptr [r15]
    lea eax,[rdx + 0x100]
    mov dword ptr [rsp + 0x88],ecx
    cmp eax,0x300
    cmovz eax,r10d
    cdqe
    mov ebx,dword ptr [r9 + rax*0x4]
    movsxd rax,edx
    mov edi,dword ptr [r9 + rax*0x4]
    mov edx,ebx
    call EXTR_FUN_180019450
    mov edx,edi
    mov ecx,r13d
    mov dword ptr [rsp + 0x28],eax
    call EXTR_FUN_180019450
    mov edx,edi
    mov ecx,esi
    mov r13d,eax
    call EXTR_FUN_180019450
    mov edx,ebx
    mov ecx,ebp
    mov dword ptr [rsp + 0x24],eax
    call EXTR_FUN_180019450
    mov ecx,dword ptr [rsp + 0x80]
    mov edx,ebx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180019450
    mov ecx,dword ptr [rsp + 0x88]
    mov edx,edi
    mov dword ptr [rsp + 0x30],eax
    call EXTR_FUN_180019450
    mov edx,ebx
    mov ecx,esi
    mov dword ptr [rsp + 0x2c],eax
    call EXTR_FUN_180019450
    mov edx,edi
    mov ecx,ebp
    mov ebx,eax
    call EXTR_FUN_180019450
    mov r10d,dword ptr [rsp + 0x20]
    mov r9d,dword ptr [rsp + 0x24]
    mov edx,dword ptr [rsp + 0x28]
    mov edi,dword ptr [rsp + 0x2c]
    mov r11d,dword ptr [rsp + 0x30]
    mov r8d,dword ptr [rsp + 0x98]
    mov ecx,r10d
    sub ecx,r13d
    mov dword ptr [rsp + 0x38],eax
    add ecx,r9d
    add ecx,edx
    add ecx,esi
    add dword ptr [r15 + r14*0x1],ecx
    mov ecx,ebx
    sub ecx,eax
    mov eax,dword ptr [rsp + 0x34]
    sub ecx,edi
    sub ecx,r11d
    sub ecx,ebp
    add dword ptr [r15],ecx
    sar dword ptr [r15 + r14*0x1],1
    sar dword ptr [r15],1
    cmp r8d,eax
    jz .lbl180017c2e
    sub ebx,dword ptr [rsp + 0x38]
    sub r13d,r10d
    sub r13d,r9d
    sub ebx,edi
    sub r13d,edx
    sub ebx,r11d
    add r13d,dword ptr [rsp + 0x80]
    sub ebx,dword ptr [rsp + 0x88]
    add dword ptr [r14 + r12*0x1],r13d
    add dword ptr [r12],ebx
    sar dword ptr [r14 + r12*0x1],1
    sar dword ptr [r12],1
    .lbl180017c2e: add r8d,0x1
    add r15,0x4
    sub r12,0x4
    cmp r8d,eax
    mov eax,dword ptr [rsp + 0x3c]
    lea r9,[glb180174d70]
    mov dword ptr [rsp + 0x98],r8d
    mov r10d,0x2ff
    jle .lbl180017b00
    mov r13,qword ptr [rsp + 0x40]
    mov r12,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x90]
    .lbl180017c73: xor eax,eax
    add rsp,0x58
    pop r15
    pop r14
    pop rdi
    pop rbx
    ret

section .data

glb180174d70: db 0, 0, 0, 0, -120, 15, -55, 0, 32, 29, -110, 1, -41, 38, 91, 2, -65, 42, 36, 3, -26, 38, -19, 3, 93, 25, -74, 4, 53, 0, 127, 5, 124, -39, 71, 6, 69, -93, 16, 7, -98, 91, -39, 7, -102, 0, -94, 8, 73, -112, 106, 9, -67, 8, 51, 10, 5, 104, -5, 10, 53, -84, -61, 11, 94, -45, -117, 12, -110, -37, 83, 13, -28, -62, 27, 14, 102, -121, -29, 14, 43, 39, -85, 15, 72, -96, 114, 16, -49, -16, 57, 17, -43, 22, 1, 18, 111, 16, -56, 18, -79, -37, -114, 19, -79, 118, 85, 20, -122, -33, 27, 21, 69, 20, -30, 21, 5, 19, -88, 22, -34, -39, 109, 23, -23, 102, 51, 24, 60, -72, -8, 24, -13, -53, -67, 25, 38, -96, -126, 26, -17, 50, 71, 27, 106, -126, 11, 28, -77, -116, -49, 28, -27, 79, -109, 29, 30, -54, 86, 30, 123, -7, 25, 31, 27, -36, -36, 31, 28, 112, -97, 32, -96, -77, 97, 33, -59, -92, 35, 34, -81, 65, -27, 34, 127, -120, -90, 35, 88, 119, 103, 36, 94, 12, 40, 37, -74, 69, -24, 37, -122, 33, -88, 38, -12, -99, 103, 39, 40, -71, 38, 40, 75, 113, -27, 40, -123, -60, -93, 41, 1, -79, 97, 42, -21, 52, 31, 43, 111, 78, -36, 43, -70, -5, -104, 44, -4, 58, 85, 45, 98, 10, 17, 46, 30, 104, -52, 46, 98, 82, -121, 47, 97, -57, 65, 48, 77, -59, -5, 48, 94, 74, -75, 49, -57, 84, 110, 50, -61, -30, 38, 51, -121, -14, -34, 51, 80, -126, -106, 52, 87, -112, 77, 53, -39, 26, 4, 54, 20, 32, -70, 54, 70, -98, 111, 55, -80, -109, 36, 56, -109, -2, -40, 56, 50, -35, -116, 57, -46, 45, 64, 58, -73, -18, -14, 58, 41, 30, -91, 59, 112, -70, 86, 60, -42, -63, 7, 61, -90, 50, -72, 61, 44, 11, 104, 62, -72, 73, 23, 63, -104, -20, -59, 63, 29, -14, 115, 64, -101, 88, 33, 65, 101, 30, -50, 65, -48, 65, 122, 66, 53, -63, 37, 67, -19, -102, -48, 67, 80, -51, 122, 68, -67, 86, 36, 69, -113, 53, -51, 69, 40, 104, 117, 70, -25, -20, 28, 71, 47, -62, -61, 71, 101, -26, 105, 72, -18, 87, 15, 73, 51, 21, -76, 73, -98, 28, 88, 74, -104, 108, -5, 74, -112, 3, -98, 75, -12, -33, 63, 76, 52, 0, -31, 76, -60, 98, -127, 77, 23, 6, 33, 78, -91, -24, -65, 78, -29, 8, 94, 79, 77, 101, -5, 79, 94, -4, -105, 80, -108, -52, 51, 81, 110, -44, -50, 81, 110, 18, 105, 82, 24, -123, 2, 83, -16, 42, -101, 83, 125, 2, 51, 84, 75, 10, -54, 84, -30, 64, 96, 85, -46, -92, -11, 85, -87, 52, -118, 86, -6, -18, 29, 87, 86, -46, -80, 87, 84, -35, 66, 88, -116, 14, -44, 88, -104, 100, 100, 89, 18, -34, -13, 89

section .text
EXTR_FUN_180019450:
    movsxd rax,ecx
    movsxd rcx,edx
    imul rax,rcx
    sar rax,0x1f
    ret

section .data


section .text
EXTR_FUN_180019460:
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    sub rsp,0x98
    test r8d,r8d
    mov eax,r9d
    mov rbp,rdx
    mov rsi,rcx
    jns .lbl180019489
    neg r8d
    .lbl180019489: mov ecx,r8d
    mov edx,0x1
    shl edx,cl
    cmp edx,0x400
    mov dword ptr [rsp + 0xc0],edx
    jle .lbl1800194af
    or eax,0xffffffff
    add rsp,0x98
    pop rsi
    pop rbp
    ret
    .lbl1800194af: mov qword ptr [rsp + 0x90],rbx
    mov qword ptr [rsp + 0x88],rdi
    lea r10d,[rdx + -0x1]
    mov qword ptr [rsp + 0x80],r12
    mov r11d,0x1
    movsxd rbx,r10d
    mov qword ptr [rsp + 0x68],r15
    xor r15d,r15d
    xor r8d,r8d
    cmp rbx,r11
    mov dword ptr [rsp + 0x20],r15d
    jl .lbl18001954a
    mov rdi,rsi
    lea r9,[rbp + 0x4]
    sub rdi,rbp
    .lbl1800194f3: mov ecx,edx
    .lbl1800194f5: sar ecx,1
    lea eax,[rcx + r8*0x1]
    cmp eax,r10d
    jg .lbl1800194f5
    lea eax,[rcx + -0x1]
    and eax,r8d
    lea r8d,[rcx + rax*0x1]
    cmp r8d,r11d
    jle .lbl180019535
    mov ecx,dword ptr [rdi + r9*0x1]
    movsxd rdx,r8d
    mov eax,dword ptr [rsi + rdx*0x4]
    mov dword ptr [rdi + r9*0x1],eax
    mov dword ptr [rsi + rdx*0x4],ecx
    mov eax,dword ptr [rbp + rdx*0x4]
    mov ecx,dword ptr [r9]
    mov dword ptr [r9],eax
    mov dword ptr [rbp + rdx*0x4],ecx
    mov edx,dword ptr [rsp + 0xc0]
    .lbl180019535: add r11d,0x1
    add r9,0x4
    sub rbx,0x1
    jnz .lbl1800194f3
    mov eax,dword ptr [rsp + 0xc8]
    .lbl18001954a: mov r11d,0x1
    mov r12d,0x9
    cmp edx,r11d
    mov dword ptr [rsp + 0x24],r12d
    jle .lbl1800197aa
    mov qword ptr [rsp + 0x78],r13
    mov qword ptr [rsp + 0x70],r14
    lea rbx,[glb180174d70]
    movsxd r14,edx
    .lbl180019578: test eax,eax
    jz .lbl1800195de
    xor r10d,r10d
    xor r8d,r8d
    test r14,r14
    jle .lbl1800195e4
    mov r9,rsi
    mov rdx,rbp
    sub r9,rbp
    .lbl180019590: mov eax,dword ptr [rdx + r9*0x1]
    test eax,eax
    jns .lbl18001959a
    neg eax
    .lbl18001959a: mov ecx,dword ptr [rdx]
    test ecx,ecx
    jns .lbl1800195a2
    neg ecx
    .lbl1800195a2: cmp eax,0x3fffffff
    jg .lbl1800195c7
    cmp ecx,0x3fffffff
    jg .lbl1800195c7
    add r8,0x1
    add rdx,0x4
    cmp r8,r14
    jl .lbl180019590
    mov edx,dword ptr [rsp + 0xc0]
    jmp .lbl1800195e4
    .lbl1800195c7: mov edx,dword ptr [rsp + 0xc0]
    mov r10d,0x1
    add r15d,r10d
    mov dword ptr [rsp + 0x20],r15d
    jmp .lbl1800195e4
    .lbl1800195de: mov r10d,0x1
    .lbl1800195e4: xor r9d,r9d
    xor r13d,r13d
    test r11d,r11d
    lea edi,[r11 + r11*0x1]
    movsxd rcx,r11d
    movsxd r10,r10d
    mov dword ptr [rsp + 0x28],r9d
    mov qword ptr [rsp + 0x48],r13
    mov dword ptr [rsp + 0x2c],edi
    mov qword ptr [rsp + 0x50],rcx
    mov qword ptr [rsp + 0x30],r10
    jle .lbl180019785
    lea r8,[rbp + rcx*0x4]
    mov rdx,rbp
    mov qword ptr [rsp + 0x40],r8
    mov qword ptr [rsp + 0x38],rdx
    nop
    nop
    nop
    .lbl180019630: mov ecx,r12d
    mov eax,r9d
    shl eax,cl
    movsxd rcx,eax
    mov r15d,dword ptr [rbx + rcx*0x4 + 0x400]
    mov ebx,dword ptr [rbx + rcx*0x4]
    neg ebx
    cmp dword ptr [rsp + 0xc8],0x0
    jz .lbl180019654
    neg ebx
    .lbl180019654: test r10,r10
    jz .lbl18001965e
    sar r15d,1
    sar ebx,1
    .lbl18001965e: cmp r13,r14
    mov rsi,r13
    jge .lbl180019734
    movsxd r13,edi
    mov rdi,qword ptr [rsp + 0xb0]
    mov r10,rdx
    sub rdi,rbp
    lea r12,[r13*0x4]
    mov r11,r8
    nop
    nop
    nop
    .lbl180019690: mov r8d,dword ptr [r11 + rdi*0x1]
    mov r9d,dword ptr [r11]
    mov ecx,r15d
    mov edx,r8d
    call EXTR_FUN_180019450
    mov edx,r9d
    mov ecx,ebx
    mov ebp,eax
    call EXTR_FUN_180019450
    mov edx,r8d
    mov ecx,ebx
    sub ebp,eax
    call EXTR_FUN_180019450
    mov edx,r9d
    mov ecx,r15d
    mov r8d,eax
    call EXTR_FUN_180019450
    mov ecx,dword ptr [r10 + rdi*0x1]
    mov edx,dword ptr [r10]
    add r8d,eax
    cmp qword ptr [rsp + 0x30],0x0
    jz .lbl1800196de
    sar ecx,1
    sar edx,1
    .lbl1800196de: mov eax,ecx
    add rsi,r13
    sub eax,ebp
    mov dword ptr [r11 + rdi*0x1],eax
    mov eax,edx
    sub eax,r8d
    mov dword ptr [r11],eax
    lea eax,[rcx + rbp*0x1]
    add r11,r12
    mov dword ptr [r10 + rdi*0x1],eax
    lea eax,[rdx + r8*0x1]
    mov dword ptr [r10],eax
    add r10,r12
    cmp rsi,r14
    jl .lbl180019690
    mov r13,qword ptr [rsp + 0x48]
    mov rdx,qword ptr [rsp + 0x38]
    mov r8,qword ptr [rsp + 0x40]
    mov r9d,dword ptr [rsp + 0x28]
    mov r10,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0xb8]
    mov edi,dword ptr [rsp + 0x2c]
    mov r12d,dword ptr [rsp + 0x24]
    .lbl180019734: add r13,0x1
    add r9d,0x1
    add rdx,0x4
    add r8,0x4
    cmp r13,qword ptr [rsp + 0x50]
    lea rbx,[glb180174d70]
    mov dword ptr [rsp + 0x28],r9d
    mov qword ptr [rsp + 0x48],r13
    mov qword ptr [rsp + 0x38],rdx
    mov qword ptr [rsp + 0x40],r8
    jl .lbl180019630
    mov rsi,qword ptr [rsp + 0xb0]
    mov edx,dword ptr [rsp + 0xc0]
    mov r15d,dword ptr [rsp + 0x20]
    lea rbx,[glb180174d70]
    .lbl180019785: mov eax,dword ptr [rsp + 0xc8]
    sub r12d,0x1
    cmp edi,edx
    mov dword ptr [rsp + 0x24],r12d
    mov r11d,edi
    jl .lbl180019578
    mov r14,qword ptr [rsp + 0x70]
    mov r13,qword ptr [rsp + 0x78]
    .lbl1800197aa: mov r12,qword ptr [rsp + 0x80]
    mov rdi,qword ptr [rsp + 0x88]
    mov rbx,qword ptr [rsp + 0x90]
    mov eax,r15d
    mov r15,qword ptr [rsp + 0x68]
    add rsp,0x98
    pop rsi
    pop rbp
    ret

section .data

glb180175170: db -1, -1, -1, 127

section .text
EXTR_FUN_180017500:
    .lbl180017090: imul ecx,ecx,0x42
    mov eax,ecx
    ret
    .lbl1800174f0: imul ecx,ecx,0x82
    mov eax,ecx
    ret
    cmp ecx,edx
    jnz .lbl180017508
    mov eax,r8d
    ret
    .lbl180017508: cmp ecx,0x40
    jnz .lbl180017527
    cmp edx,0x100
    jz .lbl180017534
    cmp edx,0x81
    jz .lbl180017549
    .lbl18001751d: imul ecx,edx
    imul ecx,r8d
    mov eax,ecx
    ret
    .lbl180017527: cmp ecx,0x100
    jnz .lbl18001753c
    cmp edx,0x40
    jnz .lbl18001751d
    .lbl180017534: mov ecx,r8d
    jmp .lbl180017090
    .lbl18001753c: cmp ecx,0x81
    jnz .lbl18001751d
    cmp edx,0x40
    jnz .lbl18001751d
    .lbl180017549: mov ecx,r8d
    jmp .lbl1800174f0

section .data


section .text
EXTR_FUN_180017560:
    mov qword ptr [rsp + 0x20],rbx
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x40
    cmp edx,r8d
    movsxd rsi,r9d
    mov r12d,r8d
    mov r13d,edx
    mov rdi,rcx
    jnz .lbl18001766a
    test r12d,r12d
    jle .lbl180017733
    mov ecx,r12d
    xor r15d,r15d
    mov ebx,0x1
    mov dword ptr [rsp + 0x30],ecx
    nop
    .lbl1800175b0: cmp ebx,r13d
    jge .lbl18001764c
    lea eax,[rbx + r15*0x1]
    imul eax,esi
    movsxd rbp,eax
    mov eax,r12d
    add rbp,rdi
    imul eax,esi
    cdqe
    mov qword ptr [rsp + 0x38],rax
    lea eax,[rcx + rbx*0x1 + -0x1]
    imul eax,esi
    movsxd r14,eax
    add r14,rdi
    mov edi,r13d
    mov r13,qword ptr [rsp + 0x38]
    sub edi,ebx
    nop
    nop
    .lbl1800175f0: mov rcx,qword ptr [rsp + 0xa0]
    mov r8d,esi
    mov rdx,rbp
    call EXTR_memcpy wrt ..plt
    mov r8d,esi
    mov rdx,r14
    mov rcx,rbp
    call EXTR_memcpy wrt ..plt
    mov rdx,qword ptr [rsp + 0xa0]
    mov r8d,esi
    mov rcx,r14
    call EXTR_memcpy wrt ..plt
    add rbp,rsi
    add r14,r13
    sub rdi,0x1
    jnz .lbl1800175f0
    mov r12d,dword ptr [rsp + 0x90]
    mov r13d,dword ptr [rsp + 0x88]
    mov rdi,qword ptr [rsp + 0x80]
    mov ecx,dword ptr [rsp + 0x30]
    .lbl18001764c: add ebx,0x1
    add ecx,r12d
    add r15d,r13d
    lea eax,[rbx + -0x1]
    mov dword ptr [rsp + 0x30],ecx
    cmp eax,r12d
    jl .lbl1800175b0
    jmp .lbl180017733
    .lbl18001766a: cmp r13d,0x100
    jnz .lbl18001774d
    cmp r12d,0x40
    jz .lbl18001775c
    .lbl180017681: mov rbx,qword ptr [rsp + 0xa0]
    mov r8d,r13d
    mov rdx,rcx
    mov rcx,rbx
    mov qword ptr [rsp + 0x30],rbx
    imul r8d,r12d
    imul r8d,esi
    call EXTR_memcpy wrt ..plt
    test r12d,r12d
    jle .lbl180017733
    mov eax,r13d
    imul eax,esi
    cdqe
    mov qword ptr [rsp + 0xa0],rax
    mov rax,r12
    mov rcx,qword ptr [rsp + 0xa0]
    mov qword ptr [rsp + 0x38],rax
    nop
    .lbl1800176d0: test r13d,r13d
    jle .lbl180017715
    mov eax,r12d
    mov rbp,r13
    imul eax,esi
    movsxd r15,eax
    .lbl1800176e1: mov r8d,esi
    mov rdx,rbx
    mov rcx,rdi
    call EXTR_memcpy wrt ..plt
    add rbx,rsi
    add rdi,r15
    sub rbp,0x1
    jnz .lbl1800176e1
    mov rbx,qword ptr [rsp + 0x30]
    mov rdi,qword ptr [rsp + 0x80]
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,qword ptr [rsp + 0xa0]
    .lbl180017715: add rbx,rcx
    add rdi,rsi
    sub rax,0x1
    mov qword ptr [rsp + 0x38],rax
    mov qword ptr [rsp + 0x30],rbx
    mov qword ptr [rsp + 0x80],rdi
    jnz .lbl1800176d0
    .lbl180017733: xor eax,eax
    mov rbx,qword ptr [rsp + 0x98]
    add rsp,0x40
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret
    .lbl18001774d: cmp r13d,0x40
    jnz .lbl180017782
    cmp r12d,0x100
    jnz .lbl180017779
    .lbl18001775c: mov rax,qword ptr [rsp + 0xa0]
    mov r9d,esi
    mov r8d,r12d
    mov edx,r13d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180016ee0
    jmp .lbl180017733
    .lbl180017779: cmp r12d,0x81
    jmp .lbl180017793
    .lbl180017782: cmp r13d,0x81
    jnz .lbl180017681
    cmp r12d,0x40
    .lbl180017793: jnz .lbl180017681
    mov rax,qword ptr [rsp + 0xa0]
    mov r9d,esi
    mov r8d,r12d
    mov edx,r13d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180017320
    jmp .lbl180017733

section .data


section .text
EXTR_FUN_180016ee0:
    mov rax,rsp
    mov dword ptr [rax + 0x18],r8d
    mov dword ptr [rax + 0x10],edx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x68
    cmp r8d,0x40
    movsxd rdi,r9d
    mov r13,rcx
    mov dword ptr [rax + -0x68],0x1
    mov dword ptr [rax + -0x64],0x2
    mov dword ptr [rax + -0x60],0x3
    mov dword ptr [rax + -0x5c],0x4
    mov dword ptr [rax + -0x58],0x6
    mov dword ptr [rax + -0x54],0x7
    lea ebx,[rdx + 0x1]
    jz .lbl180016f35
    mov ebx,edx
    .lbl180016f35: cmp edx,0x40
    mov dword ptr [rsp + 0xc8],ebx
    jnz .lbl180016f4f
    lea r12d,[r8 + 0x1]
    mov dword ptr [rsp + 0xb0],r12d
    jmp .lbl180016f57
    .lbl180016f4f: mov dword ptr [rsp + 0xb0],r8d
    .lbl180016f57: mov rbp,qword ptr [rsp + 0xd0]
    mov esi,edx
    mov r9d,edi
    lea r14,[rdi + rbp*0x1]
    imul esi,r8d
    lea r15,[rdi + r14*0x1]
    mov qword ptr [rsp + 0x20],r15
    call EXTR_FUN_180016cf0
    xor eax,eax
    mov qword ptr [rsp + 0x38],rax
    .lbl180016f81: mov ecx,dword ptr [rsp + rax*0x4 + 0x40]
    mov eax,ecx
    cdq
    imul ecx,edi
    idiv ebx
    mov r12d,edx
    mov dword ptr [rsp + 0x34],edx
    movsxd rdx,ecx
    mov ebx,eax
    mov dword ptr [rsp + 0x30],eax
    add rdx,r13
    .lbl180016fa0: mov r8d,edi
    mov rcx,rbp
    call EXTR_memcpy wrt ..plt
    imul r12d,dword ptr [rsp + 0xb0]
    add r12d,ebx
    mov eax,r12d
    mov ebx,r12d
    cdq
    idiv dword ptr [rsp + 0xc8]
    cmp ebx,esi
    mov dword ptr [rsp + 0xd0],eax
    mov r12d,edx
    mov eax,ebx
    jge .lbl180016fe0
    imul eax,edi
    movsxd rdx,eax
    add rdx,r13
    jmp .lbl180016feb
    .lbl180016fe0: sub eax,esi
    imul eax,edi
    movsxd rdx,eax
    add rdx,r15
    .lbl180016feb: mov r8d,edi
    mov rcx,r14
    call EXTR_memcpy wrt ..plt
    cmp ebx,esi
    mov r8d,edi
    mov rdx,rbp
    jge .lbl18001700b
    imul ebx,edi
    movsxd rcx,ebx
    add rcx,r13
    jmp .lbl180017016
    .lbl18001700b: sub ebx,esi
    imul ebx,edi
    movsxd rcx,ebx
    add rcx,r15
    .lbl180017016: call EXTR_memcpy wrt ..plt
    mov ebx,dword ptr [rsp + 0xd0]
    cmp dword ptr [rsp + 0x30],ebx
    jnz .lbl18001702f
    cmp dword ptr [rsp + 0x34],r12d
    jz .lbl180017037
    .lbl18001702f: mov rdx,r14
    jmp .lbl180016fa0
    .lbl180017037: mov rax,qword ptr [rsp + 0x38]
    mov ebx,dword ptr [rsp + 0xc8]
    add rax,0x1
    cmp rax,0x6
    mov qword ptr [rsp + 0x38],rax
    jl .lbl180016f81
    mov r8d,dword ptr [rsp + 0xb8]
    mov edx,dword ptr [rsp + 0xc0]
    mov r9d,edi
    mov rcx,r13
    mov qword ptr [rsp + 0x20],r15
    call EXTR_FUN_180016e40
    xor eax,eax
    add rsp,0x68
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180016cf0:
    mov dword ptr [rsp + 0x18],r8d
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r14
    sub rsp,0x28
    mov r14,rcx
    mov esi,edx
    lea ecx,[rdx + 0x1]
    movsxd rbx,r9d
    mov r12d,edx
    mov dword ptr [rsp + 0x68],ecx
    imul esi,r8d
    cmp edx,0x40
    jnz .lbl180016d53
    mov rdi,qword ptr [rsp + 0x80]
    mov esi,edx
    nop
    nop
    nop
    .lbl180016d30: mov r8d,ebx
    xor edx,edx
    mov rcx,rdi
    call EXTR_memset wrt ..plt
    add rdi,rbx
    sub rsi,0x1
    jnz .lbl180016d30
    add rsp,0x28
    pop r14
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret
    .lbl180016d53: lea edi,[rsi + -0x1]
    mov qword ptr [rsp + 0x60],r13
    mov qword ptr [rsp + 0x20],r15
    test edi,edi
    mov r15,qword ptr [rsp + 0x80]
    js .lbl180016dbe
    mov eax,ebx
    neg eax
    movsxd r13,eax
    mov eax,edi
    imul eax,ebx
    movsxd rbp,eax
    add rbp,r14
    nop
    .lbl180016d80: mov eax,edi
    cdq
    idiv r12d
    add eax,edi
    cmp eax,esi
    jl .lbl180016d99
    sub eax,esi
    imul eax,ebx
    movsxd rcx,eax
    add rcx,r15
    jmp .lbl180016da2
    .lbl180016d99: imul eax,ebx
    movsxd rcx,eax
    add rcx,r14
    .lbl180016da2: mov r8d,ebx
    mov rdx,rbp
    call EXTR_memcpy wrt ..plt
    add rbp,r13
    sub edi,0x1
    jns .lbl180016d80
    mov ecx,dword ptr [rsp + 0x68]
    mov r8d,dword ptr [rsp + 0x70]
    .lbl180016dbe: test r8d,r8d
    jle .lbl180016e23
    mov eax,ecx
    lea r12d,[rcx + -0x1]
    imul eax,ebx
    movsxd r13,eax
    mov eax,r12d
    imul eax,ebx
    movsxd rdi,eax
    mov eax,ecx
    sub eax,esi
    add rdi,r14
    mov r14d,r8d
    sub eax,0x1
    imul eax,ebx
    movsxd rbp,eax
    add rbp,r15
    mov r15d,dword ptr [rsp + 0x68]
    nop
    nop
    nop
    nop
    .lbl180016e00: mov rcx,rdi
    cmp r12d,esi
    mov r8d,ebx
    cmovge rcx,rbp
    xor edx,edx
    call EXTR_memset wrt ..plt
    add rbp,r13
    add rdi,r13
    add r12d,r15d
    sub r14,0x1
    jnz .lbl180016e00
    .lbl180016e23: mov r13,qword ptr [rsp + 0x60]
    mov r15,qword ptr [rsp + 0x20]
    add rsp,0x28
    pop r14
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180016e40:
    cmp edx,0x40
    jz .lbl180016ed9
    push rbx
    push rsi
    push r12
    push r14
    sub rsp,0x28
    mov ebx,edx
    mov qword ptr [rsp + 0x58],rdi
    xor edi,edi
    movsxd rsi,r9d
    mov r14d,edx
    mov r12,rcx
    imul ebx,r8d
    test ebx,ebx
    jle .lbl180016eca
    mov qword ptr [rsp + 0x50],rbp
    mov qword ptr [rsp + 0x60],r13
    mov qword ptr [rsp + 0x20],r15
    mov r15,qword ptr [rsp + 0x70]
    mov rbp,rcx
    .lbl180016e84: mov eax,edi
    cdq
    idiv r14d
    add eax,edi
    cmp eax,ebx
    jl .lbl180016e9d
    sub eax,ebx
    imul eax,esi
    movsxd rdx,eax
    add rdx,r15
    jmp .lbl180016ea6
    .lbl180016e9d: imul eax,esi
    movsxd rdx,eax
    add rdx,r12
    .lbl180016ea6: mov r8d,esi
    mov rcx,rbp
    call EXTR_memcpy wrt ..plt
    add edi,0x1
    add rbp,rsi
    cmp edi,ebx
    jl .lbl180016e84
    mov r15,qword ptr [rsp + 0x20]
    mov r13,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x50]
    .lbl180016eca: mov rdi,qword ptr [rsp + 0x58]
    add rsp,0x28
    pop r14
    pop r12
    pop rsi
    pop rbx
    .lbl180016ed9: ret 0x0

section .data


section .text
EXTR_FUN_180017320:
    mov rax,rsp
    mov dword ptr [rax + 0x18],r8d
    mov dword ptr [rax + 0x10],edx
    mov qword ptr [rax + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x68
    cmp r8d,0x40
    movsxd rdi,r9d
    mov dword ptr [rax + -0x68],0x1
    mov dword ptr [rax + -0x64],0x2
    mov dword ptr [rax + -0x60],0x3
    mov dword ptr [rax + -0x5c],0x5
    mov dword ptr [rax + -0x58],0x53
    mov dword ptr [rax + -0x54],0x65
    mov dword ptr [rax + -0x50],0xa6
    mov dword ptr [rax + -0x4c],0xca
    lea esi,[rdx + 0x2]
    jz .lbl180017384
    mov esi,edx
    .lbl180017384: cmp edx,0x40
    mov dword ptr [rsp + 0xc8],esi
    jnz .lbl18001739b
    lea r13d,[r8 + 0x2]
    mov dword ptr [rsp + 0x30],r13d
    jmp .lbl1800173a0
    .lbl18001739b: mov dword ptr [rsp + 0x30],r8d
    .lbl1800173a0: mov rbp,qword ptr [rsp + 0xd0]
    mov ebx,edx
    mov r9d,edi
    lea r12,[rdi + rbp*0x1]
    imul ebx,r8d
    mov dword ptr [rsp + 0x34],ebx
    lea rax,[r12 + rdi*0x1]
    mov qword ptr [rsp + 0x38],rax
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_1800170a0
    xor eax,eax
    mov qword ptr [rsp + 0xd0],rax
    nop
    nop
    nop
    .lbl1800173e0: mov ecx,dword ptr [rsp + rax*0x4 + 0x40]
    mov r8d,edi
    mov eax,ecx
    cdq
    imul ecx,edi
    idiv esi
    mov esi,edx
    mov r15d,edx
    movsxd rdx,ecx
    add rdx,qword ptr [rsp + 0xb0]
    mov rcx,rbp
    mov r13d,eax
    mov r14d,eax
    call EXTR_memcpy wrt ..plt
    nop
    .lbl180017410: imul esi,dword ptr [rsp + 0x30]
    mov r8d,edi
    add esi,r13d
    mov eax,esi
    mov ecx,esi
    cdq
    idiv dword ptr [rsp + 0xc8]
    cmp ecx,ebx
    mov esi,edx
    mov r13d,eax
    jge .lbl180017440
    imul ecx,edi
    movsxd rbx,ecx
    mov rcx,qword ptr [rsp + 0xb0]
    jmp .lbl18001744d
    .lbl180017440: sub ecx,ebx
    imul ecx,edi
    movsxd rbx,ecx
    mov rcx,qword ptr [rsp + 0x38]
    .lbl18001744d: add rbx,rcx
    mov rcx,r12
    mov rdx,rbx
    call EXTR_memcpy wrt ..plt
    mov r8d,edi
    mov rdx,rbp
    mov rcx,rbx
    call EXTR_memcpy wrt ..plt
    cmp r14d,r13d
    jnz .lbl180017473
    cmp r15d,esi
    jz .lbl180017487
    .lbl180017473: mov r8d,edi
    mov rdx,r12
    mov rcx,rbp
    call EXTR_memcpy wrt ..plt
    mov ebx,dword ptr [rsp + 0x34]
    jmp .lbl180017410
    .lbl180017487: mov rax,qword ptr [rsp + 0xd0]
    mov ebx,dword ptr [rsp + 0x34]
    mov esi,dword ptr [rsp + 0xc8]
    add rax,0x1
    cmp rax,0x8
    mov qword ptr [rsp + 0xd0],rax
    jl .lbl1800173e0
    mov rax,qword ptr [rsp + 0x38]
    mov r8d,dword ptr [rsp + 0xb8]
    mov edx,dword ptr [rsp + 0xc0]
    mov rcx,qword ptr [rsp + 0xb0]
    mov r9d,edi
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180017280
    xor eax,eax
    add rsp,0x68
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_1800170a0:
    mov dword ptr [rsp + 0x18],r8d
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    sub rsp,0x38
    mov r13d,edx
    lea r10d,[rdx + 0x2]
    movsxd rbx,r9d
    mov r9d,r8d
    mov ebp,edx
    mov r12,rcx
    imul r13d,r8d
    cmp edx,0x40
    mov dword ptr [rsp + 0x78],r10d
    jnz .lbl180017103
    mov rdi,qword ptr [rsp + 0x90]
    lea esi,[rdx + 0x40]
    nop
    .lbl1800170e0: mov r8d,ebx
    xor edx,edx
    mov rcx,rdi
    call EXTR_memset wrt ..plt
    add rdi,rbx
    sub rsi,0x1
    jnz .lbl1800170e0
    add rsp,0x38
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret
    .lbl180017103: mov r8,qword ptr [rsp + 0x90]
    lea edi,[r13 + -0x1]
    mov qword ptr [rsp + 0x88],r14
    test edi,edi
    js .lbl18001717c
    mov eax,ebx
    neg eax
    movsxd r14,eax
    mov eax,edi
    imul eax,ebx
    movsxd rsi,eax
    add rsi,rcx
    nop
    .lbl180017130: mov eax,edi
    cdq
    idiv ebp
    lea ecx,[rdi + rax*0x2]
    cmp ecx,r13d
    jl .lbl18001714b
    sub ecx,r13d
    imul ecx,ebx
    movsxd rcx,ecx
    add rcx,r8
    jmp .lbl180017154
    .lbl18001714b: imul ecx,ebx
    movsxd rcx,ecx
    add rcx,r12
    .lbl180017154: mov r8d,ebx
    mov rdx,rsi
    call EXTR_memcpy wrt ..plt
    mov r8,qword ptr [rsp + 0x90]
    add rsi,r14
    sub edi,0x1
    jns .lbl180017130
    mov r9d,dword ptr [rsp + 0x80]
    mov r10d,dword ptr [rsp + 0x78]
    .lbl18001717c: test r9d,r9d
    jle .lbl18001725d
    mov eax,ebx
    mov qword ptr [rsp + 0x30],r15
    neg eax
    movsxd rcx,eax
    mov eax,r10d
    mov qword ptr [rsp + 0x20],rcx
    lea ecx,[r10 + -0x1]
    imul eax,ebx
    movsxd rdx,eax
    mov eax,ecx
    mov dword ptr [rsp + 0x80],ecx
    mov qword ptr [rsp + 0x28],rdx
    imul eax,ebx
    movsxd r14,eax
    mov eax,r10d
    sub eax,r13d
    add r14,r12
    sub eax,0x1
    mov qword ptr [rsp + 0x90],r14
    imul eax,ebx
    movsxd r15,eax
    mov eax,r9d
    mov qword ptr [rsp + 0x70],rax
    add r15,r8
    nop
    .lbl1800171e0: mov rsi,r14
    mov r14,qword ptr [rsp + 0x20]
    mov edi,ecx
    mov rbp,r15
    mov r12d,0x2
    nop
    nop
    nop
    nop
    .lbl180017200: mov rcx,rsi
    cmp edi,r13d
    mov r8d,ebx
    cmovge rcx,rbp
    xor edx,edx
    call EXTR_memset wrt ..plt
    add rbp,r14
    add rsi,r14
    sub edi,0x1
    sub r12,0x1
    jnz .lbl180017200
    mov ecx,dword ptr [rsp + 0x80]
    mov r14,qword ptr [rsp + 0x90]
    mov rdx,qword ptr [rsp + 0x28]
    add ecx,dword ptr [rsp + 0x78]
    add r14,rdx
    add r15,rdx
    sub qword ptr [rsp + 0x70],0x1
    mov dword ptr [rsp + 0x80],ecx
    mov qword ptr [rsp + 0x90],r14
    jnz .lbl1800171e0
    mov r15,qword ptr [rsp + 0x30]
    .lbl18001725d: mov r14,qword ptr [rsp + 0x88]
    add rsp,0x38
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180017280:
    cmp edx,0x40
    jz .lbl18001731a
    push rbx
    push rsi
    push r12
    push r14
    sub rsp,0x28
    mov ebx,edx
    mov qword ptr [rsp + 0x58],rdi
    xor edi,edi
    movsxd rsi,r9d
    mov r14d,edx
    mov r12,rcx
    imul ebx,r8d
    test ebx,ebx
    jle .lbl18001730b
    mov qword ptr [rsp + 0x50],rbp
    mov qword ptr [rsp + 0x60],r13
    mov qword ptr [rsp + 0x20],r15
    mov r15,qword ptr [rsp + 0x70]
    mov rbp,rcx
    .lbl1800172c4: mov eax,edi
    cdq
    idiv r14d
    lea ecx,[rdi + rax*0x2]
    cmp ecx,ebx
    jl .lbl1800172de
    sub ecx,ebx
    imul ecx,esi
    movsxd rdx,ecx
    add rdx,r15
    jmp .lbl1800172e7
    .lbl1800172de: imul ecx,esi
    movsxd rdx,ecx
    add rdx,r12
    .lbl1800172e7: mov r8d,esi
    mov rcx,rbp
    call EXTR_memcpy wrt ..plt
    add edi,0x1
    add rbp,rsi
    cmp edi,ebx
    jl .lbl1800172c4
    mov r15,qword ptr [rsp + 0x20]
    mov r13,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x50]
    .lbl18001730b: mov rdi,qword ptr [rsp + 0x58]
    add rsp,0x28
    pop r14
    pop r12
    pop rsi
    pop rbx
    .lbl18001731a: ret 0x0

section .data


section .text
EXTR_FUN_180017800:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    xor ebx,ebx
    test edx,edx
    mov ebp,r8d
    mov rdi,rcx
    movsxd rsi,edx
    mov ecx,ebx
    mov r8,rbx
    jle .lbl18001785d
    nop
    nop
    .lbl180017830: mov edx,dword ptr [rdi + r8*0x4]
    mov eax,edx
    neg eax
    cmp edx,eax
    mov eax,edx
    jg .lbl180017840
    neg eax
    .lbl180017840: cmp ecx,eax
    jge .lbl180017854
    mov ecx,dword ptr [rdi + r8*0x4]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    mov ecx,edx
    jg .lbl180017854
    neg ecx
    .lbl180017854: add r8,0x1
    cmp r8,rsi
    jl .lbl180017830
    .lbl18001785d: call EXTR_log2ceil
    sub ebp,eax
    jns .lbl18001786a
    xor eax,eax
    jmp .lbl180017881
    .lbl18001786a: test rsi,rsi
    jle .lbl18001787f
    nop
    .lbl180017870: mov ecx,ebp
    add rbx,0x1
    shl dword ptr [rdi + rbx*0x4 + -0x4],cl
    cmp rbx,rsi
    jl .lbl180017870
    .lbl18001787f: mov eax,ebp
    .lbl180017881: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180019010:
    mov qword ptr [rsp + 0x18],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r15
    sub rsp,0x30
    mov rsi,rcx
    movsxd r15,r8d
    mov r12d,r9d
    mov ecx,r15d
    mov rbx,rdx
    call EXTR_round_pow2
    mov ecx,r9d
    sar eax,1
    lea ebp,[rax + 0x1]
    call EXTR_round_pow2
    mov r8d,r9d
    mov edi,eax
    mov edx,r15d
    mov rcx,rbx
    imul edi,ebp
    call EXTR_FUN_1800182d0
    test r12d,r12d
    jle .lbl1800190b1
    mov qword ptr [rsp + 0x60],r13
    mov r13d,dword ptr [rsp + 0x80]
    mov qword ptr [rsp + 0x68],r14
    sub r13d,eax
    mov r14,r15
    movsxd rax,edi
    movsxd rbp,ebp
    add r14,r14
    lea rdi,[rbx + rax*0x4]
    shl rbp,0x2
    .lbl180019082: mov r9d,r15d
    mov r8,rdi
    mov rdx,rbx
    mov rcx,rsi
    mov dword ptr [rsp + 0x20],r13d
    call EXTR_FUN_180018030
    add rdi,rbp
    add rbx,rbp
    add rsi,r14
    sub r12,0x1
    jnz .lbl180019082
    mov r14,qword ptr [rsp + 0x68]
    mov r13,qword ptr [rsp + 0x60]
    .lbl1800190b1: xor eax,eax
    mov rbx,qword ptr [rsp + 0x70]
    add rsp,0x30
    pop r15
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_1800182d0:
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x78
    mov rbp,rcx
    mov ecx,edx
    call EXTR_round_pow2
    mov ecx,r8d
    mov r15d,eax
    sar r15d,1
    call EXTR_log2ceil
    add r15d,0x1
    mov ecx,eax
    mov r10d,0x1
    xor edi,edi
    shl r10d,cl
    lea r8d,[rdi + 0x4]
    mov ebx,r15d
    mov edx,r10d
    mov ecx,r15d
    mov dword ptr [rsp + 0x30],eax
    imul ebx,r10d
    mov dword ptr [rsp + 0xd8],r10d
    mov dword ptr [rsp + 0xc0],r15d
    call EXTR_FUN_180017500
    movsxd r14,r10d
    cdqe
    lea ecx,[rbx + rbx*0x1]
    movsxd rdx,ecx
    lea rsi,[r14 + rax*0x1]
    mov r9d,r8d
    lea rcx,[rbp + rdx*0x4]
    mov r8d,r10d
    mov edx,r15d
    lea r13,[rax + rcx*0x1]
    add rsi,rcx
    mov qword ptr [rsp + 0x38],rcx
    mov qword ptr [rsp + 0x20],rcx
    mov rcx,rbp
    mov qword ptr [rsp + 0x58],r14
    mov qword ptr [rsp + 0x50],r13
    mov qword ptr [rsp + 0x40],rsi
    call EXTR_FUN_180017560
    mov r8d,dword ptr [rsp + 0xd8]
    movsxd rax,ebx
    lea r9d,[rdi + 0x4]
    lea r12,[rbp + rax*0x4]
    mov qword ptr [rsp + 0x60],rax
    mov rax,qword ptr [rsp + 0x38]
    mov rcx,r12
    mov edx,r15d
    mov qword ptr [rsp + 0x48],r12
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180017560
    test r15d,r15d
    jle .lbl180018495
    mov rbx,rsi
    mov esi,dword ptr [rsp + 0x30]
    lea r13,[r14*0x4]
    mov r15,r12
    mov r12d,dword ptr [rsp + 0xc0]
    mov r14,rbp
    nop
    nop
    .lbl1800183e0: mov r9d,0x1
    mov r8d,esi
    mov rdx,r15
    mov rcx,r14
    call EXTR_FUN_180019460
    cmp edi,eax
    mov dword ptr [rbx],eax
    cmovle edi,eax
    add r15,r13
    add r14,r13
    add rbx,0x4
    sub r12,0x1
    jnz .lbl1800183e0
    mov r15d,dword ptr [rsp + 0xc0]
    mov rsi,qword ptr [rsp + 0x40]
    mov r12,qword ptr [rsp + 0x48]
    test r15d,r15d
    mov r13,qword ptr [rsp + 0x50]
    mov r14,qword ptr [rsp + 0x58]
    jle .lbl180018495
    mov rax,qword ptr [rsp + 0x60]
    lea rbx,[r14*0x4]
    mov r10,rbp
    lea r11,[rbp + rax*0x4]
    .lbl180018441: cmp dword ptr [rsi],edi
    jge .lbl18001847d
    test r14,r14
    jle .lbl18001847d
    mov rdx,r11
    mov r8,r10
    mov r9,r14
    nop
    nop
    nop
    nop
    .lbl180018460: mov ecx,edi
    add rdx,0x4
    add r8,0x4
    sub ecx,dword ptr [rsi]
    sar dword ptr [r8 + -0x4],cl
    mov ecx,edi
    sub ecx,dword ptr [rsi]
    sar dword ptr [rdx + -0x4],cl
    sub r9,0x1
    jnz .lbl180018460
    .lbl18001847d: add rsi,0x4
    add r11,rbx
    add r10,rbx
    sub r15,0x1
    jnz .lbl180018441
    mov r15d,dword ptr [rsp + 0xc0]
    .lbl180018495: mov ebx,dword ptr [rsp + 0xd8]
    mov rsi,qword ptr [rsp + 0x38]
    mov r9d,0x4
    mov r8d,r15d
    mov rcx,rbp
    mov edx,ebx
    mov qword ptr [rsp + 0x20],rsi
    call EXTR_FUN_180017560
    mov r9d,0x4
    mov r8d,r15d
    mov edx,ebx
    mov rcx,r12
    mov qword ptr [rsp + 0x20],rsi
    call EXTR_FUN_180017560
    mov r11d,dword ptr [rsp + 0xd0]
    test r11d,r11d
    jle .lbl180018517
    mov r14d,dword ptr [rsp + 0xc8]
    movsxd rbx,r15d
    mov rsi,r11
    shl rbx,0x2
    .lbl1800184f0: mov r8d,r14d
    mov rdx,r12
    mov rcx,rbp
    call EXTR_FUN_180017de0
    cmp edi,eax
    mov dword ptr [r13],eax
    cmovle edi,eax
    add r12,rbx
    add rbp,rbx
    add r13,0x4
    sub rsi,0x1
    jnz .lbl1800184f0
    .lbl180018517: mov eax,edi
    add rsp,0x78
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180017de0:
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rdi
    sub rsp,0x78
    mov rdi,rcx
    mov ecx,r8d
    mov rbx,rdx
    call EXTR_log2ceil
    xor r9d,r9d
    mov r8d,0x1
    lea ecx,[rax + -0x1]
    mov dword ptr [rsp + 0x20],r9d
    shl r8d,cl
    mov dword ptr [rsp + 0x40],ecx
    mov eax,r8d
    cdq
    sub eax,edx
    movsxd rdx,r8d
    sar eax,1
    mov dword ptr [rsp + 0x24],eax
    js .lbl18001800a
    mov qword ptr [rsp + 0xa0],rbp
    mov qword ptr [rsp + 0x70],rsi
    mov qword ptr [rsp + 0x68],r12
    mov qword ptr [rsp + 0x60],r13
    mov eax,0x9
    mov qword ptr [rsp + 0x58],r14
    mov qword ptr [rsp + 0x50],r15
    sub eax,ecx
    mov r15,rdi
    mov r8,rbx
    mov qword ptr [rsp + 0x48],rbx
    lea r12,[rbx + rdx*0x4]
    sub r15,rbx
    mov dword ptr [rsp + 0x38],eax
    mov r11d,0x2ff
    lea r10,[glb180174d70]
    nop
    nop
    nop
    .lbl180017e80: mov r14d,dword ptr [r8 + r15*0x1]
    mov esi,dword ptr [r15 + r12*0x1]
    mov ebp,dword ptr [r12]
    mov ecx,eax
    mov edx,r9d
    mov dword ptr [rsp + 0x28],r14d
    shl edx,cl
    mov ecx,dword ptr [r8]
    lea eax,[rdx + 0x100]
    mov dword ptr [rsp + 0xa8],ecx
    cmp eax,0x300
    cmovz eax,r11d
    cdqe
    mov ebx,dword ptr [r10 + rax*0x4]
    movsxd rax,edx
    mov edi,dword ptr [r10 + rax*0x4]
    mov edx,ebx
    call EXTR_FUN_180019450
    mov edx,edi
    mov ecx,r14d
    mov dword ptr [rsp + 0x3c],eax
    call EXTR_FUN_180019450
    mov edx,edi
    mov ecx,esi
    mov dword ptr [rsp + 0x2c],eax
    call EXTR_FUN_180019450
    mov edx,ebx
    mov ecx,ebp
    mov dword ptr [rsp + 0x44],eax
    call EXTR_FUN_180019450
    mov edx,ebx
    mov ecx,r14d
    mov r13d,eax
    call EXTR_FUN_180019450
    mov ecx,dword ptr [rsp + 0xa8]
    mov edx,edi
    mov r14d,eax
    call EXTR_FUN_180019450
    mov edx,ebx
    mov ecx,esi
    mov dword ptr [rsp + 0x34],eax
    call EXTR_FUN_180019450
    mov edx,edi
    mov ecx,ebp
    mov ebx,eax
    mov dword ptr [rsp + 0x30],eax
    call EXTR_FUN_180019450
    mov r8,qword ptr [rsp + 0x48]
    mov r11d,dword ptr [rsp + 0x44]
    mov r10d,dword ptr [rsp + 0x2c]
    mov edx,dword ptr [rsp + 0x3c]
    mov r9d,dword ptr [rsp + 0x20]
    mov ecx,r11d
    sub ecx,r13d
    mov edi,eax
    sub ecx,r10d
    sub ecx,edx
    add ecx,esi
    add dword ptr [r8 + r15*0x1],ecx
    mov ecx,r14d
    sub ecx,eax
    mov eax,dword ptr [rsp + 0x24]
    sub ecx,ebx
    mov ebx,dword ptr [rsp + 0x34]
    sub ecx,ebx
    sub ecx,ebp
    add dword ptr [r8],ecx
    sar dword ptr [r8 + r15*0x1],1
    sar dword ptr [r8],1
    cmp r9d,eax
    jz .lbl180017fa5
    sub r14d,edi
    sub r13d,r11d
    sub r14d,dword ptr [rsp + 0x30]
    add r13d,r10d
    add r13d,edx
    sub r14d,ebx
    add r13d,dword ptr [rsp + 0x28]
    sub r14d,dword ptr [rsp + 0xa8]
    add dword ptr [r15 + r12*0x1],r13d
    add dword ptr [r12],r14d
    sar dword ptr [r15 + r12*0x1],1
    sar dword ptr [r12],1
    .lbl180017fa5: add r9d,0x1
    add r8,0x4
    sub r12,0x4
    cmp r9d,eax
    mov eax,dword ptr [rsp + 0x38]
    lea r10,[glb180174d70]
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x48],r8
    mov r11d,0x2ff
    jle .lbl180017e80
    mov ecx,dword ptr [rsp + 0x40]
    mov rbx,qword ptr [rsp + 0x98]
    mov rdi,qword ptr [rsp + 0x90]
    mov r15,qword ptr [rsp + 0x50]
    mov r14,qword ptr [rsp + 0x58]
    mov r13,qword ptr [rsp + 0x60]
    mov r12,qword ptr [rsp + 0x68]
    mov rsi,qword ptr [rsp + 0x70]
    mov rbp,qword ptr [rsp + 0xa0]
    .lbl18001800a: mov r8d,ecx
    mov r9d,0x1
    mov rdx,rbx
    mov rcx,rdi
    add rsp,0x78
    pop rdi
    pop rbx
    jmp EXTR_FUN_180019460

section .data


section .text
EXTR_FUN_180018030:
    mov qword ptr [rsp + 0x10],rbx
    push rdi
    mov eax,r9d
    mov rdi,rdx
    mov rbx,r8
    cdq
    mov r10,rcx
    sub eax,edx
    sar eax,1
    js .lbl1800180c6
    sub rdi,rcx
    xor r11d,r11d
    mov qword ptr [rsp + 0x10],rsi
    sub rbx,rcx
    lea rdx,[rcx + 0x2]
    mov ecx,dword ptr [rsp + 0x30]
    lea r8d,[rax + 0x1]
    mov esi,0xffff8000
    .lbl180018068: cmp r11d,r9d
    jge .lbl18001808a
    mov eax,dword ptr [rdi + r10*0x1]
    sar eax,cl
    cmp eax,0x7fff
    jle .lbl180018081
    mov eax,0x7fff
    jmp .lbl180018086
    .lbl180018081: cmp eax,esi
    cmovl eax,esi
    .lbl180018086: mov word ptr [r10],ax
    .lbl18001808a: lea eax,[r11 + 0x1]
    cmp eax,r9d
    jge .lbl1800180af
    mov eax,dword ptr [rbx + r10*0x1]
    sar eax,cl
    cmp eax,0x7fff
    jle .lbl1800180a7
    mov eax,0x7fff
    jmp .lbl1800180ac
    .lbl1800180a7: cmp eax,esi
    cmovl eax,esi
    .lbl1800180ac: mov word ptr [rdx],ax
    .lbl1800180af: add r11d,0x2
    add r10,0x4
    add rdx,0x4
    sub r8,0x1
    jnz .lbl180018068
    mov rsi,qword ptr [rsp + 0x10]
    .lbl1800180c6: xor eax,eax
    mov rbx,qword ptr [rsp + 0x18]
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180016b80:
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x20],rsi
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x8],rcx
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x40
    mov ecx,r8d
    mov r14,r9
    mov ebx,r8d
    mov r15d,edx
    call EXTR_round_pow2
    mov ecx,r15d
    mov r13d,eax
    call EXTR_round_pow2
    mov esi,dword ptr [rsp + 0xa0]
    mov rdi,qword ptr [rsp + 0x98]
    mov edx,esi
    mov r12d,eax
    mov rcx,rdi
    call EXTR_max_log2ceil
    mov rdx,qword ptr [rsp + 0xa8]
    mov rcx,qword ptr [rsp + 0x70]
    mov r9d,r15d
    mov r8d,ebx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180018a60
    mov ecx,dword ptr [rsp + 0x90]
    mov dword ptr [rsp + 0x30],esi
    mov qword ptr [rsp + 0x28],rdi
    mov dword ptr [rsp + 0x20],ecx
    mov rcx,qword ptr [rsp + 0xa8]
    mov r9,r14
    mov r8d,r13d
    mov edx,r12d
    mov ebx,eax
    call EXTR_FUN_180016340
    mov r8d,dword ptr [rsp + 0x80]
    mov rdx,qword ptr [rsp + 0xa8]
    mov rcx,qword ptr [rsp + 0x70]
    mov r9d,r15d
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180018d00
    mov rbx,qword ptr [rsp + 0x78]
    mov rsi,qword ptr [rsp + 0x88]
    xor eax,eax
    add rsp,0x40
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180018a60:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov dword ptr [rsp + 0x20],r9d
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    movsxd r14,r9d
    mov rbx,rcx
    movsxd rbp,r8d
    mov ecx,r14d
    mov rdi,rdx
    call EXTR_log2ceil
    mov ecx,ebp
    mov esi,eax
    mov dword ptr [rsp + 0x38],eax
    call EXTR_log2ceil
    mov edx,eax
    mov ecx,esi
    mov dword ptr [rsp + 0x3c],eax
    mov esi,0x1
    mov eax,esi
    shl eax,cl
    mov ecx,edx
    xor edx,edx
    shl esi,cl
    mov dword ptr [rsp + 0x30],eax
    mov ecx,esi
    imul ecx,eax
    lea eax,[rcx + rcx*0x1]
    mov rcx,rdi
    lea r8d,[rax*0x4]
    mov dword ptr [rsp + 0x34],eax
    call EXTR_memset wrt ..plt
    xor r13d,r13d
    test r14d,r14d
    movsxd r9,esi
    mov r12,rbp
    mov r15,r14
    mov qword ptr [rsp + 0x40],r14
    jle .lbl180018bec
    sub esi,ebp
    lea r14,[r12 + r12*0x1]
    lea r8,[r9*0x4]
    mov eax,esi
    mov r13,rdi
    lea rsi,[r14 + rbx*0x1 + -0x2]
    cdq
    sub eax,edx
    sar eax,1
    lea edx,[rbp + rax*0x1]
    mov rax,r15
    xor ebp,ebp
    mov dword ptr [rsp + 0x20],edx
    mov dword ptr [rsp + 0x28],eax
    .lbl180018b21: xor r11d,r11d
    xor r10d,r10d
    test r12,r12
    jle .lbl180018b60
    mov rcx,r13
    mov rdx,rbx
    mov r8,r12
    mov r11d,r12d
    mov r10,r12
    nop
    nop
    .lbl180018b40: movsx eax,word ptr [rdx]
    add rcx,0x4
    add rdx,0x2
    sub r8,0x1
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180018b40
    mov edx,dword ptr [rsp + 0x20]
    lea r8,[r9*0x4]
    .lbl180018b60: cmp r11d,edx
    jge .lbl180018b90
    lea rax,[r10 + rbp*0x1]
    sub edx,r11d
    lea rcx,[rdi + rax*0x4]
    add r10,rdx
    nop
    nop
    nop
    nop
    .lbl180018b80: movsx eax,word ptr [rsi]
    add rcx,0x4
    sub rdx,0x1
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180018b80
    .lbl180018b90: cmp r10,r9
    jge .lbl180018bc0
    lea rax,[r10 + rbp*0x1]
    mov rdx,r9
    lea rcx,[rdi + rax*0x4]
    sub rdx,r10
    nop
    nop
    nop
    nop
    .lbl180018bb0: movsx eax,word ptr [rbx]
    add rcx,0x4
    sub rdx,0x1
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180018bb0
    .lbl180018bc0: mov edx,dword ptr [rsp + 0x20]
    add rsi,r14
    add rbx,r14
    add rbp,r9
    add r13,r8
    sub r15,0x1
    jnz .lbl180018b21
    mov r13d,dword ptr [rsp + 0x28]
    mov r14d,dword ptr [rsp + 0x98]
    mov r15,qword ptr [rsp + 0x40]
    .lbl180018bec: movsxd rsi,dword ptr [rsp + 0x30]
    movsxd rcx,r13d
    mov eax,esi
    sub eax,r14d
    cdq
    sub eax,edx
    sar eax,1
    add eax,r14d
    cmp r13d,eax
    jge .lbl180018c5c
    imul rcx,r9
    sub eax,r13d
    lea rbx,[r9*0x4]
    mov r11d,eax
    add r13d,eax
    lea r10,[rdi + rcx*0x4]
    nop
    .lbl180018c20: test r9,r9
    jle .lbl180018c53
    lea rax,[r15 + -0x1]
    mov rcx,r10
    mov r8,r9
    imul rax,r9
    lea rdx,[rdi + rax*0x4]
    nop
    nop
    nop
    .lbl180018c40: mov eax,dword ptr [rdx]
    add rcx,0x4
    add rdx,0x4
    sub r8,0x1
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180018c40
    .lbl180018c53: add r10,rbx
    sub r11,0x1
    jnz .lbl180018c20
    .lbl180018c5c: movsxd rcx,r13d
    mov r11,rsi
    cmp rcx,rsi
    jge .lbl180018cad
    mov rax,rcx
    lea rdx,[r9*0x4]
    imul rax,r9
    sub r11,rcx
    lea r10,[rdi + rax*0x4]
    nop
    .lbl180018c80: xor ecx,ecx
    test r9,r9
    jle .lbl180018ca4
    mov r8,r10
    nop
    nop
    .lbl180018c90: mov eax,dword ptr [rdi + rcx*0x4]
    add rcx,0x1
    add r8,0x4
    cmp rcx,r9
    mov dword ptr [r8 + -0x4],eax
    jl .lbl180018c90
    .lbl180018ca4: add r10,rdx
    sub r11,0x1
    jnz .lbl180018c80
    .lbl180018cad: mov edx,dword ptr [rsp + 0x34]
    mov r8d,0x1f
    mov rcx,rdi
    sub r8d,dword ptr [rsp + 0xa0]
    call EXTR_FUN_180017800
    mov r8d,dword ptr [rsp + 0x38]
    mov edx,dword ptr [rsp + 0x3c]
    mov rcx,rdi
    mov ebx,eax
    call EXTR_FUN_180018530
    lea r11,[rsp + 0x50]
    mov eax,ebx
    mov rbx,qword ptr [r11 + 0x30]
    mov rbp,qword ptr [r11 + 0x38]
    mov rsi,qword ptr [r11 + 0x40]
    mov rsp,r11
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180018530:
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    push rbp
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov r15,rcx
    mov r14d,0x1
    mov ecx,r8d
    mov r12d,r14d
    mov r13d,r8d
    shl r12d,cl
    mov ecx,edx
    shl r14d,cl
    mov ebp,r14d
    mov dword ptr [rsp + 0x80],r14d
    imul ebp,r12d
    test r15,r15
    jnz .lbl180018599
    lea r8d,[r15 + 0x4]
    mov edx,r12d
    mov ecx,r14d
    call EXTR_FUN_180017500
    mov ecx,ebp
    mov edx,eax
    lea rax,[rdx + rcx*0x8]
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret
    .lbl180018599: test r12d,r12d
    mov qword ptr [rsp + 0x48],rbx
    lea eax,[rbp + rbp*0x1]
    movsxd rcx,eax
    mov qword ptr [rsp + 0x40],rsi
    mov qword ptr [rsp + 0x38],rdi
    lea rbx,[r15 + rcx*0x4]
    mov qword ptr [rsp + 0x98],rbx
    jle .lbl180018615
    movsxd rsi,r14d
    mov r14d,dword ptr [rsp + 0x88]
    movsxd rax,ebp
    mov rbx,r15
    mov r13d,r12d
    lea rdi,[r15 + rax*0x4]
    shl rsi,0x2
    nop
    .lbl1800185e0: xor r9d,r9d
    mov r8d,r14d
    mov rdx,rdi
    mov rcx,rbx
    call EXTR_FUN_180019460
    add rdi,rsi
    add rbx,rsi
    sub r13,0x1
    jnz .lbl1800185e0
    mov r14d,dword ptr [rsp + 0x80]
    mov r13d,dword ptr [rsp + 0x90]
    mov rbx,qword ptr [rsp + 0x98]
    .lbl180018615: mov r9d,0x4
    mov r8d,r12d
    mov edx,r14d
    mov rcx,r15
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    movsxd r11,ebp
    mov r9d,0x4
    lea rax,[r15 + r11*0x4]
    mov r8d,r12d
    mov edx,r14d
    mov rcx,rax
    mov qword ptr [rsp + 0x80],rax
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    test r14d,r14d
    jle .lbl180018695
    movsxd rax,ebp
    movsxd rsi,r12d
    mov rbx,r15
    lea rdi,[r15 + rax*0x4]
    shl rsi,0x2
    mov ebp,r14d
    nop
    .lbl180018670: xor r9d,r9d
    mov r8d,r13d
    mov rdx,rdi
    mov rcx,rbx
    call EXTR_FUN_180019460
    add rdi,rsi
    add rbx,rsi
    sub rbp,0x1
    jnz .lbl180018670
    mov rbx,qword ptr [rsp + 0x98]
    .lbl180018695: mov r9d,0x4
    mov r8d,r14d
    mov edx,r12d
    mov rcx,r15
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    mov rcx,qword ptr [rsp + 0x80]
    mov r9d,0x4
    mov r8d,r14d
    mov edx,r12d
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180017560
    mov rdi,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rbx,qword ptr [rsp + 0x48]
    xor eax,eax
    add rsp,0x50
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret

section .data


section .text
EXTR_FUN_180016340:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    push rbx
    push rbp
    push rsi
    push rdi
    push r13
    sub rsp,0x50
    movsxd rsi,dword ptr [rsp + 0xa0]
    mov ebx,r8d
    mov edi,edx
    test esi,esi
    mov rbp,rcx
    mov r8,rsi
    jle .lbl18001637d
    shl r8,0x2
    xor edx,edx
    mov rcx,r9
    call EXTR_memset wrt ..plt
    .lbl18001637d: mov eax,edi
    mov ecx,edi
    xor r13d,r13d
    cdq
    imul ecx,ebx
    sub eax,edx
    movsxd r8,ecx
    sar eax,1
    mov qword ptr [rsp + 0x30],r8
    mov r10d,eax
    mov dword ptr [rsp + 0x28],eax
    js .lbl180016751
    mov qword ptr [rsp + 0x80],r12
    mov eax,ebx
    mov qword ptr [rsp + 0x48],r14
    cdq
    mov qword ptr [rsp + 0x40],r15
    sub eax,edx
    sar eax,1
    mov r9d,eax
    mov dword ptr [rsp + 0x20],eax
    .lbl1800163c1: xor edi,edi
    test r9d,r9d
    js .lbl180016732
    mov eax,r13d
    mov ecx,r13d
    shl eax,0xa
    cdq
    idiv r10d
    mov r10d,eax
    xor eax,eax
    mov dword ptr [rsp + 0xa0],eax
    imul r10d,r10d
    imul ecx,ebx
    movsxd rdx,ecx
    mov dword ptr [rsp + 0x24],r10d
    lea rcx,[r8 + rdx*0x1]
    lea r14,[rbp + rdx*0x4]
    lea r15,[rbp + rcx*0x4]
    .lbl180016402: cdq
    idiv r9d
    mov ebx,eax
    imul ebx,ebx
    add ebx,r10d
    mov ecx,ebx
    call EXTR_FUN_180002420
    mov edx,dword ptr [rsp + 0xb0]
    mov rcx,qword ptr [rsp + 0xa8]
    mov r8d,eax
    mov r11d,eax
    call EXTR_query_range
    mov r12d,eax
    shl r12d,0x2
    test ebx,ebx
    jnz .lbl18001643f
    xor r10d,r10d
    jmp .lbl18001645e
    .lbl18001643f: cmp r11d,0x3ff
    jle .lbl18001644e
    lea r10d,[rsi + -0x1]
    jmp .lbl18001645e
    .lbl18001644e: lea r10d,[rsi + -0x2]
    imul r10d,r11d
    sar r10d,0xa
    add r10d,0x1
    .lbl18001645e: test esi,esi
    jle .lbl1800164a0
    mov eax,dword ptr [r14]
    mov ecx,dword ptr [r15]
    mov rbx,qword ptr [rsp + 0x98]
    mov r9d,eax
    movsxd r8,r10d
    neg r9d
    cmp eax,r9d
    cmovg r9d,eax
    mov eax,ecx
    neg eax
    cmp ecx,eax
    cmovg eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    mov ecx,eax
    mov eax,r9d
    cdq
    sub eax,edx
    sar eax,1
    add ecx,eax
    add dword ptr [rbx + r8*0x4],ecx
    jmp .lbl1800164a8
    .lbl1800164a0: mov rbx,qword ptr [rsp + 0x98]
    .lbl1800164a8: mov ecx,dword ptr [r14]
    mov eax,0x51eb851f
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov eax,0x51eb851f
    mov dword ptr [r14],edx
    mov ecx,dword ptr [r15]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    test edi,edi
    mov dword ptr [r15],edx
    jle .lbl180016584
    lea r11d,[r13 + 0x1]
    imul r11d,dword ptr [rsp + 0x90]
    sub r11d,edi
    test esi,esi
    jle .lbl18001653b
    movsxd rcx,r11d
    movsxd r8,r10d
    mov eax,dword ptr [rbp + rcx*0x4]
    mov r9d,eax
    neg r9d
    cmp eax,r9d
    cmovg r9d,eax
    mov rax,qword ptr [rsp + 0x30]
    add rax,rcx
    mov ecx,dword ptr [rbp + rax*0x4]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    cmovg eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    mov ecx,eax
    mov eax,r9d
    cdq
    sub eax,edx
    sar eax,1
    add ecx,eax
    add dword ptr [rbx + r8*0x4],ecx
    .lbl18001653b: movsxd r8,r11d
    mov eax,0x51eb851f
    mov ecx,dword ptr [rbp + r8*0x4]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov rax,qword ptr [rsp + 0x30]
    mov dword ptr [rbp + r8*0x4],edx
    add r8,rax
    mov eax,0x51eb851f
    mov ecx,dword ptr [rbp + r8*0x4]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov dword ptr [rbp + r8*0x4],edx
    .lbl180016584: test r13d,r13d
    jle .lbl1800166f0
    mov ebx,dword ptr [rsp + 0x88]
    mov r9d,dword ptr [rsp + 0x90]
    sub ebx,r13d
    mov r11d,ebx
    imul r11d,r9d
    add r11d,edi
    test esi,esi
    jle .lbl1800165fe
    movsxd rcx,r11d
    movsxd r8,r10d
    mov eax,dword ptr [rbp + rcx*0x4]
    mov r9d,eax
    neg r9d
    cmp eax,r9d
    cmovg r9d,eax
    mov rax,qword ptr [rsp + 0x30]
    add rax,rcx
    mov ecx,dword ptr [rbp + rax*0x4]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    cmovg eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    mov ecx,eax
    mov eax,r9d
    mov r9,qword ptr [rsp + 0x98]
    cdq
    sub eax,edx
    sar eax,1
    add ecx,eax
    add dword ptr [r9 + r8*0x4],ecx
    mov r9d,dword ptr [rsp + 0x90]
    .lbl1800165fe: movsxd r8,r11d
    mov eax,0x51eb851f
    mov ecx,dword ptr [rbp + r8*0x4]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov rax,qword ptr [rsp + 0x30]
    mov dword ptr [rbp + r8*0x4],edx
    add r8,rax
    mov eax,0x51eb851f
    mov ecx,dword ptr [rbp + r8*0x4]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    test edi,edi
    mov dword ptr [rbp + r8*0x4],edx
    jle .lbl1800166f0
    lea r11d,[rbx + 0x1]
    imul r11d,r9d
    sub r11d,edi
    test esi,esi
    jle .lbl1800166a7
    movsxd rcx,r11d
    movsxd r8,r10d
    mov eax,dword ptr [rbp + rcx*0x4]
    mov r9d,eax
    neg r9d
    cmp eax,r9d
    cmovg r9d,eax
    mov rax,qword ptr [rsp + 0x30]
    add rax,rcx
    mov ecx,dword ptr [rbp + rax*0x4]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    cmovg eax,ecx
    cdq
    sub eax,edx
    sar eax,1
    mov ecx,eax
    mov eax,r9d
    cdq
    sub eax,edx
    sar eax,1
    add ecx,eax
    mov rax,qword ptr [rsp + 0x98]
    add dword ptr [rax + r8*0x4],ecx
    .lbl1800166a7: movsxd r8,r11d
    mov eax,0x51eb851f
    mov ecx,dword ptr [rbp + r8*0x4]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov rax,qword ptr [rsp + 0x30]
    mov dword ptr [rbp + r8*0x4],edx
    add r8,rax
    mov eax,0x51eb851f
    mov ecx,dword ptr [rbp + r8*0x4]
    imul ecx,r12d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov dword ptr [rbp + r8*0x4],edx
    .lbl1800166f0: mov eax,dword ptr [rsp + 0xa0]
    mov r9d,dword ptr [rsp + 0x20]
    mov r10d,dword ptr [rsp + 0x24]
    add eax,0x400
    add edi,0x1
    add r14,0x4
    add r15,0x4
    cmp edi,r9d
    mov dword ptr [rsp + 0xa0],eax
    jle .lbl180016402
    mov ebx,dword ptr [rsp + 0x90]
    mov r8,qword ptr [rsp + 0x30]
    mov r10d,dword ptr [rsp + 0x28]
    .lbl180016732: add r13d,0x1
    cmp r13d,r10d
    jle .lbl1800163c1
    mov r15,qword ptr [rsp + 0x40]
    mov r14,qword ptr [rsp + 0x48]
    mov r12,qword ptr [rsp + 0x80]
    .lbl180016751: add rsp,0x50
    pop r13
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180018d00:
    mov qword ptr [rsp + 0x18],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    sub rsp,0x30
    movsxd rbp,r9d
    mov r13,rcx
    movsxd rsi,r8d
    mov ecx,ebp
    mov r12,rdx
    call EXTR_log2ceil
    mov ecx,esi
    mov ebx,eax
    call EXTR_log2ceil
    mov edi,0x1
    mov ecx,eax
    mov r8d,ebx
    mov edx,eax
    shl edi,cl
    mov rcx,r12
    call EXTR_FUN_1800186f0
    test ebp,ebp
    movsxd rcx,edi
    mov rbx,rsi
    mov rdi,rbp
    mov dword ptr [rsp + 0x20],eax
    jle .lbl180018df2
    mov qword ptr [rsp + 0x60],r14
    mov qword ptr [rsp + 0x68],r15
    mov r15d,dword ptr [rsp + 0x80]
    lea r14,[rcx*0x4]
    lea rsi,[rsi*0x4]
    lea rbp,[rbx + rbx*0x1]
    mov r11,r12
    mov dword ptr [rsp + 0x78],0xffff8001
    .lbl180018d88: test rbx,rbx
    jle .lbl180018dd9
    mov ecx,r15d
    mov r9,r13
    mov rdx,r11
    mov r8,r12
    mov r10,rbx
    sub ecx,eax
    nop
    .lbl180018da0: mov eax,dword ptr [r8]
    sar eax,cl
    cmp eax,0x7fff
    mov dword ptr [rdx],eax
    jle .lbl180018db5
    mov eax,0x7fff
    jmp .lbl180018dbf
    .lbl180018db5: cmp eax,0xffff8001
    cmovl eax,dword ptr [rsp + 0x78]
    .lbl180018dbf: mov word ptr [r9],ax
    add r9,0x2
    add r8,0x4
    add rdx,0x4
    sub r10,0x1
    jnz .lbl180018da0
    mov eax,dword ptr [rsp + 0x20]
    .lbl180018dd9: add r13,rbp
    add r11,rsi
    add r12,r14
    sub rdi,0x1
    jnz .lbl180018d88
    mov r15,qword ptr [rsp + 0x68]
    mov r14,qword ptr [rsp + 0x60]
    .lbl180018df2: xor eax,eax
    mov rbx,qword ptr [rsp + 0x70]
    add rsp,0x30
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_FUN_1800186f0:
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x58
    mov r9,rcx
    mov ecx,r8d
    mov r14d,edx
    xor r15d,r15d
    mov edx,0x1
    mov r10d,0x1
    shl edx,cl
    mov ecx,r14d
    lea r8d,[r15 + 0x4]
    shl r10d,cl
    mov dword ptr [rsp + 0xb8],edx
    mov ebx,r15d
    mov eax,r10d
    mov dword ptr [rsp + 0xa8],r10d
    imul eax,edx
    mov dword ptr [rsp + 0x30],eax
    add eax,eax
    movsxd rcx,eax
    lea r11,[r9 + rcx*0x4]
    mov ecx,r10d
    mov qword ptr [rsp + 0x38],r11
    call EXTR_FUN_180017500
    movsxd r13,edx
    movsxd rsi,eax
    mov qword ptr [rsp + 0x48],r13
    mov r12,r15
    add rsi,r11
    test edx,edx
    lea rdi,[rsi + r13*0x4]
    mov qword ptr [rsp + 0x40],rdi
    jle .lbl1800187dd
    mov edi,dword ptr [rsp + 0x30]
    mov ebp,r15d
    mov r15,qword ptr [rsp + 0xa0]
    nop
    .lbl180018790: lea eax,[rbp + rdi*0x1]
    mov r9d,0x1
    mov r8d,r14d
    movsxd rcx,eax
    movsxd rax,ebp
    lea rdx,[r15 + rcx*0x4]
    lea rcx,[r15 + rax*0x4]
    call EXTR_FUN_180019460
    mov r10d,dword ptr [rsp + 0xa8]
    cmp ebx,eax
    mov dword ptr [rsi + r12*0x4],eax
    cmovle ebx,eax
    add r12,0x1
    add ebp,r10d
    cmp r12,r13
    jl .lbl180018790
    mov rdi,qword ptr [rsp + 0x40]
    mov r9,qword ptr [rsp + 0xa0]
    xor r15d,r15d
    .lbl1800187dd: test r13,r13
    movsxd rbp,r10d
    jle .lbl180018855
    mov rax,r13
    lea r12,[rbp*0x4]
    mov r10,r9
    mov r14,r13
    imul rax,rbp
    lea r11,[r9 + rax*0x4]
    nop
    .lbl180018800: cmp dword ptr [rsi],ebx
    jge .lbl18001883d
    test rbp,rbp
    jle .lbl18001883d
    mov rdx,r11
    mov r8,r10
    mov r9,rbp
    nop
    nop
    nop
    nop
    .lbl180018820: mov ecx,ebx
    add rdx,0x4
    add r8,0x4
    sub ecx,dword ptr [rsi]
    sar dword ptr [r8 + -0x4],cl
    mov ecx,ebx
    sub ecx,dword ptr [rsi]
    sar dword ptr [rdx + -0x4],cl
    sub r9,0x1
    jnz .lbl180018820
    .lbl18001883d: add rsi,0x4
    add r11,r12
    add r10,r12
    sub r14,0x1
    jnz .lbl180018800
    mov r10d,dword ptr [rsp + 0xa8]
    .lbl180018855: mov rax,qword ptr [rsp + 0x38]
    mov esi,dword ptr [rsp + 0xb8]
    mov r14,qword ptr [rsp + 0xa0]
    mov r8d,esi
    mov rcx,r14
    mov r9d,0x4
    mov edx,r10d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180017560
    mov rcx,qword ptr [rsp + 0x38]
    movsxd r12,dword ptr [rsp + 0x30]
    mov edx,dword ptr [rsp + 0xa8]
    lea rax,[r14 + r12*0x4]
    mov qword ptr [rsp + 0x20],rcx
    mov rcx,rax
    mov r9d,0x4
    mov r8d,esi
    mov qword ptr [rsp + 0xa0],rax
    call EXTR_FUN_180017560
    test rbp,rbp
    jle .lbl18001897d
    mov r13d,dword ptr [rsp + 0xb8]
    mov esi,r15d
    nop
    nop
    .lbl1800188d0: mov r8d,dword ptr [rsp + 0xb0]
    movsxd rax,r12d
    mov r9d,0x1
    lea rdx,[r14 + rax*0x4]
    movsxd rax,esi
    lea rcx,[r14 + rax*0x4]
    call EXTR_FUN_180019460
    cmp ebx,eax
    mov dword ptr [rdi + r15*0x4],eax
    cmovle ebx,eax
    add r15,0x1
    add esi,r13d
    add r12d,r13d
    cmp r15,rbp
    jl .lbl1800188d0
    test rbp,rbp
    mov r13,qword ptr [rsp + 0x48]
    jle .lbl18001897d
    mov rax,r13
    lea rsi,[r13*0x4]
    mov r10,r14
    imul rax,rbp
    lea r11,[r14 + rax*0x4]
    nop
    nop
    .lbl180018930: cmp dword ptr [rdi],ebx
    jge .lbl18001896d
    test r13,r13
    jle .lbl18001896d
    mov rdx,r11
    mov r8,r10
    mov r9,r13
    nop
    nop
    nop
    nop
    .lbl180018950: mov ecx,ebx
    add rdx,0x4
    add r8,0x4
    sub ecx,dword ptr [rdi]
    sar dword ptr [r8 + -0x4],cl
    mov ecx,ebx
    sub ecx,dword ptr [rdi]
    sar dword ptr [rdx + -0x4],cl
    sub r9,0x1
    jnz .lbl180018950
    .lbl18001896d: add rdi,0x4
    add r11,rsi
    add r10,rsi
    sub rbp,0x1
    jnz .lbl180018930
    .lbl18001897d: mov esi,dword ptr [rsp + 0xa8]
    mov edi,dword ptr [rsp + 0xb8]
    mov rbp,qword ptr [rsp + 0x38]
    mov r8d,esi
    mov edx,edi
    mov r9d,0x4
    mov rcx,r14
    mov qword ptr [rsp + 0x20],rbp
    call EXTR_FUN_180017560
    mov rcx,qword ptr [rsp + 0xa0]
    mov r9d,0x4
    mov r8d,esi
    mov edx,edi
    mov qword ptr [rsp + 0x20],rbp
    call EXTR_FUN_180017560
    mov eax,ebx
    add rsp,0x58
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_memset2:
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x28
    mov eax,dword ptr [rsp + 0x40]
    mov r8d,eax
    mov edx,dword ptr [rsp + 0x38]
    mov rcx,qword ptr [rsp + 0x30]
    call EXTR_memset wrt ..plt
    add rsp,0x28
    ret

section .data


