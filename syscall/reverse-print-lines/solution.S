.intel_syntax noprefix
.global _start

.text
_start:
    // pipe
    mov rax, 22
    lea rdi, [pipe_r]
    syscall

    // pipe == -1
    cmp rax, -1
    je fail

    // creating new pipe for fork output
    mov rax, 22
    lea rdi, [out_pipe_r]
    syscall

    // pipe == -1
    cmp rax, -1
    je fail

exec_tac:
    // fork
    mov rax, 57
    syscall

    mov r10, rax

    cmp rax, 0
    jne parent

child:
    // dup2 pipe_r -> stdin
    mov rax, 33
    mov rdi, pipe_r
    mov rsi, 0 // stdin
    syscall

    // dup2 stdout -> out_pipe_w
    mov rax, 33
    mov rdi, out_pipe_w
    mov rsi, 1 // stdout
    syscall

    // closing pipes
    mov rax, 3
    mov rdi, pipe_w
    syscall

    mov rax, 3
    mov rdi, pipe_r
    syscall

    mov rax, 3
    mov rdi, out_pipe_w
    syscall

    mov rax, 3
    mov rdi, out_pipe_r
    syscall

    // execve
    mov rax, 59
    lea rdi, [tac]
    lea rsi, [argv]
    xor rdx, rdx
    syscall
    
    jmp fail

parent:
    // closing pipes
    mov rax, 3
    mov rdi, pipe_r
    syscall

    mov rax, 3
    mov rdi, out_pipe_w
    syscall

loop_read:
    // read
    mov rax, 0
    mov rdi, 0
    lea rsi, [buffer]
    mov rdx, 4096
    syscall

    add total, rax

    // read == 0
    cmp rax, 0
    je exec

    mov r10, rax
    dec rax // get last index
    mov al, [buffer + rax]
    mov last, al

loop_write:

    cmp r10, 0
    je loop_read

    // write buffer
    mov rax, 1
    mov rdi, pipe_w
    lea rsi, [buffer]
    mov rdx, r10
    syscall

    sub r10, rax
    jmp loop_write

exec:
    // if no input
    cmpq total, 0
    je parent_wait

    // no \n at the end
    cmpq last, 10
    je parent_wait

    // adding \n at the end
    // write
    mov rax, 1
    mov rdi, pipe_w
    lea rsi, [lf]
    mov rdx, 1
    syscall

parent_wait:
    // closing pipes
    mov rax, 3
    mov rdi, pipe_w
    syscall

    // wait4
    mov rax, 61
    mov rdi, r10
    mov rsi, 0
    mov rdi, 0
    mov r10, 0
    syscall

    xor r10, r10

out_loop_read:
    // read
    mov rax, 0
    mov rdi, out_pipe_r
    lea rsi, [out_buffer]
    mov rdx, 4096
    syscall

    // read == 0
    cmp rax, 0
    je success

    cmp r10, 4096
    je out_loop_write

    mov r10, rax
    dec rax // get last index
    mov al, [out_buffer + rax]

    cmp al, 10
    jne out_loop_write
    dec r10


out_loop_write:
    cmp r10, 0
    je out_loop_read

    // write buffer
    mov rax, 1
    mov rdi, 1
    lea rsi, [out_buffer]
    mov rdx, r10
    syscall

    sub r10, rax
    jmp out_loop_write

success:
    mov rax, 3
    mov rdi, out_pipe_r
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
fail:
    // exit(1)
    mov rax, 60
    mov rdi, 1
    syscall

    ret

.data
tac:
    .asciz "/usr/bin/tac"
argv:
    .quad tac
    .quad 0
pipe_r:
    .long 0
pipe_w:
    .long 1
out_pipe_r:
    .long 0
out_pipe_w:
    .long 1
buffer:
    .space 4096
out_buffer:
    .space 4096
last:
    .byte 0
total:
    .quad 0
lf:
    .asciz "\n"
