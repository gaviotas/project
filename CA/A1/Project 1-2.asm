.data
	prompt_1: .asciiz "\Enter two inputs\nThe first input : "
	prompt_2: .asciiz "\The second input : " 
	message_mul: .asciiz "\nMUL : "
	message_div_q: .asciiz "\nDIV_quotient is "
	message_div_r: .asciiz "\nDIV_remainder is "
	
.text
.main:
    li $v0 4
    la $a0, prompt_1
    syscall #syscall with $v0 == 4 : print string from $a0
    
    # Get the first inputs
    li $v0, 5
    syscall
    
    # Store the result in $t0
    move $t0, $v0
    
    li $v0 4
    la $a0, prompt_2
    syscall #syscall with $v0 == 4 : print string from $a0
    
    # Get the second inputs
    li $v0, 5
    syscall
    
    # Store the result in $t0
    move $a1, $v0  # Store the second input in $a1
    
    move $a0, $t0 # Store the first input in $a0
    
    jal MYMULT
    jal MYDIV
    
    j DISPLAY
    # Mul the two inputs --------------------------------------------------------------------------
    MYMULT:
    	addi $sp, $sp,-12
    	sw $ra, 8($sp)
    	sw $a1, 4($sp)
    	sw $a0, 0($sp)
    	
    	LOOP_MUL:
    		add $v1, $v1, $a1
    		subi $a0, $a0, 1
    		
    		bgtz $a0, LOOP_MUL
    		
    	
    	lw $a0, 0($sp)
    	lw $a1, 4($sp)
    	lw $ra, 8($sp)
    	addi $sp, $sp, 12
    	jr $ra
    	
    #Div the two inputs ---------------------------------------------------------------------------
    MYDIV:
    	addi $sp, $sp,-12
    	sw $ra, 8($sp)
    	sw $a1, 4($sp)
    	sw $a0, 0($sp)
    	
    	addi $t0, $zero, -1    	
    	
    	LOOP_DIV:
    		addi $s0, $s0, 1
    		sub $a0, $a0, $a1
    		
    	 	bgt $a0, $t0, LOOP_DIV
    	
    	subi $s0, $s0, 1
    	add $s1, $a0, $a1 
    		
    	
      	lw $a0, 0($sp)
    	lw $a1, 4($sp)
    	lw $ra, 8($sp)
    	addi $sp, $sp, 12
    	jr $ra
    	  
    # Display--------------------------------------------------------------------------------------
    DISPLAY:
    li $v0, 4
    la $a0, message_mul
    syscall
    
    # Print or show the result
    li $v0, 1
    move $a0, $v1
    syscall
    
    li $v0, 4
    la $a0, message_div_q
    syscall
    
    li $v0, 1
    move $a0, $s0
    syscall
    
    li $v0, 4
    la $a0, message_div_r
    syscall
    
    li $v0, 1
    move $a0, $s1
    syscall 
    
    li $v0, 10
    syscall #syscall with $v0 == 10 : exit
    
