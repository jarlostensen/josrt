section .text
[bits 64]

extern __libc_start_main
extern main

;; also https://wiki.osdev.org/Creating_a_C_Library
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
    ;; unreachable
    hlt
.end:

