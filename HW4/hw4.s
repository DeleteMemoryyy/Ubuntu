	.file	"hw4.c"
	.text
.globl bubble_b
	.type	bubble_b, @function
bubble_b:
.LFB2:
	decq	%rsi
	testq	%rsi, %rsi
	jle	.L11
.L9:
	movl	$0, %ecx
	cmpq	%rsi, %rcx
	jge	.L13
.L8:
	movq	8(%rdi,%rcx,8), %rdx
	movq	(%rdi,%rcx,8), %rax
	movq	%rax, 8(%rdi,%rcx,8)
	movq	%rdx, (%rdi,%rcx,8)
	incq	%rcx
	cmpq	%rsi, %rcx
	jl	.L8
.L13:
	decq	%rsi
	testq	%rsi, %rsi
	jg	.L9
.L11:
	rep ; ret
.LFE2:
	.size	bubble_b, .-bubble_b
.globl main
	.type	main, @function
main:
.LFB3:
	movl	$0, %eax
	ret
.LFE3:
	.size	main, .-main
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.quad	.LFB2
	.quad	.LFE2-.LFB2
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.quad	.LFB3
	.quad	.LFE3-.LFB3
	.align 8
.LEFDE3:
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 (Ubuntu 3.4.6-6ubuntu5)"
