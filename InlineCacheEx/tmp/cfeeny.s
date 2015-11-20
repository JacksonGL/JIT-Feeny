	.file	"cfeeny.c"
	.text
.Ltext0:
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"Expected at least 1 argument to commandline.\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"interpret_time"
.LC2:
	.string	"w"
.LC3:
	.string	"interpret: %f\n"
.LC4:
	.string	"garbage_collector_time"
.LC5:
	.string	"gabage_collector time: %f\n"
.LC6:
	.string	"halloc_bytes"
.LC7:
	.string	"halloc bytes: %zu\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB42:
	.file 1 "cfeeny.c"
	.loc 1 11 0
	.cfi_startproc
.LVL0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	.loc 1 14 0
	leal	-2(%rdi), %eax
	.loc 1 11 0
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movl	%edi, %ebx
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	.loc 1 14 0
	cmpl	$1, %eax
	ja	.L8
	.loc 1 22 0
	movq	8(%rsi), %rdi
.LVL1:
	movq	%rsi, %rbp
.LVL2:
	call	read_ast
.LVL3:
	.loc 1 26 0
	movq	%rax, %rdi
	call	compile
.LVL4:
	.loc 1 28 0
	cmpl	$3, %ebx
	je	.L9
	.loc 1 33 0
	movq	%rax, %rdi
	call	interpret_bc
.LVL5:
.L5:
	.loc 1 47 0
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
.LVL6:
	ret
.LVL7:
.L9:
	.cfi_restore_state
	.loc 1 29 0
	movl	$.LC1, %edi
	movq	%rax, 8(%rsp)
	call	_start_timer
.LVL8:
	.loc 1 33 0
	movq	8(%rsp), %rax
	movq	%rax, %rdi
	call	interpret_bc
.LVL9:
.LBB11:
	.loc 1 35 0
	movq	16(%rbp), %rdi
	movl	$.LC2, %esi
	call	fopen
.LVL10:
	.loc 1 36 0
	movl	$.LC1, %edi
	.loc 1 35 0
	movq	%rax, %rbx
.LVL11:
	.loc 1 36 0
	call	_end_timer
.LVL12:
	.loc 1 37 0
	movl	$.LC1, %edi
	call	_get_double
.LVL13:
.LBB12:
.LBB13:
	.file 2 "/usr/include/x86_64-linux-gnu/bits/stdio2.h"
	.loc 2 98 0
	movl	$.LC3, %edx
	movl	$1, %esi
	movq	%rbx, %rdi
	movl	$1, %eax
	call	__fprintf_chk
.LVL14:
.LBE13:
.LBE12:
	.loc 1 40 0
	movl	$.LC4, %edi
	call	_get_double
.LVL15:
.LBB14:
.LBB15:
	.loc 2 98 0
	movl	$.LC5, %edx
	movl	$1, %esi
	movq	%rbx, %rdi
	movl	$1, %eax
	call	__fprintf_chk
.LVL16:
.LBE15:
.LBE14:
	.loc 1 42 0
	movl	$.LC6, %edi
	call	_get_int
.LVL17:
.LBB16:
.LBB17:
	.loc 2 98 0
	movq	%rbx, %rdi
	movq	%rax, %rcx
	movl	$.LC7, %edx
	movl	$1, %esi
	xorl	%eax, %eax
	call	__fprintf_chk
.LVL18:
.LBE17:
.LBE16:
	.loc 1 44 0
	movq	%rbx, %rdi
	call	fclose
.LVL19:
	jmp	.L5
.LVL20:
.L8:
.LBE11:
.LBB18:
.LBB19:
	.loc 2 105 0
	movl	$1, %edi
	movl	$.LC0, %esi
.LVL21:
	xorl	%eax, %eax
	call	__printf_chk
.LVL22:
.LBE19:
.LBE18:
	.loc 1 16 0
	orl	$-1, %edi
	call	exit
.LVL23:
	.cfi_endproc
.LFE42:
	.size	main, .-main
	.text
.Letext0:
	.file 3 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h"
	.file 4 "/usr/include/x86_64-linux-gnu/bits/types.h"
	.file 5 "/usr/include/stdio.h"
	.file 6 "/usr/include/libio.h"
	.file 7 "/usr/include/x86_64-linux-gnu/bits/time.h"
	.file 8 "utils.h"
	.file 9 "ast.h"
	.file 10 "bytecode.h"
	.file 11 "/usr/include/stdint.h"
	.file 12 "vm.h"
	.file 13 "compiler.h"
	.file 14 "/usr/include/stdlib.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x896
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF107
	.byte	0x1
	.long	.LASF108
	.long	.LASF109
	.long	.Ldebug_ranges0+0
	.quad	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF7
	.byte	0x3
	.byte	0xd4
	.long	0x34
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
	.byte	0x4
	.byte	0x8d
	.long	0x65
	.uleb128 0x2
	.long	.LASF9
	.byte	0x4
	.byte	0x8e
	.long	0x65
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF10
	.uleb128 0x2
	.long	.LASF11
	.byte	0x4
	.byte	0x95
	.long	0x65
	.uleb128 0x2
	.long	.LASF12
	.byte	0x4
	.byte	0x97
	.long	0x65
	.uleb128 0x5
	.byte	0x8
	.uleb128 0x6
	.byte	0x8
	.long	0xa7
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF13
	.uleb128 0x2
	.long	.LASF14
	.byte	0x5
	.byte	0x31
	.long	0xb9
	.uleb128 0x7
	.long	.LASF44
	.byte	0xd8
	.byte	0x6
	.value	0x111
	.long	0x240
	.uleb128 0x8
	.long	.LASF15
	.byte	0x6
	.value	0x112
	.long	0x5e
	.byte	0
	.uleb128 0x8
	.long	.LASF16
	.byte	0x6
	.value	0x117
	.long	0xa1
	.byte	0x8
	.uleb128 0x8
	.long	.LASF17
	.byte	0x6
	.value	0x118
	.long	0xa1
	.byte	0x10
	.uleb128 0x8
	.long	.LASF18
	.byte	0x6
	.value	0x119
	.long	0xa1
	.byte	0x18
	.uleb128 0x8
	.long	.LASF19
	.byte	0x6
	.value	0x11a
	.long	0xa1
	.byte	0x20
	.uleb128 0x8
	.long	.LASF20
	.byte	0x6
	.value	0x11b
	.long	0xa1
	.byte	0x28
	.uleb128 0x8
	.long	.LASF21
	.byte	0x6
	.value	0x11c
	.long	0xa1
	.byte	0x30
	.uleb128 0x8
	.long	.LASF22
	.byte	0x6
	.value	0x11d
	.long	0xa1
	.byte	0x38
	.uleb128 0x8
	.long	.LASF23
	.byte	0x6
	.value	0x11e
	.long	0xa1
	.byte	0x40
	.uleb128 0x8
	.long	.LASF24
	.byte	0x6
	.value	0x120
	.long	0xa1
	.byte	0x48
	.uleb128 0x8
	.long	.LASF25
	.byte	0x6
	.value	0x121
	.long	0xa1
	.byte	0x50
	.uleb128 0x8
	.long	.LASF26
	.byte	0x6
	.value	0x122
	.long	0xa1
	.byte	0x58
	.uleb128 0x8
	.long	.LASF27
	.byte	0x6
	.value	0x124
	.long	0x278
	.byte	0x60
	.uleb128 0x8
	.long	.LASF28
	.byte	0x6
	.value	0x126
	.long	0x27e
	.byte	0x68
	.uleb128 0x8
	.long	.LASF29
	.byte	0x6
	.value	0x128
	.long	0x5e
	.byte	0x70
	.uleb128 0x8
	.long	.LASF30
	.byte	0x6
	.value	0x12c
	.long	0x5e
	.byte	0x74
	.uleb128 0x8
	.long	.LASF31
	.byte	0x6
	.value	0x12e
	.long	0x6c
	.byte	0x78
	.uleb128 0x8
	.long	.LASF32
	.byte	0x6
	.value	0x132
	.long	0x42
	.byte	0x80
	.uleb128 0x8
	.long	.LASF33
	.byte	0x6
	.value	0x133
	.long	0x50
	.byte	0x82
	.uleb128 0x8
	.long	.LASF34
	.byte	0x6
	.value	0x134
	.long	0x284
	.byte	0x83
	.uleb128 0x8
	.long	.LASF35
	.byte	0x6
	.value	0x138
	.long	0x294
	.byte	0x88
	.uleb128 0x8
	.long	.LASF36
	.byte	0x6
	.value	0x141
	.long	0x77
	.byte	0x90
	.uleb128 0x8
	.long	.LASF37
	.byte	0x6
	.value	0x14a
	.long	0x9f
	.byte	0x98
	.uleb128 0x8
	.long	.LASF38
	.byte	0x6
	.value	0x14b
	.long	0x9f
	.byte	0xa0
	.uleb128 0x8
	.long	.LASF39
	.byte	0x6
	.value	0x14c
	.long	0x9f
	.byte	0xa8
	.uleb128 0x8
	.long	.LASF40
	.byte	0x6
	.value	0x14d
	.long	0x9f
	.byte	0xb0
	.uleb128 0x8
	.long	.LASF41
	.byte	0x6
	.value	0x14e
	.long	0x29
	.byte	0xb8
	.uleb128 0x8
	.long	.LASF42
	.byte	0x6
	.value	0x150
	.long	0x5e
	.byte	0xc0
	.uleb128 0x8
	.long	.LASF43
	.byte	0x6
	.value	0x152
	.long	0x29a
	.byte	0xc4
	.byte	0
	.uleb128 0x9
	.long	.LASF110
	.byte	0x6
	.byte	0xb6
	.uleb128 0xa
	.long	.LASF45
	.byte	0x18
	.byte	0x6
	.byte	0xbc
	.long	0x278
	.uleb128 0xb
	.long	.LASF46
	.byte	0x6
	.byte	0xbd
	.long	0x278
	.byte	0
	.uleb128 0xb
	.long	.LASF47
	.byte	0x6
	.byte	0xbe
	.long	0x27e
	.byte	0x8
	.uleb128 0xb
	.long	.LASF48
	.byte	0x6
	.byte	0xc2
	.long	0x5e
	.byte	0x10
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x247
	.uleb128 0x6
	.byte	0x8
	.long	0xb9
	.uleb128 0xc
	.long	0xa7
	.long	0x294
	.uleb128 0xd
	.long	0x82
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x240
	.uleb128 0xc
	.long	0xa7
	.long	0x2aa
	.uleb128 0xd
	.long	0x82
	.byte	0x13
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x2b0
	.uleb128 0xe
	.long	0xa7
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF49
	.uleb128 0xa
	.long	.LASF50
	.byte	0x10
	.byte	0x7
	.byte	0x1f
	.long	0x2e1
	.uleb128 0xb
	.long	.LASF51
	.byte	0x7
	.byte	0x21
	.long	0x89
	.byte	0
	.uleb128 0xb
	.long	.LASF52
	.byte	0x7
	.byte	0x22
	.long	0x94
	.byte	0x8
	.byte	0
	.uleb128 0xf
	.byte	0x10
	.byte	0x8
	.byte	0xc
	.long	0x30e
	.uleb128 0xb
	.long	.LASF53
	.byte	0x8
	.byte	0xd
	.long	0x5e
	.byte	0
	.uleb128 0xb
	.long	.LASF54
	.byte	0x8
	.byte	0xe
	.long	0x5e
	.byte	0x4
	.uleb128 0xb
	.long	.LASF55
	.byte	0x8
	.byte	0xf
	.long	0x30e
	.byte	0x8
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x9f
	.uleb128 0x2
	.long	.LASF56
	.byte	0x8
	.byte	0x10
	.long	0x2e1
	.uleb128 0x10
	.byte	0x4
	.byte	0x9
	.byte	0x4
	.long	0x38e
	.uleb128 0x11
	.long	.LASF57
	.sleb128 0
	.uleb128 0x11
	.long	.LASF58
	.sleb128 1
	.uleb128 0x11
	.long	.LASF59
	.sleb128 2
	.uleb128 0x11
	.long	.LASF60
	.sleb128 3
	.uleb128 0x11
	.long	.LASF61
	.sleb128 4
	.uleb128 0x11
	.long	.LASF62
	.sleb128 5
	.uleb128 0x11
	.long	.LASF63
	.sleb128 6
	.uleb128 0x11
	.long	.LASF64
	.sleb128 7
	.uleb128 0x11
	.long	.LASF65
	.sleb128 8
	.uleb128 0x11
	.long	.LASF66
	.sleb128 9
	.uleb128 0x11
	.long	.LASF67
	.sleb128 10
	.uleb128 0x11
	.long	.LASF68
	.sleb128 11
	.uleb128 0x11
	.long	.LASF69
	.sleb128 12
	.uleb128 0x11
	.long	.LASF70
	.sleb128 13
	.uleb128 0x11
	.long	.LASF71
	.sleb128 14
	.uleb128 0x11
	.long	.LASF72
	.sleb128 15
	.uleb128 0x11
	.long	.LASF73
	.sleb128 16
	.byte	0
	.uleb128 0x2
	.long	.LASF74
	.byte	0x9
	.byte	0x16
	.long	0x31f
	.uleb128 0xf
	.byte	0x4
	.byte	0x9
	.byte	0x20
	.long	0x3ae
	.uleb128 0x12
	.string	"tag"
	.byte	0x9
	.byte	0x21
	.long	0x38e
	.byte	0
	.byte	0
	.uleb128 0x2
	.long	.LASF75
	.byte	0x9
	.byte	0x22
	.long	0x399
	.uleb128 0x6
	.byte	0x8
	.long	0x3ae
	.uleb128 0x6
	.byte	0x8
	.long	0xa1
	.uleb128 0x6
	.byte	0x8
	.long	0x314
	.uleb128 0xf
	.byte	0x18
	.byte	0xa
	.byte	0x93
	.long	0x3f8
	.uleb128 0xb
	.long	.LASF76
	.byte	0xa
	.byte	0x94
	.long	0x3c5
	.byte	0
	.uleb128 0xb
	.long	.LASF77
	.byte	0xa
	.byte	0x95
	.long	0x3c5
	.byte	0x8
	.uleb128 0xb
	.long	.LASF78
	.byte	0xa
	.byte	0x96
	.long	0x5e
	.byte	0x10
	.byte	0
	.uleb128 0x2
	.long	.LASF79
	.byte	0xa
	.byte	0x97
	.long	0x3cb
	.uleb128 0x2
	.long	.LASF80
	.byte	0xb
	.byte	0x78
	.long	0x65
	.uleb128 0x13
	.long	.LASF81
	.byte	0x2
	.byte	0x67
	.long	0x5e
	.byte	0x3
	.long	0x42b
	.uleb128 0x14
	.long	.LASF83
	.byte	0x2
	.byte	0x67
	.long	0x2aa
	.uleb128 0x15
	.byte	0
	.uleb128 0x13
	.long	.LASF82
	.byte	0x2
	.byte	0x60
	.long	0x5e
	.byte	0x3
	.long	0x453
	.uleb128 0x14
	.long	.LASF84
	.byte	0x2
	.byte	0x60
	.long	0x453
	.uleb128 0x14
	.long	.LASF83
	.byte	0x2
	.byte	0x60
	.long	0x2aa
	.uleb128 0x15
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xae
	.uleb128 0x16
	.long	.LASF111
	.byte	0x1
	.byte	0xb
	.long	0x5e
	.quad	.LFB42
	.quad	.LFE42-.LFB42
	.uleb128 0x1
	.byte	0x9c
	.long	0x776
	.uleb128 0x17
	.long	.LASF85
	.byte	0x1
	.byte	0xb
	.long	0x5e
	.long	.LLST0
	.uleb128 0x17
	.long	.LASF86
	.byte	0x1
	.byte	0xb
	.long	0x3bf
	.long	.LLST1
	.uleb128 0x18
	.long	.LASF87
	.byte	0x1
	.byte	0xc
	.long	0x2bc
	.uleb128 0x18
	.long	.LASF88
	.byte	0x1
	.byte	0xc
	.long	0x2bc
	.uleb128 0x19
	.string	"end"
	.byte	0x1
	.byte	0xc
	.long	0x2bc
	.uleb128 0x1a
	.long	.LASF89
	.byte	0x1
	.byte	0x15
	.long	0xa1
	.long	.LLST2
	.uleb128 0x1a
	.long	.LASF90
	.byte	0x1
	.byte	0x16
	.long	0x3b9
	.long	.LLST3
	.uleb128 0x1a
	.long	.LASF91
	.byte	0x1
	.byte	0x1a
	.long	0x776
	.long	.LLST4
	.uleb128 0x1b
	.quad	.LBB11
	.quad	.LBE11-.LBB11
	.long	0x6b6
	.uleb128 0x1a
	.long	.LASF92
	.byte	0x1
	.byte	0x23
	.long	0x453
	.long	.LLST5
	.uleb128 0x1c
	.long	0x42b
	.quad	.LBB12
	.quad	.LBE12-.LBB12
	.byte	0x1
	.byte	0x25
	.long	0x55e
	.uleb128 0x1d
	.long	0x446
	.long	.LLST6
	.uleb128 0x1d
	.long	0x43b
	.long	.LLST7
	.uleb128 0x1e
	.quad	.LVL14
	.long	0x792
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x9
	.byte	0x3
	.quad	.LC3
	.byte	0
	.byte	0
	.uleb128 0x1c
	.long	0x42b
	.quad	.LBB14
	.quad	.LBE14-.LBB14
	.byte	0x1
	.byte	0x28
	.long	0x5b2
	.uleb128 0x1d
	.long	0x446
	.long	.LLST8
	.uleb128 0x1d
	.long	0x43b
	.long	.LLST9
	.uleb128 0x1e
	.quad	.LVL16
	.long	0x792
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x9
	.byte	0x3
	.quad	.LC5
	.byte	0
	.byte	0
	.uleb128 0x1c
	.long	0x42b
	.quad	.LBB16
	.quad	.LBE16-.LBB16
	.byte	0x1
	.byte	0x2a
	.long	0x606
	.uleb128 0x1d
	.long	0x446
	.long	.LLST10
	.uleb128 0x1d
	.long	0x43b
	.long	.LLST11
	.uleb128 0x1e
	.quad	.LVL18
	.long	0x792
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x9
	.byte	0x3
	.quad	.LC7
	.byte	0
	.byte	0
	.uleb128 0x20
	.quad	.LVL10
	.long	0x7b2
	.long	0x625
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC2
	.byte	0
	.uleb128 0x20
	.quad	.LVL12
	.long	0x7cd
	.long	0x644
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.byte	0
	.uleb128 0x20
	.quad	.LVL13
	.long	0x7de
	.long	0x663
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.byte	0
	.uleb128 0x20
	.quad	.LVL15
	.long	0x7de
	.long	0x682
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC4
	.byte	0
	.uleb128 0x20
	.quad	.LVL17
	.long	0x7fa
	.long	0x6a1
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC6
	.byte	0
	.uleb128 0x1e
	.quad	.LVL19
	.long	0x80f
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x1c
	.long	0x40e
	.quad	.LBB18
	.quad	.LBE18-.LBB18
	.byte	0x1
	.byte	0xf
	.long	0x702
	.uleb128 0x21
	.long	0x41e
	.uleb128 0xa
	.byte	0x3
	.quad	.LC0
	.byte	0x9f
	.uleb128 0x1e
	.quad	.LVL22
	.long	0x824
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC0
	.byte	0
	.byte	0
	.uleb128 0x22
	.quad	.LVL3
	.long	0x83f
	.uleb128 0x22
	.quad	.LVL4
	.long	0x854
	.uleb128 0x22
	.quad	.LVL5
	.long	0x869
	.uleb128 0x20
	.quad	.LVL8
	.long	0x87a
	.long	0x748
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.byte	0
	.uleb128 0x20
	.quad	.LVL9
	.long	0x869
	.long	0x761
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x3
	.byte	0x91
	.sleb128 -40
	.byte	0x6
	.byte	0
	.uleb128 0x1e
	.quad	.LVL23
	.long	0x88b
	.uleb128 0x1f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x3f8
	.uleb128 0x23
	.long	.LASF93
	.byte	0x5
	.byte	0xa9
	.long	0x27e
	.uleb128 0x23
	.long	.LASF94
	.byte	0x5
	.byte	0xaa
	.long	0x27e
	.uleb128 0x24
	.long	.LASF95
	.byte	0x2
	.byte	0x56
	.long	0x5e
	.long	0x7b2
	.uleb128 0x25
	.long	0x453
	.uleb128 0x25
	.long	0x5e
	.uleb128 0x25
	.long	0x2aa
	.uleb128 0x15
	.byte	0
	.uleb128 0x26
	.long	.LASF96
	.byte	0x5
	.value	0x111
	.long	0x453
	.long	0x7cd
	.uleb128 0x25
	.long	0x2aa
	.uleb128 0x25
	.long	0x2aa
	.byte	0
	.uleb128 0x27
	.long	.LASF104
	.byte	0xc
	.byte	0x22
	.long	0x7de
	.uleb128 0x25
	.long	0x2aa
	.byte	0
	.uleb128 0x24
	.long	.LASF97
	.byte	0xc
	.byte	0x25
	.long	0x7f3
	.long	0x7f3
	.uleb128 0x25
	.long	0x2aa
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.byte	0x4
	.long	.LASF98
	.uleb128 0x24
	.long	.LASF99
	.byte	0xc
	.byte	0x28
	.long	0x403
	.long	0x80f
	.uleb128 0x25
	.long	0x2aa
	.byte	0
	.uleb128 0x24
	.long	.LASF100
	.byte	0x5
	.byte	0xee
	.long	0x5e
	.long	0x824
	.uleb128 0x25
	.long	0x453
	.byte	0
	.uleb128 0x24
	.long	.LASF101
	.byte	0x2
	.byte	0x58
	.long	0x5e
	.long	0x83f
	.uleb128 0x25
	.long	0x5e
	.uleb128 0x25
	.long	0x2aa
	.uleb128 0x15
	.byte	0
	.uleb128 0x24
	.long	.LASF102
	.byte	0x9
	.byte	0xa4
	.long	0x3b9
	.long	0x854
	.uleb128 0x25
	.long	0xa1
	.byte	0
	.uleb128 0x24
	.long	.LASF103
	.byte	0xd
	.byte	0x6
	.long	0x776
	.long	0x869
	.uleb128 0x25
	.long	0x3b9
	.byte	0
	.uleb128 0x27
	.long	.LASF105
	.byte	0xc
	.byte	0x2a
	.long	0x87a
	.uleb128 0x25
	.long	0x776
	.byte	0
	.uleb128 0x27
	.long	.LASF106
	.byte	0xc
	.byte	0x21
	.long	0x88b
	.uleb128 0x25
	.long	0x2aa
	.byte	0
	.uleb128 0x28
	.long	.LASF112
	.byte	0xe
	.value	0x220
	.uleb128 0x25
	.long	0x5e
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
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
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
	.uleb128 0x10
	.uleb128 0x4
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
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x13
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
	.uleb128 0x14
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
	.uleb128 0x15
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x16
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
	.uleb128 0x17
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
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x18
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
	.byte	0
	.byte	0
	.uleb128 0x19
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
	.uleb128 0x1a
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
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1c
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
	.uleb128 0x1d
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x20
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
	.uleb128 0x21
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x23
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
	.uleb128 0x24
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
	.uleb128 0x25
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x26
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
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x28
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
	.quad	.LVL0
	.quad	.LVL1
	.value	0x1
	.byte	0x55
	.quad	.LVL1
	.quad	.LVL5
	.value	0x1
	.byte	0x53
	.quad	.LVL5
	.quad	.LVL7
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL7
	.quad	.LVL11
	.value	0x1
	.byte	0x53
	.quad	.LVL11
	.quad	.LVL20
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL20
	.quad	.LFE42
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST1:
	.quad	.LVL0
	.quad	.LVL3-1
	.value	0x1
	.byte	0x54
	.quad	.LVL3-1
	.quad	.LVL6
	.value	0x1
	.byte	0x56
	.quad	.LVL6
	.quad	.LVL7
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL7
	.quad	.LVL20
	.value	0x1
	.byte	0x56
	.quad	.LVL20
	.quad	.LVL21
	.value	0x1
	.byte	0x54
	.quad	.LVL21
	.quad	.LFE42
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST2:
	.quad	.LVL2
	.quad	.LVL3-1
	.value	0x1
	.byte	0x55
	.quad	0
	.quad	0
.LLST3:
	.quad	.LVL3
	.quad	.LVL4-1
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST4:
	.quad	.LVL4
	.quad	.LVL5-1
	.value	0x1
	.byte	0x50
	.quad	.LVL7
	.quad	.LVL8-1
	.value	0x1
	.byte	0x50
	.quad	.LVL8-1
	.quad	.LVL20
	.value	0x2
	.byte	0x91
	.sleb128 -40
	.quad	0
	.quad	0
.LLST5:
	.quad	.LVL11
	.quad	.LVL12-1
	.value	0x1
	.byte	0x50
	.quad	.LVL12-1
	.quad	.LVL20
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST6:
	.quad	.LVL13
	.quad	.LVL20
	.value	0xa
	.byte	0x3
	.quad	.LC3
	.byte	0x9f
	.quad	0
	.quad	0
.LLST7:
	.quad	.LVL13
	.quad	.LVL20
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST8:
	.quad	.LVL15
	.quad	.LVL20
	.value	0xa
	.byte	0x3
	.quad	.LC5
	.byte	0x9f
	.quad	0
	.quad	0
.LLST9:
	.quad	.LVL15
	.quad	.LVL20
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST10:
	.quad	.LVL17
	.quad	.LVL20
	.value	0xa
	.byte	0x3
	.quad	.LC7
	.byte	0x9f
	.quad	0
	.quad	0
.LLST11:
	.quad	.LVL17
	.quad	.LVL20
	.value	0x1
	.byte	0x53
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
	.quad	.LFB42
	.quad	.LFE42-.LFB42
	.quad	0
	.quad	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.quad	.LFB42
	.quad	.LFE42
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF81:
	.string	"printf"
.LASF8:
	.string	"__off_t"
.LASF54:
	.string	"capacity"
.LASF16:
	.string	"_IO_read_ptr"
.LASF28:
	.string	"_chain"
.LASF7:
	.string	"size_t"
.LASF34:
	.string	"_shortbuf"
.LASF22:
	.string	"_IO_buf_base"
.LASF78:
	.string	"entry"
.LASF62:
	.string	"SLOT_EXP"
.LASF59:
	.string	"PRINTF_EXP"
.LASF63:
	.string	"SET_SLOT_EXP"
.LASF104:
	.string	"_end_timer"
.LASF92:
	.string	"stat"
.LASF49:
	.string	"long long int"
.LASF4:
	.string	"signed char"
.LASF56:
	.string	"Vector"
.LASF29:
	.string	"_fileno"
.LASF17:
	.string	"_IO_read_end"
.LASF6:
	.string	"long int"
.LASF15:
	.string	"_flags"
.LASF109:
	.string	"/scratch/gongliang13/cs294-113/CS294-113/JITEx/tmp"
.LASF23:
	.string	"_IO_buf_end"
.LASF32:
	.string	"_cur_column"
.LASF64:
	.string	"CALL_SLOT_EXP"
.LASF99:
	.string	"_get_int"
.LASF98:
	.string	"double"
.LASF101:
	.string	"__printf_chk"
.LASF31:
	.string	"_old_offset"
.LASF36:
	.string	"_offset"
.LASF55:
	.string	"array"
.LASF57:
	.string	"INT_EXP"
.LASF68:
	.string	"WHILE_EXP"
.LASF106:
	.string	"_start_timer"
.LASF103:
	.string	"compile"
.LASF45:
	.string	"_IO_marker"
.LASF93:
	.string	"stdin"
.LASF3:
	.string	"unsigned int"
.LASF82:
	.string	"fprintf"
.LASF84:
	.string	"__stream"
.LASF0:
	.string	"long unsigned int"
.LASF74:
	.string	"AstTag"
.LASF12:
	.string	"__suseconds_t"
.LASF20:
	.string	"_IO_write_ptr"
.LASF50:
	.string	"timeval"
.LASF47:
	.string	"_sbuf"
.LASF53:
	.string	"size"
.LASF2:
	.string	"short unsigned int"
.LASF75:
	.string	"ScopeStmt"
.LASF76:
	.string	"values"
.LASF73:
	.string	"EXP_STMT"
.LASF24:
	.string	"_IO_save_base"
.LASF79:
	.string	"Program"
.LASF80:
	.string	"intptr_t"
.LASF35:
	.string	"_lock"
.LASF30:
	.string	"_flags2"
.LASF42:
	.string	"_mode"
.LASF94:
	.string	"stdout"
.LASF108:
	.string	"cfeeny.c"
.LASF107:
	.string	"GNU C 4.8.1 -mtune=generic -march=x86-64 -g -O3 -std=c99 -fstack-protector"
.LASF10:
	.string	"sizetype"
.LASF89:
	.string	"filename"
.LASF51:
	.string	"tv_sec"
.LASF21:
	.string	"_IO_write_end"
.LASF58:
	.string	"NULL_EXP"
.LASF60:
	.string	"ARRAY_EXP"
.LASF110:
	.string	"_IO_lock_t"
.LASF44:
	.string	"_IO_FILE"
.LASF77:
	.string	"slots"
.LASF72:
	.string	"SEQ_STMT"
.LASF96:
	.string	"fopen"
.LASF48:
	.string	"_pos"
.LASF27:
	.string	"_markers"
.LASF105:
	.string	"interpret_bc"
.LASF69:
	.string	"REF_EXP"
.LASF1:
	.string	"unsigned char"
.LASF97:
	.string	"_get_double"
.LASF5:
	.string	"short int"
.LASF91:
	.string	"program"
.LASF61:
	.string	"OBJECT_EXP"
.LASF33:
	.string	"_vtable_offset"
.LASF87:
	.string	"total_start"
.LASF14:
	.string	"FILE"
.LASF66:
	.string	"SET_EXP"
.LASF112:
	.string	"exit"
.LASF95:
	.string	"__fprintf_chk"
.LASF90:
	.string	"stmt"
.LASF102:
	.string	"read_ast"
.LASF52:
	.string	"tv_usec"
.LASF67:
	.string	"IF_EXP"
.LASF70:
	.string	"VAR_STMT"
.LASF13:
	.string	"char"
.LASF71:
	.string	"FN_STMT"
.LASF100:
	.string	"fclose"
.LASF46:
	.string	"_next"
.LASF9:
	.string	"__off64_t"
.LASF18:
	.string	"_IO_read_base"
.LASF26:
	.string	"_IO_save_end"
.LASF83:
	.string	"__fmt"
.LASF37:
	.string	"__pad1"
.LASF38:
	.string	"__pad2"
.LASF39:
	.string	"__pad3"
.LASF40:
	.string	"__pad4"
.LASF41:
	.string	"__pad5"
.LASF11:
	.string	"__time_t"
.LASF43:
	.string	"_unused2"
.LASF25:
	.string	"_IO_backup_base"
.LASF85:
	.string	"argc"
.LASF65:
	.string	"CALL_EXP"
.LASF88:
	.string	"start"
.LASF111:
	.string	"main"
.LASF19:
	.string	"_IO_write_base"
.LASF86:
	.string	"argvs"
	.ident	"GCC: (Ubuntu 4.8.1-2ubuntu1~12.04) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
