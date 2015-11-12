#define HOLE 0xcafebabecafebabe
.extern top_of_heap
.extern heap_pointer
.extern stack_pointer
.extern frame_pointer
.extern instruction_pointer
.extern globals

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

.globl goto_op
.globl goto_op_end

goto_op:
	movq $0xcafebabecafebabe, %rax
	jmp *%rax
goto_op_end:

.globl branch_op
.globl branch_op_end

## branch operation
branch_op:
## stack pop
	subq $8, %rdx # 8 = size of pointer
# load destination if stack pointer is NULL val
	movq $0xcafebabecafebabe, %rax
# compare the stack value to NULL -
# convention is that the stack pointer points to the next free spot
	cmpq $2, 0(%rdx)
# if it is not null, jump to loaded value
	je branch_op_end
	jmp *%rax
branch_op_end:

.globl set_local_op
.globl set_local_op_end

set_local_op:
## get local index
	movq    $0xcafebabecafebabe, %rax	
## get the top of the value from the stack at -8($sp)
## move to destination value = $fp + (index+2)*8
	movq -8(%rdx), %r10
 	movq  %r10, 16(%rcx,%rax,8)
set_local_op_end:

.globl get_local_op
.globl get_local_op_end

get_local_op:
	movq $0xcafebabecafebabe, %rax
	movq 16(%rcx,%rax,8), %r10
	movq %r10, 0(%rdx)
	addq $8, %rdx # 8 = size of pointer
get_local_op_end:

.globl set_global_op
.globl set_global_op_end

set_global_op:
## get local index
	movq    $0xcafebabecafebabe, %rax	
## get the top of the value from the stack at -8($sp)
## move to destination value = $fp + (index+2)*8
	movq -8(%rdx), %r10
 	movq  %r10, globals(,%rax,8)
set_global_op_end:

.globl get_global_op
.globl get_global_op_end

get_global_op:
	movq $0xcafebabecafebabe, %rax
	movq globals(,%rax,8), %r10
	movq %r10, 0(%rdx)
	addq $8, %rdx # 8 = size of pointer
get_global_op_end:
