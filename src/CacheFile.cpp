#include <iostream>
#include <bitset>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include "CacheFile.h"

bool Cache::isPresent(uint32_t add) {
    
    std::string address = std::bitset<32>(add).to_string();

    tag = address.substr(0, tag_bits);
    index = address.substr(tag_bits, index_bits);
    offset = address.substr(tag_bits+index_bits, offset_bits);
    
    int index_num=0;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += (index[i]-'0') * pow(2,index_bits-i-1);
    }

    for (int i=0; i<associativity; i++) {
        if (tag_array[index_num][i] == tag) {
          if (data_array[index_num][0][i] == "1") {
            thisWay = i;
            return true;
          }
        }
    }
    return false;

}

// mem_address here has to be the starting address of the block.
void Cache::mainMemoryLoader(int whichCache, uint32_t mem_address, int index, int way) {
  if (whichCache == 1) // I$
  { 
    std::string data_upload;
    for (int i = 0; i < (block_size); i += 4) {
      uint32_t value_at_memory = processor.instruction_memory[mem_address + i];
      std::bitset<32> temp = value_at_memory;
      data_upload += temp.to_string();
    }
    data_array[index][3][way] = data_upload;
    return;
  } else // D$
  {
    std::string data_upload;
    for (int i = 0; i < (block_size); i++) {
      uint8_t value_at_memory = processor.memory[mem_address + i];
      data_upload += std::bitset<8>(value_at_memory).to_string();
    }
    data_array[index][3][way] = data_upload;
    return;
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
  int index = 0;
  for (int i=index_bits-1; i>=0; i--) {
    index += (index_str[i]-'0') * pow(2,index_bits-i-1);
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
      for (int i = 0; i < associativity; i++) {
        // data_array[index][2][i]--;
        if (data_array[index][0][i]=="1")
        {
          if (data_array[index][2][i] != min_val_string) {
          recencyAssignVal(index, i, ((recencyTranslateVal(index, i)) - 1));
          }
        }
      }
      data_array[index][2][way] = max_val_string;
    }
    break;
  }
  case 2: // FIFO
  {
    int queue_counter = 0;
    
    for (int i=0; i<associativity; i++) {
      if ((data_array[index][0][i]) == "1") {
        queue_counter++;
      }
    }
    if ((data_array[index][0][way])=="1")
    {
      queue_counter--;
    }
    recencyAssignVal(index, way, queue_counter);
    break;
  }
  case 3: // RANDOM
  {
    data_array[index][2][way] = min_val_string;
    break;
  }
  case 4: // LFU
  {   
      if (data_array[index][2][way] != max_val_string) {
      recencyAssignVal(index, way, (recencyTranslateVal(index, way) + 1));
    }
    break;
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
    // uint32_t victim_address_num = stoi(victim_address);
    uint32_t victim_address_num = 0;
    int victim_address_length = victim_address.length();
    for (int i=0; i<victim_address_length; i++)
    {   
      if (victim_address[victim_address_length-i-1] == '1')
      {
          victim_address_num += pow(2, i);
      }
    }
    std::string victim_value = data_array[index_num][3][way];
    // since only D$ cache can have dirty memory, we directly access
    // data main memory.
    for (int i = 0; i < (block_size); i++) {
      uint8_t victim_value_data = 0;
      for (int j = 8*i; j<(8*(i+1)); j++)
      {   
        if (victim_value[j] == '1')
        {
            victim_value_data += pow(2, 8*i + 7 - j);
        }
      }
      processor.memory[victim_address_num + i] = victim_value_data;
    }
  }
}

uint32_t Cache::noOffset(uint32_t address) {
  uint32_t no_offset_address = address >> offset_bits;
  no_offset_address = no_offset_address << offset_bits;
  return no_offset_address;
}

// IF MISS:
void Cache::allocate(uint32_t mem_address) {	
  int index_num=0;	
  for (int i = index_bits - 1; i >= 0; i--) {	
    index_num += (index[i]-'0') * pow(2, index_bits - i - 1);	
  }

  std::string max_val_string;
  for (int i = 0; i < recency_bits; i++) {
    max_val_string += '1';
  }

  if (associativity == 1) // direct mapped	
  {	
    // check if space exists in the set?	
    if ((data_array[index_num][0][0]) != "1") {	
      // space exists	
      // load data from main memory	
      data_array[index_num][0][0] = "1";	
      data_array[index_num][1][0] = "0";	
      mainMemoryLoader(whichCache, noOffset(mem_address), index_num, 0);	
      tag_array[index_num][0] = tag;	
    } else {	
      // space does not exist, kick victim (is victim dirty?)	
      dirtyVictim(index_num, 0);	
      // load data from main memory	
      data_array[index_num][0][0] = "1";	
      data_array[index_num][1][0] = "0";	
      mainMemoryLoader(whichCache, noOffset(mem_address), index_num, 0);	
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
        // if LFU policy, the new recency bits must be initialized to zero.	
        if (replace == 4) {	
          std::string min_val_string;	
          for (int i = 0; i < recency_bits; i++) {	
            min_val_string += '0';	
          }	
          data_array[0][2][i] = min_val_string;	
        }	
        recencyUpdater("0", i);	
        // load data from main memory	
        data_array[0][0][i] = "1";	
        data_array[0][1][i] = "0";	
        mainMemoryLoader(whichCache, noOffset(mem_address), 0, i);	
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
            goto updateRest1;
          }	
        }
        updateRest1:
        for (int i = 0; i < associativity; i++) {
          // data_array[index][2][i]--;
          if (data_array[index_num][2][i] != min_val_string) {
            recencyAssignVal(index_num, i, (recencyTranslateVal(index_num, i) - 1));
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
        for (int i = 1; i < associativity; i++) {	
          if (recencyTranslateVal(index_num, i) < least_value) {	
            least_value = recencyTranslateVal(index_num, i);	
            thisWay = i;	
          }	
        }	
        dirtyVictim(index_num, thisWay);	
        break;
      }	
      }	
      // load data from main memory	
      data_array[index_num][0][thisWay] = "1";	
      data_array[index_num][1][thisWay] = "0";	
      mainMemoryLoader(whichCache, noOffset(mem_address), index_num, thisWay);	
      tag_array[index_num][thisWay] = tag;	
      // update recency bits	
      // if LFU policy, the new recency bits must be initialized to zero.	
      if (replace == 4) {	
        data_array[index_num][2][thisWay] = min_val_string;	
      }	
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
        // if LFU policy, the new recency bits must be initialized to zero.	
        thisWay = i;
        recencyAssignVal(index_num,thisWay,0);
        recencyUpdater(index, i);	
        // load data from main memory	
        data_array[index_num][0][i] = "1";	
        data_array[index_num][1][i] = "0";	
        mainMemoryLoader(whichCache, noOffset(mem_address), index_num, i);	
        tag_array[index_num][i] = tag;	
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
            goto updateRest2;
          }	
        }
        updateRest2:
        for (int i = 0; i < associativity; i++) {
          // data_array[index][2][i]--;
          if (data_array[index_num][2][i] != min_val_string) {
            recencyAssignVal(index_num, i, (recencyTranslateVal(index_num, i) - 1));
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
        break;
      }	
      }	
      // load data from main memory	
      data_array[index_num][0][thisWay] = "1";	
      data_array[index_num][1][thisWay] = "0";	
      mainMemoryLoader(whichCache, noOffset(mem_address), index_num, thisWay);	
      tag_array[index_num][thisWay] = tag;	
      // update recency bits	
      // if LFU policy, the new recency bits must be initialized to zero.	
      if (replace == 4) {	
        data_array[index_num][2][thisWay] = min_val_string;	
      }	
      recencyUpdater(index, thisWay);	
    }	
  }	
}

//IF HIT:
uint8_t Cache::read() {
    
    uint8_t data_val = 0;
    offset_num = 0;

    int index_num=0;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += (index[i]-'0') * pow(2,index_bits-i-1);
    }

    for (int i=offset_bits-1; i>=0; i--) {
        offset_num += (offset[i]-'0') * pow(2,offset_bits-i-1);
    }

    std::string data = data_array[index_num][3][thisWay];

    for (int i=8*offset_num; i<8*(offset_num+1); i++) {
        data_val += (data[i]-'0') * pow(2,8*offset_num+7-i);
    }

    return data_val;

}

uint32_t Cache::readI() {
    uint32_t data_val = 0;
    offset_num=0;

    int index_num=0;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += (index[i]-'0') * pow(2,index_bits-i-1);
    }

    for (int i=offset_bits-1; i>=0; i--) {
        offset_num += (offset[i]-'0') * pow(2,offset_bits-i-1);
    }

    std::string data = data_array[index_num][3][thisWay];
    
    for (int i=8*offset_num; i<8*(offset_num+4); i++) {
        data_val += (data[i]-'0') * pow(2,8*offset_num+31-i);
    }
    return data_val;

}

void Cache::write(uint8_t data_val) {
    int index_num=0;
    for (int i=index_bits-1; i>=0; i--) {
        index_num += (index[i]-'0') * pow(2,index_bits-i-1);
    }

    offset_num = 0;
    for (int i=offset_bits-1; i>=0; i--) {
        offset_num += (offset[i]-'0') * pow(2,offset_bits-i-1);
    }

    data_array[index_num][0][thisWay] = "1"; //validity
    data_array[index_num][1][thisWay] = "1"; //dirty
    
    std::bitset<8> temp = data_val;
    std::string data_val_bits = temp.to_string();
    
    for (int i=8*offset_num; i<8*(offset_num+1); i++) {
        data_array[index_num][3][thisWay][i] = (char) (data_val_bits[i-8*offset_num]);
    }
}

int Cache::miss_type(uint32_t mem_address){
  int index_num=0;
  for (int i=index_bits-1; i>=0; i--) {
      index_num += (index[i]-'0') * pow(2,index_bits-i-1);
  }

  if(std::find(mct.begin(),mct.end(), mem_address) == mct.end()){
    mct.push_back(mem_address);
    return 1; //COLD MISS
  }else{
    for(int i=0; i<associativity; i++){
      if(data_array[index_num][0][i] == "0"){
        return 2; //CONFLICT MISS
      }
    }
    return 3; //CAPACITY MISS
  }
  	
}

void ByteReader(int add) {
  bool hit = D.isPresent(add);
  if (hit) {
      D.hits++;
      D.recencyUpdater(D.index, D.thisWay);
  } else {
      int miss = D.miss_type(add);
      switch (miss){
      case 1:
          D.cold_miss++;
          break;
      case 2:
          D.conflict_miss++;
          break;
      case 3:
          D.capacity_miss++;
          break;
      }
      processor.clock_cycle += D.penalty;
      D.memory_stalls += D.penalty;

      D.allocate(add);
  }
}

void ByteWriter(int add) {
  bool hit = D.isPresent(add);
  if (hit) {
      D.hits++;
      D.recencyUpdater(D.index, D.thisWay);
  } else {
      int miss = D.miss_type(add);
      switch (miss){
      case 1:
          D.cold_miss++;
          break;
      case 2:
          D.conflict_miss++;
          break;
      case 3:
          D.capacity_miss++;
          break;
      }
      processor.clock_cycle += D.penalty;
      D.memory_stalls += D.penalty;

      D.allocate(add);
  }
}