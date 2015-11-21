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
	movq		$0xcafebabecafebabe, %rax
## get the top of the value from the stack at -8($sp)
## move to destination value = $fp + (index+2)*8
	movq -8(%rdx), %r10
 	movq	%r10, 16(%rcx,%rax,8)
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
	movq		$0xcafebabecafebabe, %rax
## get the top of the value from the stack at -8($sp)
## move to destination value = $fp + (index+2)*8
	movq -8(%rdx), %r10
 	movq	%r10, globals(,%rax,8)
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

.globl lit_op
.globl lit_op_end

lit_op:
	movq $0xcafebabecafebabe, %r10
	movq %r10, 0(%rdx)
	addq $8, %rdx
lit_op_end:

.globl call_op_pre
.globl call_op_pre_end
.globl call_op_push_body
.globl call_op_push_body_end
.globl call_op_post
.globl call_op_post_end

call_op_pre:
	movq %rcx, %r11 # get the new parent frame
call_op_pre_end:
call_op_push_body: # need one of these blocks for each arity
	subq $8, %rcx	# allocate space for 1 arity
	subq $8, %rdx	# pop stack
	movq 0(%rdx), %r10 # get popped value
	movq %r10, 0(%rcx) # set arity value
call_op_push_body_end:
call_op_post:
	leaq call_op_post_end(%rip), %r10
	movq $0xcafebabecafebabe, %rax # function address
	jmp *%rax # call
call_op_post_end:

.globl method_prelude
.globl method_prelude_end
.globl method_local
.globl method_local_end

method_local:
	subq $8, %rcx	# allocate space for 1 arity
	movq $2, 0(%rcx) # set arity value
method_local_end:
method_prelude: # sets locals to null
	subq $16, %rcx # allocate enough room for return pointer and parent frame
	movq %r10, 8(%rcx) # return address
	movq %r11, 0(%rcx) # parent frame
method_prelude_end:

.globl object_op
.globl object_op_end

object_op:
##	ClassLayout * cl = get_class_by_idx(i->class);
				movq		$0xcafebabecafebabe, %r8		## %r8 has i->class
				movq		$0xcafebabecafebabe, %rax
				addq		%rax, %r8			 		## ci is in %r8
##	int size = sizeof(ObjectIValue)+sizeof(IValue*)*cl->num_slots;
				movslq	4(%r8), %rax							## cl->num_slots
				leal		16(,%rax,8), %eax
				movslq	%eax, %r9							 	## size is in %r9
##	ObjectIValue* obj = halloc(size);
				movq		%rsi, %r10					## obj = heap_pointer
##	heap_pointer += size;
				addq		%r9, %rsi			 		## rsi holds the value of heap_pointer
##	check if heap_pointer >= top_of_heap
				cmpq		%rdi, %rsi					## top_of_heap is in %rdi, heap_pointer is in %rsi
	 	jle		 ASSIGN_SLOTS
TRAP_2_C:
##	heap_pointer -= size;
				subq		%r9, %rsi
				leaq		OBJ_AFTER_TRAP(%rip), %rax
				movq		$0xcafebabecafebabe, %r8
				movq		%rax, (%r8)
				movq		$-2, %rax			## -2 tells the C code to do GC
				ret
OBJ_AFTER_TRAP:
##	ClassLayout * cl = get_class_by_idx(i->class);
				movq		$0xcafebabecafebabe, %r8		## %r8 has i->class
				movq		$0xcafebabecafebabe, %rax
				addq		%rax, %r8			 		## ci is in %r8
##	int size = sizeof(ObjectIValue)+sizeof(IValue*)*cl->num_slots;
				movslq	4(%r8), %rax							## cl->num_slots
				leal		16(,%rax,8), %eax
				movslq	%eax, %r9							 	## size is in %r9
##	ObjectIValue* obj = halloc(size);
				movq		%rsi, %r10					## obj = heap_pointer
##	heap_pointer += size;
				addq		%r9, %rsi			 		## rsi holds the value of heap_pointer
##	from now on %rax is i
##	assuming that *obj is in %r10
ASSIGN_SLOTS:
				movslq	4(%r8), %rax						## cl is in %r8, so %eax has cl->num_slots
START_LOOP:
				subq		$1, %rax									## eax has cl->num_slots-1 (i.e., i), i--
				cmpq		$-1, %rax							 	## check if i >= 0
				je			END_LOOP
##	stack_pop
				subq		$8, %rdx									## stack_pointer--
				movq		0(%rdx), %r11			 		## %r11 has *stack_pointer
##	obj->var_slots[i] = *stack_pointer;
				movq		%r11, 16(%r10,%rax,8)	 	## %r10 has object
				jmp		 START_LOOP
END_LOOP:
				subq		$8, %rdx									## stack_pointer--
				movq		0(%rdx), %r11			 		## %r11 has *stack_pointer
##	obj->parent_obj_ptr = *stack_pointer;
				movq		%r11, 8(%r10)
##	obj->class_ptr = cl;
				movq		%r8, (%r10)
##	from_obj_val ==> (IValue*)(((uintptr_t)val) | ARRAY_OBJ_MASK);
				movq		$0xcafebabecafebabe, %rax
				orq		 %rax, %r10					## %r10 has from_obj_val(obj)
##	push stack
				movq		%r10, 0(%rdx)
				addq		$8, %rdx
object_op_end:


.globl array_op
.globl array_op_end

array_op:
	pushq	%r12
				subq		$8, %rdx								## stack_pointer--
				movq		0(%rdx), %r8						## %r8 holds the *init_value ptr
				movq		-8(%rdx), %r9		## %r9 holds the *lengthi ptr, it also holds *len ptr
				movq 	%r9, %r12		## %r12 keeps the original value of *lengthi
				movq		%r9, %r11
				sarq		$3, %r11								## shrift arithmetic right to get the int length, %r11 holds length
##	stack push init_value
				movq		%r11, -8(%rdx)
##	size = sizeof(ArrayIValue)+sizeof(IValue*)*length
##	from now on, size is in %r9
				leal		16(,%r11,8), %eax
				movslq	%eax, %r9
##	call halloc
##	ObjectIValue* obj = halloc(size);
				movq		%rsi, %r10				## obj = heap_pointer
##	heap_pointer += size;
				addq		%r9, %rsi			 	## rsi holds the value of heap_pointer
##	check if heap_pointer >= top_of_heap
				cmpq		%rdi, %rsi				## top_of_heap is in %rdi, heap_pointer is in %rsi
				jle		 ARR_ASSIGN_SLOTS
ARR_TRAP_2_Ca:
##	restore the stack
	movq		%r12, -8(%rdx)		## push *lengthi into the stack
	movq		%r8, 0(%rdx)		## push *init_value into the stack
	addq		$8, %rdx								## stack_pointer++
##	heap_pointer += size;
				subq		%r9, %rsi
				leaq		ARR_AFTER_TRAP(%rip), %rax
				movq		$0xcafebabecafebabe, %r8
				movq		%rax, (%r8)
				movq		$-2, %rax		## -2 tells the C to do GC
				popq	%r12
				ret
ARR_AFTER_TRAP:
	subq		$8, %rdx								## stack_pointer--
				movq		0(%rdx), %r8						## %r8 holds the *init_value ptr
				movq		-8(%rdx), %r9					 ## %r9 holds the *lengthi ptr, it also holds *len ptr
				movq		%r9, %r11
				sarq		$3, %r11								## shrift arithmetic right to get the int length, %r11 holds length
##	stack push init_value
				movq		%r11, 0(%rdx)
##	size = sizeof(ArrayIValue)+sizeof(IValue*)*length
##	from now on, size is in %r9
				leal		16(,%r11,8), %eax
				movslq	%eax, %r9
##	call halloc
##	ObjectIValue* obj = halloc(size);
				movq		%rsi, %r10	 		## obj = heap_pointer
ARR_ASSIGN_SLOTS:
##	t->tag = ARRAY_OBJ;
				movq		$2, (%r10)
##	t->length = length;
				movq		%r11, 8(%r10)
##	init_value = stack_pop(); // for safety
				subq		$8, %rdx								## stack_pointer--
				movq		0(%rdx), %r8						## %r8 holds the *init_value ptr
##	start loop
				movq		$0, %rax
ARR_START_LOOP:
				cmpq		%rax, %r11			## compare i and length
##	if i < length
				jge		 ARR_END_LOOP
##	t->slots[i] = init_value;
				movq		%r8, 16(%r10,%rax,8)
				incq		%rax							## i++
				jmp		 ARR_START_LOOP
ARR_END_LOOP:
##	from_array_val(t)
				mov		 $0xcafebabecafebabe, %rax
	orq	%rax, %r10
##	stack push
				movq		%r10, (%rdx)
				addq		$8, %rdx
				popq	%r12
array_op_end:


## assume that CallSlotIns* i
## has been stored in %rdi

.globl built_in_method_op
.globl built_in_method_op_end

built_in_method_op:
##	movq %rdi, %r8
##	movq top_of_heap(%rip),%rdi
##	movq heap_pointer(%rip), %rsi
##	movq stack_pointer(%rip), %rdx
##	movq frame_pointer(%rip), %rcx
##	same the return address
##  movq 	$0xcafebabecafebabe, %rax ## return address
  movq  $0xcafebabecafebabe, %r9  ## arity
  movq  $0xcafebabecafebabe, %r11 ## method name
	pushq %r14
	pushq %r13
##	pushq	%rax
##	start the function body -----------------------------
##	movslq	8(%r8), %r9
	movq	%rdx, %rax
	salq	$3, %r9		## %r9 contains i->arity
##	IValue* receiver_ptr = *(stack_pointer - i->arity);
	subq	%r9, %rax
	movq	(%rax), %r10	## receiver_ptr
##	int method_name = i->name
##	movl	4(%r8), %eax
##	movslq %eax, %r11
## get object type -------------------------------------
## result will be stored in %rax
BUILT_IN_OBJ_TYPE:
	pushq %r10
	cmpq	$2, %r10
	je		OBJ_TYPE_NULL
	movq	%r10, %rax
	andq	$1, %rax
## if (v%2 == 1)
	cmpq	$1, %rax
	je		OBJ_TYPE_V2_EQ_1
## return INT_OBJ;
	movq	$0, %rax
	jmp	 END_BUILT_IN_OBJ_TYPE
OBJ_TYPE_V2_EQ_1:
## get_tag
	andq	$0xFFFFFFFFFFFFFFF8, %r10
## return tag > OBJ_OBJ? OBJ_OBJ : tag;
	movq	(%r10), %rax
  movq  $4, %r8
	cmpq	$4, %rax
	cmovg	%r8, %rax
	jmp	 	END_BUILT_IN_OBJ_TYPE
OBJ_TYPE_NULL:
	movq	$1, %rax
END_BUILT_IN_OBJ_TYPE:
	popq	%r10
## start doing the swtich statements ------------------
	cmpq	$0, %rax
	je		BUILT_IN_CASE_INT
	cmpq	$2, %rax
	je		BUILT_IN_CASE_ARRAY
	movq	$0, %r13
	jmp	 END_BUILT_BODY
BUILT_IN_CASE_INT:
## body of case int -----------------------------------
## IValue* arg = stack_pop();
## stack_pop();
## start comparing method name
CASE_ADD:
	movq $0xcafebabecafebabe, %rax   ## INT_ADD_NAME
	cmpq	%rax, %r11	## compare method name
	jne CASE_SUB
## body of add
	subq	$16, %rdx							 ## stack pop twice
	movq	8(%rdx), %r14					 ## %r14 has *stack_pointer
	leaq	(%r10,%r14), %rax
## push into stack
	movq	%rax, (%rdx)
	addq	$8, %rdx
## return value
	movq	$1, %r13
	jmp	 END_BUILT_BODY
CASE_SUB:
	movq $0xcafebabecafebabe, %rax  ## INT_SUB_NAME
	cmpq	%rax, %r11	## compare method name
	jne CASE_MUL
## body of sub
	subq	$8, %rdx								## stack pop twice and push once
	movq	0(%rdx), %r14					 ## %r14 has *stack_pointer, i.e., arg
	movq	%r10, -8(%rdx)
	subq	%r14, -8(%rdx)
## return value
	movq	$1, %r13
	jmp	 END_BUILT_BODY
CASE_MUL:
	movq $0xcafebabecafebabe, %rax  ## INT_MUL_NAME
	cmpq	%rax, %r11	## compare method name
	jne CASE_DIV
## body of mul
	subq	$8, %rdx							 ## stack pop twice and push once
	movq	0(%rdx), %rax					## %rax has *stack_pointer, i.e., arg
	sarq	$3, %rax
	movslq	%eax, %rax
	imulq %rax, %r10						 ## in this branch after this instruction, %r10 no longer holds receiver_ptr
## push into stack
	movq %r10, -8(%rdx)
## return value
	movq	$1, %r13
	jmp	 END_BUILT_BODY
CASE_DIV:
	movq $0xcafebabecafebabe, %rax ## INT_DIV_NAME
	cmpq	%rax, %r11	## compare method name
	jne CASE_MOD
## body of div
	subq	$8, %rdx							 ## stack pop twice and push once
	movq	0(%rdx), %r14		 	 		 ## %r14 has *stack_pointer, i.e., arg
	pushq %rdx
	movq	%r10, %rax
	cqo
	idivq %r14
	salq	$3, %rax
## push into stack
	popq	%rdx
	movq	%rax, -8(%rdx)
## return value
	movq	$1, %r13
	jmp	 END_BUILT_BODY
CASE_MOD:
  movq $0xcafebabecafebabe, %rax  ## INT_MOD_NAME
  cmpq  %rax, %r11  ## compare method name
  jne CASE_EQ
## body of mod
  subq  $8, %rdx               ## stack pop twice and push once
  movq  0(%rdx), %r14          ## %r14 has *stack_pointer, i.e., arg
  pushq %rdx
  movq  %r10, %rax
  cqo
  idivq %r14
  salq  $3, %rax
## push into stack
	movq  %rdx, %r10						## %r10 no longer holds receiver_ptr
  popq  %rdx
  movq  %r10, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_EQ:
  movq $0xcafebabecafebabe, %rax  ## INT_EQ_NAME
  cmpq  %rax, %r11  ## compare method name
  jne CASE_LT
## body of eq
  subq  $8, %rdx               ## stack pop twice and push once
	xorq  %rax, %rax
  cmpq  %r10, (%rdx)
	setne %al
	addq  %rax, %rax
  movq  %rax, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_LT:
  movq $0xcafebabecafebabe, %rax ## INT_LT_NAME
  cmpq  %rax, %r11             ## compare method name
  jne CASE_LE
## body of lt
  subq  $8, %rdx               ## stack pop twice and push once
  xorq  %rax, %rax
  cmpq  0(%rdx), %r10
  setge %al                    ## if greater or eq, %al holds $0, otherwise $1
  addq  %rax, %rax             ## if greater or eq, %rax holds $0, otherwise $2, which is null_obj
## push into stack
  movq  %rax, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_LE:
  movq $0xcafebabecafebabe, %rax  ## INT_LE_NAME
  cmpq  %rax, %r11             ## compare method name
  jne CASE_GE
## body of le
  subq  $8, %rdx               ## stack pop twice and push once
  xorq  %rax, %rax
  cmpq  0(%rdx), %r10
  setg %al                     ## if greater, %al holds $0, otherwise $1
  addq  %rax, %rax             ## if greater, %rax holds $0, otherwise $2, which is null_obj
## push into stack
  movq  %rax, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_GE:
  movq $0xcafebabecafebabe, %rax ## INT_GE_NAME
  cmpq  %rax, %r11             ## compare method name
  jne CASE_GT
## body of ge
  subq  $8, %rdx               ## stack pop twice and push once
  xorq  %rax, %rax
  cmpq  0(%rdx), %r10
  setl %al                     ## if less, %al holds $0, otherwise $1
  addq  %rax, %rax             ## if less, %rax holds $0, otherwise $2, which is null_obj
## push into stack
  movq  %rax, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_GT:
## body of gt
  subq  $8, %rdx               ## stack pop twice and push once
  xorq  %rax, %rax
  cmpq  0(%rdx), %r10
  setle %al                    ## if less, %al holds $0, otherwise $1
  addq  %rax, %rax             ## if less, %rax holds $0, otherwise $2, which is null_obj
## push into stack
  movq  %rax, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY

BUILT_IN_CASE_ARRAY:
## body of case array ---------------------------------
## start comparing method name
CASE_LENGTH:
  movq $0xcafebabecafebabe, %rax  ## ARRAY_LENGTH_NAME
  cmpq  %rax, %r11  ## compare method name
  jne CASE_ARR_SET
## body of array length
	andq  $0xFFFFFFFFFFFFFFF8, %r10
  movslq  8(%r10), %rax
  salq  $3, %rax
## push into stack
  movq  %rax, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_ARR_SET:
  movq $0xcafebabecafebabe, %rax  ## ARRAY_SET_NAME
  cmpq  %rax, %r11  ## compare method name
  jne CASE_ARR_GET
## body of array set
	subq  $16, %rdx   ## pop three times and push once
	movq  8(%rdx), %r14  ## %r14 holds the value to be pushed
	movq  0(%rdx), %rax  ## %rax holds the array index
  andq  $0xFFFFFFFFFFFFFFF8, %r10
  sarq  $3, %rax
	movslq  %eax, %rax
  movq  %r14, 16(%r10,%rax,8)
## push null_obj into stack
  movq  $2, -8(%rdx)
## return value
  movq  $1, %r13
  jmp  END_BUILT_BODY
CASE_ARR_GET:
	subq  $8, %rdx   ## pop twice and push once
	movq  0(%rdx), %rax ## %rax holds the array index
  andq  $0xFFFFFFFFFFFFFFF8, %r10
  sarq  $3, %rax
  movslq  %eax, %rax
  movq  16(%r10,%rax,8), %rax
	movq  %rax, -8(%rdx) 	## push into stack the value pointed by the array index
  movq  $1, %r13
END_BUILT_BODY:
## end the swtich structure
##	movq %rsi, heap_pointer (%rip)
##	movq %rdx, stack_pointer (%rip)
##	movq %rcx, frame_pointer (%rip)
##	pop the return address
##	popq %r9					## save the return address
	movq %r13, %rax	 ## save the return value
	popq %r13
	popq %r14
##	jmp	%r9
##	ret
built_in_method_op_end:


.globl call_slot_op
.globl general_call_slot
.globl do_call_slot_op
.globl call_slot_op_pre
.globl call_slot_op_pre_end
.globl call_slot_op_push_body
.globl call_slot_op_push_body_end
.globl call_slot_op_post
.globl call_slot_op_post_end

call_slot_op:
  cmpq $0, %rax
  je START_CALL_SLOT
  movq $0xcafebabecafebabe, %rax
  jmp *%rax
START_CALL_SLOT:
	movq $0xcafebabecafebabe, %r11 # negative arity
	movq 0(%rdx,%r11, 8), %r11 # get receiver object
	movq cached_type(%rip), %r10 # load cached type
	cmpq $-1, %r10
	je general_call_slot
	cmpq -1(%r11), %r10 # compare cached type and actual type
	je do_call_slot_op # if cache hit, do call
general_call_slot:
	movq %r11, receiver_shared
	leaq do_call_slot_op(%rip), %rax # get return code point
	movq instruction_pointer, %r8 # get instruction pointer
	movq %rax, (%r8) # store return code point in instruciton pointer
	movq $0xcafebabecafebabe, %rax #value to return
	ret
cached_type: .quad -1
cached_addr: .quad -1
do_call_slot_op:
	movq %rcx, %r11 # get the new parent frame
	movq cached_addr(%rip), %rax # load cached subroutine address
call_slot_op_pre_end:
call_slot_op_push_body: # need one of these blocks for each arity
	subq $8, %rcx	# allocate space for 1 arity
	subq $8, %rdx	# pop stack
	movq 0(%rdx), %r10 # get popped value
	movq %r10, 0(%rcx) # set arity value
call_slot_op_push_body_end:
call_slot_op_post:
# r11 must have parent frame
	leaq call_slot_op_post_end(%rip), %r10
	jmp *%rax
call_slot_op_post_end:

.globl get_slot_op
.globl get_slot_op_end

get_slot_op:
	movq -8(%rdx), %r11 # get receiver object
	movq gslot_cached_type(%rip), %r10 # load cached type
	cmpq $-1, %r10
	je general_get_slot
	cmpq -1(%r11), %r10 # compare cached type and actual type
	je do_get_slot_op # if cache hit, do call
general_get_slot:
	movq %r11, receiver_shared
	leaq general_get_slot_end(%rip), %rax # get return code point
	movq instruction_pointer, %r8 # get instruction pointer
	movq %rax, (%r8) # store return code point in instruciton pointer
	movq $0xcafebabecafebabe, %rax #value to return
	ret
gslot_cached_type: .quad -1
gslot_cached_offset: .quad -1
general_get_slot_end:
	movq receiver_shared, %r11
do_get_slot_op:
	movq gslot_cached_offset(%rip), %rax # load cached slot idx
	movq 15(%r11,%rax,8), %r10
	movq %r10, -8(%rdx)
get_slot_op_end:

.globl set_slot_op
.globl set_slot_op_end

set_slot_op:
	movq -16(%rdx), %r11 # get receiver object
	movq sslot_cached_type(%rip), %r10 # load cached type
	cmpq $-1, %r10
	je general_set_slot
	cmpq -1(%r11), %r10 # compare cached type and actual type
	je do_set_slot_op # if cache hit, do call
general_set_slot:
	movq %r11, receiver_shared
	leaq general_set_slot_end(%rip), %rax # get return code point
	movq instruction_pointer, %r8 # get instruction pointer
	movq %rax, (%r8) # store return code point in instruciton pointer
	movq $0xcafebabecafebabe, %rax #value to return
	ret
sslot_cached_type: .quad -1
sslot_cached_offset: .quad -1
general_set_slot_end:
	movq receiver_shared, %r11
do_set_slot_op:
	movq sslot_cached_offset(%rip), %rax # load cached slot idx
	movq -8(%rdx), %r10
	movq %r10, 15(%r11,%rax,8)
	movq %r10, -16(%rdx)
	subq $8, %rdx # 8 = size of pointer
set_slot_op_end:
