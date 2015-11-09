	.globl	exec_goto_op
#	.globl	stack_pop
#	.globl	stack_push
# 	.globl	_get_tag
#	.globl	obj_type
	.globl	exec_branch_op

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

stack_push:
	movq	stack@GOTPCREL(%rip), %rax
	movslq	stack_top(%rip), %rdx
	movq	%rdi, (%rax,%rdx,8)
	incl	stack_top(%rip)
	ret
stack_push_end:

stack_pop:
	movq	stack@GOTPCREL(%rip), %rax
	## stack_top -= 1
##	movl	stack_top(%rip), %ecx
	subl	$1, stack_top(%rip)
##	movl	%ecx, stack_top(%rip)
	movslq	stack_top(%rip), %rdx
	## IValue* v = stack[stack_top-1];
	movq	(%rax,%rdx,8), %rax
	ret
stack_pop_end:

_get_tag:
##   IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	andq	$0xFFFFFFFFFFFFFFF8, %rdi
##   return tv->_tag;
	movq	(%rdi), %rax
	ret
_get_tag_end:


## code for obj_type function
obj_type:
        cmpq    $2, %rdi
        jne     NEQ
## return NULL_OBJ
        movl    $1, %eax
        ret
NEQ:
        movq    %rdi, %rax
## v%2
        andq    $1, %rax
        jz      ELSE_1
## if (v%2 == 1)
## get tag start
##   IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
        andq    $0xFFFFFFFFFFFFFFF8, %rdi
##   return tv->_tag;
##      movq    (%rdi), %rax
## get tag end
## compare OBJ_OBJ and tag
        cmpq    $4, (%rdi)
        jbe     RET_1
## if (tag > OBJ_OBJ)
        movl    $4, %eax
        ret
ELSE_1:
        movl    $0, %eax
	ret
RET_1:
	movq	(%rdi), %rax
        ret
obj_type_end:


## branch operation
exec_branch_op:
## stack pop
        movq    stack@GOTPCREL(%rip), %rax
        ## stack_top -= 1
	subl	$1, stack_top(%rip)
#       movl    stack_top(%rip), %ecx
#        subl    $1, %ecx
#       movl    %ecx, stack_top(%rip)
        movslq  stack_top(%rip), %rdx
        ## IValue* v = stack[stack_top-1];
        movq    (%rax,%rdx,8), %rax
## stack pop end
## obj_type start
        cmpq    $2, %rax
        jne     V_NEQ_2
## return NULL_OBJ
## return pc+1;
        movl    %esi, %eax
        incl	%eax
        ret
V_NEQ_2:
## v%2
        andq    $1, %rax
        jz      RET_NOT_NULL
## if (v%2 == 1)
## get tag start
## IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
        andq    $0xFFFFFFFFFFFFFFF8, %rax
## return tv->_tag;
        movq    (%rax), %rdx
        movq    %rdx, %rax
## get tag end
## compare OBJ_OBJ and tag
        cmpq    $4, %rax
        jbe     RET
## if (tag > OBJ_OBJ)
## return pc+1;
        movl    %esi, %eax
        incl    %eax
        ret
RET:
        cmpq    $1, %rax
        jne     RET_NOT_NULL
## obj_type end
RET_NULL:
## return pc+1;
        movl    %esi, %eax
        incl    %eax
        ret
RET_NOT_NULL:
## return i->name;
        movl    4(%rdi), %eax
        ret
exec_branch_op_end:
