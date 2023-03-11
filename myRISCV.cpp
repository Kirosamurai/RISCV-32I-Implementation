/* 

PURPOSE OF FILE:
implementation file for simulator

[this is where you have to define all the fetch decode etc wale functions]

*/

#include<RISCV.h>
#include<bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>

//reset_proc() is called to reset the processor to default values
void reset_proc() {
  
  //setting empty memory:
  //please explain to me how the memory map thingy works
  for (int i = 0; i < MEM_SIZE; i++) {
    MEM[i] = 0x0;
  }
  
  //setting empty registers:
  //do we wanna put default values in registers? like global pointer stack pointer etc? (i think we should)
  for (int i= 0; i < ADD_LEN; i++) {
    reg[i] = 0x0;
  }
  
  reg[2] = 0x7FFFFFF0
}

//load_program_memory(char *file_name, int n) pupulates the instruction memory from file_name and stores value of N (required for all test files) to x3 in register file
void load_memory(char *file_name, int N) {
  
  //instruction memory:
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input file!\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address, instruction);
  }
  fclose(fp);
  
  //storing value of N:
  reg[3] = N;
}

//run() starts the actual single cycle processor simulator
void run() {
  while(1) {
    fetch();
    decode();
    execute();
    mem();
    write_back();
  }
}
