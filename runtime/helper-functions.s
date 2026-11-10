.bss
    MAX_INT_WRITE_BUFF: .space 11  // int has max digits of 10, plus '-'

.text
.global PRINT_INT
PRINT_INT:
    // pass in w0 as an argument
    // Calculate w0 mod 10, store it in w0

    mov w5, #10                     // use as digit offset (added in reverse)
    mov w8, #0                      // used as digit count
    mov w6, #10                     // used as constant
    mov w9, #0                      // negative flag
    ldr x7, =MAX_INT_WRITE_BUFF

    cmp w0, #0                      // make the number positive if negative but flag it
    b.ge PRINT_INT_LOOP
    neg w0, w0
    mov w9, #1

PRINT_INT_LOOP:
    mov w3, w0		                // save num
    udiv w0, w0, w6	                // integer dividing num by 10
    msub w4, w0, w6, w3 	        // w4 = w3 - (w0 * 10), next digit

    add w4, w4, #48		            // turn w4 into ascii

    strb w4, [x7, x5]	            // put the digit in the buffer at the offset of total digits
    sub w5, w5, #1
    add w8, w8, #1

    cmp w0, #0
    beq 1f

    b PRINT_INT_LOOP

1:
    // add a '-' to the front of the number if negative
    cbz w9, 2f
    mov w4, #45
    strb w4, [x7, x5]
    sub w5, w5, #1
    add w8, w8, #1

2:
    stp x29, x30, [sp, #-16]!
    add x1, x7, x5
    add x1, x1, #1
    mov w2, w8
    bl WRITE
    ldp x29, x30, [sp], #16

    ret
