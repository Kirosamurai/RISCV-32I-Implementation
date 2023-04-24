#include <iostream>
#include <bitset>
#include <string>
#include <cstring>
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

std::string Cache::mainMemoryLoader(int whichCache, uint32_t mem_address) {
  if (whichCache == 1) // I$
  {
    if ((processor.instruction_memory.find(mem_address)) == (processor.instruction_memory.end())) {
      processor.instruction_memory[mem_address] = 0;
    }
    uint8_t value_at_memory = processor.instruction_memory[mem_address];
    return std::bitset<32>(value_at_memory).to_string();

  } else // D$
  {
    if ((processor.memory.find(mem_address)) == (processor.memory.end())) {
      processor.memory[mem_address] = 0;
    }
    uint8_t value_at_memory = processor.memory[mem_address];
    return std::bitset<8>(value_at_memory).to_string();
  }
}

void Cache::recencyAssignVal(int index, int way, int value) {
  std::string answer;
  int temp = value;
  for (int i = 0; i < recency_bits; i++) {
    if (temp % 2 == 0) {
      answer += '0';
    } else {
      answer += '1';
    }
    temp = temp / 2;
  }
  data_array[index][2][way] = answer;
}

int Cache::recencyTranslateVal(int index, int way) {
  int answer = 0;
  std::string r_bits_copy = data_array[index][2][way];
  for (int i = 0; i < recency_bits; i++) {
    if (r_bits_copy[i] == '1') {
      answer += pow(2, i);
    }
  }
  return answer;
}

void Cache::recencyUpdater(std::string index_str, int way) {
  int index;
  for (int i=index_bits-1; i>=0; i--) {
    index += index_str[i] * pow(2,index_bits-i-1);
  }

  std::string max_val_string;
  std::string min_val_string;
  for (int i = 0; i < recency_bits; i++) {
    max_val_string += '1';
    min_val_string += '0';
  }

  switch (replace) {
  case 1: // LRU
  {
    if (data_array[index][2][way] != max_val_string) {
      data_array[index][2][way] = max_val_string;
      for (int i = 0; i < associativity; i++) {
        // data_array[index][2][i]--;
        if (data_array[index][2][i] != min_val_string) {
          recencyAssignVal(index, i, (recencyTranslateVal(index, i) - 1));
        }
      }
      data_array[index][2][way] = max_val_string;
    }
  }
  case 2: // FIFO
  {
    int queue_counter = 0;
    for (int i = 0; i < associativity; i++) {
      if (recencyTranslateVal(index, i) != 0) {
        queue_counter++;
      }
    }
    recencyAssignVal(index, way, queue_counter);
  }
  case 3: // RANDOM
  {
    data_array[index][2][way] = min_val_string;
  }
  case 4: // LFU
  {
    if (data_array[index][2][way] != max_val_string) {
      recencyAssignVal(index, way, (recencyTranslateVal(index, way) + 1));
    }
  }
  }
}

void Cache::dirtyVictim(int index_num, int way) {
  if ((data_array[index_num][1][way]) == "1") {
    // victim is dirty
    std::string victim_address;
    std::string offset_zero;
    for (int i = 0; i < offset_bits; i++) {
      offset_zero += '0';
    }
    victim_address = tag_array[index_num][way] + index + offset_zero;
    uint32_t victim_address_num = stoi(victim_address);
    std::string victim_value = data_array[index_num][3][way];
    // since only D$ cache can have dirty memory, we directly access
    // data main memory.
    for (int i = 0; i < (block_size / 8); i++) {
      processor.memory[victim_address_num + i] =
          stoi(victim_value.substr(8, 8 * i));
    }
  }
}

// IF MISS:
void Cache::allocate(uint32_t mem_address) {
  int index_num;
  for (int i = index_bits - 1; i >= 0; i--) {
    index_num += index[i] * pow(2, index_bits - i - 1);
  }

  if (associativity == 1) // direct mapped
  {
    // check if space exists in the set?
    if ((data_array[index_num][0][0]) != "1") {
      // space exists
      // load data from main memory
      data_array[index_num][0][0] = "1";
      data_array[index_num][1][0] = "0";
      data_array[index_num][3][0] = mainMemoryLoader(whichCache, mem_address);
      tag_array[index_num][0] = tag;
    } else {
      // space does not exist, kick victim (is victim dirty?)
      dirtyVictim(index_num, 0);
      // if ((data_array[index_num][1][0]) == "1") {
      //   // victim is dirty
      //   std::string victim_address;
      //   victim_address = tag_array[index_num][0] + index + offset;
      //   uint32_t victim_address_num = stoi(victim_address);
      //   uint8_t victim_value = stoi(data_array[index_num][3][0]);
      //   // since only D$ cache can have dirty memory, we directly access data
      //   // main memory.
      //   processor.memory[victim_address_num] = victim_value;
      //   }
      // load data from main memory
      data_array[index_num][0][0] = "1";
      data_array[index_num][1][0] = "0";
      data_array[index_num][3][0] = mainMemoryLoader(whichCache, mem_address);
      tag_array[index_num][0] = tag;
    }
    thisWay = 0;
  
  } else if (set_num == 1) // fully associative
  {
    bool space_found = false; // flag that tells whether free space exists in
                              // the block or not.
    // check if space exists in the set?
    for (int i = 0; i < associativity; i++) {
      if ((data_array[0][0][i]) != "1") {
        space_found = true;
        thisWay = i;
        // update recency bits
        recencyUpdater(0, i);
        // load data from main memory
        data_array[0][0][i] = "1";
        data_array[0][1][i] = "0";
        data_array[0][3][i] = mainMemoryLoader(whichCache, mem_address);
        tag_array[0][i] = tag;
        break;
      }
    }
    if (!space_found) {
      // space does not exist, kick victim (is victim dirty?)
      std::string min_val_string;
      for (int i = 0; i < recency_bits; i++) {
        min_val_string += '0';
      }
      switch (replace) {
      case 1: // if LRU, pick any "00000" and kick
      {
        for (int i = 0; i < associativity; i++) {
          if ((data_array[index_num][2][i]) == min_val_string) {
            thisWay = i;
            dirtyVictim(index_num, thisWay);
            break;
          }
        }
        break;
      }
      case 2: // if FIFO, pick any "00000" and kick
      {
        for (int i = 0; i < associativity; i++) {
          if ((data_array[index_num][2][i]) == min_val_string) {
            thisWay = i;
            dirtyVictim(index_num, thisWay);
            break;
          }
        }
        break;
      }
      case 3: // if RANDOM, pick any and kick
      {
        thisWay = rand() % associativity;
        dirtyVictim(index_num, thisWay);
        break;
      }
      case 4: // if LFU, find least value and kick
      {
        int least_value = recencyTranslateVal(index_num, 0);
        thisWay = 0;
        for (int i = 0; i < associativity; i++) {
          if (recencyTranslateVal(index_num, i) < least_value) {
            least_value = recencyTranslateVal(index_num, i);
            thisWay = i;
          }
        }
        dirtyVictim(index_num, thisWay);
      }
      }
      // load data from main memory
      data_array[index_num][0][thisWay] = "1";
      data_array[index_num][1][thisWay] = "0";
      data_array[index_num][3][thisWay] =
          mainMemoryLoader(whichCache, mem_address);
      tag_array[index_num][thisWay] = tag;
      // update recency bits
      recencyUpdater(index, thisWay);
    }
  
  } else // n-way set mapped
  {
    bool space_found = false; // flag that tells whether free space exists in
                              // the block or not.
    // check if space exists in the set?
    for (int i = 0; i < associativity; i++) {
      if ((data_array[index_num][0][i]) != "1") {
        space_found = true;
        // update recency bits
        recencyUpdater(0, i);
        // load data from main memory
        data_array[0][0][i] = "1";
        data_array[0][1][i] = "0";
        data_array[0][3][i] = mainMemoryLoader(whichCache, mem_address);
        tag_array[0][i] = tag;
        break;
      }
    }
    if (!space_found) {
      // space does not exist, kick victim (is victim dirty?)
      std::string min_val_string;
      for (int i = 0; i < recency_bits; i++) {
        min_val_string += '0';
      }
      switch (replace) {
      case 1: // if LRU, pick any "00000" and kick
      {
        for (int i = 0; i < associativity; i++) {
          if ((data_array[index_num][2][i]) == min_val_string) {
            thisWay = i;
            dirtyVictim(index_num, thisWay);
            break;
          }
        }
        break;
      }
      case 2: // if FIFO, pick any "00000" and kick
      {
        for (int i = 0; i < associativity; i++) {
          if ((data_array[index_num][2][i]) == min_val_string) {
            thisWay = i;
            dirtyVictim(index_num, thisWay);
            break;
          }
        }
        break;
      }
      case 3: // if RANDOM, pick any and kick
      {
        thisWay = rand() % associativity;
        dirtyVictim(index_num, thisWay);
        break;
      }
      case 4: // if LFU, find least value and kick
      {
        int least_value = recencyTranslateVal(index_num, 0);
        thisWay = 0;
        for (int i = 0; i < associativity; i++) {
          if (recencyTranslateVal(index_num, i) < least_value) {
            least_value = recencyTranslateVal(index_num, i);
            thisWay = i;
          }
        }
        dirtyVictim(index_num, thisWay);
      }
      }
      // load data from main memory
      data_array[index_num][0][thisWay] = "1";
      data_array[index_num][1][thisWay] = "0";
      data_array[index_num][3][thisWay] =
          mainMemoryLoader(whichCache, mem_address);
      tag_array[index_num][thisWay] = tag;
      // update recency bits
      recencyUpdater(index, thisWay);
    }
  }
}

//IF HIT:
uint8_t Cache::read() {
    
    uint8_t data_val;

    int index_num;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += index[i] * pow(2,index_bits-i-1);
    }

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

    for (int i=offset_bits-1; i>=0; i--) {
        offset_num += offset[i] * pow(2,offset_bits-i-1);
    }

    data_array[index_num][0][thisWay] = 1; //validity
    data_array[index_num][1][thisWay] = 1; //dirty
    
    std::string data_val_bits = std::bitset<8>(data_val).to_string();
    std::string data = data_array[index_num][3][thisWay];

    for (int i=8*offset_num; i<8*(offset_num+1); i++) {
        data_array[index_num][3][thisWay][i] = data_val_bits[i-8*offset_bits];
    }
}

