	.file	"hw4.cpp"
	.text
	.align 2
	.p2align 4,,15
.globl _Z8bubble_bPll
	.type	_Z8bubble_bPll, @function
_Z8bubble_bPll:
.LFB2:
	.p2align 4,,7
.L19:
	decq	%rsi
	testq	%rsi, %rsi
	jle	.L12
	xorl	%eax, %eax
	.p2align 4,,7
.L20:
	cmpq	%rsi, %rax
	jge	.L19
	movq	(%rdi,%rax,8), %rcx
	movq	8(%rdi,%rax,8), %rdx
	cmpq	%rdx, %rcx
	jle	.L7
	movq	%rdx, (%rdi,%rax,8)
	movq	%rcx, 8(%rdi,%rax,8)
.L7:
	incq	%rax
	jmp	.L20
.L12:
	rep ; ret
.LFE2:
	.size	_Z8bubble_bPll, .-_Z8bubble_bPll
	.align 2
	.p2align 4,,15
.globl main
	.type	main, @function
main:
.LFB3:
	xorl	%eax, %eax
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
