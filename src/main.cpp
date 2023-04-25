/* 

PURPOSE OF FILE:
handles input and output, invokes simulator

*/

#include "Core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

//Function reads command line knobs that do not have any arguments:
//-pipeline -forward -registers -trace -cache
bool forNoArguments (char** start, char** end, const std::string& knob) {
  return std::find(start, end, knob) != end;
}

//Function reads command line knobs that have arguments:
//-test <filename.mc> -n <N> -trace <trace_instruction>
char* forGetArguments (char** start, char** end, const std::string& knob) {
  char** itr = std::find(start, end, knob);
  if ( (itr != end) & (++itr != end) ) {
    return *itr;
  }
  return 0;
}

//Function will print how to execute program
void help() {
  printf("\n-------------------------------\n");
  printf("\nCommand to execute program:\n");
  printf("RISCV <knobs>\n\n");
  printf("<knobs> --\n");
  printf("          | -test <filename.mc>\n");
  printf("          | -n <value of N>\n");
  printf("          | -pipeline\n");
  printf("          | -forward\n");
  printf("          | -trace [<instruction number>]\n");
  printf("          | -registers\n");
  printf("          | -cache\n");
  printf("          | -h (or) -help\n\n");
  printf("(compulsory knob: -test <filename.mc>)\n(all other knobs are optional:\n");
  printf("          | -h (or) -help will show how to execute\n");
  printf("          | default value of N is 5\n");
  printf("          | default setting of caching, pipelining, forwarding, tracing, and showing register values is off\n");
  printf("          | if forward/trace is called without calling pipeline, it doesn't do anything\n");
  printf("          | if cache is called without calling pipeline, it doesn't work\n");
  printf("          | providing instruction number after -trace knob will activate tracing for that particular instruction only, otherwise for all instructions\n");
  printf("          | -registers knob will show values in register file after every cycle)\n\n");
  printf("\n-------------------------------\n");
}

int main(int argc, char* argv[]) {

  Core CPU;

  if (argc < 2) {
    printf("No arguments given! Please read-->\n\n");
    help();
    exit(2); //program called incorrectly.
  
  }

  if ( forNoArguments(argv, argv + argc, "-h") | forNoArguments(argv, argv + argc, "-help") ) {
    help();
    exit(1); //program not run, help called.
  }

  char* file_input = forGetArguments(argv, argv + argc, "-test");
  char* n = forGetArguments(argv, argv + argc, "-n");

  if (file_input) {
    CPU.test_file = file_input;
  } else {
    printf("No file name given! Please read-->\n\n");
    help();
    exit(3); //test file related error.
  }
  
  if (n) {
    try {
      CPU.N = std::stoi(n);
    } catch (...) {
      printf("\nTaking default value of N = 5\n"); //Value of n given cannot be typecasted to an integer
      CPU.N = 5;
    }
  } else {
    printf("\nTaking default value of N = 5\n"); //Value of n not given
    CPU.N = 5;
  }

  if ( forNoArguments(argv, argv + argc, "-pipeline") ) {
    CPU.pipelining = true;
    if ( forNoArguments(argv, argv + argc, "-forward") ) {
      CPU.forwarding = true;
    }
    if ( forNoArguments(argv, argv + argc, "-trace") ) {
      CPU.tracing = true;
      char* ins = forGetArguments(argv, argv + argc, "-trace");
      if (ins) {
        try {CPU.trace_instruction = std::stoi(ins);}
        catch(...) {CPU.trace_instruction = 0;} //Instruction number given cannot be typecasted to an integer
      } else {
        CPU.trace_instruction = 0; //Instruction number not given
      }
    }
    if ( forNoArguments(argv, argv + argc, "-registers") ) {
      CPU.register_file = true;
    }
  }

  if ( forNoArguments(argv, argv + argc, "-cache") ) {
    CPU.caching = true;

    printf("\nEnter Instruction cache specifications:\n");
    printf("Instruction Cache Size (in bytes): ");
    scanf("%d", &CPU.i_sizeCache);
    printf("Instruction Cache Block Size (in bytes): ");
    scanf("%d", &CPU.i_sizeBlock);
    printf("Instruction Cache Mapping (1 for DM, 2 for SA, 3 for FA): ");
    scanf("%d", &CPU.i_mapping);
    if (CPU.i_mapping == 2) {
      printf("Instruction Cache no. of ways: ");
      scanf("%d", &CPU.i_ways);
      // Cache I(CPU.i_sizeCache, CPU.i_sizeCache, CPU.i_ways);
      // I.whichCache = 1;
    } else {
      if (CPU.i_mapping == 1) {
        CPU.i_isDirect = true;
      }
      // Cache I(CPU.i_sizeCache, CPU.i_sizeBlock, CPU.i_isDirect);
      // I.whichCache = 1;
    }
    if (CPU.i_isDirect == false) {
      printf("Instruction Cache replacement policy (1 for LRU, 2 for FIFO, 3 for RANDOM, 4 for LFU): ");
      scanf("%d", &CPU.i_replacement);
      // I.replace = CPU.i_replacement;
    }
    printf("Instruction Cache Miss Penalty: ");
    scanf("%d", &CPU.i_penalty);

    printf("\n\nEnter Data cache specifications:\n");
    printf("Data Cache Size (in bytes): ");
    scanf("%d", &CPU.d_sizeCache);
    printf("Data Cache Block Size (in bytes): ");
    scanf("%d", &CPU.d_sizeBlock);
    printf("Data Cache Mapping (1 for DM, 2 for SA, 3 for FA): ");
    scanf("%d", &CPU.d_mapping);

    if (CPU.d_mapping == 2) {
      printf("Data Cache no. of ways: ");
      scanf("%d", &CPU.d_ways);
      // Cache D(CPU.d_sizeCache, CPU.d_sizeCache, CPU.d_ways);
      // D.whichCache = 2;
    } else {
      if (CPU.d_mapping == 1) {
        CPU.d_isDirect = true;
      }
      // Cache D(CPU.d_sizeCache, CPU.d_sizeBlock, CPU.d_isDirect);
      // D.whichCache = 2;
    }
    if (CPU.d_isDirect == false) {
      printf("Data Cache replacement policy (1 for LRU, 2 for FIFO, 3 for RANDOM, 4 for LFU): ");
      scanf("%d", &CPU.d_replacement);
      // D.replace = CPU.d_replacement;
    }
    printf("Data Cache Miss Penalty: ");
    scanf("%d", &CPU.d_penalty);

  }


  CPU.loadMemory();
  CPU.Run();

  return 0;
}
