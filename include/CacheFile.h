#ifndef CACHE_FILE_H_
#define CACHE_FILE_H_

#include <iostream>
#include <vector>
#include <cmath>

#include "RISCV.h"
#include "Pipeline.h"

using namespace std;

#define MAX_SET_NUM 500
#define MAX_ASSOCIATIVITY 1024

class Cache {
public:
    int cache_size;
    int block_size;
    int block_num;

    int set_num;
    int associativity;

    vector<uint32_t> mct;

    /*
    TAG ARRAY IS OF THE FORM:
    tag_array[index][way number] = tag

    DATA ARRAY IS OF THE FORM:
    data_array[index][0][way number] : stores validity bit
    data_array[index][1][way number] : stores dirty bit
    data_array[index][2][way number] : stores recency bits
    data_array[index][3][way number] : stores data
    */
    
    std::string tag_array[MAX_SET_NUM][MAX_ASSOCIATIVITY];
    std::string data_array[MAX_SET_NUM][4][MAX_ASSOCIATIVITY];

    /* 
    replace = 1 : LRU
    replace = 2 : FIFO
    replace = 3 : RANDOM
    replace = 4 : LFU
    */
    int replace;

    //if 1, Cache is I$ ; if 2, Cache is D$
    int whichCache;

    //direct mapped and fully associative constructor
    void init(int sizeCache, int sizeBlock, bool isDirectMapped) {
        cache_size = sizeCache;
        block_size = sizeBlock;
        block_num = cache_size/block_size;

        if (isDirectMapped) {
            associativity = 1;
            set_num = block_num;
        } else {
            associativity = block_num;
            set_num = 1;
        }

        recency_bits = log2(associativity);
        offset_bits = log2(block_size);
        index_bits = log2(set_num);
        tag_bits = 32 - index_bits - offset_bits;

        offset_num = 0;
        
    }

    //set asociative constructor
    void init(int sizeCache, int sizeBlock, int ways) {
        cache_size = sizeCache;
        block_size = sizeBlock;
        block_num = cache_size/block_size;

        associativity = ways;
        set_num = block_num/associativity;

        recency_bits = log2(associativity);
        offset_bits = log2(block_size);
        index_bits = log2(set_num);
        tag_bits = 32 - index_bits - offset_bits;

        offset_num = 0;
        
    }

    int tag_bits;
    int index_bits;
    int offset_bits;
    int recency_bits;

    int offset_num;

    std::string tag;
    std::string index;
    std::string offset;

    
    
    //returns TRUE for hit, FALSE for miss
    bool isPresent(uint32_t address);
    //stores value of correct way if HIT
    int thisWay;

    //function that uploads the block stored in main memory to cache
    //called inside allocate()
    void mainMemoryLoader(int whichCache, uint32_t mem_address, int index, int way);
    // function that converts a string recency_bits to int value
    int recencyTranslateVal(int index, int way);
    // function that converts a int value into recency_bits and stores it at given location
    void recencyAssignVal(int index, int way, int value);
    // function that checks if the victim is dirty. if dirty, the contents of the victim block are uploaded to main memory.
    void dirtyVictim(int index, int way);
    
    // IF MISS: choose and kick victim, retreive from main memory, update recency
    void allocate(uint32_t mem_address);

    //IF HIT: update recency, then read or write
    void recencyUpdater(std::string index, int way);
    void write(uint8_t data);
    uint8_t read();
    uint32_t readI();
    
    // function that makes offset bits to 0 from an address
    uint32_t noOffset(uint32_t address);
    
    //Type of miss
    int miss_type(uint32_t mem_address);

    //Stats!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int cold_miss = 0;
    int capacity_miss = 0;
    int conflict_miss = 0;
    int hits = 0;
    int memory_stalls = 0;
    int number_of_accesses = 0;

    int penalty;
};

void ByteWriter(int memadd);
void ByteReader(int memadd);

extern Cache I;
extern Cache D;

class RISCV;

extern RISCV processor;

#endif
