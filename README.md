MIPS SIMULATOR-(Team dynamic: Rithika Sapparapu, Greeshmitha B):
This is a simulator designed on the lines of QtSPIM, which has been implemented in phases.

phase1:
This is a Single Cycle MIPS Processor Simulator written in C++. Every one word in MIPS processor is assumed 32 bits. It uses BubbleSort.asm file as MIPS assembly code input and executes it.
In order to run it, this repository is to be cloned, compiled and then run. The BubbleSort.asm is a mips assembly program which sorts values using the bubble sort algorithm. It finally shows the values in the Memory and the values stored in each of the registers. There are two modes where one shows values in the memory and the registers once, after all the mips instructions are executed, and the other mode shows the same everytime an instruction is executed.  

phase2:
This is a MIPS processor simulator written in c++.In this,there are two modes in which one simulates the program by forwarding and the other simulates the program using NO forwarding.The user is given choice to choose in these two modes.It uses BubbleSort.asm file as MIPS assembly code and executes it.
This simulator outputs total number of clock cycles,total number of stalls,IPC and the list of instructions where stalls occur and also outputs the memory where the input array is sorted.
