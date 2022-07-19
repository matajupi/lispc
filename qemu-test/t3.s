.LC0:
        .ascii  "%d\012\000"
main:
        stmfd   sp!, {fp, lr}
        add     fp, sp, #4
        sub     sp, sp, #8
        mov     r0, #256
        mov     r1, #4
        bl      calloc
        mov     r3, r0
        str     r3, [fp, #-8]
        ldr     r3, [fp, #-8]
        add     r3, r3, #12
        mov     r2, #3
        str     r2, [r3, #0]
        ldr     r3, [fp, #-8]
        add     r3, r3, #16
        mov     r2, #10
        str     r2, [r3, #0]
        ldr     r2, .L2
        ldr     r3, [fp, #-8]
        add     r3, r3, #16
        ldr     r3, [r3, #0]
        mov     r0, r2
        mov     r1, r3
        bl      printf
        ldr     r0, [fp, #-8]
        bl      free
        ldr     r3, [fp, #-8]
        add     r3, r3, #12
        ldr     r3, [r3, #0]
        mov     r0, r3
        sub     sp, fp, #4
        ldmfd   sp!, {fp, pc}
.L2:
        .word   .LC0
