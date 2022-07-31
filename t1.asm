	.file	"t1.c"
	.text
	.comm	sp,8,8
	.comm	fp,8,8
	.comm	ret,8,8
	.comm	r0,8,8
	.comm	r1,8,8
	.comm	r2,8,8
	.comm	r3,8,8
	.comm	env,8,8
	.comm	benv,8,8
	.comm	proc,8,8
	.comm	stack,8192,32
	.globl	push
	.type	push, @function
push:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	sp(%rip), %rax
	subq	$1, %rax
	movq	%rax, sp(%rip)
	movq	sp(%rip), %rax
	leaq	0(,%rax,8), %rcx
	leaq	stack(%rip), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, (%rcx,%rdx)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	push, .-push
	.globl	pop
	.type	pop, @function
pop:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	sp(%rip), %rax
	leaq	1(%rax), %rdx
	movq	%rdx, sp(%rip)
	leaq	0(,%rax,8), %rdx
	leaq	stack(%rip), %rax
	movq	(%rdx,%rax), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	pop, .-pop
	.comm	args,512,32
	.globl	initMachine
	.type	initMachine, @function
initMachine:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	$1024, fp(%rip)
	movq	fp(%rip), %rax
	movq	%rax, sp(%rip)
	movq	$0, r3(%rip)
	movq	r3(%rip), %rax
	movq	%rax, r2(%rip)
	movq	r2(%rip), %rax
	movq	%rax, r1(%rip)
	movq	r1(%rip), %rax
	movq	%rax, r0(%rip)
	movq	r0(%rip), %rax
	movq	%rax, ret(%rip)
	movq	$0, benv(%rip)
	movq	benv(%rip), %rax
	movq	%rax, env(%rip)
	movq	$0, proc(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	initMachine, .-initMachine
	.globl	call
	.type	call, @function
call:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	proc(%rip), %rax
	movq	8(%rax), %rax
	movq	%rax, benv(%rip)
	movq	proc(%rip), %rax
	movq	(%rax), %rdx
	movl	$0, %eax
	call	*%rdx
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	call, .-call
	.globl	stmfd
	.type	stmfd, @function
stmfd:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	env(%rip), %rax
	movq	%rax, %rdi
	call	push
	movq	fp(%rip), %rax
	movq	%rax, %rdi
	call	push
	movq	sp(%rip), %rax
	movq	%rax, fp(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	stmfd, .-stmfd
	.globl	ldmfd
	.type	ldmfd, @function
ldmfd:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	fp(%rip), %rax
	movq	%rax, sp(%rip)
	movl	$0, %eax
	call	pop
	movq	%rax, fp(%rip)
	movl	$0, %eax
	call	pop
	movq	%rax, env(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	ldmfd, .-ldmfd
	.globl	main
	.type	main, @function
main:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movl	$0, %eax
	call	initMachine
	movl	$16, %edi
	call	malloc@PLT
	movq	%rax, env(%rip)
	movq	env(%rip), %rbx
	movl	$0, %edi
	call	malloc@PLT
	movq	%rax, (%rbx)
	movq	env(%rip), %rax
	movq	benv(%rip), %rdx
	movq	%rdx, 8(%rax)
	movl	$16, %edi
	call	malloc@PLT
	movq	%rax, r0(%rip)
	movq	r0(%rip), %rax
	leaq	Lambda0(%rip), %rdx
	movq	%rdx, (%rax)
	movq	r0(%rip), %rax
	movq	env(%rip), %rdx
	movq	%rdx, 8(%rax)
	movq	r0(%rip), %rax
	movq	%rax, %rdi
	call	push
	movl	$0, %eax
	call	pop
	movq	%rax, proc(%rip)
	movl	$0, %eax
	call	call
	movq	ret(%rip), %rax
	movq	%rax, %rdi
	call	push
	movl	$0, %eax
	call	pop
	movq	%rax, ret(%rip)
	movq	ret(%rip), %rax
	movq	(%rax), %rax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	main, .-main
	.globl	Lambda0
	.type	Lambda0, @function
Lambda0:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movl	$0, %eax
	call	stmfd
	movl	$16, %edi
	call	malloc@PLT
	movq	%rax, env(%rip)
	movq	env(%rip), %rbx
	movl	$0, %edi
	call	malloc@PLT
	movq	%rax, (%rbx)
	movq	env(%rip), %rax
	movq	benv(%rip), %rdx
	movq	%rdx, 8(%rax)
	movl	$8, %edi
	call	malloc@PLT
	movq	%rax, r0(%rip)
	movq	r0(%rip), %rax
	movq	$0, (%rax)
	movq	r0(%rip), %rax
	movq	%rax, %rdi
	call	push
	movl	$0, %eax
	call	pop
	movq	%rax, ret(%rip)
	movl	$0, %eax
	call	ldmfd
	nop
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	Lambda0, .-Lambda0
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
