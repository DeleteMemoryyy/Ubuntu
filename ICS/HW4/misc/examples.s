	.file	"examples.c"
	.text
.globl sum_list
	.type	sum_list, @function
sum_list:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	$0, %eax
	testl	%edx, %edx
	je	.L6
.L4:
	addl	(%edx), %eax
	movl	4(%edx), %edx
	testl	%edx, %edx
	jne	.L4
.L6:
	leave
	ret
	.size	sum_list, .-sum_list
.globl rsum_list
	.type	rsum_list, @function
rsum_list:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$4, %esp
	movl	8(%ebp), %edx
	movl	$0, %eax
	testl	%edx, %edx
	je	.L7
	movl	(%edx), %ebx
	movl	4(%edx), %eax
	movl	%eax, (%esp)
	call	rsum_list
	leal	(%ebx,%eax), %eax
.L7:
	addl	$4, %esp
	popl	%ebx
	leave
	ret
	.size	rsum_list, .-rsum_list
.globl copy_block
	.type	copy_block, @function
copy_block:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	movl	8(%ebp), %ebx
	movl	12(%ebp), %ecx
	movl	16(%ebp), %edx
	movl	$0, %esi
	testl	%edx, %edx
	jle	.L15
.L13:
	movl	(%ebx), %eax
	addl	$4, %ebx
	movl	%eax, (%ecx)
	addl	$4, %ecx
	xorl	%eax, %esi
	decl	%edx
	testl	%edx, %edx
	jg	.L13
.L15:
	movl	%esi, %eax
	popl	%ebx
	popl	%esi
	leave
	ret
	.size	copy_block, .-copy_block
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	andl	$-16, %esp
	subl	$16, %esp
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 (Ubuntu 3.4.6-6ubuntu5)"
