/*File for initializing GDT, 5 LDTs with appropriate data filled in. These are structs.
  5 Local Data Segments and One global code segment.
*/
#include "segmentation.h"
#include "main_memory.h"
#include "make_modules.h"

segment get_segment_entry(uint32_t selector, uint32_t process_num, int descriptor){
	uint32_t base_address;
	if(descriptor == 1){ //search LDT
		segment LDT_entry = search_LDT(selector, process_num); //get address of segment. process num acts as value of ldtr.
		return LDT_entry;
	}
	else{ //access GDT ie. code segment
		segment GDT_entry = search_GDT(selector);
		return GDT_entry;
	}
}

void get_linear_address(uint32_t virtual_address, uint32_t process_num, uint64_t** address){

	uint32_t linear_address;

	uint32_t selector = virtual_address >> 25;
	uint32_t desctriptor = (virtual_address >> 27) & DESCRIPTOR_MASK;
	uint32_t offset  = virtual_address & OFFSET_MASK;

	segment seg_entry = get_segment_entry(selector, process_num, desctriptor);

	//using only first 18 bits of linear address, no need of page_directory.
	// printf("limit %u\n",seg_entry.limit);
	// printf("offset %u\n", offset);
	linear_address = seg_entry.base + offset;
	// printf("base %u\n",seg_entry.limit);
	(*address)[0] = linear_address;
	(*address)[1] = seg_entry.base;
}

void make_descriptor_modules(){
	init_GDT();
	LDT_1 = init_LDT();
	LDT_2 = init_LDT();
	LDT_3 = init_LDT();
	LDT_4 = init_LDT();
	LDT_5 = init_LDT();
	make_entry_GDT(GDT, LDTR_1, LDT_1, 1024);
	make_entry_GDT(GDT, LDTR_2, LDT_2, 1024);
	make_entry_GDT(GDT, LDTR_3, LDT_3, 1024);
	make_entry_GDT(GDT, LDTR_4, LDT_4, 1024);
	make_entry_GDT(GDT, LDTR_5, LDT_5, 1024);
	make_entry_GDT(GDT, CS, CS_START, 0x00040400); //code segment. //update code from himanshu.
	make_entry_LDT(LDT_1, DS, DS_START_1, 0x00040400); //for DS of process 1.
	make_entry_LDT(LDT_2, DS, DS_START_2, 0x00040400); //limit from 0x00040000 has to be changed to 257kb.
	make_entry_LDT(LDT_3, DS, DS_START_3, 0x00040400);
	make_entry_LDT(LDT_4, DS, DS_START_4, 0x00040400);
	make_entry_LDT(LDT_5, DS, DS_START_5, 0x00040400);
}