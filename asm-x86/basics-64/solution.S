.intel_syntax noprefix
.global very_important_function
.extern A

.text
very_important_function:
    mov rcx, rdi
    xor r13, r13
    mov r12, A

iter:
    // input
    push rcx
    lea rdi, [fmt_input]
    lea rsi, [input]
    xor rax, rax
    call scanf
    pop rcx

    // A[i] * input
    movsxd r11, [input]
    movsxd rax, [r12 + r13]
    imul r11

    // output
    push rcx
    lea rdi, [fmt_output]
    mov rsi, rax
    xor rax, rax
    call printf
    pop rcx

    // iteration
    add r13, 4
    loop iter

    ret


.data
fmt_input:
    .asciz "%d"
fmt_output:
    .asciz "%lld "
input:
    .quad 0