#include "segmentation.h"
#include "main_memory.h"

#include <string.h>



uint8_t hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

void update_occupancy(int start_index, int finish_index){
	for(int i = start_index; i <= finish_index; i++){
		occupancy[i] = 1;
	}
}

uint32_t find_first_fit(){
	int free_count = 0;
	uint32_t start_index;
	for(int i = 0 ; i < 0x02FFFFFF; i++){
		if(free_count == 1024){
			return start_index;
		}
		if(occupancy[i] == 0){
			if(free_count == 0){
				start_index = i;
			}
			free_count += 1; 	
		}
		else if(occupancy[i] == 1){
			free_count = 0;
		}
	}

	printf("Memory Full\n");
	return start_index;
}

void initialize_page_table_for_segment(uint32_t start_address){
	for(int i = 0; i < 256 ; i++){ //only top 22 bits base address 1 bit for presence, rest is padding.
		for(int j = 0; j < 4; j++){
			main_memory[start_address + i*4 + j][0] = '0';
			main_memory[start_address + i*4 + j][1] = '0';
		}
	}
}

void update_page_table_for_segment(uint32_t segment_base, uint8_t page_num, char* hex_address){
	uint8_t page_num_offset = page_num * 4;
	char a,b;
	char arr[2];
	int count = 0;
	for(int i = 0; i < 8; i+=2){
		a = hex_address[i];
		b = hex_address[i+1];
		arr[0] = a;
		arr[1] = b;
		main_memory[segment_base + page_num_offset + count][0] = a;
		main_memory[segment_base + page_num_offset + count][1] = b;
		count++;
	}
}

void address_to_hex_string(uint32_t address, char** hex_address){
	char hex[8];
	sprintf(hex, "%x", address);
	int size = strlen(hex);
	int zfill = 8 - size;
	for(int i = 0; i < 8; i++){
		if(i >= zfill){
			(*hex_address)[i] = hex[i-zfill];
		}
		else{
			(*hex_address)[i] = '0';
		}
	}
	(*hex_address)[8] = '\0';
}

void cache_mem_access_data(uint32_t index){
	printf("Data hit occured in Main Memory\n");
}



uint32_t try_accessing_data(uint32_t segment_base, uint8_t page_num, uint16_t offset){
	uint8_t page_num_offset = page_num * 4;
	char req_nibble;
	uint8_t presence_bit;
	uint32_t page_address; 
	uint32_t page_address_dup;
	req_nibble = main_memory[segment_base + page_num_offset + 2][1];
	presence_bit = (hex2int(req_nibble) & 0x2) >> 1;
	if(presence_bit == 1){ //need to compute page_address when theres a hit.
		char temp_array[7] = {main_memory[segment_base + page_num_offset][0], 	
								main_memory[segment_base + page_num_offset][1] ,
								main_memory[segment_base + page_num_offset + 1][0],
								main_memory[segment_base + page_num_offset + 1][1],
								main_memory[segment_base + page_num_offset + 2][0],main_memory[segment_base + page_num_offset + 2][1], '\0'};
		printf("Page and Data hit occured in MM\n");
		page_address_dup = (uint32_t)strtol(temp_array, NULL, 16);
		page_address_dup = page_address_dup - 0x00000200;
		return page_address_dup;
	}
	else{
		printf("Page Fault Occured!. Bringing requested page from disk and using first fit algorithm to fit it in memory\n");
		printf(".................\n");
		page_address = find_first_fit();
		printf("Updating Page Table of Segment and Placing Page ----- Segment Base is -> %d\n", segment_base);
		update_occupancy(page_address, page_address+1024-1);
		page_address_dup = page_address;
		page_address = page_address +  0x00000200; //page address ke last 10 bits will be zero only, 1k ke blocks main allot hota hai.
		//this addition is to set present bit as 1.
		//converting 32 bit page address to 32 bit hex string.
		char* hex_address;
		hex_address = (char*)malloc(9);
		address_to_hex_string(page_address, &hex_address); //8 char ki hex string.
		update_page_table_for_segment(segment_base, page_num, hex_address);
	}
	return page_address_dup;
}

void init_memory(){
	//Occupy initial segments.
	for(int i = 0; i < 0x02FFFFFF; i++){
		main_memory[i][0] = '0';
		main_memory[i][1] = '0';
	}
	update_occupancy(GDT_START, GDT_END);
	update_occupancy(LDT_1_START, LDT_1_END);	
	update_occupancy(LDT_2_START, LDT_2_END);
	update_occupancy(LDT_3_START, LDT_3_END);
	update_occupancy(LDT_4_START, LDT_4_END);
	update_occupancy(LDT_5_START, LDT_5_END);
	update_occupancy(CS_START, CS_START + 1024 - 1);
	update_occupancy(DS_START_1, DS_START_1 + 1024 - 1);
	update_occupancy(DS_START_2, DS_START_2 + 1024 - 1);
	update_occupancy(DS_START_3, DS_START_3 + 1024 - 1);
	update_occupancy(DS_START_4, DS_START_4 + 1024 - 1);
	update_occupancy(DS_START_5, DS_START_5 + 1024 - 1);
	initialize_page_table_for_segment(CS_START);
	initialize_page_table_for_segment(DS_START_1);
	initialize_page_table_for_segment(DS_START_2);
	initialize_page_table_for_segment(DS_START_3);
	initialize_page_table_for_segment(DS_START_4);
	initialize_page_table_for_segment(DS_START_5);
}