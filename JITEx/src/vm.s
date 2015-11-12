#define HOLE 0xcafebabecafebabe
.extern top_of_heap
.extern heap_pointer
.extern stack_pointer
.extern frame_pointer
.extern instruction_pointer

.globl call_feeny

call_feeny:
	movq %rdi, %rax
	movq top_of_heap(%rip),%rdi
	movq heap_pointer(%rip), %rsi
	movq stack_pointer(%rip), %rdx
	movq frame_pointer(%rip), %rcx
	callq *0(%rax)
	movq %rsi, heap_pointer (%rip)
	movq %rdx, stack_pointer (%rip)
	movq %rcx, frame_pointer (%rip)
	ret

.globl	after_c_trap
.globl	c_trap

c_trap:
	leaq after_c_trap(%rip), %rax
	movq instruction_pointer, %r8
	movq %rax, (%r8)
	movq $0xcafebabecafebabe, %rax
	ret
after_c_trap:

.globl exec_goto_op_1
.globl exec_goto_op_end_1

exec_goto_op_1:
	movl    4(%rdi), %eax
##      movq    $0xcafebabecafebabe, %rax
        ret
exec_goto_op_end_1:

.globl exec_branch_op_1
.globl exec_branch_op_end_1

## branch operation
exec_branch_op_1:
## stack pop
	movq    stack_pointer(%rip), %rdx
        leaq    -8(%rdx), %r10
        movq    %r10, stack_pointer(%rip)
        movq    -8(%rdx), %rax
## stack pop end
## obj_type start
        cmpq    $2, %rax
        jne     V_NEQ_2
## return NULL_OBJ
## return pc+1;
        movq    %rsi, %rax ## replace %rsi with a hole for pc
        incq	%rax
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
        movq    %rsi, %rax ## replace $rsi with a hole for pc
        incq    %rax
        ret
RET:
        cmpq    $1, %rax
        jne     RET_NOT_NULL
## obj_type end
RET_NULL:
## return pc+1;
        movq    %rsi, %rax ## replace %rsi with a hole for pc
        incq    %rax
        ret
RET_NOT_NULL:
## return i->name;
        movl    4(%rdi), %eax  ## replace 4(%rdi) for a hole for i->name
        ret
exec_branch_op_end_1:

.globl exec_set_local_op_1
.globl exec_set_local_op_end_1

exec_set_local_op_1:
## stack peek
        movq    stack_pointer(%rip), %rdx	## comment
##      movq    $0xcafebabecafebabe, %r11	## uncomment, replace hole with i, where i is a SetLocalIns pointer
		movq    %rdi, %r11		## comment
        movslq  4(%r11), %r10				
## get the top of the value from the stack
        movq    -8(%rdx), %r11				
        movq    frame_pointer(%rip), %rcx	## comment
        movq    %r11, 16(%rcx,%r10,8)
##	movq 	$0xcafebabecafebabe, %rax   ## uncomment, replace hole with next pc
        ret
exec_set_local_op_1_end:

.globl exec_get_local_op_1
.globl exec_get_local_op_end_1

exec_get_local_op_1:
	movq    frame_pointer(%rip), %rcx	## comment
##      movq    $0xcafebabecafebabe, %r11	## uncomment, replace hole with i, where i is a GetLocalIns pointer
	movq    %rdi, %r11			## comment	
        movslq  4(%r11), %r10
## 	get_local(i->idx) ==> frame_pointer->slots[idx];
        movq    16(%rcx,%r10,8), %r10
##	stack_push the result of of get_local
        movq    stack_pointer(%rip), %rcx	## comment
        movq    %r10, (%rcx)
        addq	$8, %rcx
        movq    %rcx, stack_pointer(%rip)	## comment
##	movq 	$0xcafebabecafebabe, %rax       ## uncomment, replace hole with next pc
        ret
exec_get_local_op_end_1:

.globl exec_set_global_op_1
.globl exec_set_global_op_end_1

exec_set_global_op_1:
## stack peek
        movq    stack_pointer(%rip), %rdx
        movq    -8(%rdx), %r10
##      movq    $0xcafebabecafebabe, %r11       ## uncomment, replace hole with i, where i is a SetGlobalIns pointer
        movq    %rdi, %r11                      ## comment        
        movslq  4(%r11), %rax
##      movq    $0xcafebabecafebabe, %r11	## uncomment, replace hole with the address of 'global' variable
##	movq    %r10, (%r11,%rax,8)		## uncomment
        movq    %r10, globals(,%rax,8)		## comment
##	movq 	$0xcafebabecafebabe, %rax       ## uncomment, replace hole with next pc
        ret
exec_set_global_op_1_end:

.globl exec_get_global_op_1
.globl exec_get_global_op_end_1

exec_get_global_op_1:
## get_global_slot_by_idx(i->name)
##      movq    $0xcafebabecafebabe, %r11       ## uncomment, replace hole with i, where i is a GetGlobalIns pointer
	movq 	%rdi, %r11			## comment
        movslq  4(%r11), %rax
##      movq    $0xcafebabecafebabe, %r10	## uncomment, replace hole with the address of 'global' variable
##	movq    (%r10,%rax,8), %r11
        movq    globals(,%rax,8), %r11		## comment
## stack_push
        movq    stack_pointer(%rip), %rdx	## comment
        movq    %r11, (%rdx)
        addq    $8, %rdx
        movq 	%rdx, stack_pointer(%rip)	## comment
##	movq 	$0xcafebabecafebabe, %rax       ## uncomment, replace hole with next pc
        ret
exec_get_global_op_1_end:
