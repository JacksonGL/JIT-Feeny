	.globl	exec_goto_op
	.globl	stack_pop
	.globl	_get_tag

## Sets the instruction pointer to the instruction
## address associated with the name given by
## the String object at index i.

## The structure of the GotoIns is as follows:
## typedef struct {
##   OpCode tag;
##   int name;
## } GotoIns;
##
exec_goto_op:
	movl	4(%rdi), %eax
	ret
exec_goto_op_end:


stack_pop:
	movq	stack@GOTPCREL(%rip), %rax
	## stack_top -= 1
	movl	stack_top(%rip), %ecx
	subl	$1, %ecx
	movl	%ecx, stack_top(%rip)
	movslq	%ecx, %rdx
	## IValue* v = stack[stack_top-1];
	movq	(%rax,%rdx,8), %rax
	ret
stack_pop_end:

_get_tag:
##   IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	andq	CLEAR_ARRAY_OBJ_MASK(%rip), %rdi
##   return tv->_tag;
	movq	(%rdi), %rax
	ret
_get_tag_end:
