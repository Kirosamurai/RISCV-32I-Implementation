#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>

#define ADD_LEN 32
#define MEM_SIZE 0x10000

int clock_cycle = 0;
int instruction[32];
FILE* programcode;

//ALU Variables
int op_code = 0;
int rd = 0;
int f3 = 0;
int rs1 = 0;
int rs2 = 0;
int f7 = 0;
int immI = 0;
int immS = 0;
int immB = 0;
long immU = 0;
int immJ = 0;

//Control Lines
int LoadType = 0;
int StoreType = 0;
int TakeBranch = 0;
int MemRead = 0;
int MemWrite = 0;
int RegWrite = 0;

int op1;
int op2;

uint32_t MemAdr = 0;
long LoadData = 0;
long ALUres = 0;
std::map<uint32_t, uint8_t> memory;

//map<MemAdr, ByteMemory>
/*Since byte is the smallest unit of memory being loaded or stored, 
the map used in this program will have 1-byte addressing.*/

class RISCV{

public:
    //constructor function
    RISCV()
    {
        pc = 0x0;
        //setting empty memory:
        memory.clear();
        //setting empty registers:
        for (int i= 0; i < ADD_LEN; i++) {
             reg[i] = 0x0;
        }
        reg[2] = 0x7FFFFFF0; //Stack Pointer
        reg[3] = 0x10000000; //Data Pointer
    }
    uint32_t reg[32];
    uint32_t pc;
    RISCV(char* file_name, uint32_t mem_start);
    void run();
    void fetch();
    void decode();
    void execute();
    void mem();
    void write_back();
};


void run();
void load_memory(char* file_name, int N);
void reset();
uint32_t stringtohex(char input[11]);
void store_memory(); 
void instruction_exit();

RISCV processor;

//run() starts the actual single cycle processor simulator
void run() {
  while(1) {
    clock_cycle++;
    processor.fetch();
    processor.decode();
    processor.execute();
    processor.mem();
    processor.write_back();
    printf("Clock Cycle %d finished.\n",clock_cycle);
    std::cout<<"--------------------------------------------------\n\n";
  }
}


//load_program_memory(char *file_name, int n) populates the instruction memory from file_name and stores value of N (required for all test files) to R3 in register file
void load_memory(char *file_name, int N) {
  
  //instruction memory:
  programcode = fopen(file_name, "r+");
  if(programcode == NULL) {
    printf("Error opening input file!\n");
    exit(1);
  }
  
  //storing value of N:
  processor.reg[3] = N;
}


void reset(){
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
void store_memory()
{
    std::map<uint32_t, uint8_t>::iterator it = memory.begin();
    fseek(programcode, 0, SEEK_END);
    fprintf(programcode, "\n");
    while (it != memory.end())
    {
        fprintf(programcode, "0x%0x 0x%0x\n",it->first, it->second);
        ++it;
    }
}

void instruction_exit()
{
  //Terminate Program and feed all memory into the .mc file
        store_memory();
        fclose(programcode);
        printf("Clock Cycle %d finished.\n",clock_cycle);
        std::cout<<"--------------------------------------------------\n\n";
        std::cout<<"Feeding all memory to .mc file.\nProgram took  "<<clock_cycle<<" clockcycles.\n";
        std::cout<<"ENDING SIMULATOR.\n";
        exit(0);
}    

//------------------------------------------FETCH()------------------------------------------
void RISCV::fetch()
{   
    // what does processor do if pc location is invalid?: sends to end of program [program terminated.]
    char currentpc[11];
    uint32_t currentpc_number;
    char currentinstruction[11];
    fscanf(programcode,"%s %s",currentpc, currentinstruction);
    currentpc_number=stringtohex(currentpc);
    if (currentpc_number==pc)
    {
        //instruction = currentinstruction
    }
    else if (currentpc_number>pc)
    {
        if (pc%4==0) //PC address should be valid [aligned addressing]
        {
            if (fseek(programcode, 0, SEEK_SET)!= 0) 
            {
                std::cout<<"Repositioning error. Check File Handling!\n"; 
            }
            while (currentpc_number!=pc)
            {   
                fscanf(programcode,"%s %s",currentpc, currentinstruction);
                currentpc_number = stringtohex(currentpc);
            }
        }
        else strcpy(currentinstruction, "0xffffffff");
    }
    else 
    {   
        while ((currentpc_number!=pc)&&(strcmp(currentinstruction,"0xffffffff")))
        {   
            fscanf(programcode,"%s %s",currentpc, currentinstruction);
            currentpc_number+=4;
        }
    }
    //upload currentinstruction to instruction[32]
    uint32_t bits=stringtohex(currentinstruction);
    for (int i=0; i<32; i++)
    {
        if (((1<<i)&bits)!=0)
        {
            instruction[i]=1;
        }
        else instruction[i]=0;
    }
    std::cout<<"FETCH: Fetch instruction "<<currentinstruction<<" from address "<<currentpc<<'\n';
    reset(); //to reset all control lines
}

//------------------------------------------DECODE()------------------------------------------
void RISCV::decode(){

    //Decoding opcode
    for(int i=0; i<7; i++){
        op_code += instruction[i]*(1<<i);
    }

    //Decoding rd
    for(int i=7; i<12; i++){
        rd += instruction[i]*(1<<(i-7));
    }

    //Decoding f3
    for(int i=12; i<15; i++){
        f3 += instruction[i]*(1<<(i-12));
    }

    //Decoding rs1
    for(int i=15; i<20; i++){
        rs1 += instruction[i]*(1<<(i-15));
    }

    //Decoding rs2
    for(int i=20; i<25; i++){
        rs2 += instruction[i]*(1<<(i-20));
    }

    //Decoding f7
    for(int i=25; i<32; i++){
        f7 += instruction[i]*(1<<(i-25));
    }

    //Decoding ImmI
    for(int i=20; i<32; i++){
        immI += instruction[i]*(1<<(i-20));
    }

    if(instruction[31] == 1){
        immI = -(4096 - immI);
    }

    //Decoding ImmS
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
    for(int i=12; i<32; i++){
        immU += instruction[i]*(1<<i);
    }

    if(instruction[31] == 1){
        immU = -(4294967254 - immU);
    }

    //Decoding ImmJ
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
    std::cout<<"DECODE: Decoded the instruction.\n";
}

//------------------------------------------EXECUTE()------------------------------------------
//PC Values are changed in execute only
//All the if-else and switch statements make the ALU Control Unit
//All Control Lines Updated in Execute
void RISCV::execute(){
    if(op_code == 51){
        op1 = reg[rs1];
        op2 = reg[rs2];
        RegWrite = 1;
        pc += 4;


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
    }else if(op_code == 19){
        op1 = reg[rs1];
        op2 = immI;
        RegWrite = 1;
        pc += 4;

        if(f3 == 0){
            ALUres = (long) op1 + op2;
            std::cout << "EXECUTE: Operation is ADDI, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 7){
            ALUres = op1 & op2;
            std::cout << "EXECUTE: Operation is ANDI, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is R" << rd << ".\n";
        }else if(f3 == 6){
            ALUres = op1 | op2;
            std::cout << "EXECUTE: Operation is ORI, First Operand is R" << rs1 << ", Second Operand is" << op2 << ", Destination is R" << rd << ".\n";
        }
    }else if(op_code == 3){
        op1 = reg[rs1];
        op2 = immI;
        
        MemAdr = op1 + op2;
        ALUres = 0;
        MemRead = 1;
        RegWrite = 1;
        pc += 4;

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
    }else if(op_code == 35){
        op1 = reg[rs1];
        op2 = immS;

        MemAdr = op1 + op2;
        ALUres = 0;
        MemWrite = 1;
        pc += 4;

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
    }else if(op_code == 99){
        op1 = reg[rs1];
        op2 = reg[rs2];

        ALUres = (long) op1 - op2;

        switch (f3){
        case 0:
            std::cout << "EXECUTE: Operation is BEQ, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres == 0){
                TakeBranch = 1;
                pc += immB;
            }else{
                TakeBranch = 0;
                pc += 4;
            }
            break;
        case 1:
            std::cout << "EXECUTE: Operation is BNE, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres != 0){
                TakeBranch = 1;
                pc += immB;
            }else{
                TakeBranch = 0;
                pc += 4;
            }
            break;
        case 4:
            std::cout << "EXECUTE: Operation is BLT, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres < 0){
                TakeBranch = 1;
                pc += immB;
            }else{
                TakeBranch = 0;
                pc += 4;
            }
            break;
        case 5:
            std::cout << "EXECUTE: Operation is BGE, First Operand is R" << rs1 << ", Second Operand is R" << rs2 << "\n";
            if(ALUres >= 0){
                TakeBranch = 1;
                pc += immB;
            }else{
                TakeBranch = 0;
                pc += 4;
            }
            break;
        }
    }else if(op_code == 55){
        std::cout << "EXECUTE: Operation is LUI, First Operand is " << immU << "\n";
        ALUres = immU;
        RegWrite = 1;
        pc += 4;
    }else if(op_code == 23){
        std::cout << "EXECUTE: Operation is AUIPC, First Operand is " << immU << "\n";
        ALUres = pc + immU;
        RegWrite = 1;
        pc += 4;
    }else if(op_code == 111){
        std::cout << "EXECUTE: Operation is JAL, First Operand is " << immJ << "\n";
        ALUres = pc + 4;
        pc += immJ;
        RegWrite = 1;
    }else if(op_code == 103){
        std::cout << "EXECUTE: Operation is JALR, First Operand is " << immI << "\n";
        op1 = reg[rs1];
        ALUres = pc + 4;
        pc = op1 + immI;
        RegWrite = 1;
    }else{
        instruction_exit();
    }
}

//------------------------------------------MEM()------------------------------------------
void RISCV::mem()
{
    if (MemRead==1)
    {   
        RegWrite = 1;
        LoadData = 0;
        switch (LoadType)
        {   
            //rd = M[rs1+imm][0:7] LB
            case 1: 
                {   
                    if (memory.find(MemAdr)==memory.end())
                    {
                        LoadData=0;
                        memory[MemAdr]=0;
                    }
                    else
                    {
                        LoadData = memory[MemAdr];
                        if ((LoadData&(1<<7))!=0)
                        {
                            LoadData+=0xFFFFFF00;
                        }
                    }
                    std::cout<<"MEMORY: Load 1 Byte of Memory Value "<<LoadData<<" from address "<<MemAdr<<'\n';
                    break;
                }
            //rd = M[rs1+imm][0:15] LH
            case 2:
                {
                    if (memory.find(MemAdr)==memory.end())
                    {
                        LoadData+=0;
                        memory[MemAdr]=0;
                    }
                    else
                    {   
                        LoadData = memory[MemAdr];
                    }
                    if (memory.find(MemAdr+1)==memory.end())
                    {
                        memory[MemAdr+1]=0;
                    }
                    else
                    {
                        LoadData = LoadData + (memory[MemAdr+1]<<8);
                        if ((LoadData&(1<<15))!=0)
                        {
                            LoadData+=0xFFFF0000;
                        }
                    }
                    std::cout<<"MEMORY: Load 2 Bytes of Memory Value "<<LoadData<<" from address "<<MemAdr<<'\n';
                    break;
                }
            //rd = M[rs1+imm][0:31] LW
            case 3: 
               {    
                    int loop;
                    for (loop=0; loop<4; loop++)
                    {   
                        if ((memory.find(MemAdr+loop))==(memory.end()))
                        {   
                            memory[MemAdr+loop]=0;
                        }
                        else
                        {   
                            LoadData += (memory[MemAdr+loop]<<(8*loop));
                        }
                    }
                    //if ((memory[MemAdr+loop]&(1<<7))!=0)
                    if (LoadData>2147483647)
                    {
                        //Data to be loaded is negative.
                        LoadData = -(4294967294 - LoadData);
                    }
                   std::cout<<"MEMORY: Load 4 Bytes of Memory Value "<<LoadData<<" from address "<<MemAdr<<'\n';
                   break; 
                }
            default: break;   
        }
    }
    else if (MemWrite==1)
    {
        switch (StoreType)
        {
            case 1: //M[rs1+imm][0:7] = rs2[0:7] SB
            {
                memory[MemAdr]=reg[rs2];
                std::cout<<"MEMORY: Store 1 Byte of Memory Value "<<reg[rs2]<<" to address "<<MemAdr<<'\n';
                break;
            }
            case 2: //M[rs1+imm][0:15] = rs2[0:15] SH
            {
                memory[MemAdr]=reg[rs2];
                memory[MemAdr+1]=(reg[rs2]>>8);
                std::cout<<"MEMORY: Store 2 Bytes of Memory Value "<<reg[rs2]<<" to address "<<MemAdr<<'\n';
                break;
            }
            case 3: //M[rs1+imm][0:31] = rs2[0:31] SW
            {
                memory[MemAdr]=reg[rs2];
                memory[MemAdr+1]=(reg[rs2]>>8);
                memory[MemAdr+2]=(reg[rs2]>>16);
                memory[MemAdr+3]=(reg[rs2]>>24);
                std::cout<<"MEMORY: Store 4 Bytes of Memory Value "<<reg[rs2]<<" to address "<<MemAdr<<'\n';
                break;
            }
            default: break;   
        }
    }
    else
    {
       std::cout<<"MEMORY: No memory operation.\n";
    }
}

//------------------------------------------WRITE_BACK()------------------------------------------
void RISCV::write_back()
{   
    if (RegWrite == 1)
    {   
        if (rd!=0)
        {
            if(op_code == 3)
            {
                reg[rd] = LoadData;
                std::cout<<"WRITEBACK: Write "<<LoadData<<" to R"<<rd<<"\n";
            }
            else
            {
                reg[rd] = ALUres;
                std::cout<<"WRITEBACK: Write "<<ALUres<<" to R"<<rd<<"\n";
            }
        }
        else std::cout<<"WRITEBACK: Write 0 to R0\n";
    }
    else
    {
        std::cout<<"WRITEBACK: No register writeback operation.\n";
    }
}
