#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "L1_cache.h"
#include "segmentation.h"
#include "main_memory.h"
#include "make_modules.h"



void init_square_mat_array(){
	square_mat_arr[0] = 7;
	square_mat_arr[1] = 11;
	square_mat_arr[2] = 13;
	square_mat_arr[3] = 14;
}

void l1_initialize()
{
  for(int i = 0; i < 128; i++){
  	for(int j = 0; j < 4; j++){
  		l1_cache[i].lines[j].v = 0;
  		l1_cache[i].lines[j].tag_bits = 0;
  		l1_cache[i].lines[j].replacement_policy_bits = 0;
  		for(int m = 0; m < 8; m++){
  			l1_cache[i].lines[j].cache_line[m][0] = "69";
  			l1_cache[i].lines[j].cache_line[m][1] = "99";
  		}

  	}
  }

}

// void buffer_init()
// {
// 	for(int i = 0 ; i < 8; i++){
// 		write_buff.data[i] = "00";
// 		write_buff.address[i] = 0;
// 	}
// 	write_buff.head = -1;
// 	write_buff.tail = -1;
// 	write_buff.size = 0;
// }

int mm_cache_read(uint32_t address){ //address is the physical address.
	cache_mem_access_data(address);
	return 1; // this always a true case. Main memory is always a hit.
}

int look_through_read(uint32_t address){ //comes here only on a read/write miss.
	uint32_t status;
	status = l2_cache_read(address);
	if(status == 1) {
		printf("Data found in L2 cache, Updating L1\n");
		l1_cache_select_lru_and_replace(address);
		return 1;
	}
	status = mm_cache_read(address); //hass to have try accessing data.
	if(status == 1){
		printf("Data found in Main memory, Updating L1\n");
		l1_cache_select_lru_and_replace(address);
		printf("L1 is updated\n");
		return 1; //updating L1, L2 be implemented.
	}
	else 
		return 0;
}

void l1_cache_select_lru_and_replace(uint32_t address){
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	uint32_t temp_tag = (address >> 11);
	int to_be_replaced = 0;
	for(int i = 0; i < 4; i++){
		if(l1_cache[temp_set].lines[i].replacement_policy_bits == 0){
			to_be_replaced = i; // to be completed after discussion. To replace 16B cache line at once ?? Will I get 16B data from l2 and mm?
		}
	}
	l1_cache[temp_set].lines[to_be_replaced].tag_bits = temp_tag;
	l1_cache[temp_set].lines[to_be_replaced].v = 1;
	update_replacement_policy_bits(address, to_be_replaced);
}

void update_replacement_policy_bits(uint32_t address, uint32_t num){
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	l1_cache[temp_set].lines[num].replacement_policy_bits = 1111;
	for(int i = 0; i < 4; i++){
		l1_cache[temp_set].lines[i].replacement_policy_bits = (l1_cache[temp_set].lines[num].replacement_policy_bits & square_mat_arr[num]);
	}

}

int l1_cache_read(uint32_t address){ //returns status on completion either a hit or a miss. Store it in read if its a read.
	//Get Tag and Index from address
	uint32_t temp_tag = (address >> 11);
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	uint32_t temp_word_offset = (address >> 1) & WORDS_OFFSET_MASK;
	uint32_t temp_byte_offset = (address) & BYTE_OFFSET_MASK;

	for(int i = 0; i < 4; i++){
		if(l1_cache[temp_set].lines[i].v == 1){
			if(l1_cache[temp_set].lines[i].tag_bits == temp_tag){
				printf("L1 Cache Hit has occured in reading\n");
				update_replacement_policy_bits(address, i);
				return 1;
			}
			else{
				continue;
			}

		}
	}
	printf("L1 Cache Miss has occured, Looking Through\n");
	uint32_t look_result = look_through_read(address);
	return look_result;
}

// int l1_cache_write(uint32_t address, char* write){ //returns status on completion either a hit or a miss. Note that this implementation is a write-through-buffer.
// 	//Get Tag and Index from address
// 	uint32_t temp_tag = (address >> 11);
// 	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
// 	uint32_t temp_word_offset = (address >> 1) & WORDS_OFFSET_MASK;
// 	uint32_t temp_byte_offset = (address) & BYTE_OFFSET_MASK;
// 	int write_result = 0;

// 	for(int i = 0; i < 4; i++){
// 		if(l1_cache[temp_set].lines[i].v == 1){
// 			if(l1_cache[temp_set].lines[i].tag_bits == temp_tag){
// 				l1_cache[temp_set].lines[i].cache_line[temp_word_offset][temp_byte_offset] = write;
// 				write_result = 1;
// 				add_into_write_through_buffer(address, write);
// 				break;
// 			}
// 			else{
// 				continue;
// 			}

// 		}
// 	}
// 	return write_result;

// }


// void add_into_write_through_buffer(uint32_t address, char* write){
// 	if(write_buff.head == -1 && write_buff.tail == -1){
// 		write_buff.head = 0;
// 		write_buff.tail = 1;
// 		write_buff.data[write_buff.head] = write;
// 		write_buff.address[write_buff.head] = address;
// 		write_buff.size += 1;
// 	}
// 	else if(write_buff.size < 8){
// 		write_buff.data[write_buff.tail] = write;
// 		write_buff.address[write_buff.tail] = address;
// 		write_buff.tail += 1;
// 		write_buff.tail = write_buff.tail%8; 
// 		write_buff.size += 1;
// 	}
// 	else if(write_buff.size == 8){
// 		printf("Queue Full");
// 	}

// }

// void actual_write_through(){ //frees up buffer.
// 	if(write_buff.size > 0){
// 		//l2_write();
// 		//mem_write();
// 		printf("%d\n", write_buff.size);
// 		printf("Written to L2 and Main Memory\n");
// 		write_buff.size -= 1;
// 		write_buff.head += 1;
// 		write_buff.head = write_buff.head%8;
// 	}
// }

// void* thread_for_write_buffer(void* args){
// 	while(1){
// 		printf("trying to write\n");
// 		sleep(2);
// 		actual_write_through();
//	}
//}




