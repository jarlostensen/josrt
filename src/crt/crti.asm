[bits 64]
section .init
global _init:function
_init:
    push rbp
    mov  rbp, rsp
    ;; compiler will stuff code here

section .fini
global _fini:function
_fini:
    push rbp
    mov  rbp, rsp
    ;; compiler will stuff code here
