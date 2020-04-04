#define OFFSET_MASK 0x01FFFFFF
#define DESCRIPTOR_MASK 1

extern int LDTR_1;
extern int LDTR_2;
extern int LDTR_3;
extern int LDTR_4;
extern int LDTR_5;
extern int CS;
extern int DS;

segment get_segment_entry(uint32_t selector, uint32_t process_num, int descriptor);

uint32_t* get_linear_address(uint32_t virtual_address, uint32_t process_num);

void try_accessing_data(uint32_t segment_base, uint8_t page_num, uint16_t offset);

