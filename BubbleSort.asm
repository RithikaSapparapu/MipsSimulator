.data
    array: .word 9, 3, 1    
.text
.globl main
main:
    addi $t6, $zero, 1  
    addi $t5, $zero, 1
    j bigwhile
while:
    lw $t0, 0($s0)
    lw $t1, 4($s0)
    slt $t4, $t0, $t1
    bne $t4, $zero, increment 
    sw $t1, 0($s0)
    sw $t0, 4($s0)
    add $t5, $t5, $t6
    addi $s0, $s0, 4
    lw $t3, 4($s0)
    beq $t3, $zero, bigwhile
    j while
increment: 
    addi $s0, $s0, 4
    lw $t3, 4($s0)
    beq $t3, $zero, bigwhile
    j while
bigwhile:
    la $s0, array
    beq $t5, $zero, exit
    addi $t5, $zero, 0
    j while
exit:
    jr $ra

