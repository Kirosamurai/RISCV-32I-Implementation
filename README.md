# RISCV-32I-Implementation
A C++ implementation of RISCV-32I Single cycle processor

Structures for each unit in the processor:
1. Register File: Array of unsigned 32bit int values of size 32
2. PC: Starts at 0x0 at start of program
3. Memory: Array of unsigned 32bit int values of size 1024(for now)
4. Instruction Memory: .mc file consiting of instructions in hex form 
5. Decoder: Extracts information from instruction
6. Control Unit: Implemented using switch-case functionality of C++

Each Instruction goes through the following phases:

1.Fetch
2.Decode
3.Execute
4.Memory Access
5.Register Write

Separate functions are created for these phases/tasks.

After each function is executed, programs provides a message as to what it is doing.
Program end after termination code.


To add:
1. Pipeline Stages
2. Memory Hierarchy


Developer's Name: Rhitvik Anand, 2021CSB1127
Developer's Mail: rhitvik2000anand@gmail.com / 2021csb1127@iitrpr.ac.in
