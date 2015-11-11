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