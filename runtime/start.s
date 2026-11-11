.include "defs.inc"

.data
    newline: .ascii "\n"
    s0: .ascii "Hello"
    s1: .ascii "Helloworld"
    s2: .ascii "3 + 1.5"

.text
.global _start

_start:
    stp x29, x30, [sp, #-16]!
    ldr x1, =
    mov x2, #1
    bl WRITE
    ldp x29, x30, [sp], #16
    ldr x1, =newline
    mov x2, #1
    bl WRITE
    mov w0, #3
    str w0, [sp, #-16]!
    mov w0, #2
    neg w0, w0
    str w0, [sp, #-16]!
    mov w0, #4
    mov w1, w0
    ldr w0, [sp], #16
    mul w0, w0, w1
    mov w1, w0
    ldr w0, [sp], #16
    add w0, w0, w1
    bl PRINT_INT
    ldr x1, =newline
    mov x2, #1
    bl WRITE
    ldr w9, =1045220557
    fmov s0, w9
    str s0, [sp, #-16]!
    mov w0, #1
    scvtf s1, w0
    ldr s0, [sp], #16
    fadd s0, s0, s1
    bl PRINT_FLOAT
    ldr x1, =newline
    mov x2, #1
    bl WRITE
    mov w0, #3
    str w0, [sp, #-16]!
    ldr w9, =1069547520
    fmov s0, w9
    fmov s1, s0
    ldr w0, [sp], #16
    scvtf s0, w0
    fadd s0, s0, s1
    bl PRINT_FLOAT
    ldr x1, =newline
    mov x2, #1
    bl WRITE
    stp x29, x30, [sp, #-16]!
    ldr x1, =s2
    mov x2, #7
    bl WRITE
    ldp x29, x30, [sp], #16
    ldr x1, =newline
    mov x2, #1
    bl WRITE


    ldr x1, =newline
    mov x2, #1
    bl WRITE

    mov x0, #0
    b EXIT
