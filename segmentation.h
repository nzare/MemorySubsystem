/*The logical address will have a segment selector part (10 most
significant bits). These '10 bits' will tell us which segment table
to go to (LDT or GDT) and will also tell us which entry to look for
in the descriptor table. The descriptor table will contain the entries
regarding various segments. Each entry will have the base address of
the segment, the limit (of the length of the segment) and other status
bits
So, to summarize:
32 bits: Logical adrr{
    22 bits : offset
    10 bits : selector {
        7 bits : seg number	
        1 bit : GDT(0)/LDT(1)
        2 bits: Protection Level
	}
}
Base : 32 bits
Limit : 14 bits
Granularity: 1
Sys/application : 1
Privilege Level: 2 bits 
Total : 50 bits */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAX_ENTRIES 126
typedef struct segment_entry
{
	//we will the data type later, first let us confirm
	//the sizes
	uint32_t base;
	uint16_t limit;
	uint8_t status;
	/*For 'status': 2 LSBs will be protection level, 3rd bit will be sys/app process and 4th bit will be granularity bit. 5th bit will
	tell if the entry is valid or not. Valid entry(1) means that some process that is currently running has that segment
	assigned to it. Invalid means that the entry was never alloted or the process to which it was alloted, finished
	its execution and exited.*/
} segment;
extern segment *GDT;
extern uint16_t *LDTR;
//This function is used to generate the linear address
int conv_to_linear(int log_addr);

//This function is used to make a new entry in the segment table
void make_entry_LDT(segment *LDT, uint32_t base, uint16_t limit);
void make_entry_GDT(segment *GDT, uint32_t base, uint16_t limit);

//This function is used to search for the required entry
void init_desc_tables();
segment search_GDT(uint16_t selector);
segment search_LDT(uint16_t selector);

void init_GDT();	
segment* init_LDT();