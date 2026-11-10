.bss
    MAX_INT_WRITE_BUFF: .space 11   // int has max digits of 10, plus '-'
    MAX_FLOAT_WRITE_BUFF: .space 21 // float has max digits of 19, plus '-' and '.'

.text
.global PRINT_INT, PRINT_FLOAT
PRINT_INT:
    // pass in w0 as an argument

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

    // Calculate w0 mod 10, store it in w0

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


PRINT_FLOAT:
    // pass in s0 as an argument
    // s0 = float to print
    // turn s0 to x0 (multiply by 1,000,000 and store as int so printing has 6 trailing digits)

    mov w5, #20                     // use as digit offset (added in reverse)
    mov w8, #0                      // used as digit count
    mov w6, #10                     // used as constant

    ldr x10, =1000000
    scvtf s1, x10                   // used as constant
    mov w9, #0                      // negative flag
    ldr x7, =MAX_FLOAT_WRITE_BUFF

    fmul s0, s0, s1
    fcvtzs x0, s0                   // signed convert float to int
    cmp x0, #0                      // make the number positive if negative but flag it
    b.ge PRINT_FLOAT_LOOP
    neg x0, x0
    mov w9, #1

PRINT_FLOAT_LOOP:

    // Calculate x0 mod 10, store it in x0
    mov x3, x0		                // save num
    udiv x0, x0, x6	                // integer dividing num by 10
    msub x4, x0, x6, x3 	        // x4 = x3 - (x0 * 10), next digit

    add x4, x4, #48		            // turn x4 into ascii

    strb w4, [x7, x5]	            // put the digit in the buffer at the offset of total digits
    sub w5, w5, #1
    add w8, w8, #1

    cmp w8, #6                      // insert a '.' after 6 digits
    b.ne 1f
    mov w4, #0x2E
    strb w4, [x7, x5]
    sub w5, w5, #1
    add w8, w8, #1

1:
    cmp x0, #0
    beq 2f

    b PRINT_FLOAT_LOOP

2:
    // add a leading 0 for numbers <1
    cmp w8, #7                      // check for trailing 6 + '.'
    b.ne 3f

    mov w4, #0x30
    strb w4, [x7, x5]
    sub w5, w5, #1
    add w8, w8, #1

3:
    // add a '-' to the front of the number if negative
    cbz w9, 4f
    mov w4, #0x2D
    strb w4, [x7, x5]
    sub w5, w5, #1
    add w8, w8, #1

4:
    stp x29, x30, [sp, #-16]!
    add x1, x7, x5
    add x1, x1, #1
    mov x2, x8
    bl WRITE
    ldp x29, x30, [sp], #16

    ret

