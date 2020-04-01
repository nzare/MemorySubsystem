/*The implemetatoin of Segmentation
So, to summarize:
32 bits: Logical adrr{
    22 bits : offset
    10 bits : selector {
        1 bit : GDT/LDT
        7 bits : seg number
        2 bits: Protection Level
	}
}
10 bits will give tell us to look into which desc table.
Then we will get the the seg number and that will give us the base and limit and other status values
Base : 32 bits
Limit : 14 bits
Granularity: 1
Sys/application : 1
Privilege Level: 2 bits 
Total : 50 bits */
#include "segmentation.h"
void init_GDT(){
	GDT = (segment)(malloc(sizeof(segment))*MAX_ENTRIES);
	return GDT;
}
segment* init_LDT(){
	segment* LDT = (segment)(malloc(sizeof(segment))*MAX_ENTRIES);
	//segment *seg = (segment *)(malloc(sizeof(segment)));
	for(int i = 0;i<MAX_ENTRIES;i++){
		if(GDT[i].status & 0x10 != 1){
			GDT[i].base = LDT;
			GDT[i].limit = 0;
			GDT[i].status = 0x10; //default status bits
		}
	}
	return LDT;
}
void make_entry_LDT(segment *LDT, uint32_t base, uint16_t limit)
{
	for(int i = 0;i<MAX_ENTRIES;i++){
		if(LDT[i].status & 0x10 != 1){
			LDT[i].base = base;
			LDT[i].limit = limit;
			LDT[i].status = 0x10; //default status bits
		}
	}
}
void make_entry_GDT(segment *GDT, uint32_t base, uint16_t limit){
	for(int i = 0;i<MAX_ENTRIES;i++){
		if(GDT[i].status & 0x10 != 1){
			GDT[i].base = base;
			GDT[i].limit = limit;
			GDT[i].status = 0x10; //default status bits
		}
	}	
}
segment search_LDT(uint16_t selector){
	int8_t index = selector & 0x03f8;//0000011111111000
	uint8_t protec = selector & 0x0003;//0000000000000011
	int8_t ldtr_index = LDTR & 0x03f8;//getting the entry index from LDTR
	segment* LDT = (segment *)(GDT[ldtr_index].base);//base address of the LDT
	uint16_t limit = GDT[ldtr_index].limit;//length of the LDT
	if(index>limit)
		error("Address out of bound");
	if(LDT[index].status & 0x0003 >= protec){//assuming lesser protection value means higher protection. So the entry being
											   //accessed must have greater or equal value
		return LDT[index];
	}
	else
		error("Please ensure that you have the proper access permissions");
}
segment search_GDT(uint16_t selector, segment *GDT){
	int8_t index = selector & 0x03f8;//0000001111111000
	uint8_t protec = selector & 0x0003;
	if(GDT[index].status & 0x0003 >= protec){
		return GDT[index];
	}
	else
		error("Please ensure that you have the proper access permissions");
}
//Convert the logical address to linear address
int conv_to_linear(int log_addr){
	uint16_t selector = log_addr >> 22;
	if(selector & 0x0002 == 1){
		segment seg = search_LDT(selector);//get the descriptor entry in descriptor tables
		uint32_t addr = seg.base + log_addr || 0x003fffff;
		if(segment.base + segment.limit < addr)
			error("Address out of bound");
		return addr;
	}
	else{
		segment seg = search_GDT(selector,GDT);
		uint32_t addr = seg.base + log_addr || 0x003fffff;
		if(segment.base + segment.limit < addr)
			error("Address out of bound");
		return addr;
	}
}