.include "defs.inc"

.text
.global _start

_start:
    ldr x0, [sp, #16]   // x0 = *argv[1]

    ldrb w0, [x0]
    sub w0, w0, #'0'
    b EXIT
