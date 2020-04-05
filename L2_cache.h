
/************************************************************

  This file contains the code for the L2 cache. It is a 32KB, 32B, 
  8 Way set associative with Write Back, Look Through mechanisms, 
  and LRU Counter as its replacement policy

   Given cache line size = 32B = 16 words per cache line
   32KB = 16K Words = 1K cache lines = 128 sets
   
   Each cache line has: valid bit, reference bit, dirty bit,
                        tag, and cache-line data.

    An address is decomposed as follows (from lsb to msb):
    2 bits are used for byte offset within a word (bits 0-1)
    4 bits are used for word offset within a cache line (bits 2-5)
    7 bits are used for the set index, since there are 128 sets per cache (bits 6-12).
    19 bits remaining are used as the tag (bits 13-31)

   Therefore, 32-bit address is decomposed as follows:

          19             7            4        2
    ------------------------------------------------
   |     tag      |     set      | word   |  byte  |
   |              |    index     | offset | offset |
    ------------------------------------------------

   Each cache entry is structured as follows:

    1 1    3        8      19 
    ------------------------------------------------------
   |v|d|counter|reserved| tag |  16-word cache line data |
    ------------------------------------------------------

**************************************************************/

#define WORDS_PER_CACHE_LINE 16
#define BYTES_PER_CACHE_LINE 32

extern void l2_initialize();

extern int l2_mm_cache_read (uint32_t address);

extern int l2_cache_read(uint32_t address);

extern int l2_look_through_read(uint32_t address);

extern void l2_cache_select_lru_and_replace(uint32_t address);

extern void l2_update_counter_bits(uint32_t temp_index, int line);


/***************************************************
This struct defines the structure of a single cache
entry in the L2 cache. It has the following fields:
  v_d_c_tag: 32-bit unsigned word containing the 
           valid (v) bit at bit 31 (leftmost bit),
           the dirty bit (d) at bit 30, counter bits 
           in bits 27 through 29, and the tag in 
           bits 0 through 18 (the 19 rightmost bits)
  cache_line: an array of 16 words, constituting a single
              cache line.
****************************************************/

typedef struct {
  uint32_t v_d_c_tag;
  char *cache_line[WORDS_PER_CACHE_LINE][2];
} L2_CACHE_ENTRY;

//8-way set-associative cache, so there are
//8 cache lines per set.
#define L2_LINES_PER_SET 8


/***************************************************
  This structure defines an L2 cache set. Its only
  field, lines, is an array of eight cache lines.
***************************************************/

typedef struct {
  L2_CACHE_ENTRY lines[L2_LINES_PER_SET];
} L2_CACHE_SET;

//There are 128 sets in the L2 cache
#define L2_NUM_CACHE_SETS 128

//The l2 cache itself is just an array of 128 cache sets.
extern L2_CACHE_SET l2_cache[L2_NUM_CACHE_SETS];


/***************************************************
  Constants for cache entry structure
***************************************************/

//Mask for v bit: Bit 31 of v_d_c_tag
#define L2_VBIT_MASK (1 << 31)

//Mask for d bit: Bit 30 of v_d_c_tag
#define L2_DIRTYBIT_MASK (1 << 30)

//Mask for counter bits: Bits 27-29 of v_d_c_tag
#define L2_COUNTER_MASK 0x38000000

//Mask for tag bits: Bits 0-18 of v_d_c_tag
#define L2_ENTRY_TAG_MASK 0x0007FFFF


/***************************************************
  Constants for 32-bit address
***************************************************/

//The upper 19 bits (bits 13-31) of an address are used as the tag bits
//In binary, the mask is 1111 1111 1111 1111 1110 0000 0000 0000
#define L2_ADDRESS_TAG_MASK 0xFFFFE000

//After masking to extract the tag from the address, it needs to 
//be shifted right by 13.
#define L2_ADDRESS_TAG_SHIFT 13

//Bits 6-12 are used to extract the set index from an address.
//In binary, the mask is 1111 1100 0000
#define L2_SET_INDEX_MASK 0x00000FC0

//After masking to extract the set index from an address, it
//needs to be shifted to the right by 6
#define L2_SET_INDEX_SHIFT 6

//Bits 2-5 of an address specifies the offset of the addressed
//word within the cache line
//In binary, the mask is 0011 1100
#define WORD_OFFSET_MASK 0x0000003C

//After masking to extract the word offset, it needs
//to be shifted to the right by 2.
#define WORD_OFFSET_SHIFT 2

//Bits 0-1 of an address specifies the offset of the addressed
//byte within the cache line
//In binary, the mask is 0011
#define BYTE_OFFSET_MASK 0x00000003