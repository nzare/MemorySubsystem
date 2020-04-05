/*The implemetatoin of Segmentation
So, to summarize:
32 bits: Logical adrr{
	25 bits : offset
	7 bits : selector {
		4 bits : seg number
		1 bit : GDT/LDT
		2 bits: Protection Level
	}
}
10 bits will give tell us to look into which desc table.
Then we will get the the seg number and that will give us the base and limit and other status values
Base : 32 bits
Limit : 16 bits
Granularity: 1
Sys/application : 1
Privilege Level: 2 bits 
Total : 52 bits */
#include "segmentation.h"
#include "main_memory.h"

//Initialize the GDT
void init_GDT(){
	GDT = (segment *)(malloc(sizeof(segment)*MAX_GDT_ENTRIES));
}
//Initialize the LDT and hence make a new entry in GDT of new LDT
segment* init_LDT(){
	segment* LDT = (segment *)(malloc(sizeof(segment)*MAX_LDT_ENTRIES));
	return LDT;
}
//Make an entry in LDT with the help of selector
void make_entry_LDT(segment *LDT, uint8_t selector, uint64_t base, uint32_t limit)
{
	uint8_t index = selector;
	if(index > MAX_LDT_ENTRIES)
		error("Cannot make more LDT entries");
	LDT[index].base = base;
	LDT[index].limit = limit;
	LDT[index].status = 0x10; //default status bits
}
//Make an entry in GDT with the help of selector
void make_entry_GDT(segment *GDT, uint8_t selector, uint64_t base, uint32_t limit){
	uint8_t index = selector;
	if(index > MAX_GDT_ENTRIES)
		error("Cannot make more GDT entries");
	GDT[index].base = base;
	GDT[index].limit = limit;
	GDT[index].status = 0x10; //default status bits
}

//search for a particular entry in LDT. Go to GDT with the help of LDTR to find the start of LDT
segment search_LDT(uint8_t selector, uint8_t process_num){
	int8_t index = (selector & 0x78) >> 3;//01111000
	uint8_t protec = selector & 0x0003;//0000000000000011
	int8_t ldtr_index = process_num + 1;//getting the entry index from LDTR, 4 MSBs, so 01111000, MSB is 0 due to padding
	uint16_t limit = GDT[ldtr_index].limit;//length of the LDT
	if(index>limit)
		error("Address out of bound");
	segment* LDT = (segment *)(GDT[ldtr_index].base);//base address of the LDT
	if(index > (GDT[ldtr_index].limit))
		error("Index Out of LDT Bound");
	return LDT[index];
}
//search for a particular entry in GDT based on the selector
segment search_GDT(uint8_t selector){
	int8_t index = selector & 0x78;//01111000
	uint8_t protec = selector & 0x0003;
	return GDT[index];
	if(GDT[index].status & 0x0003 >= protec){
		return GDT[index];
	}
	else
		error("Please ensure that you have the proper access permissions");
}