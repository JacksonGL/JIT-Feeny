	.globl f
f:
	pushq	%rsp
	pushq	%rbx
	pushq	%rbq
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	movq	%rdi, %rax
	addq	$42, %rax
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbq
	popq	%rbx
	popq	%rsp
	ret
