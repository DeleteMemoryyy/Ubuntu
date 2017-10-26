#	Coursera换行显示有问题，请移步github查看具体代码，谢谢~
#	https://github.com/DeleteMemoryyy/Ubuntu/blob/DeleteMemory/MIPS/mips2.asm

	.data
buf:	.space 1000
success_string:	.asciiz	"Success! Location:"
fail_string:	.asciiz	"Fail!"

	.text
	.globl main
main:
	# read string
	li	$v0,8
	la	$a0,buf
	li	$a1,1000
	syscall

	li	$v0,11
	li	$a0,10
	syscall

query:
	# read character
	li	$v0,12
	syscall
	move	$s0,$v0

	li	$v0,11
	li	$a0,10
	syscall
	
	subi	$t0,$s0,63	# char == '?'
	beqz	$t0,exit

	la	$t0,buf
	li	$s1,1
	
L1:
	# start looping
	lb	$t1,($t0)
	beqz	$t1,fail
	
	sub	$t2,$t1,$s0
	beqz	$t2,success
	
	addi	$t0,$t0,1
	addi	$s1,$s1,1
	
	j	L1
	
success:
	li	$v0,4
	la	$a0,success_string
	syscall	
	
	li	$v0,1
	move	$a0,$s1
	syscall
	
	li	$v0,11
	li	$a0,10
	syscall
	
	j	query

fail:
	li	$v0,4
	la	$a0,fail_string
	syscall
	
	li	$v0,11
	li	$a0,10
	syscall

	j	query
	
exit: 
	li	$v0,10	# exit
	syscall
