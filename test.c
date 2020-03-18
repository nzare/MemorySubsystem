#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include "tlb.h"
#include "l1_tlb.c"
#include "l2_tlb.c"

/*only for testing*/
int main(){

	l1_tlb_initialize();
	l1_tlb_flush();
	tlb_entry l;
	printf("test\n");

	return 0;
}