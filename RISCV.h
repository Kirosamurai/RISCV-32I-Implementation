#include <string>
#include <map>
#include <iostream>

#define ADD_LEN 32
#define MEM_SIZE 0x10000

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

