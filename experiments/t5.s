.section .text
.global main
main:
    stmfd sp!, {fp, lr}
    mov fp, sp
    ldr r0, .LambdaWord+0
    push {r0}
    pop {r4}
    blx r4
    push {r0}
    pop {r0}
    ldr r0, [sp]
    mov sp, fp
    ldmfd sp!, {fp, pc}
.Lambda0
    stmfd sp!, {fp, lr}
    mov fp, sp
    mov r0, #8
    bl malloc
    mov r1, #1
    str r1, [r0]
    push {r0}
    mov r1
    mov r0, #8
    bl malloc
    mov r1, #0
    str r1, [r0]
    mov r1, #3
    str r1, [r0, #4]
    push {r0}
    pop {r0}
    ldr r1, [fp, #4]
    ldr r2, [r1]
    sub r2, r2, #1
