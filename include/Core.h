#ifndef CORE_H_
#define CORE_H_

#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>

#include "CacheFile.h"
#include "Pipeline.h"
#include "RISCV.h"
#include "GUI.h"

class Core {

public:

    char *test_file;
    int N;
    
    //For phase 3:
    bool caching;
    int i_sizeCache;
    int i_sizeBlock;
    int i_mapping;
    int i_ways;
    bool i_isDirect;
    int i_replacement;
    int i_penalty;
    int d_sizeCache;
    int d_sizeBlock;
    int d_mapping;
    int d_ways;
    bool d_isDirect;
    int d_replacement;
    int d_penalty;
    
    //KNOBS read from command line:
    bool pipelining;
    bool forwarding;
    bool tracing;
    int trace_instruction;
    bool register_file;
    bool pipeline_register;

    //Store values of stats:
    int instructions; //stat 2
    float CPI; //stat 3
    int data_instructions; //stat 4
    int ALU_instructions; //stat 5
    int control_instructions; //stat 6
    int bubbles; //stat 7
    int data_hazards; //stat 8
    int control_hazards; //stat 9
    int data_bubbles; //stat 11
    int control_bubbles; //stat 12
    int wrong_predicts; //stat 10

    float CPI_cache;
    float CPI_ideal_cache;

    //Constructor
    Core() {
        
        N = 5;
        
        caching = false;
        i_isDirect = false;
        d_isDirect = false;

        pipelining = false;
        forwarding = false;
        tracing = false;
        trace_instruction = 0;
        register_file = false;
        pipeline_register = false;

        instructions = 0;
        CPI = 1.00;
        data_instructions = 0;
        ALU_instructions = 0;
        control_instructions = 0;

        bubbles = 0;
        data_hazards = 0;
        control_hazards = 0;
        data_bubbles = 0;
        control_bubbles = 0;
        wrong_predicts = 0;
        
    }

    //Opening test file and defining memory
    void loadMemory();

    //Starting the actual simulator
    void Run();

    //Pipeline Functionality
    void runCycle();
    void Stop();
    void showStats();

};

void showRegisters();

extern Core CPU;

extern PipelineRegister IF_DE;
extern PipelineRegister DE_EX;
extern PipelineRegister EX_MA;
extern PipelineRegister MA_WB;

extern RISCV processor;

#endif