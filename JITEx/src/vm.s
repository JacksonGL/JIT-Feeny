	.globl	_exec_goto_op

## Sets the instruction pointer to the instruction
## address associated with the name given by
## the String object at index i.

## The structure of the GotoIns is as follows:
## typedef struct {
##   OpCode tag;
##   int name;
## } GotoIns;
##
_exec_goto_op:
	movl	4(%rdi), %eax
	ret
_exec_goto_op_end:

