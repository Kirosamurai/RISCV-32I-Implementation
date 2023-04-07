#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <iterator>
#include "Core.h"

PipelineRegister IF_DE;
PipelineRegister DE_EX;
PipelineRegister EX_MA;
PipelineRegister MA_WB;

InstructionPacket IF_DE_IP;
InstructionPacket DE_EX_IP;
InstructionPacket EX_MA_IP;
InstructionPacket MA_WB_IP;


//Function will set all pre requisites for running the processor
void Core::loadMemory() {
    
    //Instruction memory:
    processor.programcode = fopen(test_file, "r+");
    if(processor.programcode == NULL) {
        printf("Error opening input file!\n");
        exit(3); //test file related error
    }
    
    //Storing value of N:
    processor.reg[3] = N;

    //Enabling/disabling pipelining and forwarding:
    processor.pipeline = pipelining;
    processor.forward = forwarding;
}

//Function starts the actual processor
void Core::Run() {
    if(!pipelining){
        while(1) {
            //Run without pipelining:
            processor.clock_cycle++;
            processor.fetch();
            processor.clock_cycle++;
            processor.decode();
            processor.clock_cycle++;
            processor.execute();
            processor.clock_cycle++;
            processor.mem();
            processor.clock_cycle++;
            processor.write_back();
            processor.pc = processor.new_pc;
            printf("Clock Cycle %d finished.\n",processor.clock_cycle);
            std::cout<<"--------------------------------------------------\n\n";
        }
    }else{
        while(1) {
            //Run with pipelining:
            processor.clock_cycle++;
            runCycle();
            std::cout<<"--------------------------------------------------\n";
            printf("Clock Cycle %d finished.\n",processor.clock_cycle);
            std::cout<<"--------------------------------------------------\n";
            std::cout<<"--------------------------------------------------\n\n";
        }
    }
}

//Simulates all functionality in one clock cycle in a pipelined processor
//Called inside Run() if Pipelining knob is TRUE
void Core::runCycle() {

    //------------------------CHECK DEPENDENCY------------------------

    //note: giving control dependency priority over data dependency
    
    if (EX_MA.op_code_pl == 23) { //AUIPC
        EX_MA.ALUres_pl = EX_MA.current_pc_pl + EX_MA.immU_pl;
    }

    processor.DataDependency = isDataDependency();
    processor.ControlDependency = isControlDependency();
    
    if (processor.ControlDependency) {

        //There is control dependency,
        //Meaning bubbles must be inserted.
        
        control_hazards++;
        control_bubbles += 6;
        
        IF_DE.Stall();
        DE_EX.Stall();

        IF_DE.DepFlag_pl = 1;
        DE_EX.DepFlag_pl = 1;
        
        switch (processor.ControlDependency) {
        
            //B type
            case 1:
            if (predict_type == 2) {
                wrong_predicts++;
            }
            break;

            //JAL
            case 2:
            processor.new_pc = EX_MA.current_pc_pl + EX_MA.immJ_pl;
            EX_MA.ALUres_pl = EX_MA.current_pc_pl + 4;
            break;

            //JALR
            case 3:
            processor.new_pc = EX_MA.op1_pl + EX_MA.immI_pl;
            EX_MA.ALUres_pl = EX_MA.current_pc_pl + 4;
            break;

        }
    
    } else if (!forwarding & (processor.DataDependency!=0)) {
        //There is data dependency,
        //And forwarding knob is FALSE
        //Meaning bubbles must be inserted
        data_hazards++;
        data_bubbles += 3;
        IF_DE = DE_EX;
        DE_EX.Stall();
        DE_EX.DepFlag_pl = 2;
    }

    //-----------------------------FORWARDING IMPLEMENTATION----------------------------------
    
    if (forwarding) {

        //SETTING OP1 AND OP2 FOR EX
        DE_EX.op1_pl = processor.reg[DE_EX.rs1_pl];
        DE_EX.op2_pl = processor.reg[DE_EX.rs2_pl];

        switch (processor.DataDependency) {
            
            case 1: //MA-->EX FORWARDING

            if(EX_MA.rd_pl == DE_EX.rs1_pl){
                DE_EX.op1_pl = EX_MA.ALUres_pl;
            }else if((!(DE_EX.op_code_pl == 19)) & (!(DE_EX.op_code_pl == 3)) & (!(DE_EX.op_code_pl == 35))){
                DE_EX.op2_pl = EX_MA.ALUres_pl;
            }
            break;
            
            case 2:
            
            if (DE_EX.op_code_pl == 35) { //WB-->MA FORWARDING
                DE_EX.DepFlag_pl = 3;
            }else{ //LOAD-USE HAZARD
                data_hazards++;
                data_bubbles += 3;
                IF_DE = DE_EX;
                DE_EX.Stall();
                DE_EX.DepFlag_pl = 2;
            }
            break;

            case 3: //WB-->EX FORWARDING
            
            if(MA_WB.rd_pl == DE_EX.rs1_pl){
                DE_EX.op1_pl = MA_WB.ALUres_pl;
            }else if((!(DE_EX.op_code_pl == 19)) & (!(DE_EX.op_code_pl == 3)) & (!(DE_EX.op_code_pl == 35))){
                DE_EX.op2_pl = MA_WB.ALUres_pl;
            }
            break;  
            
            case 4: //WB-->EX FORWARDING (Load Type)

            if(MA_WB.rd_pl == DE_EX.rs1_pl){
                DE_EX.op1_pl = MA_WB.LoadData_pl;
            }else if((!(DE_EX.op_code_pl == 19)) & (!(DE_EX.op_code_pl == 3)) & (!(DE_EX.op_code_pl == 35))){
                DE_EX.op2_pl = MA_WB.LoadData_pl;
            }
            break;
        }

        //WB-->MA FORWARDING IMPLEMENTATION
        if (EX_MA.DepFlag_pl == 3) {
            processor.reg[EX_MA.rs2_pl] = MA_WB_IP.LoadData_ip;
        }
    
    }
    
    //------------------------IMPLEMENTING PREDICTED BRANCH------------------------
    
    if(btb.find(IF_DE.current_pc_pl) != btb.end()){
        if(btb[IF_DE.current_pc_pl].first == 1){
            processor.new_pc = btb[IF_DE.current_pc_pl].second;
        }
    }

    //------------------------UPDATING PC VALUE------------------------
    
    if (!forwarding) {
        if (!processor.DataDependency | processor.ControlDependency) {
            processor.pc = processor.new_pc;
        }
    } else {
        if ( (processor.DataDependency != 2) |(DE_EX.DepFlag_pl == 3) | processor.ControlDependency) {
            processor.pc = processor.new_pc;
        }
    }

    processor.current_pc = processor.pc;
    
    //------------------------START PIPELINE------------------------
    
    processor.fetch();
    WriteToPacket(IF_DE_IP);
    
    IF_DE.ReadFromRegister();
    processor.decode();
    WriteToPacket(DE_EX_IP);
    
    DE_EX.ReadFromRegister();
    processor.execute();
    WriteToPacket(EX_MA_IP);
    
    EX_MA.ReadFromRegister();
    processor.mem();
    WriteToPacket(MA_WB_IP);

    MA_WB.ReadFromRegister();
    processor.write_back();
    
    IF_DE.WriteToRegister(IF_DE_IP);
    DE_EX.WriteToRegister(DE_EX_IP);
    EX_MA.WriteToRegister(EX_MA_IP);
    MA_WB.WriteToRegister(MA_WB_IP);

    //------------------------STATS CALCULATION AND KNOBS------------------------

    if (EX_MA.op_code_pl) {
        instructions++;
        if ( (EX_MA.op_code_pl == 3) | (EX_MA.op_code_pl == 35) ) {
            data_instructions++;
        } else if ( (EX_MA.op_code_pl == 99) | (EX_MA.op_code_pl == 111) | (EX_MA.op_code_pl == 103) ) {
            control_instructions++;
        }
        if ( (EX_MA.op_code_pl != 99) & (EX_MA.op_code_pl != 35) ) {
            ALU_instructions++;
        }
        if (EX_MA.DepFlag_pl == 3) {instructions--;}
    }

    if (register_file) {showRegisters();}

    if (tracing) {
        if (!trace_instruction) {
            std::cout<<"--------------------------------------------------\n";
            std::cout<<"Tracing for clock cycle "<<processor.clock_cycle<<" (Instruction Number "<<instructions<<") :\n\n";
            std::cout<<"       PC OPCODE RD F3 RS1 RS2 F7 IMMI IMMS IMMB IMMU IMMJ LOADTYPE STORETYPE TAKEBRANCH MEMREAD MEMWRITE REGWRITE ALURES MEMADR LOADDATA\n";
            std::cout<<"IF-DE:"; IF_DE.Trace();
            std::cout<<"DE-EX:"; DE_EX.Trace();
            std::cout<<"EX-MA:"; EX_MA.Trace();
            std::cout<<"MA-WB:"; MA_WB.Trace();
        } else {
            if (trace_instruction == instructions) {
                IF_DE.Trace();
                DE_EX.Trace();
                EX_MA.Trace();
                MA_WB.Trace();
            }
        }
    }

    //------------------------END AND PRINT STATS------------------------
    
    if (MA_WB.op_code_pl == 127) {Stop();}

}

//If knob to print register file's values is TRUE,
//Shows values in register file after every cycle
void showRegisters() {
    std::cout<<"--------------------------------------------------\n";
    std::cout<<"Register values for clock cycle "<<processor.clock_cycle<<" :\n";
    for (int i=0; i<32; i++) {
        std::cout<<"Register "<<i<<" : "<<processor.reg[i]<< "\n";
    }
}

//Exits the simulator when last instruction is received
//Calculates and prints stats and exits program
void Core::Stop() {
    processor.store_memory();
    fclose(processor.programcode);
    printf("Clock Cycle %d finished.\n", processor.clock_cycle);
    std::cout<<"--------------------------------------------------\n";
    std::cout<<"--------------------------------------------------\n\n";
    std::cout<<"Feeding all memory to Data Memory.mc file.\n\n";
    std::cout<<"--------------------------------------------------\n";
    bubbles = data_bubbles + control_bubbles;
    CPI = (float) processor.clock_cycle / (float) instructions;
    std::cout<<"\nSTATS:\n";
    showStats();
    exit(0); //program run til completion
}

//Shows all stats at end of program
void Core::showStats() {
    std::cout<<" 1--> Total number of cycles                   : "<<processor.clock_cycle<<"\n";
    std::cout<<" 2--> Total instructions executed              : "<<instructions<<"\n";
    std::cout<<" 3--> CPI                                      : "<<CPI<<"\n";
    std::cout<<" 4--> Number of data transfer instructions     : "<<data_instructions<<"\n";
    std::cout<<" 5--> Number of ALU instructions               : "<<ALU_instructions<<"\n";
    std::cout<<" 6--> Number of Control instructions           : "<<control_instructions<<"\n";
    std::cout<<" 7--> Number of stalls in pipeline             : "<<bubbles<<"\n";
    std::cout<<" 8--> Number of data hazards                   : "<<data_hazards<<"\n";
    std::cout<<" 9--> Number of control hazards                : "<<control_hazards<<"\n";
    std::cout<<"10--> Number of branch mispredictions          : "<<wrong_predicts<<"\n";
    std::cout<<"11--> Number of bubbles due to data hazards    : "<<data_bubbles<<"\n";
    std::cout<<"12--> Number of bubbles due to control hazards : "<<control_bubbles<<"\n";
}