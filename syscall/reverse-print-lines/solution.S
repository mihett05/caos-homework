.intel_syntax noprefix
.global _start

.text
_start:
    mov rax, 59
    lea rdi, [tac]
    lea rsi, [argv]
    xor rdx, rdx
    syscall

    mov rax, 60
    mov rdi, 0
    syscall

    ret

.data
tac:
    .asciz "/usr/bin/tac"
argv:
    .quad tac
    .quad 0