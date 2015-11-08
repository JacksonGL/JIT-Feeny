	.globl f
f:
	movq	%rdi, %rax
	addq	$42, %rax
	ret
