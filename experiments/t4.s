callback_func1:
        str     fp, [sp, #-4]!
        add     fp, sp, #0
        sub     sp, sp, #12
        str     r0, [fp, #-8]
        str     r1, [fp, #-12]
        ldr     r2, [fp, #-8]
        ldr     r3, [fp, #-12]
        add     r3, r2, r3
        mov     r0, r3
        add     sp, fp, #0
        ldmfd   sp!, {fp}
        bx      lr
callback_func2:
        str     fp, [sp, #-4]!
        add     fp, sp, #0
        sub     sp, sp, #12
        str     r0, [fp, #-8]
        str     r1, [fp, #-12]
        ldr     r3, [fp, #-8]
        ldr     r2, [fp, #-12]
        mul     r3, r2, r3
        mov     r0, r3
        add     sp, fp, #0
        ldmfd   sp!, {fp}
        bx      lr
primary_func:
        stmfd   sp!, {r4, fp, lr}
        add     fp, sp, #8
        sub     sp, sp, #20
        str     r0, [fp, #-16]
        str     r1, [fp, #-20]
        str     r2, [fp, #-24]
        ldr     r3, [fp, #-16]
        ldr     r0, [fp, #-20]
        ldr     r1, [fp, #-24]
        blx     r3
        mov     r4, r0
        ldr     r2, [fp, #-20]
        ldr     r3, [fp, #-24]
        add     r1, r2, r3
        ldr     r2, [fp, #-20]
        ldr     r3, [fp, #-24]
        rsb     r2, r3, r2
        ldr     r3, [fp, #-16]
        mov     r0, r1
        mov     r1, r2
        blx     r3
        mov     r3, r0
        add     r3, r4, r3
        mov     r0, r3
        sub     sp, fp, #8
        ldmfd   sp!, {r4, fp, pc}
.LC0:
        .ascii  "%d\000"
.LC1:
        .ascii  "%d\012\000"
main:
        stmfd   sp!, {fp, lr}
        add     fp, sp, #4
        sub     sp, sp, #16
        ldr     r3, .L7
        str     r3, [fp, #-12]
        ldr     r3, .L7+4
        str     r3, [fp, #-16]
        ldr     r2, .L7+8
        sub     r3, fp, #20
        mov     r0, r2
        mov     r1, r3
        bl      __isoc99_scanf
        ldr     r3, [fp, #-20]
        cmp     r3, #0
        ble     .L5
        ldr     r0, [fp, #-12]
        mov     r1, #3
        mov     r2, #4
        bl      primary_func
        str     r0, [fp, #-8]
        b       .L6
.L5:
        ldr     r0, [fp, #-16]
        mov     r1, #3
        mov     r2, #4
        bl      primary_func
        str     r0, [fp, #-8]
.L6:
        ldr     r3, .L7+12
        mov     r0, r3
        ldr     r1, [fp, #-8]
        bl      printf
        mov     r3, #0
        mov     r0, r3
        sub     sp, fp, #4
        ldmfd   sp!, {fp, pc}
.L7:
        .word   callback_func1
        .word   callback_func2
        .word   .LC0
        .word   .LC1
