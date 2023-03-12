/* 

PURPOSE OF FILE:
implementation file for simulator

*/

#include<RISCV.h>
#include<bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>

//load_program_memory(char *file_name, int n) populates the instruction memory from file_name and stores value of N (required for all test files) to x3 in register file
void load_memory(char *file_name, int N) {
  
  //instruction memory:
  programcode = fopen(file_name, "r");
  if(programcode == NULL) {
    printf("Error opening input file!\n");
    exit(1);
  }
  
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
