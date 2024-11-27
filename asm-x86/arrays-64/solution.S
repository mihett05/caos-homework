.intel_syntax noprefix
.global sum

.text
sum:
    xor r10d, r10d // A + B value
    mov r11, rcx // R array
    mov ecx, edi
    xor r13, r13

iter:
    mov r10d, [rsi + r13]
    add r10d, [rdx + r13]
    mov [r11 + r13], r10d

    add r13, 4
    loop iter

    ret
