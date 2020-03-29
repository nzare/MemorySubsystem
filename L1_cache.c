#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#define LINES_PER_SET 4 //4 way set associative
#define NUM_SETS 128	 
#define TAG_BITS 21
#define REPL_BITS 4 //Square matrix implmentation
#define WORDS_IN_CACHE_LINE 8 // 4 bits is a word , therefore 8 words is 16B.
#define WORD_OFFSET_BITS 3
#define BLOCK_OFFSET_BITS 1
#define L1_VBIT_MASK (1 << 31)
#define L1_RBIT_MASK (1 << 30) //Might not be needed
#define SET_BITS_MASK (0x000007f)
#define WORDS_OFFSET_MASK (0x00000007)
#define BYTE_OFFSET_MASK (0x00000001)
#define MAX_BUFFER_SIZE 8 // 8 bytes.
const int squate_mat_arr [0x7, 0XB, 0xD, 0xE];
void l1_initialize();


void buffer_init();

int l1_cache_read(uint32_t address, char** read);

int l1_cache_write(uint32_t address, char* write);

void add_into_write_through_buffer(uint32_t address, char* write);

void actual_write_through();

//Struct for Cache Module

typedef struct {
  uint32_t v; //Implementing Write through buffer, so no chance of dirty bit.
  uint32_t tag_bits; 
  uint32_t replacement_policy_bits;
  char* cache_line[WORDS_IN_CACHE_LINE][2];
} L1_CACHE_ENTRY;

typedef struct {
	L1_CACHE_ENTRY lines[LINES_PER_SET];
} L1_CACHE_SET;

//Struct for write buffer
typedef struct{
	char* data[8];
	int head;
	int tail;
	int size;
	uint32_t address[8];
}WRITE_BUFFER;

typedef struct{
	uint32_t address;
	char** read;
}read_packet;


//Initializing Cache Module as an array of num sets.
L1_CACHE_SET l1_cache[NUM_SETS];

WRITE_BUFFER write_buff;

void l1_initialize()
{
  for(int i = 0; i < 128; i++){
  	for(int j = 0; j < 4; j++){
  		l1_cache[i].lines[j].v = 1;
  		l1_cache[i].lines[j].tag_bits = (0 << TAG_BITS);
  		l1_cache[i].lines[j].replacement_policy_bits = (0 << REPL_BITS);
  		for(int m = 0; m < 8; m++){
  			l1_cache[i].lines[j].cache_line[m][0] = "69";
  			l1_cache[i].lines[j].cache_line[m][1] = "99";
  		}

  	}
  }

}

void buffer_init()
{
	for(int i = 0 ; i < 8; i++){
		write_buff.data[i] = "00";
		write_buff.address[i] = 0;
	}
	write_buff.head = -1;
	write_buff.tail = -1;
	write_buff.size = 0;
}



int look_through_read(uint32_t address, char** read){ //comes here only on a read/write miss.
	uint32_t status;
	status = l2_cache_read(address, read);
	if(status == 1) {
		l1_cache_write(address, read);
		return 1;
	}
	status = mm_read(address,read);
	if(status == 1){
		l1_cache_select_lru_and_replace(address, read);
		l2_cache_select_lru_and_replace(address, read); //I dont have this.
		return 1; //updating L1, L2 be implemented.
	else return 0;
}

void l1_cache_select_lru_and_replace(uint32_t address, char** to_be_written){
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	uint32_t temp_tag = (address >> 11);
	int to_be_replaced;
	for(int i = 0; i < 4; i++){
		if(l1_cache[temp_set].lines[i].replacement_policy_bits == 0){
			to_be_replaced = i; // to be completed after discussion. To replace 16B cache line at once ?? Will I get 16B data from l2 and mm?
		}
	}
	//replace entry at address with read_val
	l1_cache[temp_set].lines[i].to_be_replaced.tag_bits = temp_tag;

}

void update_replacement_policy_bits(uint32_t address, uint32_t num){
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	l1_cache[temp_set].lines[num].replacement_policy_bits = 1111;
	for(int i = 0; i < 4; i++){
		l1_cache[temp_set].lines[i].replacement_policy_bits = l1_cache[temp_set].lines[num].replacement_policy_bits & squate_mat_arr[num];
	}

}
/* Commented functions will be needed for look aside.

void* l1_cache_thread(void* args){
	read_packet* packet = (read_packet*)args;
	int result = l1_cache_read(packet->address, packet->read);
	if(result == 0){

	}
	else{

	}

}

void* l2_cache_thread(void* args){
	read_packet* packet = (read_packet*)args;
	int result = l1_cache_read(packet->address, packet->read);
	if(result == 0){
		printf("Address Error");
	}
	else{

	}

}

void* mm_thread(void* args){
	read_packet* packet = (read_packet*)args;
	int result = l1_cache_read(packet->address, packet->read);

}
*/

int l1_cache_read(uint32_t address, char** read){ //returns status on completion either a hit or a miss. Store it in read if its a read.
	//Get Tag and Index from address
	uint32_t temp_tag = (address >> 11);
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	uint32_t temp_word_offset = (address >> 1) & WORDS_OFFSET_MASK;
	uint32_t temp_byte_offset = (address) & BYTE_OFFSET_MASK;

	for(int i = 0; i < 4; i++){
		if(l1_cache[temp_set].lines[i].v == 1){
			if(l1_cache[temp_set].lines[i].tag_bits == temp_tag){
				*read = l1_cache[temp_set].lines[i].cache_line[temp_word_offset][temp_byte_offset];
				update_replacement_policy_bits(address, i);
				return 1;
			}
			else{
				continue;
			}

		}
	}

	return 0;
}

int l1_cache_write(uint32_t address, char* write){ //returns status on completion either a hit or a miss. Note that this implementation is a write-through-buffer.
	//Get Tag and Index from address
	uint32_t temp_tag = (address >> 11);
	uint32_t temp_set = (address >> 5) & SET_BITS_MASK;
	uint32_t temp_word_offset = (address >> 1) & WORDS_OFFSET_MASK;
	uint32_t temp_byte_offset = (address) & BYTE_OFFSET_MASK;
	int write_result = 0;

	for(int i = 0; i < 4; i++){
		if(l1_cache[temp_set].lines[i].v == 1){
			if(l1_cache[temp_set].lines[i].tag_bits == temp_tag){
				l1_cache[temp_set].lines[i].cache_line[temp_word_offset][temp_byte_offset] = write;
				write_result = 1;
				add_into_write_through_buffer(address, write);
				break;
			}
			else{
				continue;
			}

		}
	}
	return write_result;

}


void add_into_write_through_buffer(uint32_t address, char* write){
	if(write_buff.head == -1 && write_buff.tail == -1){
		write_buff.head = 0;
		write_buff.tail = 1;
		write_buff.data[write_buff.head] = write;
		write_buff.address[write_buff.head] = address;
		write_buff.size += 1;
	}
	else if(write_buff.size < 8){
		write_buff.data[write_buff.tail] = write;
		write_buff.address[write_buff.tail] = address;
		write_buff.tail += 1;
		write_buff.tail = write_buff.tail%8; 
		write_buff.size += 1;
	}
	else if(write_buff.size == 8){
		printf("Queue Full");
	}

}

void actual_write_through(){ //frees up buffer.
	if(write_buff.size > 0){
		//l2_write();
		//mem_write();
		printf("%d\n", write_buff.size);
		printf("Written to L2 and Main Memory\n");
		write_buff.size -= 1;
		write_buff.head += 1;
		write_buff.head = write_buff.head%8;
	}
}

void* thread_for_write_buffer(void* args){
	while(1){
		printf("trying to write\n");
		sleep(2);
		actual_write_through();
	}
}



int main(){
	l1_initialize();
	char* read;
	int status = l1_cache_read(0X00000000, &read);
	printf("%s\n", read);
	//Create thread for buffer running async
	pthread_t tid;
	int ret = pthread_create(&tid,NULL,&thread_for_write_buffer,NULL);
	if (ret == 0){
		printf("Sucessfully Created Thread\n");
	}
	int write_result = l1_cache_write(0X00000000, "42");
	pthread_join(tid, NULL);
	return 1;
}




