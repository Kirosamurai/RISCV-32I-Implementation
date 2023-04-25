#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>

#include <string>

#include "RISCV.h"
#include "Pipeline.h"
#include "CacheFile.h"
#include "Core.h"

void RISCV::instructionMainMemoryUpload() {
    char currentpc[11];
    char currentinstruction[11];
    uint32_t currentpc_number;
    uint32_t bits;
    while (fscanf(programcode, "%s %s", currentpc, currentinstruction) != EOF) {
        currentpc_number = stringtohex(currentpc);
        bits = stringtohex(currentinstruction);
        instruction_memory[currentpc_number] = bits;
        std::cout<<instruction_memory[currentpc_number]<<endl;
    }
}

void RISCV::createCache() {
    //I$:
    if (proc_i_mapping == 2) {I.init(proc_i_sizeCache, proc_i_sizeBlock, proc_i_ways);}
    else {I.init(proc_i_sizeCache, proc_i_sizeBlock, proc_i_isDirect);}
    I.whichCache = 1;
    I.replace = proc_i_replacement;
    I.penalty = proc_i_penalty;
    //D$:
    if (proc_d_mapping == 2) {D.init(proc_d_sizeCache, proc_d_sizeBlock, proc_d_ways);}
    else {D.init(proc_d_sizeCache, proc_d_sizeBlock, proc_d_isDirect);}
    D.whichCache = 2;
    D.replace = proc_d_replacement;
    D.penalty = proc_d_penalty;
}

void RISCV::reset(){
    ALUres = 0;
    MemAdr = 0;
    LoadType = 0;
    StoreType = 0;
    TakeBranch = 0;
    MemRead = 0;
    MemWrite = 0;
    RegWrite = 0;
    op_code = 0;
    rd = 0;
    f3 = 0;
    rs1 = 0;
    rs2 = 0;
    f7 = 0;
    immI = 0;
    immS = 0;
    immB = 0;
    immU = 0;
    immJ = 0;
    op1 = 0;
    op2 = 0;
    LoadData=0;
    MemAdr = 0;
}

//Function to convert a string hexadecimal to respective integer decimal.
uint32_t stringtohex(char input[11])
{
    uint32_t answer=0x0;
    int base=1;
    for (int i=2; ; i++)
    {
        if (input[i]>='0' && input[i]<='9') 
        {
            answer = answer*16 + (input[i] - 48);
            base = base * 16;
        }
        else if (input[i]>='a' && input[i]<='f') 
        {
            answer = answer*16 + (input[i] - 87);
            base = base * 16;
        }
        else break;
    }
    return answer;
}

//Function to store all map memory values into the .mc program file.
void RISCV::store_memory()
{
    std::map<uint32_t, uint8_t>::iterator it = memory.begin();
    FILE* out = fopen("../bin/Data Memory.mc", "w");
    fseek(out, 0, SEEK_SET);
    fprintf(out, "\n");
    while (it != memory.end())
    {
        fprintf(out, "0x%0x 0x%0x\n",it->first, it->second);
        ++it;
    }
    fclose(out);
}

void RISCV::instruction_exit()
{
  //Terminate Program and feed all memory into the .mc file
    store_memory();
    fclose(programcode);
    printf("Clock Cycle %d finished.\n",clock_cycle);
    std::cout<<"--------------------------------------------------\n\n";
    std::cout<<"Feeding all memory to Data Memory.mc file.\nProgram took  "<<clock_cycle<<" clockcycles.\n";
    std::cout<<"ENDING SIMULATOR.\n";

    //------------------------------------------------------
    //json file code
    {
    jsonStringAdder('{', ' ');
    jsonStringAdder("number",clock_cycle);
    jsonStringAdder(',',' ');
    jsonStringAdder("IFpc","EOF");
    jsonStringAdder(',',' ');
    jsonStringAdder("IFhexcode","EOF");
    jsonStringAdder(' ', '}');
    jsonStringAdder(']',',');
    jsonStringAdder("totalCycles",clock_cycle);
    jsonStringAdder('\n','}');
    fclose(output);
    }
    //json file code ends
    //------------------------------------------------------
    exit(0); //program run to completion
}    

//------------------------------------------FETCH()------------------------------------------
void RISCV::fetch()
{       
    
    if (cache) {
        
        bool hit = false;
        // what does processor do if pc location is not aligned?: sends to end of program [program terminated.]
        // if pc location is invalid: 0000000 is inserted [a bubble].
        uint32_t bits;
        char currentinstruction[11];
        if (pc%4 != 0) 
        {
            bits = 0xffffffff;
        }
        else
        {   
            hit = I.isPresent(pc);
            if (hit) {
                I.hits++;
                I.recencyUpdater(I.index, I.thisWay);
            } else {
                int miss = I.miss_type(MemAdr);
                switch (miss){
                case 1:
                    I.cold_miss++;
                    break;
                case 2:
                    I.conflict_miss++;
                    break;
                case 3:
                    I.capacity_miss++;
                    break;
                }

                printf("----------------------------------------------------------\n");
                printf("--------------MISS PENALTY, PROCESSOR STALLED-------------\n");
                printf("----------------------------------------------------------\n");

                clock_cycle += I.penalty;
                I.memory_stalls += I.penalty;
                
                I.allocate(pc);
            }
            I.offset_num = 0;
            bits = I.readI();
        }

        if (bits == 0)
        {
            for (int i=0; i<32; i++) {
                instruction[i] = 0;
            }
            DepFlag = 0;
        }
        else
        {
            for (int i=0; i<32; i++) {
                if (((1<<i)&bits)!=0) {
                    instruction[i] = 1;
                } else {
                    instruction[i]=0;
                }
            }
            std::cout<<"FETCH: Fetch instruction "<<bits<<" from address "<<pc<<'\n';
        }



    }
    
    else {

    // what does processor do if pc location is invalid?: sends to end of program [program terminated.]
    // bool flag = false;
    char currentpc[11];
    uint32_t currentpc_number;
    char currentinstruction[11];
    
    bool flag;
    if (pc%4 != 0) {
        strcpy(currentinstruction, "0xffffffff");
    } else {
        fseek(programcode, 0, SEEK_SET);
        flag = true;
        while (flag) {
            if (fscanf(programcode, "%s %s", currentpc, currentinstruction) != EOF) {;
                currentpc_number = stringtohex(currentpc);
                if (currentpc_number == pc) {
                    flag = false;
                }
            } else {
                break;
            }
        }
    }

    //Uploading instruction 
    if (flag) {
        for (int i=0; i<32; i++) {
            instruction[i] = 0;
        }
        DepFlag = 0;
    } else {
        uint32_t bits=stringtohex(currentinstruction);
        for (int i=0; i<32; i++) {
            if (((1<<i)&bits)!=0) {
                instruction[i] = 1;
            } else {
                instruction[i]=0;
            }
        }
        std::cout<<"FETCH: Fetch instruction "<<currentinstruction<<" from address "<<currentpc<<'\n';
    }
    
    }
    reset(); //to reset all control lines
}

//------------------------------------------DECODE()------------------------------------------
void RISCV::decode(){

    //Decoding opcode
    op_code = 0;
    for(int i=0; i<7; i++){
        op_code += instruction[i]*(1<<i);
    }

    //Decoding rd
    rd = 0;
    for(int i=7; i<12; i++){
        rd += instruction[i]*(1<<(i-7));
    }

    //Decoding f3
    f3 = 0;
    for(int i=12; i<15; i++){
        f3 += instruction[i]*(1<<(i-12));
    }

    //Decoding rs1
    rs1 = 0;
    for(int i=15; i<20; i++){
        rs1 += instruction[i]*(1<<(i-15));
    }

    //Decoding rs2
    rs2 = 0;
    for(int i=20; i<25; i++){
        rs2 += instruction[i]*(1<<(i-20));
    }

    //Decoding f7
    f7 = 0;
    for(int i=25; i<32; i++){
        f7 += instruction[i]*(1<<(i-25));
    }

    //Decoding ImmI
    immI = 0;
    for(int i=20; i<32; i++){
        immI += instruction[i]*(1<<(i-20));
    }

    if(instruction[31] == 1){
        immI = -(4096 - immI);
    }

    //Decoding ImmS
    immS = 0;
    for(int i=7; i<12; i++){
        immS += instruction[i]*(1<<(i-7));
    }

    for(int i=25; i<32; i++){
        immS += instruction[i]*(1<<(i-20));
    }

    if(instruction[31] == 1){
        immS = -(4096 - immS);
    }

    //Decoding ImmB
    immB = 0;
    for(int i=8; i<12; i++){
        immB += instruction[i]*(1<<(i-7));
    }

    for(int i=25; i<31; i++){
        immB += instruction[i]*(1<<(i-20));
    }

    immB += instruction[7]*(1<<11);
    
    immB += instruction[31]*(1<<12);

    if(instruction[31] == 1){
        immB = -(8192 - immB);
    }

    //Decoding ImmU
    immU = 0;
    for(int i=12; i<32; i++){
        immU += instruction[i]*(1<<i);
    }

    if(instruction[31] == 1){
        immU = -(4294967254 - immU);
    }

    //Decoding ImmJ
    immJ = 0;
    for(int i=21; i<31; i++){
        immJ += instruction[i]*(1<<(i-20));
    }
    
    immJ += instruction[20]*(1<<11);

    for(int i=12; i<20; i++){
        immJ += instruction[i]*(1<<i);
    }

    immJ += instruction[31]*(1<<20);

    if(instruction[31] == 1){
        immJ = -(2097152 - immJ);
    }

    if (op_code == 127) {
        if (pipeline) {std::cout<<"DECODE: (Exit instruction)\n";}
        else {instruction_exit();}
    
    } else if (op_code != 0) {
        std::cout<<"DECODE: Decoded the instruction ";
        for (int i=0; i<32; i++) {
            std::cout<< instruction[i];
        }
        std::cout<<"\n";
    
    } else if ( (op_code == 0) & (processor.DepFlag == 2) ) {
        std::cout<<"DECODE: Data dependency bubble!\n";
    
    } else if ( (op_code == 0) & (processor.DepFlag == 1) ) {
        std::cout<<"DECODE: Control dependency bubble!\n";
    }
}

//------------------------------------------EXECUTE()------------------------------------------
//New PC value is stored in a variable here
//All the if-else and switch statements make the ALU Control Unit
//All Control Lines Updated in Execute

void RISCV::execute(){

    //R type
    if(op_code == 51){
        if(!forward){
            op1 = reg[rs1];
            op2 = reg[rs2];
        }
        RegWrite = 1;
        new_pc = pc + 4;


        if(f3 == 0){
            if(f7 == 0){
                ALUres = (long) op1 + op2;
                std::cout << "EXECUTE: Operation is ADD, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
            }
            if(f7 == 32){
                ALUres = (long) op1 - op2;
                std::cout << "EXECUTE: Operation is SUB, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
            }
        }else if(f3 == 4){
            ALUres = op1 ^ op2;
            std::cout << "EXECUTE: Operation is XOR, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 6){
            ALUres = op1 | op2;
            std::cout << "EXECUTE: Operation is OR, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 7){
            ALUres = op1 & op2;
            std::cout << "EXECUTE: Operation is AND, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 1){
            ALUres = op1 << op2;
            std::cout << "EXECUTE: Operation is SLL, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 5){
            if(f7 == 0){
                ALUres = (int)((unsigned int)op1 >> op2);
                std::cout << "EXECUTE: Operation is SRL, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
            }
            if(f7 == 32){
                ALUres = op1 >> op2;
                std::cout << "EXECUTE: Operation is SRA, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
            }
        }else if(f3 == 2){
            ALUres = (op1 < op2)?1:0;
            std::cout << "EXECUTE: Operation is SLT, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << ", Destination is R" << rd << ".\n";
        }

    // I type
    }else if(op_code == 19){
        if(!forward) {op1 = reg[rs1];}
        op2 = immI;
        RegWrite = 1;
        new_pc = pc + 4;

        if(f3 == 0){
            ALUres = (long) op1 + op2;
            std::cout << "EXECUTE: Operation is ADDI, First Operand is R" << rs1 << ", Second Operand is " << op2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 7){
            ALUres = op1 & op2;
            std::cout << "EXECUTE: Operation is ANDI, First Operand is R" << rs1 << ", Second Operand is " << op2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 6){
            ALUres = op1 | op2;
            std::cout << "EXECUTE: Operation is ORI, First Operand is R" << rs1 << ", Second Operand is " << op2 << ", Destination is R" << rd << ".\n";
        }

    //LOAD type
    }else if(op_code == 3){
        if(!forward) {op1 = reg[rs1];}
        op2 = immI;
        
        MemAdr = op1 + op2;
        ALUres = 0;
        MemRead = 1;
        RegWrite = 1;
        new_pc = pc + 4;

        switch (f3){
        case 0:
            LoadType = 1;
            std::cout << "EXECUTE: Operation is LB, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is Mem[" << MemAdr << "].\n";
            break;
        case 1:
            LoadType = 2;
            std::cout << "EXECUTE: Operation is LH, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is Mem[" << MemAdr << "].\n";
            break;
        case 2:
            LoadType = 3;
            std::cout << "EXECUTE: Operation is LW, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is Mem[" << MemAdr << "].\n";
            break;
        }

    //STORE type
    }else if(op_code == 35){
        if(!forward) {op1 = reg[rs1];}
        op2 = immS;

        MemAdr = op1 + op2;
        ALUres = 0;
        MemWrite = 1;
        new_pc = pc + 4;

        switch (f3){
        case 0:
            StoreType = 1;
            std::cout << "EXECUTE: Operation is SB, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is Mem[" << MemAdr << "].\n";
            break;
        case 1:
            StoreType = 2;
            std::cout << "EXECUTE: Operation is SH, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is Mem[" << MemAdr << "].\n";
            break;
        case 2:
            StoreType = 3;
            std::cout << "EXECUTE: Operation is SW, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is Mem[" << MemAdr << "].\n";
            break;
        }

    //B type
    }else if(op_code == 99){

        if(!forward){
            op1 = reg[rs1];
            op2 = reg[rs2];
        }

        ALUres = op1 - op2;

        switch (f3){
        case 0:
            std::cout << "EXECUTE: Operation is BEQ, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres == 0){
                TakeBranch = 1;
                new_pc = pc + immB;
            }else{
                TakeBranch = 0;
                new_pc = pc + 4;
            }
            break;
        case 1:
            std::cout << "EXECUTE: Operation is BNE, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres != 0){
                TakeBranch = 1;
                new_pc = pc + immB;
            }else{
                TakeBranch = 0;
                new_pc = pc + 4;
            }
            break;
        case 4:
            std::cout << "EXECUTE: Operation is BLT, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres < 0){
                TakeBranch = 1;
                new_pc = pc + immB;
            }else{
                TakeBranch = 0;
                new_pc = pc + 4;
            }
            break;
        case 5:
            std::cout << "EXECUTE: Operation is BGE, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres >= 0){
                TakeBranch = 1;
                new_pc = pc + immB;
            }else{
                TakeBranch = 0;
                new_pc = pc + 4;
            }
            break;
        }

    //LUI
    }else if(op_code == 55){
        std::cout << "EXECUTE: Operation is LUI, First Operand is " << immU << "\n";
        ALUres = immU;
        RegWrite = 1;
        new_pc = pc + 4;
    
    //AUIPC
    }else if(op_code == 23){
        std::cout << "EXECUTE: Operation is AUIPC, First Operand is " << immU << "\n";
        ALUres = pc + immU;
        RegWrite = 1;
        new_pc = pc + 4;
    
    //JAL
    }else if(op_code == 111){
        std::cout << "EXECUTE: Operation is JAL, First Operand is " << immJ << "\n";
        ALUres = pc + 4;
        new_pc = pc + immJ;
        RegWrite = 1;
    
    //JALR
    }else if(op_code == 103){
        std::cout << "EXECUTE: Operation is JALR, First Operand is " << immI << "\n";
        op1 = reg[rs1];
        ALUres = pc + 4;
        new_pc = op1 + immI;
        RegWrite = 1;
    
    } else if (op_code == 127) {
        std::cout<< "EXECUTE: (Exit instruction)\n";
        new_pc = pc + 4;
    
    } else if( (op_code == 0) & (processor.DepFlag == 2) ){
        std::cout << "EXECUTE: Data dependency bubble!\n";
        new_pc = pc + 4;
    
    } else if( (op_code == 0) & (processor.DepFlag == 1) ){
        std::cout << "EXECUTE: Control dependency bubble!\n";
        new_pc = pc + 4;
    
    } else{
        new_pc = pc + 4;
    }

}

//------------------------------------------MEM()------------------------------------------
void RISCV::mem()
{
    
    if (MemRead==1)
    {   
        std::cout<<"reading from mem\n";
        RegWrite = 1;
        LoadData = 0;
        switch (LoadType) {   
            
            //rd = M[rs1+imm][0:7] LB
            case 1:    
            
            if (cache) {
                LoadData = 0;
                ByteReader(MemAdr);
                LoadData += (D.read());
            } else {
                if (memory.find(MemAdr)==memory.end()) {
                    LoadData=0;
                    memory[MemAdr]=0;
                } else {
                    LoadData = memory[MemAdr];
                    if ((LoadData&(1<<7))!=0) {LoadData+=0xFFFFFF00;}
                }
            }
            
            std::cout<<"MEMORY: Load 1 Byte of Memory Value "<<LoadData<<" from address "<<MemAdr<<'\n';
            break;
                
            //rd = M[rs1+imm][0:15] LH
            case 2:
            
            if (cache) {
                LoadData = 0;
                for (int i=0 ; i<2 ; i++) {
                    ByteReader(MemAdr+i);
                    LoadData += (D.read()<<8*i);
                }
            } else {
                if (memory.find(MemAdr)==memory.end()) {
                    LoadData+=0;
                    memory[MemAdr]=0;
                } else {   
                    LoadData = memory[MemAdr];
                }
                if (memory.find(MemAdr+1)==memory.end()) {
                    memory[MemAdr+1]=0;
                } else {
                    LoadData = LoadData + (memory[MemAdr+1]<<8);
                    if ((LoadData&(1<<15))!=0) {LoadData+=0xFFFF0000;}
                }
            }
            
            std::cout<<"MEMORY: Load 2 Bytes of Memory Value "<<LoadData<<" from address "<<MemAdr<<'\n';
            break;

            //rd = M[rs1+imm][0:31] LW
            case 3:   
            
            if (cache) {
                LoadData = 0;
                for (int i=0 ; i<4 ; i++) {
                    ByteReader(MemAdr+i);
                    LoadData += (D.read()<<8*i);
                }
                
            } else {
                int loop;
                for (loop=0; loop<4; loop++) {   
                    if ((memory.find(MemAdr+loop))==(memory.end())) {   
                        memory[MemAdr+loop]=0;
                    } else {   
                        LoadData += (memory[MemAdr+loop]<<(8*loop));
                    }
                }
                //if ((memory[MemAdr+loop]&(1<<7))!=0)
                if (LoadData>2147483647) {
                    //Data to be loaded is negative.
                    LoadData = -(4294967294 - LoadData);
                }
            }

            std::cout<<"MEMORY: Load 4 Bytes of Memory Value "<<LoadData<<" from address "<<MemAdr<<'\n';
            break; 
            
            default: break;   
        }

        std::cout<<"loaded value: "<< LoadData<<endl;
    
    std::cout<<"reading from mem OVER\n";
    
    } else if (MemWrite==1) {

        std::cout<<"writing to mem\n";
    
        switch (StoreType) {
            
            case 1: //M[rs1+imm][0:7] = rs2[0:7] SB
            if (cache) {
                ByteWriter(MemAdr);
                D.write(reg[rs2]);
                memory[MemAdr] = (reg[rs2]);
                int index_num=0;
                for (int i=D.index_bits-1; i>=0; i--) {
                    index_num += (D.index[i]-'0') * pow(2,D.index_bits-i-1);
                }
                std::cout<<"data stored: "<<D.data_array[index_num][3][D.thisWay]<<endl;
                std::cout<<"writing to mem OVER\n";
            } else {
                memory[MemAdr]=reg[rs2];
                std::cout<<"MEMORY: Store 1 Byte of Memory Value "<<reg[rs2]<<" to address "<<MemAdr<<'\n';
                std::cout<<"stored value: "<<memory[MemAdr]<<endl;
                break;
            }
            
            case 2: //M[rs1+imm][0:15] = rs2[0:15] SH
            if (cache) {
                for (int i=0 ; i<2 ; i++) {
                    ByteWriter(MemAdr+i);
                    D.write(reg[rs2]>>(8*i));
                    memory[MemAdr+i] = (reg[rs2]>> 8*i);
                }
                int index_num=0;
                for (int i=D.index_bits-1; i>=0; i--) {
                    index_num += (D.index[i]-'0') * pow(2,D.index_bits-i-1);
                }
                std::cout<<"data stored: "<<D.data_array[index_num][3][D.thisWay]<<endl;
                std::cout<<"writing to mem OVER\n";
            } else {
                memory[MemAdr]=reg[rs2];
                memory[MemAdr+1]=(reg[rs2]>>8);
                std::cout<<"MEMORY: Store 2 Bytes of Memory Value "<<reg[rs2]<<" to address "<<MemAdr<<'\n';
                std::cout<<"stored value: "<<memory[MemAdr]<<endl;
                break;
            }
            
            case 3: //M[rs1+imm][0:31] = rs2[0:31] SW
            if (cache) {
                
                for (int i=0 ; i<4 ; i++) {
                    ByteWriter(MemAdr+i);
                    D.write(reg[rs2]>>(8*i));
                    memory[MemAdr+i] = (reg[rs2]>> 8*i);
                }
                int index_num=0;
                for (int i=D.index_bits-1; i>=0; i--) {
                    index_num += (D.index[i]-'0') * pow(2,D.index_bits-i-1);
                }
                std::cout<<"data stored: "<<D.data_array[index_num][3][D.thisWay]<<endl;
                std::cout<<"writing to mem OVER\n";
            
            } else {
            
                memory[MemAdr]=reg[rs2];
                memory[MemAdr+1]=(reg[rs2]>>8);
                memory[MemAdr+2]=(reg[rs2]>>16);
                memory[MemAdr+3]=(reg[rs2]>>24);
                std::cout<<"stored value: "<<memory[MemAdr]<<endl;
                std::cout<<"writing to mem OVER\n";
            }
            std::cout<<"MEMORY: Store 4 Bytes of Memory Value "<<reg[rs2]<<" to address "<<MemAdr<<'\n';
            break;

            
            default: break;   
        }
    }
    
    else if (op_code == 127) {
        std::cout<<"MEMORY: (Exit instruction)\n";
    } else if (op_code != 0) {
       std::cout<<"MEMORY: No memory operation.\n";
    } else if ( (op_code == 0) & (processor.DepFlag == 3) ) {
        std::cout<<"MEMORY: Memory stall!\n";
    } else if ( (op_code == 0) & (processor.DepFlag == 2) ) {
        std::cout<<"MEMORY: Data dependency bubble!\n";
    } else if ( (op_code == 0) & (processor.DepFlag == 1) ) {
        std::cout<<"MEMORY: Control dependency bubble!\n";
    }
}

//------------------------------------------WRITE_BACK()------------------------------------------
void RISCV::write_back() {   

    if (RegWrite == 1) {   
        if (rd!=0) {
            if(op_code == 3) {
                reg[rd] = LoadData;
                std::cout<<"WRITEBACK: Write "<<LoadData<<" to R"<<rd<<"\n";
            } else {
                reg[rd] = ALUres;
                std::cout<<"WRITEBACK: Write "<<ALUres<<" to R"<<rd<<"\n";
            }
        }
        else std::cout<<"WRITEBACK: Write 0 to R0\n";
    
    } else if (op_code != 0) {
       std::cout<<"WRITEBACK: No register write back operation.\n";
    } else if ( (op_code == 0) & (processor.DepFlag == 3) ) {
        std::cout<<"WRITEBACK: Memory stall!\n";
    } else if ( (op_code == 0) & (processor.DepFlag == 2) ) {
        std::cout<<"WRITEBACK: Data dependency bubble!\n";
    } else if ( (op_code == 0) & (processor.DepFlag == 1) ) {
        std::cout<<"WRITEBACK: Control dependency bubble!\n";
    }
    
}
