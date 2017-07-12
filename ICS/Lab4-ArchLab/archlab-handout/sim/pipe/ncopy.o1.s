	.file	"ncopy.c"
	.text
.globl ncopy
	.type	ncopy, @function
ncopy:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	16(%ebp), %esi
	movl	$0, %ebx
	cmpl	$15, %esi
	jle	.L26
.L20:
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
	movzbl	%al, %eax
	addl	%eax, %ebx
	subl	$16, %esi
	cmpl	$15, %esi
	jg	.L20
.L26:
	testl	%esi, %esi
	jle	.L28
.L24:
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%eax, (%edx)
	addl	$4, %edx
	testl	%eax, %eax
	setg	%al
	movzbl	%al, %eax
	addl	%eax, %ebx
	decl	%esi
	testl	%esi, %esi
	jg	.L24
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
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	andl	$-16, %esp
	subl	$16, %esp
	movl	$0, %edx
	movl	$src, %ecx
.L33:
	leal	1(%edx), %eax
	movl	%eax, (%ecx,%edx,4)
	movl	%eax, %edx
	cmpl	$7, %eax
	jle	.L33
	movl	$8, 8(%esp)
	movl	$dst, 4(%esp)
	movl	$src, (%esp)
	call	ncopy
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	movl	$0, (%esp)
	call	exit
	.size	main, .-main
	.comm	src,32,32
	.comm	dst,32,32
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 (Ubuntu 3.4.6-6ubuntu5)"
