/*Structure of an entry of tlb
Size of a frame = 1k = 1^10
Main memory is 64mb
No of frames = 2^26/2^10 = 2^16
Virtual address = 32 bits
Single level paging : offset = 10 bits, page no = 22 bits	
Size of entry =16 + 24 = 40 bits
But size of struct becomes 48 bits*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_L2_TLB_ENTERIES 24
#define NUM_L1_TLB_ENTERIES 12

typedef struct tlb_entry{
	uint32_t frno;//frame loc: 22 bits
	uint8_t pgno;//8 bits pgno
	uint8_t vi_bit;//1 : invalid, 0 : valid
} tlb_entry;

extern tlb_entry *l2_tlb;
extern tlb_entry *l1_tlb;

extern uint8_t l1_curr_position;
extern uint8_t l2_curr_position;

//Function to initialise tlb

void l1_tlb_initialize();
void l2_tlb_initialize();


// Function to invalidate tlb
void l1_tlb_flush();
void l2_tlb_flush();

//Boolean type function to check if the entry is found in L1 TLB
 int l1_found(uint16_t f_no, uint8_t pgno);

// Function for new entry in tlb
 void l1_tlb_update(uint16_t f_no, uint8_t pgno);
 void l1_tlb_valid_update(uint16_t old_frno,uint8_t old_pgno,uint16_t f_no,uint8_t pgno);
 void l2_tlb_valid_update(uint16_t old_frno,uint8_t old_pgno,uint16_t f_no,uint8_t pgno);
 void l2_tlb_update(uint16_t f_no, uint8_t pgno);

// Function to search in tlb
 uint32_t l1_tlb_search(uint8_t pgno);
 uint32_t l2_tlb_search(uint8_t pgno);

