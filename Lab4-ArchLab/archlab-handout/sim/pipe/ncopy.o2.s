	.file	"ncopy.c"
	.text
	.p2align 4,,15
.globl ncopy
	.type	ncopy, @function
ncopy:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	16(%ebp), %esi
	pushl	%ebx
	xorl	%ebx, %ebx
	jmp	.L29
	.p2align 4,,7
.L32:
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	subl	$16, %esi
	movzbl	%al, %eax
	addl	%eax, %ebx
.L29:
	cmpl	$15, %esi
	jg	.L32
	testl	%esi, %esi
	jle	.L28
	.p2align 4,,7
.L33:
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	decl	%esi
	movzbl	%al, %eax
	addl	%eax, %ebx
	testl	%esi, %esi
	jg	.L33
.L28:
	movl	%ebx, %eax
	popl	%ebx
	popl	%esi
	leave
	ret
	.size	ncopy, .-ncopy
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"count=%d\n"
	.text
	.p2align 4,,15
.globl main
	.type	main, @function
main:
	pushl	%ebp
	xorl	%edx, %edx
	movl	%esp, %ebp
	subl	$24, %esp
	andl	$-16, %esp
	subl	$16, %esp
	.p2align 4,,7
.L38:
	leal	1(%edx), %eax
	cmpl	$7, %eax
	movl	%eax, src(,%edx,4)
	movl	%eax, %edx
	jle	.L38
	movl	$8, 8(%esp)
	movl	$dst, 4(%esp)
	movl	$src, (%esp)
	call	ncopy
	movl	$.LC0, (%esp)
	movl	%eax, 4(%esp)
	call	printf
	movl	$0, (%esp)
	call	exit
	.size	main, .-main
	.comm	src,32,32
	.comm	dst,32,32
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 (Ubuntu 3.4.6-6ubuntu5)"
