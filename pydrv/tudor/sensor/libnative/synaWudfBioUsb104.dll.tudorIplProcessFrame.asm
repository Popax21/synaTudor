%use masm
default rel
section .note.GNU-stack noalloc noexec nowrite progbits

global EXTR_tudorIplProcessFrame

section .text
EXTR_tudorIplProcessFrame:
    mov qword ptr [rsp + 0x18],r8
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x48
    mov dword ptr [rsp + 0x30],0x0
    mov qword ptr [rsp + 0x38],0x0
    cmp qword ptr [glb180179f90],0x0
    jz .lbl1800873f8
    lea r8,[glb1800db748]
    lea rdx,[glb18016e170]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funEnter
    .lbl1800873f8: cmp qword ptr [rsp + 0x50],0x0
    jnz .lbl180087464
    .lbl180087400: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087451
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180087451
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180087451
    lea rax,[glb18016e178]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x38fb9d6b
    lea r8,[glb18016e188]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087451: xor eax,eax
    test eax,eax
    jnz .lbl180087400
    mov dword ptr [rsp + 0x30],0x6f
    jmp .lbl180087525
    .lbl180087464: cmp qword ptr [rsp + 0x60],0x0
    jnz .lbl1800874cd
    .lbl18008746c: cmp qword ptr [glb180179f90],0x0
    jz .lbl1800874bd
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl1800874bd
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl1800874bd
    lea rax,[glb18016e190]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0xf18a5fe2
    lea r8,[glb18016e1a0]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl1800874bd: xor eax,eax
    test eax,eax
    jnz .lbl18008746c
    mov dword ptr [rsp + 0x30],0x6f
    jmp .lbl180087525
    .lbl1800874cd: mov rax,qword ptr [rsp + 0x50]
    mov qword ptr [rsp + 0x38],rax
    mov rax,qword ptr [rsp + 0x58]
    mov rdx,rax
    mov rcx,qword ptr [rsp + 0x38]
    call EXTR_tudorIplProcessFrameData
    mov dword ptr [rsp + 0x30],eax
    cmp dword ptr [rsp + 0x30],0x0
    jz .lbl1800874f6
    jmp .lbl180087525
    .lbl1800874f6: mov rax,qword ptr [rsp + 0x58]
    mov rcx,qword ptr [rsp + 0x38]
    mov ecx,dword ptr [rcx + 0x1c]
    mov dword ptr [rax + 0x10],ecx
    mov rax,qword ptr [rsp + 0x38]
    cmp qword ptr [rax + 0x28],0x0
    jz .lbl180087525
    mov rdx,qword ptr [rsp + 0x60]
    mov rcx,qword ptr [rsp + 0x38]
    call EXTR_tudorIplGetImage
    mov dword ptr [rsp + 0x30],eax
    .lbl180087525: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087549
    lea r8,[glb1800db748]
    lea rdx,[glb18016e1a8]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funLeave
    .lbl180087549: mov eax,dword ptr [rsp + 0x30]
    add rsp,0x48
    ret

section .data

glb18016e178: db 112, 104, 73, 112, 108, 32, 105, 115, 32, 78, 85, 76, 76, 0
glb18016e188: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e1a8: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e170: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e190: db 112, 73, 109, 97, 103, 101, 32, 105, 115, 32, 78, 85, 76, 76, 0
glb18016e1a0: db 115, 121, 110, 97, 76, 105, 98, 0
glb180179f90: db 8 dup 0
glb1800db748: db 116, 117, 100, 111, 114, 73, 112, 108, 80, 114, 111, 99, 101, 115, 115, 70, 114, 97, 109, 101, 0

section .text
EXTR_tudorIplProcessFrameData:
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x88
    mov qword ptr [rsp + 0x58],0x0
    mov dword ptr [rsp + 0x44],0x0
    mov qword ptr [rsp + 0x78],0x0
    mov dword ptr [rsp + 0x40],0x0
    mov dword ptr [rsp + 0x48],0x0
    mov dword ptr [rsp + 0x50],0x0
    mov dword ptr [rsp + 0x4c],0x0
    mov rax,qword ptr [rsp + 0x90]
    mov qword ptr [rsp + 0x58],rax
    mov rax,qword ptr [rsp + 0x58]
    movzx eax,word ptr [rax + 0x2]
    mov dword ptr [rsp + 0x74],eax
    mov rcx,qword ptr [rsp + 0x58]
    movzx ecx,word ptr [rcx + 0x4]
    mov rdx,qword ptr [rsp + 0x58]
    movzx edx,word ptr [rdx + 0xa]
    mov dword ptr [rsp + 0x70],edx
    mov r8,qword ptr [rsp + 0x58]
    movzx r8d,word ptr [r8 + 0xc]
    mov r9,qword ptr [rsp + 0x58]
    movzx r9d,word ptr [r9]
    imul r8d,r9d
    add r8d,0x1f
    mov eax,r8d
    cdq
    and edx,0x7
    add eax,edx
    sar eax,0x3
    and eax,0xfffffffc
    mov edx,dword ptr [rsp + 0x70]
    add edx,eax
    mov eax,edx
    imul ecx,eax
    mov eax,ecx
    mov ecx,dword ptr [rsp + 0x74]
    add ecx,eax
    mov eax,ecx
    mov rcx,qword ptr [rsp + 0x98]
    cmp dword ptr [rcx],eax
    jge .lbl18008777b
    jmp .lbl180087af6
    .lbl18008777b: mov rax,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x18]
    mov dword ptr [rsp + 0x20],eax
    mov rax,qword ptr [rsp + 0x90]
    mov r9d,dword ptr [rax + 0x14]
    mov rax,qword ptr [rsp + 0x90]
    mov r8,qword ptr [rax + 0x28]
    mov rdx,qword ptr [rsp + 0x58]
    mov rax,qword ptr [rsp + 0x98]
    mov rcx,qword ptr [rax + 0x8]
    call EXTR_tudorIplImportFrame
    mov rax,qword ptr [rsp + 0x90]
    mov rcx,qword ptr [rsp + 0x90]
    mov ecx,dword ptr [rcx + 0x18]
    cmp dword ptr [rax + 0x14],ecx
    jc .lbl1800877f3
    mov rax,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x18]
    mov dword ptr [rsp + 0x50],eax
    mov rax,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x14]
    mov dword ptr [rsp + 0x4c],eax
    jmp .lbl18008790c
    .lbl1800877f3: mov rax,qword ptr [rsp + 0x90]
    mov rcx,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x14]
    imul eax,dword ptr [rcx + 0x18]
    mov eax,eax
    shl rax,1
    mov dword ptr [rsp + 0x60],eax
    mov ecx,dword ptr [rsp + 0x60]
    call EXTR_malloc wrt ..plt
    mov qword ptr [rsp + 0x68],rax
    cmp qword ptr [rsp + 0x68],0x0
    jnz .lbl18008789b
    cmp dword ptr [rsp + 0x44],0x0
    jz .lbl180087837
    cmp dword ptr [rsp + 0x44],0x6e
    jnz .lbl18008783f
    .lbl180087837: mov dword ptr [rsp + 0x44],0x25a
    .lbl18008783f: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087890
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180087890
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180087890
    lea rax,[glb18016e1b0]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x7fbdc693
    lea r8,[glb18016e1d0]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087890: xor eax,eax
    test eax,eax
    jnz .lbl18008783f
    jmp .lbl180087af6
    .lbl18008789b: mov rax,qword ptr [rsp + 0x90]
    mov r9d,dword ptr [rax + 0x14]
    mov rax,qword ptr [rsp + 0x90]
    mov r8d,dword ptr [rax + 0x18]
    mov rdx,qword ptr [rsp + 0x68]
    mov rax,qword ptr [rsp + 0x90]
    mov rcx,qword ptr [rax + 0x28]
    call EXTR_tudorImageRotate
    mov r8d,dword ptr [rsp + 0x60]
    mov rdx,qword ptr [rsp + 0x68]
    mov rax,qword ptr [rsp + 0x90]
    mov rcx,qword ptr [rax + 0x28]
    call EXTR_memcpy wrt ..plt
    mov rcx,qword ptr [rsp + 0x68]
    call EXTR_free wrt ..plt
    mov rax,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x14]
    mov dword ptr [rsp + 0x50],eax
    mov rax,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x18]
    mov dword ptr [rsp + 0x4c],eax
    .lbl18008790c: mov rax,qword ptr [rsp + 0x90]
    mov rax,qword ptr [rax + 0x28]
    mov qword ptr [rsp + 0x78],rax
    lea rax,[rsp + 0x48]
    mov qword ptr [rsp + 0x30],rax
    lea rax,[rsp + 0x40]
    mov qword ptr [rsp + 0x28],rax
    mov eax,dword ptr [rsp + 0x4c]
    mov dword ptr [rsp + 0x20],eax
    mov r9d,dword ptr [rsp + 0x50]
    mov rax,qword ptr [rsp + 0x90]
    mov r8,qword ptr [rax + 0x28]
    mov rdx,qword ptr [rsp + 0x78]
    mov rax,qword ptr [rsp + 0x90]
    mov rcx,qword ptr [rax + 0x30]
    call EXTR_tudorImageFixup
    mov dword ptr [rsp + 0x44],eax
    cmp dword ptr [rsp + 0x44],0x0
    jz .lbl1800879c7
    .lbl18008796b: cmp qword ptr [glb180179f90],0x0
    jz .lbl1800879bc
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x4
    test eax,eax
    jz .lbl1800879bc
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x3
    ja .lbl1800879bc
    lea rax,[glb18016e1d8]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x1b600f92
    lea r8,[glb18016e1f8]
    mov edx,0x3
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl1800879bc: xor eax,eax
    test eax,eax
    jnz .lbl18008796b
    jmp .lbl180087af6
    .lbl1800879c7: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087a28
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x2
    test eax,eax
    jz .lbl180087a28
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x2
    ja .lbl180087a28
    mov eax,dword ptr [rsp + 0x48]
    mov dword ptr [rsp + 0x30],eax
    mov eax,dword ptr [rsp + 0x40]
    mov dword ptr [rsp + 0x28],eax
    lea rax,[glb18016e200]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0xc01598cc
    lea r8,[glb18016e230]
    mov edx,0x2
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087a28: xor eax,eax
    test eax,eax
    jnz .lbl1800879c7
    cmp dword ptr [rsp + 0x48],0x0
    jz .lbl180087a51
    mov rax,qword ptr [rsp + 0x90]
    mov ecx,dword ptr [rsp + 0x48]
    mov eax,dword ptr [rax + 0x1c]
    or eax,ecx
    mov rcx,qword ptr [rsp + 0x90]
    mov dword ptr [rcx + 0x1c],eax
    .lbl180087a51: mov rax,qword ptr [rsp + 0x90]
    movzx ecx,byte ptr [rsp + 0x40]
    mov byte ptr [rax + 0x20],cl
    cmp dword ptr [rsp + 0x40],-0x28
    jle .lbl180087aed
    cmp dword ptr [rsp + 0x40],0x32
    jge .lbl180087aed
    .lbl180087a73: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087acc
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x2
    test eax,eax
    jz .lbl180087acc
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x2
    ja .lbl180087acc
    mov eax,dword ptr [rsp + 0x40]
    mov dword ptr [rsp + 0x28],eax
    lea rax,[glb18016e238]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x951d1aa2
    lea r8,[glb18016e258]
    mov edx,0x2
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087acc: xor eax,eax
    test eax,eax
    jnz .lbl180087a73
    mov rax,qword ptr [rsp + 0x90]
    mov eax,dword ptr [rax + 0x1c]
    or eax,0x80200000
    mov rcx,qword ptr [rsp + 0x90]
    mov dword ptr [rcx + 0x1c],eax
    .lbl180087aed: mov qword ptr [rsp + 0x78],0x0
    .lbl180087af6: mov eax,dword ptr [rsp + 0x44]
    add rsp,0x88
    ret

section .data

glb18016e1d8: db 70, 97, 105, 108, 101, 100, 32, 116, 111, 32, 102, 105, 120, 117, 112, 32, 100, 101, 110, 97, 108, 105, 32, 105, 109, 97, 103, 101, 0
glb18016e1f8: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e238: db 70, 105, 110, 103, 101, 114, 32, 99, 111, 118, 101, 114, 97, 103, 101, 32, 116, 111, 111, 32, 108, 111, 119, 58, 32, 37, 100, 0
glb18016e258: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e1b0: db 77, 101, 109, 111, 114, 121, 32, 97, 108, 108, 111, 99, 97, 116, 105, 111, 110, 32, 102, 97, 105, 108, 101, 100, 46, 10, 0
glb18016e1d0: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e200: db 70, 105, 110, 103, 101, 114, 32, 99, 111, 118, 101, 114, 97, 103, 101, 58, 32, 37, 100, 32, 32, 73, 109, 97, 103, 101, 32, 81, 117, 97, 108, 105, 116, 121, 58, 32, 48, 120, 37, 120, 0
glb18016e230: db 115, 121, 110, 97, 76, 105, 98, 0

section .text
EXTR_tudorImageRotate:
    mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x48
    cmp qword ptr [glb180179f90],0x0
    jz .lbl18008ca4c
    lea r8,[glb18013a698]
    lea rdx,[glb18016fbf0]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funEnter
    .lbl18008ca4c: mov rax,qword ptr [rsp + 0x50]
    mov qword ptr [rsp + 0x30],rax
    mov dword ptr [rsp + 0x20],0x0
    jmp .lbl18008ca6a
    .lbl18008ca60: mov eax,dword ptr [rsp + 0x20]
    inc eax
    mov dword ptr [rsp + 0x20],eax
    .lbl18008ca6a: mov eax,dword ptr [rsp + 0x60]
    cmp dword ptr [rsp + 0x20],eax
    jnc .lbl18008cae4
    mov eax,dword ptr [rsp + 0x20]
    mov rcx,qword ptr [rsp + 0x58]
    lea rax,[rcx + rax*0x2]
    mov qword ptr [rsp + 0x28],rax
    mov dword ptr [rsp + 0x24],0x0
    jmp .lbl18008ca9a
    .lbl18008ca90: mov eax,dword ptr [rsp + 0x24]
    inc eax
    mov dword ptr [rsp + 0x24],eax
    .lbl18008ca9a: mov eax,dword ptr [rsp + 0x68]
    cmp dword ptr [rsp + 0x24],eax
    jnc .lbl18008cacd
    mov eax,dword ptr [rsp + 0x24]
    mov rcx,qword ptr [rsp + 0x28]
    mov rdx,qword ptr [rsp + 0x30]
    movzx eax,word ptr [rdx + rax*0x2]
    mov word ptr [rcx],ax
    mov eax,dword ptr [rsp + 0x60]
    mov rcx,qword ptr [rsp + 0x28]
    lea rax,[rcx + rax*0x2]
    mov qword ptr [rsp + 0x28],rax
    jmp .lbl18008ca90
    .lbl18008cacd: mov eax,dword ptr [rsp + 0x68]
    mov rcx,qword ptr [rsp + 0x30]
    lea rax,[rcx + rax*0x2]
    mov qword ptr [rsp + 0x30],rax
    jmp .lbl18008ca60
    .lbl18008cae4: cmp qword ptr [glb180179f90],0x0
    jz .lbl18008cb08
    lea r8,[glb18013a698]
    lea rdx,[glb18016fbf8]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funLeave
    .lbl18008cb08: add rsp,0x48
    ret

section .data

glb18016fbf8: db 115, 121, 110, 97, 76, 105, 98, 0
glb18013a698: db 116, 117, 100, 111, 114, 73, 109, 97, 103, 101, 82, 111, 116, 97, 116, 101, 0
glb18016fbf0: db 115, 121, 110, 97, 76, 105, 98, 0

section .text
EXTR_funLeave:
    ret

section .text
EXTR_funEnter:
    ret

section .text
extern EXTR_free

section .text
extern EXTR_memcpy

section .text
extern EXTR_malloc

section .text
EXTR_log:
    ret

section .text
EXTR_tudorIplImportFrame:
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x18],r8
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x48
    mov qword ptr [rsp + 0x30],0x0
    mov qword ptr [rsp + 0x38],0x0
    mov dword ptr [rsp + 0x24],0x0
    mov dword ptr [rsp + 0x28],0x0
    mov dword ptr [rsp + 0x20],0x0
    mov rax,qword ptr [rsp + 0x58]
    movzx eax,word ptr [rax + 0xa]
    mov dword ptr [rsp + 0x2c],eax
    mov rcx,qword ptr [rsp + 0x58]
    movzx ecx,word ptr [rcx + 0xc]
    mov rdx,qword ptr [rsp + 0x58]
    movzx edx,word ptr [rdx]
    imul ecx,edx
    add ecx,0x1f
    mov eax,ecx
    cdq
    and edx,0x7
    add eax,edx
    sar eax,0x3
    and eax,0xfffffffc
    mov ecx,dword ptr [rsp + 0x2c]
    add ecx,eax
    mov eax,ecx
    mov dword ptr [rsp + 0x24],eax
    mov rax,qword ptr [rsp + 0x58]
    movzx eax,word ptr [rax + 0x2]
    mov rcx,qword ptr [rsp + 0x50]
    add rcx,rax
    mov rax,rcx
    mov rcx,qword ptr [rsp + 0x58]
    movzx ecx,word ptr [rcx + 0x6]
    imul ecx,dword ptr [rsp + 0x24]
    mov ecx,ecx
    add rax,rcx
    mov rcx,qword ptr [rsp + 0x58]
    movzx ecx,word ptr [rcx + 0xa]
    add rax,rcx
    mov rcx,qword ptr [rsp + 0x58]
    movzx ecx,word ptr [rcx + 0xe]
    lea rax,[rax + rcx*0x2]
    mov qword ptr [rsp + 0x30],rax
    mov eax,dword ptr [rsp + 0x68]
    shl rax,1
    mov dword ptr [rsp + 0x28],eax
    mov rax,qword ptr [rsp + 0x60]
    mov qword ptr [rsp + 0x38],rax
    mov dword ptr [rsp + 0x20],0x0
    jmp .lbl18008764e
    .lbl180087644: mov eax,dword ptr [rsp + 0x20]
    inc eax
    mov dword ptr [rsp + 0x20],eax
    .lbl18008764e: mov eax,dword ptr [rsp + 0x70]
    cmp dword ptr [rsp + 0x20],eax
    jnc .lbl180087696
    mov r8d,dword ptr [rsp + 0x28]
    mov rdx,qword ptr [rsp + 0x30]
    mov rcx,qword ptr [rsp + 0x38]
    call EXTR_memcpy wrt ..plt
    mov eax,dword ptr [rsp + 0x24]
    mov rcx,qword ptr [rsp + 0x30]
    add rcx,rax
    mov rax,rcx
    mov qword ptr [rsp + 0x30],rax
    mov eax,dword ptr [rsp + 0x28]
    mov rcx,qword ptr [rsp + 0x38]
    add rcx,rax
    mov rax,rcx
    mov qword ptr [rsp + 0x38],rax
    jmp .lbl180087644
    .lbl180087696: add rsp,0x48
    ret

section .data


section .text
EXTR_tudorImageFixup:
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x18],r8
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x58
    mov dword ptr [rsp + 0x38],0x0
    mov qword ptr [rsp + 0x48],0x0
    mov dword ptr [rsp + 0x34],0x0
    mov dword ptr [rsp + 0x30],0x0
    cmp qword ptr [glb180179f90],0x0
    jz .lbl18008c81d
    lea r8,[glb18013a688]
    lea rdx,[glb18016fbb8]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funEnter
    .lbl18008c81d: mov rcx,qword ptr [rsp + 0x60]
    call EXTR_tudorIplGetBufSize
    mov dword ptr [rsp + 0x3c],eax
    mov ecx,dword ptr [rsp + 0x3c]
    call EXTR_malloc wrt ..plt
    mov qword ptr [rsp + 0x48],rax
    cmp qword ptr [rsp + 0x48],0x0
    jnz .lbl18008c8b3
    cmp dword ptr [rsp + 0x38],0x0
    jz .lbl18008c84f
    cmp dword ptr [rsp + 0x38],0x6e
    jnz .lbl18008c857
    .lbl18008c84f: mov dword ptr [rsp + 0x38],0x25a
    .lbl18008c857: cmp qword ptr [glb180179f90],0x0
    jz .lbl18008c8a8
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl18008c8a8
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl18008c8a8
    lea rax,[glb18016fbc0]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x7fbdc693
    lea r8,[glb18016fbe0]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl18008c8a8: xor eax,eax
    test eax,eax
    jnz .lbl18008c857
    jmp .lbl18008c9bc
    .lbl18008c8b3: mov r8d,dword ptr [rsp + 0x3c]
    xor edx,edx
    mov rcx,qword ptr [rsp + 0x48]
    call EXTR_memset2
    mov rdx,qword ptr [rsp + 0x48]
    mov rcx,qword ptr [rsp + 0x60]
    call EXTR_tudorIplAcceptBuf
    xor r8d,r8d
    mov rdx,qword ptr [rsp + 0x68]
    mov rcx,qword ptr [rsp + 0x60]
    call EXTR_tudorIplGetCoverage
    mov dword ptr [rsp + 0x34],eax
    mov eax,dword ptr [rsp + 0x34]
    and eax,0x2000
    test eax,eax
    jz .lbl18008c902
    mov eax,dword ptr [rsp + 0x30]
    bts eax,0xd
    mov dword ptr [rsp + 0x30],eax
    .lbl18008c902: mov eax,dword ptr [rsp + 0x34]
    and eax,0x1000000
    test eax,eax
    jz .lbl18008c91b
    mov eax,dword ptr [rsp + 0x30]
    bts eax,0x18
    mov dword ptr [rsp + 0x30],eax
    .lbl18008c91b: mov eax,dword ptr [rsp + 0x34]
    and eax,0x8000000
    test eax,eax
    jz .lbl18008c934
    mov eax,dword ptr [rsp + 0x30]
    bts eax,0x1b
    mov dword ptr [rsp + 0x30],eax
    .lbl18008c934: mov eax,dword ptr [rsp + 0x34]
    and eax,0x80000000
    test eax,eax
    jz .lbl18008c94d
    mov eax,dword ptr [rsp + 0x30]
    bts eax,0x1f
    mov dword ptr [rsp + 0x30],eax
    .lbl18008c94d: cmp qword ptr [rsp + 0x90],0x0
    jz .lbl18008c966
    mov rax,qword ptr [rsp + 0x90]
    mov ecx,dword ptr [rsp + 0x30]
    mov dword ptr [rax],ecx
    .lbl18008c966: mov rdx,qword ptr [rsp + 0x68]
    mov rcx,qword ptr [rsp + 0x60]
    call EXTR_tudorIplPostProcess
    mov dword ptr [rsp + 0x40],eax
    mov rax,qword ptr [rsp + 0x68]
    cmp qword ptr [rsp + 0x70],rax
    jz .lbl18008c9a3
    mov eax,dword ptr [rsp + 0x78]
    imul eax,dword ptr [rsp + 0x80]
    mov r8d,eax
    mov rdx,qword ptr [rsp + 0x68]
    mov rcx,qword ptr [rsp + 0x70]
    call EXTR_memcpy wrt ..plt
    .lbl18008c9a3: cmp qword ptr [rsp + 0x88],0x0
    jz .lbl18008c9bc
    mov rax,qword ptr [rsp + 0x88]
    mov ecx,dword ptr [rsp + 0x40]
    mov dword ptr [rax],ecx
    .lbl18008c9bc: cmp qword ptr [rsp + 0x48],0x0
    jz .lbl18008c9d7
    mov rcx,qword ptr [rsp + 0x48]
    call EXTR_free wrt ..plt
    mov qword ptr [rsp + 0x48],0x0
    .lbl18008c9d7: cmp qword ptr [glb180179f90],0x0
    jz .lbl18008c9fb
    lea r8,[glb18013a688]
    lea rdx,[glb18016fbe8]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funLeave
    .lbl18008c9fb: mov eax,dword ptr [rsp + 0x38]
    add rsp,0x58
    ret

section .data

glb18016fbb8: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016fbe8: db 115, 121, 110, 97, 76, 105, 98, 0
glb18013a688: db 116, 117, 100, 111, 114, 73, 109, 97, 103, 101, 70, 105, 120, 117, 112, 0
glb18016fbc0: db 77, 101, 109, 111, 114, 121, 32, 97, 108, 108, 111, 99, 97, 116, 105, 111, 110, 32, 102, 97, 105, 108, 101, 100, 46, 10, 0
glb18016fbe0: db 115, 121, 110, 97, 76, 105, 98, 0

section .text
EXTR_tudorIplGetCoverage:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x20
    movzx eax,word ptr [rcx + 0x140]
    xor ebx,ebx
    mov rsi,rdx
    add eax,-0x4
    mov rdi,rcx
    cmp eax,0x1
    ja .lbl18000200b
    cmp r8d,0x7d0
    jnz .lbl180001ff9
    movzx r8d,word ptr [rcx + 0x2c]
    movzx eax,word ptr [rcx + 0x2a]
    mov rcx,qword ptr [rcx + 0x80]
    imul r8d,eax
    add r8d,r8d
    call EXTR_memcpy wrt ..plt
    mov edx,0xc8
    mov rcx,rdi
    call EXTR_FUN_180001e30
    movzx ecx,word ptr [rdi + 0x2a]
    movzx r8d,word ptr [rdi + 0x2c]
    imul r8d,ecx
    mov rdx,qword ptr [rdi + 0x80]
    mov rcx,rsi
    add r8d,r8d
    mov ebx,eax
    call EXTR_memcpy wrt ..plt
    .lbl180001ff9: mov eax,ebx
    mov rbx,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    add rsp,0x20
    pop rdi
    ret
    .lbl18000200b: mov rbx,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    xor eax,eax
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
extern EXTR_memcpy

section .text
EXTR_FUN_180001e30:
    sub rsp,0x38
    sub edx,0xc8
    mov r10,rcx
    jz .lbl180001e76
    cmp edx,0x1
    jnz .lbl180001e6f
    mov rax,qword ptr [r10]
    mov r9,qword ptr [r10 + 0x128]
    lea r8,[rcx + 0x2d8]
    lea rdx,[rcx + 0x148]
    add rcx,0x18
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180006bd0
    add rsp,0x38
    ret
    .lbl180001e6f: xor eax,eax
    add rsp,0x38
    ret
    .lbl180001e76: mov r9,qword ptr [r10]
    mov r8,qword ptr [r10 + 0x80]
    lea rdx,[rcx + 0x2d8]
    add rcx,0x18
    add rsp,0x38
    jmp .lbl1800067e0
    .lbl1800067e0: mov qword ptr [rsp + 0x20],r9
    mov qword ptr [rsp + 0x18],r8
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0xe8
    xor ebx,ebx
    mov rdi,rcx
    mov r15,rdx
    lea rcx,[rsp + 0x7c]
    lea r8d,[rbx + 0x38]
    xor edx,edx
    mov dword ptr [rsp + 0x78],ebx
    call EXTR_memset wrt ..plt
    lea r8d,[rbx + 0x38]
    lea rcx,[rsp + 0x3c]
    xor edx,edx
    mov dword ptr [rsp + 0x38],ebx
    call EXTR_memset wrt ..plt
    movzx r14d,word ptr [rdi + 0x14]
    movzx eax,word ptr [rdi + 0x12]
    imul r14d,eax
    cmp word ptr [rdi + 0x16],0x4
    mov rsi,qword ptr [r15 + 0x18]
    mov r13d,0x320
    mov r8d,0x32c8
    mov dword ptr [rsp + 0xd0],r13d
    jnz .lbl180006924
    mov eax,dword ptr [rdi + 0xc]
    test al,0x2
    jz .lbl180006924
    mov word ptr [rdi + 0x1c],0x2
    mov word ptr [rdi + 0x12c],bx
    or eax,0x1a
    mov dword ptr [rdi + 0xc],eax
    mov dword ptr [rsi],0xfffeee90
    mov dword ptr [rsi + 0x4],0x12
    mov dword ptr [rsi + 0x8],0x1
    mov dword ptr [rsi + 0x34],0xffffb1e0
    mov dword ptr [rsi + 0x38],0x1
    mov dword ptr [rsi + 0x3c],0x1
    mov dword ptr [rsi + 0x104],0xfffffce0
    mov dword ptr [rsi + 0x108],0xfffffff6
    mov dword ptr [rsi + 0x10c],0x1
    mov dword ptr [rsi + 0x138],ebx
    mov dword ptr [rsi + 0x13c],ebx
    mov dword ptr [rsi + 0x140],ebx
    mov r13d,0x46
    mov r8d,0xc8
    mov dword ptr [rsi + 0x374],0xfffffa88
    mov dword ptr [rsi + 0x378],0xfffffff6
    mov dword ptr [rsi + 0x37c],0x1
    mov dword ptr [rsi + 0x3a8],0xfffe1ba0
    mov dword ptr [rsi + 0x3ac],0x3e7
    mov dword ptr [rsi + 0x3b0],0x1
    mov dword ptr [rsp + 0xd0],r13d
    .lbl180006924: mov ecx,r14d
    mov eax,0x51eb851f
    lea r9,[rsp + 0x78]
    imul ecx,ecx,0x46
    imul ecx
    mov ecx,edx
    sar ecx,0x5
    mov eax,ecx
    shr eax,0x1f
    add ecx,eax
    mov rax,qword ptr [rsp + 0x148]
    mov dword ptr [rax],r8d
    movzx r8d,word ptr [rdi + 0x12]
    movzx edx,word ptr [rdi + 0x14]
    mov dword ptr [rsp + 0x30],ecx
    mov rcx,qword ptr [rsp + 0x140]
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000d400
    mov eax,dword ptr [rsp + 0x78]
    movzx r8d,word ptr [rdi + 0x12]
    movzx edx,word ptr [rdi + 0x14]
    mov rcx,qword ptr [rsp + 0x140]
    mov dword ptr [rsp + 0x3c],eax
    mov eax,dword ptr [rsp + 0x7c]
    mov r9d,0x258
    mov dword ptr [rsp + 0x38],0x1
    mov dword ptr [rsp + 0x40],eax
    mov eax,dword ptr [rsp + 0x80]
    mov dword ptr [rsp + 0x44],eax
    mov eax,dword ptr [rsp + 0x84]
    mov dword ptr [rsp + 0x48],eax
    mov rax,qword ptr [rsp + 0x148]
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000d5c0
    movzx r8d,word ptr [rdi + 0x12]
    movzx edx,word ptr [rdi + 0x14]
    mov r9,qword ptr [rsp + 0x148]
    mov rcx,qword ptr [rsp + 0x140]
    mov dword ptr [rsp + 0x4c],eax
    call EXTR_FUN_18000d840
    movzx r8d,word ptr [rdi + 0x12]
    movzx edx,word ptr [rdi + 0x14]
    mov dword ptr [rsp + 0x50],eax
    mov rax,qword ptr [rsp + 0x148]
    mov qword ptr [rsp + 0x28],rax
    mov r9d,r13d
    mov r13,qword ptr [rsp + 0x140]
    lea rax,[rsp + 0x78]
    mov rcx,r13
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000dac0
    mov eax,dword ptr [rsp + 0x84]
    mov r11d,dword ptr [rsp + 0x78]
    mov r8,qword ptr [rsp + 0x148]
    mov dword ptr [rsp + 0x58],eax
    mov eax,dword ptr [rsp + 0x94]
    mov rdx,rdi
    mov rcx,r13
    mov dword ptr [rsp + 0x64],eax
    mov eax,dword ptr [rsp + 0x98]
    mov dword ptr [rsp + 0x54],r11d
    mov dword ptr [rsp + 0x68],eax
    movzx eax,word ptr [rdi + 0x1c]
    mov word ptr [rdi + 0x1c],0x4
    mov word ptr [rsp + 0x130],ax
    movzx eax,word ptr [rdi + 0x22]
    mov word ptr [rsp + 0x138],ax
    movzx eax,word ptr [rsp + 0xd0]
    mov word ptr [rdi + 0x22],ax
    call EXTR_weird_pprocess_wrapper
    mov rax,qword ptr [rsp + 0x148]
    lea r9,[rsp + 0x78]
    mov r8d,0x1
    mov rdx,rdi
    mov rcx,r13
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000bfe0
    mov r11d,dword ptr [rsp + 0x78]
    mov eax,dword ptr [rsp + 0x7c]
    mov dword ptr [rsp + 0x5c],r11d
    mov dword ptr [rsp + 0x60],eax
    mov eax,dword ptr [rsp + 0x84]
    mov r13d,dword ptr [rsp + 0x80]
    sub r14d,r11d
    mov dword ptr [rsp + 0x148],eax
    mov ax,word ptr [rsp + 0x130]
    cmp ax,0x4
    jnz .lbl180006af6
    mov rcx,qword ptr [rsp + 0x140]
    mov rdx,rdi
    mov byte ptr [rdi + 0x58],0x1
    call EXTR_FUN_18000c000
    jmp .lbl180006b06
    .lbl180006af6: mov word ptr [rdi + 0x1c],ax
    mov ax,word ptr [rsp + 0x138]
    mov word ptr [rdi + 0x22],ax
    .lbl180006b06: mov edx,0x5
    lea r8,[rsp + 0x38]
    mov r9,rsi
    lea ecx,[rdx + 0x8]
    call EXTR_FUN_1800058c0
    test eax,eax
    jz .lbl180006b29
    mov ebx,0x80000000
    jmp .lbl180006bb5
    .lbl180006b29: mov edx,0xc
    lea r9,[rsi + 0x104]
    lea r8,[rsp + 0x38]
    lea ecx,[rdx + 0x1]
    call EXTR_FUN_1800058c0
    test eax,eax
    jz .lbl180006b4d
    mov ebx,0x2000
    jmp .lbl180006bb5
    .lbl180006b4d: mov edx,0xc
    lea r9,[rsi + 0x374]
    lea r8,[rsp + 0x38]
    lea ecx,[rdx + 0x1]
    call EXTR_FUN_1800058c0
    test eax,eax
    jz .lbl180006b71
    mov ebx,0x1000000
    jmp .lbl180006bb5
    .lbl180006b71: cmp r14d,dword ptr [rsp + 0x30]
    jge .lbl180006b97
    cmp r13d,-0x1e
    mov ebx,0x8000000
    jg .lbl180006b8a
    mov ebx,0x8020000
    jmp .lbl180006bb5
    .lbl180006b8a: cmp r13d,0x1e
    jl .lbl180006bb5
    mov ebx,0x8040000
    jmp .lbl180006bb5
    .lbl180006b97: mov eax,dword ptr [rsp + 0x148]
    cmp eax,-0xa
    jg .lbl180006baa
    mov ebx,0x8008000
    jmp .lbl180006bb5
    .lbl180006baa: mov ecx,0x8004000
    cmp eax,0xa
    cmovge ebx,ecx
    .lbl180006bb5: mov dword ptr [r15 + 0x14],ebx
    mov eax,ebx
    add rsp,0xe8
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
EXTR_FUN_1800058c0:
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x20
    xor r11d,r11d
    test edx,edx
    mov r12,r9
    mov r13,r8
    mov ebp,edx
    mov esi,ecx
    mov edi,0x1
    jle .lbl18000593b
    mov qword ptr [rsp + 0x40],rbx
    xor ebx,ebx
    .lbl1800058f0: movsxd rax,ebx
    mov rdx,r13
    mov ecx,esi
    lea r8,[r12 + rax*0x4]
    call EXTR_FUN_1800056e0
    xor edx,edx
    test eax,eax
    setg dl
    cmp edx,edi
    jz .lbl180005920
    mov ecx,0x1
    add r11d,0x1
    add ebx,esi
    sub ecx,edi
    cmp r11d,ebp
    mov edi,ecx
    jl .lbl1800058f0
    .lbl180005920: mov rbx,qword ptr [rsp + 0x40]
    mov eax,edx
    .lbl180005927: mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x20
    pop r13
    pop r12
    pop rdi
    ret
    .lbl18000593b: xor eax,eax
    jmp .lbl180005927

section .data


section .text
EXTR_FUN_1800056e0:
    xor r9d,r9d
    test ecx,ecx
    movsxd r10,ecx
    jle .lbl180005725
    sub rdx,r8
    nop
    .lbl1800056f0: movsxd rcx,dword ptr [rdx + r8*0x1]
    movsxd rax,dword ptr [r8]
    add r8,0x4
    imul rcx,rax
    add r9,rcx
    sub r10,0x1
    jnz .lbl1800056f0
    cmp r9,0x7fffffff
    jle .lbl180005717
    mov eax,0x7fffffff
    ret
    .lbl180005717: cmp r9,-0x80000000
    mov eax,0x80000000
    jl .lbl180005728
    .lbl180005725: mov eax,r9d
    .lbl180005728: ret 0x0

section .data


section .text
EXTR_FUN_18000d400:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x10],edx
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
    mov rbp,qword ptr [rsp + 0xc0]
    mov eax,r8d
    mov r11,rcx
    mov esi,dword ptr [rbp]
    sar eax,1
    mov r15d,r8d
    mov r10d,edx
    mov r8d,edx
    lea r9,[rsp + 0x38]
    sar r8d,0x3
    sar r10d,1
    mov qword ptr [rsp + 0x28],rbp
    mov dword ptr [rsp + 0x30],r8d
    imul r10d,eax
    mov eax,r15d
    sar eax,0x3
    mov ecx,eax
    mov dword ptr [rsp + 0xb0],eax
    xor eax,eax
    mov dword ptr [rsp + 0x38],eax
    mov dword ptr [rsp + 0x3c],eax
    mov dword ptr [rsp + 0x40],eax
    imul ecx,r8d
    movsxd r14,ecx
    mov dword ptr [rsp + 0x44],eax
    movsxd rax,r10d
    lea rbx,[rbp + rax*0x8]
    lea eax,[r10*0x4]
    mov r8d,r15d
    movsxd rcx,eax
    lea rax,[r14 + r14*0x2]
    add rcx,rax
    lea rax,[rsp + 0xc0]
    lea r13,[rbx + rcx*0x4]
    mov rcx,r11
    mov qword ptr [rsp + 0x20],rax
    lea rdi,[r13 + r14*0x4]
    lea r12,[rdi + r14*0x8]
    call EXTR_FUN_18000c700
    mov edx,dword ptr [rsp + 0xa8]
    mov rcx,qword ptr [rsp + 0xa0]
    mov r9,rbx
    mov r8d,r15d
    call EXTR_FUN_18000cb30
    mov ebx,dword ptr [rsp + 0xb0]
    mov r15d,dword ptr [rsp + 0x30]
    mov r9d,ebx
    mov r8d,r15d
    mov rdx,r13
    mov rcx,rdi
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_18000c3e0
    mov rdi,qword ptr [rsp + 0xb8]
    mov r9d,ebx
    mov dword ptr [rdi],eax
    lea rax,[r14 + r14*0x2]
    mov r8d,r15d
    lea rcx,[rbp + rax*0x4]
    mov rdx,r13
    mov dword ptr [rsp + 0x20],esi
    call EXTR_FUN_18000c3e0
    lea rdx,[r12 + r14*0x4]
    mov r9d,r15d
    mov r8,r13
    mov rcx,r12
    mov dword ptr [rsp + 0x28],esi
    mov dword ptr [rdi + 0x4],eax
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_18000c4e0
    mov r8d,dword ptr [rsp + 0xc0]
    mov dword ptr [rdi + 0x8],eax
    mov edx,0x1
    mov eax,r8d
    sub eax,dword ptr [rsp + 0x38]
    nop
    .lbl18000d550: mov ecx,r8d
    sub ecx,dword ptr [rsp + rdx*0x4 + 0x38]
    cmp ecx,eax
    cmovg eax,ecx
    add rdx,0x1
    cmp rdx,0x4
    jl .lbl18000d550
    imul eax,eax,0x64
    lea ecx,[r8 + 0x1]
    cdq
    idiv ecx
    mov dword ptr [rdi + 0xc],eax
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
EXTR_FUN_18000cb30:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    mov dword ptr [r11 + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [r11 + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x88
    movsxd rbx,edx
    movsxd rbp,r8d
    xor r8d,r8d
    mov eax,ebp
    mov r10d,ebx
    mov r13,r9
    sar eax,0x3
    sar r10d,0x3
    mov qword ptr [rsp + 0x58],rbx
    imul r10d,eax
    mov eax,ebx
    imul eax,ebp
    test ebx,ebx
    cdqe
    lea r14,[r9 + rax*0x4]
    movsxd rax,r10d
    lea rdx,[r14 + rax*0x4]
    mov qword ptr [rsp + 0x38],r14
    mov qword ptr [rsp + 0x60],rax
    mov qword ptr [rsp + 0x30],rdx
    lea rdx,[rdx + rax*0x4]
    mov qword ptr [rsp + 0x70],rdx
    lea rdx,[rdx + rax*0x4]
    mov qword ptr [rsp + 0x40],rdx
    lea rdx,[rdx + rax*0x4]
    mov qword ptr [rsp + 0x48],rdx
    lea rdx,[rdx + rax*0x4]
    lea rdi,[rdx + rax*0x4]
    mov qword ptr [rsp + 0x78],rdx
    mov rdx,rbx
    lea r15,[rdi + rax*0x4]
    mov qword ptr [rsp + 0x50],rdi
    mov qword ptr [rsp + 0x68],r15
    jle .lbl18000cc65
    lea rsi,[rbp*0x4]
    mov rax,r9
    lea r12d,[rbp + -0x1]
    lea r13,[rbp + rbp*0x1]
    lea rbx,[rcx + 0x4]
    lea r11,[rsi + rax*0x1 + -0x4]
    mov r10,rax
    mov rbp,rdx
    nop
    .lbl18000cc00: cmp r12d,0x1
    jle .lbl18000cc3a
    lea r8,[r10 + 0x4]
    mov rdx,rbx
    lea r9d,[r12 + -0x1]
    .lbl18000cc12: movsx eax,word ptr [rdx + -0x4]
    movsx ecx,word ptr [rdx]
    sub ecx,eax
    jns .lbl18000cc1f
    neg ecx
    .lbl18000cc1f: sar ecx,0x4
    add r8,0x4
    add rdx,0x2
    imul ecx,ecx
    sub r9,0x1
    mov dword ptr [r8 + -0x4],ecx
    jnz .lbl18000cc12
    xor r8d,r8d
    .lbl18000cc3a: mov dword ptr [r10],r8d
    mov dword ptr [r11],r8d
    add rbx,r13
    add r11,rsi
    add r10,rsi
    sub rbp,0x1
    jnz .lbl18000cc00
    mov r13,qword ptr [rsp + 0xe8]
    mov ebp,dword ptr [rsp + 0xe0]
    mov ebx,dword ptr [rsp + 0xd8]
    .lbl18000cc65: mov r9d,0x3
    mov r8d,ebp
    mov edx,ebx
    mov rcx,r13
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_180012ae0
    lea r14d,[rbx + -0x1]
    mov esi,0x1
    cmp r14d,esi
    jle .lbl18000cd5c
    movsxd rdi,dword ptr [rsp + 0xe0]
    mov r15,qword ptr [rsp + 0xd0]
    lea ebp,[r14 + -0x1]
    lea r12d,[rdi + -0x1]
    nop
    nop
    nop
    .lbl18000ccb0: cmp r12d,0x1
    jle .lbl18000cd3d
    lea rcx,[rsi + -0x1]
    mov r8,rdi
    lea rax,[rcx + 0x2]
    imul r8,rsi
    lea rbx,[r13 + r8*0x4 + 0x4]
    lea r9,[r15 + r8*0x2 + 0x4]
    lea r8d,[r12 + -0x1]
    imul rax,rdi
    imul rcx,rdi
    lea r11,[r15 + rcx*0x2 + 0x2]
    lea r10,[r15 + rax*0x2 + 0x2]
    nop
    nop
    .lbl18000ccf0: movsx eax,word ptr [r9 + -0x4]
    movsx edx,word ptr [r9]
    sub edx,eax
    jns .lbl18000cd06
    neg edx
    sar edx,0x4
    neg edx
    jmp .lbl18000cd09
    .lbl18000cd06: sar edx,0x4
    .lbl18000cd09: movsx ecx,word ptr [r10]
    movsx eax,word ptr [r11]
    sub ecx,eax
    jns .lbl18000cd1e
    neg ecx
    sar ecx,0x4
    neg ecx
    jmp .lbl18000cd21
    .lbl18000cd1e: sar ecx,0x4
    .lbl18000cd21: imul ecx,edx
    add rbx,0x4
    add r9,0x2
    mov dword ptr [rbx + -0x4],ecx
    add r11,0x2
    add r10,0x2
    sub r8,0x1
    jnz .lbl18000ccf0
    .lbl18000cd3d: add rsi,0x1
    sub rbp,0x1
    jnz .lbl18000ccb0
    mov rdi,qword ptr [rsp + 0x50]
    mov r15,qword ptr [rsp + 0x68]
    mov ebp,dword ptr [rsp + 0xe0]
    .lbl18000cd5c: mov r12,qword ptr [rsp + 0x58]
    xor r9d,r9d
    test ebp,ebp
    mov rcx,r9
    movsxd rbx,ebp
    jle .lbl18000cd95
    lea rax,[r12 + -0x1]
    imul rax,rbx
    lea rdx,[r13 + rax*0x4]
    nop
    .lbl18000cd80: mov dword ptr [r13 + rcx*0x4],r9d
    add rcx,0x1
    mov dword ptr [rdx],r9d
    add rdx,0x4
    cmp rcx,rbx
    jl .lbl18000cd80
    .lbl18000cd95: test r12,r12
    jle .lbl18000cdc2
    lea rdx,[rbx*0x4]
    mov rax,r13
    mov r8,r12
    lea rcx,[rdx + r13*0x1 + -0x4]
    nop
    .lbl18000cdb0: mov dword ptr [rax],r9d
    mov dword ptr [rcx],r9d
    add rcx,rdx
    add rax,rdx
    sub r8,0x1
    jnz .lbl18000cdb0
    .lbl18000cdc2: mov rsi,qword ptr [rsp + 0x30]
    mov edx,dword ptr [rsp + 0xd8]
    mov r9d,0x3
    mov r8d,ebp
    mov rcx,r13
    mov qword ptr [rsp + 0x20],rsi
    call EXTR_FUN_180012ae0
    cmp r14d,0x1
    jle .lbl18000ce91
    mov rax,qword ptr [rsp + 0xe8]
    mov rcx,qword ptr [rsp + 0xd0]
    lea r13,[rbx*0x4]
    lea rbp,[rbx + rbx*0x1]
    lea r12d,[r14 + -0x1]
    lea r11,[r13 + rax*0x1]
    lea rsi,[r13 + rcx*0x1]
    .lbl18000ce18: test rbx,rbx
    jle .lbl18000ce61
    mov r9,r11
    mov rdx,rsi
    mov r8,rcx
    mov r10,rbx
    nop
    nop
    .lbl18000ce30: movsx ecx,word ptr [rdx]
    movsx eax,word ptr [r8]
    sub ecx,eax
    jns .lbl18000ce3d
    neg ecx
    .lbl18000ce3d: sar ecx,0x4
    add r9,0x4
    add r8,0x2
    add rdx,0x2
    imul ecx,ecx
    sub r10,0x1
    mov dword ptr [r9 + -0x4],ecx
    jnz .lbl18000ce30
    mov rcx,qword ptr [rsp + 0xd0]
    .lbl18000ce61: add rcx,rbp
    add rsi,rbp
    add r11,r13
    sub r12,0x1
    mov qword ptr [rsp + 0xd0],rcx
    jnz .lbl18000ce18
    mov r13,qword ptr [rsp + 0xe8]
    mov ebp,dword ptr [rsp + 0xe0]
    mov rsi,qword ptr [rsp + 0x30]
    mov r12,qword ptr [rsp + 0x58]
    .lbl18000ce91: xor r8d,r8d
    test rbx,rbx
    mov rcx,r8
    jle .lbl18000cec5
    lea rax,[r12 + -0x1]
    imul rax,rbx
    lea rdx,[r13 + rax*0x4]
    nop
    nop
    .lbl18000ceb0: mov dword ptr [r13 + rcx*0x4],r8d
    add rcx,0x1
    mov dword ptr [rdx],r8d
    add rdx,0x4
    cmp rcx,rbx
    jl .lbl18000ceb0
    .lbl18000cec5: mov r14,qword ptr [rsp + 0x70]
    mov edx,dword ptr [rsp + 0xd8]
    mov r9d,0x3
    mov r8d,ebp
    mov rcx,r13
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_180012ae0
    mov rbx,qword ptr [rsp + 0x60]
    test rbx,rbx
    jle .lbl18000d076
    mov rbp,qword ptr [rsp + 0x38]
    mov r12,qword ptr [rsp + 0x40]
    sub rsi,r14
    sub rbp,r14
    sub r12,r14
    sub qword ptr [rsp + 0x48],r14
    mov r15,qword ptr [rsp + 0x48]
    mov r13,r14
    mov r14,rbx
    nop
    nop
    .lbl18000cf20: mov ecx,dword ptr [r13]
    mov edx,dword ptr [r13 + rbp*0x1]
    mov eax,dword ptr [rsi + r13*0x1]
    lea ebx,[rdx + rcx*0x1]
    sar ecx,0xa
    sar edx,0xa
    sar eax,0xa
    sar ebx,1
    imul edx,ecx
    mov ecx,ebx
    sar ecx,0xa
    imul eax,eax
    sub edx,eax
    imul ecx,ecx
    sub ecx,edx
    call EXTR_FUN_180002420
    mov ecx,ebx
    shl eax,0xa
    cmp eax,ebx
    cmovl ecx,eax
    add r13,0x4
    lea eax,[rcx + rbx*0x1]
    sub ebx,ecx
    sub r14,0x1
    mov dword ptr [r12 + r13*0x1 + -0x4],eax
    mov dword ptr [r15 + r13*0x1 + -0x4],ebx
    jnz .lbl18000cf20
    mov rbx,qword ptr [rsp + 0x60]
    mov r15,qword ptr [rsp + 0x68]
    mov r14,qword ptr [rsp + 0x70]
    test rbx,rbx
    jle .lbl18000d076
    mov rax,qword ptr [rsp + 0x30]
    mov r9,qword ptr [rsp + 0x40]
    mov r8,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x78]
    sub r9,rax
    sub r8,rax
    sub r14,rax
    sub rax,rdi
    sub rsi,rdi
    mov qword ptr [rsp + 0x40],r9
    mov qword ptr [rsp + 0x38],r8
    mov qword ptr [rsp + 0x30],rax
    sub r15,rdi
    .lbl18000cfc2: lea rcx,[rax + rdi*0x1]
    mov edx,dword ptr [rcx + r14*0x1]
    cmp dword ptr [rcx + r8*0x1],edx
    jle .lbl18000cff5
    mov eax,dword ptr [rcx + r9*0x1]
    sub eax,edx
    jns .lbl18000cfe1
    neg eax
    sar eax,0xc
    neg eax
    jmp .lbl18000cfe4
    .lbl18000cfe1: sar eax,0xc
    .lbl18000cfe4: mov dword ptr [rdi],eax
    mov eax,dword ptr [rcx]
    test eax,eax
    jns .lbl18000d01c
    neg eax
    sar eax,0xc
    neg eax
    jmp .lbl18000d01f
    .lbl18000cff5: mov eax,dword ptr [rcx]
    test eax,eax
    jns .lbl18000d004
    neg eax
    sar eax,0xc
    neg eax
    jmp .lbl18000d007
    .lbl18000d004: sar eax,0xc
    .lbl18000d007: mov dword ptr [rdi],eax
    mov eax,dword ptr [rcx + r9*0x1]
    sub eax,dword ptr [rcx + r8*0x1]
    jns .lbl18000d01c
    neg eax
    sar eax,0xc
    neg eax
    jmp .lbl18000d01f
    .lbl18000d01c: sar eax,0xc
    .lbl18000d01f: mov ecx,eax
    mov dword ptr [r15 + rdi*0x1],eax
    mov eax,dword ptr [rdi]
    imul ecx,ecx
    imul eax,eax
    add ecx,eax
    call EXTR_FUN_180002420
    mov dword ptr [rsi + rdi*0x1],eax
    shl dword ptr [rdi],0xa
    shl dword ptr [r15 + rdi*0x1],0xa
    mov ecx,dword ptr [rsi + rdi*0x1]
    test ecx,ecx
    jle .lbl18000d059
    mov eax,dword ptr [rdi]
    cdq
    idiv ecx
    mov dword ptr [rdi],eax
    mov eax,dword ptr [r15 + rdi*0x1]
    cdq
    idiv dword ptr [rsi + rdi*0x1]
    mov dword ptr [r15 + rdi*0x1],eax
    .lbl18000d059: mov r8,qword ptr [rsp + 0x38]
    mov r9,qword ptr [rsp + 0x40]
    mov rax,qword ptr [rsp + 0x30]
    add rdi,0x4
    sub rbx,0x1
    jnz .lbl18000cfc2
    .lbl18000d076: add rsp,0x88
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
EXTR_FUN_180012ae0:
    mov qword ptr [rsp + 0x20],rbx
    mov qword ptr [rsp + 0x8],rcx
    push rsi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x20
    mov r12,qword ptr [rsp + 0x70]
    mov ebx,edx
    movsxd r13,r9d
    movsxd r15,r8d
    mov rdx,rcx
    mov ecx,r13d
    sar ebx,cl
    mov esi,r15d
    sar esi,cl
    mov eax,esi
    imul eax,ebx
    test eax,eax
    jle .lbl180012b30
    mov r8d,eax
    xor edx,edx
    mov rcx,r12
    shl r8,0x2
    call EXTR_memset wrt ..plt
    mov rdx,qword ptr [rsp + 0x50]
    .lbl180012b30: mov ecx,r13d
    xor r11d,r11d
    movsxd r14,esi
    shl ebx,cl
    test ebx,ebx
    jle .lbl180012bac
    mov qword ptr [rsp + 0x58],rbp
    shl esi,cl
    mov qword ptr [rsp + 0x60],rdi
    xor edi,edi
    mov ebp,ebx
    nop
    .lbl180012b50: test esi,esi
    jle .lbl180012b8f
    mov rcx,r13
    mov r8,rdi
    mov rax,r15
    sar r8,cl
    xor r9d,r9d
    mov ebx,esi
    imul r8,r14
    imul rax,r11
    lea r10,[rdx + rax*0x4]
    .lbl180012b71: mov eax,dword ptr [r10]
    mov rdx,r9
    add r10,0x4
    sar rdx,cl
    add r9,0x1
    add rdx,r8
    add dword ptr [r12 + rdx*0x4],eax
    sub rbx,0x1
    jnz .lbl180012b71
    .lbl180012b8f: mov rdx,qword ptr [rsp + 0x50]
    add rdi,0x1
    add r11,0x1
    sub rbp,0x1
    jnz .lbl180012b50
    mov rdi,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x58]
    .lbl180012bac: mov rbx,qword ptr [rsp + 0x68]
    add rsp,0x20
    pop r15
    pop r14
    pop r13
    pop r12
    pop rsi
    ret

section .data


section .text
extern EXTR_memset

section .text
EXTR_FUN_18000c4e0:
    mov qword ptr [rsp + 0x18],r8
    push rbx
    push rbp
    push rdi
    sub rsp,0x40
    lea eax,[r9 + -0x1]
    mov rdi,r8
    xor r8d,r8d
    mov rbp,rdx
    xor edx,edx
    mov rbx,rcx
    xor ecx,ecx
    test eax,eax
    mov dword ptr [rsp + 0x78],r8d
    mov dword ptr [rsp + 0xc],edx
    mov dword ptr [rsp + 0x8],eax
    mov dword ptr [rsp],ecx
    jle .lbl18000c6e2
    mov r11d,dword ptr [rsp + 0x80]
    mov r8d,dword ptr [rsp + 0x8]
    mov qword ptr [rsp + 0x60],rsi
    mov qword ptr [rsp + 0x38],r12
    mov qword ptr [rsp + 0x30],r13
    lea eax,[r11 + -0x1]
    mov qword ptr [rsp + 0x28],r14
    mov qword ptr [rsp + 0x20],r15
    mov dword ptr [rsp + 0x4],eax
    .lbl18000c546: xor esi,esi
    test eax,eax
    jle .lbl18000c6a1
    mov r9d,dword ptr [rsp + 0x88]
    mov r10d,edx
    mov r15,rdi
    mov r14,rbp
    sub r15,rbx
    imul r10d,r11d
    movsxd rax,r10d
    mov dword ptr [rsp + 0x10],r10d
    sub r14,rbx
    lea r8,[rbx + rax*0x4]
    mov eax,dword ptr [rsp + 0x4]
    nop
    nop
    .lbl18000c580: cmp dword ptr [r15 + r8*0x1],r9d
    jle .lbl18000c68d
    lea eax,[rdx + 0x1]
    lea ecx,[r10 + rsi*0x1 + 0x1]
    imul eax,r11d
    add eax,esi
    xor r11d,r11d
    movsxd r13,eax
    mov rax,qword ptr [rsp + 0x70]
    cmp dword ptr [rdi + r13*0x4],r9d
    movsxd rdi,ecx
    mov ecx,dword ptr [rbx + r13*0x4]
    setg r11b
    xor r12d,r12d
    cmp dword ptr [rax + rdi*0x4],r9d
    setg r12b
    imul ecx,dword ptr [r8]
    lea eax,[r12 + r11*0x1]
    add dword ptr [rsp + 0x78],eax
    mov eax,dword ptr [rbp + r13*0x4]
    imul eax,dword ptr [r14 + r8*0x1]
    add ecx,eax
    mov eax,ecx
    neg eax
    cmp ecx,eax
    jle .lbl18000c5fa
    mov edx,dword ptr [r14 + r8*0x1]
    mov r10d,dword ptr [r8]
    mov eax,dword ptr [rbx + r13*0x4]
    imul eax,r10d
    mov r9d,edx
    imul r9d,dword ptr [rbp + r13*0x4]
    add r9d,eax
    jmp .lbl18000c618
    .lbl18000c5fa: mov edx,dword ptr [r14 + r8*0x1]
    mov r10d,dword ptr [r8]
    mov eax,dword ptr [rbx + r13*0x4]
    imul eax,r10d
    mov r9d,edx
    imul r9d,dword ptr [rbp + r13*0x4]
    add r9d,eax
    neg r9d
    .lbl18000c618: mov ecx,dword ptr [rbp + rdi*0x4]
    mov eax,dword ptr [r8]
    imul eax,dword ptr [rbx + rdi*0x4]
    imul ecx,dword ptr [r14 + r8*0x1]
    add ecx,eax
    mov eax,ecx
    neg eax
    cmp ecx,eax
    jle .lbl18000c641
    imul edx,dword ptr [rbp + rdi*0x4]
    imul r10d,dword ptr [rbx + rdi*0x4]
    add edx,r10d
    jmp .lbl18000c650
    .lbl18000c641: imul edx,dword ptr [rbp + rdi*0x4]
    imul r10d,dword ptr [rbx + rdi*0x4]
    add edx,r10d
    neg edx
    .lbl18000c650: mov eax,dword ptr [rsp + 0x4]
    mov r10d,dword ptr [rsp + 0x10]
    mov rdi,qword ptr [rsp + 0x70]
    imul r11d,r9d
    imul r12d,edx
    mov edx,dword ptr [rsp + 0xc]
    mov r9d,dword ptr [rsp + 0x88]
    sar r11d,0xa
    sar r12d,0xa
    add r11d,dword ptr [rsp]
    lea ecx,[r11 + r12*0x1]
    mov r11d,dword ptr [rsp + 0x80]
    mov dword ptr [rsp],ecx
    .lbl18000c68d: add esi,0x1
    add r8,0x4
    cmp esi,eax
    jl .lbl18000c580
    mov r8d,dword ptr [rsp + 0x8]
    .lbl18000c6a1: add edx,0x1
    cmp edx,r8d
    mov dword ptr [rsp + 0xc],edx
    jl .lbl18000c546
    mov r8d,dword ptr [rsp + 0x78]
    mov r15,qword ptr [rsp + 0x20]
    mov r14,qword ptr [rsp + 0x28]
    test r8d,r8d
    mov r13,qword ptr [rsp + 0x30]
    mov r12,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x60]
    jle .lbl18000c6e2
    mov eax,ecx
    cdq
    idiv r8d
    add rsp,0x40
    pop rdi
    pop rbp
    pop rbx
    ret
    .lbl18000c6e2: xor eax,eax
    add rsp,0x40
    pop rdi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_18000c3e0:
    mov qword ptr [rsp + 0x10],rdx
    push rbx
    push r12
    push r13
    sub rsp,0x10
    mov qword ptr [rsp + 0x30],rbp
    imul r8d,r9d
    mov r9d,dword ptr [rsp + 0x50]
    mov qword ptr [rsp + 0x40],rsi
    mov qword ptr [rsp + 0x48],rdi
    mov qword ptr [rsp + 0x8],r14
    movsxd r14,r8d
    xor r12d,r12d
    xor r13d,r13d
    xor edi,edi
    xor esi,esi
    xor r10d,r10d
    xor ebx,ebx
    xor r11d,r11d
    cmp r14,0x2
    mov rax,rdx
    mov rbp,rcx
    jl .lbl18000c480
    mov r8,rcx
    mov qword ptr [rsp],r15
    add rdx,0x4
    sub r8,rax
    lea r15,[r14 + -0x1]
    nop
    .lbl18000c440: mov eax,dword ptr [rbp + r11*0x4]
    xor ecx,ecx
    cmp dword ptr [rdx + -0x4],r9d
    setg cl
    add edi,ecx
    imul eax,ecx
    xor ecx,ecx
    add r12d,eax
    cmp dword ptr [rdx],r9d
    mov eax,dword ptr [r8 + rdx*0x1]
    setg cl
    add r11,0x2
    add rdx,0x8
    add esi,ecx
    imul eax,ecx
    add r13d,eax
    cmp r11,r15
    jl .lbl18000c440
    mov rax,qword ptr [rsp + 0x38]
    mov r15,qword ptr [rsp]
    .lbl18000c480: cmp r11,r14
    mov r14,qword ptr [rsp + 0x8]
    jge .lbl18000c49b
    cmp dword ptr [rax + r11*0x4],r9d
    mov ebx,dword ptr [rbp + r11*0x4]
    setg r10b
    imul ebx,r10d
    .lbl18000c49b: mov rbp,qword ptr [rsp + 0x30]
    lea eax,[rsi + rdi*0x1]
    mov rdi,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x40]
    add r10d,eax
    test r10d,r10d
    jle .lbl18000c4ca
    lea eax,[r13 + r12*0x1]
    add eax,ebx
    cdq
    idiv r10d
    add rsp,0x10
    pop r13
    pop r12
    pop rbx
    ret
    .lbl18000c4ca: xor eax,eax
    add rsp,0x10
    pop r13
    pop r12
    pop rbx
    ret

section .data


section .text
EXTR_FUN_18000c700:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x20],r9
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x70
    mov rbx,qword ptr [rsp + 0xd8]
    mov rdi,rcx
    mov r11d,edx
    mov r14d,edx
    mov eax,0x2e8ba2e9
    mov r15d,r8d
    sar r15d,1
    sar r14d,1
    mov r9d,0x1
    imul r8d
    mov r13d,edx
    mov eax,0x2e8ba2e9
    mov r10d,r15d
    sar r13d,0x2
    mov qword ptr [rsp + 0x20],rbx
    imul r11d
    imul r10d,r14d
    mov ecx,r13d
    movsxd rbp,r10d
    shr ecx,0x1f
    lea rsi,[rbx + rbp*0x4]
    add r13d,ecx
    mov ecx,edx
    sar ecx,0x2
    mov dword ptr [rsp + 0x30],r13d
    mov eax,ecx
    shr eax,0x1f
    add ecx,eax
    mov eax,r8d
    mov dword ptr [rsp + 0xc0],ecx
    sar eax,0x3
    mov ecx,r11d
    sar ecx,0x3
    mov dword ptr [rsp + 0xb8],eax
    mov dword ptr [rsp + 0xd8],ecx
    imul eax,ecx
    cdqe
    mov rcx,rdi
    lea rdx,[rax + rax*0x2]
    lea r12,[rbx + rdx*0x4]
    mov edx,r11d
    call EXTR_downscale
    mov r9,rsi
    mov r8d,r15d
    mov edx,r14d
    mov rcx,rbx
    call EXTR_average_img
    xor r10d,r10d
    test rbp,rbp
    jle .lbl18000c7ef
    nop
    .lbl18000c7d0: mov eax,dword ptr [rsi + r10*0x4]
    mov r9d,eax
    neg r9d
    cmp eax,r9d
    cmovg r9d,eax
    add r10,0x1
    cmp r10,rbp
    mov dword ptr [rsi + r10*0x4 + -0x4],r9d
    jl .lbl18000c7d0
    .lbl18000c7ef: mov r9d,0x2
    mov r8d,r15d
    mov edx,r14d
    mov rcx,rsi
    mov qword ptr [rsp + 0x20],r12
    call EXTR_FUN_180012ae0
    movsxd r8,dword ptr [rsp + 0xd8]
    movsxd rbx,dword ptr [rsp + 0xb8]
    xor esi,esi
    xor ebp,ebp
    test r8,r8
    movsxd r14,r13d
    mov qword ptr [rsp + 0x50],r8
    jle .lbl18000c8f0
    lea rcx,[r12 + rbx*0x4 + -0x8]
    xor r13d,r13d
    lea rdx,[rbx*0x4]
    mov r15,r12
    mov qword ptr [rsp + 0x40],r8
    mov qword ptr [rsp + 0x38],rcx
    nop
    nop
    .lbl18000c850: xor r8d,r8d
    xor r9d,r9d
    xor r10d,r10d
    xor r11d,r11d
    xor edi,edi
    cmp r14,0x2
    jl .lbl18000c8a9
    lea rdx,[r14 + -0x2]
    mov rax,r15
    shr rdx,1
    add rdx,0x1
    lea rdi,[rdx + rdx*0x1]
    nop
    nop
    nop
    .lbl18000c880: add r10d,dword ptr [rax]
    add r8d,dword ptr [rcx + 0x4]
    add r11d,dword ptr [rax + 0x4]
    add r9d,dword ptr [rcx]
    add rax,0x8
    sub rcx,0x8
    sub rdx,0x1
    jnz .lbl18000c880
    mov rcx,qword ptr [rsp + 0x38]
    lea rdx,[rbx*0x4]
    .lbl18000c8a9: cmp rdi,r14
    jge .lbl18000c8c5
    lea rax,[r13 + rdi*0x1]
    add esi,dword ptr [r12 + rax*0x4]
    mov rax,r13
    sub rax,rdi
    add rax,rbx
    add ebp,dword ptr [r12 + rax*0x4 + -0x4]
    .lbl18000c8c5: lea eax,[r8 + r9*0x1]
    add rcx,rdx
    add r13,rbx
    add ebp,eax
    lea eax,[r10 + r11*0x1]
    add r15,rdx
    add esi,eax
    sub qword ptr [rsp + 0x40],0x1
    mov qword ptr [rsp + 0x38],rcx
    jnz .lbl18000c850
    mov r8,qword ptr [rsp + 0x50]
    .lbl18000c8f0: mov rax,qword ptr [rsp + 0xc8]
    movsxd r9,dword ptr [rsp + 0xc0]
    xor r13d,r13d
    mov dword ptr [rax + 0x8],ebp
    xor ebp,ebp
    test r9d,r9d
    mov dword ptr [rax],esi
    mov rsi,r9
    jle .lbl18000ca18
    xor edx,edx
    mov rax,rbx
    lea rcx,[r8 + -0x1]
    neg rax
    lea r15,[-0x4 + r8*0x4]
    mov qword ptr [rsp + 0x38],r12
    imul rcx,rbx
    mov qword ptr [rsp + 0x60],rax
    mov rax,rbx
    mov qword ptr [rsp + 0x40],rdx
    imul r15,rbx
    neg rax
    mov qword ptr [rsp + 0x48],rcx
    mov qword ptr [rsp + 0x50],r9
    shl rax,0x2
    add r15,r12
    mov qword ptr [rsp + 0x58],rax
    nop
    .lbl18000c960: xor r8d,r8d
    xor r9d,r9d
    xor r10d,r10d
    xor r11d,r11d
    xor edi,edi
    cmp rbx,0x2
    jl .lbl18000c9b6
    mov rcx,qword ptr [rsp + 0x38]
    lea rdx,[rbx + -0x2]
    mov rax,r15
    shr rdx,1
    add rdx,0x1
    lea rdi,[rdx + rdx*0x1]
    nop
    nop
    .lbl18000c990: add r10d,dword ptr [rcx]
    add r8d,dword ptr [rax]
    add r11d,dword ptr [rcx + 0x4]
    add r9d,dword ptr [rax + 0x4]
    add rcx,0x8
    add rax,0x8
    sub rdx,0x1
    jnz .lbl18000c990
    mov rcx,qword ptr [rsp + 0x48]
    mov rdx,qword ptr [rsp + 0x40]
    .lbl18000c9b6: cmp rdi,rbx
    jge .lbl18000c9cb
    lea rax,[rdx + rdi*0x1]
    add ebp,dword ptr [r12 + rax*0x4]
    lea rax,[rcx + rdi*0x1]
    add r13d,dword ptr [r12 + rax*0x4]
    .lbl18000c9cb: add rcx,qword ptr [rsp + 0x60]
    add r15,qword ptr [rsp + 0x58]
    lea eax,[r9 + r8*0x1]
    add r13d,eax
    lea eax,[r11 + r10*0x1]
    add rdx,rbx
    add ebp,eax
    lea rax,[rbx*0x4]
    mov qword ptr [rsp + 0x40],rdx
    add qword ptr [rsp + 0x38],rax
    sub qword ptr [rsp + 0x50],0x1
    mov qword ptr [rsp + 0x48],rcx
    jnz .lbl18000c960
    mov rax,qword ptr [rsp + 0xc8]
    mov r9d,dword ptr [rsp + 0xc0]
    .lbl18000ca18: mov r8d,dword ptr [rsp + 0xd8]
    xor r10d,r10d
    mov dword ptr [rax + 0x4],ebp
    sub r8d,r9d
    mov dword ptr [rax + 0xc],r13d
    cmp r9d,r8d
    jge .lbl18000cadf
    mov r15d,dword ptr [rsp + 0x30]
    mov edx,dword ptr [rsp + 0xb8]
    imul rsi,rbx
    sub edx,r15d
    lea rax,[rsi + r14*0x1]
    lea rcx,[rbx*0x4]
    mov r13d,edx
    mov dword ptr [rsp + 0xb8],edx
    lea r11,[r12 + rax*0x4]
    sub r13d,r15d
    sub r8d,r9d
    mov ebp,r8d
    nop
    .lbl18000ca70: xor r8d,r8d
    xor r9d,r9d
    cmp r13d,0x2
    mov eax,r15d
    mov rdi,r14
    jl .lbl18000cac0
    mov eax,edx
    mov rcx,r11
    sub eax,r15d
    sub eax,0x2
    shr eax,1
    add eax,0x1
    mov edx,eax
    lea eax,[r15 + rax*0x2]
    lea rdi,[r14 + rdx*0x2]
    nop
    .lbl18000caa0: add r8d,dword ptr [rcx]
    add r9d,dword ptr [rcx + 0x4]
    add rcx,0x8
    sub rdx,0x1
    jnz .lbl18000caa0
    mov edx,dword ptr [rsp + 0xb8]
    lea rcx,[rbx*0x4]
    .lbl18000cac0: cmp eax,edx
    jge .lbl18000cacc
    lea rax,[rdi + rsi*0x1]
    add r10d,dword ptr [r12 + rax*0x4]
    .lbl18000cacc: lea eax,[r9 + r8*0x1]
    add rsi,rbx
    add r11,rcx
    add r10d,eax
    sub rbp,0x1
    jnz .lbl18000ca70
    .lbl18000cadf: mov rax,qword ptr [rsp + 0xd0]
    mov rbx,qword ptr [rsp + 0xb0]
    mov dword ptr [rax],r10d
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
EXTR_downscale:
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x18],rbp
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x20
    mov rbp,qword ptr [rsp + 0x70]
    movsxd r14,r9d
    mov r15,rcx
    mov ecx,r14d
    mov edi,r8d
    mov eax,edx
    sar edi,cl
    sar eax,cl
    mov r12d,r8d
    mov ebx,edx
    imul eax,edi
    test eax,eax
    jle .lbl180012a5b
    mov r8d,eax
    xor edx,edx
    mov rcx,rbp
    shl r8,0x2
    call EXTR_memset wrt ..plt
    .lbl180012a5b: xor r9d,r9d
    test ebx,ebx
    movsxd r13,edi
    jle .lbl180012ac4
    mov qword ptr [rsp + 0x50],rsi
    xor r8d,r8d
    mov rdi,rbx
    .lbl180012a70: test r12d,r12d
    jle .lbl180012ab1
    mov rcx,r14
    mov r10,r8
    movsxd rax,r12d
    sar r10,cl
    xor r11d,r11d
    mov rsi,r12
    imul r10,r13
    imul rax,r9
    lea rbx,[r15 + rax*0x2]
    .lbl180012a93: movsx eax,word ptr [rbx]
    mov rdx,r11
    add rbx,0x2
    sar rdx,cl
    add r11,0x1
    add rdx,r10
    add dword ptr [rbp + rdx*0x4],eax
    sub rsi,0x1
    jnz .lbl180012a93
    .lbl180012ab1: add r8,0x1
    add r9,0x1
    sub rdi,0x1
    jnz .lbl180012a70
    mov rsi,qword ptr [rsp + 0x50]
    .lbl180012ac4: mov rbx,qword ptr [rsp + 0x58]
    mov rbp,qword ptr [rsp + 0x60]
    add rsp,0x20
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_average_img:
    sub rsp,0x18
    mov qword ptr [rsp + 0x28],rbp
    mov qword ptr [rsp + 0x8],r12
    movsxd r12,edx
    lea eax,[r12 + -0x1]
    xor r11d,r11d
    mov qword ptr [rsp],r13
    cmp eax,0x1
    movsxd rbp,r8d
    mov r13,rcx
    jle .lbl18001234d
    mov qword ptr [rsp + 0x20],rbx
    mov qword ptr [rsp + 0x30],rsi
    add eax,-0x1
    mov qword ptr [rsp + 0x10],rdi
    lea esi,[rbp + -0x1]
    mov rbx,r11
    mov edi,eax
    .lbl1800122d6: cmp esi,0x1
    jle .lbl180012334
    lea rax,[rbx + 0x1]
    mov rcx,rbp
    imul rax,rbp
    lea r10,[r9 + rax*0x4 + 0x4]
    lea rax,[rbx + 0x2]
    imul rax,rbp
    imul rcx,rbx
    lea r8,[r13 + rcx*0x4 + 0x8]
    lea rdx,[r13 + rax*0x4 + 0x8]
    lea ecx,[rsi + -0x1]
    nop
    nop
    nop
    .lbl180012310: mov eax,dword ptr [r8 + -0x8]
    add r10,0x4
    add r8,0x4
    sub eax,dword ptr [rdx + -0x8]
    add rdx,0x4
    sub eax,dword ptr [r8 + -0x4]
    add eax,dword ptr [rdx + -0x4]
    sub rcx,0x1
    mov dword ptr [r10 + -0x4],eax
    jnz .lbl180012310
    .lbl180012334: add rbx,0x1
    sub rdi,0x1
    jnz .lbl1800122d6
    mov rdi,qword ptr [rsp + 0x10]
    mov rsi,qword ptr [rsp + 0x30]
    mov rbx,qword ptr [rsp + 0x20]
    .lbl18001234d: mov r13,qword ptr [rsp]
    mov r10,rbp
    mov rbp,qword ptr [rsp + 0x28]
    test r10,r10
    jle .lbl1800123a2
    lea rax,[r12 + -0x1]
    lea r8,[r9 + r10*0x4]
    imul rax,r10
    lea rdx,[r9 + rax*0x4]
    lea rax,[r12 + -0x2]
    imul rax,r10
    lea rcx,[r9 + rax*0x4]
    nop
    .lbl180012380: mov eax,dword ptr [r8]
    add r11,0x1
    add rdx,0x4
    mov dword ptr [r9 + r11*0x4 + -0x4],eax
    mov eax,dword ptr [rcx]
    add r8,0x4
    mov dword ptr [rdx + -0x4],eax
    add rcx,0x4
    cmp r11,r10
    jl .lbl180012380
    .lbl1800123a2: mov rdx,r12
    mov r12,qword ptr [rsp + 0x8]
    test rdx,rdx
    jle .lbl1800123d8
    lea r8,[r10*0x4]
    lea rcx,[r8 + r9*0x1 + -0x4]
    nop
    .lbl1800123c0: mov eax,dword ptr [r9 + 0x4]
    mov dword ptr [r9],eax
    mov eax,dword ptr [rcx + -0x4]
    add r9,r8
    mov dword ptr [rcx],eax
    add rcx,r8
    sub rdx,0x1
    jnz .lbl1800123c0
    .lbl1800123d8: xor eax,eax
    add rsp,0x18
    ret

section .data


section .text
EXTR_FUN_18000d5c0:
    mov r11,rsp
    mov dword ptr [r11 + 0x20],r9d
    mov dword ptr [r11 + 0x18],r8d
    mov qword ptr [r11 + 0x8],rcx
    push rbx
    push rsi
    push rdi
    push r13
    push r15
    sub rsp,0x80
    mov rdi,qword ptr [rsp + 0xd0]
    mov eax,edx
    xor r15d,r15d
    cdq
    mov dword ptr [rsp + 0xb8],r15d
    and edx,0x7
    add eax,edx
    sar eax,0x3
    movsxd r9,eax
    mov eax,r8d
    cdq
    mov rbx,r9
    mov dword ptr [rsp + 0x30],r9d
    and edx,0x7
    mov qword ptr [rsp + 0x48],rbx
    add eax,edx
    sar eax,0x3
    movsxd rdx,eax
    mov esi,edx
    mov r13,rdx
    imul esi,r9d
    movsxd rax,esi
    mov dword ptr [rsp + 0x38],esi
    add rax,rdi
    mov qword ptr [rsp + 0x50],rax
    xor eax,eax
    test r9d,r9d
    mov qword ptr [rsp + 0x40],rax
    jle .lbl18000d795
    mov r15d,dword ptr [r11 + 0x20]
    mov qword ptr [r11 + -0x30],rbp
    mov qword ptr [r11 + -0x38],r12
    mov qword ptr [r11 + -0x40],r14
    mov r14,rdi
    nop
    nop
    nop
    .lbl18000d660: xor r12d,r12d
    test r13,r13
    jle .lbl18000d755
    movsxd rbp,r8d
    lea rsi,[rbp + rbp*0x1]
    imul rbp,rax
    shl rbp,0x4
    add rbp,rcx
    nop
    .lbl18000d680: xor r9d,r9d
    lea rdx,[rbp + 0xc]
    lea r8d,[r9 + 0x8]
    nop
    nop
    .lbl18000d690: movsx eax,word ptr [rdx + -0xa]
    movsx ecx,word ptr [rdx + -0xc]
    add ecx,eax
    movsx eax,word ptr [rdx + -0x8]
    add ecx,eax
    movsx eax,word ptr [rdx + -0x6]
    add ecx,eax
    movsx eax,word ptr [rdx + -0x4]
    add ecx,eax
    movsx eax,word ptr [rdx + -0x2]
    add ecx,eax
    movsx eax,word ptr [rdx + 0x2]
    add ecx,eax
    movsx eax,word ptr [rdx]
    add rdx,rsi
    add r9d,eax
    add r9d,ecx
    sub r8,0x1
    jnz .lbl18000d690
    mov eax,r9d
    lea r11d,[r8 + 0x8]
    mov r9,rbp
    cdq
    and edx,0x3f
    lea edi,[rdx + rax*0x1]
    sar edi,0x6
    xor ebx,ebx
    .lbl18000d6e0: mov r8,r9
    mov r10d,0x8
    nop
    nop
    .lbl18000d6f0: movsx ecx,word ptr [r8]
    mov eax,edi
    sub eax,ecx
    mov edx,ecx
    sub edx,edi
    cmp edx,eax
    cmovg eax,edx
    add r8,0x2
    cdq
    and edx,0x7
    add eax,edx
    sar eax,0x3
    add ebx,eax
    sub r10,0x1
    jnz .lbl18000d6f0
    add r9,rsi
    sub r11,0x1
    jnz .lbl18000d6e0
    cmp ebx,r15d
    setg al
    add r12,0x1
    add rbp,0x10
    cmp r12,r13
    mov byte ptr [r14 + r12*0x1 + -0x1],al
    jl .lbl18000d680
    mov rax,qword ptr [rsp + 0x40]
    mov rbx,qword ptr [rsp + 0x48]
    mov r8d,dword ptr [rsp + 0xc0]
    mov rcx,qword ptr [rsp + 0xb0]
    .lbl18000d755: add rax,0x1
    add r14,r13
    cmp rax,rbx
    mov qword ptr [rsp + 0x40],rax
    jl .lbl18000d660
    mov r15d,dword ptr [rsp + 0xb8]
    mov rdi,qword ptr [rsp + 0xd0]
    mov r9d,dword ptr [rsp + 0x30]
    mov esi,dword ptr [rsp + 0x38]
    mov r14,qword ptr [rsp + 0x68]
    mov r12,qword ptr [rsp + 0x70]
    mov rbp,qword ptr [rsp + 0x78]
    mov edx,r13d
    .lbl18000d795: mov r8d,edx
    mov rdx,qword ptr [rsp + 0x50]
    mov rcx,rdi
    mov dword ptr [rsp + 0x20],0x2
    call EXTR_FUN_1800047d0
    test rbx,rbx
    jle .lbl18000d80e
    xor r11d,r11d
    lea rdx,[rdi + 0x1]
    nop
    nop
    .lbl18000d7c0: xor r8d,r8d
    xor r9d,r9d
    xor ecx,ecx
    cmp r13,0x2
    jl .lbl18000d7eb
    lea r10,[r13 + -0x1]
    .lbl18000d7d2: movzx eax,byte ptr [rdx + rcx*0x1 + -0x1]
    add rcx,0x2
    add r8d,eax
    movzx eax,byte ptr [rdx + rcx*0x1 + -0x2]
    add r9d,eax
    cmp rcx,r10
    jl .lbl18000d7d2
    .lbl18000d7eb: cmp rcx,r13
    jge .lbl18000d7fb
    lea rax,[r11 + rcx*0x1]
    movzx ecx,byte ptr [rax + rdi*0x1]
    add r15d,ecx
    .lbl18000d7fb: lea ecx,[r9 + r8*0x1]
    add r11,r13
    add rdx,r13
    add r15d,ecx
    sub rbx,0x1
    jnz .lbl18000d7c0
    .lbl18000d80e: imul r15d,r15d,0x64
    mov ecx,0x64
    mov eax,r15d
    cdq
    idiv esi
    sub cl,al
    movzx eax,cl
    add rsp,0x80
    pop r15
    pop r13
    pop rdi
    pop rsi
    pop rbx
    ret

section .data


section .text
EXTR_FUN_1800047d0:
    EXTR_FUN_1800047d0: mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rsi
    push rdi
    sub rsp,0x60
    mov eax,dword ptr [rsp + 0xa0]
    movsxd rdi,r8d
    mov rbx,rdx
    cmp eax,0x1
    mov rsi,rcx
    jle .lbl180004a88
    add eax,-0x1
    mov r8d,edi
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_1800047d0
    .lbl180004813: movzx eax,byte ptr [rsi + 0x1]
    movsxd rcx,dword ptr [rsp + 0x98]
    mov qword ptr [rsp + 0x58],rbp
    and al,byte ptr [rsi]
    lea rbp,[rsi + 0x1]
    lea r11,[rdi + rbx*0x1]
    and al,byte ptr [rdi + rsi*0x1]
    lea r8,[rcx + -0x1]
    mov qword ptr [rsp + 0x38],rbp
    mov rbp,qword ptr [rsp + 0x88]
    mov byte ptr [rbx],al
    movzx eax,byte ptr [rdi + rsi*0x1 + -0x2]
    imul r8,rdi
    and al,byte ptr [rsi + rdi*0x2 + -0x1]
    lea rbx,[rcx + -0x2]
    lea rdx,[rsi + rdi*0x2 + -0x1]
    imul rcx,rdi
    imul rbx,rdi
    and al,byte ptr [rdi + rsi*0x1 + -0x1]
    lea r9,[rdi + rsi*0x1 + -0x1]
    mov r10,rdi
    mov byte ptr [r11 + -0x1],al
    movzx eax,byte ptr [r8 + rsi*0x1 + 0x1]
    mov qword ptr [rsp + 0x40],r11
    and al,byte ptr [r8 + rsi*0x1]
    and al,byte ptr [rbx + rsi*0x1]
    mov byte ptr [r8 + rbp*0x1],al
    movzx eax,byte ptr [rcx + rsi*0x1 + -0x2]
    and al,byte ptr [rcx + rsi*0x1 + -0x1]
    and al,byte ptr [r8 + rsi*0x1 + -0x1]
    mov byte ptr [rcx + rbp*0x1 + -0x1],al
    lea eax,[rdi + -0x1]
    lea rbp,[rsi + 0x1]
    cmp eax,0x1
    mov dword ptr [rsp + 0xa0],eax
    jle .lbl180004936
    mov rax,qword ptr [rsp + 0x88]
    mov ebp,dword ptr [rsp + 0xa0]
    lea rcx,[r8 + rsi*0x1 + 0x2]
    lea r11,[r8 + rax*0x1 + 0x1]
    lea rdi,[rax + 0x1]
    lea rax,[r10 + rsi*0x1 + 0x1]
    lea rbx,[rbx + rsi*0x1 + 0x1]
    lea r8,[rsi + 0x2]
    add ebp,-0x1
    mov rsi,rax
    .lbl1800048e2: movzx eax,byte ptr [r8 + -0x2]
    add r11,0x1
    add rsi,0x1
    and al,byte ptr [r8 + -0x1]
    add r8,0x1
    add rdi,0x1
    and al,byte ptr [rsi + -0x1]
    add rcx,0x1
    add rbx,0x1
    and al,byte ptr [r8 + -0x1]
    mov byte ptr [rdi + -0x1],al
    movzx eax,byte ptr [rcx + -0x3]
    and al,byte ptr [rcx + -0x2]
    and al,byte ptr [rcx + -0x1]
    and al,byte ptr [rbx + -0x1]
    sub rbp,0x1
    mov byte ptr [r11 + -0x1],al
    jnz .lbl1800048e2
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x80]
    mov r11,qword ptr [rsp + 0x40]
    .lbl180004936: mov ecx,dword ptr [rsp + 0x98]
    lea edi,[rcx + -0x1]
    cmp edi,0x1
    jle .lbl1800049b0
    lea rax,[rsi + r10*0x2]
    mov rcx,r11
    mov r8,rsi
    lea r11,[r10 + rax*0x1 + -0x1]
    mov rax,qword ptr [rsp + 0x88]
    lea ebx,[rdi + -0x1]
    sub rax,rsi
    mov rbp,rax
    nop
    nop
    nop
    .lbl180004970: movzx eax,byte ptr [rdx + 0x1]
    and al,byte ptr [r9 + 0x2]
    and al,byte ptr [r9 + 0x1]
    and al,byte ptr [r8]
    add r8,r10
    mov byte ptr [rcx],al
    movzx eax,byte ptr [rdx + -0x1]
    add rcx,r10
    and al,byte ptr [r11]
    add r11,r10
    and al,byte ptr [rdx]
    and al,byte ptr [r9]
    add r9,r10
    mov byte ptr [rdx + rbp*0x1],al
    add rdx,r10
    sub rbx,0x1
    jnz .lbl180004970
    mov ecx,dword ptr [rsp + 0x98]
    lea rbp,[rsi + 0x1]
    .lbl1800049b0: cmp edi,0x1
    jle .lbl180004a57
    mov rax,qword ptr [rsp + 0x88]
    mov r11d,dword ptr [rsp + 0xa0]
    lea rbx,[r10 + rsi*0x1 + 0x2]
    lea rdi,[r10 + rax*0x1 + 0x1]
    lea eax,[rcx + -0x1]
    lea rsi,[rsi + r10*0x2 + 0x1]
    add eax,-0x1
    mov qword ptr [rsp + 0x30],rax
    .lbl1800049e3: cmp r11d,0x1
    jle .lbl180004a38
    mov rdx,rdi
    mov rcx,rbx
    mov r8,rsi
    mov r9,rbp
    add r11d,-0x1
    nop
    nop
    .lbl180004a00: movzx eax,byte ptr [rcx + -0x2]
    add rdx,0x1
    add r9,0x1
    and al,byte ptr [rcx + -0x1]
    add r8,0x1
    add rcx,0x1
    and al,byte ptr [r9 + -0x1]
    and al,byte ptr [rcx + -0x1]
    and al,byte ptr [r8 + -0x1]
    sub r11,0x1
    mov byte ptr [rdx + -0x1],al
    jnz .lbl180004a00
    mov rax,qword ptr [rsp + 0x30]
    mov r11d,dword ptr [rsp + 0xa0]
    .lbl180004a38: add rdi,r10
    add rbx,r10
    add rsi,r10
    add rbp,r10
    sub rax,0x1
    mov qword ptr [rsp + 0x30],rax
    jnz .lbl1800049e3
    mov rsi,qword ptr [rsp + 0x80]
    .lbl180004a57: mov r8d,dword ptr [rsp + 0x90]
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
    .lbl180004a88: cmp eax,0x1
    jge .lbl180004813
    mov rax,rcx
    add rsp,0x60
    pop rdi
    pop rsi
    pop rbx
    ret

section .data


section .text
EXTR_FUN_18000bfe0:
    sub rsp,0x38
    mov rax,qword ptr [rsp + 0x60]
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180015150
    add rsp,0x38
    ret

section .data


section .text
EXTR_FUN_180015150:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    movzx ebp,word ptr [rdx + 0x12]
    movzx esi,word ptr [rdx + 0x14]
    movzx eax,word ptr [rdx + 0x1c]
    mov r13d,ebp
    mov r14,r9
    mov r15d,r8d
    mov rdi,rdx
    mov r12,rcx
    imul r13d,esi
    cmp eax,0x8
    ja .lbl18001555e
    lea rcx,[EXTR_FUN_180015150]
    mov r9d,dword ptr [rcx + rax*0x4 + (.jt1800151a6 - EXTR_FUN_180015150)]
    add r9,rcx
    jmp r9
    .lbl1800151a9: movzx ecx,word ptr [rdx + 0x22]
    movzx r9d,word ptr [rdx + 0x24]
    movzx r10d,word ptr [rdx + 0x20]
    mov rax,qword ptr [rsp + 0xa0]
    neg r9d
    mov r8d,ebp
    mov qword ptr [rsp + 0x38],rax
    mov rax,qword ptr [rdx + 0x60]
    mov edx,esi
    mov qword ptr [rsp + 0x30],rax
    mov dword ptr [rsp + 0x28],ecx
    mov rcx,r12
    mov dword ptr [rsp + 0x20],r10d
    call EXTR_FUN_1800139f0
    jmp .lbl180015582
    .lbl1800151eb: movzx r9d,word ptr [rdx + 0x24]
    mov rax,qword ptr [rdx + 0x60]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    neg r9d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180011d20
    jmp .lbl180015582
    .lbl18001520e: mov rax,qword ptr [rsp + 0xa0]
    mov r9,qword ptr [rdx + 0x60]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_1800110c0
    jmp .lbl180015582
    .lbl180015231: movzx r9d,word ptr [rdx + 0x24]
    mov rax,qword ptr [rdx + 0x60]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    neg r9d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_180011a60
    movzx r11d,word ptr [rdi + 0x20]
    cmp r11w,0x7fff
    jnc .lbl180015582
    movzx ecx,r11w
    mov eax,0x55555556
    imul ecx
    mov eax,0x10624dd3
    mov r8d,edx
    shr r8d,0x1f
    add r8d,edx
    mov ecx,r8d
    movzx r9d,r8w
    mov r8d,ebp
    imul ecx,ecx,-0x6bb
    imul ecx
    mov rcx,r12
    sar edx,0x6
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov rax,qword ptr [rsp + 0xa0]
    mov qword ptr [rsp + 0x38],rax
    mov rax,qword ptr [rdi + 0x60]
    mov qword ptr [rsp + 0x30],rax
    mov dword ptr [rsp + 0x28],0xa
    mov word ptr [rsp + 0x20],dx
    mov edx,esi
    call EXTR_FUN_180011580
    jmp .lbl180015582
    .lbl1800152c8: movzx eax,word ptr [rdx + 0x22]
    mov r9,qword ptr [rdx + 0x60]
    mov rbx,qword ptr [rsp + 0xa0]
    mov dword ptr [rsp + 0x30],r8d
    mov dword ptr [rsp + 0x28],eax
    mov edx,esi
    mov r8d,ebp
    mov rcx,r12
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180013c70
    test r15d,r15d
    jz .lbl180015312
    mov eax,dword ptr [rbx]
    mov dword ptr [r14],eax
    mov eax,dword ptr [rbx + 0x4]
    mov dword ptr [r14 + 0x4],eax
    mov eax,dword ptr [rbx + 0x8]
    mov dword ptr [r14 + 0x8],eax
    mov eax,dword ptr [rbx + 0xc]
    mov dword ptr [r14 + 0xc],eax
    .lbl180015312: movzx eax,word ptr [rdi + 0x20]
    cmp ax,0x7fff
    jnc .lbl18001535a
    movzx ecx,ax
    mov qword ptr [rsp + 0x38],rbx
    mov eax,0x55555556
    mov r8d,ebp
    imul ecx
    mov rax,qword ptr [rdi + 0x60]
    mov r9d,edx
    mov qword ptr [rsp + 0x30],rax
    shr r9d,0x1f
    mov rcx,r12
    add r9d,edx
    mov edx,esi
    mov dword ptr [rsp + 0x28],0x1
    mov word ptr [rsp + 0x20],0x8000
    call EXTR_FUN_180011580
    .lbl18001535a: mov rcx,qword ptr [rdi + 0x60]
    mov r9d,0x8
    mov r8d,ebp
    mov edx,esi
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180011500
    jmp .lbl180015582
    .lbl180015378: mov r8,qword ptr [rdx + 0x60]
    xor r9d,r9d
    mov ecx,esi
    mov edx,ebp
    call EXTR_FUN_180011540
    movzx eax,word ptr [rdi + 0x22]
    mov r9,qword ptr [rdi + 0x60]
    mov dword ptr [rsp + 0x30],0xc
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    mov dword ptr [rsp + 0x28],0x14
    mov word ptr [rsp + 0x20],ax
    call EXTR_FUN_180011610
    movzx r11d,word ptr [rdi + 0x24]
    mov rbx,qword ptr [rsp + 0xa0]
    mov r9,qword ptr [rdi + 0x60]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    mov qword ptr [rsp + 0x28],rbx
    mov dword ptr [rsp + 0x20],r11d
    call EXTR_FUN_1800136d0
    movzx eax,word ptr [rdi + 0x20]
    mov r9,qword ptr [rdi + 0x60]
    mov dword ptr [rsp + 0x30],0x8
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    mov dword ptr [rsp + 0x28],0x28
    mov word ptr [rsp + 0x20],ax
    call EXTR_FUN_180011800
    mov rcx,qword ptr [rdi + 0x60]
    mov r9d,0x1b1
    mov r8d,ebp
    mov edx,esi
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180012f50
    mov rcx,qword ptr [rdi + 0x60]
    mov r9d,0x8
    mov r8d,ebp
    mov edx,esi
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180011500
    jmp .lbl180015582
    .lbl18001543b: movzx ecx,word ptr [rdx + 0x22]
    movzx eax,word ptr [rdx + 0x1e]
    mov r9,qword ptr [rdx + 0x60]
    mov rbx,qword ptr [rsp + 0xa0]
    mov dword ptr [rsp + 0x30],eax
    mov dword ptr [rsp + 0x28],ecx
    mov rcx,r12
    mov r8d,ebp
    mov edx,esi
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_1800134b0
    movzx r9d,word ptr [rdi + 0x24]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180011a60
    mov rcx,qword ptr [rdi + 0x60]
    mov r8d,r13d
    mov rdx,rbx
    call EXTR_FUN_180004e50
    mov rcx,qword ptr [rdi + 0x60]
    mov edx,r13d
    call EXTR_FUN_180004e20
    movzx r9d,word ptr [rdi + 0x20]
    mov rcx,qword ptr [rdi + 0x60]
    mov r8d,ebp
    mov edx,esi
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180012f50
    mov rcx,qword ptr [rdi + 0x60]
    mov edx,r13d
    call EXTR_FUN_180004e20
    jmp .lbl180015582
    .lbl1800154c4: movzx ecx,word ptr [rdx + 0x24]
    movzx edx,word ptr [rdx + 0x20]
    movsx r8d,word ptr [rdi + 0x1e]
    mov rax,qword ptr [rsp + 0xa0]
    movzx r9d,word ptr [rdi + 0x22]
    mov qword ptr [rsp + 0x48],rax
    mov rax,qword ptr [rdi + 0x60]
    mov qword ptr [rsp + 0x40],rax
    mov dword ptr [rsp + 0x38],0x3
    mov dword ptr [rsp + 0x30],ecx
    mov dword ptr [rsp + 0x28],edx
    mov dword ptr [rsp + 0x20],r8d
    mov rcx,r12
    mov r8d,ebp
    mov edx,esi
    call EXTR_FUN_180014df0
    jmp .lbl180015582
    .lbl180015510: movzx ecx,word ptr [rdx + 0x24]
    movzx edx,word ptr [rdx + 0x20]
    movzx r8d,word ptr [rdi + 0x1e]
    mov rax,qword ptr [rsp + 0xa0]
    movzx r10d,word ptr [rdi + 0x22]
    movzx r9d,word ptr [rdi + 0x16]
    mov qword ptr [rsp + 0x48],rax
    mov rax,qword ptr [rdi + 0x60]
    mov qword ptr [rsp + 0x40],rax
    mov dword ptr [rsp + 0x38],ecx
    mov dword ptr [rsp + 0x30],edx
    mov dword ptr [rsp + 0x28],r8d
    mov edx,esi
    mov rcx,r12
    mov r8d,ebp
    mov dword ptr [rsp + 0x20],r10d
    call EXTR_FUN_180014340
    jmp .lbl180015582
    .lbl18001555e: xor ecx,ecx
    test r13d,r13d
    movsxd rdx,r13d
    jle .lbl180015582
    nop
    nop
    .lbl180015570: mov rax,qword ptr [rdi + 0x60]
    add rcx,0x1
    cmp rcx,rdx
    mov byte ptr [rcx + rax*0x1 + -0x1],0x1
    jl .lbl180015570
    .lbl180015582: lea r11,[rsp + 0x50]
    mov byte ptr [rdi + 0x58],0x1
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
    .jt1800151a6 dq (.lbl1800151a9 - EXTR_FUN_180015150), (.lbl1800151eb - EXTR_FUN_180015150), (.lbl18001520e - EXTR_FUN_180015150), (.lbl180015231 - EXTR_FUN_180015150), (.lbl1800152c8 - EXTR_FUN_180015150), (.lbl180015378 - EXTR_FUN_180015150), (.lbl18001543b - EXTR_FUN_180015150), (.lbl1800154c4 - EXTR_FUN_180015150), (.lbl180015510 - EXTR_FUN_180015150)


section .text
EXTR_FUN_1800134b0:
    mov qword ptr [rsp + 0x8],rbx
    mov dword ptr [rsp + 0x10],edx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov rdi,qword ptr [rsp + 0x90]
    mov r13d,r8d
    mov ebp,r8d
    mov r14d,edx
    mov r8d,edx
    mov r15,r9
    sar r13d,1
    sar r14d,1
    mov r9d,0x1
    imul r8d,ebp
    mov eax,r13d
    mov dword ptr [rsp + 0x80],r8d
    mov r8d,ebp
    mov qword ptr [rsp + 0x20],rdi
    imul eax,r14d
    movsxd rsi,eax
    lea r12,[rdi + rsi*0x4]
    lea rbx,[r12 + rsi*0x4]
    call EXTR_downscale
    mov r9,r12
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rdi
    call EXTR_average_img
    xor edx,edx
    test rsi,rsi
    jle .lbl18001354b
    nop
    .lbl180013530: mov eax,dword ptr [r12 + rdx*0x4]
    mov ecx,eax
    neg ecx
    cmp eax,ecx
    cmovg ecx,eax
    add rdx,0x1
    cmp rdx,rsi
    mov dword ptr [r12 + rdx*0x4 + -0x4],ecx
    jl .lbl180013530
    .lbl18001354b: mov r9,rdi
    mov r8d,r13d
    mov edx,r14d
    mov rcx,r12
    call EXTR_average_img
    xor r11d,r11d
    test rsi,rsi
    jle .lbl180013596
    mov edx,dword ptr [rsp + 0x98]
    nop
    nop
    .lbl180013570: mov ecx,dword ptr [rdi + r11*0x4]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    mov eax,ecx
    jg .lbl180013580
    neg eax
    .lbl180013580: sar eax,0x2
    cmp eax,edx
    setg al
    add r11,0x1
    cmp r11,rsi
    mov byte ptr [r11 + rbx*0x1 + -0x1],al
    jl .lbl180013570
    .lbl180013596: mov r9d,dword ptr [rsp + 0xa0]
    mov r8d,r13d
    mov rcx,rbx
    test r9d,r9d
    jle .lbl1800135b8
    mov edx,r14d
    mov qword ptr [rsp + 0x20],rdi
    call EXTR_FUN_180012f50
    jmp .lbl1800135cb
    .lbl1800135b8: mov r9d,r14d
    mov rdx,r12
    mov dword ptr [rsp + 0x20],0x3
    call EXTR_FUN_180004d60
    .lbl1800135cb: mov r9,r12
    mov r8d,r14d
    mov edx,r13d
    mov rcx,rbx
    call EXTR_FUN_180015da0
    mov r9d,r14d
    mov r8d,r13d
    mov rdx,r12
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_1800047d0
    mov r9d,0x1
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rbx
    call EXTR_FUN_180010550
    mov r14d,dword ptr [rsp + 0x78]
    xor r9d,r9d
    test r14d,r14d
    movsxd r13,r13d
    jle .lbl18001366c
    xor r11d,r11d
    mov rsi,r14
    nop
    .lbl180013620: test ebp,ebp
    jle .lbl18001365e
    movsxd rcx,ebp
    mov r10,r11
    mov r8,rbp
    sar r10,1
    imul rcx,r9
    add rcx,r15
    imul r10,r13
    xor edx,edx
    nop
    .lbl180013640: mov rax,rdx
    add rcx,0x1
    add rdx,0x1
    sar rax,1
    add rax,r10
    sub r8,0x1
    movzx eax,byte ptr [rax + rbx*0x1]
    mov byte ptr [rcx + -0x1],al
    jnz .lbl180013640
    .lbl18001365e: add r11,0x1
    add r9,0x1
    sub rsi,0x1
    jnz .lbl180013620
    .lbl18001366c: mov r9,r12
    mov r8d,r14d
    mov edx,ebp
    mov rcx,r15
    call EXTR_FUN_180015da0
    mov ebx,dword ptr [rsp + 0x80]
    mov rcx,r15
    mov edx,ebx
    call EXTR_FUN_180004e20
    mov r9d,0xc8
    mov r8d,ebp
    mov edx,r14d
    mov rcx,r15
    mov qword ptr [rsp + 0x20],rdi
    call EXTR_FUN_180012f50
    mov edx,ebx
    mov rcx,r15
    call EXTR_FUN_180004e20
    mov rbx,qword ptr [rsp + 0x70]
    xor eax,eax
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
EXTR_FUN_180004d60:
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
    call EXTR_FUN_180004aa0
    mov r9d,edi
    mov r8d,esi
    mov rdx,rbp
    mov rcx,rax
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_1800047d0
    mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x30
    pop rdi
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
EXTR_FUN_180013c70:
    mov r11,rsp
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0xc8
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0xb0],rax
    mov r14,qword ptr [rsp + 0x130]
    xor ebx,ebx
    mov r13d,r8d
    mov ebp,edx
    mov r15d,r8d
    mov qword ptr [rsp + 0x60],r9
    sar r13d,1
    sar ebp,1
    lea r9d,[rbx + 0x1]
    imul r15d,edx
    mov eax,r13d
    mov dword ptr [rsp + 0x48],r8d
    mov dword ptr [rsp + 0x58],edx
    mov qword ptr [rsp + 0x70],r14
    mov word ptr [r11 + -0x60],0x22ae
    mov word ptr [r11 + -0x5e],0x5d81
    imul eax,ebp
    movsxd rdi,eax
    mov word ptr [r11 + -0x5c],0x22ae
    mov dword ptr [rsp + 0x68],r15d
    mov dword ptr [rsp + 0x40],ebx
    mov dword ptr [rsp + 0x54],ebx
    mov dword ptr [rsp + 0x44],ebx
    lea r12,[r14 + rdi*0x2]
    lea rsi,[r14 + rdi*0x4]
    mov dword ptr [rsp + 0x4c],ebx
    mov dword ptr [rsp + 0x50],ebx
    mov qword ptr [rsp + 0x20],r14
    call EXTR_downscale
    mov r9,rsi
    mov r8d,r13d
    mov edx,ebp
    mov rcx,r14
    call EXTR_average_img
    test r15d,r15d
    mov r11,rbx
    movsxd r8,r15d
    jle .lbl180013db7
    mov r9d,0xffff8000
    nop
    nop
    .lbl180013d40: mov eax,dword ptr [rsi + r11*0x4]
    sar eax,0x2
    cmp eax,0x7fff
    jle .lbl180013d55
    mov edx,0x7fff
    jmp .lbl180013d5e
    .lbl180013d55: mov edx,eax
    cmp eax,r9d
    cmovl edx,r9d
    .lbl180013d5e: cmp eax,0x7fff
    jle .lbl180013d6c
    mov ecx,0x7fff
    jmp .lbl180013d75
    .lbl180013d6c: mov ecx,eax
    cmp eax,r9d
    cmovl ecx,r9d
    .lbl180013d75: neg ecx
    cmp edx,ecx
    jle .lbl180013d92
    cmp eax,0x7fff
    jle .lbl180013d89
    mov eax,0x7fff
    jmp .lbl180013da9
    .lbl180013d89: cmp eax,r9d
    cmovl eax,r9d
    jmp .lbl180013da9
    .lbl180013d92: cmp eax,0x7fff
    jle .lbl180013da0
    mov eax,0x7fff
    jmp .lbl180013da7
    .lbl180013da0: cmp eax,r9d
    cmovl eax,r9d
    .lbl180013da7: neg eax
    .lbl180013da9: mov word ptr [r12 + r11*0x2],ax
    add r11,0x1
    cmp r11,r8
    jl .lbl180013d40
    .lbl180013db7: mov qword ptr [rsp + 0x30],r14
    lea r9,[rsp + 0xa8]
    mov r8d,r13d
    mov edx,ebp
    mov rcx,r12
    mov dword ptr [rsp + 0x28],0x3
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    mov qword ptr [rsp + 0x30],r12
    lea r9,[rsp + 0xa8]
    mov r8d,r13d
    mov edx,ebp
    mov rcx,r14
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x3
    call EXTR_FUN_180002820
    test rdi,rdi
    jle .lbl180013e3f
    mov r8,r14
    mov rdx,r12
    sub r8,r12
    nop
    nop
    .lbl180013e20: movsx ecx,word ptr [rdx]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    movsx eax,word ptr [rdx]
    jg .lbl180013e30
    neg eax
    .lbl180013e30: mov word ptr [r8 + rdx*0x1],ax
    add rdx,0x2
    sub rdi,0x1
    jnz .lbl180013e20
    .lbl180013e3f: movsxd rbp,dword ptr [rsp + 0x58]
    movsxd r15,r13d
    movsxd r13,dword ptr [rsp + 0x48]
    test rbp,rbp
    mov r9d,ebx
    mov qword ptr [rsp + 0xa0],r15
    mov rdi,r13
    mov qword ptr [rsp + 0x98],r13
    mov qword ptr [rsp + 0x78],rbp
    jle .lbl180013ed2
    mov r10,qword ptr [rsp + 0x60]
    mov r11d,dword ptr [rsp + 0x138]
    mov rsi,rbp
    nop
    .lbl180013e80: test r13d,r13d
    mov edx,ebx
    mov rcx,rbx
    jle .lbl180013ec5
    movsxd r8,r9d
    sar r8,1
    imul r8,r15
    nop
    nop
    nop
    .lbl180013ea0: movsxd rax,edx
    sar rax,1
    add rax,r8
    movsx eax,word ptr [r14 + rax*0x2]
    cmp eax,r11d
    setg al
    add rcx,0x1
    add edx,0x1
    cmp rcx,r13
    mov byte ptr [rcx + r10*0x1 + -0x1],al
    jl .lbl180013ea0
    .lbl180013ec5: add r9d,0x1
    add r10,r13
    sub rsi,0x1
    jnz .lbl180013e80
    .lbl180013ed2: mov r14d,dword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    mov r8d,r13d
    mov edx,r14d
    mov rcx,rsi
    call EXTR_FUN_180011ea0
    mov r9,r12
    mov r8d,r14d
    mov edx,r13d
    mov rcx,rsi
    call EXTR_FUN_180015da0
    cmp dword ptr [rsp + 0x140],0x1
    jnz .lbl18001430c
    test r14d,r14d
    jle .lbl1800140c5
    mov r12d,dword ptr [rsp + 0x58]
    mov eax,dword ptr [rsp + 0x48]
    mov r14,rbx
    mov qword ptr [rsp + 0x90],rbx
    mov r8d,ebx
    mov dword ptr [rsp + 0xa8],ebx
    mov qword ptr [rsp + 0x88],r12
    mov r13,rsi
    mov qword ptr [rsp + 0x80],rsi
    .lbl180013f43: cmp eax,0x2
    mov r10d,ebx
    mov r11d,ebx
    mov esi,ebx
    mov ebp,ebx
    mov r9d,ebx
    jl .lbl180014018
    mov r8,r14
    mov r14,qword ptr [rsp + 0x70]
    add eax,-0x2
    sar r8,1
    shr eax,1
    mov rdi,r13
    mov r12,rbx
    imul r8,r15
    add eax,0x1
    lea r15,[r13 + 0x1]
    lea r9d,[rax + rax*0x1]
    mov r13d,eax
    .lbl180013f81: movzx edx,byte ptr [rdi]
    mov eax,ebx
    test dl,dl
    setz al
    add esi,eax
    mov rax,r12
    sar rax,1
    add rax,r8
    test dl,dl
    movzx edx,byte ptr [r15]
    movsx ecx,word ptr [r14 + rax*0x2]
    mov eax,ebx
    setnz al
    imul ecx,eax
    add r10d,ecx
    test dl,dl
    mov eax,ebx
    setz al
    add ebp,eax
    lea rax,[r12 + 0x1]
    sar rax,1
    add rax,r8
    test dl,dl
    movsx ecx,word ptr [r14 + rax*0x2]
    mov eax,ebx
    setnz al
    add rdi,0x2
    add r12,0x2
    add r15,0x2
    imul ecx,eax
    add r11d,ecx
    sub r13,0x1
    jnz .lbl180013f81
    mov rdi,qword ptr [rsp + 0x98]
    mov r14,qword ptr [rsp + 0x90]
    mov r13,qword ptr [rsp + 0x80]
    mov r8d,dword ptr [rsp + 0xa8]
    mov r12,qword ptr [rsp + 0x88]
    mov r15,qword ptr [rsp + 0xa0]
    mov eax,dword ptr [rsp + 0x48]
    .lbl180014018: cmp r9d,eax
    jge .lbl180014066
    lea eax,[r8 + r9*0x1]
    movsxd rcx,eax
    mov rax,qword ptr [rsp + 0x60]
    movzx edx,byte ptr [rcx + rax*0x1]
    mov eax,ebx
    mov rcx,r14
    test dl,dl
    setz al
    sar rcx,1
    add dword ptr [rsp + 0x40],eax
    movsxd rax,r9d
    sar rax,1
    imul rcx,r15
    add rcx,rax
    mov rax,qword ptr [rsp + 0x70]
    test dl,dl
    movsx ecx,word ptr [rax + rcx*0x2]
    mov edx,dword ptr [rsp + 0x54]
    mov eax,ebx
    setnz al
    imul ecx,eax
    add edx,ecx
    jmp .lbl18001406a
    .lbl180014066: mov edx,dword ptr [rsp + 0x54]
    .lbl18001406a: lea eax,[r10 + r11*0x1]
    add r13,rdi
    add r14,0x1
    add edx,eax
    lea eax,[rsi + rbp*0x1]
    mov qword ptr [rsp + 0x80],r13
    add dword ptr [rsp + 0x40],eax
    mov eax,dword ptr [rsp + 0x48]
    mov dword ptr [rsp + 0x54],edx
    add r8d,eax
    sub r12,0x1
    mov qword ptr [rsp + 0x90],r14
    mov qword ptr [rsp + 0x88],r12
    mov dword ptr [rsp + 0xa8],r8d
    jnz .lbl180013f43
    mov rbp,qword ptr [rsp + 0x78]
    mov rsi,qword ptr [rsp + 0x60]
    mov r14d,dword ptr [rsp + 0x58]
    mov r13d,eax
    .lbl1800140c5: test rbp,rbp
    jle .lbl180014180
    mov r12,qword ptr [rsp + 0x78]
    mov rbp,rbx
    lea r9,[rsi + 0x1]
    nop
    nop
    .lbl1800140e0: cmp rdi,0x2
    mov r10d,ebx
    mov r11d,ebx
    mov r8d,ebx
    mov rdx,rbx
    jl .lbl180014138
    lea rsi,[rdi + -0x1]
    nop
    nop
    nop
    .lbl180014100: mov eax,ebx
    mov ecx,ebx
    cmp byte ptr [r9 + rdx*0x1 + -0x1],al
    setnz al
    imul eax,r8d
    cmp byte ptr [r9 + rdx*0x1],cl
    setnz cl
    lea r10d,[r10 + rax*0x2]
    lea eax,[r8 + 0x1]
    add rdx,0x2
    add r8d,0x2
    imul ecx,eax
    cmp rdx,rsi
    lea r11d,[r11 + rcx*0x2]
    jl .lbl180014100
    mov rsi,qword ptr [rsp + 0x60]
    .lbl180014138: cmp rdx,rdi
    jge .lbl180014158
    mov ecx,ebx
    lea rax,[rdx + rbp*0x1]
    cmp byte ptr [rax + rsi*0x1],cl
    setnz cl
    imul ecx,r8d
    mov r8d,dword ptr [rsp + 0x44]
    lea r8d,[r8 + rcx*0x2]
    jmp .lbl18001415d
    .lbl180014158: mov r8d,dword ptr [rsp + 0x44]
    .lbl18001415d: lea eax,[r11 + r10*0x1]
    add rbp,rdi
    add r9,rdi
    add r8d,eax
    sub r12,0x1
    mov dword ptr [rsp + 0x44],r8d
    jnz .lbl1800140e0
    mov rbp,qword ptr [rsp + 0x78]
    jmp .lbl180014183
    .lbl180014180: mov r8d,ebx
    .lbl180014183: mov r9d,dword ptr [rsp + 0x40]
    mov r11d,dword ptr [rsp + 0x68]
    cmp r9d,r11d
    jge .lbl1800141b5
    mov eax,r8d
    lea ecx,[r13 + r13*0x1]
    cdq
    idiv ecx
    mov ecx,r11d
    sub ecx,r9d
    imul eax,eax,0xc8
    cdq
    idiv ecx
    lea r8d,[rax + -0x64]
    mov dword ptr [rsp + 0x44],r8d
    .lbl1800141b5: mov eax,r14d
    cdq
    sub eax,edx
    sar eax,1
    test eax,eax
    jle .lbl1800142b8
    lea r12,[rbp + -0x1]
    mov r14,rdi
    mov r13,rbx
    neg r14
    lea r8,[rsi + 0x1]
    mov r15d,eax
    imul r12,rdi
    lea r9,[r12 + rsi*0x1 + 0x1]
    .lbl1800141e2: cmp rdi,0x2
    mov r10d,ebx
    mov r11d,ebx
    mov esi,ebx
    mov ebp,ebx
    mov rdx,rbx
    jl .lbl180014239
    lea rcx,[rdi + -0x1]
    nop
    nop
    .lbl180014200: mov eax,ebx
    cmp byte ptr [r8 + rdx*0x1 + -0x1],al
    setz al
    add esi,eax
    mov eax,ebx
    cmp byte ptr [r9 + rdx*0x1 + -0x1],al
    setz al
    add r10d,eax
    mov eax,ebx
    cmp byte ptr [r8 + rdx*0x1],al
    setz al
    add ebp,eax
    mov eax,ebx
    cmp byte ptr [r9 + rdx*0x1],al
    setz al
    add rdx,0x2
    add r11d,eax
    cmp rdx,rcx
    jl .lbl180014200
    .lbl180014239: cmp rdx,rdi
    jge .lbl180014272
    mov rdi,qword ptr [rsp + 0x60]
    lea rax,[rdx + r13*0x1]
    mov ecx,ebx
    cmp byte ptr [rax + rdi*0x1],cl
    lea rax,[r12 + rdx*0x1]
    mov rdx,rdi
    mov rdi,qword ptr [rsp + 0x98]
    setz cl
    add dword ptr [rsp + 0x4c],ecx
    mov ecx,ebx
    cmp byte ptr [rax + rdx*0x1],cl
    mov edx,dword ptr [rsp + 0x50]
    setz cl
    add edx,ecx
    jmp .lbl180014276
    .lbl180014272: mov edx,dword ptr [rsp + 0x50]
    .lbl180014276: lea eax,[r11 + r10*0x1]
    mov r10d,dword ptr [rsp + 0x4c]
    add r13,rdi
    add edx,eax
    lea eax,[rbp + rsi*0x1]
    add r8,rdi
    add r10d,eax
    add r12,r14
    add r9,r14
    sub r15,0x1
    mov dword ptr [rsp + 0x4c],r10d
    mov dword ptr [rsp + 0x50],edx
    jnz .lbl1800141e2
    mov r8d,dword ptr [rsp + 0x44]
    mov r9d,dword ptr [rsp + 0x40]
    mov r11d,dword ptr [rsp + 0x68]
    jmp .lbl1800142bd
    .lbl1800142b8: mov edx,ebx
    mov r10d,ebx
    .lbl1800142bd: mov ecx,dword ptr [rsp + 0x48]
    add ecx,ecx
    cmp r10d,ecx
    jg .lbl1800142cc
    cmp edx,ecx
    jle .lbl1800142eb
    .lbl1800142cc: cmp edx,r10d
    jle .lbl1800142dc
    mov eax,edx
    lea ecx,[r10 + 0x1]
    cdq
    idiv ecx
    jmp .lbl1800142e9
    .lbl1800142dc: jge .lbl1800142eb
    lea ecx,[rdx + 0x1]
    mov eax,r10d
    cdq
    idiv ecx
    neg eax
    .lbl1800142e9: mov ebx,eax
    .lbl1800142eb: mov rcx,qword ptr [rsp + 0x70]
    mov eax,dword ptr [rsp + 0x54]
    sub r11d,r9d
    cdq
    add r11d,0x1
    mov dword ptr [rcx],r9d
    mov dword ptr [rcx + 0x8],r8d
    mov dword ptr [rcx + 0xc],ebx
    idiv r11d
    mov dword ptr [rcx + 0x4],eax
    .lbl18001430c: xor eax,eax
    mov rcx,qword ptr [rsp + 0xb0]
    xor rcx,rsp
    call EXTR_check_cookie
    add rsp,0xc8
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
EXTR_FUN_180011ea0:
    mov qword ptr [rsp + 0x8],rbx
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov eax,edx
    mov r10d,edx
    xor r14d,r14d
    cdq
    mov r15,rcx
    mov ecx,r8d
    sub eax,edx
    mov ebp,r14d
    mov r12,r14
    sar eax,1
    movsxd rbx,r8d
    mov r13d,eax
    mov dword ptr [rsp + 0x20],eax
    mov eax,r8d
    cdq
    and edx,0x3
    add eax,edx
    sar eax,0x2
    sub ecx,eax
    test r13d,r13d
    mov dword ptr [rsp + 0x88],eax
    movsxd rdi,ecx
    jle .lbl180011f6a
    mov rsi,r14
    .lbl180011f00: movsxd rdx,eax
    cmp rdx,rdi
    mov rcx,rdx
    jge .lbl180011f35
    mov r8,rbx
    imul r8,r12
    add r8,r15
    .lbl180011f15: cmp byte ptr [r8 + rcx*0x1],r14b
    ja .lbl180011f29
    add rcx,0x1
    add edx,0x1
    cmp rcx,rdi
    jl .lbl180011f15
    jmp .lbl180011f35
    .lbl180011f29: movsxd rax,edx
    add rax,rsi
    cmp byte ptr [rax + r15*0x1],r14b
    jnz .lbl180011f5e
    .lbl180011f35: test rbx,rbx
    jle .lbl180011f48
    lea rcx,[rsi + r15*0x1]
    mov r8,rbx
    xor edx,edx
    call EXTR_memset wrt ..plt
    .lbl180011f48: mov eax,dword ptr [rsp + 0x88]
    add ebp,0x1
    add r12,0x1
    add rsi,rbx
    cmp ebp,r13d
    jl .lbl180011f00
    .lbl180011f5e: mov eax,dword ptr [rsp + 0x88]
    mov r10d,dword ptr [rsp + 0x78]
    .lbl180011f6a: lea esi,[r10 + -0x1]
    cmp esi,r13d
    movsxd r12,esi
    jle .lbl180011ffb
    mov rbp,rbx
    mov r14,rbx
    imul rbp,r12
    neg r14
    lea r13,[r15 + rbp*0x1]
    nop
    nop
    .lbl180011f90: movsxd rdx,eax
    cmp rdx,rdi
    mov rcx,rdx
    jge .lbl180011fc7
    mov r8,rbx
    imul r8,r12
    add r8,r15
    .lbl180011fa5: cmp byte ptr [r8 + rcx*0x1],0x0
    ja .lbl180011fba
    add rcx,0x1
    add edx,0x1
    cmp rcx,rdi
    jl .lbl180011fa5
    jmp .lbl180011fc7
    .lbl180011fba: movsxd rax,edx
    add rax,rbp
    cmp byte ptr [rax + r15*0x1],0x0
    jnz .lbl180011ff3
    .lbl180011fc7: test rbx,rbx
    jle .lbl180011fd9
    mov r8,rbx
    xor edx,edx
    mov rcx,r13
    call EXTR_memset wrt ..plt
    .lbl180011fd9: mov eax,dword ptr [rsp + 0x88]
    sub esi,0x1
    sub r12,0x1
    add rbp,r14
    add r13,r14
    cmp esi,dword ptr [rsp + 0x20]
    jg .lbl180011f90
    .lbl180011ff3: mov r10d,dword ptr [rsp + 0x78]
    xor r14d,r14d
    .lbl180011ffb: mov ebp,dword ptr [rsp + 0x80]
    mov ecx,r10d
    mov r9,r14
    mov eax,ebp
    movsxd r11,r10d
    cdq
    sub eax,edx
    sar eax,1
    mov edi,eax
    mov eax,r10d
    cdq
    and edx,0x3
    add eax,edx
    sar eax,0x2
    movsxd rsi,eax
    sub ecx,esi
    test edi,edi
    movsxd r8,ecx
    jle .lbl180012099
    nop
    .lbl180012030: cmp rsi,r8
    mov rdx,rsi
    mov rcx,rsi
    jge .lbl18001206f
    mov rax,rbx
    imul rax,rcx
    add rax,r9
    add rax,r15
    .lbl180012048: cmp byte ptr [rax],0x0
    ja .lbl18001205e
    add rcx,0x1
    add edx,0x1
    add rax,rbx
    cmp rcx,r8
    jl .lbl180012048
    jmp .lbl18001206f
    .lbl18001205e: movsxd rax,edx
    imul rax,rbx
    add rax,r9
    cmp byte ptr [rax + r15*0x1],0x0
    jnz .lbl180012099
    .lbl18001206f: test r10d,r10d
    jle .lbl18001208c
    lea rax,[r9 + r15*0x1]
    mov rcx,r11
    nop
    nop
    .lbl180012080: mov byte ptr [rax],0x0
    add rax,rbx
    sub rcx,0x1
    jnz .lbl180012080
    .lbl18001208c: add r14d,0x1
    add r9,0x1
    cmp r14d,edi
    jl .lbl180012030
    .lbl180012099: lea r10d,[rbp + -0x1]
    cmp r10d,edi
    movsxd r9,r10d
    jle .lbl18001210c
    .lbl1800120a5: cmp rsi,r8
    mov rdx,rsi
    mov rcx,rsi
    jge .lbl1800120e7
    mov rax,rbx
    imul rax,rcx
    add rax,r9
    add rax,r15
    nop
    .lbl1800120c0: cmp byte ptr [rax],0x0
    ja .lbl1800120d6
    add rcx,0x1
    add edx,0x1
    add rax,rbx
    cmp rcx,r8
    jl .lbl1800120c0
    jmp .lbl1800120e7
    .lbl1800120d6: movsxd rax,edx
    imul rax,rbx
    add rax,r9
    cmp byte ptr [rax + r15*0x1],0x0
    jnz .lbl18001210c
    .lbl1800120e7: test r11,r11
    jle .lbl1800120ff
    lea rax,[r9 + r15*0x1]
    mov rcx,r11
    .lbl1800120f3: mov byte ptr [rax],0x0
    add rax,rbx
    sub rcx,0x1
    jnz .lbl1800120f3
    .lbl1800120ff: sub r10d,0x1
    sub r9,0x1
    cmp r10d,edi
    jg .lbl1800120a5
    .lbl18001210c: mov rbx,qword ptr [rsp + 0x70]
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
EXTR_check_cookie:
    ret

section .text
EXTR_FUN_180011d20:
    mov qword ptr [rsp + 0x10],rbx
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov r14,qword ptr [rsp + 0x90]
    movsxd rbx,r8d
    movsxd r15,edx
    mov rbp,rcx
    mov rcx,r14
    mov r8d,ebx
    mov edx,r15d
    mov esi,r9d
    call EXTR_FUN_180011310
    mov eax,r15d
    xor r8d,r8d
    cdq
    mov edi,r8d
    mov r11,r8
    sub eax,edx
    mov rdx,rbx
    sar eax,1
    imul eax,ebx
    cdqe
    lea r12,[rbp + rax*0x2]
    lea eax,[r15 + -0x1]
    mov qword ptr [rsp + 0x20],r12
    imul eax,ebx
    test ebx,ebx
    cdqe
    lea r13,[rbp + rax*0x2]
    jle .lbl180011ddb
    mov r10,r12
    mov r12,r13
    sub rbp,r13
    sub r12,r10
    .lbl180011da1: movsx r9d,word ptr [r10]
    lea rbx,[r12 + r10*0x1]
    movsx eax,word ptr [rbx + rbp*0x1]
    mov ecx,r9d
    sub ecx,eax
    cmp ecx,esi
    jl .lbl180011dd1
    movsx eax,word ptr [rbx]
    sub r9d,eax
    cmp r9d,esi
    jl .lbl180011dd1
    add r11,0x1
    add edi,0x1
    add r10,0x2
    cmp r11,rdx
    jl .lbl180011da1
    .lbl180011dd1: mov rbp,qword ptr [rsp + 0x70]
    mov r12,qword ptr [rsp + 0x20]
    .lbl180011ddb: test edi,edi
    mov rbx,r15
    movsxd r9,edi
    jle .lbl180011e06
    .lbl180011de5: test r15d,r15d
    jle .lbl180011dfd
    lea rax,[r8 + r14*0x1]
    mov rcx,rbx
    .lbl180011df1: mov byte ptr [rax],0x0
    add rax,rdx
    sub rcx,0x1
    jnz .lbl180011df1
    .lbl180011dfd: add r8,0x1
    cmp r8,r9
    jl .lbl180011de5
    .lbl180011e06: mov r11d,dword ptr [rsp + 0x80]
    add r11d,-0x1
    movsxd r10,r11d
    js .lbl180011e4f
    sub rbp,r13
    lea r9,[r12 + r10*0x2]
    sub r13,r12
    .lbl180011e21: movsx r8d,word ptr [r9]
    lea rdi,[r9 + r13*0x1]
    movsx eax,word ptr [rdi + rbp*0x1]
    mov ecx,r8d
    sub ecx,eax
    cmp ecx,esi
    jl .lbl180011e4f
    movsx eax,word ptr [rdi]
    sub r8d,eax
    cmp r8d,esi
    jl .lbl180011e4f
    sub r11d,0x1
    sub r9,0x2
    sub r10,0x1
    jns .lbl180011e21
    .lbl180011e4f: add r11d,0x1
    movsxd r8,r11d
    cmp r8,rdx
    jge .lbl180011e85
    nop
    nop
    .lbl180011e60: test r15d,r15d
    jle .lbl180011e7c
    lea rax,[r8 + r14*0x1]
    mov rcx,rbx
    nop
    .lbl180011e70: mov byte ptr [rax],0x0
    add rax,rdx
    sub rcx,0x1
    jnz .lbl180011e70
    .lbl180011e7c: add r8,0x1
    cmp r8,rdx
    jl .lbl180011e60
    .lbl180011e85: mov rbx,qword ptr [rsp + 0x78]
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
EXTR_FUN_180011310:
    imul edx,r8d
    mov r8d,edx
    mov dl,0x1
    jmp EXTR_memset wrt ..plt

section .data


section .text
extern EXTR_memset

section .text
EXTR_FUN_180011a60:
    mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x48
    mov rbp,rcx
    mov rcx,qword ptr [rsp + 0xb0]
    mov r14d,r9d
    mov r12d,r8d
    mov ebx,edx
    call EXTR_FUN_180011310
    mov eax,0x55555556
    lea r11d,[rbx + -0x1]
    xor r13d,r13d
    mov dword ptr [rsp + 0x20],r11d
    mov edi,r13d
    imul ebx
    mov r9d,edx
    mov eax,0x55555556
    shr r9d,0x1f
    add edx,r9d
    imul edx,r12d
    movsxd rcx,edx
    lea rsi,[rbp + rcx*0x2]
    lea ecx,[rbx + rbx*0x1]
    lea ebx,[r12 + -0x1]
    mov qword ptr [rsp + 0x30],rsi
    imul ecx
    mov eax,edx
    shr eax,0x1f
    add eax,edx
    imul eax,r12d
    cdqe
    lea r9,[rbp + rax*0x2]
    mov eax,r11d
    mov qword ptr [rsp + 0x28],r9
    imul eax,r12d
    test ebx,ebx
    cdqe
    lea r15,[rbp + rax*0x2]
    mov qword ptr [rsp + 0x38],r15
    jle .lbl180011b49
    mov r8,rsi
    mov r10,rbp
    mov rdx,r9
    sub r8,r9
    sub r10,r9
    nop
    nop
    .lbl180011b20: movzx eax,word ptr [r8 + rdx*0x1]
    movzx ecx,word ptr [rdx]
    cmp ax,cx
    cmovl cx,ax
    movsx eax,word ptr [r10 + rdx*0x1]
    movsx ecx,cx
    sub ecx,eax
    cmp ecx,r14d
    jl .lbl180011b49
    add edi,0x1
    add rdx,0x2
    cmp edi,ebx
    jl .lbl180011b20
    .lbl180011b49: test ebx,ebx
    mov r8d,r13d
    jle .lbl180011b90
    mov r10,rsi
    mov r11,r15
    mov rdx,r9
    sub r10,r9
    sub r11,r9
    nop
    .lbl180011b60: movzx eax,word ptr [rdx + r10*0x1]
    movzx ecx,word ptr [rdx]
    cmp ax,cx
    cmovl cx,ax
    movsx eax,word ptr [r11 + rdx*0x1]
    movsx ecx,cx
    sub ecx,eax
    cmp ecx,r14d
    jl .lbl180011b8b
    add r8d,0x1
    add rdx,0x2
    cmp r8d,ebx
    jl .lbl180011b60
    .lbl180011b8b: mov r11d,dword ptr [rsp + 0x20]
    .lbl180011b90: mov ecx,dword ptr [rsp + 0x98]
    mov r12,r13
    test ecx,ecx
    jle .lbl180011c1a
    movsxd r14,dword ptr [rsp + 0xa0]
    mov eax,r8d
    mov esi,r11d
    neg eax
    mov ebp,r13d
    mov r15,rcx
    mov r13d,eax
    imul esi,r8d
    nop
    nop
    .lbl180011bc0: lea eax,[rsi + rbp*0x1]
    cdq
    idiv r11d
    test eax,eax
    jle .lbl180011be9
    mov rcx,r14
    mov r8d,eax
    xor edx,edx
    imul rcx,r12
    add rcx,qword ptr [rsp + 0xb0]
    call EXTR_memset wrt ..plt
    mov r11d,dword ptr [rsp + 0x20]
    .lbl180011be9: add esi,r13d
    add ebp,edi
    add r12,0x1
    sub r15,0x1
    jnz .lbl180011bc0
    mov r14d,dword ptr [rsp + 0xa8]
    mov r9,qword ptr [rsp + 0x28]
    mov rsi,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x90]
    mov r15,qword ptr [rsp + 0x38]
    xor r13d,r13d
    .lbl180011c1a: test ebx,ebx
    movsxd r12,ebx
    mov r8,r12
    jle .lbl180011c5f
    mov r10,rsi
    lea rdx,[r9 + r12*0x2]
    sub r10,r9
    sub rbp,r9
    .lbl180011c31: movzx eax,word ptr [rdx + r10*0x1]
    movzx ecx,word ptr [rdx]
    cmp ax,cx
    cmovl cx,ax
    movsx eax,word ptr [rdx + rbp*0x1]
    movsx ecx,cx
    sub ecx,eax
    cmp ecx,r14d
    jl .lbl180011c5f
    sub r8,0x1
    sub r12d,0x1
    sub rdx,0x2
    test r8,r8
    jg .lbl180011c31
    .lbl180011c5f: test ebx,ebx
    movsxd r8,ebx
    jle .lbl180011c9d
    sub rsi,r9
    lea rdx,[r9 + r8*0x2]
    sub r15,r9
    .lbl180011c70: movzx eax,word ptr [rdx + rsi*0x1]
    movzx ecx,word ptr [rdx]
    cmp ax,cx
    cmovl cx,ax
    movsx eax,word ptr [rdx + r15*0x1]
    movsx ecx,cx
    sub ecx,eax
    cmp ecx,r14d
    jl .lbl180011c9d
    sub r8,0x1
    sub ebx,0x1
    sub rdx,0x2
    test r8,r8
    jg .lbl180011c70
    .lbl180011c9d: mov eax,dword ptr [rsp + 0x98]
    test eax,eax
    jle .lbl180011d02
    movsxd rbp,dword ptr [rsp + 0xa0]
    mov r14d,ebx
    mov edi,r11d
    neg r14d
    mov esi,r13d
    imul edi,ebx
    mov rbx,rax
    .lbl180011cc2: lea eax,[rsi + rdi*0x1]
    cdq
    idiv r11d
    add eax,0x1
    movsxd rdx,eax
    cmp rdx,rbp
    jge .lbl180011cf3
    lea rcx,[r13 + rdx*0x1]
    mov r8,rbp
    add rcx,qword ptr [rsp + 0xb0]
    sub r8,rdx
    xor edx,edx
    call EXTR_memset wrt ..plt
    mov r11d,dword ptr [rsp + 0x20]
    .lbl180011cf3: add edi,r14d
    add esi,r12d
    add r13,rbp
    sub rbx,0x1
    jnz .lbl180011cc2
    .lbl180011d02: add rsp,0x48
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
EXTR_FUN_180011580:
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x30
    mov edi,edx
    xor r10d,r10d
    mov rdx,rcx
    mov rcx,qword ptr [rsp + 0x70]
    mov eax,edi
    mov ebx,r8d
    imul eax,r8d
    test eax,eax
    movsxd r11,eax
    jle .lbl1800115de
    movzx r8d,word ptr [rsp + 0x60]
    nop
    .lbl1800115b0: movzx eax,word ptr [rdx + r10*0x2]
    cmp ax,r9w
    jge .lbl1800115cf
    cmp ax,r8w
    jle .lbl1800115cf
    cmp byte ptr [r10 + rcx*0x1],0x0
    jz .lbl1800115cf
    mov eax,0x1
    jmp .lbl1800115d1
    .lbl1800115cf: xor eax,eax
    .lbl1800115d1: mov byte ptr [r10 + rcx*0x1],al
    add r10,0x1
    cmp r10,r11
    jl .lbl1800115b0
    .lbl1800115de: mov eax,dword ptr [rsp + 0x68]
    mov rdx,qword ptr [rsp + 0x78]
    mov r9d,edi
    mov r8d,ebx
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_180004dc0
    mov rbx,qword ptr [rsp + 0x40]
    xor eax,eax
    add rsp,0x30
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180004dc0:
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
    call EXTR_FUN_1800047d0
    mov r9d,edi
    mov r8d,esi
    mov rdx,rbp
    mov rcx,rax
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180004aa0
    mov rbx,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    add rsp,0x30
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180011610:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    push rsi
    push r13
    push r14
    push r15
    sub rsp,0x48
    mov r14d,dword ptr [rsp + 0xa0]
    mov eax,edx
    movsxd r13,r8d
    cdq
    mov r15,rcx
    movsxd rsi,r14d
    idiv r14d
    mov r10d,eax
    mov eax,r14d
    imul eax,r14d
    mov dword ptr [rsp + 0xa0],eax
    mov eax,r13d
    cdq
    idiv r14d
    test r10d,r10d
    movsxd r8,eax
    mov qword ptr [rsp + 0x38],r8
    jle .lbl1800117ef
    mov qword ptr [r11 + 0x8],rbx
    mov qword ptr [r11 + 0x10],rbp
    movzx ebp,word ptr [rsp + 0x90]
    xor ecx,ecx
    mov rdx,r10
    mov qword ptr [r11 + 0x18],rdi
    mov qword ptr [r11 + -0x28],r12
    mov qword ptr [rsp + 0x20],rcx
    mov qword ptr [rsp + 0x30],rdx
    nop
    nop
    .lbl180011690: test r8,r8
    jle .lbl1800117c1
    xor r12d,r12d
    mov qword ptr [rsp + 0x28],r8
    .lbl1800116a1: xor r11d,r11d
    test r14d,r14d
    jle .lbl18001174a
    mov rbx,rcx
    mov rdi,r14
    nop
    nop
    nop
    nop
    .lbl1800116c0: xor r8d,r8d
    xor r9d,r9d
    xor r10d,r10d
    cmp rsi,0x2
    jl .lbl180011711
    mov rax,r13
    lea rdx,[rsi + -0x2]
    shr rdx,1
    imul rax,rbx
    add rax,r12
    add rdx,0x1
    lea r10,[rdx + rdx*0x1]
    lea rcx,[r15 + rax*0x2]
    nop
    .lbl1800116f0: xor eax,eax
    cmp word ptr [rcx],bp
    setg al
    add r8d,eax
    xor eax,eax
    cmp word ptr [rcx + 0x2],bp
    setg al
    add rcx,0x4
    add r9d,eax
    sub rdx,0x1
    jnz .lbl1800116f0
    .lbl180011711: cmp r10,rsi
    jge .lbl180011730
    xor ecx,ecx
    mov rax,r13
    imul rax,rbx
    add rax,r12
    add rax,r10
    cmp word ptr [r15 + rax*0x2],bp
    setg cl
    add r11d,ecx
    .lbl180011730: lea ecx,[r9 + r8*0x1]
    add rbx,0x1
    add r11d,ecx
    sub rdi,0x1
    jnz .lbl1800116c0
    mov rcx,qword ptr [rsp + 0x20]
    .lbl18001174a: imul r11d,r11d,0x64
    mov eax,r11d
    cdq
    idiv dword ptr [rsp + 0xa0]
    cmp eax,dword ptr [rsp + 0x98]
    jg .lbl1800117a3
    test r14d,r14d
    jle .lbl1800117a3
    mov rbp,qword ptr [rsp + 0x88]
    mov rbx,rcx
    mov rdi,r14
    .lbl180011775: test r14d,r14d
    jle .lbl180011791
    mov rcx,r13
    mov r8,rsi
    mov dl,0x1
    imul rcx,rbx
    add rcx,r12
    add rcx,rbp
    call EXTR_memset wrt ..plt
    .lbl180011791: add rbx,0x1
    sub rdi,0x1
    jnz .lbl180011775
    mov bp,word ptr [rsp + 0x90]
    .lbl1800117a3: mov rcx,qword ptr [rsp + 0x20]
    add r12,rsi
    sub qword ptr [rsp + 0x28],0x1
    jnz .lbl1800116a1
    mov rdx,qword ptr [rsp + 0x30]
    mov r8,qword ptr [rsp + 0x38]
    .lbl1800117c1: add rcx,rsi
    sub rdx,0x1
    mov qword ptr [rsp + 0x30],rdx
    mov qword ptr [rsp + 0x20],rcx
    jnz .lbl180011690
    mov r12,qword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x80]
    mov rbp,qword ptr [rsp + 0x78]
    mov rbx,qword ptr [rsp + 0x70]
    .lbl1800117ef: xor eax,eax
    add rsp,0x48
    pop r15
    pop r14
    pop r13
    pop rsi
    ret

section .data


section .text
EXTR_FUN_180004e50:
    test r8d,r8d
    movsxd r9,r8d
    jle .lbl180004e73
    mov r8,rcx
    sub rdx,rcx
    nop
    .lbl180004e60: movzx eax,byte ptr [rdx + r8*0x1]
    add r8,0x1
    and byte ptr [r8 + -0x1],al
    sub r9,0x1
    jnz .lbl180004e60
    .lbl180004e73: mov rax,rcx
    ret

section .data


section .text
EXTR_FUN_1800139f0:
    mov qword ptr [rsp + 0x20],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x70
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x60],rax
    mov rax,qword ptr [rsp + 0xe0]
    mov rbp,qword ptr [rsp + 0xe8]
    mov r12d,r8d
    mov qword ptr [rsp + 0x48],rax
    mov r13d,edx
    mov r15d,r9d
    sar r12d,1
    sar r13d,1
    mov r9d,0x1
    mov eax,r12d
    mov dword ptr [rsp + 0x44],r8d
    mov dword ptr [rsp + 0x40],edx
    mov word ptr [rsp + 0x50],0x1c47
    mov word ptr [rsp + 0x52],0x446e
    mov word ptr [rsp + 0x54],0x5d81
    imul eax,r13d
    movsxd rsi,eax
    mov word ptr [rsp + 0x56],0x446e
    mov word ptr [rsp + 0x58],0x1c47
    lea rdi,[rbp + rsi*0x4]
    mov qword ptr [rsp + 0x20],rbp
    lea r14,[rdi + rsi*0x2]
    lea rbx,[rdi + rsi*0x4]
    call EXTR_downscale
    xor ecx,ecx
    test rsi,rsi
    jle .lbl180013aa6
    .lbl180013a91: mov eax,dword ptr [rbp + rcx*0x4]
    add rcx,0x1
    sar eax,0x2
    cmp rcx,rsi
    mov word ptr [rdi + rcx*0x2 + -0x2],ax
    jl .lbl180013a91
    .lbl180013aa6: mov qword ptr [rsp + 0x30],r14
    lea r9,[rsp + 0x50]
    mov r8d,r12d
    mov edx,r13d
    mov rcx,rdi
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x5
    call EXTR_FUN_180002820
    mov qword ptr [rsp + 0x30],rdi
    lea r9,[rsp + 0x50]
    mov r8d,r12d
    mov edx,r13d
    mov rcx,r14
    mov dword ptr [rsp + 0x28],0x5
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    mov r9d,r15d
    mov r8d,r12d
    mov edx,r13d
    mov rcx,rdi
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_180011320
    xor r11d,r11d
    test rsi,rsi
    jle .lbl180013b3b
    mov ecx,dword ptr [rsp + 0xd0]
    nop
    nop
    .lbl180013b20: mov eax,dword ptr [rbp + r11*0x4]
    sar eax,0x2
    cmp eax,ecx
    setl al
    add r11,0x1
    and byte ptr [r11 + rbx*0x1 + -0x1],al
    cmp r11,rsi
    jl .lbl180013b20
    .lbl180013b3b: mov r9d,r13d
    mov r8d,r12d
    mov rdx,r14
    mov rcx,rbx
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_1800047d0
    mov r9,rdi
    mov r8d,r12d
    mov edx,r13d
    mov rcx,rbp
    call EXTR_average_img
    xor r11d,r11d
    test rsi,rsi
    jle .lbl180013b9a
    mov edx,dword ptr [rsp + 0xd8]
    .lbl180013b74: mov ecx,dword ptr [rdi + r11*0x4]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    mov eax,ecx
    jg .lbl180013b84
    neg eax
    .lbl180013b84: sar eax,0x2
    cmp eax,edx
    setg al
    add r11,0x1
    and byte ptr [r11 + rbx*0x1 + -0x1],al
    cmp r11,rsi
    jl .lbl180013b74
    .lbl180013b9a: mov r9,rdi
    mov r8d,r13d
    mov edx,r12d
    mov rcx,rbx
    call EXTR_FUN_180015da0
    mov r9d,0x4
    mov r8d,r12d
    mov edx,r13d
    mov rcx,rbx
    call EXTR_FUN_180010550
    movsxd r13,dword ptr [rsp + 0x40]
    movsxd r14,dword ptr [rsp + 0x44]
    xor r9d,r9d
    test r13d,r13d
    movsxd rbp,r12d
    mov r12,qword ptr [rsp + 0x48]
    mov rsi,r13
    jle .lbl180013c2c
    mov rdx,r12
    nop
    .lbl180013be0: xor r11d,r11d
    xor r10d,r10d
    test r14d,r14d
    jle .lbl180013c1f
    movsxd rcx,r9d
    sar rcx,1
    imul rcx,rbp
    nop
    nop
    nop
    .lbl180013c00: movsxd rax,r11d
    add r10,0x1
    add r11d,0x1
    sar rax,1
    add rax,rcx
    cmp r10,r14
    movzx eax,byte ptr [rax + rbx*0x1]
    mov byte ptr [rdx + r10*0x1 + -0x1],al
    jl .lbl180013c00
    .lbl180013c1f: add r9d,0x1
    add rdx,r14
    sub rsi,0x1
    jnz .lbl180013be0
    .lbl180013c2c: mov r9,rdi
    mov r8d,r13d
    mov edx,r14d
    mov rcx,r12
    call EXTR_FUN_180015da0
    xor eax,eax
    mov rcx,qword ptr [rsp + 0x60]
    xor rcx,rsp
    call EXTR_check_cookie
    mov rbx,qword ptr [rsp + 0xc8]
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
EXTR_FUN_180011320:
    mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x38
    mov r12,rcx
    mov rcx,qword ptr [rsp + 0xa0]
    mov esi,r9d
    mov ebx,r8d
    mov r15d,edx
    call EXTR_FUN_180011310
    lea r9d,[r15 + -0x1]
    xor r13d,r13d
    mov eax,r9d
    mov dword ptr [rsp + 0x20],r9d
    mov edi,r13d
    imul eax,ebx
    cdqe
    add ebx,-0x1
    test ebx,ebx
    lea r8,[r12 + rax*0x2]
    mov qword ptr [rsp + 0x28],r8
    jle .lbl180011398
    mov rcx,r12
    .lbl180011386: movsx eax,word ptr [rcx]
    cmp eax,esi
    jg .lbl180011398
    add edi,0x1
    add rcx,0x2
    cmp edi,ebx
    jl .lbl180011386
    .lbl180011398: test ebx,ebx
    mov ecx,r13d
    jle .lbl1800113b4
    mov rdx,r8
    .lbl1800113a2: movsx eax,word ptr [rdx]
    cmp eax,esi
    jg .lbl1800113b4
    add ecx,0x1
    add rdx,0x2
    cmp ecx,ebx
    jl .lbl1800113a2
    .lbl1800113b4: test r15d,r15d
    mov r14,r13
    jle .lbl18001143a
    movsxd rsi,dword ptr [rsp + 0x90]
    mov eax,ecx
    mov ebp,r9d
    neg eax
    mov r12d,r13d
    mov r13d,eax
    imul ebp,ecx
    nop
    nop
    nop
    .lbl1800113e0: lea eax,[rbp + r12*0x1]
    cdq
    idiv r9d
    test eax,eax
    jle .lbl18001140b
    mov rcx,rsi
    mov r8d,eax
    xor edx,edx
    imul rcx,r14
    add rcx,qword ptr [rsp + 0xa0]
    call EXTR_memset wrt ..plt
    mov r9d,dword ptr [rsp + 0x20]
    .lbl18001140b: add ebp,r13d
    add r12d,edi
    add r14,0x1
    sub r15,0x1
    jnz .lbl1800113e0
    mov esi,dword ptr [rsp + 0x98]
    mov r12,qword ptr [rsp + 0x80]
    mov r8,qword ptr [rsp + 0x28]
    mov r15d,dword ptr [rsp + 0x88]
    xor r13d,r13d
    .lbl18001143a: test ebx,ebx
    movsxd rbp,ebx
    mov rcx,rbp
    jle .lbl180011459
    .lbl180011444: movsx eax,word ptr [r12 + rcx*0x2]
    cmp eax,esi
    jg .lbl180011459
    sub rcx,0x1
    sub ebp,0x1
    test rcx,rcx
    jg .lbl180011444
    .lbl180011459: test ebx,ebx
    movsxd rcx,ebx
    jle .lbl180011475
    .lbl180011460: movsx eax,word ptr [r8 + rcx*0x2]
    cmp eax,esi
    jg .lbl180011475
    sub rcx,0x1
    sub ebx,0x1
    test rcx,rcx
    jg .lbl180011460
    .lbl180011475: test r15d,r15d
    jle .lbl1800114df
    movsxd r12,dword ptr [rsp + 0x90]
    mov r14d,ebx
    mov edi,r9d
    neg r14d
    mov esi,r13d
    imul edi,ebx
    mov ebx,r15d
    nop
    nop
    nop
    .lbl1800114a0: lea eax,[rdi + rsi*0x1]
    cdq
    idiv r9d
    add eax,0x1
    movsxd rdx,eax
    cmp rdx,r12
    jge .lbl1800114d1
    lea rcx,[r13 + rdx*0x1]
    mov r8,r12
    add rcx,qword ptr [rsp + 0xa0]
    sub r8,rdx
    xor edx,edx
    call EXTR_memset wrt ..plt
    mov r9d,dword ptr [rsp + 0x20]
    .lbl1800114d1: add edi,r14d
    add esi,ebp
    add r13,r12
    sub rbx,0x1
    jnz .lbl1800114a0
    .lbl1800114df: add rsp,0x38
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
EXTR_FUN_180011540:
    sub rsp,0x28
    imul ecx,edx
    mov rax,r8
    test ecx,ecx
    movsxd r8,ecx
    jle .lbl18001155d
    movzx edx,r9b
    mov rcx,rax
    call EXTR_memset wrt ..plt
    .lbl18001155d: xor eax,eax
    add rsp,0x28
    ret

section .data


section .text
EXTR_FUN_1800110c0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x40
    mov rbx,qword ptr [rsp + 0x70]
    mov rbp,r9
    mov esi,r8d
    mov qword ptr [rsp + 0x30],rbx
    mov qword ptr [rsp + 0x28],r9
    mov r9d,0xf5
    mov edi,edx
    mov dword ptr [rsp + 0x20],0xffff8000
    call EXTR_FUN_180010f20
    mov r9,rbx
    mov r8d,edi
    mov edx,esi
    mov rcx,rbp
    call EXTR_FUN_180015da0
    mov rbx,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    xor eax,eax
    add rsp,0x40
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180010f20:
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x60
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x50],rax
    mov r13,qword ptr [rsp + 0xd0]
    mov rbp,qword ptr [rsp + 0xc8]
    mov eax,r8d
    imul eax,edx
    test rcx,rcx
    movsxd r12,eax
    mov dword ptr [rsp + 0x40],r9d
    mov r15d,r8d
    mov r14d,edx
    mov word ptr [rsp + 0x44],0x2710
    lea rsi,[r13 + r12*0x4]
    mov word ptr [rsp + 0x46],0x4e20
    mov word ptr [rsp + 0x48],0x2710
    jnz .lbl180010f8f
    lea rax,[r12*0x8]
    jmp .lbl180011088
    .lbl180010f8f: mov qword ptr [rsp + 0x30],r13
    lea r9,[rsp + 0x44]
    mov dword ptr [rsp + 0x28],0x3
    mov qword ptr [rsp + 0xb8],rbx
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    lea rbx,[r13 + r12*0x2]
    lea r9,[rsp + 0x44]
    mov r8d,r15d
    mov qword ptr [rsp + 0x30],rbx
    mov edx,r14d
    mov rcx,r13
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x3
    call EXTR_FUN_180002820
    mov r9,rsi
    mov r8d,r15d
    mov edx,r14d
    mov rcx,rbx
    call EXTR_FUN_180010c70
    lea r8d,[r12*0x4]
    mov rdx,rsi
    mov rcx,r13
    call EXTR_memcpy wrt ..plt
    mov r11d,r12d
    mov eax,0x51eb851f
    lea r8d,[r12 + -0x1]
    mov rcx,r13
    imul r11d,r11d,0x4b
    imul r11d
    sar edx,0x5
    mov r9d,edx
    shr r9d,0x1f
    add r9d,edx
    xor edx,edx
    call EXTR_FUN_180002690
    mov ecx,dword ptr [rsp + 0xc0]
    mov rbx,qword ptr [rsp + 0xb8]
    mov r11d,eax
    mov eax,dword ptr [rsp + 0x40]
    cmp r11d,eax
    cmovg r11d,eax
    cmp r11d,ecx
    cmovl r11d,ecx
    xor ecx,ecx
    test r12d,r12d
    jle .lbl180011086
    imul r11d,r11d,0x46
    mov eax,0x51eb851f
    imul r11d
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    .lbl180011073: cmp dword ptr [rsi + rcx*0x4],edx
    setg al
    add rcx,0x1
    cmp rcx,r12
    mov byte ptr [rcx + rbp*0x1 + -0x1],al
    jl .lbl180011073
    .lbl180011086: xor eax,eax
    .lbl180011088: mov rcx,qword ptr [rsp + 0x50]
    xor rcx,rsp
    call EXTR_check_cookie
    add rsp,0x60
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
EXTR_FUN_180010c70:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push r14
    sub rsp,0x70
    lea eax,[rdx + -0x1]
    mov qword ptr [rsp + 0x38],r15
    mov r15d,0x1
    cmp eax,r15d
    mov r14,r9
    mov r11d,edx
    mov r9,rcx
    mov qword ptr [rsp + 0x20],r15
    jle .lbl180010e76
    mov qword ptr [rsp + 0x68],rbx
    mov qword ptr [rsp + 0x60],rbp
    mov qword ptr [rsp + 0x58],rsi
    mov qword ptr [rsp + 0x50],rdi
    lea r10d,[r8 + -0x1]
    add eax,-0x1
    mov qword ptr [rsp + 0x48],r12
    mov qword ptr [rsp + 0x40],r13
    mov dword ptr [rsp],r10d
    mov qword ptr [rsp + 0x28],rax
    nop
    .lbl180010ce0: cmp r10d,0x1
    jle .lbl180010e38
    movsxd rdx,r8d
    lea rcx,[r15 + -0x1]
    mov r8,rdx
    imul r8,r15
    lea rax,[r14 + r8*0x4 + 0x4]
    mov qword ptr [rsp + 0x10],rax
    lea rax,[rcx + 0x2]
    imul rax,rdx
    imul rcx,rdx
    lea edx,[r10 + -0x1]
    lea r15,[r9 + rcx*0x2 + 0x4]
    lea rcx,[r9 + r8*0x2 + 0x4]
    lea r14,[r9 + rax*0x2 + 0x4]
    mov qword ptr [rsp + 0x8],rcx
    mov qword ptr [rsp + 0x18],rdx
    nop
    nop
    .lbl180010d30: movsx r8d,word ptr [rcx + -0x2]
    movsx r10d,word ptr [rcx + -0x4]
    movsx r9d,word ptr [rcx]
    movsx ebx,word ptr [r14 + -0x2]
    movsx r11d,word ptr [r15 + -0x2]
    movsx r13d,word ptr [r14]
    movsx esi,word ptr [r15]
    movsx ebp,word ptr [r14 + -0x4]
    movsx r12d,word ptr [r15 + -0x4]
    lea eax,[r8 + r8*0x1]
    mov ecx,r10d
    sub ecx,eax
    sub ecx,ebx
    sub ecx,r11d
    add ecx,r9d
    lea eax,[r13 + rcx*0x2]
    sub r13d,esi
    lea ecx,[r8 + r8*0x1]
    add eax,esi
    sub ebx,ecx
    movsx ecx,word ptr [r14 + -0x4]
    add eax,ebp
    sub r13d,ebp
    sub ebx,r10d
    add eax,r12d
    add r13d,r12d
    sub ebx,r9d
    cdq
    sar r13d,0x4
    add ebx,r11d
    and edx,0x3
    imul r13d,r13d
    lea edi,[rdx + rax*0x1]
    lea eax,[rcx + rbx*0x2]
    movsx ecx,word ptr [r14]
    add eax,ecx
    movsx ecx,word ptr [r15]
    sar edi,0x2
    add eax,ecx
    movsx ecx,word ptr [r15 + -0x4]
    sar edi,0x4
    add eax,ecx
    cdq
    and edx,0x3
    lea ecx,[rdx + rax*0x1]
    sar ecx,0x2
    sar ecx,0x4
    imul ecx,edi
    mov eax,ecx
    sub eax,r13d
    sub r13d,ecx
    mov rcx,qword ptr [rsp + 0x8]
    cmp eax,r13d
    cmovg r13d,eax
    mov rax,qword ptr [rsp + 0x10]
    add rcx,0x2
    add rax,0x4
    mov dword ptr [rax + -0x4],r13d
    add r15,0x2
    add r14,0x2
    sub qword ptr [rsp + 0x18],0x1
    mov qword ptr [rsp + 0x8],rcx
    mov qword ptr [rsp + 0x10],rax
    jnz .lbl180010d30
    mov r14,qword ptr [rsp + 0x98]
    mov r15,qword ptr [rsp + 0x20]
    mov r9,qword ptr [rsp + 0x80]
    mov rax,qword ptr [rsp + 0x28]
    mov r10d,dword ptr [rsp]
    mov r8d,dword ptr [rsp + 0x90]
    .lbl180010e38: add r15,0x1
    sub rax,0x1
    mov qword ptr [rsp + 0x28],rax
    mov qword ptr [rsp + 0x20],r15
    jnz .lbl180010ce0
    mov r11d,dword ptr [rsp + 0x88]
    mov r13,qword ptr [rsp + 0x40]
    mov r12,qword ptr [rsp + 0x48]
    mov rdi,qword ptr [rsp + 0x50]
    mov rsi,qword ptr [rsp + 0x58]
    mov rbp,qword ptr [rsp + 0x60]
    mov rbx,qword ptr [rsp + 0x68]
    .lbl180010e76: mov r15,qword ptr [rsp + 0x38]
    xor edx,edx
    test r8d,r8d
    movsxd r10,r8d
    jle .lbl180010ed3
    movsxd rcx,r11d
    lea r9,[r14 + r10*0x4]
    lea rax,[rcx + -0x1]
    imul rax,r10
    lea r8,[r14 + rax*0x4]
    lea rax,[rcx + -0x2]
    imul rax,r10
    lea rcx,[r14 + rax*0x4]
    nop
    nop
    nop
    .lbl180010eb0: mov eax,dword ptr [r9]
    add rdx,0x1
    add r8,0x4
    mov dword ptr [r14 + rdx*0x4 + -0x4],eax
    mov eax,dword ptr [rcx]
    add r9,0x4
    mov dword ptr [r8 + -0x4],eax
    add rcx,0x4
    cmp rdx,r10
    jl .lbl180010eb0
    .lbl180010ed3: test r11d,r11d
    movsxd rdx,r11d
    jle .lbl180010f08
    lea r8,[r10*0x4]
    lea rcx,[r8 + r14*0x1 + -0x4]
    nop
    nop
    .lbl180010ef0: mov eax,dword ptr [r14 + 0x4]
    mov dword ptr [r14],eax
    mov eax,dword ptr [rcx + -0x4]
    add r14,r8
    mov dword ptr [rcx],eax
    add rcx,r8
    sub rdx,0x1
    jnz .lbl180010ef0
    .lbl180010f08: xor eax,eax
    add rsp,0x70
    pop r14
    ret

section .data


section .text
EXTR_FUN_180002690:
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
    jz .lbl1800026d8
    cmp edx,r8d
    .lbl1800026b7: jge .lbl1800026fa
    mov r8d,edi
    mov edx,ebx
    mov rcx,rbp
    call EXTR_FUN_180002580
    cmp esi,eax
    jz .lbl1800026f4
    jge .lbl1800026d1
    lea edi,[rax + -0x1]
    jmp .lbl1800026d4
    .lbl1800026d1: lea ebx,[rax + 0x1]
    .lbl1800026d4: cmp ebx,edi
    jnz .lbl1800026b7
    .lbl1800026d8: movsxd rax,ebx
    mov eax,dword ptr [rbp + rax*0x4]
    .lbl1800026df: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rdi
    ret
    .lbl1800026f4: mov eax,dword ptr [rbp + rsi*0x4]
    jmp .lbl1800026df
    .lbl1800026fa: mov eax,0xff676981
    jmp .lbl1800026df

section .data


section .text
EXTR_FUN_180002580:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rdi
    movsxd r9,edx
    lea r11d,[r8 + 0x1]
    mov r10,rcx
    mov ebx,dword ptr [rcx + r9*0x4]
    lea rdi,[rcx + r9*0x4]
    movsxd rdx,r11d
    movsxd r8,r8d
    .lbl1800025a2: add r9,0x1
    cmp dword ptr [r10 + r9*0x4],ebx
    jg .lbl1800025b1
    cmp r9,r8
    jle .lbl1800025a2
    .lbl1800025b1: sub rdx,0x1
    sub r11d,0x1
    cmp dword ptr [r10 + rdx*0x4],ebx
    jg .lbl1800025b1
    cmp r9,rdx
    jge .lbl1800025d6
    mov eax,dword ptr [r10 + rdx*0x4]
    mov ecx,dword ptr [r10 + r9*0x4]
    mov dword ptr [r10 + r9*0x4],eax
    mov dword ptr [r10 + rdx*0x4],ecx
    jmp .lbl1800025a2
    .lbl1800025d6: mov r8d,dword ptr [rdi]
    mov rbx,qword ptr [rsp + 0x8]
    movsxd rdx,r11d
    mov ecx,dword ptr [r10 + rdx*0x4]
    mov eax,r11d
    mov dword ptr [rdi],ecx
    mov rdi,qword ptr [rsp + 0x10]
    mov dword ptr [r10 + rdx*0x4],r8d
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
EXTR_FUN_1800136d0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x20],r9
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x40
    mov r13,qword ptr [rsp + 0xa8]
    mov eax,edx
    mov r15d,edx
    sar eax,1
    mov edx,r8d
    mov esi,0x1
    sar edx,1
    mov dword ptr [rsp + 0x90],eax
    mov r9d,esi
    mov r11d,edx
    mov dword ptr [rsp + 0x88],edx
    mov edi,r8d
    mov qword ptr [rsp + 0x20],r13
    imul r11d,eax
    movsxd r12,r11d
    lea r10d,[r11 + r11*0x4]
    mov eax,0x51eb851f
    lea rbx,[r13 + r12*0x4]
    imul r10d
    sar edx,0x5
    mov eax,edx
    lea r14,[rbx + r12*0x2]
    shr eax,0x1f
    lea rbp,[r14 + r12*0x2]
    add edx,eax
    mov dword ptr [rsp + 0x30],edx
    mov edx,r15d
    call EXTR_downscale
    mov r8d,dword ptr [rsp + 0x88]
    mov edx,dword ptr [rsp + 0x90]
    mov r9,rbx
    mov rcx,r13
    call EXTR_FUN_180011130
    xor edx,edx
    test r12,r12
    jle .lbl180013792
    mov r8d,dword ptr [rsp + 0xa0]
    nop
    .lbl180013780: cmp dword ptr [rbx + rdx*0x4],r8d
    setl al
    mov byte ptr [rdx + rbp*0x1],al
    add rdx,rsi
    cmp rdx,r12
    jl .lbl180013780
    .lbl180013792: mov r8d,dword ptr [rsp + 0x88]
    mov edx,dword ptr [rsp + 0x90]
    mov r9,rbx
    mov rcx,rbp
    mov byte ptr [rsp + 0x28],0x31
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_180012d30
    cmp eax,0x2
    mov dword ptr [rsp + 0xa8],eax
    jl .lbl1800139d2
    mov r8d,dword ptr [rsp + 0x88]
    mov edx,dword ptr [rsp + 0x90]
    mov r9d,eax
    mov rcx,rbx
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_1800103d0
    movsxd rdx,dword ptr [rsp + 0xa8]
    xor ebp,ebp
    xor r9d,r9d
    xor r10d,r10d
    xor r8d,r8d
    cmp rsi,rdx
    mov rcx,rsi
    jg .lbl18001382d
    .lbl180013800: movsx eax,word ptr [r14 + rcx*0x2]
    cmp ebp,eax
    jg .lbl180013816
    mov r9d,ebp
    mov r10d,r8d
    mov ebp,eax
    mov r8d,esi
    jmp .lbl180013821
    .lbl180013816: cmp r9d,eax
    jg .lbl180013821
    mov r9d,eax
    mov r10d,esi
    .lbl180013821: add rcx,0x1
    add esi,0x1
    cmp rcx,rdx
    jle .lbl180013800
    .lbl18001382d: cmp r9d,dword ptr [rsp + 0x30]
    movsxd r14,dword ptr [rsp + 0x88]
    jge .lbl1800138b7
    xor r11d,r11d
    test r15d,r15d
    jle .lbl1800139d2
    mov r12,qword ptr [rsp + 0x98]
    xor esi,esi
    mov rbp,r15
    .lbl180013855: test edi,edi
    jle .lbl1800138a4
    movsxd rcx,edi
    mov r9,rsi
    mov r10,rdi
    sar r9,1
    imul rcx,r11
    add rcx,r12
    imul r9,r14
    xor edx,edx
    nop
    nop
    nop
    nop
    .lbl180013880: mov rax,rdx
    sar rax,1
    add rax,r9
    movsx eax,word ptr [rbx + rax*0x2]
    cmp eax,r8d
    setz al
    add rcx,0x1
    add rdx,0x1
    and byte ptr [rcx + -0x1],al
    sub r10,0x1
    jnz .lbl180013880
    .lbl1800138a4: add rsi,0x1
    add r11,0x1
    sub rbp,0x1
    jnz .lbl180013855
    jmp .lbl1800139d2
    .lbl1800138b7: xor r11d,r11d
    xor edx,edx
    xor eax,eax
    test r12,r12
    jle .lbl1800138e6
    .lbl1800138c3: movsx ecx,word ptr [rbx + rax*0x2]
    cmp ecx,r8d
    jnz .lbl1800138d3
    add r11d,dword ptr [r13 + rax*0x4]
    jmp .lbl1800138dd
    .lbl1800138d3: cmp ecx,r10d
    jnz .lbl1800138dd
    add edx,dword ptr [r13 + rax*0x4]
    .lbl1800138dd: add rax,0x1
    cmp rax,r12
    jl .lbl1800138c3
    .lbl1800138e6: mov eax,edx
    cdq
    idiv r9d
    mov ecx,eax
    mov eax,r11d
    cdq
    idiv ebp
    cmp eax,ecx
    jle .lbl180013964
    xor r11d,r11d
    test r15d,r15d
    jle .lbl1800139d2
    mov r12,qword ptr [rsp + 0x98]
    xor esi,esi
    mov rbp,r15
    .lbl180013911: test edi,edi
    jle .lbl180013954
    movsxd rcx,edi
    mov r8,rsi
    mov r9,rdi
    sar r8,1
    imul rcx,r11
    add rcx,r12
    imul r8,r14
    xor edx,edx
    nop
    .lbl180013930: mov rax,rdx
    sar rax,1
    add rax,r8
    movsx eax,word ptr [rbx + rax*0x2]
    cmp eax,r10d
    setz al
    add rcx,0x1
    add rdx,0x1
    and byte ptr [rcx + -0x1],al
    sub r9,0x1
    jnz .lbl180013930
    .lbl180013954: add rsi,0x1
    add r11,0x1
    sub rbp,0x1
    jnz .lbl180013911
    jmp .lbl1800139d2
    .lbl180013964: jge .lbl1800139d2
    xor r11d,r11d
    test r15d,r15d
    jle .lbl1800139d2
    mov r12,qword ptr [rsp + 0x98]
    xor esi,esi
    mov rbp,r15
    nop
    nop
    .lbl180013980: test edi,edi
    jle .lbl1800139c4
    movsxd rcx,edi
    mov r9,rsi
    mov r10,rdi
    sar r9,1
    imul rcx,r11
    add rcx,r12
    imul r9,r14
    xor edx,edx
    nop
    .lbl1800139a0: mov rax,rdx
    sar rax,1
    add rax,r9
    movsx eax,word ptr [rbx + rax*0x2]
    cmp eax,r8d
    setz al
    add rcx,0x1
    add rdx,0x1
    and byte ptr [rcx + -0x1],al
    sub r10,0x1
    jnz .lbl1800139a0
    .lbl1800139c4: add rsi,0x1
    add r11,0x1
    sub rbp,0x1
    jnz .lbl180013980
    .lbl1800139d2: xor eax,eax
    mov rbx,qword ptr [rsp + 0x80]
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
EXTR_FUN_180011130:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    sub rsp,0x80
    lea r10d,[rdx + -0x1]
    mov rbx,r9
    movsxd r11,r8d
    cmp r10d,0x1
    mov eax,edx
    mov r9,rcx
    jle .lbl18001128d
    mov qword ptr [rsp + 0x78],rbp
    mov qword ptr [rsp + 0x70],rsi
    mov qword ptr [rsp + 0x68],rdi
    lea eax,[r11 + 0x1]
    mov qword ptr [rsp + 0x60],r12
    mov qword ptr [rsp + 0x58],r13
    movsxd r13,eax
    mov qword ptr [rsp + 0x50],r14
    lea r8d,[r11 + -0x1]
    lea eax,[r10 + -0x1]
    lea rdx,[r11*0x4]
    mov rcx,r11
    mov qword ptr [rsp + 0x48],r15
    mov dword ptr [rsp + 0x20],r8d
    lea r14,[rbx + r13*0x4]
    mov qword ptr [rsp + 0x28],rax
    mov qword ptr [rsp + 0x30],rcx
    mov qword ptr [rsp + 0x38],rdx
    lea r15,[r9 + r13*0x4 + 0x4]
    nop
    .lbl1800111c0: cmp r8d,0x1
    jle .lbl18001123b
    lea rax,[rcx + r13*0x1]
    mov rbx,r14
    mov rdi,r15
    lea rsi,[r9 + rax*0x4]
    mov rax,r13
    lea r12d,[r8 + -0x1]
    sub rax,rcx
    lea rbp,[r9 + rax*0x4]
    .lbl1800111e2: mov eax,dword ptr [rsi]
    mov ecx,dword ptr [rdi]
    sub eax,dword ptr [rbp]
    sub ecx,dword ptr [rdi + -0x8]
    sar eax,1
    sar ecx,1
    imul ecx,ecx
    imul eax,eax
    add ecx,eax
    jns .lbl180011202
    mov dword ptr [rbx],0x10000
    jmp .lbl180011209
    .lbl180011202: call EXTR_FUN_180002420
    mov dword ptr [rbx],eax
    .lbl180011209: add rbp,0x4
    add rsi,0x4
    add rdi,0x4
    add rbx,0x4
    sub r12,0x1
    jnz .lbl1800111e2
    mov rcx,qword ptr [rsp + 0x30]
    mov rdx,qword ptr [rsp + 0x38]
    mov rax,qword ptr [rsp + 0x28]
    mov r8d,dword ptr [rsp + 0x20]
    mov r9,qword ptr [rsp + 0x90]
    .lbl18001123b: add r13,rcx
    add r14,rdx
    add r15,rdx
    sub rax,0x1
    mov qword ptr [rsp + 0x28],rax
    jnz .lbl1800111c0
    mov r11d,dword ptr [rsp + 0xa0]
    mov eax,dword ptr [rsp + 0x98]
    mov rbx,qword ptr [rsp + 0xa8]
    mov r15,qword ptr [rsp + 0x48]
    mov r14,qword ptr [rsp + 0x50]
    mov r13,qword ptr [rsp + 0x58]
    mov r12,qword ptr [rsp + 0x60]
    mov rdi,qword ptr [rsp + 0x68]
    mov rsi,qword ptr [rsp + 0x70]
    mov rbp,qword ptr [rsp + 0x78]
    .lbl18001128d: mov r9d,0x1
    mov r8d,r11d
    mov edx,eax
    mov rcx,rbx
    call EXTR_FUN_1800109f0
    xor eax,eax
    add rsp,0x80
    pop rbx
    ret

section .data


section .text
EXTR_FUN_1800109f0:
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
    js .lbl180010b50
    mov r13d,r15d
    mov r12d,r9d
    lea rbp,[rbx*0x4]
    sub r13d,edi
    sub r12d,edi
    .lbl180010a60: cmp edi,r13d
    jge .lbl180010ad1
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
    lea r9,[rsi + rcx*0x4]
    add rax,r14
    lea r8,[rsi + rax*0x4]
    mov rax,r15
    imul rax,rbx
    add rax,r14
    lea rcx,[rsi + rax*0x4]
    lea rax,[r15 + 0x1]
    imul rax,r14
    sub r10d,edi
    lea rdx,[rsi + rax*0x4]
    nop
    .lbl180010ab0: mov eax,dword ptr [rdx]
    add r8,0x4
    add rdx,0x4
    mov dword ptr [rcx],eax
    mov eax,dword ptr [r9]
    add rcx,0x4
    mov dword ptr [r8 + -0x4],eax
    add r9,0x4
    sub r10,0x1
    jnz .lbl180010ab0
    .lbl180010ad1: cmp edi,r12d
    jge .lbl180010b3d
    lea rcx,[r14*0x4]
    mov rax,r15
    lea r9,[r15*0x4]
    lea rdx,[rcx + 0x4]
    lea r10,[r9 + 0x4]
    imul rax,r14
    add rax,rbx
    imul rdx,r15
    imul r10,r14
    add r10,rsi
    lea r11,[rsi + rax*0x4]
    mov rax,rdx
    sub rdx,rcx
    lea rcx,[rdx + rsi*0x1 + -0x4]
    sub rax,rbp
    mov edx,r12d
    sub edx,edi
    lea r8,[rax + rsi*0x1 + -0x4]
    nop
    .lbl180010b20: mov eax,dword ptr [r10]
    add r10,r9
    mov dword ptr [r11],eax
    mov eax,dword ptr [rcx]
    add r11,r9
    mov dword ptr [r8],eax
    add r8,r9
    add rcx,r9
    sub rdx,0x1
    jnz .lbl180010b20
    .lbl180010b3d: mov r9d,dword ptr [rsp + 0x58]
    sub rbp,0x4
    sub rbx,0x1
    jns .lbl180010a60
    .lbl180010b50: test edi,edi
    jle .lbl180010c3e
    mov r8,r15
    lea rax,[r15 + 0x1]
    movsxd rdx,r9d
    mov rcx,rdx
    lea r11,[-0x4 + rdx*0x4]
    lea rbx,[rsi + r8*0x4 + -0x4]
    imul rax,r14
    lea rbp,[rsi + rax*0x4]
    sub rcx,r14
    lea rax,[r14 + 0x1]
    imul r11,r8
    imul rax,r15
    sub rax,r14
    lea r12,[rsi + rax*0x4 + -0x4]
    lea rax,[rcx + -0x1]
    imul rcx,r15
    sub rcx,r14
    imul rax,r15
    add rax,r14
    lea r14,[rsi + rcx*0x4 + -0x4]
    lea r13,[rsi + rax*0x4]
    lea rax,[r15*0x4]
    neg r15
    mov qword ptr [rsp + 0x10],rax
    mov rax,rdx
    shl r15,0x2
    imul rax,r8
    add r11,rsi
    lea rdi,[rsi + rax*0x4 + -0x4]
    mov rax,qword ptr [rsp + 0x8]
    mov qword ptr [rsp],rax
    nop
    nop
    .lbl180010be0: mov rcx,r11
    mov rdx,rsi
    mov r8,rdi
    mov r9,rbx
    mov r10,rax
    nop
    .lbl180010bf0: mov eax,dword ptr [rbp]
    sub r8,0x4
    add rdx,0x4
    mov dword ptr [rdx + -0x4],eax
    mov eax,dword ptr [r12]
    add rcx,0x4
    mov dword ptr [r9],eax
    mov eax,dword ptr [r13]
    sub r9,0x4
    sub r10,0x1
    mov dword ptr [rcx + -0x4],eax
    mov eax,dword ptr [r14]
    mov dword ptr [r8 + 0x4],eax
    jnz .lbl180010bf0
    mov rax,qword ptr [rsp + 0x10]
    add r11,r15
    add rdi,r15
    add rsi,rax
    add rbx,rax
    sub qword ptr [rsp],0x1
    mov rax,qword ptr [rsp + 0x8]
    jnz .lbl180010be0
    .lbl180010c3e: mov rbx,qword ptr [rsp + 0x50]
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
EXTR_FUN_180011800:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    push rsi
    push r13
    push r14
    push r15
    sub rsp,0x48
    mov r14d,dword ptr [rsp + 0xa0]
    mov eax,edx
    movsxd r13,r8d
    cdq
    mov r15,rcx
    movsxd rsi,r14d
    idiv r14d
    mov r10d,eax
    mov eax,r14d
    imul eax,r14d
    mov dword ptr [rsp + 0xa0],eax
    mov eax,r13d
    cdq
    idiv r14d
    test r10d,r10d
    movsxd r8,eax
    mov qword ptr [rsp + 0x38],r8
    jle .lbl1800119df
    mov qword ptr [r11 + 0x8],rbx
    mov qword ptr [r11 + 0x10],rbp
    movzx ebp,word ptr [rsp + 0x90]
    xor ecx,ecx
    mov rdx,r10
    mov qword ptr [r11 + 0x18],rdi
    mov qword ptr [r11 + -0x28],r12
    mov qword ptr [rsp + 0x20],rcx
    mov qword ptr [rsp + 0x30],rdx
    nop
    nop
    .lbl180011880: test r8,r8
    jle .lbl1800119b1
    xor r12d,r12d
    mov qword ptr [rsp + 0x28],r8
    .lbl180011891: xor r11d,r11d
    test r14d,r14d
    jle .lbl18001193a
    mov rbx,rcx
    mov rdi,r14
    nop
    nop
    nop
    nop
    .lbl1800118b0: xor r8d,r8d
    xor r9d,r9d
    xor r10d,r10d
    cmp rsi,0x2
    jl .lbl180011901
    mov rax,r13
    lea rdx,[rsi + -0x2]
    shr rdx,1
    imul rax,rbx
    add rax,r12
    add rdx,0x1
    lea r10,[rdx + rdx*0x1]
    lea rcx,[r15 + rax*0x2]
    nop
    .lbl1800118e0: xor eax,eax
    cmp word ptr [rcx],bp
    setg al
    add r8d,eax
    xor eax,eax
    cmp word ptr [rcx + 0x2],bp
    setg al
    add rcx,0x4
    add r9d,eax
    sub rdx,0x1
    jnz .lbl1800118e0
    .lbl180011901: cmp r10,rsi
    jge .lbl180011920
    xor ecx,ecx
    mov rax,r13
    imul rax,rbx
    add rax,r12
    add rax,r10
    cmp word ptr [r15 + rax*0x2],bp
    setg cl
    add r11d,ecx
    .lbl180011920: lea ecx,[r9 + r8*0x1]
    add rbx,0x1
    add r11d,ecx
    sub rdi,0x1
    jnz .lbl1800118b0
    mov rcx,qword ptr [rsp + 0x20]
    .lbl18001193a: imul r11d,r11d,0x64
    mov eax,r11d
    cdq
    idiv dword ptr [rsp + 0xa0]
    cmp eax,dword ptr [rsp + 0x98]
    jle .lbl180011993
    test r14d,r14d
    jle .lbl180011993
    mov rbp,qword ptr [rsp + 0x88]
    mov rbx,rcx
    mov rdi,r14
    .lbl180011965: test r14d,r14d
    jle .lbl180011981
    mov rcx,r13
    mov r8,rsi
    xor edx,edx
    imul rcx,rbx
    add rcx,r12
    add rcx,rbp
    call EXTR_memset wrt ..plt
    .lbl180011981: add rbx,0x1
    sub rdi,0x1
    jnz .lbl180011965
    mov bp,word ptr [rsp + 0x90]
    .lbl180011993: mov rcx,qword ptr [rsp + 0x20]
    add r12,rsi
    sub qword ptr [rsp + 0x28],0x1
    jnz .lbl180011891
    mov rdx,qword ptr [rsp + 0x30]
    mov r8,qword ptr [rsp + 0x38]
    .lbl1800119b1: add rcx,rsi
    sub rdx,0x1
    mov qword ptr [rsp + 0x30],rdx
    mov qword ptr [rsp + 0x20],rcx
    jnz .lbl180011880
    mov r12,qword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x80]
    mov rbp,qword ptr [rsp + 0x78]
    mov rbx,qword ptr [rsp + 0x70]
    .lbl1800119df: xor eax,eax
    add rsp,0x48
    pop r15
    pop r14
    pop r13
    pop rsi
    ret

section .data


section .text
EXTR_FUN_180011500:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x20
    mov esi,r8d
    mov ebx,edx
    mov rdi,rcx
    call EXTR_FUN_180010550
    mov r9,qword ptr [rsp + 0x50]
    mov r8d,ebx
    mov edx,esi
    mov rcx,rdi
    mov rbx,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    add rsp,0x20
    pop rdi
    jmp EXTR_FUN_180015da0

section .data


section .text
EXTR_weird_pprocess_wrapper:
    push rbp
    push rsi
    push rdi
    push r12
    push r14
    push r15
    sub rsp,0x88
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x70],rax
    movzx r12d,word ptr [rdx + 0x12]
    movzx ebp,word ptr [rdx + 0x14]
    mov rdi,rdx
    mov qword ptr [rsp + 0xd8],rbx
    mov rsi,rcx
    mov rdx,rcx
    mov rcx,qword ptr [rdi + 0x68]
    mov r14,r8
    mov ebx,r12d
    imul ebx,ebp
    lea r15d,[rbx + rbx*0x1]
    mov r8d,r15d
    call EXTR_memcpy wrt ..plt
    movzx eax,word ptr [rdi + 0x16]
    cmp ax,0x6
    jz .lbl18000bd75
    cmp ax,0x7
    jz .lbl18000bd75
    test byte ptr [rdi + 0xc],0x1
    jz .lbl18000be85
    mov r9,qword ptr [rdi + 0x48]
    xor ecx,ecx
    test r9,r9
    setz cl
    not ecx
    test ecx,ecx
    jz .lbl18000be85
    mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    call EXTR_FUN_18000b700
    jmp .lbl18000be85
    .lbl18000bd75: cmp ax,0x6
    movsxd rbx,ebx
    mov qword ptr [rsp + 0x80],r13
    lea r13,[r14 + rbx*0x2]
    jnz .lbl18000bd9d
    mov r9d,0x1
    mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    call EXTR_FUN_180010770
    .lbl18000bd9d: mov r8d,r15d
    mov rdx,rsi
    mov rcx,r13
    call EXTR_memcpy wrt ..plt
    mov edx,dword ptr [rdi + 0xc]
    test dl,0x1
    jz .lbl18000be06
    mov r9,qword ptr [rdi + 0x48]
    xor ecx,ecx
    test r9,r9
    mov eax,ecx
    setz al
    not eax
    test eax,eax
    jz .lbl18000be06
    mov r8,qword ptr [rdi + 0x50]
    test r8,r8
    setz cl
    not ecx
    test ecx,ecx
    jz .lbl18000bdf9
    bt edx,0x9
    jnc .lbl18000bdf9
    movzx eax,word ptr [rdi + 0x26]
    mov qword ptr [rsp + 0x28],r8
    mov edx,ebp
    mov r8d,r12d
    mov rcx,rsi
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_18000b730
    jmp .lbl18000be06
    .lbl18000bdf9: mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    call EXTR_FUN_18000b700
    .lbl18000be06: cmp word ptr [rdi + 0x16],0x6
    jnz .lbl18000be6a
    lea rbx,[r13 + rbx*0x2]
    lea r9,[rsp + 0x40]
    mov r8d,r12d
    mov qword ptr [rsp + 0x30],rbx
    mov edx,ebp
    mov rcx,rsi
    mov dword ptr [rsp + 0x28],0x1
    mov word ptr [rsp + 0x40],0x1388
    mov word ptr [rsp + 0x42],0x4e20
    mov dword ptr [rsp + 0x20],0x5
    mov word ptr [rsp + 0x44],0x61a8
    mov word ptr [rsp + 0x46],0x4e20
    mov word ptr [rsp + 0x48],0x1388
    call EXTR_FUN_180002820
    mov r8d,r15d
    mov rdx,rbx
    mov rcx,rsi
    call EXTR_memcpy wrt ..plt
    .lbl18000be6a: mov r9d,0x8000
    mov r8d,r12d
    mov edx,ebp
    mov rcx,r13
    call EXTR_FUN_18000b6b0
    mov r13,qword ptr [rsp + 0x80]
    .lbl18000be85: mov eax,dword ptr [rdi + 0xc]
    mov rbx,qword ptr [rsp + 0xd8]
    test al,0x8
    jz .lbl18000bea5
    test al,0x1
    jnz .lbl18000bea5
    mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    call EXTR_normalize_img
    .lbl18000bea5: test byte ptr [rdi + 0xc],0x4
    jz .lbl18000bfb8
    movzx eax,word ptr [rdi + 0x16]
    cmp ax,0x6
    jz .lbl18000bf4d
    cmp ax,0x7
    jz .lbl18000bf4d
    movzx r9d,word ptr [rdi + 0x1a]
    lea rax,[rsp + 0x50]
    mov qword ptr [rsp + 0x30],r14
    mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    mov dword ptr [rsp + 0x28],0xd
    mov word ptr [rsp + 0x50],0x64
    mov word ptr [rsp + 0x52],0x64
    mov qword ptr [rsp + 0x20],rax
    mov word ptr [rsp + 0x54],0x64
    mov word ptr [rsp + 0x56],0x5f
    mov word ptr [rsp + 0x58],0x46
    mov word ptr [rsp + 0x5a],0x28
    mov word ptr [rsp + 0x5c],0x14
    mov word ptr [rsp + 0x5e],0x5
    mov word ptr [rsp + 0x60],0x0
    mov word ptr [rsp + 0x62],0x0
    mov word ptr [rsp + 0x64],0x0
    mov word ptr [rsp + 0x66],0x0
    mov word ptr [rsp + 0x68],0x0
    call EXTR_weird_pprocess
    jmp .lbl18000bfb8
    .lbl18000bf4d: mov r9d,0x1
    mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    mov word ptr [rsp + 0x40],0x181
    mov word ptr [rsp + 0x42],0x2018
    mov word ptr [rsp + 0x44],0x590a
    mov word ptr [rsp + 0x46],0x2018
    mov word ptr [rsp + 0x48],0x181
    call EXTR_FUN_180010770
    mov qword ptr [rsp + 0x30],r14
    lea r9,[rsp + 0x40]
    mov r8d,r12d
    mov edx,ebp
    mov rcx,rsi
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x5
    call EXTR_FUN_180002820
    mov r8d,r15d
    mov rdx,r14
    mov rcx,rsi
    call EXTR_memcpy wrt ..plt
    .lbl18000bfb8: mov rcx,qword ptr [rsp + 0x70]
    xor rcx,rsp
    call EXTR_check_cookie
    add rsp,0x88
    pop r15
    pop r14
    pop r12
    pop rdi
    pop rsi
    pop rbp
    ret

section .data


section .text
EXTR_normalize_img:
    push rdi
    test edx,edx
    movsxd rdi,r8d
    mov r10,rcx
    movsxd r11,edx
    jle .lbl180002102
    mov qword ptr [rsp + 0x10],rbx
    lea rbx,[rdi + rdi*0x1]
    nop
    .lbl180002040: xor edx,edx
    test edi,edi
    jle .lbl18000206f
    lea r8,[rdi + -0x1]
    mov rcx,r10
    shr r8,1
    add r8,0x1
    nop
    nop
    nop
    .lbl180002060: movsx eax,word ptr [rcx]
    add rcx,0x4
    add edx,eax
    sub r8,0x1
    jnz .lbl180002060
    .lbl18000206f: cmp rdi,0x1
    jle .lbl18000209f
    lea r8,[rdi + -0x2]
    lea rax,[r10 + 0x2]
    shr r8,1
    add r8,0x1
    nop
    nop
    nop
    .lbl180002090: movsx ecx,word ptr [rax]
    add rax,0x4
    sub edx,ecx
    sub r8,0x1
    jnz .lbl180002090
    .lbl18000209f: mov eax,edx
    cdq
    idiv edi
    test edi,edi
    mov r8d,eax
    jle .lbl1800020cd
    lea rdx,[rdi + -0x1]
    mov rcx,r10
    shr rdx,1
    add rdx,0x1
    nop
    nop
    .lbl1800020c0: sub word ptr [rcx],ax
    add rcx,0x4
    sub rdx,0x1
    jnz .lbl1800020c0
    .lbl1800020cd: cmp rdi,0x1
    jle .lbl1800020f0
    lea rcx,[rdi + -0x2]
    lea rax,[r10 + 0x2]
    shr rcx,1
    add rcx,0x1
    .lbl1800020e2: add word ptr [rax],r8w
    add rax,0x4
    sub rcx,0x1
    jnz .lbl1800020e2
    .lbl1800020f0: add r10,rbx
    sub r11,0x1
    jnz .lbl180002040
    mov rbx,qword ptr [rsp + 0x10]
    .lbl180002102: pop rdi
    ret

section .data


section .text
EXTR_weird_pprocess:
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    sub rsp,0x48
    movsxd rsi,r8d
    mov edi,r9d
    mov ebx,edx
    mov ecx,esi
    call EXTR_round_pow2
    mov edx,dword ptr [rsp + 0x98]
    mov rcx,qword ptr [rsp + 0x90]
    mov dword ptr [rsp + 0x78],eax
    call EXTR_max_log2ceil
    xor ebp,ebp
    test ebx,ebx
    mov r9d,eax
    mov dword ptr [rsp + 0x20],eax
    jle .lbl1800169c9
    mov rcx,qword ptr [rsp + 0x70]
    mov qword ptr [rsp + 0x80],r12
    mov qword ptr [rsp + 0x40],r13
    mov qword ptr [rsp + 0x38],r14
    mov qword ptr [rsp + 0x30],r15
    mov r13,rbx
    mov r12,rcx
    lea r15,[rsi + rsi*0x1]
    nop
    nop
    .lbl180016910: xor edx,edx
    test esi,esi
    jle .lbl1800169a1
    mov rax,rsi
    imul rax,rbp
    lea rcx,[rcx + rax*0x2]
    .lbl180016925: movsx r8d,word ptr [rcx]
    cmp r8d,edi
    jg .lbl180016949
    mov eax,edi
    neg eax
    cmp r8d,eax
    jl .lbl180016949
    add edx,0x1
    add rcx,0x2
    cmp edx,esi
    jl .lbl180016925
    mov rcx,qword ptr [rsp + 0x70]
    jmp .lbl1800169a1
    .lbl180016949: mov rdx,qword ptr [rsp + 0xa0]
    mov r8d,esi
    mov rcx,r12
    call EXTR_weird_pprocess2
    mov r9d,dword ptr [rsp + 0x98]
    mov r8,qword ptr [rsp + 0x90]
    mov edx,dword ptr [rsp + 0x78]
    mov rcx,qword ptr [rsp + 0xa0]
    mov ebx,eax
    call EXTR_apply_range_mutlpls
    mov rdx,qword ptr [rsp + 0xa0]
    movzx r9d,bx
    movzx r8d,si
    mov rcx,r12
    call EXTR_undo_weird_pprocess
    mov rcx,qword ptr [rsp + 0x70]
    mov r9d,dword ptr [rsp + 0x20]
    .lbl1800169a1: add r12,r15
    add rbp,0x1
    sub r13,0x1
    jnz .lbl180016910
    mov r15,qword ptr [rsp + 0x30]
    mov r14,qword ptr [rsp + 0x38]
    mov r13,qword ptr [rsp + 0x40]
    mov r12,qword ptr [rsp + 0x80]
    .lbl1800169c9: xor eax,eax
    add rsp,0x48
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_apply_range_mutlpls:
    push rsi
    push r13
    push r14
    push r15
    sub rsp,0x28
    movsxd r10,edx
    xor r11d,r11d
    mov r14d,r9d
    mov eax,r10d
    mov r15,r8
    mov rsi,rcx
    cdq
    sub eax,edx
    sar eax,1
    mov r13d,eax
    js .lbl180016875
    mov qword ptr [rsp + 0x50],rbx
    mov qword ptr [rsp + 0x58],rbp
    mov qword ptr [rsp + 0x60],rdi
    lea rbp,[rcx + r10*0x2]
    lea rbx,[rcx + r10*0x2]
    lea rcx,[r10 + r10*0x1]
    mov qword ptr [rsp + 0x20],r12
    mov r12d,r11d
    lea rdi,[rsi + rcx*0x2]
    .lbl1800167b3: mov eax,r12d
    mov rcx,r15
    cdq
    idiv r13d
    mov edx,r14d
    mov r8d,eax
    call EXTR_query_range
    movsx ecx,word ptr [rsi]
    mov r10d,eax
    imul ecx,eax
    mov eax,0x51eb851f
    imul ecx
    mov eax,0x51eb851f
    sar edx,0x5
    mov ecx,edx
    shr ecx,0x1f
    add edx,ecx
    mov word ptr [rsi],dx
    movsx ecx,word ptr [rbp]
    imul ecx,r10d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    test r11d,r11d
    mov word ptr [rbp],dx
    jle .lbl18001683d
    movsx ecx,word ptr [rbx]
    mov eax,0x51eb851f
    imul ecx,r10d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov eax,0x51eb851f
    mov word ptr [rbx],dx
    movsx ecx,word ptr [rdi]
    imul ecx,r10d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov word ptr [rdi],dx
    .lbl18001683d: add r11d,0x1
    add r12d,0x400
    add rsi,0x2
    add rbp,0x2
    sub rbx,0x2
    sub rdi,0x2
    cmp r11d,r13d
    jle .lbl1800167b3
    mov r12,qword ptr [rsp + 0x20]
    mov rdi,qword ptr [rsp + 0x60]
    mov rbp,qword ptr [rsp + 0x58]
    mov rbx,qword ptr [rsp + 0x50]
    .lbl180016875: add rsp,0x28
    pop r15
    pop r14
    pop r13
    pop rsi
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
EXTR_weird_pprocess2:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x20
    movsxd rdi,r8d
    mov rbx,rcx
    mov r13d,r9d
    mov ecx,edi
    mov rsi,rdx
    call EXTR_log2ceil
    mov ebp,0x1
    xor edx,edx
    movzx ecx,al
    mov r12d,eax
    shl ebp,cl
    mov rcx,rsi
    lea r8d,[rbp*0x4]
    call EXTR_memset wrt ..plt
    test edi,edi
    jle .lbl180017923
    mov r8,rbx
    mov rcx,rsi
    mov rdx,rdi
    sub r8,rsi
    nop
    nop
    .lbl180017910: movzx eax,word ptr [r8 + rcx*0x1]
    add rcx,0x2
    sub rdx,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl180017910
    .lbl180017923: mov eax,ebp
    sub eax,edi
    cdq
    sub eax,edx
    sar eax,1
    lea r8d,[rax + rdi*0x1]
    cmp edi,r8d
    jge .lbl180017953
    mov edx,r8d
    lea rcx,[rsi + rdi*0x2]
    sub edx,edi
    nop
    .lbl180017940: movzx eax,word ptr [rbx + rdi*0x2 + -0x2]
    add rcx,0x2
    sub rdx,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl180017940
    .lbl180017953: movsxd rcx,r8d
    movsxd rdi,ebp
    cmp rcx,rdi
    jge .lbl180017971
    nop
    .lbl180017960: movzx eax,word ptr [rbx]
    add rcx,0x1
    cmp rcx,rdi
    mov word ptr [rsi + rcx*0x2 + -0x2],ax
    jl .lbl180017960
    .lbl180017971: mov r8d,0xf
    mov edx,ebp
    mov rcx,rsi
    sub r8d,r13d
    call EXTR_shift_image
    lea rdx,[rsi + rdi*0x2]
    xor r9d,r9d
    movzx r8d,r12w
    mov rcx,rsi
    movsx ebx,ax
    call EXTR_weird_img_shuffle
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    mov eax,ebx
    mov rbx,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_shift_image:
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
    jle .lbl180016ca3
    nop
    nop
    .lbl180016c90: movsx eax,word ptr [rdi + r8*0x2]
    cmp ecx,eax
    cmovl ecx,eax
    add r8,0x1
    cmp r8,rsi
    jl .lbl180016c90
    .lbl180016ca3: call EXTR_log2ceil
    sub ebp,eax
    jns .lbl180016cb1
    xor ax,ax
    jmp .lbl180016cd4
    .lbl180016cb1: test rsi,rsi
    jle .lbl180016cd1
    nop
    nop
    nop
    .lbl180016cc0: movzx ecx,bp
    add rbx,0x1
    shl word ptr [rdi + rbx*0x2 + -0x2],cl
    cmp rbx,rsi
    jl .lbl180016cc0
    .lbl180016cd1: mov ax,bp
    .lbl180016cd4: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_weird_img_shuffle:
    mov word ptr [rsp + 0x20],r9w
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rdi
    push r12
    sub rsp,0x68
    mov ebx,0x1
    mov rbp,rcx
    movzx ecx,r8b
    mov r8d,ebx
    movzx edi,r9w
    mov r12,rdx
    shl r8d,cl
    mov dword ptr [rsp + 0x30],ebx
    cmp r8d,0x400
    mov dword ptr [rsp + 0x24],r8d
    jle .lbl180019142
    or ax,0xffff
    add rsp,0x68
    pop r12
    pop rdi
    pop rbp
    pop rbx
    ret
    .lbl180019142: mov qword ptr [rsp + 0x60],rsi
    xor si,si
    lea r10d,[r8 + -0x1]
    xor r9d,r9d
    cmp r10d,ebx
    mov qword ptr [rsp + 0x58],r13
    mov word ptr [rsp + 0x22],si
    movzx r11d,bx
    jl .lbl1800191d0
    mov edx,ebx
    nop
    nop
    nop
    .lbl180019170: mov ecx,r8d
    .lbl180019173: sar ecx,1
    lea eax,[rcx + r9*0x1]
    cmp eax,r10d
    jg .lbl180019173
    lea eax,[rcx + -0x1]
    and eax,r9d
    lea r9d,[rcx + rax*0x1]
    cmp r9d,edx
    jle .lbl1800191c3
    movsx r8,r11w
    movsxd rdx,r9d
    movzx ecx,word ptr [rbp + r8*0x2]
    movzx eax,word ptr [rbp + rdx*0x2]
    mov word ptr [rbp + r8*0x2],ax
    mov word ptr [rbp + rdx*0x2],cx
    movzx ecx,word ptr [r12 + r8*0x2]
    movzx eax,word ptr [r12 + rdx*0x2]
    mov word ptr [r12 + r8*0x2],ax
    mov r8d,dword ptr [rsp + 0x24]
    mov word ptr [r12 + rdx*0x2],cx
    .lbl1800191c3: add r11w,bx
    movsx edx,r11w
    cmp edx,r10d
    jle .lbl180019170
    .lbl1800191d0: cmp r8d,ebx
    mov r13d,0x9
    mov dword ptr [rsp + 0x2c],r13d
    jle .lbl180019437
    mov qword ptr [rsp + 0x50],r14
    mov qword ptr [rsp + 0x48],r15
    lea r15,[glb180174770]
    movsxd r14,r8d
    .lbl1800191f8: test di,di
    jz .lbl180019276
    xor r10w,r10w
    xor r8d,r8d
    test r14,r14
    mov word ptr [rsp + 0xa0],r10w
    jle .lbl180019286
    mov r9,rbp
    mov rdx,r12
    sub r9,r12
    nop
    nop
    .lbl180019220: movsx eax,word ptr [r9 + rdx*0x1]
    test eax,eax
    jns .lbl18001922b
    neg eax
    .lbl18001922b: movzx ecx,word ptr [rdx]
    test cx,cx
    jns .lbl180019236
    neg cx
    .lbl180019236: cmp eax,0x3fff
    jg .lbl180019258
    cmp cx,0x3fff
    jg .lbl180019258
    add r8,0x1
    add rdx,0x2
    cmp r8,r14
    jl .lbl180019220
    mov r8d,dword ptr [rsp + 0x24]
    jmp .lbl18001928b
    .lbl180019258: mov r8d,dword ptr [rsp + 0x24]
    mov r10w,0x1
    add si,r10w
    mov word ptr [rsp + 0xa0],r10w
    mov word ptr [rsp + 0x22],si
    jmp .lbl18001928b
    .lbl180019276: mov r10w,0x1
    mov word ptr [rsp + 0xa0],r10w
    jmp .lbl18001928b
    .lbl180019286: mov r8d,dword ptr [rsp + 0x24]
    .lbl18001928b: xor r9w,r9w
    test ebx,ebx
    lea r11d,[rbx + rbx*0x1]
    mov dword ptr [rsp + 0x28],r11d
    mov word ptr [rsp + 0x20],r9w
    jle .lbl180019414
    movsxd rax,ebx
    xor edx,edx
    movsxd r8,r11d
    mov qword ptr [rsp + 0x38],rax
    .lbl1800192b3: mov ecx,r13d
    mov eax,edx
    shl eax,cl
    movsxd rcx,eax
    lea rax,[glb180174770]
    movzx esi,word ptr [rax + rcx*0x2]
    movzx r15d,word ptr [r15 + rcx*0x2 + 0x200]
    neg si
    test di,di
    jz .lbl1800192dc
    neg si
    .lbl1800192dc: test r10w,r10w
    jz .lbl1800192e9
    sar r15w,1
    sar si,1
    .lbl1800192e9: movsxd rdi,edx
    cmp rdi,r14
    jge .lbl1800193d8
    mov rax,qword ptr [rsp + 0x38]
    lea r13,[r8 + r8*0x1]
    lea r11,[r12 + rdi*0x2]
    add rax,rdi
    sub rbp,r12
    lea rbx,[r12 + rax*0x2]
    nop
    .lbl180019310: movzx r9d,word ptr [rbx + rbp*0x1]
    movzx r10d,word ptr [rbx]
    movzx ecx,r15w
    movzx edx,r9w
    call EXTR_weird_multpl
    movzx edx,r10w
    movzx ecx,si
    movzx r12d,ax
    call EXTR_weird_multpl
    movzx edx,r9w
    movzx ecx,si
    sub r12w,ax
    call EXTR_weird_multpl
    movzx edx,r10w
    movzx ecx,r15w
    movzx r9d,ax
    call EXTR_weird_multpl
    movzx r10d,word ptr [rsp + 0xa0]
    movzx ecx,word ptr [r11 + rbp*0x1]
    movzx edx,word ptr [r11]
    add r9w,ax
    test r10w,r10w
    jz .lbl180019379
    sar cx,1
    sar dx,1
    .lbl180019379: movzx eax,cx
    add cx,r12w
    sub ax,r12w
    mov word ptr [rbx + rbp*0x1],ax
    movzx eax,dx
    add dx,r9w
    sub ax,r9w
    mov word ptr [rbx],ax
    movsxd rax,dword ptr [rsp + 0x28]
    mov word ptr [r11 + rbp*0x1],cx
    add rdi,rax
    mov word ptr [r11],dx
    add rbx,r13
    add r11,r13
    cmp rdi,r14
    mov r8,rax
    jl .lbl180019310
    movzx r9d,word ptr [rsp + 0x20]
    mov r12,qword ptr [rsp + 0x98]
    mov ebx,dword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x90]
    mov r13d,dword ptr [rsp + 0x2c]
    .lbl1800193d8: mov di,word ptr [rsp + 0xa8]
    add r9w,0x1
    lea r15,[glb180174770]
    movsx edx,r9w
    mov word ptr [rsp + 0x20],r9w
    cmp edx,ebx
    jl .lbl1800192b3
    mov r11d,dword ptr [rsp + 0x28]
    mov r8d,dword ptr [rsp + 0x24]
    movzx esi,word ptr [rsp + 0x22]
    lea r15,[glb180174770]
    .lbl180019414: sub r13d,0x1
    cmp r11d,r8d
    mov ebx,r11d
    mov dword ptr [rsp + 0x2c],r13d
    mov dword ptr [rsp + 0x30],ebx
    jl .lbl1800191f8
    mov r15,qword ptr [rsp + 0x48]
    mov r14,qword ptr [rsp + 0x50]
    .lbl180019437: mov r13,qword ptr [rsp + 0x58]
    mov ax,si
    mov rsi,qword ptr [rsp + 0x60]
    add rsp,0x68
    pop r12
    pop rdi
    pop rbp
    pop rbx
    ret

section .data

glb180174770: db 0, 0, -55, 0, -110, 1, 91, 2, 36, 3, -19, 3, -74, 4, 127, 5, 72, 6, 17, 7, -39, 7, -94, 8, 107, 9, 51, 10, -5, 10, -60, 11, -116, 12, 84, 13, 28, 14, -28, 14, -85, 15, 115, 16, 58, 17, 1, 18, -56, 18, -113, 19, 85, 20, 28, 21, -30, 21, -88, 22, 110, 23, 51, 24, -7, 24, -66, 25, -125, 26, 71, 27, 12, 28, -48, 28, -109, 29, 87, 30, 26, 31, -35, 31, -97, 32, 98, 33, 36, 34, -27, 34, -89, 35, 103, 36, 40, 37, -24, 37, -88, 38, 104, 39, 39, 40, -27, 40, -92, 41, 98, 42, 31, 43, -36, 43, -103, 44, 85, 45, 17, 46, -52, 46, -121, 47, 66, 48, -4, 48, -75, 49, 110, 50, 39, 51, -33, 51, -105, 52, 78, 53, 4, 54, -70, 54, 112, 55, 37, 56, -39, 56, -115, 57, 64, 58, -13, 58, -91, 59, 87, 60, 8, 61, -72, 61, 104, 62, 23, 63, -58, 63, 116, 64, 33, 65, -50, 65, 122, 66, 38, 67, -47, 67, 123, 68, 36, 69, -51, 69, 117, 70, 29, 71, -60, 71, 106, 72, 15, 73, -76, 73, 88, 74, -5, 74, -98, 75, 64, 76, -31, 76, -127, 77, 33, 78, -64, 78, 94, 79, -5, 79, -104, 80, 52, 81, -49, 81, 105, 82, 3, 83, -101, 83, 51, 84, -54, 84, 96, 85, -10, 85, -118, 86, 30, 87, -79, 87, 67, 88, -44, 88, 100, 89, -12, 89, -126, 90, 16, 91, -99, 91, 41, 92, -76, 92, 62, 93, -56, 93, 80, 94, -41, 94, 94, 95, -28, 95, 104, 96, -20, 96, 111, 97, -15, 97, 114, 98, -14, 98, 113, 99, -17, 99, 108, 100, -23, 100, 100, 101, -34, 101, 87, 102, -48, 102, 71, 103, -67, 103, 50, 104, -89, 104, 26, 105, -116, 105, -3, 105, 110, 106, -35, 106, 75, 107, -72, 107, 36, 108, -113, 108, -7, 108, 98, 109, -54, 109, 49, 110, -105, 110, -5, 110, 95, 111, -62, 111, 35, 112, -125, 112, -29, 112, 65, 113, -98, 113, -6, 113, 85, 114, -81, 114, 8, 115, 95, 115, -74, 115, 11, 116, 96, 116, -77, 116, 5, 117, 86, 117, -90, 117, -12, 117, 66, 118, -114, 118, -39, 118, 35, 119, 108, 119, -76, 119, -5, 119, 64, 120, -123, 120, -56, 120, 10, 121, 74, 121, -118, 121, -55, 121, 6, 122, 66, 122, 125, 122, -73, 122, -17, 122, 39, 123, 93, 123, -110, 123, -58, 123, -7, 123, 42, 124, 90, 124, -119, 124, -73, 124, -28, 124, 15, 125, 58, 125, 99, 125, -118, 125, -79, 125, -42, 125, -5, 125, 30, 126, 63, 126, 96, 126, 127, 126, -99, 126, -70, 126, -42, 126, -16, 126, 10, 127, 34, 127, 56, 127, 78, 127, 98, 127, 117, 127, -121, 127, -104, 127, -89, 127, -75, 127, -62, 127, -50, 127, -39, 127, -30, 127, -22, 127, -15, 127, -10, 127, -6, 127, -2, 127, -1, 127, -1, 127, -1, 127, -2, 127, -6, 127, -10, 127, -15, 127, -22, 127, -30, 127, -39, 127, -50, 127, -62, 127, -75, 127, -89, 127, -104, 127, -121, 127, 117, 127, 98, 127, 78, 127, 56, 127, 34, 127, 10, 127, -16, 126, -42, 126, -70, 126, -99, 126, 127, 126, 96, 126, 63, 126, 30, 126, -5, 125, -42, 125, -79, 125, -118, 125, 99, 125, 58, 125, 15, 125, -28, 124, -73, 124, -119, 124, 90, 124, 42, 124, -7, 123, -58, 123, -110, 123, 93, 123, 39, 123, -17, 122, -73, 122, 125, 122, 66, 122, 6, 122, -55, 121, -118, 121, 74, 121, 10, 121, -56, 120, -123, 120, 64, 120, -5, 119, -76, 119, 108, 119, 35, 119, -39, 118, -114, 118, 66, 118, -12, 117, -90, 117, 86, 117, 5, 117, -77, 116, 96, 116, 11, 116, -74, 115, 95, 115, 8, 115, -81, 114, 85, 114, -6, 113, -98, 113, 65, 113, -29, 112, -125, 112, 35, 112, -62, 111, 95, 111, -5, 110, -105, 110, 49, 110, -54, 109, 98, 109, -7, 108, -113, 108, 36, 108, -72, 107, 75, 107, -35, 106, 110, 106, -3, 105, -116, 105, 26, 105, -89, 104, 50, 104, -67, 103, 71, 103, -48, 102, 87, 102, -34, 101, 100, 101, -23, 100, 108, 100, -17, 99, 113, 99, -14, 98, 114, 98, -15, 97, 111, 97, -20, 96, 104, 96, -28, 95, 94, 95, -41, 94, 80, 94, -56, 93, 62, 93, -76, 92, 41, 92, -99, 91, 16, 91, -126, 90, -12, 89, 100, 89, -44, 88, 67, 88, -79, 87, 30, 87, -118, 86, -10, 85, 96, 85, -54, 84, 51, 84, -101, 83, 3, 83, 105, 82, -49, 81, 52, 81, -104, 80, -5, 79, 94, 79, -64, 78, 33, 78, -127, 77, -31, 76, 64, 76, -98, 75, -5, 74, 88, 74, -76, 73, 15, 73, 106, 72, -60, 71, 29, 71, 117, 70, -51, 69, 36, 69, 123, 68, -47, 67, 38, 67, 122, 66, -50, 65, 33, 65, 116, 64, -58, 63, 23, 63, 104, 62, -72, 61, 8, 61, 87, 60, -91, 59, -13, 58, 64, 58, -115, 57, -39, 56, 37, 56, 112, 55, -70, 54, 4, 54, 78, 53, -105, 52, -33, 51, 39, 51, 110, 50, -75, 49, -4, 48, 66, 48, -121, 47, -52, 46, 17, 46, 85, 45, -103, 44, -36, 43, 31, 43, 98, 42, -92, 41, -27, 40, 39, 40, 104, 39, -88, 38, -24, 37, 40, 37, 103, 36, -89, 35, -27, 34, 36, 34, 98, 33, -97, 32, -35, 31, 26, 31, 87, 30, -109, 29, -48, 28, 12, 28, 71, 27, -125, 26, -66, 25, -7, 24, 51, 24, 110, 23, -88, 22, -30, 21, 28, 21, 85, 20, -113, 19, -56, 18, 1, 18, 58, 17, 115, 16, -85, 15, -28, 14, 28, 14, 84, 13, -116, 12, -60, 11, -5, 10, 51, 10, 107, 9, -94, 8, -39, 7, 17, 7, 72, 6, 127, 5, -74, 4, -19, 3, 36, 3, 91, 2, -110, 1, -55, 0, 0, 0, 55, -1, 110, -2, -91, -3, -36, -4, 19, -4, 74, -5, -127, -6, -72, -7, -17, -8, 39, -8, 94, -9, -107, -10, -51, -11, 5, -11, 60, -12, 116, -13, -84, -14, -28, -15, 28, -15, 85, -16, -115, -17, -58, -18, -1, -19, 56, -19, 113, -20, -85, -21, -28, -22, 30, -22, 88, -23, -110, -24, -51, -25, 7, -25, 66, -26, 125, -27, -71, -28, -12, -29, 48, -29, 109, -30, -87, -31, -26, -32, 35, -32, 97, -33, -98, -34, -36, -35, 27, -35, 89, -36, -103, -37, -40, -38, 24, -38, 88, -39, -104, -40, -39, -41, 27, -41, 92, -42, -98, -43, -31, -44, 36, -44, 103, -45, -85, -46, -17, -47, 52, -47, 121, -48, -66, -49, 4, -49, 75, -50, -110, -51, -39, -52, 33, -52, 105, -53, -78, -54, -4, -55, 70, -55, -112, -56, -37, -57, 39, -57, 115, -58, -64, -59, 13, -59, 91, -60, -87, -61, -8, -62, 72, -62, -104, -63, -23, -64, 58, -64, -116, -65, -33, -66, 50, -66, -122, -67, -38, -68, 47, -68, -123, -69, -36, -70, 51, -70, -117, -71, -29, -72, 60, -72, -106, -73, -15, -74, 76, -74, -88, -75, 5, -75, 98, -76, -64, -77, 31, -77, 127, -78, -33, -79, 64, -79, -94, -80, 5, -80, 104, -81, -52, -82, 49, -82, -105, -83, -3, -84, 101, -84, -51, -85, 54, -85, -96, -86, 10, -86, 118, -87, -30, -88, 79, -88, -67, -89, 44, -89, -100, -90, 12, -90, 126, -91, -16, -92, 99, -92, -41, -93, 76, -93, -62, -94, 56, -94, -80, -95, 41, -95, -94, -96, 28, -96, -104, -97, 20, -97, -111, -98, 15, -98, -114, -99, 14, -99, -113, -100, 17, -100, -108, -101, 23, -101, -100, -102, 34, -102, -87, -103, 48, -103, -71, -104, 67, -104, -50, -105, 89, -105, -26, -106, 116, -106, 3, -106, -110, -107, 35, -107, -75, -108, 72, -108, -36, -109, 113, -109, 7, -109, -98, -110, 54, -110, -49, -111, 105, -111, 5, -111, -95, -112, 62, -112, -35, -113, 125, -113, 29, -113, -65, -114, 98, -114, 6, -114, -85, -115, 81, -115, -8, -116, -95, -116, 74, -116, -11, -117, -96, -117, 77, -117, -5, -118, -86, -118, 90, -118, 12, -118, -66, -119, 114, -119, 39, -119, -35, -120, -108, -120, 76, -120, 5, -120, -64, -121, 123, -121, 56, -121, -10, -122, -74, -122, 118, -122, 55, -122, -6, -123, -66, -123, -125, -123, 73, -123, 17, -123, -39, -124, -93, -124, 110, -124, 58, -124, 7, -124, -42, -125, -90, -125, 119, -125, 73, -125, 28, -125, -15, -126, -58, -126, -99, -126, 118, -126, 79, -126, 42, -126, 5, -126, -30, -127, -63, -127, -96, -127, -127, -127, 99, -127, 70, -127, 42, -127, 16, -127, -10, -128, -34, -128, -56, -128, -78, -128, -98, -128, -117, -128, 121, -128, 104, -128, 89, -128, 75, -128, 62, -128, 50, -128, 39, -128, 30, -128, 22, -128, 15, -128, 10, -128, 6, -128, 2, -128, 1, -128, 0, 0, 0, 0, -120, 15, -55, 0, 32, 29, -110, 1, -41, 38, 91, 2, -65, 42, 36, 3, -26, 38, -19, 3, 93, 25, -74, 4, 53, 0, 127, 5, 124, -39, 71, 6, 69, -93, 16, 7, -98, 91, -39, 7, -102, 0, -94, 8, 73, -112, 106, 9, -67, 8, 51, 10, 5, 104, -5, 10, 53, -84, -61, 11, 94, -45, -117, 12, -110, -37, 83, 13, -28, -62, 27, 14, 102, -121, -29, 14, 43, 39, -85, 15, 72, -96, 114, 16, -49, -16, 57, 17, -43, 22, 1, 18, 111, 16, -56, 18, -79, -37, -114, 19, -79, 118, 85, 20, -122, -33, 27, 21, 69, 20, -30, 21, 5, 19, -88, 22, -34, -39, 109, 23, -23, 102, 51, 24, 60, -72, -8, 24, -13, -53, -67, 25, 38, -96, -126, 26, -17, 50, 71, 27, 106, -126, 11, 28, -77, -116, -49, 28, -27, 79, -109, 29, 30, -54, 86, 30, 123, -7, 25, 31, 27, -36, -36, 31, 28, 112, -97, 32, -96, -77, 97, 33, -59, -92, 35, 34, -81, 65, -27, 34, 127, -120, -90, 35, 88, 119, 103, 36, 94, 12, 40, 37, -74, 69, -24, 37, -122, 33, -88, 38, -12, -99, 103, 39, 40, -71, 38, 40, 75, 113, -27, 40, -123, -60, -93, 41, 1, -79, 97, 42, -21, 52, 31, 43, 111, 78, -36, 43, -70, -5, -104, 44, -4, 58, 85, 45, 98, 10, 17, 46, 30, 104, -52, 46, 98, 82, -121, 47, 97, -57, 65, 48, 77, -59, -5, 48, 94, 74, -75, 49, -57, 84, 110, 50, -61, -30, 38, 51, -121, -14, -34, 51, 80, -126, -106, 52, 87, -112, 77, 53, -39, 26, 4, 54, 20, 32, -70, 54, 70, -98, 111, 55, -80, -109, 36, 56, -109, -2, -40, 56, 50, -35, -116, 57, -46, 45, 64, 58, -73, -18, -14, 58, 41, 30, -91, 59, 112, -70, 86, 60, -42, -63, 7, 61, -90, 50, -72, 61, 44, 11, 104, 62, -72, 73, 23, 63, -104, -20, -59, 63, 29, -14, 115, 64, -101, 88, 33, 65, 101, 30, -50, 65, -48, 65, 122, 66, 53, -63, 37, 67, -19, -102, -48, 67, 80, -51, 122, 68, -67, 86, 36, 69, -113, 53, -51, 69, 40, 104, 117, 70, -25, -20, 28, 71, 47, -62, -61, 71, 101, -26, 105, 72, -18, 87, 15, 73, 51, 21, -76, 73, -98, 28, 88, 74, -104, 108, -5, 74, -112, 3, -98, 75, -12, -33, 63, 76, 52, 0, -31, 76, -60, 98, -127, 77, 23, 6, 33, 78, -91, -24, -65, 78, -29, 8, 94, 79, 77, 101, -5, 79, 94, -4, -105, 80, -108, -52, 51, 81, 110, -44, -50, 81, 110, 18, 105, 82, 24, -123, 2, 83, -16, 42, -101, 83, 125, 2, 51, 84, 75, 10, -54, 84, -30, 64, 96, 85, -46, -92, -11, 85, -87, 52, -118, 86, -6, -18, 29, 87, 86, -46, -80, 87, 84, -35, 66, 88, -116, 14, -44, 88, -104, 100, 100, 89, 18, -34, -13, 89
glb180174970: db -1, 127, -1, 127, -2, 127, -6, 127, -10, 127, -15, 127, -22, 127, -30, 127, -39, 127, -50, 127, -62, 127, -75, 127, -89, 127, -104, 127, -121, 127, 117, 127, 98, 127, 78, 127, 56, 127, 34, 127, 10, 127, -16, 126, -42, 126, -70, 126, -99, 126, 127, 126, 96, 126, 63, 126, 30, 126, -5, 125, -42, 125, -79, 125, -118, 125, 99, 125, 58, 125, 15, 125, -28, 124, -73, 124, -119, 124, 90, 124, 42, 124, -7, 123, -58, 123, -110, 123, 93, 123, 39, 123, -17, 122, -73, 122, 125, 122, 66, 122, 6, 122, -55, 121, -118, 121, 74, 121, 10, 121, -56, 120, -123, 120, 64, 120, -5, 119, -76, 119, 108, 119, 35, 119, -39, 118, -114, 118, 66, 118, -12, 117, -90, 117, 86, 117, 5, 117, -77, 116, 96, 116, 11, 116, -74, 115, 95, 115, 8, 115, -81, 114, 85, 114, -6, 113, -98, 113, 65, 113, -29, 112, -125, 112, 35, 112, -62, 111, 95, 111, -5, 110, -105, 110, 49, 110, -54, 109, 98, 109, -7, 108, -113, 108, 36, 108, -72, 107, 75, 107, -35, 106, 110, 106, -3, 105, -116, 105, 26, 105, -89, 104, 50, 104, -67, 103, 71, 103, -48, 102, 87, 102, -34, 101, 100, 101, -23, 100, 108, 100, -17, 99, 113, 99, -14, 98, 114, 98, -15, 97, 111, 97, -20, 96, 104, 96, -28, 95, 94, 95, -41, 94, 80, 94, -56, 93, 62, 93, -76, 92, 41, 92, -99, 91, 16, 91, -126, 90, -12, 89, 100, 89, -44, 88, 67, 88, -79, 87, 30, 87, -118, 86, -10, 85, 96, 85, -54, 84, 51, 84, -101, 83, 3, 83, 105, 82, -49, 81, 52, 81, -104, 80, -5, 79, 94, 79, -64, 78, 33, 78, -127, 77, -31, 76, 64, 76, -98, 75, -5, 74, 88, 74, -76, 73, 15, 73, 106, 72, -60, 71, 29, 71, 117, 70, -51, 69, 36, 69, 123, 68, -47, 67, 38, 67, 122, 66, -50, 65, 33, 65, 116, 64, -58, 63, 23, 63, 104, 62, -72, 61, 8, 61, 87, 60, -91, 59, -13, 58, 64, 58, -115, 57, -39, 56, 37, 56, 112, 55, -70, 54, 4, 54, 78, 53, -105, 52, -33, 51, 39, 51, 110, 50, -75, 49, -4, 48, 66, 48, -121, 47, -52, 46, 17, 46, 85, 45, -103, 44, -36, 43, 31, 43, 98, 42, -92, 41, -27, 40, 39, 40, 104, 39, -88, 38, -24, 37, 40, 37, 103, 36, -89, 35, -27, 34, 36, 34, 98, 33, -97, 32, -35, 31, 26, 31, 87, 30, -109, 29, -48, 28, 12, 28, 71, 27, -125, 26, -66, 25, -7, 24, 51, 24, 110, 23, -88, 22, -30, 21, 28, 21, 85, 20, -113, 19, -56, 18, 1, 18, 58, 17, 115, 16, -85, 15, -28, 14, 28, 14, 84, 13, -116, 12, -60, 11, -5, 10, 51, 10, 107, 9, -94, 8, -39, 7, 17, 7, 72, 6, 127, 5, -74, 4, -19, 3, 36, 3, 91, 2, -110, 1, -55, 0, 0, 0, 55, -1, 110, -2, -91, -3, -36, -4, 19, -4, 74, -5, -127, -6, -72, -7, -17, -8, 39, -8, 94, -9, -107, -10, -51, -11, 5, -11, 60, -12, 116, -13, -84, -14, -28, -15, 28, -15, 85, -16, -115, -17, -58, -18, -1, -19, 56, -19, 113, -20, -85, -21, -28, -22, 30, -22, 88, -23, -110, -24, -51, -25, 7, -25, 66, -26, 125, -27, -71, -28, -12, -29, 48, -29, 109, -30, -87, -31, -26, -32, 35, -32, 97, -33, -98, -34, -36, -35, 27, -35, 89, -36, -103, -37, -40, -38, 24, -38, 88, -39, -104, -40, -39, -41, 27, -41, 92, -42, -98, -43, -31, -44, 36, -44, 103, -45, -85, -46, -17, -47, 52, -47, 121, -48, -66, -49, 4, -49, 75, -50, -110, -51, -39, -52, 33, -52, 105, -53, -78, -54, -4, -55, 70, -55, -112, -56, -37, -57, 39, -57, 115, -58, -64, -59, 13, -59, 91, -60, -87, -61, -8, -62, 72, -62, -104, -63, -23, -64, 58, -64, -116, -65, -33, -66, 50, -66, -122, -67, -38, -68, 47, -68, -123, -69, -36, -70, 51, -70, -117, -71, -29, -72, 60, -72, -106, -73, -15, -74, 76, -74, -88, -75, 5, -75, 98, -76, -64, -77, 31, -77, 127, -78, -33, -79, 64, -79, -94, -80, 5, -80, 104, -81, -52, -82, 49, -82, -105, -83, -3, -84, 101, -84, -51, -85, 54, -85, -96, -86, 10, -86, 118, -87, -30, -88, 79, -88, -67, -89, 44, -89, -100, -90, 12, -90, 126, -91, -16, -92, 99, -92, -41, -93, 76, -93, -62, -94, 56, -94, -80, -95, 41, -95, -94, -96, 28, -96, -104, -97, 20, -97, -111, -98, 15, -98, -114, -99, 14, -99, -113, -100, 17, -100, -108, -101, 23, -101, -100, -102, 34, -102, -87, -103, 48, -103, -71, -104, 67, -104, -50, -105, 89, -105, -26, -106, 116, -106, 3, -106, -110, -107, 35, -107, -75, -108, 72, -108, -36, -109, 113, -109, 7, -109, -98, -110, 54, -110, -49, -111, 105, -111, 5, -111, -95, -112, 62, -112, -35, -113, 125, -113, 29, -113, -65, -114, 98, -114, 6, -114, -85, -115, 81, -115, -8, -116, -95, -116, 74, -116, -11, -117, -96, -117, 77, -117, -5, -118, -86, -118, 90, -118, 12, -118, -66, -119, 114, -119, 39, -119, -35, -120, -108, -120, 76, -120, 5, -120, -64, -121, 123, -121, 56, -121, -10, -122, -74, -122, 118, -122, 55, -122, -6, -123, -66, -123, -125, -123, 73, -123, 17, -123, -39, -124, -93, -124, 110, -124, 58, -124, 7, -124, -42, -125, -90, -125, 119, -125, 73, -125, 28, -125, -15, -126, -58, -126, -99, -126, 118, -126, 79, -126, 42, -126, 5, -126, -30, -127, -63, -127, -96, -127, -127, -127, 99, -127, 70, -127, 42, -127, 16, -127, -10, -128, -34, -128, -56, -128, -78, -128, -98, -128, -117, -128, 121, -128, 104, -128, 89, -128, 75, -128, 62, -128, 50, -128, 39, -128, 30, -128, 22, -128, 15, -128, 10, -128, 6, -128, 2, -128, 1, -128, 0, 0, 0, 0, -120, 15, -55, 0, 32, 29, -110, 1, -41, 38, 91, 2, -65, 42, 36, 3, -26, 38, -19, 3, 93, 25, -74, 4, 53, 0, 127, 5, 124, -39, 71, 6, 69, -93, 16, 7, -98, 91, -39, 7, -102, 0, -94, 8, 73, -112, 106, 9, -67, 8, 51, 10, 5, 104, -5, 10, 53, -84, -61, 11, 94, -45, -117, 12, -110, -37, 83, 13, -28, -62, 27, 14, 102, -121, -29, 14, 43, 39, -85, 15, 72, -96, 114, 16, -49, -16, 57, 17, -43, 22, 1, 18, 111, 16, -56, 18, -79, -37, -114, 19, -79, 118, 85, 20, -122, -33, 27, 21, 69, 20, -30, 21, 5, 19, -88, 22, -34, -39, 109, 23, -23, 102, 51, 24, 60, -72, -8, 24, -13, -53, -67, 25, 38, -96, -126, 26, -17, 50, 71, 27, 106, -126, 11, 28, -77, -116, -49, 28, -27, 79, -109, 29, 30, -54, 86, 30, 123, -7, 25, 31, 27, -36, -36, 31, 28, 112, -97, 32, -96, -77, 97, 33, -59, -92, 35, 34, -81, 65, -27, 34, 127, -120, -90, 35, 88, 119, 103, 36, 94, 12, 40, 37, -74, 69, -24, 37, -122, 33, -88, 38, -12, -99, 103, 39, 40, -71, 38, 40, 75, 113, -27, 40, -123, -60, -93, 41, 1, -79, 97, 42, -21, 52, 31, 43, 111, 78, -36, 43, -70, -5, -104, 44, -4, 58, 85, 45, 98, 10, 17, 46, 30, 104, -52, 46, 98, 82, -121, 47, 97, -57, 65, 48, 77, -59, -5, 48, 94, 74, -75, 49, -57, 84, 110, 50, -61, -30, 38, 51, -121, -14, -34, 51, 80, -126, -106, 52, 87, -112, 77, 53, -39, 26, 4, 54, 20, 32, -70, 54, 70, -98, 111, 55, -80, -109, 36, 56, -109, -2, -40, 56, 50, -35, -116, 57, -46, 45, 64, 58, -73, -18, -14, 58, 41, 30, -91, 59, 112, -70, 86, 60, -42, -63, 7, 61, -90, 50, -72, 61, 44, 11, 104, 62, -72, 73, 23, 63, -104, -20, -59, 63, 29, -14, 115, 64, -101, 88, 33, 65, 101, 30, -50, 65, -48, 65, 122, 66, 53, -63, 37, 67, -19, -102, -48, 67, 80, -51, 122, 68, -67, 86, 36, 69, -113, 53, -51, 69, 40, 104, 117, 70, -25, -20, 28, 71, 47, -62, -61, 71, 101, -26, 105, 72, -18, 87, 15, 73, 51, 21, -76, 73, -98, 28, 88, 74, -104, 108, -5, 74, -112, 3, -98, 75, -12, -33, 63, 76, 52, 0, -31, 76, -60, 98, -127, 77, 23, 6, 33, 78, -91, -24, -65, 78, -29, 8, 94, 79, 77, 101, -5, 79, 94, -4, -105, 80, -108, -52, 51, 81, 110, -44, -50, 81, 110, 18, 105, 82, 24, -123, 2, 83, -16, 42, -101, 83, 125, 2, 51, 84, 75, 10, -54, 84, -30, 64, 96, 85, -46, -92, -11, 85, -87, 52, -118, 86, -6, -18, 29, 87, 86, -46, -80, 87, 84, -35, 66, 88, -116, 14, -44, 88, -104, 100, 100, 89, 18, -34, -13, 89

section .text
EXTR_weird_multpl:
    movsx r8d,cx
    movsx eax,dx
    imul r8d,eax
    sar r8d,0xe
    mov eax,r8d
    mov cl,r8b
    sar eax,1
    and cx,0x1
    add ax,cx
    ret

section .data


section .text
EXTR_undo_weird_pprocess:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    movsx ebp,r8w
    mov rdi,rcx
    movzx esi,r9w
    mov ecx,ebp
    mov rbx,rdx
    call EXTR_log2ceil
    mov r8d,0x1
    mov r9w,0x1
    movzx ecx,al
    shl r8d,cl
    mov rcx,rbx
    movsxd r8,r8d
    lea rdx,[rbx + r8*0x2]
    movzx r8d,ax
    call EXTR_weird_img_shuffle
    test ebp,ebp
    movsx r8d,ax
    jle .lbl180017a36
    movzx ecx,si
    mov edx,ebp
    sub cx,r8w
    sub rdi,rbx
    .lbl180017a21: sar word ptr [rbx],cl
    movzx eax,word ptr [rbx]
    add rbx,0x2
    sub rdx,0x1
    mov word ptr [rdi + rbx*0x1 + -0x2],ax
    jnz .lbl180017a21
    .lbl180017a36: mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    movsx eax,si
    mov rsi,qword ptr [rsp + 0x40]
    sub eax,r8d
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_18000b6b0:
    imul edx,r8d
    mov r8,rcx
    test edx,edx
    jle .lbl18000b6ee
    mov r10d,0xffff8000
    .lbl18000b6c1: movzx eax,word ptr [rcx]
    add rcx,0x2
    sub eax,r9d
    cmp eax,0x7fff
    jle .lbl18000b6d9
    mov eax,0x7fff
    jmp .lbl18000b6e0
    .lbl18000b6d9: cmp eax,r10d
    cmovl eax,r10d
    .lbl18000b6e0: mov word ptr [r8],ax
    add r8,0x2
    sub rdx,0x1
    jnz .lbl18000b6c1
    .lbl18000b6ee: xor eax,eax
    ret

section .data


section .text
EXTR_FUN_18000b700:
    imul edx,r8d
    test edx,edx
    movsxd r8,edx
    jle .lbl18000b723
    sub r9,rcx
    nop
    .lbl18000b710: movzx eax,word ptr [r9 + rcx*0x1]
    add rcx,0x2
    sub word ptr [rcx + -0x2],ax
    sub r8,0x1
    jnz .lbl18000b710
    .lbl18000b723: xor eax,eax
    ret

section .data


section .text
EXTR_FUN_18000b730:
    sub rsp,0x8
    imul edx,r8d
    mov r11,rcx
    test edx,edx
    movsxd r10,edx
    jle .lbl18000b7ae
    mov r8,qword ptr [rsp + 0x38]
    mov qword ptr [rsp + 0x10],rbx
    mov ebx,dword ptr [rsp + 0x30]
    sub r9,r8
    mov qword ptr [rsp],rdi
    mov edi,0xffff8000
    sub r11,r8
    nop
    .lbl18000b760: movzx eax,word ptr [r9 + r8*0x1]
    movzx edx,word ptr [r11 + r8*0x1]
    sub edx,eax
    movzx eax,word ptr [r8]
    test ax,ax
    jz .lbl18000b782
    imul edx,ebx
    movzx ecx,ax
    mov eax,edx
    cdq
    idiv ecx
    mov edx,eax
    .lbl18000b782: cmp edx,0x7fff
    jle .lbl18000b791
    mov edx,0x7fff
    jmp .lbl18000b796
    .lbl18000b791: cmp edx,edi
    cmovl edx,edi
    .lbl18000b796: mov word ptr [r11 + r8*0x1],dx
    add r8,0x2
    sub r10,0x1
    jnz .lbl18000b760
    mov rdi,qword ptr [rsp]
    mov rbx,qword ptr [rsp + 0x10]
    .lbl18000b7ae: xor eax,eax
    add rsp,0x8
    ret

section .data


section .text
EXTR_FUN_18000dac0:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0xc0
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0xb0],rax
    mov r15,qword ptr [rsp + 0x128]
    mov rbx,rcx
    mov eax,r8d
    mov ecx,edx
    sar eax,1
    mov dword ptr [rsp + 0x48],r9d
    mov r9,qword ptr [rsp + 0x120]
    sar ecx,1
    mov dword ptr [rsp + 0x44],eax
    imul eax,ecx
    movsxd r12,eax
    mov dword ptr [rsp + 0x80],eax
    lea rbp,[r15 + r12*0x4]
    mov esi,r8d
    mov edi,edx
    lea rax,[rbp + r12*0x4]
    mov dword ptr [rsp + 0x40],ecx
    mov qword ptr [rsp + 0x98],rbp
    xor ebp,ebp
    lea r13,[r12 + rax*0x1]
    xor edx,edx
    lea r8d,[rbp + 0x24]
    mov rcx,r9
    mov qword ptr [rsp + 0x90],r9
    lea r14,[r12 + r13*0x1]
    mov qword ptr [rsp + 0x88],r15
    mov qword ptr [rsp + 0x70],rax
    mov word ptr [r11 + -0x58],0xca7
    mov word ptr [r11 + -0x56],0x38b6
    mov word ptr [r11 + -0x54],0x5d81
    mov word ptr [r11 + -0x52],0x38b6
    mov word ptr [r11 + -0x50],0xca7
    mov dword ptr [rsp + 0x4c],ebp
    mov dword ptr [rsp + 0x5c],ebp
    mov dword ptr [rsp + 0x60],ebp
    mov dword ptr [rsp + 0x58],ebp
    mov dword ptr [rsp + 0x50],ebp
    mov dword ptr [rsp + 0x64],ebp
    mov dword ptr [rsp + 0x54],ebp
    call EXTR_memset wrt ..plt
    lea r9d,[rbp + 0x1]
    mov r8d,esi
    mov edx,edi
    mov rcx,rbx
    mov qword ptr [rsp + 0x20],r15
    call EXTR_downscale
    mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    lea rsi,[r15 + r12*0x4]
    mov rcx,r15
    mov r9,rsi
    call EXTR_average_img
    test r12,r12
    mov r11,rbp
    jle .lbl18000dc73
    mov r8d,0xffff8000
    mov r14d,ebp
    nop
    nop
    nop
    .lbl18000dbf0: mov eax,dword ptr [rsi + r11*0x4]
    add r14d,eax
    sar eax,0x2
    cmp eax,0x7fff
    jle .lbl18000dc08
    mov edx,0x7fff
    jmp .lbl18000dc11
    .lbl18000dc08: mov edx,eax
    cmp eax,r8d
    cmovl edx,r8d
    .lbl18000dc11: cmp eax,0x7fff
    jle .lbl18000dc1f
    mov ecx,0x7fff
    jmp .lbl18000dc28
    .lbl18000dc1f: mov ecx,eax
    cmp eax,r8d
    cmovl ecx,r8d
    .lbl18000dc28: neg ecx
    cmp edx,ecx
    jle .lbl18000dc45
    cmp eax,0x7fff
    jle .lbl18000dc3c
    mov eax,0x7fff
    jmp .lbl18000dc5c
    .lbl18000dc3c: cmp eax,r8d
    cmovl eax,r8d
    jmp .lbl18000dc5c
    .lbl18000dc45: cmp eax,0x7fff
    jle .lbl18000dc53
    mov eax,0x7fff
    jmp .lbl18000dc5a
    .lbl18000dc53: cmp eax,r8d
    cmovl eax,r8d
    .lbl18000dc5a: neg eax
    .lbl18000dc5c: mov word ptr [r15 + r11*0x2],ax
    add r11,0x1
    cmp r11,r12
    jl .lbl18000dbf0
    mov dword ptr [rsp + 0x5c],r14d
    lea r14,[r12 + r13*0x1]
    .lbl18000dc73: mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    lea rdi,[r15 + r12*0x2]
    mov qword ptr [rsp + 0x30],rdi
    lea r9,[rsp + 0xa0]
    mov ebx,0x1
    mov rcx,r15
    mov dword ptr [rsp + 0x28],0x5
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180002820
    mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov qword ptr [rsp + 0x30],r15
    lea r9,[rsp + 0xa0]
    mov rcx,rdi
    mov dword ptr [rsp + 0x28],ebx
    mov dword ptr [rsp + 0x20],0x5
    call EXTR_FUN_180002820
    test r12,r12
    mov r11,rbp
    jle .lbl18000dcf1
    nop
    nop
    .lbl18000dce0: movsx eax,word ptr [r15 + r11*0x2]
    mov dword ptr [rsi + r11*0x4],eax
    add r11,rbx
    cmp r11,r12
    jl .lbl18000dce0
    .lbl18000dcf1: test r12,r12
    mov r8,rbp
    jle .lbl18000dd33
    mov eax,dword ptr [rsp + 0x48]
    lea ecx,[rax + rax*0x8]
    mov eax,0x66666667
    add ecx,ecx
    imul ecx
    sar edx,0x2
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    nop
    nop
    nop
    nop
    .lbl18000dd20: cmp dword ptr [rsi + r8*0x4],edx
    setg al
    mov byte ptr [r8 + r13*0x1],al
    add r8,rbx
    cmp r8,r12
    jl .lbl18000dd20
    .lbl18000dd33: mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov r9d,ebx
    mov rcx,r13
    call EXTR_FUN_180010450
    mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov r9,r15
    mov rcx,r13
    mov byte ptr [rsp + 0x28],0x30
    mov qword ptr [rsp + 0x20],rdi
    call EXTR_FUN_180012d30
    mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov r9d,ebx
    mov rcx,r15
    movsxd rdi,eax
    call EXTR_FUN_180010770
    mov r8d,dword ptr [rsp + 0x44]
    mov edx,dword ptr [rsp + 0x40]
    mov r9d,ebx
    mov rcx,r13
    call EXTR_FUN_180010550
    test edi,edi
    jnz .lbl18000dd9b
    xor eax,eax
    jmp .lbl18000e265
    .lbl18000dd9b: mov r9d,dword ptr [rsp + 0x44]
    mov r8d,dword ptr [rsp + 0x40]
    mov rdx,r13
    mov rcx,rsi
    mov qword ptr [rsp + 0x20],r14
    call EXTR_FUN_180005750
    mov eax,dword ptr [rsp + 0x80]
    cdq
    sub eax,edx
    sar eax,1
    test eax,eax
    mov esi,eax
    mov dword ptr [rsp + 0x48],esi
    jle .lbl18000dddc
    mov r8,rsi
    xor edx,edx
    mov rcx,r14
    shl r8,0x2
    call EXTR_memset wrt ..plt
    .lbl18000dddc: test r12,r12
    mov rcx,rbp
    jle .lbl18000ddfd
    .lbl18000dde4: cmp word ptr [r15 + rcx*0x2],bp
    jle .lbl18000ddf5
    movsx rax,word ptr [r15 + rcx*0x2]
    add dword ptr [r14 + rax*0x4 + -0x4],ebx
    .lbl18000ddf5: add rcx,rbx
    cmp rcx,r12
    jl .lbl18000dde4
    .lbl18000ddfd: test edi,edi
    mov rax,rbp
    jle .lbl18000de24
    mov esi,eax
    .lbl18000de06: mov ecx,dword ptr [r14 + rax*0x4]
    cmp ecx,esi
    jle .lbl18000de14
    mov esi,ecx
    mov dword ptr [rsp + 0x4c],ebx
    .lbl18000de14: add rax,0x1
    add ebx,0x1
    cmp rax,rdi
    jl .lbl18000de06
    mov esi,dword ptr [rsp + 0x48]
    .lbl18000de24: test r12,r12
    mov rcx,rbp
    jle .lbl18000de48
    mov edx,dword ptr [rsp + 0x4c]
    .lbl18000de30: movsx eax,word ptr [r15 + rcx*0x2]
    cmp eax,edx
    setz al
    add rcx,0x1
    cmp rcx,r12
    mov byte ptr [rcx + r13*0x1 + -0x1],al
    jl .lbl18000de30
    .lbl18000de48: mov r9,qword ptr [rsp + 0x70]
    mov r8d,dword ptr [rsp + 0x40]
    mov edx,dword ptr [rsp + 0x44]
    mov rcx,r13
    call EXTR_FUN_180015da0
    test r12,r12
    mov dword ptr [rsp + 0x4c],ebp
    mov rbx,rbp
    lea rdi,[r15 + r12*0x4]
    jle .lbl18000deb8
    mov r14d,ebx
    mov esi,ebx
    nop
    nop
    nop
    nop
    .lbl18000de80: mov ecx,dword ptr [rdi + rbx*0x4]
    mov eax,ebp
    cmp byte ptr [rbx + r13*0x1],al
    setnz al
    add r14d,eax
    call EXTR_FUN_180002420
    movzx ecx,byte ptr [rbx + r13*0x1]
    add rbx,0x1
    imul eax,ecx
    add esi,eax
    cmp rbx,r12
    jl .lbl18000de80
    mov dword ptr [rsp + 0x60],esi
    mov esi,dword ptr [rsp + 0x48]
    mov dword ptr [rsp + 0x4c],r14d
    lea r14,[r12 + r13*0x1]
    .lbl18000deb8: test esi,esi
    jle .lbl18000decd
    mov r8d,esi
    xor edx,edx
    mov rcx,r14
    shl r8,0x2
    call EXTR_memset wrt ..plt
    .lbl18000decd: test r12,r12
    mov rcx,rbp
    jle .lbl18000def0
    .lbl18000ded5: cmp word ptr [r15 + rcx*0x2],bp
    jle .lbl18000dee7
    movsx rax,word ptr [r15 + rcx*0x2]
    add dword ptr [r14 + rax*0x4 + -0x4],0x1
    .lbl18000dee7: add rcx,0x1
    cmp rcx,r12
    jl .lbl18000ded5
    .lbl18000def0: test r12,r12
    mov r8,rbp
    jle .lbl18000df24
    nop
    nop
    .lbl18000df00: movsx rcx,word ptr [r15 + r8*0x2]
    cmp dword ptr [r14 + rcx*0x4 + -0x4],0x25
    setg dl
    test cx,cx
    setg al
    add r8,0x1
    and dl,al
    cmp r8,r12
    mov byte ptr [r8 + r13*0x1 + -0x1],dl
    jl .lbl18000df00
    .lbl18000df24: movsxd rsi,dword ptr [rsp + 0x44]
    mov r8d,dword ptr [rsp + 0x40]
    lea r9,[rdi + r12*0x4]
    mov edx,esi
    mov rcx,r13
    call EXTR_FUN_180015da0
    test r12,r12
    mov r14d,ebp
    mov dword ptr [rsp + 0x48],ebp
    mov rbx,rbp
    jle .lbl18000df8a
    mov r15d,ebx
    nop
    .lbl18000df50: mov ecx,dword ptr [rdi + rbx*0x4]
    mov eax,ebp
    cmp byte ptr [rbx + r13*0x1],al
    setnz al
    add r14d,eax
    call EXTR_FUN_180002420
    movzx ecx,byte ptr [rbx + r13*0x1]
    add rbx,0x1
    imul eax,ecx
    add r15d,eax
    cmp rbx,r12
    jl .lbl18000df50
    mov dword ptr [rsp + 0x58],r15d
    mov r15,qword ptr [rsp + 0x88]
    mov dword ptr [rsp + 0x48],r14d
    .lbl18000df8a: movsxd r12,dword ptr [rsp + 0x40]
    mov r9,rsi
    test r12d,r12d
    mov r11,r12
    mov qword ptr [rsp + 0x70],r12
    jle .lbl18000e04a
    mov rsi,rbp
    lea r10,[r13 + 0x1]
    mov r15d,esi
    nop
    .lbl18000dfb0: cmp r9,0x2
    mov r11d,ebp
    mov ebx,ebp
    mov r8d,ebp
    mov rdx,rbp
    jl .lbl18000e002
    lea rdi,[r9 + -0x1]
    nop
    nop
    nop
    .lbl18000dfd0: mov eax,ebp
    mov ecx,ebp
    cmp byte ptr [r10 + rdx*0x1 + -0x1],al
    setnz al
    imul eax,r8d
    cmp byte ptr [r10 + rdx*0x1],cl
    setnz cl
    lea r11d,[r11 + rax*0x2]
    lea eax,[r8 + 0x1]
    add rdx,0x2
    add r8d,0x2
    imul ecx,eax
    cmp rdx,rdi
    lea ebx,[rbx + rcx*0x2]
    jl .lbl18000dfd0
    .lbl18000e002: cmp rdx,r9
    jge .lbl18000e01c
    mov ecx,ebp
    lea rax,[rsi + rdx*0x1]
    cmp byte ptr [rax + r13*0x1],cl
    setnz cl
    imul ecx,r8d
    lea r15d,[r15 + rcx*0x2]
    .lbl18000e01c: lea ecx,[r11 + rbx*0x1]
    add rsi,r9
    add r10,r9
    add r15d,ecx
    sub r12,0x1
    jnz .lbl18000dfb0
    mov r11,qword ptr [rsp + 0x70]
    mov esi,dword ptr [rsp + 0x44]
    mov r12d,dword ptr [rsp + 0x40]
    mov dword ptr [rsp + 0x50],r15d
    mov r15,qword ptr [rsp + 0x88]
    .lbl18000e04a: test r14d,r14d
    jle .lbl18000e06a
    mov eax,dword ptr [rsp + 0x50]
    lea ecx,[rsi + rsi*0x1]
    cdq
    idiv ecx
    imul eax,eax,0xc8
    cdq
    idiv r14d
    sub eax,0x64
    mov dword ptr [rsp + 0x50],eax
    .lbl18000e06a: mov eax,r12d
    cdq
    sub eax,edx
    sar eax,1
    test eax,eax
    jle .lbl18000e16c
    lea r12,[r11 + -0x1]
    mov rcx,r9
    mov r14,rbp
    neg rcx
    lea r8,[r13 + 0x1]
    mov r15d,ebp
    mov qword ptr [rsp + 0x70],rcx
    mov ecx,eax
    imul r12,r9
    lea r10,[r12 + r13*0x1 + 0x1]
    mov qword ptr [rsp + 0x78],rcx
    .lbl18000e0a3: cmp r9,0x2
    mov r11d,ebp
    mov ebx,ebp
    mov edi,ebp
    mov esi,ebp
    mov rdx,rbp
    jl .lbl18000e0f8
    lea rcx,[r9 + -0x1]
    nop
    nop
    .lbl18000e0c0: mov eax,ebp
    cmp byte ptr [r8 + rdx*0x1 + -0x1],al
    setz al
    add edi,eax
    mov eax,ebp
    cmp byte ptr [r10 + rdx*0x1 + -0x1],al
    setz al
    add r11d,eax
    mov eax,ebp
    cmp byte ptr [r8 + rdx*0x1],al
    setz al
    add esi,eax
    mov eax,ebp
    cmp byte ptr [r10 + rdx*0x1],al
    setz al
    add rdx,0x2
    add ebx,eax
    cmp rdx,rcx
    jl .lbl18000e0c0
    .lbl18000e0f8: cmp rdx,r9
    jge .lbl18000e122
    lea rax,[rdx + r14*0x1]
    mov ecx,ebp
    cmp byte ptr [rax + r13*0x1],cl
    lea rax,[r12 + rdx*0x1]
    mov edx,dword ptr [rsp + 0x54]
    setz cl
    add r15d,ecx
    mov ecx,ebp
    cmp byte ptr [rax + r13*0x1],cl
    setz cl
    add edx,ecx
    jmp .lbl18000e126
    .lbl18000e122: mov edx,dword ptr [rsp + 0x54]
    .lbl18000e126: lea eax,[r11 + rbx*0x1]
    add r14,r9
    add r8,r9
    add edx,eax
    lea eax,[rdi + rsi*0x1]
    add r15d,eax
    mov rax,qword ptr [rsp + 0x70]
    mov dword ptr [rsp + 0x54],edx
    add r12,rax
    add r10,rax
    sub qword ptr [rsp + 0x78],0x1
    jnz .lbl18000e0a3
    mov r14d,dword ptr [rsp + 0x48]
    mov r12d,dword ptr [rsp + 0x40]
    mov dword ptr [rsp + 0x64],r15d
    mov r15,qword ptr [rsp + 0x88]
    jmp .lbl18000e16e
    .lbl18000e16c: mov edx,ebp
    .lbl18000e16e: mov eax,dword ptr [rsp + 0x64]
    mov edi,dword ptr [rsp + 0x44]
    cmp eax,edi
    jg .lbl18000e17e
    cmp edx,edi
    jle .lbl18000e19f
    .lbl18000e17e: cmp edx,eax
    jle .lbl18000e18f
    lea ecx,[rax + 0x1]
    mov eax,edx
    cdq
    idiv ecx
    mov r8d,eax
    jmp .lbl18000e1a2
    .lbl18000e18f: jge .lbl18000e19f
    lea ecx,[rdx + 0x1]
    cdq
    idiv ecx
    mov r8d,eax
    neg r8d
    jmp .lbl18000e1a2
    .lbl18000e19f: mov r8d,ebp
    .lbl18000e1a2: test r14d,r14d
    mov esi,dword ptr [rsp + 0x58]
    jle .lbl18000e1b9
    mov eax,esi
    imul eax,eax,0x1fe
    cdq
    idiv r14d
    jmp .lbl18000e1bb
    .lbl18000e1b9: mov eax,ebp
    .lbl18000e1bb: mov rcx,qword ptr [rsp + 0x90]
    mov dword ptr [rcx],eax
    lea ecx,[r14*0x4]
    mov r14,qword ptr [rsp + 0x90]
    mov dword ptr [r14 + 0x4],ecx
    mov ecx,dword ptr [rsp + 0x4c]
    test ecx,ecx
    jle .lbl18000e1f0
    mov eax,dword ptr [rsp + 0x60]
    imul eax,eax,0x1fe
    cdq
    idiv ecx
    mov ebp,eax
    .lbl18000e1f0: lea eax,[rcx*0x4]
    mov dword ptr [r14 + 0x8],ebp
    mov rbp,qword ptr [rsp + 0x98]
    mov dword ptr [r14 + 0xc],eax
    mov eax,dword ptr [rsp + 0x5c]
    mov dword ptr [r14 + 0x18],r8d
    mov dword ptr [r14 + 0x10],eax
    mov eax,dword ptr [rsp + 0x50]
    lea rbx,[r15 + 0x50]
    mov r9d,edi
    mov r8d,r12d
    mov rdx,r13
    mov rcx,rbp
    mov qword ptr [rsp + 0x28],rbx
    mov dword ptr [r14 + 0x14],eax
    mov dword ptr [rsp + 0x20],0x64
    call EXTR_FUN_180003b80
    mov r9d,edi
    mov r8d,r12d
    mov rdx,r13
    mov rcx,rbp
    mov qword ptr [rsp + 0x28],rbx
    mov dword ptr [r14 + 0x1c],eax
    mov dword ptr [rsp + 0x20],0x5a
    call EXTR_FUN_180003b80
    mov dword ptr [r14 + 0x20],eax
    mov eax,esi
    .lbl18000e265: mov rcx,qword ptr [rsp + 0xb0]
    xor rcx,rsp
    call EXTR_check_cookie
    mov rbx,qword ptr [rsp + 0x118]
    add rsp,0xc0
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
EXTR_FUN_180003b80:
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push r12
    push r14
    push r15
    sub rsp,0x28
    xor ebx,ebx
    imul r8d,r9d
    mov rsi,rdx
    movsxd rbp,r8d
    mov r12d,0x80000000
    mov r14d,0x7fffffff
    cmp rbp,0x2
    mov edx,ebx
    mov r11d,ebx
    mov r15d,ebx
    mov r10,rbx
    jl .lbl180003be1
    lea rcx,[rbp + -0x1]
    nop
    .lbl180003bc0: mov eax,ebx
    cmp byte ptr [rsi + r10*0x1],al
    setnz al
    add edx,eax
    mov eax,ebx
    cmp byte ptr [rsi + r10*0x1 + 0x1],al
    setnz al
    add r10,0x2
    add r11d,eax
    cmp r10,rcx
    jl .lbl180003bc0
    .lbl180003be1: cmp r10,rbp
    jge .lbl180003bee
    cmp byte ptr [r10 + rsi*0x1],bl
    setnz r15b
    .lbl180003bee: lea eax,[r11 + rdx*0x1]
    add r15d,eax
    mov dword ptr [rsp + 0x70],r15d
    jnz .lbl180003c0d
    or eax,0xffffffff
    add rsp,0x28
    pop r15
    pop r14
    pop r12
    pop rsi
    pop rbp
    pop rbx
    ret
    .lbl180003c0d: test rbp,rbp
    mov qword ptr [rsp + 0x20],r13
    mov r8,rbx
    jle .lbl180003c53
    mov r15,qword ptr [rsp + 0x60]
    nop
    .lbl180003c20: mov edx,dword ptr [r15 + r8*0x4]
    movzx ecx,byte ptr [r8 + rsi*0x1]
    mov eax,ebx
    cmp r12d,edx
    setl al
    test ecx,eax
    mov eax,ebx
    cmovnz r12d,edx
    cmp r14d,edx
    setg al
    test ecx,eax
    cmovnz r14d,edx
    add r8,0x1
    cmp r8,rbp
    jl .lbl180003c20
    mov r15d,dword ptr [rsp + 0x70]
    .lbl180003c53: mov r13d,r12d
    sub r13d,r14d
    jnz .lbl180003c63
    mov eax,r12d
    jmp .lbl180003d1b
    .lbl180003c63: mov qword ptr [rsp + 0x68],rdi
    mov rdi,qword ptr [rsp + 0x88]
    xor edx,edx
    mov rcx,rdi
    mov r8d,0x12c4
    call EXTR_memset wrt ..plt
    test rbp,rbp
    jle .lbl180003cb7
    mov r8,qword ptr [rsp + 0x60]
    nop
    nop
    .lbl180003c90: cmp byte ptr [rbx + rsi*0x1],0x0
    jz .lbl180003cae
    mov eax,dword ptr [r8 + rbx*0x4]
    sub eax,r14d
    imul eax,eax,0x4b0
    cdq
    idiv r13d
    movsxd rcx,eax
    add dword ptr [rdi + rcx*0x4],0x1
    .lbl180003cae: add rbx,0x1
    cmp rbx,rbp
    jl .lbl180003c90
    .lbl180003cb7: mov eax,dword ptr [rsp + 0x80]
    cmp eax,0x64
    jnz .lbl180003cc8
    mov eax,r12d
    jmp .lbl180003d16
    .lbl180003cc8: mov r9d,0x1
    mov r10d,r15d
    mov rdx,r9
    imul r10d,eax
    nop
    nop
    .lbl180003ce0: mov ecx,dword ptr [rdi + rdx*0x4 + -0x4]
    mov eax,dword ptr [rdi + rdx*0x4]
    lea r8d,[rcx + rax*0x1]
    mov dword ptr [rdi + rdx*0x4],r8d
    imul ecx,ecx,0x64
    cmp ecx,r10d
    jg .lbl180003d00
    imul r8d,r8d,0x64
    cmp r10d,r8d
    jl .lbl180003d2e
    .lbl180003d00: add rdx,0x1
    add r9d,0x1
    cmp rdx,0x4b0
    jle .lbl180003ce0
    mov eax,0xfffffffe
    .lbl180003d16: mov rdi,qword ptr [rsp + 0x68]
    .lbl180003d1b: mov r13,qword ptr [rsp + 0x20]
    add rsp,0x28
    pop r15
    pop r14
    pop r12
    pop rsi
    pop rbp
    pop rbx
    ret
    .lbl180003d2e: movsxd rax,r9d
    mov eax,dword ptr [rdi + rax*0x4 + -0x4]
    sub eax,dword ptr [rdi]
    imul eax,r13d
    cdq
    idiv r15d
    add eax,r14d
    jmp .lbl180003d16

section .data


section .text
EXTR_FUN_180005750:
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r14
    push r15
    sub rsp,0x20
    mov rdi,qword ptr [rsp + 0x80]
    movsxd r10,r9d
    mov r15d,r8d
    cmp r8d,r10d
    mov eax,r10d
    mov r14,rcx
    cmovg eax,r8d
    xor ebp,ebp
    test r9d,r9d
    cdqe
    movsxd rsi,r15d
    mov rbx,r10
    lea r12,[rdi + rax*0x4]
    lea r8,[r12 + rax*0x4]
    mov qword ptr [rsp + 0x80],r8
    jle .lbl18000584c
    mov r14,qword ptr [rsp + 0x68]
    mov qword ptr [rsp + 0x70],r13
    mov r9d,0x98967f
    mov r13,rcx
    nop
    .lbl1800057c0: xor ecx,ecx
    test r15d,r15d
    jle .lbl1800057e8
    lea rdx,[r14 + rbp*0x1]
    nop
    nop
    .lbl1800057d0: xor eax,eax
    cmp byte ptr [rdx],al
    cmovz eax,r9d
    add rcx,0x1
    add rdx,rbx
    cmp rcx,rsi
    mov dword ptr [rdi + rcx*0x4 + -0x4],eax
    jl .lbl1800057d0
    .lbl1800057e8: mov r9,r8
    mov rdx,rdi
    mov rcx,r12
    mov r8d,r15d
    call EXTR_FUN_180005570
    xor r11d,r11d
    test r15d,r15d
    jle .lbl180005822
    lea rdx,[rbx*0x4]
    mov rcx,r13
    nop
    .lbl180005810: mov eax,dword ptr [r12 + r11*0x4]
    add r11,0x1
    mov dword ptr [rcx],eax
    add rcx,rdx
    cmp r11,rsi
    jl .lbl180005810
    .lbl180005822: mov r8,qword ptr [rsp + 0x80]
    add rbp,0x1
    add r13,0x4
    cmp rbp,rbx
    mov r9d,0x98967f
    jl .lbl1800057c0
    mov r14,qword ptr [rsp + 0x60]
    mov r10d,dword ptr [rsp + 0x78]
    mov r13,qword ptr [rsp + 0x70]
    .lbl18000584c: test r15d,r15d
    jle .lbl1800058ad
    lea rbp,[rbx*0x4]
    mov rsi,r14
    mov r12,r15
    nop
    .lbl180005860: xor ecx,ecx
    test rbx,rbx
    jle .lbl180005883
    mov rdx,r14
    nop
    nop
    .lbl180005870: mov eax,dword ptr [rdx]
    add rcx,0x1
    add rdx,0x4
    cmp rcx,rbx
    mov dword ptr [rdi + rcx*0x4 + -0x4],eax
    jl .lbl180005870
    .lbl180005883: mov r9,r8
    mov rdx,rdi
    mov rcx,rsi
    mov r8d,r10d
    call EXTR_FUN_180005570
    mov r8,qword ptr [rsp + 0x80]
    mov r10d,dword ptr [rsp + 0x78]
    add rsi,rbp
    add r14,rbp
    sub r12,0x1
    jnz .lbl180005860
    .lbl1800058ad: add rsp,0x20
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
EXTR_FUN_180005570:
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push r12
    push r14
    sub rsp,0x48
    movsxd rax,r8d
    xor ebx,ebx
    mov qword ptr [rsp + 0x40],rsi
    lea r12,[r9 + rax*0x4]
    mov dword ptr [r9],ebx
    mov esi,0x1
    cmp eax,esi
    mov qword ptr [rsp + 0x38],rdi
    mov r14,r9
    mov dword ptr [r12],0xff676981
    mov dword ptr [r12 + 0x4],0x98967f
    mov r10,rdx
    mov rbp,rcx
    mov r11,rax
    mov qword ptr [rsp + 0x88],rax
    mov rdi,rbx
    jle .lbl180005673
    mov rbx,qword ptr [rsp + 0x78]
    mov qword ptr [rsp + 0x30],r13
    mov qword ptr [rsp + 0x28],r15
    lea r15,[rbx + 0x4]
    nop
    nop
    nop
    .lbl1800055f0: movsxd rcx,dword ptr [r14 + rdi*0x4]
    mov r9d,dword ptr [r15]
    lea r13,[r14 + rdi*0x4]
    mov r8d,dword ptr [rbx + rcx*0x4]
    mov edx,esi
    call EXTR_FUN_180005520
    cmp eax,dword ptr [r12 + rdi*0x4]
    jg .lbl180005630
    mov rbp,r12
    sub rbp,r14
    .lbl180005612: movsxd rcx,dword ptr [r13 + -0x4]
    sub r13,0x4
    mov edx,esi
    mov r8d,dword ptr [rbx + rcx*0x4]
    sub rdi,0x1
    call EXTR_FUN_180005520
    cmp eax,dword ptr [r13 + rbp*0x1]
    jle .lbl180005612
    .lbl180005630: add rdi,0x1
    add r15,0x4
    mov dword ptr [r14 + rdi*0x4],esi
    add esi,0x1
    mov dword ptr [r12 + rdi*0x4],eax
    cmp esi,dword ptr [rsp + 0x80]
    mov dword ptr [r12 + rdi*0x4 + 0x4],0x98967f
    jl .lbl1800055f0
    mov r10,qword ptr [rsp + 0x78]
    mov r11,qword ptr [rsp + 0x88]
    mov rbp,qword ptr [rsp + 0x70]
    mov r15,qword ptr [rsp + 0x28]
    mov r13,qword ptr [rsp + 0x30]
    xor ebx,ebx
    .lbl180005673: test r11,r11
    mov rdi,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov r8,rbx
    mov r9,rbx
    jle .lbl1800056cc
    nop
    nop
    .lbl180005690: cmp dword ptr [r12 + r8*0x4 + 0x4],ebx
    lea rax,[r12 + r8*0x4 + 0x4]
    jge .lbl1800056ac
    nop
    .lbl1800056a0: add rax,0x4
    add r8,0x1
    cmp dword ptr [rax],ebx
    jl .lbl1800056a0
    .lbl1800056ac: movsxd rax,dword ptr [r14 + r8*0x4]
    mov edx,ebx
    add r9,0x1
    sub edx,eax
    add ebx,0x1
    imul edx,edx
    add edx,dword ptr [r10 + rax*0x4]
    cmp r9,r11
    mov dword ptr [rbp + r9*0x4 + -0x4],edx
    jl .lbl180005690
    .lbl1800056cc: add rsp,0x48
    pop r14
    pop r12
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180005520:
    mov eax,edx
    mov r11d,edx
    mov r10d,ecx
    sub r11d,ecx
    imul eax,edx
    imul r10d,ecx
    sub eax,r10d
    sub eax,r8d
    add r11d,r11d
    add eax,r9d
    cdq
    idiv r11d
    test edx,edx
    jz .lbl180005560
    xor r8d,r8d
    test edx,edx
    mov ecx,r8d
    sets cl
    test r11d,r11d
    sets r8b
    cmp ecx,r8d
    jz .lbl180005560
    sub eax,0x1
    .lbl180005560: ret 0x0

section .data


section .text
EXTR_FUN_18000c000:
    sub rsp,0x28
    test byte ptr [rdx + 0xc],0x80
    mov r9,rdx
    jz .lbl18000c01f
    movzx r8d,word ptr [rdx + 0x12]
    mov r9,qword ptr [r9 + 0x60]
    movzx edx,word ptr [rdx + 0x14]
    call EXTR_FUN_180002110
    .lbl18000c01f: add rsp,0x28
    ret

section .data


section .text
EXTR_FUN_180002110:
    test edx,edx
    jle .lbl180002205
    mov qword ptr [rsp + 0x18],rbx
    mov qword ptr [rsp + 0x20],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x10
    mov qword ptr [rsp + 0x30],rbp
    movsxd rbx,r8d
    mov qword ptr [rsp + 0x38],r14
    mov r13,r9
    mov rsi,rcx
    xor edi,edi
    lea rbp,[rbx + rbx*0x1]
    movsxd r12,edx
    mov r14d,0xffff8000
    nop
    .lbl180002150: xor r11d,r11d
    xor r9d,r9d
    xor edx,edx
    test rbx,rbx
    jle .lbl1800021d8
    mov r10,rbx
    mov r8,rsi
    imul r10,rdi
    add r10,r13
    nop
    nop
    .lbl180002170: cmp byte ptr [r10 + rdx*0x1],0x0
    jz .lbl180002182
    movsx ecx,word ptr [r8]
    add r11d,ecx
    add r9d,0x1
    .lbl180002182: add rdx,0x1
    add r8,0x2
    cmp rdx,rbx
    jl .lbl180002170
    test r9d,r9d
    jle .lbl18000219e
    mov eax,r11d
    cdq
    idiv r9d
    mov r11d,eax
    .lbl18000219e: mov rcx,rsi
    mov rdx,rbx
    nop
    nop
    nop
    .lbl1800021b0: movsx eax,word ptr [rcx]
    sub eax,r11d
    cmp eax,0x7fff
    jle .lbl1800021c4
    mov eax,0x7fff
    jmp .lbl1800021cb
    .lbl1800021c4: cmp eax,r14d
    cmovl eax,r14d
    .lbl1800021cb: mov word ptr [rcx],ax
    add rcx,0x2
    sub rdx,0x1
    jnz .lbl1800021b0
    .lbl1800021d8: add rdi,0x1
    add rsi,rbp
    cmp rdi,r12
    jl .lbl180002150
    mov r14,qword ptr [rsp + 0x38]
    mov rbp,qword ptr [rsp + 0x30]
    mov rbx,qword ptr [rsp + 0x40]
    mov rsi,qword ptr [rsp + 0x48]
    add rsp,0x10
    pop r13
    pop r12
    pop rdi
    .lbl180002205: ret 0x0

section .data


section .text
EXTR_FUN_180006bd0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x50
    cmp word ptr [rcx + 0xb2],0x0
    mov rsi,r9
    mov rdi,r8
    mov rax,rdx
    mov rbx,rcx
    jnz .lbl180006c07
    xor eax,eax
    mov rbx,qword ptr [rsp + 0x60]
    mov rsi,qword ptr [rsp + 0x68]
    add rsp,0x50
    pop rdi
    ret
    .lbl180006c07: movzx r11d,word ptr [rax + 0x8]
    mov r10d,dword ptr [rax + 0xc]
    movzx r8d,word ptr [rcx + 0x88]
    movzx r9d,word ptr [rcx + 0x86]
    movzx edx,word ptr [rdx]
    lea rcx,[rax + 0x28]
    mov rax,qword ptr [rsp + 0x80]
    mov qword ptr [rsp + 0x48],rax
    mov rax,qword ptr [rdi + 0x18]
    mov qword ptr [rsp + 0x40],rax
    mov qword ptr [rsp + 0x38],rcx
    mov word ptr [rsp + 0x30],r10w
    mov word ptr [rsp + 0x28],r11w
    mov dword ptr [rsp + 0x20],edx
    mov rdx,qword ptr [rbx + 0x100]
    mov rcx,rsi
    call EXTR_FUN_1800065e0
    mov rbx,qword ptr [rsp + 0x60]
    mov rsi,qword ptr [rsp + 0x68]
    mov dword ptr [rdi + 0x14],eax
    add rsp,0x50
    pop rdi
    ret

section .data


section .text
EXTR_FUN_1800065e0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x70
    mov r10,qword ptr [rsp + 0xc0]
    mov rbp,qword ptr [rsp + 0xc8]
    movzx r11d,word ptr [rsp + 0xa8]
    lea rax,[rbp + 0x460]
    mov rdi,rcx
    movzx ecx,word ptr [rsp + 0xb0]
    mov qword ptr [rsp + 0x68],rax
    mov eax,dword ptr [r10 + 0xc]
    mov qword ptr [rsp + 0x60],rbp
    mov dword ptr [rsp + 0x58],eax
    mov eax,dword ptr [r10 + 0x8]
    lea rbx,[r10 + 0x10]
    mov dword ptr [rsp + 0x50],eax
    mov eax,dword ptr [r10 + 0x4]
    xor esi,esi
    mov dword ptr [rsp + 0x48],eax
    mov eax,dword ptr [r10]
    lea r12,[rbx + 0x1ea0]
    mov dword ptr [rsp + 0x40],eax
    mov rax,qword ptr [rsp + 0xb8]
    mov dword ptr [rsp + 0x38],0x20
    mov qword ptr [rsp + 0x30],rax
    mov dword ptr [rsp + 0x28],r11d
    mov dword ptr [rsp + 0x20],ecx
    mov rcx,rdi
    call EXTR_FUN_180006290
    lea edx,[rsi + 0x7]
    mov r9,rbx
    mov r8,rbp
    mov ecx,0x118
    call EXTR_FUN_1800058c0
    lea edx,[rsi + 0x7]
    mov r9,r12
    mov r8,rbp
    mov ecx,0x118
    mov ebx,eax
    call EXTR_FUN_1800058c0
    cmp dword ptr [rsp + 0xa0],esi
    jnz .lbl1800066f7
    test ebx,ebx
    jz .lbl1800066ed
    test eax,eax
    jz .lbl1800066e6
    lea r9,[r12 + 0x1ea0]
    lea edx,[rsi + 0x7]
    mov r8,rbp
    mov ecx,0x118
    call EXTR_FUN_1800058c0
    mov ecx,0x1000000
    mov edx,0x10000
    cmp eax,0x1
    cmovz ecx,edx
    mov eax,ecx
    jmp .lbl1800066f9
    .lbl1800066e6: mov eax,0x1000000
    jmp .lbl1800066f9
    .lbl1800066ed: mov edx,0x10000
    test eax,eax
    cmovnz esi,edx
    .lbl1800066f7: mov eax,esi
    .lbl1800066f9: lea r11,[rsp + 0x70]
    mov rbx,qword ptr [r11 + 0x10]
    mov rbp,qword ptr [r11 + 0x18]
    mov rsi,qword ptr [r11 + 0x20]
    mov rdi,qword ptr [r11 + 0x28]
    mov rsp,r11
    pop r12
    ret

section .data


section .text
EXTR_FUN_180006290:
    mov dword ptr [rsp + 0x20],r9d
    mov dword ptr [rsp + 0x18],r8d
    mov qword ptr [rsp + 0x10],rdx
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
    mov rsi,qword ptr [rsp + 0x108]
    mov r15d,r9d
    mov r14d,r8d
    mov r10d,r8d
    and r15d,0xfffffffc
    and r14d,0xfffffffc
    mov eax,r15d
    mov edx,r9d
    mov ecx,r8d
    imul r10d,r9d
    movsxd r12,r10d
    mov edi,r9d
    lea r13,[rsi + r12*0x2]
    imul eax,r14d
    movsxd rbx,eax
    lea rax,[rsi + rbx*0x2]
    mov qword ptr [rsp + 0x40],rax
    lea rax,[r12 + r13*0x1]
    mov qword ptr [rsp + 0x108],rax
    call EXTR_FUN_18000f490
    mov r8d,dword ptr [rsp + 0xb0]
    mov r9d,dword ptr [rsp + 0xb8]
    mov rdx,qword ptr [rsp + 0xa0]
    movsxd rbp,eax
    mov ecx,edi
    mov eax,r8d
    xor edi,edi
    sub ecx,r15d
    sub eax,r14d
    add rbp,rsi
    mov dword ptr [rsp + 0x38],eax
    mov dword ptr [rsp + 0x30],edi
    mov dword ptr [rsp + 0x28],ecx
    mov rcx,rsi
    mov dword ptr [rsp + 0x20],edi
    call EXTR_FUN_180003fb0
    test rbx,rbx
    mov r11,rdi
    jle .lbl18000635f
    .lbl180006350: shl word ptr [rsi + r11*0x2],0x2
    add r11,0x1
    cmp r11,rbx
    jl .lbl180006350
    .lbl18000635f: lea rax,[rsi + rbx*0x2]
    mov r9d,0x16
    mov r8d,r15d
    mov qword ptr [rsp + 0x28],rax
    mov edx,r14d
    mov rcx,rsi
    mov dword ptr [rsp + 0x20],0xb
    call EXTR_FUN_180005e90
    test rbx,rbx
    mov r11,rdi
    jle .lbl1800063ad
    nop
    .lbl180006390: movzx eax,word ptr [rsi + r11*0x2]
    test ax,ax
    movsx ecx,ax
    jg .lbl18000639f
    mov ecx,edi
    .lbl18000639f: mov word ptr [rsi + r11*0x2],cx
    add r11,0x1
    cmp r11,rbx
    jl .lbl180006390
    .lbl1800063ad: mov rax,qword ptr [rsp + 0x40]
    mov rbx,qword ptr [rsp + 0x100]
    mov r9d,dword ptr [rsp + 0xe0]
    mov qword ptr [rsp + 0x38],rax
    mov eax,dword ptr [rsp + 0xf0]
    mov qword ptr [rsp + 0x30],rbx
    mov dword ptr [rsp + 0x28],eax
    mov eax,dword ptr [rsp + 0xe8]
    mov r8d,r15d
    mov edx,r14d
    mov rcx,rsi
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_18000e310
    mov eax,dword ptr [rsp + 0xc8]
    mov r11d,dword ptr [rsp + 0xc0]
    mov r14d,dword ptr [rsp + 0xb8]
    mov r15d,dword ptr [rsp + 0xb0]
    mov rcx,qword ptr [rsp + 0xa0]
    mov dword ptr [rbx + 0x38],eax
    lea rax,[r12 + r13*0x1]
    mov r9,rsi
    mov r8d,r14d
    mov edx,r15d
    mov dword ptr [rbx + 0x34],r11d
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000e4a0
    test r12,r12
    mov r11,rdi
    jle .lbl180006468
    mov ecx,dword ptr [rsp + 0xf8]
    nop
    nop
    nop
    .lbl180006450: movzx eax,word ptr [rsi + r11*0x2]
    cmp eax,ecx
    setg al
    add r11,0x1
    cmp r11,r12
    mov byte ptr [r11 + r13*0x1 + -0x1],al
    jl .lbl180006450
    .lbl180006468: mov rax,qword ptr [rsp + 0x108]
    lea r9,[rbx + 0x3c]
    mov r8d,r14d
    mov edx,r15d
    mov rcx,r13
    mov r12,rbx
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000e820
    mov r11d,dword ptr [rbx + 0x3c]
    mov eax,dword ptr [rbx + 0x40]
    mov edx,dword ptr [rsp + 0xd8]
    mov rcx,qword ptr [rsp + 0xd0]
    mov dword ptr [rbx + 0x3c],eax
    mov dword ptr [rbx + 0x40],r11d
    call EXTR_FUN_18000e920
    mov r8d,r14d
    mov edx,r15d
    mov rcx,rsi
    mov dword ptr [rbx + 0x44],eax
    call EXTR_FUN_18000e970
    mov r9,qword ptr [rsp + 0x108]
    movzx r11d,ax
    mov r8d,r14d
    mov edx,r15d
    mov rcx,r13
    mov dword ptr [rbx + 0x48],r11d
    call EXTR_FUN_18000ea90
    mov r13,qword ptr [rsp + 0xa8]
    movzx r8d,r14w
    mov dword ptr [rbx + 0x4c],eax
    mov rbx,qword ptr [rsp + 0xa0]
    mov r9,r13
    mov rcx,rbx
    movzx edx,r15w
    call EXTR_FUN_18000ec90
    mov r9,r13
    movzx r8d,r14w
    movzx edx,r15w
    mov rcx,rbx
    mov qword ptr [rsp + 0x20],rsi
    mov dword ptr [r12 + 0x50],eax
    call EXTR_FUN_18000ee80
    mov r9,r13
    movzx r8d,r14w
    movzx edx,r15w
    mov rcx,rbx
    mov dword ptr [r12 + 0x54],eax
    call EXTR_FUN_18000f1a0
    mov r9,r13
    movzx r8d,r14w
    movzx edx,r15w
    mov rcx,rbx
    mov dword ptr [r12 + 0x58],eax
    mov dword ptr [rsp + 0x20],eax
    call EXTR_FUN_18000f270
    mov r9,rbp
    mov r8d,r14d
    mov edx,r15d
    mov rcx,rbx
    mov qword ptr [rsp + 0x28],rsi
    mov dword ptr [r12 + 0x5c],eax
    mov dword ptr [rsp + 0x20],edi
    call EXTR_FUN_18000f4a0
    lea rcx,[r12 + 0x64]
    nop
    nop
    nop
    .lbl180006580: movzx eax,word ptr [rbp + rdi*0x2]
    add rdi,0x2
    add rcx,0x8
    cmp rdi,0x100
    mov dword ptr [rcx + -0xc],eax
    movzx eax,word ptr [rbp + rdi*0x2 + -0x2]
    mov dword ptr [rcx + -0x8],eax
    jl .lbl180006580
    xor eax,eax
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
EXTR_FUN_180005e90:
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x68
    movsxd rsi,r8d
    mov r15,rcx
    movsxd rcx,dword ptr [rsp + 0xd0]
    lea eax,[rcx + rcx*0x1]
    mov r14,rcx
    mov r12,rsi
    lea r8d,[rax + rdx*0x1]
    lea r9d,[rax + rsi*0x1]
    movsxd rdx,edx
    mov eax,r9d
    mov dword ptr [rsp + 0xb8],r8d
    mov dword ptr [rsp + 0xc0],r9d
    mov qword ptr [rsp + 0x38],rdx
    imul eax,r8d
    test rdx,rdx
    mov r8,qword ptr [rsp + 0xd8]
    movsxd r10,eax
    lea rax,[r8 + r10*0x4]
    movsxd r10,r9d
    mov qword ptr [rsp + 0x50],rax
    mov qword ptr [rsp + 0x30],r10
    jle .lbl180005f72
    mov rax,rcx
    lea rdi,[r10*0x4]
    lea rbx,[rsi + rsi*0x1]
    mov r9,r15
    mov r11,rdx
    imul rax,r10
    add rax,rcx
    lea r10,[r8 + rax*0x4]
    nop
    nop
    .lbl180005f30: test esi,esi
    jle .lbl180005f54
    mov rcx,r10
    mov rdx,r9
    mov r8,rsi
    nop
    .lbl180005f40: movsx eax,word ptr [rdx]
    add rcx,0x4
    add rdx,0x2
    sub r8,0x1
    mov dword ptr [rcx + -0x4],eax
    jnz .lbl180005f40
    .lbl180005f54: add r10,rdi
    add r9,rbx
    sub r11,0x1
    jnz .lbl180005f30
    mov rdx,qword ptr [rsp + 0x38]
    mov r8,qword ptr [rsp + 0xd8]
    mov r10,qword ptr [rsp + 0x30]
    .lbl180005f72: xor ebp,ebp
    test r14,r14
    mov r11,rbp
    jle .lbl18000606a
    mov ebp,dword ptr [rsp + 0xd0]
    lea rbx,[rdx + rdx*0x1 + -0x2]
    mov rax,r10
    neg rax
    lea r13d,[rsi + rbp*0x1]
    lea rdx,[r12 + r12*0x1]
    imul rbx,r12
    shl rax,0x2
    mov qword ptr [rsp + 0x40],rax
    mov rax,rsi
    neg rax
    add rax,rax
    mov qword ptr [rsp + 0x48],rax
    mov rax,r14
    sub rax,r14
    lea rcx,[rax + rax*0x1]
    lea rax,[r14 + -0x1]
    add rbx,rcx
    lea rdi,[rcx + r15*0x1]
    mov rcx,qword ptr [rsp + 0x48]
    imul rax,r10
    add rax,r14
    add rbx,r15
    mov r15,qword ptr [rsp + 0x38]
    lea rsi,[r8 + rax*0x4]
    mov rax,qword ptr [rsp + 0x40]
    .lbl180005fe7: cmp ebp,r13d
    jge .lbl18000604e
    lea rax,[r15 + r14*0x1]
    mov rdx,rbx
    mov r9,rdi
    add rax,r11
    imul rax,r10
    mov r10d,r13d
    add rax,r14
    sub r10d,ebp
    lea rcx,[r8 + rax*0x4]
    mov r8,rsi
    nop
    .lbl180006010: movsx eax,word ptr [r9]
    add rcx,0x4
    add r9,0x2
    mov dword ptr [r8],eax
    movsx eax,word ptr [rdx]
    add rdx,0x2
    mov dword ptr [rcx + -0x4],eax
    add r8,0x4
    sub r10,0x1
    jnz .lbl180006010
    mov rax,qword ptr [rsp + 0x40]
    mov rcx,qword ptr [rsp + 0x48]
    mov r8,qword ptr [rsp + 0xd8]
    mov r10,qword ptr [rsp + 0x30]
    lea rdx,[r12 + r12*0x1]
    .lbl18000604e: add r11,0x1
    add rbx,rcx
    add rsi,rax
    add rdi,rdx
    cmp r11,r14
    jl .lbl180005fe7
    mov r15,qword ptr [rsp + 0xb0]
    xor ebp,ebp
    .lbl18000606a: test r14,r14
    movsxd rax,dword ptr [rsp + 0xb8]
    mov r13,rax
    jle .lbl1800060ff
    lea rbx,[r8 + r14*0x4]
    lea r15,[r12 + r14*0x1]
    lea rdi,[rbx + -0x4]
    lea rsi,[r8 + r15*0x4 + -0x4]
    nop
    .lbl180006090: test r13,r13
    jle .lbl1800060d6
    lea rax,[r15 + rbp*0x1]
    lea rcx,[r10*0x4]
    mov rdx,rbx
    mov r9,rdi
    mov r10,rsi
    mov r11,r13
    lea r8,[r8 + rax*0x4]
    .lbl1800060b1: mov eax,dword ptr [rdx]
    add rdx,rcx
    mov dword ptr [r9],eax
    mov eax,dword ptr [r10]
    add r9,rcx
    mov dword ptr [r8],eax
    add r8,rcx
    add r10,rcx
    sub r11,0x1
    jnz .lbl1800060b1
    mov r8,qword ptr [rsp + 0xd8]
    .lbl1800060d6: mov r10,qword ptr [rsp + 0x30]
    add rbp,0x1
    add rbx,0x4
    sub rdi,0x4
    sub rsi,0x4
    cmp rbp,r14
    jl .lbl180006090
    mov r15,qword ptr [rsp + 0xb0]
    mov eax,dword ptr [rsp + 0xb8]
    .lbl1800060ff: mov rcx,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0xd8]
    mov r9d,dword ptr [rsp + 0xc8]
    mov r8d,dword ptr [rsp + 0xc0]
    mov qword ptr [rsp + 0x20],rcx
    mov edx,eax
    mov rcx,rdi
    call EXTR_FUN_180005970
    mov rbx,qword ptr [rsp + 0x38]
    test rbx,rbx
    jle .lbl180006191
    mov rcx,qword ptr [rsp + 0x30]
    mov rax,r14
    lea r10,[r12 + r12*0x1]
    lea r11,[rcx*0x4]
    imul rax,rcx
    add rax,r14
    lea r9,[rdi + rax*0x4]
    .lbl180006154: test r12,r12
    jle .lbl180006185
    mov rcx,r15
    mov rdx,r9
    mov r8,r12
    nop
    nop
    nop
    nop
    .lbl180006170: movzx eax,word ptr [rdx]
    add rcx,0x2
    add rdx,0x4
    sub r8,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl180006170
    .lbl180006185: add r9,r11
    add r15,r10
    sub rbx,0x1
    jnz .lbl180006154
    .lbl180006191: add rsp,0x68
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
EXTR_FUN_180005970:
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0xa8
    mov rcx,qword ptr [rsp + 0x110]
    mov r14d,edx
    mov eax,r8d
    cdq
    mov r13d,r8d
    idiv r9d
    movsxd rsi,eax
    mov eax,r14d
    cdq
    mov dword ptr [rsp + 0x108],esi
    idiv r9d
    mov r12d,eax
    mov r9d,eax
    mov rax,rsi
    mov dword ptr [rsp + 0x3c],r12d
    mov qword ptr [rsp + 0x88],rax
    imul r9d,esi
    test r12d,r12d
    movsxd r10,r9d
    lea rbx,[rcx + r10*0x4]
    jle .lbl180005b46
    xor r8d,r8d
    shl rax,0x2
    mov r9,rcx
    mov qword ptr [rsp + 0x40],rax
    mov rax,r12
    mov qword ptr [rsp + 0x50],rcx
    mov rcx,qword ptr [rsp + 0x40]
    mov dword ptr [rsp + 0x20],r8d
    mov qword ptr [rsp + 0x48],rax
    .lbl180005a07: test esi,esi
    jle .lbl180005b1f
    mov eax,r8d
    mov r13,r9
    cdq
    idiv r12d
    movsxd rcx,eax
    lea eax,[r8 + r14*0x1]
    mov r14d,esi
    cdq
    mov rbp,rcx
    idiv r12d
    mov r15d,eax
    sub r15d,ecx
    xor r12d,r12d
    mov dword ptr [rsp + 0x28],r15d
    movsxd rdi,r15d
    nop
    nop
    .lbl180005a40: mov eax,r12d
    xor r9d,r9d
    xor r8d,r8d
    cdq
    idiv esi
    movsxd rcx,eax
    mov eax,dword ptr [rsp + 0x100]
    add r12d,eax
    mov eax,r12d
    mov dword ptr [rsp + 0x24],r12d
    cdq
    idiv esi
    mov rsi,rcx
    mov r11d,eax
    sub r11d,ecx
    test rdi,rdi
    movsxd r10,r11d
    jle .lbl180005ac8
    movsxd r15,dword ptr [rsp + 0x100]
    mov r12,qword ptr [rsp + 0xf0]
    .lbl180005a85: test r11d,r11d
    jle .lbl180005ab5
    lea rcx,[r9 + rbp*0x1]
    imul rcx,r15
    add rcx,rsi
    lea rdx,[r12 + rcx*0x4]
    mov rcx,r10
    nop
    .lbl180005aa0: mov eax,dword ptr [rdx]
    add r8,0x1
    add rdx,0x4
    sub rcx,0x1
    mov dword ptr [rbx + r8*0x4 + -0x4],eax
    jnz .lbl180005aa0
    .lbl180005ab5: add r9,0x1
    cmp r9,rdi
    jl .lbl180005a85
    mov r12d,dword ptr [rsp + 0x24]
    mov r15d,dword ptr [rsp + 0x28]
    .lbl180005ac8: imul r11d,r15d
    mov rcx,rbx
    mov eax,r11d
    cdq
    lea r8d,[r11 + -0x1]
    sub eax,edx
    xor edx,edx
    sar eax,1
    mov r9d,eax
    call EXTR_FUN_180002690
    mov esi,dword ptr [rsp + 0x108]
    add r13,0x4
    sub r14,0x1
    mov dword ptr [r13 + -0x4],eax
    jnz .lbl180005a40
    mov r8d,dword ptr [rsp + 0x20]
    mov r9,qword ptr [rsp + 0x50]
    mov rax,qword ptr [rsp + 0x48]
    mov rcx,qword ptr [rsp + 0x40]
    mov r12d,dword ptr [rsp + 0x3c]
    mov r14d,dword ptr [rsp + 0xf8]
    .lbl180005b1f: add r8d,r14d
    add r9,rcx
    sub rax,0x1
    mov qword ptr [rsp + 0x48],rax
    mov dword ptr [rsp + 0x20],r8d
    mov qword ptr [rsp + 0x50],r9
    jnz .lbl180005a07
    mov r13d,dword ptr [rsp + 0x100]
    .lbl180005b46: xor r11d,r11d
    xor ebx,ebx
    test r12d,r12d
    mov dword ptr [rsp + 0x20],r11d
    jle .lbl180005e2e
    nop
    nop
    .lbl180005b60: xor ebp,ebp
    xor edi,edi
    test esi,esi
    mov dword ptr [rsp + 0x2c],ebp
    jle .lbl180005e18
    mov eax,r11d
    imul eax,r14d
    cdq
    idiv r12d
    movsxd rcx,eax
    lea eax,[r11 + 0x1]
    mov r9,rcx
    mov qword ptr [rsp + 0x90],rcx
    imul eax,r14d
    cdq
    idiv r12d
    mov r10d,eax
    sub r10d,ecx
    mov dword ptr [rsp + 0x24],r10d
    movsxd r8,r10d
    .lbl180005ba3: mov eax,ebp
    xor r14d,r14d
    imul eax,r13d
    cdq
    idiv esi
    movsxd rcx,eax
    lea eax,[rbp + 0x1]
    mov dword ptr [rsp + 0x50],eax
    mov qword ptr [rsp + 0x70],rcx
    imul eax,r13d
    cdq
    idiv esi
    mov esi,eax
    xor eax,eax
    sub esi,ecx
    test r8,r8
    mov qword ptr [rsp + 0x68],rax
    movsxd rcx,esi
    mov dword ptr [rsp + 0x48],esi
    mov qword ptr [rsp + 0x80],rcx
    jle .lbl180005df5
    nop
    nop
    .lbl180005bf0: xor r8d,r8d
    test rcx,rcx
    mov dword ptr [rsp + 0x28],r8d
    jle .lbl180005ddc
    add rax,r9
    mov qword ptr [rsp + 0x60],rcx
    mov qword ptr [rsp + 0x78],rax
    mov eax,r10d
    cdq
    sub eax,edx
    sar eax,1
    mov r15d,eax
    mov eax,esi
    cdq
    sub eax,edx
    sar eax,1
    mov r9d,eax
    mov dword ptr [rsp + 0x58],eax
    mov eax,esi
    imul eax,r10d
    mov dword ptr [rsp + 0x5c],eax
    mov eax,esi
    cdq
    sub eax,edx
    sar eax,1
    neg eax
    mov dword ptr [rsp + 0x30],eax
    mov rax,qword ptr [rsp + 0x70]
    mov qword ptr [rsp + 0x40],rax
    lea eax,[r9 + rsi*0x1]
    mov dword ptr [rsp + 0x34],eax
    mov eax,esi
    sub eax,r9d
    mov dword ptr [rsp + 0x38],eax
    nop
    nop
    .lbl180005c60: cmp r14d,r15d
    jge .lbl180005c80
    test r11d,r11d
    mov r12,rbx
    jz .lbl180005c77
    lea r13,[rbx + -0x1]
    lea edx,[r15 + r14*0x1]
    jmp .lbl180005c9e
    .lbl180005c77: xor r13d,r13d
    lea edx,[r15 + r14*0x1]
    jmp .lbl180005c9e
    .lbl180005c80: lea eax,[r12 + -0x1]
    mov r13,rbx
    mov r12,rbx
    cmp r11d,eax
    jge .lbl180005c94
    lea r12,[rbx + 0x1]
    .lbl180005c94: mov eax,dword ptr [rsp + 0x38]
    mov edx,r14d
    sub edx,r15d
    .lbl180005c9e: mov r11d,r10d
    mov rcx,rdi
    sub r11d,edx
    cmp r8d,r9d
    jge .lbl180005cc4
    test ebp,ebp
    jz .lbl180005cbb
    lea rsi,[rdi + -0x1]
    add r8d,r9d
    mov ebp,eax
    jmp .lbl180005ce2
    .lbl180005cbb: xor esi,esi
    mov ebp,eax
    add r8d,r9d
    jmp .lbl180005ce2
    .lbl180005cc4: mov eax,dword ptr [rsp + 0x108]
    mov rsi,rdi
    add eax,-0x1
    cmp ebp,eax
    jge .lbl180005cd9
    lea rcx,[rdi + 0x1]
    .lbl180005cd9: mov r8d,dword ptr [rsp + 0x30]
    mov ebp,dword ptr [rsp + 0x34]
    .lbl180005ce2: mov rax,qword ptr [rsp + 0x88]
    mov r9,qword ptr [rsp + 0x110]
    movsxd r10,dword ptr [rsp + 0x100]
    imul r10,qword ptr [rsp + 0x78]
    imul r12,rax
    imul r13,rax
    add r10,qword ptr [rsp + 0x40]
    add dword ptr [rsp + 0x30],0x1
    sub dword ptr [rsp + 0x34],0x1
    add qword ptr [rsp + 0x40],0x1
    lea rax,[r12 + rcx*0x1]
    mov r9d,dword ptr [r9 + rax*0x4]
    lea rax,[r13 + rcx*0x1]
    mov rcx,qword ptr [rsp + 0x110]
    mov ecx,dword ptr [rcx + rax*0x4]
    mov rax,qword ptr [rsp + 0x110]
    imul r9d,edx
    imul ecx,r11d
    add r9d,ecx
    lea rcx,[r12 + rsi*0x1]
    mov r12d,dword ptr [rsp + 0x3c]
    imul r9d,r8d
    mov r8d,dword ptr [rax + rcx*0x4]
    lea rcx,[r13 + rsi*0x1]
    mov eax,dword ptr [rax + rcx*0x4]
    mov rcx,qword ptr [rsp + 0xf0]
    imul r8d,edx
    imul eax,r11d
    mov r11d,dword ptr [rsp + 0x20]
    add eax,r8d
    mov r8d,dword ptr [rsp + 0x28]
    imul eax,ebp
    mov ebp,dword ptr [rsp + 0x2c]
    add eax,r9d
    mov r9d,dword ptr [rsp + 0x58]
    cdq
    add r8d,0x1
    mov dword ptr [rsp + 0x28],r8d
    idiv dword ptr [rsp + 0x5c]
    sub dword ptr [rcx + r10*0x4],eax
    mov eax,dword ptr [rsp + 0x38]
    mov r10d,dword ptr [rsp + 0x24]
    sub eax,0x1
    sub qword ptr [rsp + 0x60],0x1
    mov dword ptr [rsp + 0x38],eax
    jnz .lbl180005c60
    mov rax,qword ptr [rsp + 0x68]
    mov esi,dword ptr [rsp + 0x48]
    mov rcx,qword ptr [rsp + 0x80]
    mov r13d,dword ptr [rsp + 0x100]
    mov r9,qword ptr [rsp + 0x90]
    .lbl180005ddc: add rax,0x1
    movsxd r8,r10d
    add r14d,0x1
    cmp rax,r8
    mov qword ptr [rsp + 0x68],rax
    jl .lbl180005bf0
    .lbl180005df5: mov ebp,dword ptr [rsp + 0x50]
    mov esi,dword ptr [rsp + 0x108]
    add rdi,0x1
    cmp ebp,esi
    mov dword ptr [rsp + 0x2c],ebp
    jl .lbl180005ba3
    mov r14d,dword ptr [rsp + 0xf8]
    .lbl180005e18: add r11d,0x1
    add rbx,0x1
    cmp r11d,r12d
    mov dword ptr [rsp + 0x20],r11d
    jl .lbl180005b60
    .lbl180005e2e: xor eax,eax
    add rsp,0xa8
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
EXTR_FUN_18000ee80:
    mov qword ptr [rsp + 0x8],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x90
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x80],rax
    mov r12,qword ptr [rsp + 0xf0]
    movzx r14d,r8w
    movzx esi,dx
    mov eax,esi
    mov r15,r9
    mov word ptr [rsp + 0x58],0x13
    mov ebp,r14d
    mov word ptr [rsp + 0x5a],0xe0
    mov word ptr [rsp + 0x5c],0x201
    imul eax,r14d
    test eax,eax
    movsxd rbx,eax
    mov word ptr [rsp + 0x5e],0xe0
    lea rdi,[r12 + rbx*0x2]
    mov word ptr [rsp + 0x60],0x13
    mov word ptr [rsp + 0x68],0x9
    lea r13,[rdi + rbx*0x2]
    mov word ptr [rsp + 0x6a],0x28
    mov word ptr [rsp + 0x6c],0x71
    lea r10,[r13 + rbx*0x2]
    mov word ptr [rsp + 0x6e],0xd1
    mov word ptr [rsp + 0x70],0x101
    mov word ptr [rsp + 0x72],0xd1
    mov word ptr [rsp + 0x74],0x71
    mov word ptr [rsp + 0x76],0x28
    mov qword ptr [rsp + 0x50],r10
    mov word ptr [rsp + 0x78],0x9
    jle .lbl18000ef70
    mov r9,r10
    mov r8,rbx
    sub r9,rcx
    nop
    nop
    .lbl18000ef50: movzx eax,word ptr [rcx]
    cmp ax,0x7fff
    movzx edx,ax
    jc .lbl18000ef61
    mov edx,0x7fff
    .lbl18000ef61: mov word ptr [r9 + rcx*0x1],dx
    add rcx,0x2
    sub r8,0x1
    jnz .lbl18000ef50
    .lbl18000ef70: mov qword ptr [rsp + 0x30],r12
    lea r9,[rsp + 0x58]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r10
    mov dword ptr [rsp + 0x28],0x5
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    mov qword ptr [rsp + 0x30],rdi
    lea r9,[rsp + 0x58]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r12
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x5
    call EXTR_FUN_180002820
    mov r12,qword ptr [rsp + 0x50]
    mov qword ptr [rsp + 0x30],r13
    lea r9,[rsp + 0x68]
    mov rcx,r12
    mov r8d,ebp
    mov edx,esi
    mov dword ptr [rsp + 0x28],0x9
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    mov qword ptr [rsp + 0x30],r12
    lea r9,[rsp + 0x68]
    mov r8d,ebp
    mov edx,esi
    mov rcx,r13
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x9
    call EXTR_FUN_180002820
    test rbx,rbx
    jle .lbl18000f043
    mov rdx,r12
    sub rdi,r12
    nop
    .lbl18000f020: movzx ecx,word ptr [rdx]
    add rdx,0x2
    sub cx,word ptr [rdi + rdx*0x1 + -0x2]
    movzx eax,cx
    shl ax,0x2
    add cx,ax
    add cx,cx
    sub rbx,0x1
    mov word ptr [rdx + -0x2],cx
    jnz .lbl18000f020
    .lbl18000f043: lea eax,[rsi + -0xe]
    xor r12d,r12d
    xor r13d,r13d
    cmp eax,0xe
    mov qword ptr [rsp + 0x58],0xe
    jle .lbl18000f164
    lea r9d,[rbp + -0xe]
    mov rbp,qword ptr [rsp + 0x58]
    add eax,-0xe
    lea edi,[r12 + 0x12]
    mov qword ptr [rsp + 0x48],rax
    mov dword ptr [rsp + 0x40],r9d
    nop
    nop
    .lbl18000f080: mov ebx,0xe
    cmp r9d,ebx
    jle .lbl18000f13f
    lea ecx,[rdi + -0x8]
    lea rdx,[rbp + -0x4]
    lea r8d,[rbx + 0x4]
    lea esi,[r9 + -0xe]
    nop
    .lbl18000f0a0: cmp ecx,edi
    mov r11d,0x1
    mov r9d,ecx
    mov r10,rdx
    jg .lbl18000f0f9
    .lbl18000f0b0: cmp r11d,0x1
    jnz .lbl18000f11c
    lea edx,[r8 + -0x8]
    cmp edx,r8d
    jg .lbl18000f0e6
    movzx eax,r14w
    imul rax,r10
    add rax,rbx
    lea rcx,[rax + r15*0x1 + -0x4]
    nop
    .lbl18000f0d0: cmp byte ptr [rcx],0x0
    jz .lbl18000f0e3
    add edx,0x1
    add rcx,0x1
    cmp edx,r8d
    jle .lbl18000f0d0
    jmp .lbl18000f0e6
    .lbl18000f0e3: xor r11d,r11d
    .lbl18000f0e6: add r9d,0x1
    add r10,0x1
    cmp r9d,edi
    jle .lbl18000f0b0
    cmp r11d,0x1
    jnz .lbl18000f11c
    .lbl18000f0f9: mov rcx,qword ptr [rsp + 0x50]
    movzx eax,r14w
    imul rax,rbp
    add rax,rbx
    movzx ecx,word ptr [rcx + rax*0x2]
    test cx,cx
    jle .lbl18000f11c
    movsx eax,cx
    add r13d,0x1
    add r12d,eax
    .lbl18000f11c: add r8d,0x1
    add rbx,0x1
    sub rsi,0x1
    lea ecx,[rdi + -0x8]
    lea rdx,[rbp + -0x4]
    jnz .lbl18000f0a0
    mov rax,qword ptr [rsp + 0x48]
    mov r9d,dword ptr [rsp + 0x40]
    .lbl18000f13f: add edi,0x1
    add rbp,0x1
    sub rax,0x1
    mov qword ptr [rsp + 0x48],rax
    jnz .lbl18000f080
    test r13d,r13d
    jle .lbl18000f164
    lea eax,[r12 + r12*0x1]
    cdq
    idiv r13d
    jmp .lbl18000f167
    .lbl18000f164: mov eax,r12d
    .lbl18000f167: mov rcx,qword ptr [rsp + 0x80]
    xor rcx,rsp
    call EXTR_check_cookie
    mov rbx,qword ptr [rsp + 0xd0]
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
EXTR_FUN_18000f1a0:
    mov qword ptr [rsp + 0x18],rbx
    mov qword ptr [rsp + 0x20],rbp
    push r12
    push r13
    mov r10d,0xe
    movzx eax,dx
    or r11w,0xffff
    sub eax,r10d
    xor bx,bx
    mov r12,r9
    cmp eax,r10d
    movzx ebp,r8w
    mov r13,rcx
    jle .lbl18000f247
    mov qword ptr [rsp + 0x18],rsi
    movzx esi,r8w
    mov qword ptr [rsp + 0x20],rdi
    sub esi,r10d
    add eax,-0xe
    mov edi,eax
    .lbl18000f1e8: cmp esi,0xe
    jle .lbl18000f233
    movzx eax,bp
    lea r9d,[rsi + -0xe]
    imul rax,r10
    lea rdx,[rax + r12*0x1 + 0xe]
    lea r8,[r13 + rax*0x2 + 0x1c]
    .lbl18000f202: cmp byte ptr [rdx],0x0
    jbe .lbl18000f225
    movzx ecx,word ptr [r8]
    cmp r11w,cx
    movzx eax,cx
    cmovc ax,r11w
    cmp bx,cx
    cmova cx,bx
    movzx r11d,ax
    movzx ebx,cx
    .lbl18000f225: add r8,0x2
    add rdx,0x1
    sub r9,0x1
    jnz .lbl18000f202
    .lbl18000f233: add r10,0x1
    sub rdi,0x1
    jnz .lbl18000f1e8
    mov rdi,qword ptr [rsp + 0x20]
    mov rsi,qword ptr [rsp + 0x18]
    .lbl18000f247: mov rbp,qword ptr [rsp + 0x30]
    movzx eax,bx
    mov rbx,qword ptr [rsp + 0x28]
    movzx ecx,r11w
    sub eax,ecx
    sar eax,1
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_18000e310:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    sub rsp,0x80
    mov r13d,edx
    xor edx,edx
    mov r12d,r8d
    mov r14,rcx
    lea r8d,[rdx + 0x38]
    lea rcx,[rsp + 0x44]
    mov ebp,r9d
    mov dword ptr [rsp + 0x40],0x0
    call EXTR_memset wrt ..plt
    mov rbx,qword ptr [rsp + 0xd8]
    mov eax,dword ptr [rsp + 0xc8]
    mov r11d,r13d
    imul r11d,r12d
    movsxd rsi,r11d
    lea r9,[rsp + 0x40]
    mov r8d,r12d
    mov edx,r13d
    mov rcx,r14
    mov dword ptr [rbx],eax
    add rsi,rbx
    mov qword ptr [rsp + 0x20],rbx
    call EXTR_FUN_18000d400
    mov rdi,qword ptr [rsp + 0xd0]
    mov eax,dword ptr [rsp + 0x40]
    mov r9d,dword ptr [rsp + 0xc0]
    mov dword ptr [rdi + 0x4],eax
    mov eax,dword ptr [rsp + 0x44]
    mov r8d,r12d
    mov dword ptr [rdi + 0x8],eax
    mov eax,dword ptr [rsp + 0x48]
    mov edx,r13d
    mov dword ptr [rdi + 0xc],eax
    mov eax,dword ptr [rsp + 0x4c]
    mov rcx,r14
    mov dword ptr [rdi],0x1
    mov qword ptr [rsp + 0x20],rbx
    mov dword ptr [rdi + 0x10],eax
    call EXTR_FUN_18000d5c0
    mov r9,rbx
    mov r8d,r12d
    mov edx,r13d
    mov rcx,r14
    mov dword ptr [rdi + 0x14],eax
    call EXTR_FUN_18000d840
    mov r9d,ebp
    mov r8d,r12d
    mov dword ptr [rdi + 0x18],eax
    lea rax,[rsp + 0x40]
    mov edx,r13d
    mov rcx,r14
    mov qword ptr [rsp + 0x28],rbx
    mov qword ptr [rsp + 0x20],rax
    call EXTR_FUN_18000dac0
    mov eax,dword ptr [rsp + 0x4c]
    mov r11d,dword ptr [rsp + 0x40]
    mov dword ptr [rdi + 0x20],eax
    mov eax,dword ptr [rsp + 0x5c]
    mov dword ptr [rsp + 0x30],0x1
    mov dword ptr [rdi + 0x2c],eax
    mov eax,dword ptr [rsp + 0x60]
    mov r9,rbx
    mov r8d,r12d
    mov edx,r13d
    mov rcx,r14
    mov dword ptr [rsp + 0x28],ebp
    mov dword ptr [rdi + 0x30],eax
    mov dword ptr [rdi + 0x1c],r11d
    mov qword ptr [rsp + 0x20],rsi
    call EXTR_FUN_180013c70
    mov rax,qword ptr [rsi]
    mov qword ptr [rsp + 0x40],rax
    mov rax,qword ptr [rsi + 0x8]
    lea r11,[rsp + 0x80]
    mov rbx,qword ptr [r11 + 0x20]
    mov rbp,qword ptr [r11 + 0x28]
    mov qword ptr [rsp + 0x48],rax
    mov eax,dword ptr [rsp + 0x40]
    mov dword ptr [rdi + 0x24],eax
    mov eax,dword ptr [rsp + 0x44]
    mov dword ptr [rdi + 0x28],eax
    xor eax,eax
    mov rsi,qword ptr [r11 + 0x30]
    mov rdi,qword ptr [r11 + 0x38]
    mov rsp,r11
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_18000d840:
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
    push r15
    sub rsp,0x38
    mov eax,edx
    xor r15d,r15d
    mov rbp,r9
    cdq
    xor r13d,r13d
    xor r12d,r12d
    and edx,0x7
    mov r9d,r8d
    mov dword ptr [rsp + 0x20],r15d
    add eax,edx
    sar eax,0x3
    movsxd r10,eax
    mov eax,r8d
    cdq
    mov r8,r10
    mov qword ptr [rsp + 0x28],r10
    and edx,0x7
    add eax,edx
    sar eax,0x3
    mov esi,eax
    movsxd r14,eax
    imul esi,r10d
    test r10d,r10d
    mov dword ptr [rsp + 0x88],esi
    jle .lbl18000d97c
    lea rax,[r14 + r14*0x1]
    mov r15,rbp
    .lbl18000d8b5: test r14,r14
    jle .lbl18000d958
    movsxd rbx,r9d
    mov rsi,r15
    mov rbp,r14
    lea rdi,[rbx + rbx*0x1]
    imul rbx,r12
    shl rbx,0x4
    add rbx,rcx
    nop
    nop
    nop
    .lbl18000d8e0: xor r8d,r8d
    mov r10,rbx
    lea r11d,[r8 + 0x8]
    nop
    nop
    .lbl18000d8f0: mov rdx,r10
    mov r9d,0x8
    nop
    nop
    .lbl18000d900: movsx ecx,word ptr [rdx]
    mov eax,ecx
    neg eax
    cmp ecx,eax
    movsx eax,word ptr [rdx]
    jg .lbl18000d910
    neg eax
    .lbl18000d910: cmp eax,r8d
    cmovg r8d,eax
    add rdx,0x2
    sub r9,0x1
    jnz .lbl18000d900
    add r10,rdi
    sub r11,0x1
    jnz .lbl18000d8f0
    mov word ptr [rsi],r8w
    add rsi,0x2
    add r13d,r8d
    add rbx,0x10
    sub rbp,0x1
    jnz .lbl18000d8e0
    mov r8,qword ptr [rsp + 0x28]
    mov r9d,dword ptr [rsp + 0x90]
    mov rcx,qword ptr [rsp + 0x80]
    lea rax,[r14 + r14*0x1]
    .lbl18000d958: add r12,0x1
    add r15,rax
    cmp r12,r8
    jl .lbl18000d8b5
    mov r15d,dword ptr [rsp + 0x20]
    mov esi,dword ptr [rsp + 0x88]
    mov rbp,qword ptr [rsp + 0x98]
    .lbl18000d97c: test r8,r8
    jle .lbl18000da41
    xor edi,edi
    lea r12,[r14 + r14*0x1]
    mov rbx,rbp
    nop
    .lbl18000d990: xor r10d,r10d
    xor r11d,r11d
    xor r9d,r9d
    cmp r14,0x2
    jl .lbl18000d9ff
    mov r12d,dword ptr [rsp + 0x88]
    mov eax,r13d
    lea r8,[r14 + -0x2]
    cdq
    shr r8,1
    idiv esi
    add r8,0x1
    mov rsi,rbx
    lea r9,[r8 + r8*0x1]
    mov ebp,eax
    .lbl18000d9c1: movsx ecx,word ptr [rsi]
    mov eax,ebp
    add rsi,0x4
    sub eax,ecx
    movsx ecx,word ptr [rsi + -0x2]
    imul eax,eax
    cdq
    idiv r12d
    add r10d,eax
    mov eax,ebp
    sub eax,ecx
    imul eax,eax
    cdq
    idiv r12d
    add r11d,eax
    sub r8,0x1
    jnz .lbl18000d9c1
    mov r8,qword ptr [rsp + 0x28]
    mov rbp,qword ptr [rsp + 0x98]
    lea r12,[r14 + r14*0x1]
    .lbl18000d9ff: cmp r9,r14
    mov esi,dword ptr [rsp + 0x88]
    jge .lbl18000da25
    lea rax,[rdi + r9*0x1]
    movsx ecx,word ptr [rbp + rax*0x2]
    mov eax,r13d
    cdq
    idiv esi
    sub eax,ecx
    imul eax,eax
    cdq
    idiv esi
    add r15d,eax
    .lbl18000da25: lea eax,[r11 + r10*0x1]
    add rdi,r14
    add rbx,r12
    add r15d,eax
    sub r8,0x1
    mov qword ptr [rsp + 0x28],r8
    jnz .lbl18000d990
    .lbl18000da41: mov ecx,r15d
    add rsp,0x38
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    jmp EXTR_FUN_180002420

section .data


section .text
EXTR_FUN_18000e970:
    push rbp
    sub rsp,0x20
    xor r9d,r9d
    mov eax,edx
    mov r10,rcx
    mov rbp,r9
    imul eax,r8d
    test edx,edx
    jz .lbl18000ea69
    test r8d,r8d
    jz .lbl18000ea69
    mov qword ptr [rsp + 0x30],rbx
    mov qword ptr [rsp + 0x40],rdi
    movsxd rdi,eax
    cmp rdi,0x2
    mov r8,r9
    mov r11,r9
    mov rdx,r9
    mov rbx,r9
    mov rcx,r9
    jl .lbl18000e9f0
    mov qword ptr [rsp + 0x38],rsi
    lea rsi,[rdi + -0x1]
    .lbl18000e9c1: movzx eax,word ptr [r10 + rcx*0x2]
    add rcx,0x2
    add rdx,rax
    imul eax,eax
    cdqe
    add r8,rax
    movzx eax,word ptr [r10 + rcx*0x2 + -0x2]
    add rbx,rax
    imul eax,eax
    cdqe
    add r11,rax
    cmp rcx,rsi
    jl .lbl18000e9c1
    mov rsi,qword ptr [rsp + 0x38]
    .lbl18000e9f0: cmp rcx,rdi
    jge .lbl18000ea04
    movzx r9d,word ptr [r10 + rcx*0x2]
    mov ecx,r9d
    imul ecx,r9d
    movsxd rbp,ecx
    .lbl18000ea04: lea rcx,[rbx + rdx*0x1]
    add r9,rcx
    imul r9,r9
    mov rax,r9
    cqo
    idiv rdi
    cqo
    idiv rdi
    mov r9,rax
    lea rax,[r11 + r8*0x1]
    add rax,rbp
    cqo
    idiv rdi
    sub rax,r9
    mov rcx,rax
    call EXTR_FUN_18000d090
    mov rdi,qword ptr [rsp + 0x40]
    mov rbx,qword ptr [rsp + 0x30]
    test rax,rax
    mov r10,rax
    jz .lbl18000ea5f
    mov rcx,r9
    call EXTR_FUN_18000d090
    shl rax,0x6
    cqo
    idiv r10
    add rsp,0x20
    pop rbp
    ret
    .lbl18000ea5f: or ax,0xffff
    add rsp,0x20
    pop rbp
    ret
    .lbl18000ea69: xor ax,ax
    add rsp,0x20
    pop rbp
    ret

section .data


section .text
EXTR_FUN_18000d090:
    xor edx,edx
    test rcx,rcx
    mov r8,rcx
    mov rax,0x4000000000000000
    jg .lbl18000d0b0
    xor eax,eax
    ret
    .lbl18000d0b0: mov rcx,rdx
    shr rdx,1
    or rcx,rax
    cmp r8,rcx
    jc .lbl18000d0c4
    sub r8,rcx
    or rdx,rax
    .lbl18000d0c4: shr rax,0x2
    jnz .lbl18000d0b0
    mov rax,rdx
    ret

section .data


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
EXTR_FUN_18000e4a0:
    mov r11,rsp
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    sub rsp,0xe0
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0xb0],rax
    movsxd rdi,r8d
    mov esi,edx
    mov dword ptr [rsp + 0x4c],edx
    mov rdx,qword ptr [rsp + 0x130]
    lea eax,[rsi + 0x4]
    mov rbp,r9
    mov qword ptr [rsp + 0xa0],r9
    mov qword ptr [rsp + 0x60],rdx
    mov qword ptr [rsp + 0x20],rdx
    lea ebx,[rdi + 0x4]
    mov r9d,0x2
    mov r8d,edi
    mov dword ptr [rsp + 0x48],edi
    mov word ptr [r11 + -0x60],0xda2
    mov word ptr [r11 + -0x5e],0x64bb
    imul eax,ebx
    cdqe
    mov word ptr [r11 + -0x5c],0xda2
    lea r12,[rdx + rax*0x2]
    mov edx,esi
    mov qword ptr [rsp + 0x98],r12
    call EXTR_FUN_18000d0d0
    lea eax,[rsi + 0x2]
    cmp eax,0x2
    movsxd r8,ebx
    mov rcx,rdi
    mov qword ptr [rsp + 0x58],r8
    jle .lbl18000e790
    add rcx,rcx
    lea edx,[rdi + 0x2]
    xor edi,edi
    mov qword ptr [rsp + 0x78],rcx
    mov qword ptr [rsp + 0xd8],r13
    mov ebx,0x4
    mov r9,qword ptr [rsp + 0x78]
    mov rcx,rbp
    add eax,-0x2
    mov qword ptr [rsp + 0xd0],r14
    mov dword ptr [rsp + 0x44],edx
    mov qword ptr [rsp + 0x90],rdi
    mov qword ptr [rsp + 0x68],rcx
    mov qword ptr [rsp + 0xc8],r15
    mov dword ptr [rsp + 0x40],ebx
    mov qword ptr [rsp + 0x88],rax
    .lbl18000e591: cmp edx,0x2
    jle .lbl18000e733
    mov qword ptr [rsp + 0x70],rcx
    lea ecx,[rdx + -0x2]
    lea r9d,[rbx + -0x4]
    mov r12d,0x4
    xor r14d,r14d
    mov qword ptr [rsp + 0x80],rcx
    nop
    nop
    nop
    .lbl18000e5c0: xor esi,esi
    xor r15d,r15d
    cmp r9d,ebx
    jg .lbl18000e6a7
    lea ecx,[r12 + -0x4]
    mov edx,r12d
    mov r13d,ebx
    sub edx,ecx
    mov r11,rdi
    sub r13d,r9d
    add edx,0x1
    mov dword ptr [rsp + 0x50],edx
    imul r11,r8
    add r13d,0x1
    .lbl18000e5f0: xor ebx,ebx
    xor edi,edi
    cmp edx,0x2
    mov r9d,ecx
    mov rbp,r14
    jl .lbl18000e664
    mov rcx,qword ptr [rsp + 0x60]
    lea rax,[r14 + r11*0x1]
    lea r10,[rcx + rax*0x2]
    mov eax,r12d
    sub eax,r9d
    sub eax,0x1
    shr eax,1
    add eax,0x1
    mov r8d,eax
    lea r9d,[r9 + rax*0x2]
    lea rbp,[r14 + rax*0x2]
    nop
    nop
    nop
    .lbl18000e630: movzx eax,word ptr [r10]
    movzx edx,word ptr [r10 + 0x2]
    add r10,0x4
    add rbx,rax
    add rdi,rdx
    imul eax,eax
    imul edx,edx
    movsxd rcx,eax
    movsxd rax,edx
    add r15,rax
    add r15,rcx
    sub r8,0x1
    jnz .lbl18000e630
    mov edx,dword ptr [rsp + 0x50]
    mov r8,qword ptr [rsp + 0x58]
    .lbl18000e664: cmp r9d,r12d
    jg .lbl18000e682
    mov rcx,qword ptr [rsp + 0x60]
    lea rax,[r11 + rbp*0x1]
    movzx ecx,word ptr [rcx + rax*0x2]
    add rsi,rcx
    imul ecx,ecx
    movsxd rax,ecx
    add r15,rax
    .lbl18000e682: lea rax,[rdi + rbx*0x1]
    add r11,r8
    lea ecx,[r12 + -0x4]
    add rsi,rax
    sub r13,0x1
    jnz .lbl18000e5f0
    mov ebx,dword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x90]
    .lbl18000e6a7: imul rsi,rsi
    mov rax,-0x5c28f5c28f5c28f5
    imul rsi
    add rdx,rsi
    sar rdx,0x4
    mov rax,rdx
    shr rax,0x3f
    add rdx,rax
    mov rax,0x2aaaaaaaaaaaaaab
    sub r15,rdx
    imul r15
    sar rdx,0x2
    mov rcx,rdx
    shr rcx,0x3f
    add rcx,rdx
    call EXTR_FUN_180002420
    mov rcx,qword ptr [rsp + 0x70]
    mov r8,qword ptr [rsp + 0x58]
    mov word ptr [rcx],ax
    add rcx,0x2
    add r12d,0x1
    add r14,0x1
    sub qword ptr [rsp + 0x80],0x1
    lea r9d,[rbx + -0x4]
    mov qword ptr [rsp + 0x70],rcx
    jnz .lbl18000e5c0
    mov rcx,qword ptr [rsp + 0x68]
    mov rax,qword ptr [rsp + 0x88]
    mov edx,dword ptr [rsp + 0x44]
    mov r9,qword ptr [rsp + 0x78]
    .lbl18000e733: add ebx,0x1
    add rcx,r9
    add rdi,0x1
    sub rax,0x1
    mov dword ptr [rsp + 0x40],ebx
    mov qword ptr [rsp + 0x68],rcx
    mov qword ptr [rsp + 0x88],rax
    mov qword ptr [rsp + 0x90],rdi
    jnz .lbl18000e591
    mov edi,dword ptr [rsp + 0x48]
    mov esi,dword ptr [rsp + 0x4c]
    mov rbp,qword ptr [rsp + 0xa0]
    mov r12,qword ptr [rsp + 0x98]
    mov r15,qword ptr [rsp + 0xc8]
    mov r14,qword ptr [rsp + 0xd0]
    mov r13,qword ptr [rsp + 0xd8]
    .lbl18000e790: mov qword ptr [rsp + 0x30],r12
    lea r9,[rsp + 0xa8]
    mov r8d,edi
    mov edx,esi
    mov rcx,rbp
    mov dword ptr [rsp + 0x28],0x1
    mov dword ptr [rsp + 0x20],0x3
    call EXTR_FUN_180002820
    mov qword ptr [rsp + 0x30],rbp
    lea r9,[rsp + 0xa8]
    mov r8d,edi
    mov edx,esi
    mov rcx,r12
    mov dword ptr [rsp + 0x28],0x3
    mov dword ptr [rsp + 0x20],0x1
    call EXTR_FUN_180002820
    mov rcx,qword ptr [rsp + 0xb0]
    xor rcx,rsp
    call EXTR_check_cookie
    add rsp,0xe0
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

section .data


section .text
EXTR_FUN_18000d0d0:
    mov dword ptr [rsp + 0x20],r9d
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    sub rsp,0x40
    movsxd rbp,r9d
    movsxd rsi,r8d
    movsxd rbx,edx
    lea eax,[rbp + rbp*0x1]
    lea edx,[rax + rbx*0x1]
    lea edi,[rax + rsi*0x1]
    mov eax,edi
    mov dword ptr [rsp + 0x88],edx
    imul eax,edx
    test eax,eax
    jle .lbl18000d12d
    mov r12,qword ptr [rsp + 0xa0]
    mov r8d,eax
    xor edx,edx
    mov rcx,r12
    add r8,r8
    call EXTR_memset wrt ..plt
    mov rcx,qword ptr [rsp + 0x80]
    jmp .lbl18000d135
    .lbl18000d12d: mov r12,qword ptr [rsp + 0xa0]
    .lbl18000d135: test ebx,ebx
    movsxd r10,edi
    mov r13,rbp
    mov r14,rsi
    mov r9,rbx
    mov qword ptr [rsp + 0x20],rbx
    mov qword ptr [rsp + 0x28],r10
    jle .lbl18000d1ab
    mov r11,qword ptr [rsp + 0x20]
    mov rax,rbp
    lea rdi,[r10 + r10*0x1]
    imul rax,r10
    add rax,rbp
    lea rbx,[rsi + rsi*0x1]
    lea r10,[r12 + rax*0x2]
    mov r9,rcx
    nop
    .lbl18000d170: test esi,esi
    jle .lbl18000d195
    mov rcx,r10
    mov rdx,r9
    mov r8,rsi
    nop
    .lbl18000d180: movzx eax,word ptr [rdx]
    add rcx,0x2
    add rdx,0x2
    sub r8,0x1
    mov word ptr [rcx + -0x2],ax
    jnz .lbl18000d180
    .lbl18000d195: add r10,rdi
    add r9,rbx
    sub r11,0x1
    jnz .lbl18000d170
    mov r9,qword ptr [rsp + 0x20]
    mov r10,qword ptr [rsp + 0x28]
    .lbl18000d1ab: xor edi,edi
    test ebp,ebp
    mov r11,rdi
    jle .lbl18000d2ac
    mov rdx,qword ptr [rsp + 0x80]
    mov edi,dword ptr [rsp + 0x98]
    mov r8,qword ptr [rsp + 0xa0]
    mov qword ptr [rsp + 0x90],r15
    mov rax,rsi
    lea rbx,[r9 + r9*0x1 + -0x2]
    neg rax
    mov r15,r10
    lea r12d,[rsi + rdi*0x1]
    imul rbx,r14
    add rax,rax
    neg r15
    mov qword ptr [rsp + 0x30],rax
    mov rax,rbp
    add r15,r15
    sub rax,rbp
    lea rcx,[rax + rax*0x1]
    lea rax,[rbp + -0x1]
    add rbx,rcx
    lea rsi,[rcx + rdx*0x1]
    lea rcx,[r14 + r14*0x1]
    imul rax,r10
    add rax,rbp
    add rbx,rdx
    lea rbp,[r8 + rax*0x2]
    mov rax,qword ptr [rsp + 0x30]
    .lbl18000d227: cmp edi,r12d
    jge .lbl18000d290
    lea rax,[r9 + r13*0x1]
    mov rdx,rbx
    mov r9,rsi
    add rax,r11
    imul rax,r10
    mov r10d,r12d
    add rax,r13
    sub r10d,edi
    lea rcx,[r8 + rax*0x2]
    mov r8,rbp
    nop
    .lbl18000d250: movzx eax,word ptr [r9]
    add rcx,0x2
    add r9,0x2
    mov word ptr [r8],ax
    movzx eax,word ptr [rdx]
    add r8,0x2
    mov word ptr [rcx + -0x2],ax
    add rdx,0x2
    sub r10,0x1
    jnz .lbl18000d250
    mov rax,qword ptr [rsp + 0x30]
    mov r8,qword ptr [rsp + 0xa0]
    mov r9,qword ptr [rsp + 0x20]
    mov r10,qword ptr [rsp + 0x28]
    lea rcx,[r14 + r14*0x1]
    .lbl18000d290: add r11,0x1
    add rbx,rax
    add rbp,r15
    add rsi,rcx
    cmp r11,r13
    jl .lbl18000d227
    mov r15,qword ptr [rsp + 0x90]
    xor edi,edi
    .lbl18000d2ac: test r13,r13
    movsxd r12,dword ptr [rsp + 0x88]
    jle .lbl18000d333
    mov r8,qword ptr [rsp + 0xa0]
    add r14,r13
    lea rbx,[r8 + r13*0x2]
    lea rbp,[r8 + r14*0x2 + -0x2]
    lea rsi,[rbx + -0x2]
    .lbl18000d2d1: test r12,r12
    jle .lbl18000d319
    lea rax,[r14 + rdi*0x1]
    lea rcx,[r10 + r10*0x1]
    mov rdx,rbx
    mov r9,rsi
    mov r10,rbp
    mov r11,r12
    lea r8,[r8 + rax*0x2]
    nop
    .lbl18000d2f0: movzx eax,word ptr [rdx]
    add rdx,rcx
    mov word ptr [r9],ax
    movzx eax,word ptr [r10]
    add r9,rcx
    mov word ptr [r8],ax
    add r8,rcx
    add r10,rcx
    sub r11,0x1
    jnz .lbl18000d2f0
    mov r8,qword ptr [rsp + 0xa0]
    .lbl18000d319: mov r10,qword ptr [rsp + 0x28]
    add rdi,0x1
    add rbx,0x2
    sub rsi,0x2
    sub rbp,0x2
    cmp rdi,r13
    jl .lbl18000d2d1
    .lbl18000d333: add rsp,0x40
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
EXTR_FUN_18000f270:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    mov qword ptr [r11 + 0x8],rcx
    push r15
    sub rsp,0x50
    xor ecx,ecx
    movzx eax,dx
    movzx r15d,r8w
    xor r8d,r8d
    sub eax,0xe
    mov r10,r9
    cmp eax,0xe
    mov dword ptr [rsp + 0x68],r8d
    mov dword ptr [rsp],ecx
    jle .lbl18000f478
    mov qword ptr [r11 + 0x18],rbx
    mov qword ptr [r11 + -0x10],rbp
    mov qword ptr [r11 + -0x18],rsi
    mov qword ptr [r11 + -0x20],rdi
    mov qword ptr [r11 + -0x28],r12
    mov qword ptr [r11 + -0x30],r13
    movzx edx,r15w
    mov r9d,0xd
    mov qword ptr [r11 + -0x38],r14
    sub edx,0xe
    add eax,-0xe
    lea r11d,[r8 + 0x1]
    lea r13d,[r8 + 0xf]
    mov qword ptr [rsp + 0x8],r9
    mov qword ptr [rsp + 0x10],rax
    mov dword ptr [rsp + 0x4],edx
    nop
    nop
    nop
    .lbl18000f2f0: mov esi,0xe
    cmp edx,esi
    jle .lbl18000f413
    lea eax,[r13 + -0x2]
    lea r14d,[rsi + -0x1]
    nop
    nop
    nop
    .lbl18000f310: or bp,0xffff
    xor r12w,r12w
    cmp eax,r13d
    mov edi,r11d
    mov rbx,r9
    mov r11d,eax
    jg .lbl18000f3c0
    nop
    nop
    .lbl18000f330: cmp edi,0x1
    jnz .lbl18000f3d9
    lea r8d,[rsi + -0x1]
    lea r9d,[rsi + 0x1]
    cmp r8d,r9d
    jg .lbl18000f3a2
    mov rax,qword ptr [rsp + 0x60]
    movzx ecx,r15w
    imul rcx,rbx
    add rcx,r14
    lea rdx,[rcx + r10*0x1]
    lea r10,[rax + rcx*0x2]
    nop
    .lbl18000f360: cmp byte ptr [rdx],0x0
    jbe .lbl18000f39b
    movzx ecx,word ptr [r10]
    cmp bp,cx
    movzx eax,cx
    cmovc ax,bp
    cmp r12w,cx
    cmova cx,r12w
    add r8d,0x1
    add r10,0x2
    add rdx,0x1
    cmp r8d,r9d
    movzx ebp,ax
    movzx r12d,cx
    jle .lbl18000f360
    mov r10,qword ptr [rsp + 0x78]
    jmp .lbl18000f3a2
    .lbl18000f39b: mov r10,qword ptr [rsp + 0x78]
    xor edi,edi
    .lbl18000f3a2: add r11d,0x1
    add rbx,0x1
    cmp r11d,r13d
    jle .lbl18000f330
    cmp edi,0x1
    jnz .lbl18000f3e4
    mov r8d,dword ptr [rsp + 0x68]
    mov ecx,dword ptr [rsp]
    mov edx,dword ptr [rsp + 0x4]
    .lbl18000f3c0: add ecx,0x1
    movzx eax,bp
    mov dword ptr [rsp],ecx
    movzx ecx,r12w
    sub ecx,eax
    add r8d,ecx
    mov dword ptr [rsp + 0x68],r8d
    jmp .lbl18000f3ed
    .lbl18000f3d9: mov r8d,dword ptr [rsp + 0x68]
    mov edx,dword ptr [rsp + 0x4]
    jmp .lbl18000f3ed
    .lbl18000f3e4: mov r8d,dword ptr [rsp + 0x68]
    mov edx,dword ptr [rsp + 0x4]
    .lbl18000f3ed: mov ecx,dword ptr [rsp]
    mov r9,qword ptr [rsp + 0x8]
    add esi,0x1
    add r14,0x1
    cmp esi,edx
    lea eax,[r13 + -0x2]
    mov r11d,0x1
    jl .lbl18000f310
    mov rax,qword ptr [rsp + 0x10]
    .lbl18000f413: add r9,0x1
    add r13d,0x1
    sub rax,0x1
    mov qword ptr [rsp + 0x10],rax
    mov qword ptr [rsp + 0x8],r9
    jnz .lbl18000f2f0
    test ecx,ecx
    mov r14,qword ptr [rsp + 0x20]
    mov r13,qword ptr [rsp + 0x28]
    mov r12,qword ptr [rsp + 0x30]
    mov rdi,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rbx,qword ptr [rsp + 0x70]
    jle .lbl18000f478
    imul r8d,r8d,0x64
    mov eax,r8d
    cdq
    idiv ecx
    mov ecx,dword ptr [rsp + 0x80]
    cmp ecx,0x1
    cmovl ecx,r11d
    cdq
    idiv ecx
    add rsp,0x50
    pop r15
    ret
    .lbl18000f478: mov eax,r8d
    add rsp,0x50
    pop r15
    ret

section .data


section .text
EXTR_FUN_18000f490:
    lea eax,[rcx + -0x2]
    lea ecx,[rdx + -0x2]
    imul eax,ecx
    shl eax,0x2
    ret

section .data


section .text
EXTR_FUN_18000ec90:
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    mov qword ptr [r11 + 0x8],rcx
    push r14
    sub rsp,0x50
    xor ecx,ecx
    movzx eax,dx
    movzx r14d,r8w
    xor r8d,r8d
    sub eax,0xe
    mov dword ptr [rsp],ecx
    cmp eax,0xe
    mov dword ptr [rsp + 0x68],r8d
    jle .lbl18000ee71
    mov qword ptr [r11 + 0x18],rbx
    mov qword ptr [r11 + -0x10],rbp
    mov qword ptr [r11 + -0x18],rsi
    mov qword ptr [r11 + -0x20],rdi
    mov qword ptr [r11 + -0x28],r12
    movzx ebx,r14w
    sub ebx,0xe
    mov edx,0x8
    add eax,-0xe
    mov qword ptr [r11 + -0x30],r13
    lea r12d,[r8 + 0x14]
    mov qword ptr [rsp + 0x10],rax
    mov qword ptr [r11 + -0x38],r15
    mov dword ptr [rsp + 0x4],ebx
    mov qword ptr [rsp + 0x8],rdx
    nop
    .lbl18000ed00: cmp ebx,0xe
    jle .lbl18000ee15
    mov r10d,0x14
    lea r11d,[r12 + -0xc]
    lea r15d,[rbx + -0xe]
    lea r13d,[r10 + -0xc]
    nop
    .lbl18000ed20: or si,0xffff
    xor bp,bp
    cmp r11d,r12d
    mov edi,0x1
    mov rbx,rdx
    jg .lbl18000edc9
    nop
    nop
    .lbl18000ed40: cmp edi,0x1
    jnz .lbl18000ede1
    lea r8d,[r10 + -0xc]
    cmp r8d,r10d
    jg .lbl18000edaf
    mov rax,qword ptr [rsp + 0x60]
    movzx ecx,r14w
    imul rcx,rbx
    add rcx,r13
    lea rdx,[rcx + r9*0x1]
    lea r9,[rax + rcx*0x2]
    nop
    nop
    .lbl18000ed70: cmp byte ptr [rdx],0x0
    jbe .lbl18000eda8
    movzx ecx,word ptr [r9]
    cmp si,cx
    movzx eax,cx
    cmovc ax,si
    cmp bp,cx
    cmova cx,bp
    add r8d,0x1
    add r9,0x2
    add rdx,0x1
    cmp r8d,r10d
    movzx esi,ax
    movzx ebp,cx
    jle .lbl18000ed70
    mov r9,qword ptr [rsp + 0x78]
    jmp .lbl18000edaf
    .lbl18000eda8: mov r9,qword ptr [rsp + 0x78]
    xor edi,edi
    .lbl18000edaf: add r11d,0x1
    add rbx,0x1
    cmp r11d,r12d
    jle .lbl18000ed40
    cmp edi,0x1
    jnz .lbl18000ede8
    mov r8d,dword ptr [rsp + 0x68]
    mov ecx,dword ptr [rsp]
    .lbl18000edc9: add ecx,0x1
    movzx eax,si
    mov dword ptr [rsp],ecx
    movzx ecx,bp
    sub ecx,eax
    add r8d,ecx
    mov dword ptr [rsp + 0x68],r8d
    jmp .lbl18000eded
    .lbl18000ede1: mov r8d,dword ptr [rsp + 0x68]
    jmp .lbl18000eded
    .lbl18000ede8: mov r8d,dword ptr [rsp + 0x68]
    .lbl18000eded: mov ecx,dword ptr [rsp]
    mov rdx,qword ptr [rsp + 0x8]
    add r10d,0x1
    add r13,0x1
    sub r15,0x1
    lea r11d,[r12 + -0xc]
    jnz .lbl18000ed20
    mov ebx,dword ptr [rsp + 0x4]
    mov rax,qword ptr [rsp + 0x10]
    .lbl18000ee15: add rdx,0x1
    add r12d,0x1
    sub rax,0x1
    mov qword ptr [rsp + 0x10],rax
    mov qword ptr [rsp + 0x8],rdx
    jnz .lbl18000ed00
    test ecx,ecx
    mov r15,qword ptr [rsp + 0x20]
    mov r13,qword ptr [rsp + 0x28]
    mov r12,qword ptr [rsp + 0x30]
    mov rdi,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rbx,qword ptr [rsp + 0x70]
    jle .lbl18000ee71
    mov eax,r8d
    cdq
    idiv ecx
    mov ecx,0x28
    cmp eax,ecx
    cmovg ecx,eax
    mov eax,ecx
    add rsp,0x50
    pop r14
    ret
    .lbl18000ee71: mov eax,r8d
    add rsp,0x50
    pop r14
    ret

section .data


section .text
EXTR_FUN_18000e920:
    mov r8d,0x2
    xor r10d,r10d
    xor r9d,r9d
    cmp edx,r8d
    jbe .lbl18000e964
    add rcx,0x8
    .lbl18000e935: cmp r8d,0x9
    jnc .lbl18000e940
    mov eax,dword ptr [rcx]
    add r10,rax
    .lbl18000e940: mov eax,dword ptr [rcx]
    add r8d,0x1
    add rcx,0x4
    add r9,rax
    cmp r8d,edx
    jc .lbl18000e935
    test r9,r9
    jle .lbl18000e964
    shl r10,0x6
    mov rax,r10
    cqo
    idiv r9
    ret
    .lbl18000e964: xor eax,eax
    ret

section .data


section .text
EXTR_FUN_18000ea90:
    mov qword ptr [rsp + 0x8],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    movsxd r15,edx
    movsxd rdi,r8d
    mov rdx,rcx
    mov r8d,r15d
    mov rbx,r9
    xor r13d,r13d
    xor r14d,r14d
    imul r8d,edi
    movsxd rsi,r8d
    add rsi,r9
    mov rcx,rsi
    call EXTR_memcpy wrt ..plt
    mov r9b,0x1
    mov r8d,edi
    mov edx,r15d
    mov rcx,rsi
    call EXTR_FUN_18000d350
    mov r9d,edi
    mov r8d,r15d
    mov rdx,rbx
    mov rcx,rsi
    mov dword ptr [rsp + 0x20],0x1
    mov dword ptr [rsp + 0x78],eax
    call EXTR_FUN_1800047d0
    mov r9b,0x1
    mov r8d,edi
    mov edx,r15d
    mov rcx,rsi
    call EXTR_FUN_18000d350
    mov ebx,dword ptr [rsp + 0x78]
    xor edx,edx
    mov ecx,ebx
    mov r9,rdi
    sub ecx,eax
    cmp rdi,0x2
    mov dword ptr [rsp + 0x80],ecx
    mov r8d,ecx
    jl .lbl18000eb77
    lea rax,[r15 + -0x1]
    lea r11,[rdi + -0x1]
    imul rax,r9
    lea r10,[rax + rsi*0x1 + 0x1]
    nop
    nop
    .lbl18000eb40: mov al,byte ptr [rsi + rdx*0x1]
    mov cl,byte ptr [r10 + rdx*0x1 + -0x1]
    add rdx,0x2
    and eax,0x1
    and ecx,0x1
    add eax,r13d
    lea r13d,[rax + rcx*0x1]
    mov al,byte ptr [rsi + rdx*0x1 + -0x1]
    mov cl,byte ptr [r10 + rdx*0x1 + -0x2]
    and eax,0x1
    and ecx,0x1
    add eax,r14d
    cmp rdx,r11
    lea r14d,[rax + rcx*0x1]
    jl .lbl18000eb40
    mov ecx,r8d
    .lbl18000eb77: cmp rdx,rdi
    jge .lbl18000eb9a
    mov r8b,byte ptr [rdx + rsi*0x1]
    lea rax,[r15 + -0x1]
    and r8d,0x1
    imul rax,r9
    add rax,rdx
    mov al,byte ptr [rax + rsi*0x1]
    and eax,0x1
    add eax,ecx
    add r8d,eax
    .lbl18000eb9a: lea eax,[r14 + r13*0x1]
    lea r14d,[r15 + -0x1]
    xor ebp,ebp
    add r8d,eax
    lea ecx,[r14 + -0x1]
    mov eax,0x1
    xor r12d,r12d
    cmp ecx,0x2
    mov r13,rax
    jl .lbl18000ec2d
    lea rax,[rsi + rdi*0x2]
    lea r10,[rdi + rdi*0x1]
    lea r11,[rdi + rsi*0x1]
    lea rbx,[rdi + rax*0x1 + -0x1]
    lea eax,[rcx + -0x2]
    lea rdx,[r10 + rsi*0x1]
    shr eax,1
    add eax,r13d
    mov edi,eax
    lea eax,[rax + rax*0x1 + 0x1]
    mov qword ptr [rsp + 0x88],rax
    lea r13,[rdi + rdi*0x1 + 0x1]
    nop
    nop
    .lbl18000ebf0: mov al,byte ptr [r11]
    mov cl,byte ptr [rdx + -0x1]
    add r11,r10
    and eax,0x1
    and ecx,0x1
    add eax,ebp
    lea ebp,[rax + rcx*0x1]
    mov al,byte ptr [rbx]
    mov cl,byte ptr [rdx]
    and eax,0x1
    and ecx,0x1
    add rbx,r10
    add eax,r12d
    add rdx,r10
    sub rdi,0x1
    lea r12d,[rax + rcx*0x1]
    jnz .lbl18000ebf0
    mov rax,qword ptr [rsp + 0x88]
    mov ebx,dword ptr [rsp + 0x78]
    .lbl18000ec2d: cmp eax,r14d
    jge .lbl18000ec52
    lea rax,[r13 + 0x1]
    imul rax,r9
    imul r9,r13
    mov cl,byte ptr [r9 + rsi*0x1]
    mov dl,byte ptr [rax + rsi*0x1 + -0x1]
    and ecx,0x1
    and edx,0x1
    add r8d,ecx
    add r8d,edx
    .lbl18000ec52: lea ecx,[r12 + rbp*0x1]
    add r8d,ecx
    test ebx,ebx
    jnz .lbl18000ec61
    xor eax,eax
    jmp .lbl18000ec7a
    .lbl18000ec61: imul r8d,dword ptr [rsp + 0x80]
    mov ecx,0x7fff
    mov eax,r8d
    cdq
    idiv ebx
    cmp eax,ecx
    cmovg eax,ecx
    .lbl18000ec7a: mov rbx,qword ptr [rsp + 0x70]
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
EXTR_FUN_18000d350:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    mov qword ptr [rsp + 0x18],rdi
    xor r10d,r10d
    mov edi,edx
    mov rbx,rcx
    mov ecx,r10d
    mov edx,r10d
    mov esi,r10d
    imul edi,r8d
    cmp edi,0x2
    mov r11,r10
    jl .lbl18000d3b3
    lea eax,[rdi + -0x2]
    shr eax,1
    add eax,0x1
    mov r8d,eax
    lea esi,[rax + rax*0x1]
    nop
    nop
    .lbl18000d390: test byte ptr [rbx + r11*0x1],r9b
    mov eax,r10d
    setnz al
    add ecx,eax
    test byte ptr [rbx + r11*0x1 + 0x1],r9b
    mov eax,r10d
    setnz al
    add r11,0x2
    add edx,eax
    sub r8,0x1
    jnz .lbl18000d390
    .lbl18000d3b3: cmp esi,edi
    jge .lbl18000d3bf
    test byte ptr [r11 + rbx*0x1],r9b
    setnz r10b
    .lbl18000d3bf: mov rbx,qword ptr [rsp + 0x8]
    mov rsi,qword ptr [rsp + 0x10]
    mov rdi,qword ptr [rsp + 0x18]
    lea eax,[rdx + rcx*0x1]
    add eax,r10d
    ret

section .data


section .text
EXTR_FUN_18000f4a0:
    mov r11,rsp
    mov qword ptr [r11 + 0x8],rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x2b0
    mov rax,qword ptr [glb180161a18]
    xor rax,rsp
    mov qword ptr [rsp + 0x2a0],rax
    lea r15d,[rdx + -0x2]
    lea r12d,[r8 + -0x2]
    mov rbx,r9
    mov r9,qword ptr [rsp + 0x318]
    mov eax,r15d
    mov r13,rcx
    mov qword ptr [rsp + 0x88],rbx
    mov dword ptr [rsp + 0x28],r12d
    mov dword ptr [rsp + 0x24],r15d
    imul eax,r12d
    cdqe
    mov qword ptr [rsp + 0x80],r9
    mov word ptr [rsp + 0xa0],0x1
    lea rcx,[r9 + rax*0x2]
    mov word ptr [rsp + 0xa2],0x2
    mov word ptr [rsp + 0xa4],0x3
    mov word ptr [rsp + 0xa6],0x4
    mov word ptr [rsp + 0xa8],0x5
    mov word ptr [rsp + 0xaa],0x0
    mov qword ptr [rsp + 0x78],rcx
    mov word ptr [rsp + 0xac],0x6
    mov word ptr [rsp + 0xae],0x7
    mov word ptr [rsp + 0xb0],0x8
    mov word ptr [rsp + 0xb2],0x0
    mov word ptr [rsp + 0xb4],0x0
    mov word ptr [rsp + 0xb6],0x0
    mov word ptr [rsp + 0xb8],0x9
    mov word ptr [rsp + 0xba],0x0
    mov word ptr [rsp + 0xbc],0xa
    mov word ptr [rsp + 0xbe],0xb
    mov word ptr [rsp + 0xc0],0xc
    mov word ptr [rsp + 0xc2],0x0
    mov word ptr [rsp + 0xc4],0x0
    mov word ptr [rsp + 0xc6],0x0
    mov word ptr [rsp + 0xc8],0x0
    mov word ptr [rsp + 0xca],0x0
    mov word ptr [rsp + 0xcc],0x0
    mov word ptr [rsp + 0xce],0x0
    mov word ptr [rsp + 0xd0],0xd
    mov word ptr [rsp + 0xd2],0x0
    mov word ptr [rsp + 0xd4],0x0
    mov word ptr [rsp + 0xd6],0x0
    mov word ptr [rsp + 0xd8],0xe
    mov word ptr [rsp + 0xda],0x0
    mov word ptr [rsp + 0xdc],0xf
    mov word ptr [rsp + 0xde],0x10
    mov word ptr [rsp + 0xe0],0x11
    mov word ptr [rsp + 0xe2],0x0
    mov word ptr [rsp + 0xe4],0x0
    mov word ptr [rsp + 0xe6],0x0
    mov word ptr [rsp + 0xe8],0x0
    mov word ptr [rsp + 0xea],0x0
    mov word ptr [rsp + 0xec],0x0
    mov word ptr [rsp + 0xee],0x0
    mov word ptr [rsp + 0xf0],0x0
    mov word ptr [rsp + 0xf2],0x0
    mov word ptr [rsp + 0xf4],0x0
    mov word ptr [rsp + 0xf6],0x0
    mov word ptr [rsp + 0xf8],0x0
    mov word ptr [rsp + 0xfa],0x0
    mov word ptr [rsp + 0xfc],0x0
    mov word ptr [rsp + 0xfe],0x0
    mov word ptr [rsp + 0x100],0x12
    mov word ptr [rsp + 0x102],0x0
    mov word ptr [rsp + 0x104],0x0
    mov word ptr [rsp + 0x106],0x0
    mov word ptr [rsp + 0x108],0x0
    mov word ptr [rsp + 0x10a],0x0
    mov word ptr [rsp + 0x10c],0x0
    mov word ptr [rsp + 0x10e],0x0
    mov word ptr [rsp + 0x110],0x13
    mov word ptr [rsp + 0x112],0x0
    mov word ptr [rsp + 0x114],0x0
    mov word ptr [rsp + 0x116],0x0
    mov word ptr [rsp + 0x118],0x14
    mov word ptr [rsp + 0x11a],0x0
    mov word ptr [rsp + 0x11c],0x15
    mov word ptr [rsp + 0x11e],0x16
    mov word ptr [rsp + 0x120],0x17
    mov word ptr [rsp + 0x122],0x0
    mov word ptr [rsp + 0x124],0x0
    mov word ptr [rsp + 0x126],0x0
    mov word ptr [rsp + 0x128],0x0
    mov word ptr [rsp + 0x12a],0x0
    mov word ptr [rsp + 0x12c],0x0
    mov word ptr [rsp + 0x12e],0x0
    mov word ptr [rsp + 0x130],0x0
    mov word ptr [rsp + 0x132],0x0
    mov word ptr [rsp + 0x134],0x0
    mov word ptr [rsp + 0x136],0x0
    mov word ptr [rsp + 0x138],0x0
    mov word ptr [rsp + 0x13a],0x0
    mov word ptr [rsp + 0x13c],0x0
    mov word ptr [rsp + 0x13e],0x0
    mov word ptr [rsp + 0x140],0x0
    mov word ptr [rsp + 0x142],0x0
    mov word ptr [rsp + 0x144],0x0
    mov word ptr [rsp + 0x146],0x0
    mov word ptr [rsp + 0x148],0x0
    mov word ptr [rsp + 0x14a],0x0
    mov word ptr [rsp + 0x14c],0x0
    mov word ptr [rsp + 0x14e],0x0
    mov word ptr [rsp + 0x150],0x0
    mov word ptr [rsp + 0x152],0x0
    mov word ptr [rsp + 0x154],0x0
    mov word ptr [rsp + 0x156],0x0
    mov word ptr [rsp + 0x158],0x0
    mov word ptr [rsp + 0x15a],0x0
    mov word ptr [rsp + 0x15c],0x0
    mov word ptr [rsp + 0x15e],0x0
    mov word ptr [rsp + 0x160],0x18
    mov word ptr [rsp + 0x162],0x0
    mov word ptr [rsp + 0x164],0x0
    mov word ptr [rsp + 0x166],0x0
    mov word ptr [rsp + 0x168],0x0
    mov word ptr [rsp + 0x16a],0x0
    mov word ptr [rsp + 0x16c],0x0
    mov word ptr [rsp + 0x16e],0x0
    mov word ptr [rsp + 0x170],0x0
    mov word ptr [rsp + 0x172],0x0
    mov word ptr [rsp + 0x174],0x0
    mov word ptr [rsp + 0x176],0x0
    mov word ptr [rsp + 0x178],0x0
    mov word ptr [rsp + 0x17a],0x0
    mov word ptr [rsp + 0x17c],0x0
    mov word ptr [rsp + 0x17e],0x0
    mov word ptr [rsp + 0x180],0x19
    mov word ptr [rsp + 0x182],0x0
    mov word ptr [rsp + 0x184],0x0
    mov word ptr [rsp + 0x186],0x0
    mov word ptr [rsp + 0x188],0x0
    mov word ptr [rsp + 0x18a],0x0
    mov word ptr [rsp + 0x18c],0x0
    mov word ptr [rsp + 0x18e],0x0
    mov word ptr [rsp + 0x190],0x1a
    mov word ptr [rsp + 0x192],0x0
    mov word ptr [rsp + 0x194],0x0
    mov word ptr [rsp + 0x196],0x0
    mov word ptr [rsp + 0x198],0x1b
    mov word ptr [rsp + 0x19a],0x0
    mov word ptr [rsp + 0x19c],0x1c
    mov word ptr [rsp + 0x19e],0x1d
    mov word ptr [rsp + 0x1a0],0x1e
    mov word ptr [rsp + 0x1a2],0x1f
    mov word ptr [rsp + 0x1a4],0x0
    mov word ptr [rsp + 0x1a6],0x20
    mov word ptr [rsp + 0x1a8],0x0
    mov word ptr [rsp + 0x1aa],0x0
    mov word ptr [rsp + 0x1ac],0x0
    mov word ptr [rsp + 0x1ae],0x21
    mov word ptr [rsp + 0x1b0],0x0
    mov word ptr [rsp + 0x1b2],0x0
    mov word ptr [rsp + 0x1b4],0x0
    mov word ptr [rsp + 0x1b6],0x0
    mov word ptr [rsp + 0x1b8],0x0
    mov word ptr [rsp + 0x1ba],0x0
    mov word ptr [rsp + 0x1bc],0x0
    mov word ptr [rsp + 0x1be],0x22
    mov word ptr [rsp + 0x1c0],0x0
    mov word ptr [rsp + 0x1c2],0x0
    mov word ptr [rsp + 0x1c4],0x0
    mov word ptr [rsp + 0x1c6],0x0
    mov word ptr [rsp + 0x1c8],0x0
    mov word ptr [rsp + 0x1ca],0x0
    mov word ptr [rsp + 0x1cc],0x0
    mov word ptr [rsp + 0x1ce],0x0
    mov word ptr [rsp + 0x1d0],0x0
    mov word ptr [rsp + 0x1d2],0x0
    mov word ptr [rsp + 0x1d4],0x0
    mov word ptr [rsp + 0x1d6],0x0
    mov word ptr [rsp + 0x1d8],0x0
    mov word ptr [rsp + 0x1da],0x0
    mov word ptr [rsp + 0x1dc],0x0
    mov word ptr [rsp + 0x1de],0x23
    mov word ptr [rsp + 0x1e0],0x0
    mov word ptr [rsp + 0x1e2],0x0
    mov word ptr [rsp + 0x1e4],0x0
    mov word ptr [rsp + 0x1e6],0x0
    mov word ptr [rsp + 0x1e8],0x0
    mov word ptr [rsp + 0x1ea],0x0
    mov word ptr [rsp + 0x1ec],0x0
    mov word ptr [rsp + 0x1ee],0x0
    mov word ptr [rsp + 0x1f0],0x0
    mov word ptr [rsp + 0x1f2],0x0
    mov word ptr [rsp + 0x1f4],0x0
    mov word ptr [rsp + 0x1f6],0x0
    mov word ptr [rsp + 0x1f8],0x0
    mov word ptr [rsp + 0x1fa],0x0
    mov word ptr [rsp + 0x1fc],0x0
    mov word ptr [rsp + 0x1fe],0x0
    mov word ptr [rsp + 0x200],0x0
    mov word ptr [rsp + 0x202],0x0
    mov word ptr [rsp + 0x204],0x0
    mov word ptr [rsp + 0x206],0x0
    mov word ptr [rsp + 0x208],0x0
    mov word ptr [rsp + 0x20a],0x0
    mov word ptr [rsp + 0x20c],0x0
    mov word ptr [rsp + 0x20e],0x0
    mov word ptr [rsp + 0x210],0x0
    mov word ptr [rsp + 0x212],0x0
    mov word ptr [rsp + 0x214],0x0
    mov word ptr [rsp + 0x216],0x0
    mov word ptr [rsp + 0x218],0x0
    mov word ptr [rsp + 0x21a],0x0
    mov word ptr [rsp + 0x21c],0x0
    mov word ptr [rsp + 0x21e],0x24
    mov word ptr [rsp + 0x220],0x25
    mov word ptr [rsp + 0x222],0x26
    mov word ptr [rsp + 0x224],0x0
    mov word ptr [rsp + 0x226],0x27
    mov word ptr [rsp + 0x228],0x0
    mov word ptr [rsp + 0x22a],0x0
    mov word ptr [rsp + 0x22c],0x0
    mov word ptr [rsp + 0x22e],0x28
    mov word ptr [rsp + 0x230],0x0
    mov word ptr [rsp + 0x232],0x0
    mov word ptr [rsp + 0x234],0x0
    mov word ptr [rsp + 0x236],0x0
    mov word ptr [rsp + 0x238],0x0
    mov word ptr [rsp + 0x23a],0x0
    mov word ptr [rsp + 0x23c],0x0
    mov word ptr [rsp + 0x23e],0x29
    mov word ptr [rsp + 0x240],0x0
    mov word ptr [rsp + 0x242],0x0
    mov word ptr [rsp + 0x244],0x0
    mov word ptr [rsp + 0x246],0x0
    mov word ptr [rsp + 0x248],0x0
    mov word ptr [rsp + 0x24a],0x0
    mov word ptr [rsp + 0x24c],0x0
    mov word ptr [rsp + 0x24e],0x0
    mov word ptr [rsp + 0x250],0x0
    mov word ptr [rsp + 0x252],0x0
    mov word ptr [rsp + 0x254],0x0
    mov word ptr [rsp + 0x256],0x0
    mov word ptr [rsp + 0x258],0x0
    mov word ptr [rsp + 0x25a],0x0
    mov word ptr [rsp + 0x25c],0x0
    mov word ptr [rsp + 0x25e],0x2a
    mov word ptr [rsp + 0x260],0x2b
    mov word ptr [rsp + 0x262],0x2c
    mov word ptr [rsp + 0x264],0x0
    mov word ptr [rsp + 0x266],0x2d
    mov word ptr [r11 + -0x80],0x0
    mov eax,dword ptr [rsp + 0x310]
    xor esi,esi
    mov word ptr [r11 + -0x7e],0x0
    test eax,eax
    mov word ptr [r11 + -0x7c],0x0
    mov word ptr [r11 + -0x7a],0x2e
    mov word ptr [r11 + -0x78],0x0
    mov word ptr [r11 + -0x76],0x0
    mov word ptr [r11 + -0x74],0x0
    mov word ptr [r11 + -0x72],0x0
    mov word ptr [r11 + -0x70],0x0
    mov word ptr [r11 + -0x6e],0x0
    mov word ptr [r11 + -0x6c],0x0
    mov word ptr [r11 + -0x6a],0x2f
    mov word ptr [r11 + -0x68],0x30
    mov word ptr [r11 + -0x66],0x31
    mov word ptr [r11 + -0x64],0x0
    mov word ptr [r11 + -0x62],0x32
    mov word ptr [r11 + -0x60],0x0
    mov word ptr [r11 + -0x5e],0x0
    mov word ptr [r11 + -0x5c],0x0
    mov word ptr [r11 + -0x5a],0x33
    mov word ptr [r11 + -0x58],0x34
    mov word ptr [r11 + -0x56],0x35
    mov word ptr [r11 + -0x54],0x0
    mov word ptr [r11 + -0x52],0x36
    mov word ptr [r11 + -0x50],0x37
    mov word ptr [r11 + -0x4e],0x38
    mov word ptr [r11 + -0x4c],0x39
    mov word ptr [r11 + -0x4a],0x3a
    mov r10d,0x100
    mov r11d,0x3b
    mov r14,r9
    cmovnz r10d,r11d
    test eax,eax
    lea eax,[rdx + -0x1]
    cmovnz r14,rcx
    mov edi,0x1
    mov dword ptr [rsp + 0x58],0xffffffff
    cmp eax,edi
    mov dword ptr [rsp + 0x5c],0xffffffff
    mov dword ptr [rsp + 0x60],0xffffffff
    mov dword ptr [rsp + 0x64],esi
    mov dword ptr [rsp + 0x68],esi
    mov dword ptr [rsp + 0x6c],0x1
    mov dword ptr [rsp + 0x70],0x1
    mov dword ptr [rsp + 0x74],0x1
    mov dword ptr [rsp + 0x38],0xffffffff
    mov dword ptr [rsp + 0x3c],esi
    mov dword ptr [rsp + 0x40],0x1
    mov dword ptr [rsp + 0x44],0xffffffff
    mov dword ptr [rsp + 0x48],0x1
    mov dword ptr [rsp + 0x4c],0xffffffff
    mov dword ptr [rsp + 0x50],esi
    mov dword ptr [rsp + 0x54],0x1
    mov dword ptr [rsp + 0x20],r10d
    mov qword ptr [rsp + 0x90],r14
    movsxd rbp,r8d
    jle .lbl180010086
    mov rbx,qword ptr [rsp + 0x78]
    lea rcx,[rbp + rbp*0x1]
    lea edx,[r8 + -0x1]
    add eax,-0x1
    mov dword ptr [rsp + 0x2c],edx
    lea r15,[rcx + r13*0x1 + 0x2]
    mov qword ptr [rsp + 0x30],rax
    nop
    .lbl18000ff90: mov r10d,0x1
    cmp edx,r10d
    jle .lbl180010051
    lea rcx,[rbp + -0x2]
    lea rax,[rdi + -0x1]
    mov r12,rbx
    mov r11,r15
    lea r14d,[rdx + -0x1]
    imul rcx,rax
    lea r8,[r9 + rcx*0x2]
    sub r12,r9
    nop
    .lbl18000ffc0: mov word ptr [r8],si
    mov r9d,0x7
    mov rdx,rsi
    mov word ptr [r12 + r8*0x1],si
    .lbl18000ffd2: movsxd rax,dword ptr [rsp + rdx*0x1 + 0x58]
    movsxd rcx,dword ptr [rsp + rdx*0x1 + 0x38]
    add rax,rdi
    imul rax,rbp
    add rax,r10
    add rcx,rax
    movzx eax,word ptr [r11]
    cmp word ptr [r13 + rcx*0x2],ax
    jc .lbl180010002
    mov ecx,r9d
    mov eax,0x1
    shl ax,cl
    jmp .lbl180010005
    .lbl180010002: xor ax,ax
    .lbl180010005: add word ptr [r8],ax
    add rdx,0x4
    sub r9d,0x1
    cmp rdx,0x20
    jl .lbl18000ffd2
    movzx eax,word ptr [r8]
    add r8,0x2
    add r10,0x1
    movzx ecx,word ptr [rsp + rax*0x2 + 0xa0]
    add r11,0x2
    sub r14,0x1
    mov word ptr [r12 + r8*0x1 + -0x2],cx
    jnz .lbl18000ffc0
    mov rax,qword ptr [rsp + 0x30]
    mov edx,dword ptr [rsp + 0x2c]
    mov r9,qword ptr [rsp + 0x80]
    lea rcx,[rbp + rbp*0x1]
    .lbl180010051: add rdi,0x1
    add r15,rcx
    sub rax,0x1
    mov qword ptr [rsp + 0x30],rax
    jnz .lbl18000ff90
    mov rbx,qword ptr [rsp + 0x88]
    mov r12d,dword ptr [rsp + 0x28]
    mov r14,qword ptr [rsp + 0x90]
    mov r15d,dword ptr [rsp + 0x24]
    mov r10d,dword ptr [rsp + 0x20]
    .lbl180010086: test r10d,r10d
    movsxd r8,r10d
    jle .lbl18001009b
    add r8,r8
    xor edx,edx
    mov rcx,rbx
    call EXTR_memset wrt ..plt
    .lbl18001009b: test r15d,r15d
    jle .lbl1800100dc
    mov r9d,r15d
    .lbl1800100a3: test r12d,r12d
    jle .lbl1800100d2
    lea rax,[rbp + -0x2]
    mov r8d,r12d
    imul rax,rsi
    lea rdx,[r14 + rax*0x2]
    nop
    nop
    nop
    .lbl1800100c0: movzx eax,word ptr [rdx]
    add rdx,0x2
    add word ptr [rbx + rax*0x2],0x1
    sub r8,0x1
    jnz .lbl1800100c0
    .lbl1800100d2: add rsi,0x1
    sub r9,0x1
    jnz .lbl1800100a3
    .lbl1800100dc: mov rcx,qword ptr [rsp + 0x2a0]
    xor rcx,rsp
    call EXTR_check_cookie
    mov rbx,qword ptr [rsp + 0x2f0]
    add rsp,0x2b0
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
EXTR_FUN_18000e820:
    mov r11,rsp
    mov qword ptr [r11 + 0x8],rbx
    mov qword ptr [r11 + 0x10],rbp
    mov qword ptr [r11 + 0x18],rsi
    mov qword ptr [r11 + 0x20],r9
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x50
    mov rdi,qword ptr [rsp + 0xa0]
    mov r13d,0x1
    lea esi,[r8 + 0x2]
    lea ebp,[rdx + 0x2]
    xor r12d,r12d
    mov eax,esi
    mov dword ptr [r11 + -0x30],r12d
    mov byte ptr [r11 + -0x38],r12b
    mov dword ptr [r11 + -0x40],r13d
    imul eax,ebp
    movsxd r10,eax
    mov dword ptr [r11 + -0x48],r13d
    mov r9d,r8d
    lea r15,[rdi + r10*0x2]
    mov r8d,edx
    mov rdx,rcx
    lea rbx,[r15 + r10*0x2]
    mov dword ptr [r11 + -0x50],r13d
    mov r14d,r12d
    mov rcx,rbx
    mov dword ptr [r11 + -0x58],r13d
    call EXTR_FUN_180003d50
    mov r9,rdi
    mov r8d,esi
    mov edx,ebp
    mov rcx,rbx
    mov byte ptr [rsp + 0x28],0x30
    mov qword ptr [rsp + 0x20],r15
    call EXTR_FUN_180012d30
    mov r8d,esi
    movsxd rbx,eax
    mov edx,ebp
    mov rcx,rdi
    mov r9d,ebx
    mov qword ptr [rsp + 0x20],r15
    call EXTR_FUN_1800103d0
    cmp r13,rbx
    jg .lbl18000e8ee
    nop
    nop
    .lbl18000e8d0: movsx eax,word ptr [r15 + r13*0x2]
    cmp eax,r12d
    cmovg r12d,eax
    cmp eax,0xa
    jl .lbl18000e8e5
    add r14d,0x1
    .lbl18000e8e5: add r13,0x1
    cmp r13,rbx
    jle .lbl18000e8d0
    .lbl18000e8ee: mov rax,qword ptr [rsp + 0x98]
    lea r11,[rsp + 0x50]
    mov rbx,qword ptr [r11 + 0x30]
    mov rbp,qword ptr [r11 + 0x38]
    mov rsi,qword ptr [r11 + 0x40]
    mov dword ptr [rax],r14d
    mov dword ptr [rax + 0x4],r12d
    xor eax,eax
    mov rsp,r11
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
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
EXTR_tudorIplGetBufSize:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x20
    movzx esi,word ptr [rcx + 0x2c]
    movzx eax,word ptr [rcx + 0x2a]
    mov rbp,rcx
    add rcx,0x18
    imul esi,eax
    call EXTR_calc_img_bit_count
    lea rcx,[rbp + 0x18]
    mov ebx,eax
    call EXTR_calc_sth_img_size
    cmp ebx,eax
    mov edi,eax
    cmova edi,ebx
    cmp word ptr [rbp + 0x40],0x0
    jbe .lbl180001efa
    lea rcx,[rbp + 0x18]
    call EXTR_FUN_1800067b0
    cmp edi,eax
    cmova eax,edi
    mov edi,eax
    .lbl180001efa: movzx ecx,word ptr [rbp + 0x40]
    mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    lea eax,[rdi + rsi*0x1]
    mov rdi,qword ptr [rsp + 0x48]
    lea eax,[rax + rsi*0x2]
    mov rsi,qword ptr [rsp + 0x40]
    lea eax,[rax + rcx*0x4]
    add rsp,0x20
    pop r12
    ret

section .data


section .text
EXTR_calc_img_bit_count:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    sub rsp,0x20
    movzx eax,word ptr [rcx + 0x16]
    movzx r12d,word ptr [rcx + 0x14]
    movzx ebp,word ptr [rcx + 0x12]
    cmp ax,0x6
    mov rsi,rcx
    jz .lbl18000bc8a
    cmp ax,0x7
    jz .lbl18000bc8a
    xor edi,edi
    jmp .lbl18000bc94
    .lbl18000bc8a: mov edi,ebp
    imul edi,r12d
    add edi,edi
    add edi,edi
    .lbl18000bc94: mov ecx,ebp
    call EXTR_bytes2bits_aligned
    movzx r8d,word ptr [rsi + 0x1c]
    mov edx,ebp
    mov ebx,eax
    cmp edi,eax
    mov ecx,r12d
    cmova ebx,edi
    call EXTR_get_img_bits
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    mov rdi,qword ptr [rsp + 0x48]
    cmp ebx,eax
    cmova eax,ebx
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop r12
    ret

section .data


section .text
EXTR_get_img_bits:
    .lbl180011a50: imul ecx,edx
    lea eax,[rcx + rcx*0x8]
    cdq
    and edx,0x3
    add eax,edx
    sar eax,0x2
    ret
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x20
    cmp r8d,0x8
    mov ebx,edx
    mov edi,ecx
    ja .lbl180012cf5
    lea rcx,[EXTR_get_img_bits]
    movsxd rax,r8d
    mov r8d,dword ptr [rcx + rax*0x4 + (.jt180012c2d - EXTR_get_img_bits)]
    add r8,rcx
    jmp r8
    .lbl180012c30: mov ecx,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    jmp .lbl180011a50
    .lbl180012c41: xor eax,eax
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret
    .lbl180012c4e: mov ecx,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    jmp EXTR_calc_raw_bits
    .lbl180012c5f: mov ecx,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    jmp EXTR_calc_raw_bits
    .lbl180012c70: mov ecx,edi
    call EXTR_calc_img_bits
    mov edx,ebx
    mov ecx,edi
    mov r11d,eax
    call EXTR_calc_raw_bits
    mov ecx,edi
    mov ebx,eax
    cmp r11d,eax
    cmova ebx,r11d
    call EXTR_calc_pallete_bits
    cmp ebx,eax
    cmova eax,ebx
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret
    .lbl180012ca3: mov ecx,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    jmp EXTR_FUN_180011a00
    .lbl180012cb4: mov ecx,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    jmp EXTR_FUN_1800112b0
    .lbl180012cc5: mov ecx,edi
    call EXTR_FUN_180012840
    imul edi,ebx
    lea eax,[rax + rdi*0x2]
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret
    .lbl180012cdd: mov ecx,edi
    call EXTR_FUN_1800128c0
    imul edi,ebx
    lea eax,[rax + rdi*0x2]
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret
    .lbl180012cf5: mov eax,0x1
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret

section .data
    .jt180012c2d dq (.lbl180012c30 - EXTR_get_img_bits), (.lbl180012c41 - EXTR_get_img_bits), (.lbl180012c4e - EXTR_get_img_bits), (.lbl180012c5f - EXTR_get_img_bits), (.lbl180012c70 - EXTR_get_img_bits), (.lbl180012ca3 - EXTR_get_img_bits), (.lbl180012cb4 - EXTR_get_img_bits), (.lbl180012cc5 - EXTR_get_img_bits), (.lbl180012cdd - EXTR_get_img_bits), (.lbl180012cf5 - EXTR_get_img_bits)


section .text
EXTR_calc_img_bits:
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x20
    mov eax,ecx
    imul eax,edx
    lea ebx,[rax + rax*0x1]
    lea edi,[rbx*0x4]
    call EXTR_calc_pallete_bits
    add eax,ebx
    mov rbx,qword ptr [rsp + 0x30]
    cmp edi,eax
    cmovg eax,edi
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_calc_pallete_bits:
    lea eax,[rcx*0x4]
    shl rax,0x3
    ret

section .data


section .text
EXTR_FUN_1800112b0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    mov ebx,ecx
    mov esi,edx
    mov ebp,ecx
    imul ebx,edx
    lea edi,[rbx + rbx*0x8]
    call EXTR_calc_pallete_bits
    mov edx,esi
    lea r8d,[rax + rbx*0x4]
    mov ecx,ebp
    cmp edi,r8d
    cmovg r8d,edi
    call EXTR_FUN_1800103c0
    mov rbx,qword ptr [rsp + 0x30]
    mov rbp,qword ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x40]
    add eax,0x1
    cmp r8d,eax
    cmovg eax,r8d
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_1800103c0:
    imul ecx,edx
    mov eax,ecx
    add eax,eax
    add rax,rax
    ret

section .data


section .text
EXTR_FUN_180011a00:
    push rbx
    sub rsp,0x20
    mov r8d,ecx
    mov r10d,edx
    mov r11d,ecx
    imul r8d,edx
    call EXTR_FUN_1800119f0
    mov edx,r10d
    mov r9d,eax
    cmp r8d,eax
    mov ecx,r11d
    cmovg r9d,r8d
    call EXTR_FUN_1800103c0
    mov ecx,r11d
    mov ebx,eax
    cmp r9d,eax
    cmovg ebx,r9d
    call EXTR_calc_pallete_bits
    cmp ebx,eax
    cmovg eax,ebx
    add rsp,0x20
    pop rbx
    ret

section .data


section .text
EXTR_FUN_1800119f0:
    sar ecx,1
    sar edx,1
    imul ecx,edx
    lea eax,[rcx + rcx*0x8]
    ret

section .data


section .text
EXTR_FUN_1800128c0:
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x20
    mov edi,edx
    mov ebx,ecx
    call EXTR_calc_pallete_bits
    mov edx,edi
    mov ecx,ebx
    mov r11d,eax
    call EXTR_FUN_180010c60
    mov edx,ebx
    cmp r11d,eax
    cmovg eax,r11d
    imul edx,edi
    lea ecx,[rdx + rdx*0x2]
    shl ecx,0x2
    cmp eax,ecx
    cmovg ecx,eax
    cmp ebx,edi
    cmovg edi,ebx
    mov rbx,qword ptr [rsp + 0x30]
    shl edi,0x4
    cmp ecx,edi
    cmovg edi,ecx
    mov eax,edi
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180010c60:
    imul ecx,edx
    mov eax,ecx
    shl eax,0x2
    add rax,rax
    ret

section .data


section .text
EXTR_FUN_180012840:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    mov ebp,ecx
    mov edi,edx
    mov esi,ecx
    imul ebp,edx
    lea ebx,[rbp + rbp*0x2]
    shl ebx,0x2
    call EXTR_FUN_180012410
    mov edx,edi
    mov r11d,eax
    cmp ebx,eax
    mov ecx,esi
    cmovg r11d,ebx
    call EXTR_FUN_1800103c0
    mov ecx,esi
    mov ebx,eax
    cmp r11d,eax
    cmovg ebx,r11d
    call EXTR_calc_pallete_bits
    mov edx,edi
    cmp ebx,eax
    mov ecx,esi
    cmovg eax,ebx
    cmp eax,ebp
    cmovg ebp,eax
    call EXTR_FUN_1800103c0
    mov rbx,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x40]
    cmp ebp,eax
    cmovg eax,ebp
    mov rbp,qword ptr [rsp + 0x38]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180012410:
    .lbl1800036d0: mov eax,ecx
    shl rax,0x2
    ret
    mov ecx,0x3e8
    jmp .lbl1800036d0

section .data


section .text
EXTR_calc_raw_bits:
    imul ecx,edx
    mov eax,ecx
    add eax,eax
    shl rax,0x2
    ret

section .data


section .text
EXTR_bytes2bits_aligned:
    sub rsp,0x28
    call EXTR_log2ceil
    mov ecx,eax
    mov eax,0x1
    shl eax,cl
    shl rax,0x3
    add rsp,0x28
    ret

section .data


section .text
EXTR_calc_sth_img_size:
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x20
    mov rbx,rcx
    xor edi,edi
    call EXTR_contains_25
    test eax,eax
    jz .lbl18000c0d7
    cmp word ptr [rbx + 0x12e],di
    movzx edx,word ptr [rbx + 0x12]
    movzx ecx,word ptr [rbx + 0x14]
    jz .lbl18000c0d0
    call EXTR_FUN_180016ab0
    jmp .lbl18000c0d5
    .lbl18000c0d0: call EXTR_FUN_1800169e0
    .lbl18000c0d5: mov edi,eax
    .lbl18000c0d7: test byte ptr [rbx + 0xc],0x20
    jz .lbl18000c10c
    movzx ecx,word ptr [rbx + 0x18]
    mov eax,0x3e0f83e1
    imul ecx
    movzx ecx,word ptr [rbx + 0x14]
    mov r8d,edx
    movzx edx,word ptr [rbx + 0x12]
    sar r8d,0x3
    mov eax,r8d
    shr eax,0x1f
    add r8d,eax
    call EXTR_get_squarified_bits
    cmp edi,eax
    cmova eax,edi
    mov edi,eax
    .lbl18000c10c: test byte ptr [rbx + 0xc],0x10
    jz .lbl18000c12f
    movzx ecx,word ptr [rbx + 0x26]
    mov edx,ecx
    neg edx
    call EXTR_get_ranged_array_bits
    cmp edi,eax
    cmova eax,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret
    .lbl18000c12f: mov eax,edi
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180016ab0:
    mov eax,edx
    mov edx,ecx
    mov ecx,eax
    jmp EXTR_FUN_180018a00

section .data


section .text
EXTR_FUN_180018a00:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x20
    mov ebx,ecx
    mov ecx,edx
    call EXTR_log2ceil
    mov esi,0x1
    mov ecx,eax
    mov edi,esi
    shl edi,cl
    mov ecx,ebx
    call EXTR_log2ceil
    mov r8d,0x4
    mov edx,edi
    mov ecx,eax
    shl esi,cl
    mov ecx,esi
    call EXTR_FUN_180017500
    mov rbx,qword ptr [rsp + 0x30]
    lea ecx,[rdi + rdi*0x1 + 0x1]
    imul ecx,esi
    mov rsi,qword ptr [rsp + 0x38]
    lea edx,[rcx + rdi*0x1 + 0x1]
    mov r8d,eax
    lea rax,[r8 + rdx*0x4]
    add rsp,0x20
    pop rdi
    ret

section .data


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
EXTR_get_ranged_array_bits:
    sub ecx,edx
    lea eax,[rcx + 0x1]
    shl rax,0x2
    ret

section .data


section .text
EXTR_FUN_1800169e0:
    mov eax,edx
    mov edx,ecx
    mov ecx,eax
    jmp .lbl180019000
    .lbl180019000: jmp EXTR_FUN_180018280

section .data


section .text
EXTR_FUN_180018280:
    sub rsp,0x28
    mov r8d,ecx
    mov ecx,edx
    call EXTR_round_pow2
    mov ecx,r8d
    mov r10d,eax
    call EXTR_round_pow2
    mov r8d,0x4
    mov r9d,eax
    mov edx,r10d
    sar r9d,1
    add r9d,0x1
    mov ecx,r9d
    call EXTR_FUN_180017500
    lea ecx,[r10 + r10*0x1 + 0x1]
    mov r8d,eax
    imul ecx,r9d
    lea edx,[r10 + rcx*0x1]
    lea rax,[r8 + rdx*0x4]
    add rsp,0x28
    ret

section .data


section .text
EXTR_contains_25:
    xor edx,edx
    add rcx,0x2a
    .lbl18000bc26: cmp word ptr [rcx],0x19
    jnz .lbl18000bc3d
    add rdx,0x1
    add rcx,0x2
    cmp rdx,0xd
    jl .lbl18000bc26
    xor eax,eax
    ret
    .lbl18000bc3d: mov eax,0x1
    ret

section .data


section .text
EXTR_get_squarified_bits:
    mov r9d,r8d
    jmp EXTR_get_resized_bits

section .data


section .text
EXTR_get_resized_bits:
    mov qword ptr [rsp + 0x8],rbx
    mov eax,edx
    mov r10d,edx
    mov r11d,ecx
    cdq
    idiv r8d
    mov ebx,eax
    mov eax,ecx
    xor ecx,ecx
    cdq
    mov r8d,ecx
    idiv r9d
    mov r9d,eax
    mov eax,r10d
    cdq
    idiv ebx
    test edx,edx
    setnz r8b
    add r8d,eax
    mov eax,r11d
    cdq
    idiv r9d
    test edx,edx
    setnz cl
    add ecx,eax
    cmp r8d,ecx
    cmovl r8d,ecx
    imul r9d,ebx
    mov rbx,qword ptr [rsp + 0x8]
    imul r8d,r8d
    lea eax,[r8 + r9*0x2]
    add rax,rax
    ret

section .data


section .text
EXTR_FUN_1800067b0:
    cmp word ptr [rcx + 0x12a],0x1
    jz .lbl1800067bf
    jmp EXTR_FUN_180006720
    .lbl1800067bf: movzx edx,word ptr [rcx + 0x86]
    movzx ecx,word ptr [rcx + 0x88]
    jmp EXTR_FUN_1800065c0

section .data


section .text
EXTR_FUN_180006720:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    sub rsp,0x20
    movzx ebp,word ptr [rcx + 0x12]
    movzx r12d,word ptr [rcx + 0x14]
    mov r13,rcx
    mov edx,ebp
    mov ecx,r12d
    call EXTR_FUN_18000d3e0
    mov edx,ebp
    mov ecx,r12d
    mov ebx,eax
    call EXTR_FUN_18000d590
    mov edx,ebp
    mov edi,eax
    cmp ebx,eax
    mov ecx,r12d
    cmovg edi,ebx
    call EXTR_FUN_18000d590
    mov edx,ebp
    mov esi,eax
    cmp edi,eax
    mov ecx,r12d
    cmovg esi,edi
    call EXTR_FUN_18000da60
    mov rcx,r13
    mov ebx,eax
    cmp esi,eax
    cmovg ebx,esi
    call EXTR_calc_img_bit_count
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    cmp ebx,eax
    cmovg eax,ebx
    mov rbx,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_18000da60:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x20
    mov esi,ecx
    mov ecx,edx
    mov edi,edx
    cmp esi,edx
    cmovg ecx,esi
    call EXTR_calc_pallete_bits
    mov edx,edi
    mov ecx,esi
    mov ebx,eax
    call EXTR_FUN_180005730
    cmp ebx,eax
    mov r8d,eax
    cmovg r8d,ebx
    mov rbx,qword ptr [rsp + 0x30]
    imul esi,edi
    mov ecx,esi
    mov rsi,qword ptr [rsp + 0x38]
    add ecx,ecx
    lea rax,[rcx + rcx*0x4]
    add rax,r8
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180005730:
    cmp ecx,edx
    cmovg edx,ecx
    lea eax,[0x2 + rdx*0x4]
    shl rax,0x2
    ret

section .data


section .text
EXTR_FUN_18000d590:
    mov eax,ecx
    mov r8d,edx
    cdq
    and edx,0x7
    lea ecx,[rdx + rax*0x1]
    mov eax,r8d
    cdq
    sar ecx,0x3
    and edx,0x7
    add eax,edx
    sar eax,0x3
    imul ecx,eax
    lea rax,[rcx + rcx*0x1]
    ret

section .data


section .text
EXTR_FUN_18000d3e0:
    sub rsp,0x28
    mov r10d,ecx
    call EXTR_FUN_18000cb10
    mov ecx,r10d
    mov r9d,eax
    call EXTR_FUN_18000c6f0
    add eax,r9d
    add rsp,0x28
    ret

section .data


section .text
EXTR_FUN_18000cb10:
    mov r8d,ecx
    mov eax,edx
    imul ecx,edx
    sar eax,0x3
    sar r8d,0x3
    imul r8d,eax
    lea eax,[rcx + r8*0x8]
    shl rax,0x2
    ret

section .data


section .text
EXTR_FUN_18000c6f0:
    and ecx,0xfffffffe
    sar edx,1
    mov eax,ecx
    imul eax,edx
    shl rax,0x2
    ret

section .data


section .text
EXTR_FUN_1800065c0:
    sub rsp,0x28
    call EXTR_FUN_1800061b0
    add eax,0x460
    add rsp,0x28
    ret

section .data


section .text
EXTR_FUN_1800061b0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    push r12
    push r13
    push r14
    sub rsp,0x20
    mov esi,edx
    mov ebp,ecx
    mov r12d,ecx
    and esi,0xfffffffc
    and ebp,0xfffffffc
    mov r9d,0xb
    imul r12d,edx
    mov eax,esi
    mov r13d,edx
    mov r14d,ecx
    lea r8d,[r9 + 0xb]
    mov edx,esi
    mov ecx,ebp
    imul eax,ebp
    lea edi,[rax + rax*0x1]
    call EXTR_FUN_180005e50
    mov edx,esi
    mov ecx,ebp
    lea ebx,[rdi + rax*0x1]
    call EXTR_FUN_18000e290
    mov edx,r13d
    mov ecx,r14d
    lea esi,[rdi + rax*0x1]
    lea ebp,[r12 + r12*0x2]
    cmp ebx,esi
    cmovg esi,ebx
    call EXTR_FUN_18000e490
    mov edx,r13d
    lea ebx,[rbp + rax*0x1]
    mov ecx,r14d
    cmp esi,ebx
    cmovg ebx,esi
    call EXTR_FUN_18000e810
    mov edx,r13d
    lea edi,[rbp + rax*0x1]
    mov ecx,r14d
    cmp ebx,edi
    cmovg edi,ebx
    call EXTR_FUN_18000ea80
    mov edx,r13d
    lea ebx,[rbp + rax*0x1]
    mov ecx,r14d
    cmp edi,ebx
    cmovg ebx,edi
    call EXTR_FUN_18000f490
    mov rbp,qword ptr [rsp + 0x48]
    mov rsi,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    add eax,0x200
    cmp ebx,eax
    cmovg eax,ebx
    mov rbx,qword ptr [rsp + 0x40]
    add rsp,0x20
    pop r14
    pop r13
    pop r12
    ret

section .data


section .text
EXTR_FUN_18000e490:
    lea eax,[rcx + 0x4]
    lea ecx,[rdx + 0x4]
    imul eax,ecx
    shl eax,0x2
    ret

section .data


section .text
EXTR_FUN_180005e50:
    push rbx
    sub rsp,0x20
    lea eax,[r9 + r9*0x1]
    lea r11d,[rax + rcx*0x1]
    lea ebx,[rax + rdx*0x1]
    mov edx,ebx
    mov ecx,r11d
    call EXTR_FUN_180005940
    imul ebx,r11d
    lea eax,[rax + rbx*0x4]
    add rsp,0x20
    pop rbx
    ret

section .data


section .text
EXTR_FUN_180005940:
    mov r10d,edx
    mov eax,ecx
    cdq
    imul ecx,r10d
    idiv r8d
    mov r9d,eax
    mov eax,r10d
    cdq
    idiv r8d
    imul r9d,eax
    lea eax,[rcx + r9*0x1]
    shl rax,0x2
    ret

section .data


section .text
EXTR_FUN_18000e290:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x20
    mov esi,edx
    mov ebp,ecx
    call EXTR_FUN_18000d3e0
    mov edx,esi
    mov ecx,ebp
    mov r11d,eax
    call EXTR_FUN_18000d590
    mov edx,esi
    mov ecx,ebp
    mov r9d,eax
    cmp r11d,eax
    cmovg r9d,r11d
    call EXTR_FUN_18000d590
    mov edx,esi
    mov ebx,eax
    cmp r9d,eax
    mov ecx,ebp
    cmovg ebx,r9d
    call EXTR_FUN_18000da60
    mov edx,esi
    mov edi,eax
    cmp ebx,eax
    mov ecx,ebp
    cmovg edi,ebx
    call EXTR_calc_img_bits
    mov rbx,qword ptr [rsp + 0x30]
    imul ebp,esi
    mov rsi,qword ptr [rsp + 0x40]
    add eax,ebp
    mov rbp,qword ptr [rsp + 0x38]
    cmp edi,eax
    cmovg eax,edi
    add rsp,0x20
    pop rdi
    ret

section .data


section .text
EXTR_FUN_18000ea80:
    imul ecx,edx
    lea eax,[rcx + rcx*0x1]
    ret

section .data


section .text
EXTR_FUN_18000e810:
    lea r8d,[rcx + 0x2]
    lea eax,[rdx + 0x2]
    imul r8d,eax
    lea rax,[r8 + r8*0x4]
    ret

section .data


section .text
EXTR_tudorIplAcceptBuf:
    mov qword ptr [rsp + 0x10],rbx
    push rsi
    sub rsp,0x20
    cmp word ptr [rcx + 0x142],0x1
    mov r10,rdx
    mov rsi,rcx
    jz .lbl1800018bb
    movzx r8d,word ptr [rcx + 0x2a]
    movzx edx,word ptr [rcx + 0x2c]
    mov qword ptr [rcx + 0x78],r10
    mov ebx,r8d
    add rcx,0x70
    imul ebx,edx
    call EXTR_FUN_1800016b0
    mov rax,qword ptr [rsi + 0x78]
    movsxd r11,ebx
    lea rcx,[r11 + rax*0x1]
    lea eax,[rbx + rbx*0x2]
    lea r8,[rcx + r11*0x2]
    mov qword ptr [rsi + 0x80],rcx
    movzx ecx,word ptr [rsi + 0x40]
    lea eax,[rax + rcx*0x4]
    jmp .lbl18000193e
    .lbl1800018bb: movzx r8d,word ptr [rcx + 0x9e]
    movzx edx,word ptr [rcx + 0xa0]
    add rcx,0x118
    mov r9d,r8d
    mov qword ptr [rsp + 0x30],rdi
    mov qword ptr [rcx],r10
    imul r9d,edx
    movsxd rbx,r9d
    lea edi,[r9 + r9*0x1]
    lea rax,[rbx + r10*0x1]
    mov qword ptr [rsi + 0x120],rax
    call EXTR_FUN_1800012a0
    mov rax,qword ptr [rsi + 0x120]
    movzx ecx,word ptr [rsi + 0xa0]
    lea r8,[rbx + rax*0x1]
    movzx eax,word ptr [rsi + 0x9e]
    imul ecx,eax
    lea edx,[rdi + rcx*0x2]
    mov rdi,qword ptr [rsp + 0x30]
    movsxd rax,ecx
    movzx ecx,word ptr [rsi + 0xca]
    mov qword ptr [rsi + 0x128],r8
    lea r8,[r8 + rax*0x2]
    lea eax,[rdx + rcx*0x4]
    mov qword ptr [rsi + 0x2f8],0x0
    .lbl18000193e: mov rbx,qword ptr [rsp + 0x38]
    lea rcx,[r8 + rcx*0x4]
    mov qword ptr [rsi + 0x2f0],r8
    mov qword ptr [rsi],rcx
    add rsp,0x20
    pop rsi
    ret

section .data


section .text
EXTR_FUN_1800012a0:
    mov qword ptr [rsp + 0x8],rbx
    push rdi
    sub rsp,0x20
    mov ebx,edx
    mov rdi,rcx
    mov rcx,qword ptr [rcx]
    xor edx,edx
    imul ebx,r8d
    mov r8d,ebx
    call EXTR_memset wrt ..plt
    mov rcx,qword ptr [rdi + 0x8]
    mov r8d,ebx
    xor edx,edx
    mov rbx,qword ptr [rsp + 0x30]
    add rsp,0x20
    pop rdi
    jmp EXTR_memset wrt ..plt

section .data


section .text
EXTR_FUN_1800016b0:
    imul edx,r8d
    mov byte ptr [rcx],0x0
    mov rcx,qword ptr [rcx + 0x8]
    mov r8d,edx
    xor edx,edx
    jmp EXTR_memset wrt ..plt

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


section .text
EXTR_tudorIplPostProcess:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x20
    mov r8,qword ptr [rcx]
    mov rsi,rdx
    lea rdx,[rcx + 0x18]
    mov rdi,rcx
    mov rcx,rsi
    call EXTR_FUN_18000c030
    mov r8,qword ptr [rdi]
    lea rdx,[rdi + 0x18]
    mov rcx,rsi
    mov rbx,qword ptr [rsp + 0x30]
    mov rsi,qword ptr [rsp + 0x38]
    add rsp,0x20
    pop rdi
    jmp EXTR_FUN_18000c140

section .data


section .text
EXTR_FUN_18000c140:
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    sub rsp,0x50
    movzx r11d,word ptr [rdx + 0x14]
    mov rsi,r8
    mov rbx,rdx
    test r11w,r11w
    mov rdi,rcx
    jz .lbl18000c3bc
    movzx r10d,word ptr [rdx + 0x12]
    test r10w,r10w
    jz .lbl18000c3bc
    mov rcx,rdx
    mov qword ptr [rsp + 0x60],rbp
    call EXTR_contains_25
    test eax,eax
    jz .lbl18000c1d5
    cmp word ptr [rbx + 0x12e],0x0
    jz .lbl18000c1b4
    lea r9,[rbx + 0x2a]
    movzx r8d,r10w
    movzx edx,r11w
    mov rcx,rdi
    mov qword ptr [rsp + 0x28],rsi
    mov dword ptr [rsp + 0x20],0xd
    call EXTR_FUN_180016ac0
    jmp .lbl18000c1d5
    .lbl18000c1b4: lea r9,[rbx + 0x2a]
    movzx r8d,r10w
    movzx edx,r11w
    mov rcx,rdi
    mov qword ptr [rsp + 0x28],rsi
    mov dword ptr [rsp + 0x20],0xd
    call EXTR_FUN_1800169f0
    .lbl18000c1d5: test byte ptr [rbx + 0xc],0x80
    jz .lbl18000c1f0
    movzx r8d,word ptr [rbx + 0x12]
    movzx edx,word ptr [rbx + 0x14]
    mov r9,qword ptr [rbx + 0x60]
    mov rcx,rdi
    call EXTR_FUN_180002110
    .lbl18000c1f0: test byte ptr [rbx + 0xc],0x40
    jz .lbl18000c20b
    movzx r8d,word ptr [rbx + 0x12]
    movzx edx,word ptr [rbx + 0x14]
    mov r9,qword ptr [rbx + 0x60]
    mov rcx,rdi
    call EXTR_FUN_180002210
    .lbl18000c20b: test byte ptr [rbx + 0xc],0x20
    jz .lbl18000c2c1
    movzx eax,word ptr [rbx + 0x16]
    add eax,-0x6
    cmp eax,0x1
    ja .lbl18000c272
    movzx ecx,word ptr [rbx + 0x18]
    movzx r8d,word ptr [rbx + 0x12]
    mov qword ptr [rsp + 0x40],rsi
    mov dword ptr [rsp + 0x38],0x55
    mov eax,0x3e0f83e1
    mov dword ptr [rsp + 0x30],0xf
    mov word ptr [rsp + 0x28],0x9c4
    mov word ptr [rsp + 0x20],0xf63c
    imul ecx
    mov r9d,edx
    movzx edx,word ptr [rbx + 0x14]
    mov rcx,rdi
    sar r9d,0x3
    mov eax,r9d
    shr eax,0x1f
    add r9d,eax
    call EXTR_FUN_180004780
    jmp .lbl18000c2c1
    .lbl18000c272: movzx ecx,word ptr [rbx + 0x18]
    movzx r8d,word ptr [rbx + 0x12]
    mov qword ptr [rsp + 0x40],rsi
    mov dword ptr [rsp + 0x38],0x4b
    mov eax,0x3e0f83e1
    mov dword ptr [rsp + 0x30],0x19
    mov word ptr [rsp + 0x28],0x9c4
    mov word ptr [rsp + 0x20],0xf63c
    imul ecx
    mov r9d,edx
    movzx edx,word ptr [rbx + 0x14]
    mov rcx,rdi
    sar r9d,0x3
    mov eax,r9d
    shr eax,0x1f
    add r9d,eax
    call EXTR_FUN_180004780
    .lbl18000c2c1: test byte ptr [rbx + 0xc],0x10
    jz .lbl18000c2fa
    movzx ecx,word ptr [rbx + 0x26]
    movzx r9d,word ptr [rbx + 0x12]
    movzx r8d,word ptr [rbx + 0x14]
    mov rdx,qword ptr [rbx + 0x60]
    mov eax,ecx
    mov dword ptr [rsp + 0x38],0x1
    neg eax
    mov qword ptr [rsp + 0x30],rsi
    mov dword ptr [rsp + 0x28],eax
    mov dword ptr [rsp + 0x20],ecx
    mov rcx,rdi
    call EXTR_FUN_1800037a0
    .lbl18000c2fa: bt dword ptr [rbx + 0xc],0x8
    jnc .lbl18000c32c
    movzx ecx,word ptr [rbx + 0x26]
    movzx r9d,word ptr [rbx + 0x12]
    movzx r8d,word ptr [rbx + 0x14]
    mov rdx,qword ptr [rbx + 0x60]
    mov eax,ecx
    mov qword ptr [rsp + 0x30],rsi
    neg eax
    mov dword ptr [rsp + 0x28],eax
    mov dword ptr [rsp + 0x20],ecx
    mov rcx,rdi
    call EXTR_FUN_180003920
    .lbl18000c32c: movzx r9d,word ptr [rbx + 0x26]
    movzx r8d,word ptr [rbx + 0x12]
    movzx edx,word ptr [rbx + 0x14]
    mov rcx,rdi
    call EXTR_FUN_18000bbc0
    test byte ptr [rbx + 0xc],0x2
    jz .lbl18000c35a
    movzx r8d,word ptr [rbx + 0x12]
    movzx edx,word ptr [rbx + 0x14]
    mov r9,qword ptr [rbx + 0x60]
    call EXTR_FUN_180012bc0
    .lbl18000c35a: movzx r8d,word ptr [rbx + 0x12]
    movzx edx,word ptr [rbx + 0x14]
    mov rcx,qword ptr [rbx + 0x60]
    call EXTR_FUN_180012910
    cmp word ptr [rbx + 0x16],0x6
    mov ebp,eax
    jnz .lbl18000c3a1
    movzx r8d,word ptr [rbx + 0x12]
    movzx edx,word ptr [rbx + 0x14]
    mov r9,rsi
    mov rcx,rdi
    call EXTR_FUN_18000ba10
    movzx ecx,word ptr [rbx + 0x12]
    movzx r8d,word ptr [rbx + 0x14]
    imul r8d,ecx
    mov rdx,rsi
    mov rcx,rdi
    call EXTR_memcpy wrt ..plt
    .lbl18000c3a1: mov eax,ebp
    mov rbp,qword ptr [rsp + 0x60]
    mov byte ptr [rbx + 0x58],0x0
    mov rbx,qword ptr [rsp + 0x68]
    mov rsi,qword ptr [rsp + 0x70]
    add rsp,0x50
    pop rdi
    ret
    .lbl18000c3bc: mov rbx,qword ptr [rsp + 0x68]
    mov rsi,qword ptr [rsp + 0x70]
    mov eax,0x64
    add rsp,0x50
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180002210:
    test r8d,r8d
    jle .lbl1800023a0
    mov rax,rsp
    mov qword ptr [rax + 0x20],r9
    mov qword ptr [rax + 0x8],rcx
    push rbx
    push r13
    push r14
    sub rsp,0x50
    mov qword ptr [rax + 0x10],rbp
    mov qword ptr [rax + -0x20],rsi
    movsxd rbx,r8d
    mov qword ptr [rax + -0x28],rdi
    lea r10,[rbx + rbx*0x1]
    mov r14,r9
    mov r13,rcx
    xor r11d,r11d
    mov rbp,rcx
    mov qword ptr [rsp + 0x8],rcx
    movsxd r9,edx
    add rcx,r10
    mov qword ptr [rax + -0x30],r12
    mov qword ptr [rsp + 0x28],rbx
    mov qword ptr [rsp],rcx
    mov qword ptr [rsp + 0x20],r9
    mov qword ptr [rax + -0x38],r15
    nop
    .lbl180002270: xor edi,edi
    xor esi,esi
    xor edx,edx
    xor r8d,r8d
    xor r12d,r12d
    cmp r9,0x2
    jl .lbl180002305
    lea rax,[rbx*0x4]
    lea r13,[r11 + r14*0x1]
    mov r15,rbp
    lea r14,[r13 + rbx*0x1]
    lea rbx,[r9 + -0x2]
    mov r9,rcx
    shr rbx,1
    mov rbp,rax
    add rbx,0x1
    lea r12,[rbx + rbx*0x1]
    nop
    .lbl1800022b0: movsx eax,word ptr [r15]
    xor ecx,ecx
    cmp byte ptr [r13],cl
    setnz cl
    add edi,ecx
    imul eax,ecx
    xor ecx,ecx
    add edx,eax
    cmp byte ptr [r14],cl
    movsx eax,word ptr [r9]
    setnz cl
    add r15,rbp
    add r9,rbp
    add esi,ecx
    add r13,r10
    add r14,r10
    imul eax,ecx
    add edx,eax
    sub rbx,0x1
    jnz .lbl1800022b0
    mov r9,qword ptr [rsp + 0x20]
    mov rbp,qword ptr [rsp + 0x8]
    mov rbx,qword ptr [rsp + 0x28]
    mov r13,qword ptr [rsp + 0x70]
    mov r14,qword ptr [rsp + 0x88]
    .lbl180002305: cmp r12,r9
    jge .lbl180002328
    mov rcx,rbx
    imul rcx,r12
    add rcx,r11
    cmp byte ptr [rcx + r14*0x1],r8b
    movsx ecx,word ptr [r13 + rcx*0x2]
    setnz r8b
    imul ecx,r8d
    add edx,ecx
    .lbl180002328: lea ecx,[rsi + rdi*0x1]
    add r8d,ecx
    test r8d,r8d
    jle .lbl18000233b
    mov eax,edx
    cdq
    idiv r8d
    mov edx,eax
    .lbl18000233b: test r9,r9
    jle .lbl18000235c
    mov rax,rbp
    mov rcx,r9
    nop
    nop
    nop
    .lbl180002350: sub word ptr [rax],dx
    add rax,r10
    sub rcx,0x1
    jnz .lbl180002350
    .lbl18000235c: mov rcx,qword ptr [rsp]
    add rbp,0x2
    add r11,0x1
    add rcx,0x2
    cmp r11,rbx
    mov qword ptr [rsp + 0x8],rbp
    mov qword ptr [rsp],rcx
    jl .lbl180002270
    mov r15,qword ptr [rsp + 0x30]
    mov r12,qword ptr [rsp + 0x38]
    mov rdi,qword ptr [rsp + 0x40]
    mov rsi,qword ptr [rsp + 0x48]
    mov rbp,qword ptr [rsp + 0x78]
    add rsp,0x50
    pop r14
    pop r13
    pop rbx
    .lbl1800023a0: ret 0x0

section .data


section .text
EXTR_FUN_18000bbc0:
    sub rsp,0x8
    imul edx,r8d
    xor r8d,r8d
    test edx,edx
    movsxd r10,edx
    jle .lbl18000bc10
    mov qword ptr [rsp],rbx
    lea r11d,[r9 + r9*0x1]
    mov ebx,0xff
    nop
    .lbl18000bbe0: movsx eax,word ptr [rcx + r8*0x2]
    add eax,r9d
    imul eax,eax,0xff
    cdq
    idiv r11d
    test eax,eax
    jns .lbl18000bbfa
    xor eax,eax
    jmp .lbl18000bbff
    .lbl18000bbfa: cmp eax,ebx
    cmovg eax,ebx
    .lbl18000bbff: mov byte ptr [r8 + rcx*0x1],al
    add r8,0x1
    cmp r8,r10
    jl .lbl18000bbe0
    mov rbx,qword ptr [rsp]
    .lbl18000bc10: add rsp,0x8
    ret

section .data


section .text
EXTR_FUN_180016ac0:
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
    call EXTR_FUN_180018a60
    mov rcx,qword ptr [rsp + 0x88]
    mov r9,r13
    mov r8d,r12d
    mov edx,ebp
    mov dword ptr [rsp + 0x20],edi
    mov ebx,eax
    call EXTR_FUN_1800160e0
    mov rdx,qword ptr [rsp + 0x88]
    mov rcx,qword ptr [rsp + 0x60]
    mov r9d,r14d
    mov r8d,r15d
    mov dword ptr [rsp + 0x20],ebx
    call EXTR_FUN_180018d00
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

glb180174d70: db 0, 0, 0, 0, -120, 15, -55, 0, 32, 29, -110, 1, -41, 38, 91, 2, -65, 42, 36, 3, -26, 38, -19, 3, 93, 25, -74, 4, 53, 0, 127, 5, 124, -39, 71, 6, 69, -93, 16, 7, -98, 91, -39, 7, -102, 0, -94, 8, 73, -112, 106, 9, -67, 8, 51, 10, 5, 104, -5, 10, 53, -84, -61, 11, 94, -45, -117, 12, -110, -37, 83, 13, -28, -62, 27, 14, 102, -121, -29, 14, 43, 39, -85, 15, 72, -96, 114, 16, -49, -16, 57, 17, -43, 22, 1, 18, 111, 16, -56, 18, -79, -37, -114, 19, -79, 118, 85, 20, -122, -33, 27, 21, 69, 20, -30, 21, 5, 19, -88, 22, -34, -39, 109, 23, -23, 102, 51, 24, 60, -72, -8, 24, -13, -53, -67, 25, 38, -96, -126, 26, -17, 50, 71, 27, 106, -126, 11, 28, -77, -116, -49, 28, -27, 79, -109, 29, 30, -54, 86, 30, 123, -7, 25, 31, 27, -36, -36, 31, 28, 112, -97, 32, -96, -77, 97, 33, -59, -92, 35, 34, -81, 65, -27, 34, 127, -120, -90, 35, 88, 119, 103, 36, 94, 12, 40, 37, -74, 69, -24, 37, -122, 33, -88, 38, -12, -99, 103, 39, 40, -71, 38, 40, 75, 113, -27, 40, -123, -60, -93, 41, 1, -79, 97, 42, -21, 52, 31, 43, 111, 78, -36, 43, -70, -5, -104, 44, -4, 58, 85, 45, 98, 10, 17, 46, 30, 104, -52, 46, 98, 82, -121, 47, 97, -57, 65, 48, 77, -59, -5, 48, 94, 74, -75, 49, -57, 84, 110, 50, -61, -30, 38, 51, -121, -14, -34, 51, 80, -126, -106, 52, 87, -112, 77, 53, -39, 26, 4, 54, 20, 32, -70, 54, 70, -98, 111, 55, -80, -109, 36, 56, -109, -2, -40, 56, 50, -35, -116, 57, -46, 45, 64, 58, -73, -18, -14, 58, 41, 30, -91, 59, 112, -70, 86, 60, -42, -63, 7, 61, -90, 50, -72, 61, 44, 11, 104, 62, -72, 73, 23, 63, -104, -20, -59, 63, 29, -14, 115, 64, -101, 88, 33, 65, 101, 30, -50, 65, -48, 65, 122, 66, 53, -63, 37, 67, -19, -102, -48, 67, 80, -51, 122, 68, -67, 86, 36, 69, -113, 53, -51, 69, 40, 104, 117, 70, -25, -20, 28, 71, 47, -62, -61, 71, 101, -26, 105, 72, -18, 87, 15, 73, 51, 21, -76, 73, -98, 28, 88, 74, -104, 108, -5, 74, -112, 3, -98, 75, -12, -33, 63, 76, 52, 0, -31, 76, -60, 98, -127, 77, 23, 6, 33, 78, -91, -24, -65, 78, -29, 8, 94, 79, 77, 101, -5, 79, 94, -4, -105, 80, -108, -52, 51, 81, 110, -44, -50, 81, 110, 18, 105, 82, 24, -123, 2, 83, -16, 42, -101, 83, 125, 2, 51, 84, 75, 10, -54, 84, -30, 64, 96, 85, -46, -92, -11, 85, -87, 52, -118, 86, -6, -18, 29, 87, 86, -46, -80, 87, 84, -35, 66, 88, -116, 14, -44, 88, -104, 100, 100, 89, 18, -34, -13, 89
glb180175170: db -1, -1, -1, 127

section .text
EXTR_FUN_180019450:
    movsxd rax,ecx
    movsxd rcx,edx
    imul rax,rcx
    sar rax,0x1f
    ret

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
EXTR_FUN_1800160e0:
    mov qword ptr [rsp + 0x20],r9
    mov dword ptr [rsp + 0x10],edx
    push rsi
    push rdi
    push r14
    sub rsp,0x50
    mov r10d,edx
    mov eax,edx
    xor esi,esi
    cdq
    mov rdi,rcx
    mov r14d,r8d
    imul r10d,r8d
    sub eax,edx
    movsxd r10,r10d
    sar eax,1
    mov qword ptr [rsp + 0x28],r10
    mov ecx,eax
    mov dword ptr [rsp + 0x20],eax
    js .lbl180016334
    mov qword ptr [rsp + 0x70],rbx
    mov qword ptr [rsp + 0x48],rbp
    mov qword ptr [rsp + 0x40],r12
    mov eax,r8d
    mov qword ptr [rsp + 0x38],r13
    mov qword ptr [rsp + 0x30],r15
    cdq
    sub eax,edx
    sar eax,1
    mov r15d,eax
    nop
    .lbl180016140: xor ebx,ebx
    test r15d,r15d
    js .lbl180016310
    mov eax,esi
    xor r13d,r13d
    shl eax,0xa
    cdq
    idiv ecx
    mov ecx,esi
    mov r8d,eax
    imul r8d,r8d
    imul ecx,r14d
    movsxd rdx,ecx
    mov dword ptr [rsp + 0x80],r8d
    lea rcx,[rdx + r10*0x1]
    lea rbp,[rdi + rdx*0x4]
    lea r12,[rdi + rcx*0x4]
    nop
    nop
    .lbl180016180: mov eax,r13d
    cdq
    idiv r15d
    imul eax,eax
    lea ecx,[r8 + rax*0x1]
    call EXTR_FUN_180002420
    mov edx,dword ptr [rsp + 0x90]
    mov rcx,qword ptr [rsp + 0x88]
    mov r8d,eax
    call EXTR_query_range
    mov ecx,dword ptr [rbp]
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
    mov ecx,r11d
    mov dword ptr [rbp],edx
    imul ecx,dword ptr [r12]
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    test ebx,ebx
    mov dword ptr [r12],edx
    jle .lbl18001623d
    mov r10,qword ptr [rsp + 0x28]
    lea eax,[rsi + 0x1]
    imul eax,r14d
    sub eax,ebx
    movsxd r8,eax
    mov eax,0x51eb851f
    mov ecx,dword ptr [rdi + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov eax,0x51eb851f
    mov dword ptr [rdi + r8*0x4],edx
    add r8,r10
    mov ecx,dword ptr [rdi + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov dword ptr [rdi + r8*0x4],edx
    jmp .lbl180016242
    .lbl18001623d: mov r10,qword ptr [rsp + 0x28]
    .lbl180016242: test esi,esi
    jle .lbl1800162e9
    mov r9d,dword ptr [rsp + 0x78]
    sub r9d,esi
    mov eax,r9d
    imul eax,r14d
    add eax,ebx
    movsxd r8,eax
    mov eax,0x51eb851f
    mov ecx,dword ptr [rdi + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov eax,0x51eb851f
    mov dword ptr [rdi + r8*0x4],edx
    add r8,r10
    mov ecx,dword ptr [rdi + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    test ebx,ebx
    mov dword ptr [rdi + r8*0x4],edx
    jle .lbl1800162e9
    lea eax,[r9 + 0x1]
    imul eax,r14d
    sub eax,ebx
    movsxd r8,eax
    mov eax,0x51eb851f
    mov ecx,dword ptr [rdi + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov eax,0x51eb851f
    mov dword ptr [rdi + r8*0x4],edx
    add r8,r10
    mov ecx,dword ptr [rdi + r8*0x4]
    imul ecx,r11d
    imul ecx
    sar edx,0x5
    mov eax,edx
    shr eax,0x1f
    add edx,eax
    mov dword ptr [rdi + r8*0x4],edx
    .lbl1800162e9: mov r8d,dword ptr [rsp + 0x80]
    add ebx,0x1
    add r13d,0x400
    add rbp,0x4
    add r12,0x4
    cmp ebx,r15d
    jle .lbl180016180
    mov ecx,dword ptr [rsp + 0x20]
    .lbl180016310: add esi,0x1
    cmp esi,ecx
    jle .lbl180016140
    mov r15,qword ptr [rsp + 0x30]
    mov r13,qword ptr [rsp + 0x38]
    mov r12,qword ptr [rsp + 0x40]
    mov rbp,qword ptr [rsp + 0x48]
    mov rbx,qword ptr [rsp + 0x70]
    .lbl180016334: add rsp,0x50
    pop r14
    pop rdi
    pop rsi
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
EXTR_FUN_18000ba10:
    test edx,edx
    jle .lbl18000bbb7
    mov r11,rsp
    mov qword ptr [r11 + 0x20],r9
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [r11 + 0x8],rcx
    push rbp
    push r13
    sub rsp,0x68
    mov qword ptr [r11 + 0x18],rbx
    mov qword ptr [r11 + -0x18],rsi
    mov qword ptr [r11 + -0x20],rdi
    mov qword ptr [r11 + -0x28],r12
    mov eax,edx
    xor r13d,r13d
    mov qword ptr [r11 + -0x30],r14
    mov ebp,r8d
    mov esi,0x5be
    mov qword ptr [rsp + 0x30],r13
    mov qword ptr [rsp + 0x38],rax
    mov qword ptr [r11 + -0x38],r15
    nop
    .lbl18000ba60: test ebp,ebp
    jle .lbl18000bb74
    mov eax,ebp
    movsxd rdi,ebp
    mov r14d,esi
    mov r12d,0x19
    imul eax,eax,0x32
    imul rdi,r13
    mov r15d,eax
    sar eax,1
    and r15d,0xfffffffe
    add rdi,r9
    mov r13,rbp
    imul r14d,esi
    sub r12d,eax
    nop
    nop
    nop
    nop
    .lbl18000baa0: mov ecx,r12d
    imul ecx,r12d
    add ecx,r14d
    call EXTR_FUN_180002420
    mov ecx,eax
    mov ebx,eax
    call EXTR_FUN_18000b7c0
    mov byte ptr [rsp + 0x28],0x80
    mov r11d,eax
    mov eax,0x6aff5f81
    imul r11d,r11d,0x2d5
    imul r11d
    mov eax,r12d
    mov r9d,edx
    sar r9d,0x9
    mov ecx,r9d
    shr ecx,0x1f
    add r9d,ecx
    add r9d,ebx
    imul eax,r9d
    cdq
    idiv ebx
    lea ecx,[r15 + rax*0x2 + -0x32]
    mov eax,0x51eb851f
    shl ecx,0x9
    imul ecx
    mov r8d,edx
    sar r8d,0x4
    mov eax,r8d
    shr eax,0x1f
    add r8d,eax
    mov eax,esi
    mov dword ptr [rsp + 0x20],r8d
    mov r8d,ebp
    imul eax,r9d
    cdq
    idiv ebx
    lea ecx,[rax + -0x5be]
    mov eax,0x51eb851f
    shl ecx,0xa
    imul ecx
    mov rcx,qword ptr [rsp + 0x80]
    sar edx,0x4
    mov r9d,edx
    shr r9d,0x1f
    add r9d,edx
    mov edx,dword ptr [rsp + 0x88]
    call EXTR_FUN_18000b8e0
    add rdi,0x1
    add r12d,0x32
    sub r13,0x1
    mov byte ptr [rdi + -0x1],al
    jnz .lbl18000baa0
    mov r13,qword ptr [rsp + 0x30]
    mov rax,qword ptr [rsp + 0x38]
    mov r9,qword ptr [rsp + 0x98]
    .lbl18000bb74: add r13,0x1
    add esi,0x32
    sub rax,0x1
    mov qword ptr [rsp + 0x38],rax
    mov qword ptr [rsp + 0x30],r13
    jnz .lbl18000ba60
    mov r15,qword ptr [rsp + 0x40]
    mov r14,qword ptr [rsp + 0x48]
    mov r12,qword ptr [rsp + 0x50]
    mov rdi,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    mov rbx,qword ptr [rsp + 0x90]
    add rsp,0x68
    pop r13
    pop rbp
    .lbl18000bbb7: ret 0x0

section .data


section .text
EXTR_FUN_18000b8e0:
    mov qword ptr [rsp + 0x20],rbp
    push r12
    test r9d,r9d
    mov ebp,r9d
    movsxd r10,r8d
    mov r12,rcx
    js .lbl18000b9fd
    lea eax,[rdx + -0x1]
    mov edx,r9d
    sar edx,0xa
    cmp edx,eax
    jge .lbl18000b9fd
    mov qword ptr [rsp + 0x18],rsi
    mov esi,dword ptr [rsp + 0x30]
    mov qword ptr [rsp + 0x20],rdi
    test esi,esi
    js .lbl18000b9e7
    mov edi,esi
    lea r9d,[r10 + -0x1]
    sar edi,0xa
    cmp edi,r9d
    jge .lbl18000b9e7
    xor ecx,ecx
    cmp edx,eax
    mov qword ptr [rsp + 0x10],rbx
    mov r8d,ecx
    setl r8b
    mov eax,edx
    add r8d,edx
    cmp edi,r9d
    mov r11d,0x400
    setl cl
    shl eax,0xa
    mov ebx,r11d
    add ecx,edi
    sub ebp,eax
    mov eax,edi
    movsxd r9,ecx
    shl eax,0xa
    movsxd rcx,r8d
    sub esi,eax
    mov rax,r10
    sub ebx,ebp
    sub r11d,esi
    movsxd r10,edx
    movsxd rdx,edi
    imul rcx,rax
    mov rdi,qword ptr [rsp + 0x20]
    imul r10,rax
    lea rax,[rcx + rdx*0x1]
    movzx r8d,byte ptr [rax + r12*0x1]
    lea rax,[rcx + r9*0x1]
    movzx ecx,byte ptr [rax + r12*0x1]
    lea rax,[rdx + r10*0x1]
    imul r8d,r11d
    imul r8d,ebp
    imul ecx,esi
    imul ecx,ebp
    add r8d,ecx
    movzx ecx,byte ptr [rax + r12*0x1]
    lea rax,[r9 + r10*0x1]
    imul ecx,r11d
    imul ecx,ebx
    add r8d,ecx
    movzx ecx,byte ptr [rax + r12*0x1]
    imul ecx,esi
    mov rsi,qword ptr [rsp + 0x18]
    imul ecx,ebx
    mov rbx,qword ptr [rsp + 0x10]
    lea eax,[r8 + rcx*0x1 + 0x80000]
    shr eax,0x14
    mov rbp,qword ptr [rsp + 0x28]
    pop r12
    ret
    .lbl18000b9e7: mov al,byte ptr [rsp + 0x38]
    mov rsi,qword ptr [rsp + 0x18]
    mov rdi,qword ptr [rsp + 0x20]
    mov rbp,qword ptr [rsp + 0x28]
    pop r12
    ret
    .lbl18000b9fd: mov al,byte ptr [rsp + 0x38]
    mov rbp,qword ptr [rsp + 0x28]
    pop r12
    ret

section .data


section .text
EXTR_FUN_18000b7c0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rdi
    xor edi,edi
    cmp ecx,0xc80
    mov ebx,ecx
    lea eax,[rcx + -0x6a4]
    mov r8d,edi
    mov r11d,edi
    setge r11b
    cmp ecx,0x9c4
    mov r10d,edi
    setge r8b
    cmp ecx,0x6a4
    mov r9d,edi
    setge r10b
    cmp ecx,0x578
    mov edx,edi
    setge r9b
    cmp ecx,0x9c4
    mov ecx,edi
    setl dl
    and edx,r10d
    imul edx,eax
    mov eax,r8d
    imul eax,eax,0x320
    add edx,eax
    mov eax,r11d
    imul eax,eax,0x15e
    add edx,eax
    cmp ebx,0xc80
    lea eax,[rbx + -0x9c4]
    setl cl
    and ecx,r8d
    imul ecx,eax
    mov eax,r9d
    lea r8d,[rcx + rdx*0x2]
    mov edx,edi
    mov ecx,edi
    imul r8d,r8d,0x33
    cmp ebx,0x578
    setl dl
    imul eax,eax,0x578
    imul r10d,r10d,0x96
    add edx,eax
    lea eax,[rbx + -0x578]
    add edx,r10d
    cmp ebx,0x6a4
    setl cl
    and ecx,r9d
    imul ecx,eax
    lea eax,[rcx + rdx*0x2]
    mov ecx,edi
    imul eax,eax,0x133
    add r8d,eax
    cmp ebx,0x157c
    lea eax,[rbx + -0xc80]
    setl cl
    and ecx,r11d
    imul ecx,eax
    cmp ebx,0x157c
    mov rbx,qword ptr [rsp + 0x8]
    setge dil
    imul edi,edi,0x8fc
    add ecx,edi
    mov rdi,qword ptr [rsp + 0x10]
    lea eax,[rcx + rcx*0x4]
    lea eax,[r8 + rax*0x2 + 0x200]
    sar eax,0xa
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
EXTR_FUN_1800037a0:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x20
    mov r13d,dword ptr [rsp + 0x70]
    mov r12,qword ptr [rsp + 0x80]
    mov edi,r8d
    sub r13d,dword ptr [rsp + 0x78]
    mov r15,rdx
    mov r14,rcx
    lea r8d,[0x4 + r13*0x4]
    xor edx,edx
    mov rcx,r12
    mov ebx,r9d
    mov esi,0xffff8000
    mov ebp,0x7fff
    call EXTR_memset wrt ..plt
    xor r11d,r11d
    mov rdx,r11
    imul edi,ebx
    test edi,edi
    movsxd r9,edi
    jle .lbl180003828
    nop
    nop
    nop
    .lbl180003810: movsx eax,word ptr [r14 + rdx*0x2]
    cmp esi,eax
    cmovl esi,eax
    cmp ebp,eax
    cmovg ebp,eax
    add rdx,0x1
    cmp rdx,r9
    jl .lbl180003810
    .lbl180003828: sub esi,ebp
    jz .lbl1800038fc
    test edi,edi
    mov r8,r11
    jle .lbl180003866
    nop
    nop
    nop
    .lbl180003840: movsx eax,word ptr [r14 + r8*0x2]
    add r8,0x1
    sub eax,ebp
    imul eax,r13d
    cdq
    idiv esi
    movsxd rcx,eax
    mov al,byte ptr [r8 + r15*0x1 + -0x1]
    and eax,0x1
    add dword ptr [r12 + rcx*0x4],eax
    cmp r8,r9
    jl .lbl180003840
    .lbl180003866: mov eax,dword ptr [r12]
    mov r8d,dword ptr [rsp + 0x88]
    test eax,eax
    jle .lbl18000387d
    add eax,r8d
    mov dword ptr [r12],eax
    .lbl18000387d: lea eax,[r13 + 0x1]
    cmp eax,0x1
    jle .lbl1800038ab
    add eax,-0x1
    lea rcx,[r12 + 0x4]
    mov edx,eax
    .lbl180003890: mov eax,dword ptr [rcx]
    test eax,eax
    jle .lbl18000389b
    add eax,r8d
    mov dword ptr [rcx],eax
    .lbl18000389b: mov eax,dword ptr [rcx + -0x4]
    add rcx,0x4
    add dword ptr [rcx + -0x4],eax
    sub rdx,0x1
    jnz .lbl180003890
    .lbl1800038ab: mov eax,dword ptr [r12]
    movsxd r10,r13d
    cmp dword ptr [r12 + r10*0x4],eax
    jz .lbl1800038fc
    test edi,edi
    jle .lbl1800038fc
    mov ebx,dword ptr [rsp + 0x78]
    .lbl1800038c0: movsx eax,word ptr [r14 + r11*0x2]
    mov r8d,dword ptr [r12]
    add r11,0x1
    sub eax,ebp
    imul eax,r13d
    cdq
    idiv esi
    movsxd rcx,eax
    mov eax,dword ptr [r12 + rcx*0x4]
    mov ecx,dword ptr [r12 + r10*0x4]
    sub eax,r8d
    sub ecx,r8d
    imul eax,r13d
    cdq
    idiv ecx
    add ax,bx
    cmp r11,r9
    mov word ptr [r14 + r11*0x2 + -0x2],ax
    jl .lbl1800038c0
    .lbl1800038fc: mov rbx,qword ptr [rsp + 0x50]
    mov rbp,qword ptr [rsp + 0x58]
    mov rsi,qword ptr [rsp + 0x60]
    add rsp,0x20
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    ret

section .data


section .text
EXTR_FUN_180003920:
    mov qword ptr [rsp + 0x10],rbx
    mov qword ptr [rsp + 0x8],rcx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x30
    mov eax,dword ptr [rsp + 0x90]
    mov rbp,rcx
    mov rcx,qword ptr [rsp + 0xa0]
    sub eax,dword ptr [rsp + 0x98]
    mov edi,r8d
    mov r14,rdx
    xor esi,esi
    lea r8d,[0x4 + rax*0x4]
    xor edx,edx
    mov ebx,r9d
    mov r12d,0xffff8000
    mov r13d,0x7fff
    mov dword ptr [rsp + 0x88],eax
    mov r15d,esi
    mov dword ptr [rsp + 0x80],esi
    call EXTR_memset wrt ..plt
    mov r11,rsi
    imul edi,ebx
    test edi,edi
    movsxd rbx,edi
    jle .lbl1800039bd
    nop
    nop
    nop
    .lbl1800039a0: movsx eax,word ptr [rbp + r11*0x2]
    cmp r12d,eax
    cmovl r12d,eax
    cmp r13d,eax
    cmovg r13d,eax
    add r11,0x1
    cmp r11,rbx
    jl .lbl1800039a0
    .lbl1800039bd: sub r12d,r13d
    jz .lbl180003b66
    test edi,edi
    mov r11d,dword ptr [rsp + 0x88]
    mov r8,rsi
    jle .lbl180003a0c
    mov r15,qword ptr [rsp + 0xa0]
    nop
    .lbl1800039e0: movsx eax,word ptr [rbp + r8*0x2]
    add r8,0x1
    sub eax,r13d
    imul eax,r11d
    cdq
    idiv r12d
    movsxd rcx,eax
    mov al,byte ptr [r8 + r14*0x1 + -0x1]
    and eax,0x1
    add dword ptr [r15 + rcx*0x4],eax
    cmp r8,rbx
    jl .lbl1800039e0
    mov r15d,esi
    .lbl180003a0c: cmp rbx,0x2
    mov edx,esi
    mov r8d,esi
    mov r10d,esi
    mov rcx,rsi
    jl .lbl180003a42
    lea r9,[rbx + -0x1]
    .lbl180003a21: mov eax,esi
    cmp byte ptr [r14 + rcx*0x1],al
    setnz al
    add edx,eax
    mov eax,esi
    cmp byte ptr [r14 + rcx*0x1 + 0x1],al
    setnz al
    add rcx,0x2
    add r8d,eax
    cmp rcx,r9
    jl .lbl180003a21
    .lbl180003a42: cmp rcx,rbx
    jge .lbl180003a4f
    cmp byte ptr [rcx + r14*0x1],sil
    setnz r10b
    .lbl180003a4f: lea eax,[rdx + r8*0x1]
    add r10d,eax
    jz .lbl180003b66
    lea eax,[r11 + 0x1]
    cmp eax,0x1
    jle .lbl180003b66
    mov rsi,qword ptr [rsp + 0xa0]
    mov ebp,dword ptr [rsp + 0x80]
    mov r11d,r10d
    imul r11d,r11d,0x63
    add eax,-0x1
    mov rdi,rsi
    mov r14d,eax
    nop
    nop
    .lbl180003a90: mov r8d,dword ptr [rdi]
    mov eax,dword ptr [rdi + 0x4]
    add rdi,0x4
    mov r9d,r8d
    lea ecx,[r8 + rax*0x1]
    mov dword ptr [rdi],ecx
    imul r9d,r9d,0x64
    cmp r9d,r10d
    jg .lbl180003ac8
    mov eax,ecx
    imul eax,eax,0x64
    cmp eax,r10d
    jle .lbl180003ac8
    mov eax,r8d
    sub eax,dword ptr [rsi]
    imul eax,r12d
    cdq
    idiv r10d
    lea r15d,[r13 + rax*0x1]
    .lbl180003ac8: cmp r9d,r11d
    jg .lbl180003ae8
    imul ecx,ecx,0x64
    cmp ecx,r11d
    jle .lbl180003ae8
    sub r8d,dword ptr [rsi]
    imul r8d,r12d
    mov eax,r8d
    cdq
    idiv r10d
    lea ebp,[r13 + rax*0x1]
    .lbl180003ae8: sub r14,0x1
    jnz .lbl180003a90
    mov dword ptr [rsp + 0x80],ebp
    cmp ebp,r15d
    mov rbp,qword ptr [rsp + 0x70]
    mov rsi,r14
    jz .lbl180003b66
    test rbx,rbx
    jle .lbl180003b66
    mov r8d,dword ptr [rsp + 0x80]
    mov r9d,dword ptr [rsp + 0x98]
    mov r10d,dword ptr [rsp + 0x88]
    nop
    .lbl180003b20: movsx eax,word ptr [rbp + rsi*0x2]
    cmp eax,r8d
    jl .lbl180003b33
    mov eax,dword ptr [rsp + 0x90]
    jmp .lbl180003b58
    .lbl180003b33: cmp eax,r15d
    jg .lbl180003b44
    mov eax,dword ptr [rsp + 0x98]
    mov r9d,eax
    jmp .lbl180003b58
    .lbl180003b44: sub eax,r15d
    mov ecx,r8d
    sub ecx,r15d
    imul eax,r10d
    cdq
    idiv ecx
    add ax,r9w
    .lbl180003b58: mov word ptr [rbp + rsi*0x2],ax
    add rsi,0x1
    cmp rsi,rbx
    jl .lbl180003b20
    .lbl180003b66: mov rbx,qword ptr [rsp + 0x78]
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
EXTR_FUN_180004780:
    sub rsp,0x58
    mov rax,qword ptr [rsp + 0xa0]
    mov qword ptr [rsp + 0x48],rax
    mov eax,dword ptr [rsp + 0x98]
    mov dword ptr [rsp + 0x40],eax
    mov eax,dword ptr [rsp + 0x90]
    mov dword ptr [rsp + 0x38],eax
    movzx eax,word ptr [rsp + 0x88]
    mov word ptr [rsp + 0x30],ax
    movzx eax,word ptr [rsp + 0x80]
    mov word ptr [rsp + 0x28],ax
    mov dword ptr [rsp + 0x20],r9d
    call EXTR_FUN_1800040f0
    add rsp,0x58
    ret

section .data


section .text
EXTR_FUN_1800040f0:
    mov dword ptr [rsp + 0x18],r8d
    mov dword ptr [rsp + 0x10],edx
    mov qword ptr [rsp + 0x8],rcx
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp,0x98
    mov r14,qword ptr [rsp + 0x128]
    mov ebp,edx
    mov eax,r8d
    cdq
    mov r12d,r8d
    mov rdi,rcx
    idiv r9d
    movsxd r13,eax
    mov eax,ebp
    cdq
    mov dword ptr [rsp + 0x20],r13d
    idiv dword ptr [rsp + 0x100]
    mov r11d,eax
    mov r9d,eax
    mov rax,r13
    mov dword ptr [rsp + 0x28],r11d
    mov qword ptr [rsp + 0x88],rax
    imul r9d,r13d
    test r11d,r11d
    movsxd r10,r9d
    lea rbx,[r14 + r10*0x2]
    mov qword ptr [rsp + 0x80],rbx
    lea rsi,[rbx + r10*0x2]
    jle .lbl18000435a
    xor ecx,ecx
    mov rdx,r11
    add rax,rax
    mov r9,rbx
    mov qword ptr [rsp + 0x50],rbx
    mov qword ptr [rsp + 0x30],rax
    mov dword ptr [rsp + 0x100],ecx
    mov qword ptr [rsp + 0x58],rdx
    .lbl180004191: test r13d,r13d
    jle .lbl18000433a
    mov eax,ecx
    mov r15,r9
    mov r9d,dword ptr [rsp + 0x20]
    cdq
    xor r13d,r13d
    mov qword ptr [rsp + 0x40],r9
    idiv r11d
    movsxd rbp,eax
    mov eax,dword ptr [rsp + 0xe8]
    add eax,ecx
    cdq
    idiv r11d
    mov r8d,eax
    mov dword ptr [rsp + 0x24],eax
    mov rax,r14
    sub rax,rbx
    mov qword ptr [rsp + 0x68],rax
    .lbl1800041d2: mov eax,r13d
    add r13d,r12d
    movzx ecx,r8w
    cdq
    sub cx,bp
    xor r8d,r8d
    movsx r14d,cx
    mov dword ptr [rsp + 0xf8],r13d
    idiv r9d
    movsx rbx,ax
    mov eax,r13d
    cdq
    idiv r9d
    mov r12d,eax
    sub r12w,bx
    test r14d,r14d
    jle .lbl180004269
    movsxd r13,dword ptr [rsp + 0xf0]
    movsx r11d,r12w
    xor r9d,r9d
    mov r10d,r14d
    nop
    nop
    .lbl180004220: test r11d,r11d
    jle .lbl180004257
    lea rcx,[r9 + rbp*0x1]
    imul rcx,r13
    add rcx,rbx
    lea rdx,[rdi + rcx*0x2]
    mov ecx,r11d
    nop
    nop
    nop
    .lbl180004240: movzx eax,word ptr [rdx]
    add r8,0x1
    add rdx,0x2
    sub rcx,0x1
    mov word ptr [rsi + r8*0x2 + -0x2],ax
    jnz .lbl180004240
    .lbl180004257: add r9,0x1
    sub r10,0x1
    jnz .lbl180004220
    mov r13d,dword ptr [rsp + 0xf8]
    .lbl180004269: mov eax,0x51eb851f
    movsx ebx,r12w
    imul ebx,r14d
    mov ecx,ebx
    lea edi,[rbx + -0x1]
    imul ecx,dword ptr [rsp + 0x118]
    mov r8d,edi
    imul ecx
    mov rcx,rsi
    sar edx,0x5
    mov r9d,edx
    shr r9d,0x1f
    add r9d,edx
    xor edx,edx
    call EXTR_FUN_180002710
    mov rcx,qword ptr [rsp + 0x68]
    mov r8d,edi
    imul ebx,dword ptr [rsp + 0x120]
    mov word ptr [rcx + r15*0x1],ax
    mov eax,0x51eb851f
    sub ebx,0x1
    mov rcx,rsi
    imul ebx
    sar edx,0x5
    mov r9d,edx
    shr r9d,0x1f
    add r9d,edx
    xor edx,edx
    call EXTR_FUN_180002710
    mov rdi,qword ptr [rsp + 0xe0]
    mov r8d,dword ptr [rsp + 0x24]
    mov r9d,dword ptr [rsp + 0x20]
    mov r12d,dword ptr [rsp + 0xf0]
    mov word ptr [r15],ax
    add r15,0x2
    sub qword ptr [rsp + 0x40],0x1
    jnz .lbl1800041d2
    mov ecx,dword ptr [rsp + 0x100]
    mov r9,qword ptr [rsp + 0x50]
    mov rdx,qword ptr [rsp + 0x58]
    mov rax,qword ptr [rsp + 0x30]
    mov r13d,dword ptr [rsp + 0x20]
    mov r11d,dword ptr [rsp + 0x28]
    mov rbx,qword ptr [rsp + 0x80]
    mov r14,qword ptr [rsp + 0x128]
    mov ebp,dword ptr [rsp + 0xe8]
    .lbl18000433a: add ecx,ebp
    add r9,rax
    sub rdx,0x1
    mov qword ptr [rsp + 0x58],rdx
    mov dword ptr [rsp + 0x100],ecx
    mov qword ptr [rsp + 0x50],r9
    jnz .lbl180004191
    .lbl18000435a: xor ecx,ecx
    xor r15d,r15d
    test r11d,r11d
    mov dword ptr [rsp + 0x2c],ecx
    jle .lbl180004759
    nop
    .lbl180004370: xor r12d,r12d
    xor r8d,r8d
    test r13d,r13d
    mov dword ptr [rsp + 0x38],r12d
    mov qword ptr [rsp + 0x40],r8
    jle .lbl180004745
    mov edi,dword ptr [rsp + 0xf0]
    mov eax,ecx
    imul eax,ebp
    cdq
    idiv r11d
    mov r10d,eax
    mov dword ptr [rsp + 0x68],eax
    lea eax,[rcx + 0x1]
    imul eax,ebp
    cdq
    idiv r11d
    mov r11d,eax
    mov dword ptr [rsp + 0x58],eax
    .lbl1800043b1: mov eax,r12d
    movzx esi,r11w
    sub si,r10w
    imul eax,edi
    cdq
    mov word ptr [rsp + 0x100],si
    idiv r13d
    mov r9d,eax
    mov dword ptr [rsp + 0x74],eax
    lea eax,[r12 + 0x1]
    mov dword ptr [rsp + 0x50],eax
    imul eax,edi
    cdq
    idiv r13d
    movsx edx,si
    mov dword ptr [rsp + 0x70],edx
    mov ebp,eax
    sub bp,r9w
    xor r9d,r9d
    test edx,edx
    mov dword ptr [rsp + 0x3c],r9d
    mov dword ptr [rsp + 0x24],ebp
    jle .lbl18000471d
    movsx r11d,bp
    mov dword ptr [rsp + 0x78],r11d
    nop
    .lbl180004410: xor r14d,r14d
    test r11d,r11d
    jle .lbl180004706
    lea eax,[r10 + r9*0x1]
    mov dword ptr [rsp + 0x48],eax
    mov eax,edx
    cdq
    sub eax,edx
    sar eax,1
    mov r10d,eax
    mov dword ptr [rsp + 0x60],eax
    mov eax,r11d
    cdq
    sub eax,edx
    sar eax,1
    mov ebx,eax
    mov dword ptr [rsp + 0x64],eax
    movzx eax,bp
    imul ax,si
    mov word ptr [rsp + 0xf8],ax
    movsxd rax,dword ptr [rsp + 0x74]
    mov qword ptr [rsp + 0x30],rax
    nop
    nop
    .lbl180004460: cmp r9d,r10d
    mov r11,r15
    jge .lbl18000447e
    test ecx,ecx
    jz .lbl180004476
    lea rdi,[r15 + -0x1]
    lea ecx,[r9 + r10*0x1]
    jmp .lbl180004498
    .lbl180004476: xor edi,edi
    lea ecx,[r9 + r10*0x1]
    jmp .lbl180004498
    .lbl18000447e: mov eax,dword ptr [rsp + 0x28]
    mov rdi,r15
    add eax,-0x1
    cmp ecx,eax
    jge .lbl180004490
    lea r11,[r15 + 0x1]
    .lbl180004490: movzx ecx,r9w
    sub cx,r10w
    .lbl180004498: movzx edx,si
    mov r9,r8
    sub dx,cx
    cmp r14d,ebx
    jge .lbl1800044be
    test r12d,r12d
    jz .lbl1800044b5
    lea r10,[r8 + -0x1]
    lea r12d,[r14 + rbx*0x1]
    jmp .lbl1800044d6
    .lbl1800044b5: xor r10d,r10d
    lea r12d,[r14 + rbx*0x1]
    jmp .lbl1800044d6
    .lbl1800044be: lea eax,[r13 + -0x1]
    mov r10,r8
    cmp r12d,eax
    jge .lbl1800044ce
    lea r9,[r8 + 0x1]
    .lbl1800044ce: movzx r12d,r14w
    sub r12w,bx
    .lbl1800044d6: mov rax,qword ptr [rsp + 0x88]
    mov r8w,word ptr [rsp + 0xf8]
    movzx r13d,bp
    sub r13w,r12w
    movzx ebp,r12w
    movzx ebx,r13w
    imul r12w,dx
    imul bp,cx
    imul r13w,dx
    movzx edx,r12w
    imul bx,cx
    mov rcx,rax
    imul rcx,r11
    imul rax,rdi
    lea rsi,[rcx + r9*0x1]
    lea r11,[rax + r10*0x1]
    add r9,rax
    mov rax,qword ptr [rsp + 0x128]
    add r10,rcx
    movzx ecx,word ptr [rax + r9*0x2]
    call EXTR_FUN_180004070
    movzx edx,bx
    movzx edi,ax
    mov rax,qword ptr [rsp + 0x128]
    movzx ecx,word ptr [rax + r10*0x2]
    call EXTR_FUN_180004070
    movzx edx,r13w
    add di,ax
    mov rax,qword ptr [rsp + 0x128]
    movzx ecx,word ptr [rax + r11*0x2]
    call EXTR_FUN_180004070
    movzx edx,bp
    add di,ax
    mov rax,qword ptr [rsp + 0x128]
    movzx ecx,word ptr [rax + rsi*0x2]
    call EXTR_FUN_180004070
    movzx edx,r12w
    mov r12,qword ptr [rsp + 0x80]
    movzx ecx,word ptr [r12 + r9*0x2]
    add di,ax
    call EXTR_FUN_180004070
    movzx ecx,word ptr [r12 + r10*0x2]
    movzx edx,bx
    movzx r9d,ax
    call EXTR_FUN_180004070
    movzx ecx,word ptr [r12 + r11*0x2]
    movzx edx,r13w
    add r9w,ax
    call EXTR_FUN_180004070
    movzx ecx,word ptr [r12 + rsi*0x2]
    movzx edx,bp
    add r9w,ax
    call EXTR_FUN_180004070
    add ax,r9w
    cwde
    cdq
    sub eax,edx
    sar eax,1
    mov r8d,eax
    movsx eax,di
    cdq
    sub eax,edx
    sar eax,1
    cmp eax,r8d
    mov r11d,eax
    jz .lbl18000468e
    movsxd rcx,dword ptr [rsp + 0xf0]
    movsx ebx,word ptr [rsp + 0x108]
    movsxd r10,dword ptr [rsp + 0x48]
    imul r10,rcx
    mov rdi,qword ptr [rsp + 0xe0]
    add r10,qword ptr [rsp + 0x30]
    sub r8w,ax
    mov eax,ebx
    cdq
    sub eax,edx
    sar eax,1
    mov ecx,eax
    movsx eax,word ptr [rsp + 0x110]
    cdq
    sub eax,edx
    sar eax,1
    mov r9d,eax
    movsx eax,word ptr [rdi + r10*0x2]
    cdq
    sub r9w,cx
    sub eax,edx
    movzx edx,r9w
    sar eax,1
    sub ax,r11w
    movzx ecx,ax
    call EXTR_FUN_180004070
    cmp ax,0x3fff
    jle .lbl18000466d
    mov ax,0x3fff
    add ax,ax
    add ax,bx
    mov word ptr [rdi + r10*0x2],ax
    mov edi,dword ptr [rsp + 0xf0]
    jmp .lbl1800046b8
    .lbl18000466d: cmp ax,0xc001
    mov ecx,0xffffc001
    cmovl ax,cx
    add ax,ax
    add ax,bx
    mov word ptr [rdi + r10*0x2],ax
    mov edi,dword ptr [rsp + 0xf0]
    jmp .lbl1800046b8
    .lbl18000468e: movsxd rax,dword ptr [rsp + 0x48]
    movsxd rdi,dword ptr [rsp + 0xf0]
    mov rcx,qword ptr [rsp + 0xe0]
    imul rax,rdi
    add rax,qword ptr [rsp + 0x30]
    movzx edx,word ptr [rsp + 0x108]
    mov word ptr [rcx + rax*0x2],dx
    .lbl1800046b8: mov r11d,dword ptr [rsp + 0x78]
    add qword ptr [rsp + 0x30],0x1
    mov r8,qword ptr [rsp + 0x40]
    mov r9d,dword ptr [rsp + 0x3c]
    mov r10d,dword ptr [rsp + 0x60]
    mov ebx,dword ptr [rsp + 0x64]
    mov si,word ptr [rsp + 0x100]
    mov ebp,dword ptr [rsp + 0x24]
    mov r12d,dword ptr [rsp + 0x38]
    mov ecx,dword ptr [rsp + 0x2c]
    mov r13d,dword ptr [rsp + 0x20]
    add r14d,0x1
    cmp r14d,r11d
    jl .lbl180004460
    mov edx,dword ptr [rsp + 0x70]
    mov r10d,dword ptr [rsp + 0x68]
    .lbl180004706: add r9d,0x1
    cmp r9d,edx
    mov dword ptr [rsp + 0x3c],r9d
    jl .lbl180004410
    mov r11d,dword ptr [rsp + 0x58]
    .lbl18000471d: mov r12d,dword ptr [rsp + 0x50]
    add r8,0x1
    cmp r12d,r13d
    mov dword ptr [rsp + 0x38],r12d
    mov qword ptr [rsp + 0x40],r8
    jl .lbl1800043b1
    mov r11d,dword ptr [rsp + 0x28]
    mov ebp,dword ptr [rsp + 0xe8]
    .lbl180004745: add ecx,0x1
    add r15,0x1
    cmp ecx,r11d
    mov dword ptr [rsp + 0x2c],ecx
    jl .lbl180004370
    .lbl180004759: xor eax,eax
    add rsp,0x98
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
EXTR_FUN_180004070:
    movsx eax,cx
    movsx ecx,dx
    imul eax,ecx
    movsx ecx,r8w
    cdq
    idiv ecx
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
EXTR_FUN_180012910:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rbp
    mov qword ptr [rsp + 0x18],rsi
    mov qword ptr [rsp + 0x20],rdi
    xor r9d,r9d
    mov esi,edx
    movsxd rbp,r8d
    mov r11,rcx
    mov ecx,r9d
    mov edx,r9d
    imul esi,ebp
    movsxd rbx,esi
    mov edi,r9d
    mov r10,r9
    cmp rbx,0x2
    jl .lbl180012972
    lea r8,[rbx + -0x1]
    nop
    nop
    .lbl180012950: mov eax,r9d
    cmp byte ptr [r11 + r10*0x1],al
    setnz al
    add ecx,eax
    mov eax,r9d
    cmp byte ptr [r11 + r10*0x1 + 0x1],al
    setnz al
    add r10,0x2
    add edx,eax
    cmp r10,r8
    jl .lbl180012950
    .lbl180012972: cmp r10,rbx
    jge .lbl18001297f
    cmp byte ptr [r10 + r11*0x1],dil
    setnz dil
    .lbl18001297f: lea eax,[rdx + rcx*0x1]
    mov rcx,r9
    add eax,edi
    imul eax,eax,0x64
    cdq
    idiv esi
    test ebp,ebp
    mov r10d,eax
    mov eax,r9d
    jle .lbl1800129a9
    .lbl180012997: cmp byte ptr [rcx + r11*0x1],r9b
    jnz .lbl1800129a9
    add rcx,0x1
    add eax,0x1
    cmp rcx,rbp
    jl .lbl180012997
    .lbl1800129a9: imul eax,eax,0x64
    cdq
    idiv ebp
    test ebp,ebp
    mov rdx,r9
    jle .lbl1800129d6
    lea rcx,[rbp + r11*0x1 + -0x1]
    nop
    nop
    .lbl1800129c0: cmp byte ptr [rcx],0x0
    jnz .lbl1800129d6
    add rdx,0x1
    add r9d,0x1
    sub rcx,0x1
    cmp rdx,rbp
    jl .lbl1800129c0
    .lbl1800129d6: cmp eax,0x5
    jl .lbl1800129ed
    imul r9d,r9d,0x64
    mov eax,r9d
    cdq
    idiv ebp
    cmp eax,0x5
    jl .lbl1800129ed
    neg r10d
    .lbl1800129ed: mov rbx,qword ptr [rsp + 0x8]
    mov rbp,qword ptr [rsp + 0x10]
    mov rsi,qword ptr [rsp + 0x18]
    mov rdi,qword ptr [rsp + 0x20]
    mov eax,r10d
    ret

section .data


section .text
EXTR_FUN_180012bc0:
    mov r10d,edx
    xor eax,eax
    imul r10d,r8d
    test r10d,r10d
    movsxd r8,r10d
    jle .lbl180012bed
    sub r9,rcx
    .lbl180012bd4: cmp byte ptr [r9 + rcx*0x1],0x0
    jz .lbl180012be0
    add eax,0x1
    jmp .lbl180012be3
    .lbl180012be0: mov byte ptr [rcx],0x80
    .lbl180012be3: add rcx,0x1
    sub r8,0x1
    jnz .lbl180012bd4
    .lbl180012bed: imul eax,eax,0x64
    cdq
    idiv r10d
    ret

section .data


section .text
EXTR_FUN_18000c030:
    mov qword ptr [rsp + 0x8],rbx
    mov qword ptr [rsp + 0x10],rsi
    push rdi
    sub rsp,0x40
    cmp word ptr [rdx + 0x14],0x0
    mov rsi,r8
    mov rbx,rdx
    mov rdi,rcx
    jz .lbl18000c084
    cmp word ptr [rdx + 0x12],0x0
    jz .lbl18000c084
    cmp byte ptr [rdx + 0x58],0x0
    jnz .lbl18000c084
    call EXTR_weird_pprocess_wrapper
    lea r9,[rsp + 0x30]
    xor r8d,r8d
    mov rdx,rbx
    mov rcx,rdi
    mov qword ptr [rsp + 0x20],rsi
    call EXTR_FUN_18000bfe0
    mov rdx,rbx
    mov rcx,rdi
    call EXTR_FUN_18000c000
    .lbl18000c084: mov rbx,qword ptr [rsp + 0x50]
    mov rsi,qword ptr [rsp + 0x58]
    add rsp,0x40
    pop rdi
    ret

section .data


section .text
EXTR_tudorIplGetImage:
    mov qword ptr [rsp + 0x10],rdx
    mov qword ptr [rsp + 0x8],rcx
    sub rsp,0x58
    mov dword ptr [rsp + 0x38],0x0
    mov qword ptr [rsp + 0x48],0x0
    mov qword ptr [rsp + 0x40],0x0
    mov dword ptr [rsp + 0x34],0x0
    mov dword ptr [rsp + 0x30],0x0
    cmp qword ptr [glb180179f90],0x0
    jz .lbl180087b6c
    lea r8,[glb1800db760]
    lea rdx,[glb18016e260]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funEnter
    .lbl180087b6c: cmp qword ptr [rsp + 0x68],0x0
    jnz .lbl180087bd8
    .lbl180087b74: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087bc5
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180087bc5
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180087bc5
    lea rax,[glb18016e268]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0xf18a5fe2
    lea r8,[glb18016e278]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087bc5: xor eax,eax
    test eax,eax
    jnz .lbl180087b74
    mov dword ptr [rsp + 0x38],0x6f
    jmp .lbl180087d93
    .lbl180087bd8: mov rax,qword ptr [rsp + 0x68]
    mov qword ptr [rax + 0x8],0x0
    mov rax,qword ptr [rsp + 0x68]
    mov dword ptr [rax],0x0
    mov rax,qword ptr [rsp + 0x60]
    mov rax,qword ptr [rax + 0x28]
    mov qword ptr [rsp + 0x48],rax
    mov rax,qword ptr [rsp + 0x60]
    mov rcx,qword ptr [rsp + 0x60]
    mov ecx,dword ptr [rcx + 0x18]
    cmp dword ptr [rax + 0x14],ecx
    jc .lbl180087c2a
    mov rax,qword ptr [rsp + 0x60]
    mov eax,dword ptr [rax + 0x18]
    mov dword ptr [rsp + 0x34],eax
    mov rax,qword ptr [rsp + 0x60]
    mov eax,dword ptr [rax + 0x14]
    mov dword ptr [rsp + 0x30],eax
    jmp .lbl180087c42
    .lbl180087c2a: mov rax,qword ptr [rsp + 0x60]
    mov eax,dword ptr [rax + 0x14]
    mov dword ptr [rsp + 0x34],eax
    mov rax,qword ptr [rsp + 0x60]
    mov eax,dword ptr [rax + 0x18]
    mov dword ptr [rsp + 0x30],eax
    .lbl180087c42: mov eax,dword ptr [rsp + 0x30]
    imul eax,dword ptr [rsp + 0x34]
    add eax,0x15
    mov rcx,qword ptr [rsp + 0x68]
    mov dword ptr [rcx],eax
    mov rax,qword ptr [rsp + 0x68]
    mov ecx,dword ptr [rax]
    call EXTR_malloc wrt ..plt
    mov rcx,qword ptr [rsp + 0x68]
    mov qword ptr [rcx + 0x8],rax
    mov rax,qword ptr [rsp + 0x68]
    cmp qword ptr [rax + 0x8],0x0
    jnz .lbl180087ce8
    cmp dword ptr [rsp + 0x38],0x0
    jz .lbl180087c84
    cmp dword ptr [rsp + 0x38],0x6e
    jnz .lbl180087c8c
    .lbl180087c84: mov dword ptr [rsp + 0x38],0x25a
    .lbl180087c8c: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087cdd
    mov rax,qword ptr [glb180179f90]
    mov eax,dword ptr [rax]
    and eax,0x8
    test eax,eax
    jz .lbl180087cdd
    mov rax,qword ptr [glb180179f90]
    cmp dword ptr [rax + 0x4],0x4
    ja .lbl180087cdd
    lea rax,[glb18016e280]
    mov qword ptr [rsp + 0x20],rax
    mov r9d,0x7fbdc693
    lea r8,[glb18016e2a0]
    mov edx,0x4
    mov rcx,qword ptr [glb180179f90]
    call EXTR_log
    .lbl180087cdd: xor eax,eax
    test eax,eax
    jnz .lbl180087c8c
    jmp .lbl180087d93
    .lbl180087ce8: mov rax,qword ptr [rsp + 0x68]
    mov r8d,dword ptr [rax]
    xor edx,edx
    mov rax,qword ptr [rsp + 0x68]
    mov rcx,qword ptr [rax + 0x8]
    call EXTR_memset2
    mov rax,qword ptr [rsp + 0x68]
    mov rax,qword ptr [rax + 0x8]
    mov qword ptr [rsp + 0x40],rax
    mov eax,0x8
    mov rcx,qword ptr [rsp + 0x40]
    mov word ptr [rcx + 0x10],ax
    mov rax,qword ptr [rsp + 0x40]
    mov rcx,qword ptr [rsp + 0x60]
    movzx ecx,word ptr [rcx + 0x38]
    mov word ptr [rax + 0x12],cx
    mov rax,qword ptr [rsp + 0x40]
    movzx ecx,word ptr [rsp + 0x30]
    mov word ptr [rax + 0xc],cx
    mov rax,qword ptr [rsp + 0x40]
    movzx ecx,word ptr [rsp + 0x34]
    mov word ptr [rax + 0xe],cx
    mov rax,qword ptr [rsp + 0x40]
    mov rcx,qword ptr [rsp + 0x60]
    mov ecx,dword ptr [rcx + 0x1c]
    mov dword ptr [rax],ecx
    mov rax,qword ptr [rsp + 0x40]
    mov rcx,qword ptr [rsp + 0x60]
    movzx ecx,byte ptr [rcx + 0x20]
    mov byte ptr [rax + 0x14],cl
    mov eax,dword ptr [rsp + 0x30]
    imul eax,dword ptr [rsp + 0x34]
    mov ecx,0x1
    imul rcx,rcx,0x0
    mov rdx,qword ptr [rsp + 0x40]
    lea rcx,[rdx + rcx*0x1 + 0x15]
    mov r8d,eax
    mov rdx,qword ptr [rsp + 0x48]
    call EXTR_memcpy wrt ..plt
    .lbl180087d93: cmp qword ptr [glb180179f90],0x0
    jz .lbl180087db7
    lea r8,[glb1800db760]
    lea rdx,[glb18016e2a8]
    mov rcx,qword ptr [glb180179f90]
    call EXTR_funLeave
    .lbl180087db7: mov eax,dword ptr [rsp + 0x38]
    add rsp,0x58
    ret

section .data

glb18016e268: db 112, 73, 109, 97, 103, 101, 32, 105, 115, 32, 78, 85, 76, 76, 0
glb18016e278: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e2a8: db 115, 121, 110, 97, 76, 105, 98, 0
glb1800db760: db 116, 117, 100, 111, 114, 73, 112, 108, 71, 101, 116, 73, 109, 97, 103, 101, 0
glb18016e260: db 115, 121, 110, 97, 76, 105, 98, 0
glb18016e280: db 77, 101, 109, 111, 114, 121, 32, 97, 108, 108, 111, 99, 97, 116, 105, 111, 110, 32, 102, 97, 105, 108, 101, 100, 46, 10, 0
glb18016e2a0: db 115, 121, 110, 97, 76, 105, 98, 0

