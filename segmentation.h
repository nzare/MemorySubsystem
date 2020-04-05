/*The implemetatoin of Segmentation
So, to summarize:
32 bits: Logical adrr{
	25 bits : offset
	7 bits : selector {
		1 bit : GDT/LDT
		4 bits : seg number
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
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAX_LDT_ENTRIES 15
#define MAX_GDT_ENTRIES 127
typedef struct segment_entry
{
	uint64_t base;//base adress of the segment
	uint32_t limit;//length of the segment
	uint8_t status;
	/*For 'status': 2 LSBs will be protection level, 3rd bit will be sys/app process and 4th bit will be granularity bit. 5th bit will
	tell if the entry is valid or not. Valid entry(1) means that some process that is currently running has that segment
	assigned to it. Invalid means that the entry was never alloted or the process to which it was alloted, finished
	its execution and exited.*/
} segment;
//Global Descriptor Table
extern segment *GDT;//Local Descriptor Table Register
extern segment *LDT_1;//Local Descriptor Table Regis
extern segment *LDT_2;//Local Descriptor Table Regis
extern segment *LDT_3;//Local Descriptor Table Regis
extern segment *LDT_4;//Local Descriptor Table Regis
extern segment *LDT_5;//Local Descriptor Table Regis

//These 2 functions are used to make a new entry in the Local and Global segment table respectively
void make_entry_LDT(segment *LDT,uint8_t selector ,uint64_t base, uint32_t limit);
void make_entry_GDT(segment *GDT,uint8_t selector ,uint64_t base, uint32_t limit);

//These two functions are used to search for the required entry in GDT and LDT respectively
segment search_GDT(uint8_t selector);
segment search_LDT(uint8_t selector, uint8_t process_num);
//These two functions are used to initialize the GDT and LDT respectively
void init_GDT();	
segment* init_LDT();