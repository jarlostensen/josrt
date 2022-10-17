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

global _syscall_exit:function
;; void _syscall_exit(void);
_syscall_exit:
    mov rax, 0x3c
    xor rdi, rdi
    syscall

