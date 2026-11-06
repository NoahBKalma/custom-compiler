.include "defs.inc"

.data
    newline: .ascii "\n"
    s0: .ascii "Hello world"
    s1: .ascii "3"

.text
.global _start

_start:
    stp x29, x30, [sp, #-16]!
    ldr x1, =s0
    mov x2, #11
    bl WRITE
    ldp x29, x30, [sp], #16
    mov w0, #3
    stp x29, x30, [sp, #-16]!
    ldr x1, =s1
    mov x2, #1
    bl WRITE
    ldp x29, x30, [sp], #16


    ldr x1, =newline
mov x2, #1
bl WRITE

    mov x0, #0
    b EXIT
