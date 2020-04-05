#define OFFSET_MASK 0x0003FFFF
#define DESCRIPTOR_MASK 1

extern int LDTR_1;
extern int LDTR_2;
extern int LDTR_3;
extern int LDTR_4;
extern int LDTR_5;
extern int CS;
extern int DS;

segment get_segment_entry(uint32_t selector, uint32_t process_num, int descriptor);

void get_linear_address(uint32_t virtual_address, uint32_t process_num, uint64_t** hex_address);

void make_descriptor_modules();

