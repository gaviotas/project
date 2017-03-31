.data
	prompt: .asciiz "\Enter the inputs : "
	message: .asciiz "\The result of Fibonacci is : "
	
.text
.main:
    li $v0 4
    la $a0, prompt
    syscall #syscall with $v0 == 4 : print string from $a0
    
    # Get the input
    li $v0, 5
    syscall
    
    # Store the result in $t0
    move $a0, $v0
    
    jal FIBONACCI
     	  
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
    
   # compute the fibonacci num --------------------------------------------------------------------
    FIBONACCI:
    	addi $sp, $sp,-12
    	sw $ra, 8($sp)
    	sw $s0, 4($sp)
    	sw $a0, 0($sp)
  	
  	li $v1, 1
  	ble $a0, 2, EXIT
  	
  	addi $a0, $a0, -1
  	jal FIBONACCI
  	
  	move $s0, $v1
  	
  	addi $a0, $a0, -1
  	jal FIBONACCI
  	
  	add $v1, $s0, $v1
  	
    EXIT:
    	lw $a0, 0($sp)
    	lw $s0, 4($sp)
    	lw $ra, 8($sp)
    	addi $sp, $sp, 12
    	jr $ra
    	
