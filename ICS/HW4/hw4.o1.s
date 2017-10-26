	.file	"hw4.cpp"
	.text
	.align 2
.globl _Z8bubble_bPll
	.type	_Z8bubble_bPll, @function
_Z8bubble_bPll:
.LFB2:
	decq	%rsi
	testq	%rsi, %rsi
	jle	.L12
.L10:
	movl	$0, %edx
	cmpq	%rsi, %rdx
	jge	.L14
.L9:
	movq	(%rdi,%rdx,8), %rcx
	cmpq	8(%rdi,%rdx,8), %rcx
	jle	.L7
	movq	8(%rdi,%rdx,8), %rax
	movq	%rax, (%rdi,%rdx,8)
	movq	%rcx, 8(%rdi,%rdx,8)
.L7:
	incq	%rdx
	cmpq	%rsi, %rdx
	jl	.L9
.L14:
	decq	%rsi
	testq	%rsi, %rsi
	jg	.L10
.L12:
	rep ; ret
.LFE2:
	.size	_Z8bubble_bPll, .-_Z8bubble_bPll
	.align 2
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
	.string	"zP"
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.uleb128 0x9
	.byte	0x0
	.quad	__gxx_personality_v0
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
	.uleb128 0x0
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.quad	.LFB3
	.quad	.LFE3-.LFB3
	.uleb128 0x0
	.align 8
.LEFDE3:
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 (Ubuntu 3.4.6-6ubuntu5)"
