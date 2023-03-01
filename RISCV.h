#include <string>

#define ADD_LEN 32
#define MEM_SIZE 0x10000

void run();
void reset_proc();
void load_memory(char* file_name);
void write_data_mem();
void exit();

void fetch();
void decode();
void execute();
void mem();
void write_back();


class RISCV{
    uint8_t mem[ADD_LEN];
    uint32_t mem_start = 0x0;
    
    uint32_t reg[32];
    uint32_t pc = 0x0;
    uint32_t pc_incr = 0x0;

    int read_word(uint32_t addr, char* mem);
    void write_word(char *mem, uint32_t addr, uint32_t data);

public:
    RISCV(char* file_name, uint32_t mem_start);
    void run();
};