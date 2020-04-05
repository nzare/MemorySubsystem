#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define GDT_START 0x00000000
#define GDT_END 0x000000A9
#define LDT_1_START 0x000000AA
#define LDT_1_END 0x00000153
#define LDT_2_START 0x00000154
#define LDT_2_END 0x0000001FD
#define LDT_3_START 0x000001FE
#define LDT_3_END 0x000002A6
#define LDT_4_START 0x000002A7
#define LDT_4_END 0x00000350
#define LDT_5_START 0x00000351
#define LDT_5_END 0x000003FF
// The whole block above is of 1k.
#define CS_START 0x00000400 //1k is the difference to fit CS ka page table.
#define DS_START_1 0x00000800 //1k block below as well.
#define DS_START_2 0x00000C00
#define DS_START_3 0x00001000
#define DS_START_4 0x00001400
#define DS_START_5 0x00001800

extern char main_memory[0x02FFFFFF][2];

extern int occupancy[0x02FFFFFF];


void update_occupancy(int start, int finish);


uint32_t find_first_fit();

uint8_t hex2int(char ch);

void initialize_page_table_for_segment(uint32_t start_address);

void update_page_table_for_segment(uint32_t segment_base, uint8_t page_num, char* hex_address);

void address_to_hex_string(uint32_t address, char** hex_address);

uint32_t try_accessing_data(uint32_t segment_base, uint8_t page_num, uint16_t offset);

void init_memory();