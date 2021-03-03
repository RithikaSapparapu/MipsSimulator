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
    lui $s0, 0x1001
    j while

while:
    add $t5, $zero, $zero
    lw $t0, 0($s0)
    lw $t1, 4($s0)
    slt $t4, $t0, $t1
    bne $t4, $zero, nextcomp 
    sw $t1, 0($s0)
    sw $t0, 4($s0)
    add $t5, $t5, $t6
    j nextcomp

nextcomp:
    lw $t0, 4($s0)
    lw $t1, 8($s0)
    slt $t4, $t0, $t1
    bne $t4, $zero, nextcomp1 
    sw $t1, 4($s0)
    sw $t0, 8($s0)
    add $t5, $t5, $t6
    j nextcomp1

nextcomp1:
    lw $t0, 8($s0)
    lw $t1, 12($s0)
    slt $t4, $t0, $t1
    bne $t4, $zero, next 
    sw $t1, 8($s0)
    sw $t0, 12($s0)
    add $t5, $t5, $t6
    j next

next:
    beq $t5, $zero, exit
    j while

exit:
    jr $ra
