.include "defs.inc"

.text
.global EXIT, WRITE
EXIT:
    // pass x0 as argument for exit code
    mov x8, #EXIT_CODE
    svc #0

WRITE:
    // pass x1 as the buffer, x2 as the size
    mov x0, #1  // stdout
    mov x8, #WRITE_CODE
    svc #0
    ret
