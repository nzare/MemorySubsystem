/*Structure of an entry of tlb
Size of a frame = 4k = 2^12
Main memory is 64mb
No of frames = 2^26/2^12 = 2^14
Virtual address = 32 bits
Single level paging : offset = 12 bits, page no = 20bits
Size of entry =16 + 24 = 40 bits
But size of struct becomes 48 bits*/

#pragma once

#define NUM_L2_TLB_ENTERIES 24
#define NUM_L1_TLB_ENTERIES 12

typedef struct tlb_entry{
	uint16_t frno;  //frame no
	uint8_t pgno[3];  //20 bits pgno, 1 bit valid/invalid, 3 bits are unused
} tlb_entry;

//Function to initialise tlb

void tlb_initialize();

// Function to invalidate tlb
void tlb_flush();

//Boolean type function to check if the entry is found in L1 TLB
int l1_found(uint16_t f_no, uint8_t pgno[3]);

// Function for new entry in tlb
void l1_tlb_update(uint16_t f_no, uint8_t pgno[3]);
void l1_tlb_valid_update(uint16_t old_frno,uint8_t old_pgno[3],uint16_t f_no,uint8_t pgno[3]);
void l2_tlb_valid_update(uint16_t old_frno,uint8_t* old_pgno,uint16_t f_no,uint8_t* pgno);
void l2_tlb_invalid_update(uint16_t f_no,uint8_t pgno[3]);

// Function to search in tlb
uint16_t l1_tlb_search(uint8_t pgno[3]);
uint16_t l2_tlb_search(uint8_t *pgno);


// Function to call main memory after l2 tlb miss
uint16_t call_main_memory(uint8_t pgno[3]);
