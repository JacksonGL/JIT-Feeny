	.globl	exec_goto_op
	.globl	stack_pop
	.globl	_get_tag
	.globl	obj_type

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


## code for obj_type function
obj_type:
	cmpq	$2, %rdi
	jne	V_NEQ_2
## return NULL_OBJ
	movl	$1, %eax
	ret
V_NEQ_2:
	movq 	%rdi, %rax
## v%2
	andq	$1, %rax
	cmpq	$1, %rax
	jne	ELSE
## if (v%2 == 1)
## get tag start
##   IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	andq	CLEAR_ARRAY_OBJ_MASK(%rip), %rdi
##   return tv->_tag;
	movq	(%rdi), %rax
## get tag end
## compare OBJ_OBJ and tag
	cmpq	$4, %rax
	jbe	TAG_LE_OBJ_OBJ
## if (tag > OBJ_OBJ)
	movabsq	$4, %rax
	ret
## if (tag <= OBJ_OBJ)
TAG_LE_OBJ_OBJ:
	ret
ELSE:
	movq	$0, %rax
	ret
obj_type_end:
