#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "L2_cache.h"
#include "segmentation.h"
#include "main_memory.h"
#include "make_modules.h"

void l2_initialize()
{
  //CODE HERE
  for(int i = 0; i < 128; i++) 
  {
    for(int j = 0; j < 8; j++) 
    {
      
      //clearing the valid bit, and counter bits of all entries
      l2_cache[i].lines[j].v_d_c_tag = (l2_cache[i].lines[j].v_d_c_tag & 0x47FFFFFF);

      //storing 0s in the cache line
      for (int k = 0; k < 16; k++) {
        l2_cache[i].lines[j].cache_line[k][0] = "69";
        l2_cache[i].lines[j].cache_line[k][1] = "99";
      }

    }
  }

}


void l2_update_counter_bits(uint32_t temp_index, int line)
{
  l2_cache[temp_index].lines[line].v_d_c_tag = (l2_cache[temp_index].lines[line].v_d_c_tag | L2_COUNTER_MASK);
  for (int i = 0; i < 8; i++){
    if (i != line)
    {
      uint32_t temp_count = (l2_cache[temp_index].lines[i].v_d_c_tag & L2_COUNTER_MASK) >> 27;
      if(temp_count != 0)
      {
        --temp_count;
        temp_count = (temp_count << 27);
        l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag | temp_count);
      }
    }
  }
}



int l2_mm_cache_read(uint32_t address)
{ 

  //address is the physical address.
  cache_mem_access_data(address);
  return 1; // this always a true case. Main memory is always a hit.

}


int l2_look_through_read(uint32_t address)
{

  uint32_t status;
  status = l2_mm_cache_read(address);
  if(status == 1)
  {
    printf("Data found in Main memory, Updating L2\n");
    l2_cache_select_lru_and_replace(address);
    return 1;
  }
  else return 0;

}

void l2_cache_select_lru_and_replace(uint32_t address)
{
	uint32_t temp_tag = (address & L2_ADDRESS_TAG_MASK) >> L2_ADDRESS_TAG_SHIFT;
    uint32_t temp_index = (address & L2_SET_INDEX_MASK) >> L2_SET_INDEX_SHIFT;
	int to_be_replaced = 0;
	for(int i = 0; i < 8; i++){
		uint32_t temp_count = (l2_cache[temp_index].lines[i].v_d_c_tag & L2_COUNTER_MASK) >> 27;
		if(temp_count == 0){
			to_be_replaced = i;
		}
	}
	l2_cache[temp_index].lines[to_be_replaced].v_d_c_tag = ((l2_cache[temp_index].lines[to_be_replaced].v_d_c_tag & 0xFFF80000) | temp_tag);
	l2_cache[temp_index].lines[to_be_replaced].v_d_c_tag = (l2_cache[temp_index].lines[to_be_replaced].v_d_c_tag | L2_VBIT_MASK);
	l2_update_counter_bits(temp_index, to_be_replaced);
}



int l2_cache_read(uint32_t address)
{

  uint32_t temp_tag = (address & L2_ADDRESS_TAG_MASK) >> L2_ADDRESS_TAG_SHIFT;
  uint32_t temp_index = (address & L2_SET_INDEX_MASK) >> L2_SET_INDEX_SHIFT;
  uint32_t temp_word_offset = (address & WORD_OFFSET_MASK) >> WORD_OFFSET_SHIFT;
  uint32_t temp_byte_offset = (address & BYTE_OFFSET_MASK);
  

  for (int i = 0; i < 8; i++) 
  {
    if (((l2_cache[temp_index].lines[i].v_d_c_tag & L2_VBIT_MASK) == (1 << 31)) && ((l2_cache[temp_index].lines[i].v_d_c_tag & L2_ENTRY_TAG_MASK) == temp_tag)) 
    {
      printf("L2 Cache Hit has occured in reading\n");
      l2_update_counter_bits(temp_index, i);
      return 1;
    }
  }

  printf("L2 Cache Miss has occured, Looking Through\n");
  uint32_t look_result = l2_look_through_read(address);
  return look_result;

}
