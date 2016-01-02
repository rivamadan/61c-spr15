LfsrPalindrome:
		addu $t0, $a0, $0
loop:
		lfsr $t0, $t0
		beq $t0, $a0, not_found
		bitpal $t1, $t0
		bne $t1, $0, found
		j loop
found:
		addu $v0, $t0, $0
		jr $ra
not_found:
		addu $v0, $a0, $0
		jr $ra
		
