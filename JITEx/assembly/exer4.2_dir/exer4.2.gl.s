	.globl f
f:
	movq	%rdi, %rax # binary encoding: 0x48	0x89	0xf8
	subq	%rsi, %rax # binary encoding: 0x48	0x29	0xf0
	ret # binary encoding: 0xc3

# binary encoding of all f instructions:
#0x4005c4 <f>:	0x48	0x89	0xf8	0x48	0x29	0xf0	0xc3
