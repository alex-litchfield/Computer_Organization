################################################################################
# Homework 3: Matrix-matrix multiplication
# CSCI-2500 Fall 2021
################################################################################
## Data segment
################################################################################
.data
  newline:  .asciiz "\n"
  tab:      .asciiz "\t"

################################################################################
## Text segment
################################################################################
.text
  .align 2          # instructions must be on word boundaries
  .globl main       # main is a global label
  .globl mm_read    # read in NxM matrix
  .globl mm_alloc   # allocate space for NxM matrix
  .globl mm_print   # print out formatted NxM matrix
  .globl mm_mult    # multiple two non-square matrices

################################################################################
# Main segment
################################################################################
main:
  # save return address on the stack
  sub $sp, $sp, 4   
  sw  $ra, 0($sp)

  # Read in a matrix 1 of some NxM size
  # Return in $v0 - N
  # Return in $v1 - M
  # Return in 0($sp) - base address of matrix
  sub $sp, $sp, 4   # make space on stack for return value
  jal mm_read

  # save matrix 1 values
  move  $s0, $v0
  move  $s1, $v1
  lw    $s2, 0($sp)
  add   $sp, $sp, 4   # restore stack

  # Setup arguments and print out matrix 1
  move  $a0, $s0
  move  $a1, $s1
  move  $a2, $s2
  jal mm_print

  # Read in matrix 2 of some NxM size
  sub $sp, $sp, 4   # make space on stack for return value
  jal mm_read

  # save matrix 2 values
  move  $s3, $v0
  move  $s4, $v1
  lw    $s5, 0($sp)
  add   $sp, $sp, 4   # restore stack

  # Setup arguments and print out matrix 2
  move  $a0, $s3
  move  $a1, $s4
  move  $a2, $s5
  jal mm_print

  # Setup arguments and call mm_mult
  # Note: 5 arguments, so we need to use stack
  # Note: num cols matrix 1 == num rows matrix 2
  move  $a0, $s0    # num rows matrix1
  move  $a1, $s1    # num cols matrix1
  move  $a2, $s2    # address of matrix 1
  move  $a3, $s4    # num cols matrix2
  sub   $sp, $sp, 4   
  sw    $s5, 0($sp) # address of matrix 2
  jal mm_mult
  add   $sp, $sp, 4

  # print the result
  move $a0, $s0 
  move $a1, $s4
  move $a2, $v0
  jal mm_print

  # restore $ra, free stack space, and return
  lw  $ra, 0($sp)
  add $sp, $sp, 4
  jr  $ra

################################################################################
# mm_read: Read in a NxM matrix from standard input
# Note: You will be calling mm_alloc from this function, so you'll need to save 
#       $ra on the stack. Also, you'll be returning 3 values back to main, one
#       value (the matrix base address), will need to be saved on the stack.
#       Matrix format is:
#         First line:  N = num rows
#         Second line: M = num cols
#         Subsequent lines contain one value each, filling each row in turn
#         (same format as hw01, except N,M on separate lines)  
# Input:
#   None
# Return:
#   v0 = N
#   v1 = M
#   0($sp) = base address of matrix
################################################################################
mm_read:
  # save return address and any saved registers on the stack, if necessary
  sub $sp, $sp, 12 #$sp = $sp-4
  sw $ra, 0($sp) #Memory[$sp+0] = $ra (Word from register to memory)
  sw $s0, 4($sp)
  sw $s1, 8($sp)

  # get N
  addi $v0, $zero, 5 #$v0 = $zero+5
  syscall #stop until input recieved
  addi $a0, $v0, 0 #$a0 = $v0+0
  addi $s0, $a0, 0

  # get M
  addi $v0, $zero, 5 #$v0 = $0+5
  syscall #stop until input recieved
  addi $a1, $v0, 0 #$a0 = $v0+0
  addi $s1, $a1, 0

  # Setup up arguments and call mm_alloc - v0 is returned as base address
  jal mm_alloc
  # do nested loops to read in values
  #for (i = 0; i < rows_num; i++) {
  #  for (j = 0; j < cols_num; j++) {
  #   mat -> data[i][j] = data_point; resultMatrix[i][j]=data_point would also work
  #  }
  #} - sourced from my hw01.c file.
  addi $t1, $v0, 0 #$t0 = $v0 + 0
  addi $t2, $v0, 0 #$t1 = $v0 + 0
  addi $t3, $zero, 0 #$t2 = $zero + 0 and $t2=i 

read_in_rows: #for loop w/ rows_num in it
  beq $t3, $s0, end_of_row #if($t2==$a0){row_end;} (if i==rows_num)
  addi $t4, $zero, 0 #$t3 = $zero + 0 and $t3=j

read_in_cols: #for loop w/ cols_num in it
  beq $t4, $s1, end_of_col # if j = cols
  addi $v0, $zero, 5 #$v0 = $zero + 5
  syscall
  sw $v0, 0($t2) #Memory[$t1+0] = $v0 (Word from register to memory) - putting value into matrix
  addi $t2, $t2, 4 #$t1 = $t1+4 - moving to next element in matrix
  addi $t4, $t4, 1 #$t3 = $t3+1 - this is j++ in our for loop
  j read_in_cols #jump back to beginning of read_in_cols

end_of_col:
  addi $t3, $t3, 1 #$t2 = $t2+1 - this is i++ in our for loop
  j read_in_rows #jump back to beginning of read_in_rows

end_of_row:
  # setup up return values
  addi $v0, $s0, 0 #$v0 = $a0+0 - this is $v0 = N
  addi $v1, $s1, 0 #$v1 = $a1+0 - this is $v1 = M

  # restore stack, ra, and any saved registers, if necessary
  lw $ra, 0($sp) #$ra = Memory[$sp + 0] (Word from memory to register)
  lw $s0, 4($sp)
  lw $s1, 8($sp)
  addi $sp, $sp, 12 #$sp= $sp+4

  # Note: third return value goes on the stack *after* restoration below
  sw $t1, 0($sp) #Memory[$sp+0] = $t1 (Word from register to memory)

  # return to main
  jr  $ra

################################################################################
# mm_alloc: Allocate an NxM matrix
# Note: You can just allocate a single array of length N*M.
# Input:
#   a0 = N
#   a1 = M
# Return:
#   v0 = base address of allocated space
################################################################################
mm_alloc:
  # save return address and any saved registers on the stack, if necessary

  # Allocate sufficient space

  mul $t0, $a0, $a1 #$t0 = $a0x$a1
  addi $v0, $zero, 9 #$t2 = $zero + 9
  sll $a0, $t0, 2 #$a0 = $t0 << 2 (shift left by two positions)
  syscall

  # set return value

  # restore stack, ra, and any saved registers, if necessary

  # return to main
  jr  $ra

################################################################################
# mm_print: Output the formatted contents of NxM matrix
# Note: Within a row, separate each value by tab
#       End each row with a newline
# Input:
#   a0 = N
#   a1 = M
#   a2 = base pointer
# Return:
#   None
################################################################################
mm_print:
  # save return address and any saved registers on the stack, if necessary
  addi $t0, $a0, 0 #$t0 = $a0+0
  addi $t1, $zero, 0 #$t1 = $zero+0 - this is also int i = 0

start_of_rows:
  beq $t1, $t0, end_of_rows #if($t1==$t0){end_of_rows;} this is also if(i = rows_num)
  addi $t2, $zero, 0 #$t2 = $zero+0 - this is also int j = 0

start_of_cols:
  beq $t2, $a1, end_of_cols #if($t2==$a1){end_of_cols;} this is also if(j == cols)
  # print number
  addi $v0, $zero, 1 #$v0 = $zero + 1
  lw $a0, 0($a2) #$a0 = Memory[$a2+0] (Word from memory to register)
  syscall
  # print tab
  addi $v0, $zero, 4 #$v0 = $zero + 0
  la $a0, tab #$a0 = address of tab
  syscall
  # move to next element in matrix
  addi $a2, $a2, 4 #$a2 = $a2 + 4
  addi $t2, $t2, 1 #$t2 = $t2 + 1 - this is our j++
  j start_of_cols #jump to beginning of start_of_cols

end_of_cols:
  addi $v0, $zero, 4 #$v0 = $zero + 4
  la $a0, newline #$a0 = address of newline
  syscall
  addi $t1, $t1, 1 #$t1 = $t1 + 1 - this is our i++
  j start_of_rows #jump to beginning of start_of_rows

end_of_rows:
  # restore stack, ra, and any saved registers, if necessary
  addi $a0, $t0, 0 #$a0 = $t0 + 0
  # return to main
  jr  $ra

################################################################################
# mm_mult: Perform the actual NxM matrix multiplication
# Input:
#   a0 = N for matrix 1
#   a1 = M for matrix 1 <-> N for matrix 2
#   a2 = base address for matrix 1
#   a3 = M for matrix 2
#   0($sp) = base address for matrix 2
# Return:
#   v0 = base address for result matrix
################################################################################
mm_mult:
  # save return address and any saved registers on the stack, if necessary
  lw $t1, 0($sp)   # get base address of matrix 2
  sub $sp, $sp, 24
  sw $ra, 0($sp)
  sw $s0, 4($sp)
  sw $s1, 8($sp)
  sw $s2, 12($sp)
  sw $s3, 16($sp)
  sw $s4, 20($sp)

  # set up and call mm_alloc for result matrix
  addi $s0, $a0, 0 # saving the rows of matrix 1
  addi $s1, $a1, 0 # saving columns of matrix 1 as well as rows of matrix 2 (they are the same)
  addi $s2, $a3, 0 # saving the columns of matrix 2
  addi $s4, $a2, 0 # saves the base address of matrix 1
  addi $a1, $a3, 0 # columns of matrix 1 edited
  jal mm_alloc #allocates space in mm_alloc then returns
  addi $s3, $v0, 0 # keeps result of base matrix 
  addi $t2, $zero, 0 # i = 0
 
# three nested loops for matrix-matrix multiplication
rows_start:
  beq $t2, $s0, rows_end    # if i = # of rows in result matrix
  addi $t3, $zero, 0               # j = 0

columns_start:
  beq $t3, $s2, columns_end    # if = # of columns in result matrix
  addi $t4, $zero, 0               # k = 0
  addi $t6, $zero, 0 
    
loop3_start:
  beq $t4, $s1, loop3_end    # if k = # of columns in matrix 1

matrix1_offset:
  # find offset of mat1[i][k]
  mul $t8, $s1, $t2
  add $t8, $t8, $t4
  sll $t8, $t8, 2
  add $t8, $t8, $s4     
  lw $t9, 0($t8)     # get mat1[i][k] 

matrix2_offset:
  # find offset of mat2[k][j]
  mul $t7, $s2, $t4
  add $t7, $t7, $t3
  sll $t7, $t7, 2
  add $t7, $t7, $t1            
  lw $t5, 0($t7)    # get mat2[k][j]  

find_result:
  mul $t7, $t9, $t5
  add $t6, $t6, $t7
  addi $t4, $t4, 1 # k++
  j loop3_start         

loop3_end:
  # resultMatrix[i][j]
  mul $t9, $s2, $t2
  add $t9, $t9, $t3
  sll $t9, $t9, 2
  add $t9, $t9, $s3              

  # set return value
keep_result:
  sw $t6, 0($t9) # storing answer in resultMatrix
  addi $t3, $t3, 1
  j columns_start

columns_end:
  addi $t2, $t2, 1
  j rows_start

rows_end:
  # saving base address of result matrix in $v0
  addi $v0, $s3, 0

  # restore stack, ra, and any saved registers, if necessary
  lw $ra, 0($sp)
  lw $s0, 4($sp)
  lw $s1, 8($sp)
  lw $s2, 12($sp)
  lw $s3, 16($sp)
  lw $s4, 20($sp)
  addi $sp, $sp 24

  # return to main
  jr  $ra
