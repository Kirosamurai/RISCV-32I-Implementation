#include <iostream>
#include <bitset>
#include "Cache.h"

bool Cache::isPresent(uint32_t add) {
    
    std::string address = std::bitset<32>(add).to_string();

    tag = address.substr(0, tag_bits);
    index = address.substr(tag_bits, index_bits);
    offset = address.substr(tag_bits+index_bits, offset_bits);

    int index_num;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += index[i] * pow(2,index_bits-i-1);
    }

    for (int i=0; i<associativity; i++) {
        if (tag_array[index_num][i] == tag) {
            thisWay = i;
            return true;
        }
    }

    return false;

}

uint8_t Cache::read() {
    uint8_t data_val;

    int index_num;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += index[i] * pow(2,index_bits-i-1);
    }
    int offset_num;
    for (int i=offset_bits-1; i>=0; i--) {
        offset_num += offset[i] * pow(2,offset_bits-i-1);
    }

    std::string data = data_array[index_num][3][thisWay];

    for (int i=8*offset_num; i<8*(offset_num+1); i++) {
        data_val += data[i] * pow(2,8*offset_bits+7-i);
    }

    return data_val;

}

void Cache::write(uint8_t data_val) {
    int index_num;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += index[i] * pow(2,index_bits-i-1);
    }
    int offset_num;
    for (int i=offset_bits-1; i>=0; i--) {
        offset_num += offset[i] * pow(2,offset_bits-i-1);
    }

    data_array[index_num][0][thisWay] = 1; //validity
    data_array[index_num][1][thisWay] = 1; //dirty
    recencyUpdater(index_num, thisWay); //recency

    std::string data_val_bits = std::bitset<8>(data_val).to_string();
    std::string data = data_array[index_num][3][thisWay];

    for (int i=8*offset_num; i<8*(offset_num+1); i++) {
        data_array[index_num][3][thisWay][i] = data_val_bits[i-8*offset_bits];
    }
}
