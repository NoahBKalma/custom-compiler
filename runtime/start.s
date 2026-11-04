.include "defs.inc"

.data
    s0: .ascii "Hello World"

.text
.global _start

_start:
    stp x29, x30, [sp, #-16]!
    ldr x1, =s0
    mov x2, #11
    ldp x29, x30, [sp], #16

    mov x0, #0
    b EXIT
