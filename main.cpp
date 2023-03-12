/* 

PURPOSE OF FILE:
handles input and output, invokes simulator

*/

#include<RISCV.h>
#include<bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>
#include<string>

int main(int argc, char** argv) {
  char* prog_mem_file;
  int n;
  if(argc < 2) {
    printf("Incorrect number of arguments. Invoke the simulator: \n\t./myRISCVSim <input mem file> [<vale of N>]\n");
    exit(1);
  } else if(argc == 2) {
    printf("Since value of N not provided, taking default value N = 5\n\n");
    n = 5;
  } else if(argc == 3) {
    n = stoi(argv[2]);
  } else {
    printf("Incorrect call of program. Please check for typos. Invoke the simulator: \n\t./myRISCVSim <input mem file> [<value of N>]\n");
    exit(1);
  }
  
  //load the program memory
  load_memory(argv[1], n);
  //run the simulator
  run();

  return 0;
}
