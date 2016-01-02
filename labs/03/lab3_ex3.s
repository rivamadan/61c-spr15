	.file	1 "lab3_ex3.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.gnu_attribute 4, 1
	.text
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$6,%hi(source)
	lw	$3,%lo(source)($6)
	beq	$3,$0,$L2 #checks source[k]!=0
	lui	$7,%hi(dest) # $7 dest
	addiu	$7,$7,%lo(dest)
	addiu	$6,$6,%lo(source) # $6 source
	move	$2,$0 # k = 0
$L3:
	addu	$5,$7,$2 #$5 is $a1, $2 is k, $5 offsets pointer by $2 (points to next element of dest)
	addu	$4,$6,$2 #$4 is $a0, $2 is k, $5 offsets pointer by $2 (points to next element of source)
	sw	$3,0($5) # $3 is $v1, store $3 into dest[0]
	lw	$3,4($4) # $3 is $v1, load source[1] into $3
	addiu	$2,$2,4 #increment k
	bne	$3,$0,$L3 #the loop source[k]!=0
$L2:
	move	$2,$0 # $2 is $v0
	j	$31
	.end	main
	.size	main, .-main
	.globl	source
	.data
	.align	2
	.type	source, @object
	.size	source, 28
source:
	.word	3
	.word	1
	.word	4
	.word	1
	.word	5
	.word	9
	.word	0

	.comm	dest,40,4
	.ident	"GCC: (GNU) 4.4.1"
