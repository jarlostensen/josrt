[bits 64]
section .text

global _syscall_write:function
;; void _syscall_write(const char* str, int len);
_syscall_write:     
    mov rdx, rsi
    mov rsi, rdi        ;; string pointer
    mov rax, 1          ;; write and stdout both
    mov rdi, rax
    syscall
    ret

global __exit:function
;; this function is used by __libc_main
;; void __exit(int);
__exit:
    mov rax, 0x3c
    syscall

