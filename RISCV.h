#include <string>

#define ADD_LEN 32
#define MEM_SIZE 0x10000

void run();
void reset_proc();
void load_memory(char* file_name, int N);
void write_data_mem();
void exit();

void fetch();
void decode();
void execute();
void mem();
void write_back();

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
int immU = 0;
int immJ = 0;

//Control Lines
int ALUres = 0;
int MemAdr = 0;
int LoadType = 0;
int StoreType = 0;
int TakeBranch = 0;
int MemRead = 0;
int MemWrite = 0;
int RegWrite = 0;

int op1;
int op2;

class RISCV{
    
    uint32_t reg[32];
    uint32_t pc = 0x0;

    int read_word(uint32_t addr, char* mem); //check later: do we need?
    void write_word(char *mem, uint32_t addr, uint32_t data); //check later: do we need?

public:
    RISCV(char* file_name, uint32_t mem_start);
    void run();
};

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


void fetch()
{   
    // what to do if pc location is invalid? send to end of program [program terminated.]
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
        if (fseek(programcode, 0, SEEK_SET)!= 0) 
        {
            std::cout<<"Repositioning error. Check File Handling!"; 
        }
        while (currentpc_number!=pc)
        {
            fscanf(programcode,"%s %s",currentpc, currentinstruction);
            currentpc_number=stringtohex(currentpc);
        }
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
}

//Decode
void decode(){
    //Decoding opcode
    for(int i=0; i<7; i++){
        op_code *= 2;
        op_code += instruction[i];
    }

    //Decoding rd
    for(int i=7; i<12; i++){
        rd *= 2;
        rd += instruction[i];
    }

    //Decoding f3
    for(int i=12; i<15; i++){
        f3 *= 2;
        f3 += instruction[i];
    }

    //Decoding rs1
    for(int i=15; i<20; i++){
        rs1 *= 2;
        rs1 += instruction[i];
    }

    //Decoding rs2
    for(int i=20; i<25; i++){
        rs2 *= 2;
        rs2 += instruction[i];
    }

    //Decoding f7
    for(int i=25; i<32; i++){
        f7 *= 2;
        f7 += instruction[i];
    }

    //Decoding ImmI
    for(int i=20; i<32; i++){
        immI *= 2;
        immI += instruction[i];
    }

    //Decoding ImmS
    for(int i=7; i<12; i++){
        immS *= 2;
        immS += instruction[i];
    }

    for(int i=25; i<32; i++){
        immS *= 2;
        immS += instruction[i];
    }

    //Decoding ImmB
    for(int i=8; i<12; i++){
        immB *= 2;
        immB += instruction[i];
    }

    for(int i=25; i<31; i++){
        immB *= 2;
        immB += instruction[i];
    }

    immB *= 2;
    immB += instruction[7];

    immB *= 2;
    immB += instruction[31];

    immB *= 2; //0th bit is always 0

    //Decoding ImmU
    for(int i=12; i<32; i++){
        immU *= 2;
        immU += instruction[i];
    }

    //Decoding ImmJ
    for(int i=20; i<31; i++){
        immJ *= 2;
        immJ += instruction[i];
    }
    
    immJ *= 2;
    immJ += instruction[11];

    for(int i=12; i<20; i++){
        immJ *= 2;
        immJ += instruction[i];
    }

    immJ *= 2;
    immJ += instruction[31];

    immJ *= 2; //0th bit is always 0
}

//Execute
//PC Values are changed in execute only
//All the if-else and switch statements make the ALU Control Unit
//All Control Lines Updated in Execute
void execute(){
    if(op_code == 51){
        op1 = reg[rs1];
        op2 = reg[rs2];
        RegWrite = 1;
        pc += 4;

        if(f3 == 0){
            if(f7 == 0){
                ALUres = op1 + op2;
            }
            if(f7 == 32){
                ALUres = op1 - op2;
            }
        }else if(f3 == 4){
            ALUres = op1 ^ op2;
        }else if(f3 == 6){
            ALUres = op1 | op2;
        }else if(f3 == 7){
            ALUres = op1 & op2;
        }else if(f3 == 1){
            ALUres = op1 << op2;
        }else if(f3 == 5){
            if(f7 == 0){
                ALUres = op1 >> op2;
            }
            if(f7 == 32){
                //ON HOLD
            }
        }else if(f3 == 2){
            ALUres = (op1 < op2)?1:0;
        }
    }else if(op_code == 19){
        op1 = reg[rs1];
        op2 = immI;
        RegWrite = 1;
        pc += 4;

        if(f3 == 0){
            ALUres = op1 + op2;
        }else if(f3 == 7){
            ALUres = op1 & op2;
        }else if(f3 == 6){
            ALUres = op1 | op2;
        }
    }else if(op_code == 3){
        op1 = reg[rs1];
        op2 = immI;
        
        MemAdr = op1 + op2;
        ALUres = 0;
        MemRead = 1;
        pc += 4;

        switch (f3){
        case 0:
            LoadType = 1;
            break;
        case 1:
            LoadType = 2;
            break;
        case 2:
            LoadType = 3;
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
            break;
        case 1:
            StoreType = 2;
            break;
        case 2:
            StoreType = 3;
            break;
        }
    }else if(op_code == 99){
        op1 = reg[rs1];
        op2 = reg[rs2];

        ALUres = op1 - op2;

        switch (f3){
        case 0:
            if(ALUres == 0){
                TakeBranch = 1;
                pc += immS;
            }else{
                TakeBranch = 0;
            }
            break;
        case 1:
            if(ALUres != 0){
                TakeBranch = 1;
                pc += immS;
            }else{
                TakeBranch = 0;
            }
            break;
        case 4:
            if(ALUres < 0){
                TakeBranch = 1;
                pc += immS;
            }else{
                TakeBranch = 0;
            }
            break;
        case 5:
            if(ALUres >= 0){
                TakeBranch = 1;
                pc += immS;
            }else{
                TakeBranch = 0;
            }
            break;
        }
    }else if(op_code == 55){
        ALUres = immU << 12;
        pc += 4;
    }else if(op_code == 23){
        ALUres = pc + (immU << 12);
    }else if(op_code == 111){
        ALUres = PC + 4;
        pc += immJ;
    }else if(op_code = 103){
        op1 = reg[rs1];
        ALUres = PC + 4;
        pc = op1 + immI;
    }
}

//int MemAdr = 0;
//MemAdr int or long?
// uint32_t MemAdr = 0;
std::map<uint32_t, uint8_t> memory;

//map<MemAdr, ByteMemory>
/*Since byte is the smallest unit of memory being loaded or stored, 
the map used in this program will have 1-byte addressing.*/

void mem()
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
                    if (memory.count(MemAdr)==0)
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
                    break;
                }
            //rd = M[rs1+imm][0:15] LH
            case 2:
                {
                    if (memory.count(MemAdr)==0)
                    {
                        LoadData+=0;
                        memory[MemAdr]=0;
                    }
                    else
                    {   
                        LoadData = memory[MemAdr];
                    }
                    if (memory.count(MemAdr+1)==0)
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
                    break;
                }
            //rd = M[rs1+imm][0:31] LW
            case 3: 
               {    
                    for (int i=0; i<4; i++)
                    {   
                        if (memory.count(MemAdr+i)==0)
                        {
                            memory[MemAdr+i]=0;
                        }
                        else
                        {   
                            LoadData += (memory[MemAdr+i]<<(8*i));
                        }
                    }
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
                break;
            }
            case 2: //M[rs1+imm][0:15] = rs2[0:15] SH
            {
                memory[MemAdr]=reg[rs2];
                memory[MemAdr+1]=(reg[rs2]>>8);
                break;
            }
            case 3: //M[rs1+imm][0:31] = rs2[0:31] SW
            {
                memory[MemAdr]=reg[rs2];
                memory[MemAdr+1]=(reg[rs2]>>8);
                memory[MemAdr+2]=(reg[rs2]>>16);
                memory[MemAdr+3]=(reg[rs2]>>24);
                break;
            }
            default: break;   
        }
    }
}

void write_back()
{   
    if (RegWrite == 1)
    {
        if(op_code == 3)
        {
            reg[rd] = LoadData;
        }
        else
        {
            reg[rd] = ALUres;
        }
    }
}

void reset_proc(){
    int ALUres = 0;
    int MemAdr = 0;
    int LoadType = 0;
    int StoreType = 0;
    int TakeBranch = 0;
    int MemRead = 0;
    int MemWrite = 0;
    int RegWrite = 0;
}
