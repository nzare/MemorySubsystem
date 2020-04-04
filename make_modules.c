/*File for initializing GDT, 5 LDTs with appropriate data filled in. These are structs.
  5 Local Data Segments and One global code segment.
*/


#include "segmentation.h"
#include "main_memory.h"
#include "make_modules.h"

int LDTR_1 = 2;
int LDTR_1 = 3;
int LDTR_2 = 4;
int LDTR_3 = 5;
int LDTR_4 = 6;
int LDTR_5 = 7;
int CS = 1; //selector for segment for all process is the same, 1st entry of GDT gives address of code segment. This info is inferred extracted from sir's file.
int DS = 7; //selector for all process is the same, 7th entry of LDT gives address of data segment. This info is inferred extracted from sir's file.



segment get_base_address(uint32_t selector, uint32_t process_num, int descriptor){
	uint32_t base_address;
	if(desctriptor == 1){ //search LDT
		segment LDT_entry = search_LDT(selector, process_num); //get address of segment. process num acts as value of ldtr.
		return LDT_entry;
	}
	else{ //access GDT ie. code segment
		segment GDT_entry = search_GDT(CS);
		return GDT_entry;
	}
}

int main(){
	GDT = init_GDT();
	segment* LDT_1 = init_LDT();
	segment* LDT_2 = init_LDT();
	segment* LDT_3 = init_LDT();
	segment* LDT_4 = init_LDT();
	segment* LDT_5 = init_LDT();
	make_entry_GDT(GDT, LDT_1, 30, 1);
	make_entry_GDT(GDT, LDT_2, 30, 2);
	make_entry_GDT(GDT, LDT_3, 30, 3);
	make_entry_GDT(GDT, LDT_4, 30, 4);
	make_entry_GDT(GDT, LDT_5, 30, 5); //30 is in bytes
	make_entry_GDT(GDT, CS_START , 0x00040000, CS); //code segment. //update code from himanshu.
	make_entry_LDT(LDT_1, DS_START_1, 0x00040000, DS); //for DS of process 1.
	make_entry_LDT(LDT_2, DS_START_2, 0x00040000, DS); //limit from 0x00040000 has to be changed to 257kb.
	make_entry_LDT(LDT_3, DS_START_3, 0x00040000, DS);
	make_entry_LDT(LDT_4, DS_START_4, 0x00040000, DS);
	make_entry_LDT(LDT_5, DS_START_5, 0x00040000, DS);


	// Take input as virtual address from file and I'm assuming I have it here.
	// I have information of process number also.

	uint32_t virtual_address; //hex format
	uint32_t process_num;
	uint32_t linear_address;

	uint32_t selector = virtual_address >> 28;
	uint32_t desctriptor = (virtual_address >> 27) & DESCRIPTOR_MASK;
	uint32_t offset  = virtual_address & OFFSET_MASK;

	segment seg_entry = get_linear_address(selector, process_num, offset, desctriptor);

	//using only first 18 bits of linear address, no need of page_directory.

	if(offset <= seg_entry.limit){
		linear_address = seg_entry.base + offset;
	else{
		error("Error -- you are off the limits");
	}

	uint8_t page_index = (linear_address >> 10) & 0xFF;

	uint32_t page_offset = linear_address & 0x3FF;

	







}