	.globl f
f:
	movq	%rdi, %rax
	subq	%rsi, %rax
	ret
