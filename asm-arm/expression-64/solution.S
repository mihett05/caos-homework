.global f
.text

f:
    mul x8, x3, x3     // x**2
    mul x8, x8, x0     // A * x ** 2
    mul x9, x1, x3     // B * x
    add x8, x8, x9     // A * x ** 2 + B * x
    add x8, x8, x2     // A * x ** 2 + B * x + C
    mov x0, x8
    ret