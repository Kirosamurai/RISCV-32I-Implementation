add x1, x0, x0
.here:
addi x2, x0, 5
addi x1, x1, 1
add x0, x0, x0
add x0, x1, x2
add x0, x3, x4
add x0, x0, x0
add x0, x1, x2
add x0, x3, x4
add x0, x0, x0
add x0, x1, x2
add x0, x3, x4
bne x2, x1, .here