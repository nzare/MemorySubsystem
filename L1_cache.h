#define LINES_PER_SET 4 //4 way set associative
#define NUM_SETS 128	 
#define TAG_BITS 21
#define REPL_BITS 4 //Square matrix implmentation
#define WORDS_IN_CACHE_LINE 8 // 4 bits is a word , therefore 8 words is 16B.
#define WORD_OFFSET_BITS 3
#define BLOCK_OFFSET_BITS 1
#define L1_VBIT_MASK (1 << 31)
#define L1_RBIT_MASK (1 << 30) //Might not be needed
#define SET_BITS_MASK (0x000007f)
#define WORDS_OFFSET_MASK (0x00000007)
#define BYTE_OFFSET_MASK (0x00000001)
#define MAX_BUFFER_SIZE 8 // 8 bytes.

extern uint8_t square_mat_arr[4];

void l1_initialize();

int mm_cache_read();

void buffer_init();

int l1_cache_read(uint32_t address);

int look_through_read(uint32_t address);

void l1_cache_select_lru_and_replace(uint32_t address);

void init_square_mat_array();

// int l1_cache_write(uint32_t address, char* write);

// void add_into_write_through_buffer(uint32_t address, char* write);

// void actual_write_through();

//Struct for Cache Module

typedef struct {
  uint32_t v; //Implementing Write through buffer, so no chance of dirty bit.
  uint32_t tag_bits; 
  uint32_t replacement_policy_bits;
  char* cache_line[WORDS_IN_CACHE_LINE][2];
} L1_CACHE_ENTRY;

typedef struct {
	L1_CACHE_ENTRY lines[LINES_PER_SET];
} L1_CACHE_SET;

//Struct for write buffer
typedef struct{
	char* data[8];
	int head;
	int tail;
	int size;
	uint32_t address[8];
}WRITE_BUFFER;

typedef struct{
	uint32_t address;
	char** read;
}read_packet;


//Initializing Cache Module as an array of num sets.
extern L1_CACHE_SET l1_cache[NUM_SETS];

extern WRITE_BUFFER write_buff;