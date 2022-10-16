section .text
[bits 64]

extern main
extern __libc_start_main

global _start:function (_start.end - _start)
_start:
    xor        ebp,ebp
    mov        r9,rdx
    pop        rsi
    mov        rdx,rsp
    and        rsp,-0x10
    push       rax
    push       rsp
    xor        r8d,r8d
    xor        ecx,ecx
    lea        rdi,[main]
    call       __libc_start_main
    hlt
.end:

