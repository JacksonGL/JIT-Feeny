	.globl	exec_goto_op
#	.globl	stack_peek
#	.globl	stack_pop
#	.globl	stack_push
# 	.globl	_get_tag
#	.globl	obj_type
	.globl	exec_branch_op
	.globl	exec_lit_op
	.globl	exec_lit_null_op
	.globl	exec_set_local_op
	.globl	exec_get_local_op
	.globl	exec_set_global_op
	.globl	exec_get_global_op
	.globl	exec_drop_op
	.globl	exec_return_op

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

exec_lit_op_1:
	movslq	4(%rdi), %rcx
	shlq	$3, %rcx
	movslq	stack_top(%rip), %rax
	movq	%rcx, stack(,%rax,8)
	incl	stack_top(%rip)
	ret
exec_lit_op_end_1:

exec_lit_op:
        movslq  4(%rdi), %rcx
        movl    stack_top(%rip), %eax
        movslq  %eax, %rdx
        addl    $1, %eax
	    salq    $3, %rcx
        movl    %eax, stack_top(%rip)
        movq    %rcx, stack(,%rdx,8)
        ret
exec_lit_op_end:

exec_lit_null_op:
        movslq  stack_top(%rip), %rdx
        movq    $2, stack(,%rdx,8)
        addl    $1, stack_top(%rip)
        ret
exec_lit_null_op_end:

exec_lit_null_op_1:
        movl    stack_top(%rip), %eax
        movslq  %eax, %rdx
        addl    $1, %eax
        movq    $2, stack(,%rdx,8)
        movl    %eax, stack_top(%rip)
        ret
exec_lit_null_op_end_1:

exec_set_local_op:
        movq    stack_top(%rip), %rax
        subq    $1, %rax
        movq    stack(,%rax,8), %rcx
        ## store i->idx into %rdx
        movslq  4(%rdi), %rdx
        ## get the current frame
        movq    current(%rip), %rax
        ## current->slots[idx] = v;
        movq    %rcx, 16(%rax,%rdx,8)
        ret
exec_set_local_op_end:

exec_get_local_op:
	## i->idx
	movslq  4(%rdi), %rdx
        movq    current(%rip), %rax
        ## current->slots[idx]
        movq    16(%rax,%rdx,8), %rcx
        ## stack push
        movq    stack_top(%rip), %rax
        movq    %rcx, stack(,%rax,8)
        addq    $1, stack_top(%rip)
        ret
exec_get_local_op_end:

exec_set_global_op:
        movq    stack_top(%rip), %rcx
        movslq  4(%rdi), %rax
        leaq    -1(%rcx), %rdx
        movq    stack(,%rdx,8), %rdx
        movq    %rdx, globals(,%rax,8)
        ret
exec_set_global_op_end:

exec_get_global_op:
        movslq  4(%rdi), %rcx
        movq    stack_top(%rip), %rax
        movq    globals(,%rcx,8), %rcx
        addq    $1, stack_top(%rip)
        movq    %rcx, stack(,%rax,8)
        ret
exec_get_global_op_end:

exec_drop_op:
        subl    $1, stack_top(%rip)
        ret
exec_drop_op_end:

exec_return_op:
		## frames + frame_top
        movq    frame_top(%rip), %rsi
        movq  	%rsi, %rcx
	leaq	frames(%rip), %rdx
        addq    %rdx, %rcx
        ## size_of_current_frame = (frames + frame_top) - current
        movq    current(%rip), %rdx
        movl    8(%rdx), %eax  ## current->return_addr is stored as return value
        subq    %rdx, %rcx
        ## frame_top -= size_of_current_frame
        subl    %ecx, %esi
        movl    %esi, frame_top(%rip)
        ## *current = current->parent
        movq    (%rdx), %rdx
        movq    %rdx, current(%rip)
        ret
exec_return_op_end:

stack_peek:
        movq    stack_top(%rip), %rax
        subq    $1, %rax
        movq    stack(,%rax,8), %rax
        ret
stack_peek_end:

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
