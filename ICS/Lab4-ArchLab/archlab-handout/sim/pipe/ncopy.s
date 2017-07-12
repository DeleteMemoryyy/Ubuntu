	.file	"ncopy.c"
	.text
	.p2align 4,,15
.globl ncopy
	.type	ncopy, @function
ncopy:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	xorl	%edi, %edi
	movl	12(%ebp), %ecx
	pushl	%esi
	movl	16(%ebp), %esi
	pushl	%ebx
	movl	8(%ebp), %ebx
	jmp	.L34
	.p2align 4,,7
.L37:
	movl	(%ebx), %eax
	xorl	%edx, %edx
	testl	%eax, %eax
	movl	%eax, (%ecx)
	setg	%dl
	leal	(%edi,%edx), %edx
	movl	4(%ebx), %edi
	movl	%edi, 4(%ecx)
	testl	%edi, %edi
	movl	8(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	movl	%edi, 8(%ecx)
	addl	%eax, %edx
	testl	%edi, %edi
	movl	12(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 12(%ecx)
	testl	%edi, %edi
	movl	16(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	movl	%edi, 16(%ecx)
	addl	%eax, %edx
	testl	%edi, %edi
	movl	20(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 20(%ecx)
	testl	%edi, %edi
	movl	24(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	movl	%edi, 24(%ecx)
	addl	%eax, %edx
	testl	%edi, %edi
	movl	28(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 28(%ecx)
	testl	%edi, %edi
	movl	32(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	movl	%edi, 32(%ecx)
	addl	%eax, %edx
	testl	%edi, %edi
	movl	36(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 36(%ecx)
	testl	%edi, %edi
	movl	40(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	movl	%edi, 40(%ecx)
	addl	%eax, %edx
	testl	%edi, %edi
	movl	44(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 44(%ecx)
	testl	%edi, %edi
	movl	48(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	movl	%edi, 48(%ecx)
	addl	%eax, %edx
	testl	%edi, %edi
	movl	52(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 52(%ecx)
	testl	%edi, %edi
	movl	56(%ebx), %edi
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 56(%ecx)
	testl	%edi, %edi
	movl	60(%ebx), %edi
	setg	%al
	addl	$64, %ebx
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	%edi, 60(%ecx)
	addl	$64, %ecx
	testl	%edi, %edi
	setg	%al
	subl	$16, %esi
	movzbl	%al, %eax
	leal	(%edx,%eax), %edi
.L34:
	cmpl	$15, %esi
	jg	.L37
	testl	%esi, %esi
	jle	.L28
	.p2align 4,,7
.L38:
	movl	(%ebx), %edx
	addl	$4, %ebx
	movl	%edx, (%ecx)
	addl	$4, %ecx
	testl	%edx, %edx
	setg	%al
	decl	%esi
	movzbl	%al, %edx
	addl	%edx, %edi
	testl	%esi, %esi
	jg	.L38
.L28:
	popl	%ebx
	popl	%esi
	movl	%edi, %eax
	popl	%edi
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
.L43:
	leal	1(%edx), %eax
	cmpl	$7, %eax
	movl	%eax, src(,%edx,4)
	movl	%eax, %edx
	jle	.L43
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
