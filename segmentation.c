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
//Initialize the GDT
void init_GDT(){
	GDT = (segment *)(malloc(sizeof(segment)*MAX_ENTRIES));
}
//Initialize the LDT and hence make a new entry in GDT of new LDT
segment* init_LDT(){
	segment* LDT = (segment *)(malloc(sizeof(segment)*MAX_ENTRIES));
	//segment *seg = (segment *)(malloc(sizeof(segment)));
	for(int i = 0;i<MAX_GDT_ENTRIES;i++){
		if(GDT[i].status & 0x10 != 1){//00010000
			GDT[i].base = LDT;
			GDT[i].limit = 0;
			GDT[i].status = 0x10; //default status bits
		}
	}
	return LDT;
}
//Make an entry in LDT with the help of selector
void make_entry_LDT(segment *LDT, uint8_t selector,uint32_t base, uint16_t limit)
{
	uint8_t index = selector & 0x78;
	if(index > MAX_LDT_ENTRIES)
		error("Cannot make more LDT entries");
	if(LDT[i].status & 0x10 != 1){
		LDT[i].base = base;
		LDT[i].limit = limit;
		LDT[i].status = 0x10; //default status bits
	}
	else
		error("Invalid attempt to make entry");
}
//Make an entry in GDT with the help of selector
void make_entry_GDT(segment *GDT, uint8_t selector, uint32_t base, uint16_t limit){

	uint8_t index = selector & 0x78;
	if(index > MAX_GDT_ENTRIES)
		error("Cannot make more GDT entries");
	if(GDT[index].status & 0x10 != 1){
		GDT[index].base = base;
		GDT[index].limit = limit;
		GDT[index].status = 0x10; //default status bits
	}
	else
		error("Invalid attempt to make entry");
}
//search for a particular entry in LDT. Go to GDT with the help of LDTR to find the start of LDT
segment search_LDT(uint8_t selector, uint8_t process_num){
	int8_t index = selector & 0x78;//01111000
	uint8_t protec = selector & 0x0003;//0000000000000011
	int8_t ldtr_index = process_num + 1;//getting the entry index from LDTR, 4 MSBs, so 01111000, MSB is 0 due to padding
	uint16_t limit = GDT[ldtr_index].limit;//length of the LDT
	if(index>limit)
		error("Address out of bound");
	segment* LDT = (segment *)(GDT[ldtr_index].base);//base address of the LDT
	if(index > GDT[ldtr_index].index)
		error("Index Out of LDT Bound");
	if(LDT[index].status & 0x0003 >= protec){//assuming lesser protection value means higher protection. So the entry being
											   //accessed must have greater or equal value
		return LDT[index];
	}
	else
		error("Please ensure that you have the proper access permissions");
}
//search for a particular entry in GDT based on the selector
segment search_GDT(uint8_t selector){
	int8_t index = selector & 0x78;//01111000
	uint8_t protec = selector & 0x0003;
	if(GDT[index].status & 0x0003 >= protec){
		return GDT[index];
	}
	else
		error("Please ensure that you have the proper access permissions");
}