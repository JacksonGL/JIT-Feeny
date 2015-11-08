
	.globl	inc_counter_ins
	.globl	inc_counter_ins_end
inc_counter_ins:
	movq $counter, %rcx
	movq (%rcx), %rax
	addq $1, %rax
	movq %rax, (%rcx)
	ret
inc_counter_ins_end:
