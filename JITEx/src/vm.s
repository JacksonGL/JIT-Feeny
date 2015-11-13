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

.globl drop_op
.globl drop_op_end

drop_op:
	subq $8, %rdx # 8 = size of pointer
drop_op_end:

.globl return_op
.globl return_op_end

return_op:
	movq 8(%rcx), %rax
	movq 0(%rcx), %rcx
	jmp *%rax
return_op_end:

.globl exec_object_op_1
.globl exec_object_op_end_1


exec_object_op_1:
        movq %rdi, %rax
        movq top_of_heap(%rip),%rdi
        movq heap_pointer(%rip), %rsi
        movq stack_pointer(%rip), %rdx
        movq frame_pointer(%rip), %rcx
## ClassLayout * cl = get_class_by_idx(i->class);
        movslq  4(%rax), %r8            ## %r8 has i->class
        addq    $class_objs, %r8        ## cl is in %r8
## int size = sizeof(ObjectIValue)+sizeof(IValue*)*cl->num_slots;
        movslq  4(%r8), %rax			## cl->num_slots
        leal    16(,%rax,8), %eax
        movslq  %eax, %r9       		## size is in %r9
## ObjectIValue* obj = halloc(size);
		movq 	%rsi, %r10		## obj = heap_pointer
## heap_pointer += size;
        addq    %r9, %rsi       ## rsi holds the value of heap_pointer
## check if heap_pointer >= top_of_heap
        cmpq    %rdi, %rsi      ## top_of_heap is in %rdi, heap_pointer is in %rsi
        jle		ASSIGN_SLOTS
TRAP_2_C:
## from now on %rax is i
## todo: trap to C
        ret
## assuming that *obj is in %r10
ASSIGN_SLOTS:
        movslq  4(%r8), %rax    		## cl is in %r8, so %eax has cl->num_slots
START_LOOP:
        subq    $1, %rax                ## eax has cl->num_slots-1 (i.e., i), i--
        cmpq    $-1, %rax               ## check if i >= 0
        je      END_LOOP
## stack_pop
        subq    $8, %rdx                ## stack_pointer--
        movq    0(%rdx), %r11    		## %r11 has *stack_pointer
## obj->var_slots[i] = *stack_pointer;
        movq    %r11, 16(%r10,%rax,8)   ## %r10 has object
        jmp     START_LOOP
END_LOOP:
        subq    $8, %rdx                ## stack_pointer--
        movq    0(%rdx), %r11   		## %r11 has *stack_pointer
## obj->parent_obj_ptr = *stack_pointer;
        movq    %r11, 8(%r10)
## obj->class_ptr = cl;
        movq    %r8, (%r10)
## from_obj_val ==> (IValue*)(((uintptr_t)val) | ARRAY_OBJ_MASK);
        orq     ARRAY_OBJ_MASK(%rip), %r10      ## %r10 has from_obj_val(obj)
## push stack
        movq    %r10, 0(%rdx)
        addq    $8, %rdx
        movq    %rsi, heap_pointer (%rip)
        movq    %rdx, stack_pointer (%rip)
        movq    %rcx, frame_pointer (%rip)
        ret
exec_object_op_end_1:
