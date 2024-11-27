.intel_syntax noprefix
.global _start
.text
_start:
    mov rax, 59
    lea rdi, [tac]
    lea rsi, [tac]
    mov rdx, 0
    syscall

.data
tac:
    .asciz "tac"