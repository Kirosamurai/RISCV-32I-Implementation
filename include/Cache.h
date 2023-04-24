#ifndef CACHE_H_
#define CACHE_H_

#include <iostream>
#include <vector>

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
    Cache(int sizeCache, int sizeBlock, bool isDirectMapped) {
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
    }

    //set asociative constructor
    Cache(int sizeCache, int sizeBlock, int ways) {
        cache_size = sizeCache;
        block_size = sizeBlock;
        block_num = cache_size/block_size;

        associativity = ways;
        set_num = block_num/associativity;
    }

    int recency_bits = log2(associativity);
    int offset_bits = log2(block_size);
    int index_bits = log2(set_num);
    int tag_bits = 32 - index_bits - offset_bits;

    std::string tag;
    std::string index;
    std::string offset;
    
    //returns TRUE for hit, FALSE for miss
    bool isPresent(uint32_t address);
    //stores value of correct way if HIT
    int thisWay;

    //function that returns the data stored in main memory
    //called inside allocate()
    std::string mainMemoryLoader(int whichCache, uint32_t mem_address);
    
    // IF MISS: choose and kick victim, retreive from main memory, update recency
    void allocate(uint32_t mem_address);

    //IF HIT: update recency, then read or write
    void recencyUpdater(int index, int way);
    void write(uint8_t data);
    uint8_t read();

};

extern Cache I$;
extern Cache D$;

#endif