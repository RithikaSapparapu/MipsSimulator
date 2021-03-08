.data
    .word 9 # 0x10010000 - 0001 0000 0000 0001 0000 0000 0000 0000 
    .word 3
    .word 1
    .word 5
    
    # array: .word 9,3,1,5
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
    lui $s0, 0x1001

    beq $t5, $zero, exit
    addi $t5, $zero, 0
    j while

exit:
    jr $ra
