
	.globl	inc_counter_ins
	.globl	inc_counter_ins_end
inc_counter_ins:
	movq counter(%rip), %rax
	addq $1, %rax
	movq %rax, counter(%rip)
	ret
inc_counter_ins_end:
