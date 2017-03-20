#	Coursera换行显示有问题，请移步github查看具体代码，谢谢~
#	

	.data 
uppercase_word_string: .asciiz
	"Alpha","Bravo","China","Delta","Echo","Foxtrot","Golf","Hotel","India","Juliet","Kilo","Lima","Mary","November","Oscar",
	"Paper","Quebec","Research","Siberra","Tango","Uniform","Victor","Whisky","X-ray","Yankee","Zulu"
lowercase_word_string: .asciiz
	"alpha","bravo","china","delta","echo","foxtrot","glof","hotel","india","juliet","kilo","lima","mary","november","oscar",
	"paper","quebec","research","siberra","tango","uniform","victor","whisky","x-ray","yankee","zulu"
	
word_offset: .word	# Strings in MIPS are end with two characters '\0'
	0,6,12,18,24,29,37,42,48,54,61,66,71,76,85,91,97,104,113,121,127,135,142,149,155,162

number_string: .asciiz
	"Zero","First","Second","Third","Fourth","Fifth","Sixth","Seventh","Eighth","Ninth"

number_offset: .word	# Strings in MIPS are end with two characters '\0'
	0,5,11,18,24,31,37,43,51,58	
	
others_string: .asciiz
	"?"

		
	.text
	.globl main
main:
	li	$v0,12	# read_char
	syscall
	
	move	$s2,$v0
	
	li	$v0,11
	li	$a0,10
	syscall
	
	subi	$s0,$s2,63	# input == '?'
	beqz	$s0,exit
	
	subi	$s0,$s2,48	# input < '0'
	bltz	$s0,others
	
	subi	$s0,$s2,57	# '0' <= input <= '9'
	blez	$s0,number
	
	subi	$s0,$s2,65	# '9' < input < 'A'
	bltz	$s0,others
	
	subi	$s0,$s2,90	# 'A' <= input <= 'Z'
	blez	$s0,uppercase_word
	
	subi	$s0,$s2,97	# 'Z' < input < 'a'
	bltz	$s0,others
	
	subi	$s0,$s2,122	# 'a' <= input < 'z'
	blez	$s0,lowercase_word

others:
	li	$v0,4
	la	$a0,others_string
	syscall
	li	$v0,11
	li	$a0,10
	syscall
	j	main
	
number:	
	la	$t0,number_offset
	subi	$s0,$s2,48
	sll	$s0,$s0,2
	add	$t0,$t0,$s0
	lw	$s1,($t0)
	la	$t1,number_string
	add	$a0,$s1,$t1
	li	$v0,4
	syscall
	li	$v0,11
	li	$a0,10
	syscall
	j main

uppercase_word:
	subi	$s0,$s2,65
	la 	$t1,uppercase_word_string
	j print
	
lowercase_word:
	subi	$s0,$s2,97
	la 	$t1,lowercase_word_string
	
print:
	la	$t0,word_offset
	sll	$s0,$s0,2
	add	$t0,$t0,$s0
	lw	$s1,($t0)
	add	$a0,$s1,$t1
	li	$v0,4
	syscall
	li	$v0,11
	li	$a0,10
	syscall
	j	main	

exit: 
	li	$v0,10	# exit
	syscall
	