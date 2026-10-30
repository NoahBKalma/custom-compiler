.include "defs.inc"

.text
.global EXIT
EXIT:
    // pass x1 as argument for exit code
    mov x8, #EXIT_CODE
    svc #0
