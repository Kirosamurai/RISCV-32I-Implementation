#include <string>
#include <map>
#include <iostream>

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
int immU = 0;
int immJ = 0;

//Control Lines
int ALUres = 0;
int LoadType = 0;
int StoreType = 0;
int TakeBranch = 0;
int MemRead = 0;
int MemWrite = 0;
int RegWrite = 0;

int op1;
int op2;

uint32_t MemAdr = 0;
uint32_t LoadData = 0;
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

