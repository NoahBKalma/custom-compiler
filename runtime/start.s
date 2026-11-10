.include "defs.inc"

.data
    newline: .ascii "\n"
    s0: .ascii "Hello world"

.text
.global _start

_start:
    stp x29, x30, [sp, #-16]!
    ldr x1, =s0
    mov x2, #11
    bl WRITE
    ldp x29, x30, [sp], #16
    ldr x1, =newline
    mov x2, #1
    bl WRITE
    ldr w9, =1079194419
    fmov s0, w9
    str w0, [sp, #-16]!
    mov w0, #2


    ldr x1, =newline
mov x2, #1
bl WRITE

    mov x0, #0
    b EXIT
