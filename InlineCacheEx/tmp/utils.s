	.file	"utils.c"
	.text
.Ltext0:
	.p2align 4,,15
	.globl	max
	.type	max, @function
max:
.LFB42:
	.file 1 "utils.c"
	.loc 1 10 0
	.cfi_startproc
.LVL0:
	.loc 1 11 0
	cmpl	%edi, %esi
	movl	%edi, %eax
	cmovge	%esi, %eax
	.loc 1 12 0
	ret
	.cfi_endproc
.LFE42:
	.size	max, .-max
	.p2align 4,,15
	.globl	min
	.type	min, @function
min:
.LFB43:
	.loc 1 14 0
	.cfi_startproc
.LVL1:
	.loc 1 15 0
	cmpl	%edi, %esi
	movl	%edi, %eax
	cmovle	%esi, %eax
	.loc 1 16 0
	ret
	.cfi_endproc
.LFE43:
	.size	min, .-min
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"\""
.LC1:
	.string	"\\n"
.LC2:
	.string	"\\\\"
.LC3:
	.string	"\\\""
.LC4:
	.string	"%c"
	.text
	.p2align 4,,15
	.globl	print_string
	.type	print_string, @function
print_string:
.LFB44:
	.loc 1 18 0
	.cfi_startproc
.LVL2:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
.LBB30:
.LBB31:
	.file 2 "/usr/include/x86_64-linux-gnu/bits/stdio2.h"
	.loc 2 105 0
	movl	$.LC0, %esi
.LBE31:
.LBE30:
	.loc 1 18 0
	movq	%rdi, %rbx
.LBB33:
.LBB32:
	.loc 2 105 0
	xorl	%eax, %eax
	movl	$1, %edi
.LVL3:
	call	__printf_chk
.LVL4:
.L4:
.LBE32:
.LBE33:
.LBB34:
	.loc 1 21 0
	movsbl	(%rbx), %edx
.LVL5:
	.loc 1 22 0
	addq	$1, %rbx
.LVL6:
	.loc 1 23 0
	cmpb	$10, %dl
	je	.L6
.L15:
	jle	.L14
	cmpb	$34, %dl
	je	.L9
	cmpb	$92, %dl
	.p2align 4,,5
	jne	.L5
.LVL7:
.LBB35:
.LBB36:
	.loc 2 105 0
	xorl	%eax, %eax
	movl	$.LC2, %esi
	movl	$1, %edi
	call	__printf_chk
.LVL8:
.LBE36:
.LBE35:
	.loc 1 21 0
	movsbl	(%rbx), %edx
.LVL9:
	.loc 1 22 0
	addq	$1, %rbx
.LVL10:
	.loc 1 23 0
	cmpb	$10, %dl
	jne	.L15
.LVL11:
.L6:
.LBB37:
.LBB38:
	.loc 2 105 0
	movl	$.LC1, %esi
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk
.LVL12:
	jmp	.L4
.LVL13:
	.p2align 4,,10
	.p2align 3
.L14:
.LBE38:
.LBE37:
	.loc 1 23 0
	testb	%dl, %dl
	jne	.L5
.LVL14:
.LBE34:
	.loc 1 41 0
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
.LVL15:
.LBB45:
.LBB39:
.LBB40:
	.loc 2 105 0
	movl	$.LC0, %esi
	movl	$1, %edi
	xorl	%eax, %eax
	jmp	__printf_chk
.LVL16:
	.p2align 4,,10
	.p2align 3
.L5:
	.cfi_restore_state
.LBE40:
.LBE39:
.LBB41:
.LBB42:
	movl	$.LC4, %esi
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk
.LVL17:
	jmp	.L4
.LVL18:
	.p2align 4,,10
	.p2align 3
.L9:
.LBE42:
.LBE41:
.LBB43:
.LBB44:
	movl	$.LC3, %esi
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk
.LVL19:
	jmp	.L4
.LBE44:
.LBE43:
.LBE45:
	.cfi_endproc
.LFE44:
	.size	print_string, .-print_string
	.p2align 4,,15
	.globl	make_vector
	.type	make_vector, @function
make_vector:
.LFB45:
	.loc 1 47 0
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	.loc 1 48 0
	movl	$16, %edi
	call	malloc
.LVL20:
	.loc 1 51 0
	movl	$64, %edi
	.loc 1 48 0
	movq	%rax, %rbx
.LVL21:
	.loc 1 49 0
	movl	$0, (%rax)
	.loc 1 50 0
	movl	$8, 4(%rax)
	.loc 1 51 0
	call	malloc
.LVL22:
	movq	%rax, 8(%rbx)
	.loc 1 53 0
	movq	%rbx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 8
.LVL23:
	ret
	.cfi_endproc
.LFE45:
	.size	make_vector, .-make_vector
	.p2align 4,,15
	.globl	vector_ensure_capacity
	.type	vector_ensure_capacity, @function
vector_ensure_capacity:
.LFB46:
	.loc 1 55 0
	.cfi_startproc
.LVL24:
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	.loc 1 56 0
	movl	4(%rdi), %eax
	.loc 1 55 0
	movq	%rdi, %rbx
	.loc 1 56 0
	cmpl	%esi, %eax
	jge	.L18
.LVL25:
.LBB51:
	.loc 1 57 0
	addl	%eax, %eax
.LVL26:
.LBB52:
.LBB53:
	.loc 1 11 0
	cmpl	%eax, %esi
	cmovge	%esi, %eax
.LVL27:
.LBE53:
.LBE52:
	.loc 1 58 0
	movslq	%eax, %rdi
.LVL28:
.LBB55:
.LBB54:
	.loc 1 11 0
	movl	%eax, %r12d
.LBE54:
.LBE55:
	.loc 1 58 0
	salq	$3, %rdi
	call	malloc
.LVL29:
	.loc 1 59 0
	movslq	(%rbx), %rdx
.LBB56:
.LBB57:
	.file 3 "/usr/include/x86_64-linux-gnu/bits/string3.h"
	.loc 3 52 0
	movq	8(%rbx), %rsi
	movq	%rax, %rdi
.LBE57:
.LBE56:
	.loc 1 58 0
	movq	%rax, %rbp
.LVL30:
	.loc 1 59 0
	salq	$3, %rdx
.LVL31:
.LBB59:
.LBB58:
	.loc 3 52 0
	call	memcpy
.LVL32:
.LBE58:
.LBE59:
	.loc 1 60 0
	movq	8(%rbx), %rdi
	call	free
.LVL33:
	.loc 1 61 0
	movl	%r12d, 4(%rbx)
	.loc 1 62 0
	movq	%rbp, 8(%rbx)
.LVL34:
.L18:
.LBE51:
	.loc 1 64 0
	popq	%rbx
	.cfi_def_cfa_offset 24
.LVL35:
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE46:
	.size	vector_ensure_capacity, .-vector_ensure_capacity
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC5:
	.string	"Negative length given to vector.\n"
	.text
	.p2align 4,,15
	.globl	vector_set_length
	.type	vector_set_length, @function
vector_set_length:
.LFB47:
	.loc 1 66 0
	.cfi_startproc
.LVL36:
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movl	%esi, %r14d
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	.loc 1 67 0
	testl	%esi, %esi
	js	.L29
	.loc 1 71 0
	movslq	(%rdi), %rbx
	movq	%rdi, %rbp
	cmpl	%ebx, %esi
	jle	.L30
	movq	%rdx, %r15
	movl	4(%rdi), %r12d
	jmp	.L27
.LVL37:
	.p2align 4,,10
	.p2align 3
.L31:
	movq	8(%rbp), %rax
.LBB71:
.LBB72:
	.loc 1 81 0
	movq	%r15, (%rax,%rbx,8)
	.loc 1 82 0
	movslq	%ecx, %rbx
.LBE72:
.LBE71:
	.loc 1 74 0
	cmpl	%ebx, %r14d
.LBB87:
.LBB84:
	.loc 1 82 0
	movl	%ebx, 0(%rbp)
.LBE84:
.LBE87:
	.loc 1 74 0
	jle	.L21
.LVL38:
.L27:
.LBB88:
.LBB85:
	.loc 1 80 0
	leal	1(%rbx), %ecx
.LVL39:
.LBB73:
.LBB74:
	.loc 1 56 0
	cmpl	%r12d, %ecx
	jle	.L31
.LVL40:
.LBB75:
	.loc 1 57 0
	addl	%r12d, %r12d
.LVL41:
.LBB76:
.LBB77:
	.loc 1 11 0
	cmpl	%ecx, %r12d
	cmovl	%ecx, %r12d
.LVL42:
.LBE77:
.LBE76:
	.loc 1 58 0
	movslq	%r12d, %rdi
	salq	$3, %rdi
	call	malloc
.LVL43:
.LBB78:
.LBB79:
	.loc 3 52 0
	movq	8(%rbp), %rsi
.LBE79:
.LBE78:
	.loc 1 59 0
	leaq	0(,%rbx,8), %rdx
.LBB82:
.LBB80:
	.loc 3 52 0
	movq	%rax, %rdi
.LBE80:
.LBE82:
	.loc 1 58 0
	movq	%rax, %r13
.LVL44:
.LBB83:
.LBB81:
	.loc 3 52 0
	call	memcpy
.LVL45:
.LBE81:
.LBE83:
	.loc 1 60 0
	movq	8(%rbp), %rdi
	call	free
.LVL46:
	movslq	0(%rbp), %rbx
.LVL47:
	.loc 1 62 0
	movq	%r13, %rax
	.loc 1 61 0
	movl	%r12d, 4(%rbp)
	.loc 1 62 0
	movq	%r13, 8(%rbp)
	leal	1(%rbx), %ecx
.LBE75:
.LBE74:
.LBE73:
	.loc 1 81 0
	movq	%r15, (%rax,%rbx,8)
	.loc 1 82 0
	movslq	%ecx, %rbx
.LBE85:
.LBE88:
	.loc 1 74 0
	cmpl	%ebx, %r14d
.LBB89:
.LBB86:
	.loc 1 82 0
	movl	%ebx, 0(%rbp)
.LBE86:
.LBE89:
	.loc 1 74 0
	jg	.L27
.LVL48:
.L21:
	.loc 1 77 0
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
.LVL49:
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
.LVL50:
	popq	%r15
	.cfi_def_cfa_offset 8
.LVL51:
	ret
.LVL52:
	.p2align 4,,10
	.p2align 3
.L30:
	.cfi_restore_state
	.loc 1 72 0
	movl	%esi, 0(%rbp)
	.loc 1 77 0
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L29:
	.cfi_restore_state
.LVL53:
.LBB90:
.LBB91:
	.loc 2 105 0
	movl	$1, %edi
.LVL54:
	movl	$.LC5, %esi
.LVL55:
	xorl	%eax, %eax
	call	__printf_chk
.LVL56:
.LBE91:
.LBE90:
	.loc 1 69 0
	movl	$-1, %edi
	call	exit
.LVL57:
	.cfi_endproc
.LFE47:
	.size	vector_set_length, .-vector_set_length
	.p2align 4,,15
	.globl	vector_add
	.type	vector_add, @function
vector_add:
.LFB48:
	.loc 1 79 0
	.cfi_startproc
.LVL58:
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movq	%rsi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	.loc 1 80 0
	movslq	(%rdi), %rbp
	.loc 1 79 0
	movq	%rdi, %rbx
.LBB99:
.LBB100:
	.loc 1 56 0
	movl	4(%rdi), %eax
.LBE100:
.LBE99:
	.loc 1 80 0
	leal	1(%rbp), %edx
.LVL59:
.LBB114:
.LBB112:
	.loc 1 56 0
	cmpl	%eax, %edx
	jg	.L33
	movq	8(%rdi), %rax
.LBE112:
.LBE114:
	.loc 1 81 0
	movq	%r12, (%rax,%rbp,8)
	.loc 1 82 0
	movl	%edx, (%rbx)
	.loc 1 83 0
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.LVL60:
	.p2align 4,,10
	.p2align 3
.L33:
	.cfi_restore_state
.LBB115:
.LBB113:
.LBB101:
	.loc 1 57 0
	addl	%eax, %eax
.LVL61:
.LBB102:
.LBB103:
	.loc 1 11 0
	cmpl	%edx, %eax
	cmovge	%eax, %edx
.LVL62:
.LBE103:
.LBE102:
	.loc 1 58 0
	movslq	%edx, %rdi
.LBB105:
.LBB104:
	.loc 1 11 0
	movl	%edx, %r14d
.LBE104:
.LBE105:
	.loc 1 58 0
	salq	$3, %rdi
	call	malloc
.LVL63:
.LBB106:
.LBB107:
	.loc 3 52 0
	movq	8(%rbx), %rsi
.LBE107:
.LBE106:
	.loc 1 59 0
	leaq	0(,%rbp,8), %rdx
.LBB110:
.LBB108:
	.loc 3 52 0
	movq	%rax, %rdi
.LBE108:
.LBE110:
	.loc 1 58 0
	movq	%rax, %r13
.LVL64:
.LBB111:
.LBB109:
	.loc 3 52 0
	call	memcpy
.LVL65:
.LBE109:
.LBE111:
	.loc 1 60 0
	movq	8(%rbx), %rdi
	call	free
.LVL66:
	movslq	(%rbx), %rbp
.LVL67:
	.loc 1 62 0
	movq	%r13, %rax
	.loc 1 61 0
	movl	%r14d, 4(%rbx)
	.loc 1 62 0
	movq	%r13, 8(%rbx)
	leal	1(%rbp), %edx
.LBE101:
.LBE113:
.LBE115:
	.loc 1 81 0
	movq	%r12, (%rax,%rbp,8)
	.loc 1 82 0
	movl	%edx, (%rbx)
	.loc 1 83 0
	popq	%rbx
	.cfi_def_cfa_offset 40
.LVL68:
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
.LVL69:
	popq	%r13
	.cfi_def_cfa_offset 16
.LVL70:
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE48:
	.size	vector_add, .-vector_add
	.section	.rodata.str1.1
.LC6:
	.string	"Pop from empty vector.\n"
	.text
	.p2align 4,,15
	.globl	vector_pop
	.type	vector_pop, @function
vector_pop:
.LFB49:
	.loc 1 85 0
	.cfi_startproc
.LVL71:
	.loc 1 86 0
	movl	(%rdi), %eax
	testl	%eax, %eax
	je	.L39
	.loc 1 91 0
	movq	8(%rdi), %rdx
	.loc 1 90 0
	subl	$1, %eax
	movl	%eax, (%rdi)
	.loc 1 91 0
	cltq
	movq	(%rdx,%rax,8), %rax
	.loc 1 92 0
	ret
.L39:
.LVL72:
	.loc 1 85 0
	pushq	%rdx
	.cfi_def_cfa_offset 16
.LBB116:
.LBB117:
	.loc 2 105 0
	movl	$1, %edi
.LVL73:
	movl	$.LC6, %esi
	call	__printf_chk
.LVL74:
.LBE117:
.LBE116:
	.loc 1 88 0
	orl	$-1, %edi
	call	exit
.LVL75:
	.cfi_endproc
.LFE49:
	.size	vector_pop, .-vector_pop
	.section	.rodata.str1.1
.LC7:
	.string	"Peek from empty vector.\n"
	.text
	.p2align 4,,15
	.globl	vector_peek
	.type	vector_peek, @function
vector_peek:
.LFB50:
	.loc 1 94 0
	.cfi_startproc
.LVL76:
	.loc 1 95 0
	movslq	(%rdi), %rax
	testl	%eax, %eax
	je	.L43
	.loc 1 99 0
	movq	8(%rdi), %rdx
	movq	-8(%rdx,%rax,8), %rax
	.loc 1 100 0
	ret
.L43:
.LVL77:
	.loc 1 94 0
	pushq	%rcx
	.cfi_def_cfa_offset 16
.LBB118:
.LBB119:
	.loc 2 105 0
	movl	$1, %edi
.LVL78:
	movl	$.LC7, %esi
	call	__printf_chk
.LVL79:
.LBE119:
.LBE118:
	.loc 1 97 0
	orl	$-1, %edi
	call	exit
.LVL80:
	.cfi_endproc
.LFE50:
	.size	vector_peek, .-vector_peek
	.p2align 4,,15
	.globl	vector_clear
	.type	vector_clear, @function
vector_clear:
.LFB51:
	.loc 1 102 0
	.cfi_startproc
.LVL81:
	.loc 1 103 0
	movl	$0, (%rdi)
	ret
	.cfi_endproc
.LFE51:
	.size	vector_clear, .-vector_clear
	.p2align 4,,15
	.globl	vector_free
	.type	vector_free, @function
vector_free:
.LFB52:
	.loc 1 106 0
	.cfi_startproc
.LVL82:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	.loc 1 106 0
	movq	%rdi, %rbx
	.loc 1 107 0
	movq	8(%rdi), %rdi
.LVL83:
	call	free
.LVL84:
	.loc 1 108 0
	movq	%rbx, %rdi
	.loc 1 109 0
	popq	%rbx
	.cfi_def_cfa_offset 8
.LVL85:
	.loc 1 108 0
	jmp	free
.LVL86:
	.cfi_endproc
.LFE52:
	.size	vector_free, .-vector_free
	.section	.rodata.str1.1
.LC8:
	.string	"Index %d out of bounds.\n"
	.text
	.p2align 4,,15
	.globl	vector_get
	.type	vector_get, @function
vector_get:
.LFB53:
	.loc 1 111 0
	.cfi_startproc
.LVL87:
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	.loc 1 112 0
	testl	%esi, %esi
	js	.L48
	.loc 1 112 0 is_stmt 0 discriminator 1
	cmpl	(%rdi), %esi
	jge	.L48
	.loc 1 116 0 is_stmt 1
	movq	8(%rdi), %rax
	movslq	%esi, %rsi
	movq	(%rax,%rsi,8), %rax
	.loc 1 117 0
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L48:
	.cfi_restore_state
.LVL88:
.LBB120:
.LBB121:
	.loc 2 105 0
	movl	%esi, %edx
	movl	$1, %edi
.LVL89:
	movl	$.LC8, %esi
.LVL90:
	xorl	%eax, %eax
	call	__printf_chk
.LVL91:
.LBE121:
.LBE120:
	.loc 1 114 0
	orl	$-1, %edi
	call	exit
.LVL92:
	.cfi_endproc
.LFE53:
	.size	vector_get, .-vector_get
	.p2align 4,,15
	.globl	vector_set
	.type	vector_set, @function
vector_set:
.LFB54:
	.loc 1 119 0
	.cfi_startproc
.LVL93:
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$24, %rsp
	.cfi_def_cfa_offset 64
	.loc 1 120 0
	testl	%esi, %esi
	js	.L52
	.loc 1 120 0 is_stmt 0 discriminator 1
	cmpl	(%rdi), %esi
	movq	%rdi, %rbx
	jg	.L52
	movq	%rdx, %rbp
	.loc 1 123 0 is_stmt 1
	je	.L59
	.loc 1 126 0
	movq	8(%rdi), %rax
	movslq	%esi, %rsi
	movq	%rdx, (%rax,%rsi,8)
.LVL94:
.L51:
	.loc 1 128 0
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
.LVL95:
	popq	%rbp
	.cfi_def_cfa_offset 24
.LVL96:
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.LVL97:
	.p2align 4,,10
	.p2align 3
.L59:
	.cfi_restore_state
.LBB133:
.LBB134:
.LBB135:
.LBB136:
	.loc 1 56 0
	movl	4(%rdi), %edx
.LVL98:
.LBE136:
.LBE135:
	.loc 1 80 0
	leal	1(%rsi), %eax
.LVL99:
.LBB150:
.LBB148:
	.loc 1 56 0
	cmpl	%edx, %eax
	jg	.L55
	movq	8(%rdi), %rax
.LVL100:
.L56:
.LBE148:
.LBE150:
	.loc 1 81 0
	movslq	%esi, %rdx
	.loc 1 82 0
	addl	$1, %esi
	.loc 1 81 0
	movq	%rbp, (%rax,%rdx,8)
	.loc 1 82 0
	movl	%esi, (%rbx)
	jmp	.L51
.LVL101:
	.p2align 4,,10
	.p2align 3
.L55:
.LBB151:
.LBB149:
.LBB137:
	.loc 1 57 0
	addl	%edx, %edx
	movl	%esi, 12(%rsp)
.LVL102:
.LBB138:
.LBB139:
	.loc 1 11 0
	cmpl	%edx, %eax
	cmovge	%eax, %edx
.LBE139:
.LBE138:
	.loc 1 58 0
	movslq	%edx, %rdi
.LBB141:
.LBB140:
	.loc 1 11 0
	movl	%edx, %r13d
.LBE140:
.LBE141:
	.loc 1 58 0
	salq	$3, %rdi
	call	malloc
.LVL103:
	.loc 1 59 0
	movl	12(%rsp), %esi
.LBB142:
.LBB143:
	.loc 3 52 0
	movq	%rax, %rdi
.LBE143:
.LBE142:
	.loc 1 58 0
	movq	%rax, %r12
.LVL104:
	.loc 1 59 0
	movslq	%esi, %rdx
.LBB146:
.LBB144:
	.loc 3 52 0
	movq	8(%rbx), %rsi
.LVL105:
.LBE144:
.LBE146:
	.loc 1 59 0
	salq	$3, %rdx
.LVL106:
.LBB147:
.LBB145:
	.loc 3 52 0
	call	memcpy
.LVL107:
.LBE145:
.LBE147:
	.loc 1 60 0
	movq	8(%rbx), %rdi
	call	free
.LVL108:
	.loc 1 61 0
	movl	%r13d, 4(%rbx)
	.loc 1 62 0
	movq	%r12, 8(%rbx)
	movq	%r12, %rax
	movl	(%rbx), %esi
	jmp	.L56
.LVL109:
.L52:
.LBE137:
.LBE149:
.LBE151:
.LBE134:
.LBE133:
.LBB152:
.LBB153:
	.loc 2 105 0
	movl	%esi, %edx
.LVL110:
	movl	$1, %edi
.LVL111:
	movl	$.LC8, %esi
.LVL112:
	xorl	%eax, %eax
	call	__printf_chk
.LVL113:
.LBE153:
.LBE152:
	.loc 1 122 0
	orl	$-1, %edi
	call	exit
.LVL114:
	.cfi_endproc
.LFE54:
	.size	vector_set, .-vector_set
.Letext0:
	.file 4 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h"
	.file 5 "/usr/include/x86_64-linux-gnu/bits/types.h"
	.file 6 "/usr/include/libio.h"
	.file 7 "utils.h"
	.file 8 "/usr/include/stdio.h"
	.file 9 "/usr/include/stdlib.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0xe93
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF73
	.byte	0x1
	.long	.LASF74
	.long	.LASF75
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF7
	.byte	0x4
	.byte	0xd4
	.long	0x38
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF2
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF3
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF4
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF5
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF6
	.uleb128 0x2
	.long	.LASF8
	.byte	0x5
	.byte	0x8d
	.long	0x69
	.uleb128 0x2
	.long	.LASF9
	.byte	0x5
	.byte	0x8e
	.long	0x69
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF10
	.uleb128 0x5
	.byte	0x8
	.uleb128 0x6
	.byte	0x8
	.long	0x95
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF11
	.uleb128 0x7
	.long	.LASF41
	.byte	0xd8
	.byte	0x6
	.value	0x111
	.long	0x223
	.uleb128 0x8
	.long	.LASF12
	.byte	0x6
	.value	0x112
	.long	0x62
	.byte	0
	.uleb128 0x8
	.long	.LASF13
	.byte	0x6
	.value	0x117
	.long	0x8f
	.byte	0x8
	.uleb128 0x8
	.long	.LASF14
	.byte	0x6
	.value	0x118
	.long	0x8f
	.byte	0x10
	.uleb128 0x8
	.long	.LASF15
	.byte	0x6
	.value	0x119
	.long	0x8f
	.byte	0x18
	.uleb128 0x8
	.long	.LASF16
	.byte	0x6
	.value	0x11a
	.long	0x8f
	.byte	0x20
	.uleb128 0x8
	.long	.LASF17
	.byte	0x6
	.value	0x11b
	.long	0x8f
	.byte	0x28
	.uleb128 0x8
	.long	.LASF18
	.byte	0x6
	.value	0x11c
	.long	0x8f
	.byte	0x30
	.uleb128 0x8
	.long	.LASF19
	.byte	0x6
	.value	0x11d
	.long	0x8f
	.byte	0x38
	.uleb128 0x8
	.long	.LASF20
	.byte	0x6
	.value	0x11e
	.long	0x8f
	.byte	0x40
	.uleb128 0x8
	.long	.LASF21
	.byte	0x6
	.value	0x120
	.long	0x8f
	.byte	0x48
	.uleb128 0x8
	.long	.LASF22
	.byte	0x6
	.value	0x121
	.long	0x8f
	.byte	0x50
	.uleb128 0x8
	.long	.LASF23
	.byte	0x6
	.value	0x122
	.long	0x8f
	.byte	0x58
	.uleb128 0x8
	.long	.LASF24
	.byte	0x6
	.value	0x124
	.long	0x25b
	.byte	0x60
	.uleb128 0x8
	.long	.LASF25
	.byte	0x6
	.value	0x126
	.long	0x261
	.byte	0x68
	.uleb128 0x8
	.long	.LASF26
	.byte	0x6
	.value	0x128
	.long	0x62
	.byte	0x70
	.uleb128 0x8
	.long	.LASF27
	.byte	0x6
	.value	0x12c
	.long	0x62
	.byte	0x74
	.uleb128 0x8
	.long	.LASF28
	.byte	0x6
	.value	0x12e
	.long	0x70
	.byte	0x78
	.uleb128 0x8
	.long	.LASF29
	.byte	0x6
	.value	0x132
	.long	0x46
	.byte	0x80
	.uleb128 0x8
	.long	.LASF30
	.byte	0x6
	.value	0x133
	.long	0x54
	.byte	0x82
	.uleb128 0x8
	.long	.LASF31
	.byte	0x6
	.value	0x134
	.long	0x267
	.byte	0x83
	.uleb128 0x8
	.long	.LASF32
	.byte	0x6
	.value	0x138
	.long	0x277
	.byte	0x88
	.uleb128 0x8
	.long	.LASF33
	.byte	0x6
	.value	0x141
	.long	0x7b
	.byte	0x90
	.uleb128 0x8
	.long	.LASF34
	.byte	0x6
	.value	0x14a
	.long	0x8d
	.byte	0x98
	.uleb128 0x8
	.long	.LASF35
	.byte	0x6
	.value	0x14b
	.long	0x8d
	.byte	0xa0
	.uleb128 0x8
	.long	.LASF36
	.byte	0x6
	.value	0x14c
	.long	0x8d
	.byte	0xa8
	.uleb128 0x8
	.long	.LASF37
	.byte	0x6
	.value	0x14d
	.long	0x8d
	.byte	0xb0
	.uleb128 0x8
	.long	.LASF38
	.byte	0x6
	.value	0x14e
	.long	0x2d
	.byte	0xb8
	.uleb128 0x8
	.long	.LASF39
	.byte	0x6
	.value	0x150
	.long	0x62
	.byte	0xc0
	.uleb128 0x8
	.long	.LASF40
	.byte	0x6
	.value	0x152
	.long	0x27d
	.byte	0xc4
	.byte	0
	.uleb128 0x9
	.long	.LASF76
	.byte	0x6
	.byte	0xb6
	.uleb128 0xa
	.long	.LASF42
	.byte	0x18
	.byte	0x6
	.byte	0xbc
	.long	0x25b
	.uleb128 0xb
	.long	.LASF43
	.byte	0x6
	.byte	0xbd
	.long	0x25b
	.byte	0
	.uleb128 0xb
	.long	.LASF44
	.byte	0x6
	.byte	0xbe
	.long	0x261
	.byte	0x8
	.uleb128 0xb
	.long	.LASF45
	.byte	0x6
	.byte	0xc2
	.long	0x62
	.byte	0x10
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x22a
	.uleb128 0x6
	.byte	0x8
	.long	0x9c
	.uleb128 0xc
	.long	0x95
	.long	0x277
	.uleb128 0xd
	.long	0x86
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x223
	.uleb128 0xc
	.long	0x95
	.long	0x28d
	.uleb128 0xd
	.long	0x86
	.byte	0x13
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x293
	.uleb128 0xe
	.long	0x95
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF46
	.uleb128 0x6
	.byte	0x8
	.long	0x2a5
	.uleb128 0xf
	.uleb128 0x10
	.byte	0x10
	.byte	0x7
	.byte	0xc
	.long	0x2d3
	.uleb128 0xb
	.long	.LASF47
	.byte	0x7
	.byte	0xd
	.long	0x62
	.byte	0
	.uleb128 0xb
	.long	.LASF48
	.byte	0x7
	.byte	0xe
	.long	0x62
	.byte	0x4
	.uleb128 0xb
	.long	.LASF49
	.byte	0x7
	.byte	0xf
	.long	0x2d3
	.byte	0x8
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x8d
	.uleb128 0x2
	.long	.LASF50
	.byte	0x7
	.byte	0x10
	.long	0x2a6
	.uleb128 0x11
	.long	.LASF52
	.byte	0x2
	.byte	0x67
	.long	0x62
	.byte	0x3
	.long	0x301
	.uleb128 0x12
	.long	.LASF51
	.byte	0x2
	.byte	0x67
	.long	0x28d
	.uleb128 0x13
	.byte	0
	.uleb128 0x14
	.string	"max"
	.byte	0x1
	.byte	0xa
	.long	0x62
	.byte	0x1
	.long	0x324
	.uleb128 0x15
	.string	"a"
	.byte	0x1
	.byte	0xa
	.long	0x62
	.uleb128 0x15
	.string	"b"
	.byte	0x1
	.byte	0xa
	.long	0x62
	.byte	0
	.uleb128 0x11
	.long	.LASF53
	.byte	0x3
	.byte	0x31
	.long	0x8d
	.byte	0x3
	.long	0x356
	.uleb128 0x12
	.long	.LASF54
	.byte	0x3
	.byte	0x31
	.long	0x8d
	.uleb128 0x12
	.long	.LASF55
	.byte	0x3
	.byte	0x31
	.long	0x29f
	.uleb128 0x12
	.long	.LASF56
	.byte	0x3
	.byte	0x31
	.long	0x2d
	.byte	0
	.uleb128 0x16
	.long	0x301
	.quad	.LFB42
	.quad	.LFE42-.LFB42
	.uleb128 0x1
	.byte	0x9c
	.long	0x380
	.uleb128 0x17
	.long	0x311
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x17
	.long	0x31a
	.uleb128 0x1
	.byte	0x54
	.byte	0
	.uleb128 0x18
	.string	"min"
	.byte	0x1
	.byte	0xe
	.long	0x62
	.quad	.LFB43
	.quad	.LFE43-.LFB43
	.uleb128 0x1
	.byte	0x9c
	.long	0x3b8
	.uleb128 0x19
	.string	"a"
	.byte	0x1
	.byte	0xe
	.long	0x62
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x19
	.string	"b"
	.byte	0x1
	.byte	0xe
	.long	0x62
	.uleb128 0x1
	.byte	0x54
	.byte	0
	.uleb128 0x1a
	.long	.LASF57
	.byte	0x1
	.byte	0x12
	.quad	.LFB44
	.quad	.LFE44-.LFB44
	.uleb128 0x1
	.byte	0x9c
	.long	0x59c
	.uleb128 0x1b
	.string	"str"
	.byte	0x1
	.byte	0x12
	.long	0x8f
	.long	.LLST0
	.uleb128 0x1c
	.long	0x2e4
	.quad	.LBB30
	.long	.Ldebug_ranges0+0
	.byte	0x1
	.byte	0x13
	.long	0x42c
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC0
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL4
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC0
	.byte	0
	.byte	0
	.uleb128 0x1f
	.long	.Ldebug_ranges0+0x30
	.uleb128 0x20
	.string	"c"
	.byte	0x1
	.byte	0x15
	.long	0x95
	.long	.LLST1
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB35
	.quad	.LBE35-.LBB35
	.byte	0x1
	.byte	0x1c
	.long	0x483
	.uleb128 0x22
	.long	0x2f4
	.long	.LLST2
	.uleb128 0x1d
	.quad	.LVL8
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC2
	.byte	0
	.byte	0
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB37
	.quad	.LBE37-.LBB37
	.byte	0x1
	.byte	0x19
	.long	0x4c8
	.uleb128 0x22
	.long	0x2f4
	.long	.LLST3
	.uleb128 0x1d
	.quad	.LVL12
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.byte	0
	.byte	0
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB39
	.quad	.LBE39-.LBB39
	.byte	0x1
	.byte	0x22
	.long	0x50d
	.uleb128 0x22
	.long	0x2f4
	.long	.LLST4
	.uleb128 0x23
	.quad	.LVL16
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC0
	.byte	0
	.byte	0
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB41
	.quad	.LBE41-.LBB41
	.byte	0x1
	.byte	0x25
	.long	0x552
	.uleb128 0x22
	.long	0x2f4
	.long	.LLST5
	.uleb128 0x1d
	.quad	.LVL17
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC4
	.byte	0
	.byte	0
	.uleb128 0x24
	.long	0x2e4
	.quad	.LBB43
	.quad	.LBE43-.LBB43
	.byte	0x1
	.byte	0x1f
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC3
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL19
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC3
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x25
	.long	.LASF58
	.byte	0x1
	.byte	0x2f
	.long	0x5f6
	.quad	.LFB45
	.quad	.LFE45-.LFB45
	.uleb128 0x1
	.byte	0x9c
	.long	0x5f6
	.uleb128 0x20
	.string	"v"
	.byte	0x1
	.byte	0x30
	.long	0x5f6
	.long	.LLST6
	.uleb128 0x26
	.quad	.LVL20
	.long	0xe43
	.long	0x5e1
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x40
	.byte	0
	.uleb128 0x1d
	.quad	.LVL22
	.long	0xe43
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x8
	.byte	0x40
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x2d9
	.uleb128 0x27
	.long	.LASF59
	.byte	0x1
	.byte	0x37
	.byte	0x1
	.long	0x631
	.uleb128 0x15
	.string	"v"
	.byte	0x1
	.byte	0x37
	.long	0x5f6
	.uleb128 0x15
	.string	"c"
	.byte	0x1
	.byte	0x37
	.long	0x62
	.uleb128 0x28
	.uleb128 0x29
	.string	"c2"
	.byte	0x1
	.byte	0x39
	.long	0x62
	.uleb128 0x29
	.string	"a2"
	.byte	0x1
	.byte	0x3a
	.long	0x2d3
	.byte	0
	.byte	0
	.uleb128 0x16
	.long	0x5fc
	.quad	.LFB46
	.quad	.LFE46-.LFB46
	.uleb128 0x1
	.byte	0x9c
	.long	0x71d
	.uleb128 0x22
	.long	0x608
	.long	.LLST7
	.uleb128 0x22
	.long	0x611
	.long	.LLST8
	.uleb128 0x2a
	.quad	.LBB51
	.quad	.LBE51-.LBB51
	.uleb128 0x2b
	.long	0x61b
	.uleb128 0x2c
	.long	0x625
	.long	.LLST9
	.uleb128 0x1c
	.long	0x301
	.quad	.LBB52
	.long	.Ldebug_ranges0+0x60
	.byte	0x1
	.byte	0x39
	.long	0x6a7
	.uleb128 0x22
	.long	0x31a
	.long	.LLST10
	.uleb128 0x22
	.long	0x311
	.long	.LLST11
	.byte	0
	.uleb128 0x1c
	.long	0x324
	.quad	.LBB56
	.long	.Ldebug_ranges0+0x90
	.byte	0x1
	.byte	0x3b
	.long	0x6ee
	.uleb128 0x22
	.long	0x34a
	.long	.LLST12
	.uleb128 0x22
	.long	0x33f
	.long	.LLST13
	.uleb128 0x22
	.long	0x334
	.long	.LLST9
	.uleb128 0x1d
	.quad	.LVL32
	.long	0xe59
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x26
	.quad	.LVL29
	.long	0xe43
	.long	0x70e
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0xa
	.byte	0x7c
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0
	.uleb128 0x2d
	.quad	.LVL33
	.long	0xe76
	.byte	0
	.byte	0
	.uleb128 0x27
	.long	.LASF60
	.byte	0x1
	.byte	0x4f
	.byte	0x1
	.long	0x73e
	.uleb128 0x15
	.string	"v"
	.byte	0x1
	.byte	0x4f
	.long	0x5f6
	.uleb128 0x15
	.string	"val"
	.byte	0x1
	.byte	0x4f
	.long	0x8d
	.byte	0
	.uleb128 0x1a
	.long	.LASF61
	.byte	0x1
	.byte	0x42
	.quad	.LFB47
	.quad	.LFE47-.LFB47
	.uleb128 0x1
	.byte	0x9c
	.long	0x8f7
	.uleb128 0x1b
	.string	"v"
	.byte	0x1
	.byte	0x42
	.long	0x5f6
	.long	.LLST15
	.uleb128 0x1b
	.string	"len"
	.byte	0x1
	.byte	0x42
	.long	0x62
	.long	.LLST16
	.uleb128 0x1b
	.string	"x"
	.byte	0x1
	.byte	0x42
	.long	0x8d
	.long	.LLST17
	.uleb128 0x1c
	.long	0x71d
	.quad	.LBB71
	.long	.Ldebug_ranges0+0xc0
	.byte	0x1
	.byte	0x4b
	.long	0x896
	.uleb128 0x2e
	.long	0x732
	.uleb128 0x2e
	.long	0x729
	.uleb128 0x24
	.long	0x5fc
	.quad	.LBB73
	.quad	.LBE73-.LBB73
	.byte	0x1
	.byte	0x50
	.uleb128 0x22
	.long	0x611
	.long	.LLST18
	.uleb128 0x2e
	.long	0x608
	.uleb128 0x2a
	.quad	.LBB75
	.quad	.LBE75-.LBB75
	.uleb128 0x2b
	.long	0x61b
	.uleb128 0x2c
	.long	0x625
	.long	.LLST19
	.uleb128 0x21
	.long	0x301
	.quad	.LBB76
	.quad	.LBE76-.LBB76
	.byte	0x1
	.byte	0x39
	.long	0x817
	.uleb128 0x22
	.long	0x31a
	.long	.LLST20
	.uleb128 0x22
	.long	0x311
	.long	.LLST21
	.byte	0
	.uleb128 0x1c
	.long	0x324
	.quad	.LBB78
	.long	.Ldebug_ranges0+0x110
	.byte	0x1
	.byte	0x3b
	.long	0x866
	.uleb128 0x22
	.long	0x34a
	.long	.LLST22
	.uleb128 0x22
	.long	0x33f
	.long	.LLST23
	.uleb128 0x22
	.long	0x334
	.long	.LLST19
	.uleb128 0x1d
	.quad	.LVL45
	.long	0xe59
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7d
	.sleb128 0
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x4
	.byte	0x73
	.sleb128 0
	.byte	0x33
	.byte	0x24
	.byte	0
	.byte	0
	.uleb128 0x26
	.quad	.LVL43
	.long	0xe43
	.long	0x886
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0xa
	.byte	0x7c
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0
	.uleb128 0x2d
	.quad	.LVL46
	.long	0xe76
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB90
	.quad	.LBE90-.LBB90
	.byte	0x1
	.byte	0x44
	.long	0x8e2
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC5
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL56
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC5
	.byte	0
	.byte	0
	.uleb128 0x1d
	.quad	.LVL57
	.long	0xe88
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x16
	.long	0x71d
	.quad	.LFB48
	.quad	.LFE48-.LFB48
	.uleb128 0x1
	.byte	0x9c
	.long	0xa11
	.uleb128 0x22
	.long	0x729
	.long	.LLST25
	.uleb128 0x22
	.long	0x732
	.long	.LLST26
	.uleb128 0x2f
	.long	0x5fc
	.quad	.LBB99
	.long	.Ldebug_ranges0+0x150
	.byte	0x1
	.byte	0x50
	.uleb128 0x22
	.long	0x611
	.long	.LLST27
	.uleb128 0x22
	.long	0x608
	.long	.LLST28
	.uleb128 0x2a
	.quad	.LBB101
	.quad	.LBE101-.LBB101
	.uleb128 0x2b
	.long	0x61b
	.uleb128 0x2c
	.long	0x625
	.long	.LLST29
	.uleb128 0x1c
	.long	0x301
	.quad	.LBB102
	.long	.Ldebug_ranges0+0x190
	.byte	0x1
	.byte	0x39
	.long	0x992
	.uleb128 0x22
	.long	0x31a
	.long	.LLST30
	.uleb128 0x22
	.long	0x311
	.long	.LLST31
	.byte	0
	.uleb128 0x1c
	.long	0x324
	.quad	.LBB106
	.long	.Ldebug_ranges0+0x1c0
	.byte	0x1
	.byte	0x3b
	.long	0x9e1
	.uleb128 0x22
	.long	0x34a
	.long	.LLST32
	.uleb128 0x22
	.long	0x33f
	.long	.LLST33
	.uleb128 0x22
	.long	0x334
	.long	.LLST29
	.uleb128 0x1d
	.quad	.LVL65
	.long	0xe59
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7d
	.sleb128 0
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x4
	.byte	0x76
	.sleb128 0
	.byte	0x33
	.byte	0x24
	.byte	0
	.byte	0
	.uleb128 0x26
	.quad	.LVL63
	.long	0xe43
	.long	0xa01
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0xa
	.byte	0x7e
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0
	.uleb128 0x2d
	.quad	.LVL66
	.long	0xe76
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x30
	.long	.LASF62
	.byte	0x1
	.byte	0x55
	.long	0x8d
	.quad	.LFB49
	.quad	.LFE49-.LFB49
	.uleb128 0x1
	.byte	0x9c
	.long	0xaa0
	.uleb128 0x1b
	.string	"v"
	.byte	0x1
	.byte	0x55
	.long	0x5f6
	.long	.LLST35
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB116
	.quad	.LBE116-.LBB116
	.byte	0x1
	.byte	0x57
	.long	0xa8b
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC6
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL74
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC6
	.byte	0
	.byte	0
	.uleb128 0x1d
	.quad	.LVL75
	.long	0xe88
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x30
	.long	.LASF63
	.byte	0x1
	.byte	0x5e
	.long	0x8d
	.quad	.LFB50
	.quad	.LFE50-.LFB50
	.uleb128 0x1
	.byte	0x9c
	.long	0xb2f
	.uleb128 0x1b
	.string	"v"
	.byte	0x1
	.byte	0x5e
	.long	0x5f6
	.long	.LLST36
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB118
	.quad	.LBE118-.LBB118
	.byte	0x1
	.byte	0x60
	.long	0xb1a
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC7
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL79
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC7
	.byte	0
	.byte	0
	.uleb128 0x1d
	.quad	.LVL80
	.long	0xe88
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x1a
	.long	.LASF64
	.byte	0x1
	.byte	0x66
	.quad	.LFB51
	.quad	.LFE51-.LFB51
	.uleb128 0x1
	.byte	0x9c
	.long	0xb58
	.uleb128 0x19
	.string	"v"
	.byte	0x1
	.byte	0x66
	.long	0x5f6
	.uleb128 0x1
	.byte	0x55
	.byte	0
	.uleb128 0x1a
	.long	.LASF65
	.byte	0x1
	.byte	0x6a
	.quad	.LFB52
	.quad	.LFE52-.LFB52
	.uleb128 0x1
	.byte	0x9c
	.long	0xba5
	.uleb128 0x1b
	.string	"v"
	.byte	0x1
	.byte	0x6a
	.long	0x5f6
	.long	.LLST37
	.uleb128 0x2d
	.quad	.LVL84
	.long	0xe76
	.uleb128 0x23
	.quad	.LVL86
	.long	0xe76
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0
	.byte	0
	.uleb128 0x30
	.long	.LASF66
	.byte	0x1
	.byte	0x6f
	.long	0x8d
	.quad	.LFB53
	.quad	.LFE53-.LFB53
	.uleb128 0x1
	.byte	0x9c
	.long	0xc48
	.uleb128 0x1b
	.string	"v"
	.byte	0x1
	.byte	0x6f
	.long	0x5f6
	.long	.LLST38
	.uleb128 0x1b
	.string	"i"
	.byte	0x1
	.byte	0x6f
	.long	0x62
	.long	.LLST39
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB120
	.quad	.LBE120-.LBB120
	.byte	0x1
	.byte	0x71
	.long	0xc33
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC8
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL91
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC8
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0
	.byte	0
	.uleb128 0x1d
	.quad	.LVL92
	.long	0xe88
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x1a
	.long	.LASF67
	.byte	0x1
	.byte	0x77
	.quad	.LFB54
	.quad	.LFE54-.LFB54
	.uleb128 0x1
	.byte	0x9c
	.long	0xe12
	.uleb128 0x1b
	.string	"v"
	.byte	0x1
	.byte	0x77
	.long	0x5f6
	.long	.LLST40
	.uleb128 0x1b
	.string	"i"
	.byte	0x1
	.byte	0x77
	.long	0x62
	.long	.LLST41
	.uleb128 0x1b
	.string	"x"
	.byte	0x1
	.byte	0x77
	.long	0x8d
	.long	.LLST42
	.uleb128 0x21
	.long	0x71d
	.quad	.LBB133
	.quad	.LBE133-.LBB133
	.byte	0x1
	.byte	0x7c
	.long	0xdaa
	.uleb128 0x22
	.long	0x732
	.long	.LLST43
	.uleb128 0x22
	.long	0x729
	.long	.LLST44
	.uleb128 0x2f
	.long	0x5fc
	.quad	.LBB135
	.long	.Ldebug_ranges0+0x200
	.byte	0x1
	.byte	0x50
	.uleb128 0x22
	.long	0x611
	.long	.LLST45
	.uleb128 0x22
	.long	0x608
	.long	.LLST46
	.uleb128 0x2a
	.quad	.LBB137
	.quad	.LBE137-.LBB137
	.uleb128 0x2b
	.long	0x61b
	.uleb128 0x2c
	.long	0x625
	.long	.LLST47
	.uleb128 0x1c
	.long	0x301
	.quad	.LBB138
	.long	.Ldebug_ranges0+0x240
	.byte	0x1
	.byte	0x39
	.long	0xd23
	.uleb128 0x22
	.long	0x31a
	.long	.LLST48
	.uleb128 0x2e
	.long	0x311
	.byte	0
	.uleb128 0x1c
	.long	0x324
	.quad	.LBB142
	.long	.Ldebug_ranges0+0x270
	.byte	0x1
	.byte	0x3b
	.long	0xd7a
	.uleb128 0x22
	.long	0x34a
	.long	.LLST49
	.uleb128 0x22
	.long	0x33f
	.long	.LLST50
	.uleb128 0x22
	.long	0x334
	.long	.LLST47
	.uleb128 0x1d
	.quad	.LVL107
	.long	0xe59
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7c
	.sleb128 0
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x51
	.uleb128 0xc
	.byte	0x91
	.sleb128 -52
	.byte	0x94
	.byte	0x4
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0
	.byte	0
	.uleb128 0x26
	.quad	.LVL103
	.long	0xe43
	.long	0xd9a
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0xa
	.byte	0x7d
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0
	.uleb128 0x2d
	.quad	.LVL108
	.long	0xe76
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x21
	.long	0x2e4
	.quad	.LBB152
	.quad	.LBE152-.LBB152
	.byte	0x1
	.byte	0x79
	.long	0xdfd
	.uleb128 0x17
	.long	0x2f4
	.uleb128 0xa
	.byte	0x3
	.quad	.LC8
	.byte	0x9f
	.uleb128 0x1d
	.quad	.LVL113
	.long	0xe28
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC8
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0
	.byte	0
	.uleb128 0x1d
	.quad	.LVL114
	.long	0xe88
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x31
	.long	.LASF68
	.byte	0x8
	.byte	0xa9
	.long	0x261
	.uleb128 0x31
	.long	.LASF69
	.byte	0x8
	.byte	0xaa
	.long	0x261
	.uleb128 0x32
	.long	.LASF70
	.byte	0x2
	.byte	0x58
	.long	0x62
	.long	0xe43
	.uleb128 0x33
	.long	0x62
	.uleb128 0x33
	.long	0x28d
	.uleb128 0x13
	.byte	0
	.uleb128 0x34
	.long	.LASF71
	.byte	0x9
	.value	0x1d7
	.long	0x8d
	.long	0xe59
	.uleb128 0x33
	.long	0x2d
	.byte	0
	.uleb128 0x35
	.long	.LASF53
	.long	0x8d
	.long	0xe76
	.uleb128 0x33
	.long	0x8d
	.uleb128 0x33
	.long	0x29f
	.uleb128 0x33
	.long	0x86
	.byte	0
	.uleb128 0x36
	.long	.LASF72
	.byte	0x9
	.value	0x1e8
	.long	0xe88
	.uleb128 0x33
	.long	0x8d
	.byte	0
	.uleb128 0x37
	.long	.LASF77
	.byte	0x9
	.value	0x220
	.uleb128 0x33
	.long	0x62
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x34
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2115
	.uleb128 0x19
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0xb
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2d
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2e
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2f
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x30
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x31
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x32
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x33
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x34
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x35
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x36
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x37
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.quad	.LVL2-.Ltext0
	.quad	.LVL3-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL3-.Ltext0
	.quad	.LVL15-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL16-.Ltext0
	.quad	.LFE44-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST1:
	.quad	.LVL5-.Ltext0
	.quad	.LVL8-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL9-.Ltext0
	.quad	.LVL12-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL13-.Ltext0
	.quad	.LVL16-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL16-.Ltext0
	.quad	.LVL17-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL18-.Ltext0
	.quad	.LVL19-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST2:
	.quad	.LVL7-.Ltext0
	.quad	.LVL11-.Ltext0
	.value	0xa
	.byte	0x3
	.quad	.LC2
	.byte	0x9f
	.quad	0
	.quad	0
.LLST3:
	.quad	.LVL11-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0xa
	.byte	0x3
	.quad	.LC1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST4:
	.quad	.LVL14-.Ltext0
	.quad	.LVL16-.Ltext0
	.value	0xa
	.byte	0x3
	.quad	.LC0
	.byte	0x9f
	.quad	0
	.quad	0
.LLST5:
	.quad	.LVL16-.Ltext0
	.quad	.LVL18-.Ltext0
	.value	0xa
	.byte	0x3
	.quad	.LC4
	.byte	0x9f
	.quad	0
	.quad	0
.LLST6:
	.quad	.LVL21-.Ltext0
	.quad	.LVL22-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL22-1-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL23-.Ltext0
	.quad	.LFE45-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST7:
	.quad	.LVL24-.Ltext0
	.quad	.LVL28-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL28-.Ltext0
	.quad	.LVL35-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL35-.Ltext0
	.quad	.LFE46-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST8:
	.quad	.LVL24-.Ltext0
	.quad	.LVL29-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL29-1-.Ltext0
	.quad	.LFE46-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST9:
	.quad	.LVL30-.Ltext0
	.quad	.LVL32-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL32-1-.Ltext0
	.quad	.LVL34-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LLST10:
	.quad	.LVL25-.Ltext0
	.quad	.LVL29-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL29-1-.Ltext0
	.quad	.LVL34-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST11:
	.quad	.LVL25-.Ltext0
	.quad	.LVL26-.Ltext0
	.value	0x5
	.byte	0x70
	.sleb128 0
	.byte	0x31
	.byte	0x24
	.byte	0x9f
	.quad	.LVL26-.Ltext0
	.quad	.LVL27-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL27-.Ltext0
	.quad	.LVL28-.Ltext0
	.value	0x7
	.byte	0x75
	.sleb128 4
	.byte	0x94
	.byte	0x4
	.byte	0x31
	.byte	0x24
	.byte	0x9f
	.quad	.LVL28-.Ltext0
	.quad	.LVL29-1-.Ltext0
	.value	0x7
	.byte	0x73
	.sleb128 4
	.byte	0x94
	.byte	0x4
	.byte	0x31
	.byte	0x24
	.byte	0x9f
	.quad	0
	.quad	0
.LLST12:
	.quad	.LVL30-.Ltext0
	.quad	.LVL31-.Ltext0
	.value	0xd
	.byte	0x73
	.sleb128 0
	.byte	0x94
	.byte	0x4
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0x9f
	.quad	.LVL31-.Ltext0
	.quad	.LVL32-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST13:
	.quad	.LVL30-.Ltext0
	.quad	.LVL32-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	0
	.quad	0
.LLST15:
	.quad	.LVL36-.Ltext0
	.quad	.LVL37-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL37-.Ltext0
	.quad	.LVL49-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL49-.Ltext0
	.quad	.LVL52-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL52-.Ltext0
	.quad	.LVL54-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL54-.Ltext0
	.quad	.LFE47-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST16:
	.quad	.LVL36-.Ltext0
	.quad	.LVL37-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL37-.Ltext0
	.quad	.LVL50-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL50-.Ltext0
	.quad	.LVL52-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL52-.Ltext0
	.quad	.LVL55-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL55-.Ltext0
	.quad	.LFE47-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LLST17:
	.quad	.LVL36-.Ltext0
	.quad	.LVL37-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL37-.Ltext0
	.quad	.LVL51-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL51-.Ltext0
	.quad	.LVL52-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	.LVL52-.Ltext0
	.quad	.LVL56-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL56-1-.Ltext0
	.quad	.LFE47-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	0
	.quad	0
.LLST18:
	.quad	.LVL37-.Ltext0
	.quad	.LVL38-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL39-.Ltext0
	.quad	.LVL43-1-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL43-1-.Ltext0
	.quad	.LVL47-.Ltext0
	.value	0x3
	.byte	0x73
	.sleb128 1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST19:
	.quad	.LVL44-.Ltext0
	.quad	.LVL45-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL45-1-.Ltext0
	.quad	.LVL48-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	0
	.quad	0
.LLST20:
	.quad	.LVL40-.Ltext0
	.quad	.LVL43-1-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL43-1-.Ltext0
	.quad	.LVL47-.Ltext0
	.value	0x3
	.byte	0x73
	.sleb128 1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST21:
	.quad	.LVL40-.Ltext0
	.quad	.LVL41-.Ltext0
	.value	0x5
	.byte	0x7c
	.sleb128 0
	.byte	0x31
	.byte	0x24
	.byte	0x9f
	.quad	.LVL41-.Ltext0
	.quad	.LVL42-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST22:
	.quad	.LVL44-.Ltext0
	.quad	.LVL47-.Ltext0
	.value	0xb
	.byte	0x73
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0x9f
	.quad	0
	.quad	0
.LLST23:
	.quad	.LVL44-.Ltext0
	.quad	.LVL45-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	0
	.quad	0
.LLST25:
	.quad	.LVL58-.Ltext0
	.quad	.LVL60-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL60-.Ltext0
	.quad	.LVL68-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL68-.Ltext0
	.quad	.LFE48-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST26:
	.quad	.LVL58-.Ltext0
	.quad	.LVL63-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL63-1-.Ltext0
	.quad	.LVL69-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL69-.Ltext0
	.quad	.LFE48-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST27:
	.quad	.LVL59-.Ltext0
	.quad	.LVL62-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL62-.Ltext0
	.quad	.LVL67-.Ltext0
	.value	0x3
	.byte	0x76
	.sleb128 1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST28:
	.quad	.LVL59-.Ltext0
	.quad	.LVL60-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL60-.Ltext0
	.quad	.LVL68-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL68-.Ltext0
	.quad	.LFE48-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST29:
	.quad	.LVL64-.Ltext0
	.quad	.LVL65-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL65-1-.Ltext0
	.quad	.LVL70-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	.LVL70-.Ltext0
	.quad	.LFE48-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST30:
	.quad	.LVL60-.Ltext0
	.quad	.LVL62-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL62-.Ltext0
	.quad	.LVL67-.Ltext0
	.value	0x3
	.byte	0x76
	.sleb128 1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST31:
	.quad	.LVL60-.Ltext0
	.quad	.LVL61-.Ltext0
	.value	0x5
	.byte	0x70
	.sleb128 0
	.byte	0x31
	.byte	0x24
	.byte	0x9f
	.quad	.LVL61-.Ltext0
	.quad	.LVL63-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST32:
	.quad	.LVL64-.Ltext0
	.quad	.LVL67-.Ltext0
	.value	0xb
	.byte	0x76
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0x9f
	.quad	0
	.quad	0
.LLST33:
	.quad	.LVL64-.Ltext0
	.quad	.LVL65-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	0
	.quad	0
.LLST35:
	.quad	.LVL71-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL73-.Ltext0
	.quad	.LFE49-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST36:
	.quad	.LVL76-.Ltext0
	.quad	.LVL78-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL78-.Ltext0
	.quad	.LFE50-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST37:
	.quad	.LVL82-.Ltext0
	.quad	.LVL83-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL83-.Ltext0
	.quad	.LVL85-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL85-.Ltext0
	.quad	.LVL86-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL86-1-.Ltext0
	.quad	.LFE52-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST38:
	.quad	.LVL87-.Ltext0
	.quad	.LVL89-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL89-.Ltext0
	.quad	.LFE53-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST39:
	.quad	.LVL87-.Ltext0
	.quad	.LVL90-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL90-.Ltext0
	.quad	.LVL91-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL91-1-.Ltext0
	.quad	.LFE53-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST40:
	.quad	.LVL93-.Ltext0
	.quad	.LVL94-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL94-.Ltext0
	.quad	.LVL95-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL95-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL97-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL109-.Ltext0
	.quad	.LVL111-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL111-.Ltext0
	.quad	.LFE54-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST41:
	.quad	.LVL93-.Ltext0
	.quad	.LVL94-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL94-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL97-.Ltext0
	.quad	.LVL100-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL100-.Ltext0
	.quad	.LVL101-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL101-.Ltext0
	.quad	.LVL103-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL103-1-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x2
	.byte	0x91
	.sleb128 -52
	.quad	.LVL109-.Ltext0
	.quad	.LVL112-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL112-.Ltext0
	.quad	.LVL113-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL113-1-.Ltext0
	.quad	.LFE54-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST42:
	.quad	.LVL93-.Ltext0
	.quad	.LVL94-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL94-.Ltext0
	.quad	.LVL96-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL96-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	.LVL97-.Ltext0
	.quad	.LVL98-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL98-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL109-.Ltext0
	.quad	.LVL110-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL110-.Ltext0
	.quad	.LFE54-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	0
	.quad	0
.LLST43:
	.quad	.LVL97-.Ltext0
	.quad	.LVL98-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL98-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LLST44:
	.quad	.LVL97-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST45:
	.quad	.LVL99-.Ltext0
	.quad	.LVL100-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL100-.Ltext0
	.quad	.LVL101-.Ltext0
	.value	0x6
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x23
	.uleb128 0x1
	.byte	0x9f
	.quad	.LVL101-.Ltext0
	.quad	.LVL103-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL103-1-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x7
	.byte	0x91
	.sleb128 -52
	.byte	0x94
	.byte	0x4
	.byte	0x23
	.uleb128 0x1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST46:
	.quad	.LVL99-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST47:
	.quad	.LVL104-.Ltext0
	.quad	.LVL107-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL107-1-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST48:
	.quad	.LVL102-.Ltext0
	.quad	.LVL103-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL103-1-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x7
	.byte	0x91
	.sleb128 -52
	.byte	0x94
	.byte	0x4
	.byte	0x23
	.uleb128 0x1
	.byte	0x9f
	.quad	0
	.quad	0
.LLST49:
	.quad	.LVL104-.Ltext0
	.quad	.LVL105-.Ltext0
	.value	0xb
	.byte	0x74
	.sleb128 0
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0x9f
	.quad	.LVL105-.Ltext0
	.quad	.LVL106-.Ltext0
	.value	0xd
	.byte	0x91
	.sleb128 -52
	.byte	0x94
	.byte	0x4
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0x9f
	.quad	.LVL106-.Ltext0
	.quad	.LVL107-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL107-1-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0xd
	.byte	0x91
	.sleb128 -52
	.byte	0x94
	.byte	0x4
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x33
	.byte	0x24
	.byte	0x9f
	.quad	0
	.quad	0
.LLST50:
	.quad	.LVL104-.Ltext0
	.quad	.LVL107-1-.Ltext0
	.value	0x2
	.byte	0x73
	.sleb128 8
	.quad	0
	.quad	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.quad	.LBB30-.Ltext0
	.quad	.LBE30-.Ltext0
	.quad	.LBB33-.Ltext0
	.quad	.LBE33-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB34-.Ltext0
	.quad	.LBE34-.Ltext0
	.quad	.LBB45-.Ltext0
	.quad	.LBE45-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB52-.Ltext0
	.quad	.LBE52-.Ltext0
	.quad	.LBB55-.Ltext0
	.quad	.LBE55-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB56-.Ltext0
	.quad	.LBE56-.Ltext0
	.quad	.LBB59-.Ltext0
	.quad	.LBE59-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB71-.Ltext0
	.quad	.LBE71-.Ltext0
	.quad	.LBB87-.Ltext0
	.quad	.LBE87-.Ltext0
	.quad	.LBB88-.Ltext0
	.quad	.LBE88-.Ltext0
	.quad	.LBB89-.Ltext0
	.quad	.LBE89-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB78-.Ltext0
	.quad	.LBE78-.Ltext0
	.quad	.LBB82-.Ltext0
	.quad	.LBE82-.Ltext0
	.quad	.LBB83-.Ltext0
	.quad	.LBE83-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB99-.Ltext0
	.quad	.LBE99-.Ltext0
	.quad	.LBB114-.Ltext0
	.quad	.LBE114-.Ltext0
	.quad	.LBB115-.Ltext0
	.quad	.LBE115-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB102-.Ltext0
	.quad	.LBE102-.Ltext0
	.quad	.LBB105-.Ltext0
	.quad	.LBE105-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB106-.Ltext0
	.quad	.LBE106-.Ltext0
	.quad	.LBB110-.Ltext0
	.quad	.LBE110-.Ltext0
	.quad	.LBB111-.Ltext0
	.quad	.LBE111-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB135-.Ltext0
	.quad	.LBE135-.Ltext0
	.quad	.LBB150-.Ltext0
	.quad	.LBE150-.Ltext0
	.quad	.LBB151-.Ltext0
	.quad	.LBE151-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB138-.Ltext0
	.quad	.LBE138-.Ltext0
	.quad	.LBB141-.Ltext0
	.quad	.LBE141-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB142-.Ltext0
	.quad	.LBE142-.Ltext0
	.quad	.LBB146-.Ltext0
	.quad	.LBE146-.Ltext0
	.quad	.LBB147-.Ltext0
	.quad	.LBE147-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF20:
	.string	"_IO_buf_end"
.LASF28:
	.string	"_old_offset"
.LASF49:
	.string	"array"
.LASF58:
	.string	"make_vector"
.LASF23:
	.string	"_IO_save_end"
.LASF5:
	.string	"short int"
.LASF7:
	.string	"size_t"
.LASF71:
	.string	"malloc"
.LASF10:
	.string	"sizetype"
.LASF33:
	.string	"_offset"
.LASF73:
	.string	"GNU C 4.8.1 -mtune=generic -march=x86-64 -g -O3 -std=c99 -fstack-protector"
.LASF17:
	.string	"_IO_write_ptr"
.LASF12:
	.string	"_flags"
.LASF53:
	.string	"memcpy"
.LASF62:
	.string	"vector_pop"
.LASF19:
	.string	"_IO_buf_base"
.LASF24:
	.string	"_markers"
.LASF14:
	.string	"_IO_read_end"
.LASF72:
	.string	"free"
.LASF46:
	.string	"long long int"
.LASF32:
	.string	"_lock"
.LASF6:
	.string	"long int"
.LASF54:
	.string	"__dest"
.LASF52:
	.string	"printf"
.LASF29:
	.string	"_cur_column"
.LASF45:
	.string	"_pos"
.LASF77:
	.string	"exit"
.LASF44:
	.string	"_sbuf"
.LASF41:
	.string	"_IO_FILE"
.LASF75:
	.string	"/scratch/gongliang13/cs294-113/CS294-113/JITEx/tmp"
.LASF69:
	.string	"stdout"
.LASF1:
	.string	"unsigned char"
.LASF55:
	.string	"__src"
.LASF4:
	.string	"signed char"
.LASF48:
	.string	"capacity"
.LASF3:
	.string	"unsigned int"
.LASF42:
	.string	"_IO_marker"
.LASF31:
	.string	"_shortbuf"
.LASF16:
	.string	"_IO_write_base"
.LASF40:
	.string	"_unused2"
.LASF13:
	.string	"_IO_read_ptr"
.LASF66:
	.string	"vector_get"
.LASF2:
	.string	"short unsigned int"
.LASF11:
	.string	"char"
.LASF57:
	.string	"print_string"
.LASF43:
	.string	"_next"
.LASF34:
	.string	"__pad1"
.LASF35:
	.string	"__pad2"
.LASF36:
	.string	"__pad3"
.LASF37:
	.string	"__pad4"
.LASF38:
	.string	"__pad5"
.LASF56:
	.string	"__len"
.LASF50:
	.string	"Vector"
.LASF64:
	.string	"vector_clear"
.LASF63:
	.string	"vector_peek"
.LASF51:
	.string	"__fmt"
.LASF65:
	.string	"vector_free"
.LASF0:
	.string	"long unsigned int"
.LASF18:
	.string	"_IO_write_end"
.LASF9:
	.string	"__off64_t"
.LASF8:
	.string	"__off_t"
.LASF25:
	.string	"_chain"
.LASF47:
	.string	"size"
.LASF22:
	.string	"_IO_backup_base"
.LASF68:
	.string	"stdin"
.LASF27:
	.string	"_flags2"
.LASF39:
	.string	"_mode"
.LASF15:
	.string	"_IO_read_base"
.LASF74:
	.string	"utils.c"
.LASF30:
	.string	"_vtable_offset"
.LASF70:
	.string	"__printf_chk"
.LASF21:
	.string	"_IO_save_base"
.LASF60:
	.string	"vector_add"
.LASF26:
	.string	"_fileno"
.LASF67:
	.string	"vector_set"
.LASF59:
	.string	"vector_ensure_capacity"
.LASF61:
	.string	"vector_set_length"
.LASF76:
	.string	"_IO_lock_t"
	.ident	"GCC: (Ubuntu 4.8.1-2ubuntu1~12.04) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
