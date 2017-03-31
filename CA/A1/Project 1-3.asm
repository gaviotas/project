.data
	prompt_1: .asciiz "\Enter two inputs\nThe first input(x) : "
	prompt_2: .asciiz "\The second input(y) : " 
	message: .asciiz "\2x^2+4xy+3y^2-7 = : "
	
.text
.main:
    li $v0 4
    la $a0, prompt_1
    syscall #syscall with $v0 == 4 : print string from $a0
    
    # Get the first inputs
    li $v0, 5
    syscall
    
    # Store the result in $t0
    move $t0, $v0 # $t0 = x
    
    li $v0 4
    la $a0, prompt_2
    syscall #syscall with $v0 == 4 : print string from $a0
    
    # Get the second inputs
    li $v0, 5
    syscall
    
    # Store the result in $t0
    move $a1, $v0  # Store the second input in $a1 ($a1 = y)
    
    move $a0, $t0 # Store the first input in $a0 ($a0 = x)
    
    jal EQUATION
     	  
    # Display--------------------------------------------------------------------------------------
    DISPLAY:
    li $v0, 4
    la $a0, message
    syscall
    
    # Print or show the result
    li $v0, 1
    move $a0, $v1
    syscall
    
    li $v0, 10
    syscall #syscall with $v0 == 10 : exit
    
   # Mul the two inputs --------------------------------------------------------------------------
    MULTIPLY:
    	addi $sp, $sp,-12
    	sw $ra, 8($sp)
    	sw $a1, 4($sp)
    	sw $a0, 0($sp)
  	
  	mul $v1, $a0, $a1 		
    	
    	lw $a0, 0($sp)
    	lw $a1, 4($sp)
    	lw $ra, 8($sp)
    	addi $sp, $sp, 12
    	jr $ra
    	
    #Div the two inputs ---------------------------------------------------------------------------
    SQUARE:
    	addi $sp, $sp,-8
    	sw $ra, 4($sp)
    	sw $a0, 0($sp)
    	
    	mul $v1, $a0, $a0
   	
      	lw $a0, 0($sp)
     	lw $ra, 4($sp)
    	addi $sp, $sp, 8
    	jr $ra
    	
    #Compute the EQUATION -------------------------------------------------------------------------
    # $a0 = x, $a1 = y, equation = 2x^2+4xy+3y^2-7
    EQUATION:
    	addi $sp, $sp,-12
    	sw $ra, 8($sp)
    	sw $a1, 4($sp)
    	sw $a0, 0($sp)
    	
    	jal SQUARE # $v1 = x^2	
    	move $s0, $v1 # $s0 = x^2
    	
    	jal MULTIPLY # $v1 = xy
    	move $s1, $v1 # $s1 = xy
    	
    	move $a0, $a1
    	
    	jal SQUARE # $v1 = y^2
    	move $s2, $v1 # $s2 = y^2
    	
    	move $a0, $s0
    	addi $a1, $zero, 2
    	
    	jal MULTIPLY # $v1 = 2x^2
    	move $s0, $v1 # $s0 = 2x^2
    	
    	move $a0, $s1
    	addi $a1, $zero, 4
    	
    	jal MULTIPLY # $v1 = 4xy
    	move $s1, $v1 # $s1 = 4xy
    	
    	add $s0, $s0, $s1 # $s0 = 2x^2+4xy
    	
    	move $a0, $s2
    	addi $a1, $zero, 3
    	
    	jal MULTIPLY # $v1 = 3y^2
    	
    	add $v1, $v1, $s0 # $v1 = 2x^2+4xy+3y^2
    	
    	subi $v1, $v1, 7 # $v1 = 2x^2+4xy+3y^2-7
	    	   	
      	lw $a0, 0($sp)
    	lw $a1, 4($sp)
    	lw $ra, 8($sp)
    	addi $sp, $sp, 12
    	jr $ra    	