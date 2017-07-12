	.file	"csim.c"
	.globl	bMask
	.bss
	.align 8
	.type	bMask, @object
	.size	bMask, 8
bMask:
	.zero	8
	.globl	sMask
	.align 8
	.type	sMask, @object
	.size	sMask, 8
sMask:
	.zero	8
	.globl	tMask
	.align 8
	.type	tMask, @object
	.size	tMask, 8
tMask:
	.zero	8
	.globl	hit_count
	.align 4
	.type	hit_count, @object
	.size	hit_count, 4
hit_count:
	.zero	4
	.globl	miss_count
	.align 4
	.type	miss_count, @object
	.size	miss_count, 4
miss_count:
	.zero	4
	.globl	eviction_count
	.align 4
	.type	eviction_count, @object
	.size	eviction_count, 4
eviction_count:
	.zero	4
	.globl	opt
	.align 4
	.type	opt, @object
	.size	opt, 4
opt:
	.zero	4
	.globl	s
	.align 4
	.type	s, @object
	.size	s, 4
s:
	.zero	4
	.globl	sNumber
	.align 4
	.type	sNumber, @object
	.size	sNumber, 4
sNumber:
	.zero	4
	.globl	E
	.align 4
	.type	E, @object
	.size	E, 4
E:
	.zero	4
	.globl	b
	.align 4
	.type	b, @object
	.size	b, 4
b:
	.zero	4
	.globl	bNumber
	.align 4
	.type	bNumber, @object
	.size	bNumber, 4
bNumber:
	.zero	4
	.comm	fileName,8,8
	.text
	.globl	initialize
	.type	initialize, @function
initialize:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$4, -8(%rbp)
	movl	$8, -4(%rbp)
	movl	E(%rip), %eax
	imull	-8(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	malloc
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movl	E(%rip), %eax
	imull	-8(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	malloc
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 8(%rax)
	movl	E(%rip), %eax
	imull	-4(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	malloc
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 16(%rax)
	movl	E(%rip), %eax
	imull	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset
	movl	E(%rip), %eax
	imull	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset
	movl	E(%rip), %eax
	imull	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	initialize, .-initialize
	.globl	destruct
	.type	destruct, @function
destruct:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	je	.L3
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	free
.L3:
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	je	.L4
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	free
.L4:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	testq	%rax, %rax
	je	.L6
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, %rdi
	call	free
.L6:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	destruct, .-destruct
	.globl	exist
	.type	exist, @function
exist:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -36(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L8
.L11:
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	jne	.L9
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	cmpq	-32(%rbp), %rax
	jne	.L9
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rax, %rdx
	movl	-36(%rbp), %eax
	movl	%eax, (%rdx)
	movl	$1, %eax
	jmp	.L10
.L9:
	addl	$1, -4(%rbp)
.L8:
	movl	E(%rip), %eax
	cmpl	%eax, -4(%rbp)
	jl	.L11
	movl	$0, %eax
.L10:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	exist, .-exist
	.globl	replace
	.type	replace, @function
replace:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -36(%rbp)
	movl	$32768, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L13
.L17:
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rdx, %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L14
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rdx, %rax
	movl	$1, (%rax)
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movq	-32(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rax, %rdx
	movl	-36(%rbp), %eax
	movl	%eax, (%rdx)
	movl	$0, %eax
	jmp	.L15
.L14:
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	-12(%rbp), %eax
	jge	.L16
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)
.L16:
	addl	$1, -4(%rbp)
.L13:
	movl	E(%rip), %eax
	cmpl	%eax, -4(%rbp)
	jl	.L17
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movl	-8(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movq	-32(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-8(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rax, %rdx
	movl	-36(%rbp), %eax
	movl	%eax, (%rdx)
	movl	$1, %eax
.L15:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	replace, .-replace
	.section	.rodata
.LC0:
	.string	"s:E:b:t:"
.LC1:
	.string	"r"
.LC2:
	.string	"%c %llx,%*d"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, opterr(%rip)
	jmp	.L19
.L26:
	movl	opt(%rip), %eax
	cmpl	$98, %eax
	je	.L21
	cmpl	$98, %eax
	jg	.L22
	cmpl	$69, %eax
	je	.L23
	jmp	.L20
.L22:
	cmpl	$115, %eax
	je	.L24
	cmpl	$116, %eax
	je	.L25
	jmp	.L20
.L24:
	movq	optarg(%rip), %rax
	movq	%rax, %rdi
	call	atoi
	movl	%eax, s(%rip)
	movl	s(%rip), %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	movl	%eax, sNumber(%rip)
	jmp	.L19
.L23:
	movq	optarg(%rip), %rax
	movq	%rax, %rdi
	call	atoi
	movl	%eax, E(%rip)
	jmp	.L19
.L21:
	movq	optarg(%rip), %rax
	movq	%rax, %rdi
	call	atoi
	movl	%eax, b(%rip)
	movl	b(%rip), %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	movl	%eax, bNumber(%rip)
.L25:
	movq	optarg(%rip), %rax
	movq	%rax, fileName(%rip)
	jmp	.L19
.L20:
	nop
.L19:
	movq	-80(%rbp), %rcx
	movl	-68(%rbp), %eax
	movl	$.LC0, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	getopt
	movl	%eax, opt(%rip)
	movl	opt(%rip), %eax
	cmpl	$-1, %eax
	jne	.L26
	movl	s(%rip), %eax
	testl	%eax, %eax
	js	.L27
	movl	E(%rip), %eax
	testl	%eax, %eax
	js	.L27
	movl	b(%rip), %eax
	testl	%eax, %eax
	js	.L27
	movq	fileName(%rip), %rax
	testq	%rax, %rax
	jne	.L28
.L27:
	movl	$0, %eax
	jmp	.L44
.L28:
	movl	b(%rip), %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	subl	$1, %eax
	movl	%eax, %eax
	movq	%rax, bMask(%rip)
	movl	s(%rip), %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	leal	-1(%rax), %edx
	movl	b(%rip), %eax
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	movl	%eax, %eax
	movq	%rax, sMask(%rip)
	movq	bMask(%rip), %rax
	notq	%rax
	movq	%rax, %rdx
	movq	sMask(%rip), %rax
	subq	%rax, %rdx
	movq	%rdx, %rax
	movq	%rax, tMask(%rip)
	movq	stdin(%rip), %rdx
	movq	fileName(%rip), %rax
	movl	$.LC1, %esi
	movq	%rax, %rdi
	call	freopen
	movl	sNumber(%rip), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc
	movq	%rax, -24(%rbp)
	movl	$0, -52(%rbp)
	jmp	.L30
.L31:
	movl	-52(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	initialize
	addl	$1, -52(%rbp)
.L30:
	movl	sNumber(%rip), %eax
	cmpl	%eax, -52(%rbp)
	jl	.L31
	movl	$0, -48(%rbp)
	jmp	.L32
.L41:
	addl	$1, -48(%rbp)
	cmpb	$32, -53(%rbp)
	jne	.L38
	leaq	-32(%rbp), %rdx
	leaq	-54(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	__isoc99_scanf
	movq	-32(%rbp), %rdx
	movq	tMask(%rip), %rax
	andq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movq	-32(%rbp), %rdx
	movq	sMask(%rip), %rax
	andq	%rax, %rdx
	movl	b(%rip), %eax
	movl	%eax, %ecx
	shrq	%cl, %rdx
	movq	%rdx, %rax
	movl	%eax, -40(%rbp)
	movl	-40(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movl	-48(%rbp), %edx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	exist
	movl	%eax, -36(%rbp)
	cmpl	$1, -36(%rbp)
	jne	.L34
	movl	hit_count(%rip), %eax
	addl	$1, %eax
	movl	%eax, hit_count(%rip)
	jmp	.L35
.L34:
	movl	miss_count(%rip), %eax
	addl	$1, %eax
	movl	%eax, miss_count(%rip)
	movl	-40(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movl	-48(%rbp), %edx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	replace
	cmpl	$1, %eax
	jne	.L35
	movl	eviction_count(%rip), %eax
	addl	$1, %eax
	movl	%eax, eviction_count(%rip)
.L35:
	movzbl	-54(%rbp), %eax
	cmpb	$77, %al
	jne	.L36
	movl	hit_count(%rip), %eax
	addl	$1, %eax
	movl	%eax, hit_count(%rip)
.L36:
	call	getchar
	cmpl	$-1, %eax
	jne	.L32
	jmp	.L37
.L40:
	cmpb	$10, -53(%rbp)
	je	.L46
.L38:
	call	getchar
	movb	%al, -53(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L40
	jmp	.L39
.L46:
	nop
.L39:
	cmpb	$-1, -53(%rbp)
	je	.L47
.L32:
	call	getchar
	movb	%al, -53(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L41
	jmp	.L37
.L47:
	nop
.L37:
	movl	$0, -44(%rbp)
	jmp	.L42
.L43:
	movl	-44(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	destruct
	addl	$1, -44(%rbp)
.L42:
	movl	sNumber(%rip), %eax
	cmpl	%eax, -44(%rbp)
	jl	.L43
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movl	eviction_count(%rip), %edx
	movl	miss_count(%rip), %ecx
	movl	hit_count(%rip), %eax
	movl	%ecx, %esi
	movl	%eax, %edi
	call	printSummary
	movl	$0, %eax
.L44:
	movq	-8(%rbp), %rsi
	xorq	%fs:40, %rsi
	je	.L45
	call	__stack_chk_fail
.L45:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.2) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
