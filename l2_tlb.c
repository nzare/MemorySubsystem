#include "tlb.h"

// Invalidate tlb
void l2_tlb_flush(){
  int i=0;
  for(i=0;i< NUM_L2_TLB_ENTERIES; i++){
    l2_tlb[i].vi_bit  = 1;
  }
  // while(i<NUM_L2_TLB_ENTERIES){
  //   l2_tlb[i] = l1_tlb[i];
  //   i++;
  // }
}

// All entries in tlb are invalid in the beginning
void l2_tlb_initialize(){
  l2_curr_position=0;
  l2_tlb = (tlb_entry *) malloc(sizeof(tlb_entry) * NUM_L2_TLB_ENTERIES);
  l2_tlb_flush();
}

// Update tlb entry
void l2_tlb_update(uint16_t f_no, uint8_t pgno){
 
  for(int i=0;i<NUM_L2_TLB_ENTERIES;i++){
    if(l2_tlb[i].vi_bit!=0){     
      l2_tlb[i].pgno = pgno;
      l2_tlb[i].frno = f_no;
      l2_tlb[i].vi_bit = 0;
      l1_tlb_update(l2_tlb[i].frno,l2_tlb[i].pgno);
      return;
    }
  }  
  uint8_t replace  = (l2_curr_position+1) % NUM_L2_TLB_ENTERIES;

  if(l1_found(l2_tlb[replace].frno, l2_tlb[replace].pgno)==1){
    l1_tlb_valid_update(l2_tlb[replace].frno,l2_tlb[replace].pgno, f_no, pgno);
  }
  
  l2_tlb[replace].frno = f_no;
  l2_tlb[replace].pgno=pgno;
  l2_tlb[replace].vi_bit = 0;
  return;
 

}
void l2_tlb_valid_update(uint16_t old_frno,uint8_t old_pgno,uint16_t f_no,uint8_t pgno){

  for(int i=0;i< NUM_L2_TLB_ENTERIES ;i++){
    if(l2_tlb[i].vi_bit!=0)
      continue;

    if(l2_tlb[i].frno==old_frno && l2_tlb[i].pgno==old_pgno){    
      l2_curr_position = i ;
      l2_tlb[i].frno = f_no;
      l2_tlb[i].pgno=pgno;
      l2_tlb[i].vi_bit=0;
      return;        
    }


  }

}
uint32_t l2_tlb_search(uint8_t pgno){
 
  for(int i=0;i< NUM_L2_TLB_ENTERIES ;i++){

  if(l2_tlb[i].vi_bit!=0)
    continue;

    if(l2_tlb[i].pgno==pgno){
      l2_curr_position = i ;
      l1_tlb_update(l2_tlb[i].frno,l2_tlb[i].pgno);
      return l2_tlb[i].frno;
    }
  }
  return 0;
}
