	.globl	return42
	.globl	return42_end
return42:
	movq	$42, %rax
	ret
return42_end:
