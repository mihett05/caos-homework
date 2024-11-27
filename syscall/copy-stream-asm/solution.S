.intel_syntax noprefix
.global _start
.text
_start:
    mov rbx, 0  // read amount
read_loop:
    // read
    mov rax, 0
    mov rdi, 0
    lea rsi, [buffer]
    mov rdx, 1024
    syscall

    // read == -1
    mov rbx, rax
    cmp rbx, -1
    je exit_fail

    // read == 0
    cmp rbx, 0
    je exit_success

    // write
    mov rax, 1
    mov rdi, 1
    lea rsi, [buffer]
    mov rdx, rbx
    syscall

    // write == -1
    cmp rax, -1
    je exit_fail

    // loop
    jmp read_loop

exit_success:
    mov rax, 60
    mov rdi, 0
    syscall
    jmp end
exit_fail:
    mov rax, 60
    mov rdi, 1
    syscall
end:


.data
buffer:
    .fill 1024, 1, 0