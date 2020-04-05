#include "tlb.h"

// All entries in tlb are invalid in the beginning
// Invalidate tlb
void l1_tlb_flush(){
  for(int i=0;i< NUM_L1_TLB_ENTERIES; i++){
    l1_tlb[i].vi_bit = 1;//1 is for invalid
  }
 
}

//Initialize the L1 TLB with all entries as invalid
void l1_tlb_initialize(){
  l1_curr_position=0;
  l1_tlb = (tlb_entry *) malloc(sizeof(tlb_entry) * NUM_L1_TLB_ENTERIES);
  l1_tlb_flush();
}

// Update tlb entry
void l1_tlb_update(uint16_t f_no, uint8_t pgno){
  //replacing the first invalid entry
  for(int i=0;i<NUM_L1_TLB_ENTERIES;i++){
    if(l1_tlb[i].vi_bit!=0){
      l1_tlb[i].frno = f_no;
      l1_tlb[i].pgno = pgno;
      l1_tlb[i].vi_bit = 0;
      return;
    }
  }
  // If no invalid entry in the TLB, update the L2 TLB with the entry that we will be replacing and replace the entry with the new entry
  uint8_t to_replace  = (l1_curr_position+1) % NUM_L1_TLB_ENTERIES;

  l2_tlb_valid_update(l1_tlb[to_replace].frno,l1_tlb[to_replace].pgno, f_no, pgno);
  // Replace the entry in L1 TLB
  l1_tlb[to_replace].pgno = pgno;
  l1_tlb[to_replace].frno = f_no;
  // Make sure it is valid
  l1_tlb[to_replace].vi_bit = 0;
  // change the position of the entry number to be replaced for the next time such situation occurs
  l1_curr_position = (l1_curr_position+1) % NUM_L1_TLB_ENTERIES;
  return;
}
// Search for the corresponding page number-frame number entry in the TLB
uint32_t l1_tlb_search(uint8_t pgno){
 
  for(int i=0;i< NUM_L1_TLB_ENTERIES ;i++){

    if(l1_tlb[i].vi_bit!=0)
      continue;

    if(l1_tlb[i].pgno==pgno){
    	//printf("THE PAGE NUMBER IS %u\n",pgno);
    
        l1_curr_position = i ;
        //printf("FOUND IN L1 TLB\n");
        return l1_tlb[i].frno;
    }
  }
  // If the entry is not found, search it in L2 TLB
  return l2_tlb_search(pgno);
}

//Function for checking if the entry is there in L1 TLB
int l1_found(uint16_t f_no, uint8_t pgno){

  for(int i=0;i< NUM_L1_TLB_ENTERIES ;i++){

    if(l1_tlb[i].vi_bit!=0)
      continue;

    if(l1_tlb[i].frno==f_no && l1_tlb[i].pgno==pgno)
        return 1;
  }
  return 0;
}

//When there is a TLB miss in both L1 and L2 TLB, we will replace an entry in L2 TLB with a new, correct one. So, if this replaced entry
//is also a part of L1 TLb, we need to update it with the latest one. This function does that.
void l1_tlb_valid_update(uint16_t old_frno,uint8_t old_pgno,uint16_t f_no,uint8_t pgno){

  for(int i=0;i< NUM_L1_TLB_ENTERIES ;i++){
    if(l1_tlb[i].vi_bit!=0)
      continue;
    if(l1_tlb[i].frno==old_frno && l1_tlb[i].pgno==old_pgno){
      
      l1_curr_position = i ;
      l1_tlb[i].frno = f_no;
      l1_tlb[i].pgno=pgno;
      l1_tlb[i].vi_bit=0;
      return;
    }
  }
  return;
}

