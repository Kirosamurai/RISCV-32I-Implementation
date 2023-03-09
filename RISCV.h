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

int instruction[32];
FILE* programcode;

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
            std::cout<<"Repositioning error";
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
