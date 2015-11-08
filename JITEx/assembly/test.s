.data
.globl test
test:
.string "Hello World\n"

.text
.global main
main:
	pushq	%rbp
	movq	%rsp,	%rbp
	movq	$test,	%rdi
	call	puts
	movq	$0,	%rax
	leave
	ret
