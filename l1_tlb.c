#include "tlb.h"

tlb_entry *l1_tlb;
uint8_t l1_curr_position = 0;

// All entries in tlb are invalid in the beginning
// Invalidate tlb
void l1_tlb_flush(){
  for(int i=0;i< NUM_L1_TLB_ENTERIES; i++){
    l1_tlb[i].pgno[2] = (l1_tlb[i].pgno[2])|00000010;
  }
 
}

//Initialize the L1 TLB with all entries as invalid
void l1_tlb_initialize(){
 
  l1_tlb = (tlb_entry *) malloc(sizeof(tlb_entry) * NUM_L1_TLB_ENTERIES);
  l1_tlb_flush();
}

// Update tlb entry
void l1_tlb_update(uint16_t f_no, uint8_t pgno[3]){
  //replacing the first invalid entry
  for(int i=0;i<NUM_L1_TLB_ENTERIES;i++){
    if((l1_tlb[i].pgno[2]&00000010)!=0){

      for(int j=0;j<3;j++){
       
        l1_tlb[i].pgno[j] = pgno[j];

      }
      l1_tlb[i].frno = f_no;
      l1_tlb[i].pgno[2] = l1_tlb[i].pgno[2]&11111101;
      return;
    }
  }
  // If no invalid entry in the TLB, update the L2 TLB with the entry that we will be replacing and replace the entry with the new entry
  uint8_t to_replace  = (l1_curr_position+1) % NUM_L1_TLB_ENTERIES;

  l2_tlb_valid_update(l1_tlb[to_replace].frno,l1_tlb[to_replace].pgno, f_no, pgno);
  // Replace the entry in L1 TLB
  for(int j=0;j<3;j++){
       
    l1_tlb[to_replace].pgno[j] = pgno[j];

  }
  l1_tlb[to_replace].frno = f_no;
  // Make sure it is valid
  l1_tlb[to_replace].pgno[2] = l1_tlb[to_replace].pgno[2]&11111101;
  // change the position of the entry number to be replaced for the next time such situation occurs
  l1_curr_position = (l1_curr_position+1) % NUM_L1_TLB_ENTERIES;
  return;
}
// Search for the corresponding page number-frame number entry in the TLB
uint16_t l1_tlb_search(uint8_t pgno[3]){
 
  for(int i=0;i< NUM_L1_TLB_ENTERIES ;i++){

  if((l1_tlb[i].pgno[2]&00000010)!=0) continue;

    if(l1_tlb[i].pgno[0]==pgno[0]
      && l1_tlb[i].pgno[1]==pgno[1]
      && (l1_tlb[i].pgno[2]&11111100)==(pgno[2]&11111100)){
    
        l1_curr_position = i ;
        return l1_tlb[i].frno;
    }
  }
  // If the entry is not found, search it in L2 TLB
  return l2_tlb_search(pgno);
}

//Function for checking if the entry is there in L1 TLB
int l1_found(uint16_t f_no, uint8_t pgno[3]){

  for(int i=0;i< NUM_L1_TLB_ENTERIES ;i++){

  if((l1_tlb[i].pgno[2]&00000010)!=0)
    continue;

    if(l1_tlb[i].frno==f_no && l1_tlb[i].pgno[0]==pgno[0]
      && l1_tlb[i].pgno[1]==pgno[1]
      && (l1_tlb[i].pgno[2]&11111100)==(pgno[2]&11111100))
        return 1;
  }
  return 0;
}

//When there is a TLB miss in both L1 and L2 TLB, we will replace an entry in L2 TLB with a new, correct one. So, if this replaced entry
//is also a part of L1 TLb, we need to update it with the latest one. This function does that.
void l1_tlb_valid_update(uint16_t old_frno,uint8_t old_pgno[3],uint16_t f_no,uint8_t pgno[3]){

  for(int i=0;i< NUM_L1_TLB_ENTERIES ;i++){
  if((l1_tlb[i].pgno[2]&00000010)!=0)
    continue;
  if(l1_tlb[i].frno==old_frno && l1_tlb[i].pgno[0]==old_pgno[0]
      && l1_tlb[i].pgno[1]==old_pgno[1]
      && (l1_tlb[i].pgno[2]&11111100)==(old_pgno[2]&11111100)){
    
        l1_curr_position = i ;
        l1_tlb[i].frno = f_no;

        for(int j=0;j<3;j++){
         
          l1_tlb[i].pgno[j]=pgno[j];
       
        }
        l1_tlb[i].pgno[2]=pgno[2]&11111101;
        return;
  }
  }
return;
}

