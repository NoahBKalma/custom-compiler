.include "defs.inc"

.data
    newline: .ascii "
"
    s0: .ascii "Hello World\n"
    s1: .ascii "this is on line 3\n"
    s2: .ascii "So is this.\n"

.text
.global _start

_start:
    stp x29, x30, [sp, #-16]!
    ldr x1, =s0
    mov x2, #13
    bl WRITE
    ldp x29, x30, [sp], #16
    stp x29, x30, [sp, #-16]!
    ldr x1, =s1
    mov x2, #19
    bl WRITE
    ldp x29, x30, [sp], #16
    stp x29, x30, [sp, #-16]!
    ldr x1, =s2
    mov x2, #13
    bl WRITE
    ldp x29, x30, [sp], #16


    ldr x1, =newline
mov x2, #1
bl WRITE

    mov x0, #0
    b EXIT
