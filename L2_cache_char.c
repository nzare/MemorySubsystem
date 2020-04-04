
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
    7 bits are used for the set index, since there are 32 sets per cache (bits 6-12).
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WORDS_PER_CACHE_LINE 16
#define BYTES_PER_CACHE_LINE 32
#define READ_ENABLE_MASK 0x1
#define WRITE_ENABLE_MASK 0x2

void l2_initialize();

void l2_cache_read(uint32_t address, char **read_data, uint8_t *status);

void l2_cache_write(uint32_t address, char *write_data, uint8_t *status);

void l2_insert_line(uint32_t address, uint32_t write_data[], 
        uint32_t *evicted_writeback_address, 
        uint32_t evicted_writeback_data[], 
        uint8_t *status);

void l2_clear_c_bits();


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
L2_CACHE_SET l2_cache[L2_NUM_CACHE_SETS];


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


/************************************************
              l2_initialize()

This procedure initializes the L2 cache
************************************************/

void l2_initialize()
{
  //CODE HERE
  for(int i = 0; i < 128; i++) 
  {
    for(int j = 0; j < 8; j++) 
    {
      
      //clearing the valid bit, and counter bits of all entries
      l2_cache[i].lines[j].v_d_c_tag = (l2_cache[i].lines[j].v_d_c_tag & 0x47FFFFFF);

      //storing 0s in the cache line
      for (int k = 0; k < 16; k++) {
        l2_cache[i].lines[j].cache_line[k][0] = "0";
        l2_cache[i].lines[j].cache_line[k][1] = "0";
      }

    }
  }

}


/**********************************************************

             update_counter_bits()

This procedure updates the counter bits of the particular
set that is mentioned in the address passed to the function.
It sets the counter value of the most recently used entry
to 111 and decrements the counter value by 1 of all the 
other entries in that set


**********************************************************/

void update_counter_bits(uint32_t temp_index, int line)
{
  l2_cache[temp_index].lines[line].v_d_c_tag = (l2_cache[temp_index].lines[line].v_d_c_tag | L2_COUNTER_MASK);
  for (int i = 0; i < 8; i++){
    if (i != line)
    {
      uint32_t temp_count = (l2_cache[temp_index].lines[i].v_d_c_tag & L2_COUNTER_MASK) >> 27;
      --temp_count;
      temp_count = (temp_count << 27);
      l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag | temp_count);
    }
  }
}


/***************************************************************

                      l2_cache_read()

This procedure implements the reading of a single byte
to the L2 cache. 

The parameters are:

address:  unsigned 32-bit address. This address can be anywhere within
          a cache line.

read_data: an output parameter (thus, a pointer to it is passed).
         On a read operation, if there is a cache hit, the appropriate
         byte of the appropriate cache line in the cache is written
         to read_data.

status: this in an 8-bit output parameter (thus, a pointer to it is 
        passed).  The lowest bit of this byte should be set to 
        indicate whether a cache hit occurred or not:
              cache hit: bit 0 of status = 1
              cache miss: bit 0 of status = 0

***************************************************************/


void l2_cache_read(uint32_t address, char **read_data, uint8_t *status)
{

  //Extract from the address the tag bits.
  //Use the L2_ADDRESS_TAG_MASK mask to mask out the appropriate
  //bits of the address and L2_ADDRESS_TAG_SHIFT to shift the 
  //bits the appropriate amount.

  //CODE HERE
  uint32_t temp_tag = (address & L2_ADDRESS_TAG_MASK) >> L2_ADDRESS_TAG_SHIFT;

  //Extract from the address the index of the cache set in the cache.
  //Use the L2_SET_INDEX_MASK mask to mask out the appropriate
  //bits of the address and L2_SET_INDEX_SHIFT to shift the 
  //bits the appropriate amount.

  //CODE HERE
  uint32_t temp_index = (address & L2_SET_INDEX_MASK) >> L2_SET_INDEX_SHIFT;

  //Extract from the address the word offset within the cache line.
  //Use the WORD_OFFSET_MASK to mask out the appropriate bits of
  //the address and WORD_OFFSET_SHIFT to shift the bits the 
  //appropriate amount.

  //CODE HERE
  uint32_t temp_word_offset = (address & WORD_OFFSET_MASK) >> WORD_OFFSET_SHIFT;

  //Extract from the address the byte offset within the cache line.

  //CODE HERE
  uint32_t temp_byte_offset = (address & BYTE_OFFSET_MASK);


  //Within the set specified by the set index extracted from the address,
  //look through the cache entries for an entry that 1) has its valid 
  //bit set AND 2) has a tag that matches the tag extracted from the address.

  //If no such entry exists in the set, then the result is a cache miss.
  //The low bit of the status output parameter should be set to 0. Look through
  //performed to get data from the Main Memory.

  //Otherwise, if an entry is found with a set valid bit and a matching tag, 
  //then it is a cache hit. The status output parameter should be set to 1.

  //The appropriate byte (as specified by the byte offset extracted from 
  //the address) of the entry's cache line data should be written to read_data.

  //CODE HERE
  for (int i = 0; i < 8; i++) 
  {
    if (((l2_cache[temp_index].lines[i].v_d_c_tag & L2_VBIT_MASK) == (1 << 31)) && ((l2_cache[temp_index].lines[i].v_d_c_tag & L2_ENTRY_TAG_MASK) == temp_tag)) 
    {
      status = 1;
      *read = l2_cache[temp_index].lines[i].cache_line[temp_word_offset][temp_byte_offset];
      update_counter_bits(temp_index, i);
    }
    
    else status = 0;

  }

  //Performing 'Look Through'
  //If the status is 0 from the for-loop, then it's a cache miss.
  //Look through the main memory for the corresponding address and get 
  //the resulting entry to L2: 
  //1) Call main_memory_read with appropriate parameters from here. 
  //2) If there is valid data in the main memory at that address,
  //fetch it and update one of the cache lines using l2_insert_line()
  //3) Copy the contents from updated line to read_data parameter from 
  //this function itself.
  //4) Else data in address is invalid and hence return an error.

  //CODE HERE

}


/***************************************************************

                      l2_cache_write()

This procedure implements the writing of a single byte
to the L2 cache. 

The parameters are:

address:  unsigned 32-bit address. This address can be anywhere within
          a cache line.

write_data: an input parameter (a pointer to it is passed).
         On a write operation, if there is a cache hit, the appropriate
         byte of the appropriate cache line in the cache is replaced
         by contents of write_data parameter.

status: this in an 8-bit output parameter (thus, a pointer to it is 
        passed).  The lowest bit of this byte should be set to 
        indicate whether a cache hit occurred or not:
              cache hit: bit 0 of status = 1
              cache miss: bit 0 of status = 0

***************************************************************/


void l2_cache_write(uint32_t address, char *write_data, uint8_t *status)
{

  //Extract from the address the tag bits.
  //Use the L2_ADDRESS_TAG_MASK mask to mask out the appropriate
  //bits of the address and L2_ADDRESS_TAG_SHIFT to shift the 
  //bits the appropriate amount.

  //CODE HERE
  uint32_t temp_tag = (address & L2_ADDRESS_TAG_MASK) >> L2_ADDRESS_TAG_SHIFT;

  //Extract from the address the index of the cache set in the cache.
  //Use the L2_SET_INDEX_MASK mask to mask out the appropriate
  //bits of the address and L2_SET_INDEX_SHIFT to shift the 
  //bits the appropriate amount.

  //CODE HERE
  uint32_t temp_index = (address & L2_SET_INDEX_MASK) >> L2_SET_INDEX_SHIFT;

  //Extract from the address the word offset within the cache line.
  //Use the WORD_OFFSET_MASK to mask out the appropriate bits of
  //the address and WORD_OFFSET_SHIFT to shift the bits the 
  //appropriate amount.

  //CODE HERE
  uint32_t temp_word_offset = (address & WORD_OFFSET_MASK) >> WORD_OFFSET_SHIFT;

  //Extract from the address the byte offset within the cache line.

  //CODE HERE
  uint32_t temp_byte_offset = (address & BYTE_OFFSET_MASK);


  //Within the set specified by the set index extracted from the address,
  //look through the cache entries for an entry that 1) has its valid 
  //bit set AND 2) has a tag that matches the tag extracted from the address.

  //If no such entry exists in the set, then the result is a cache miss.
  //The low bit of the status output parameter should be set to 0. There
  //is nothing further to do in this case.

  //Otherwise, if an entry is found with a set valid bit and a matching tag, 
  //then it is a cache hit. The status output parameter should be set to 1.

  //The value of write_data should be written to the appropriate word 
  //of the entry's cache line data and the entry's dirty bit should be set.

  //CODE HERE
  for (int i = 0; i < 8; i++) 
  {
    if (((l2_cache[temp_index].lines[i].v_d_c_tag & L2_VBIT_MASK) == (1 << 31)) && ((l2_cache[temp_index].lines[i].v_d_c_tag & L2_ENTRY_TAG_MASK) == temp_tag)) 
    {      
      status = 1;
      strcpy(l2_cache[temp_index].lines[i].cache_line[temp_word_offset][temp_byte_offset], write);
      update_counter_bits(temp_index, i);
    }

    else status = 0;

  }

  //If the status is 0 from the for-loop, then it's a cache miss.
  //Look through the main memory for the corresponding address and get 
  //the resulting entry to L2: 
  //1) Call main_memory_read with appropriate parameters from here. 
  //2) If there is valid data in the main memory at that address,
  //fetch it and update one of the cache lines using l2_insert_line()
  //3) Copy the contents from write_data parameter to the appropriate 
  //byte space in the cache line in this function itself.
  //4) Else data in address is invalid and hence return an error.

  //CODE HERE


}


/************************************************************

                 l2_insert_line()

This procedure inserts a new cache line into the L2 cache.

The parameters are:

address: 32-bit memory address for the new cache line.

write_data: an array of 32 bytes containing the 
            cache line data to be inserted into the cache.

evicted_writeback_address: a 32-bit output parameter (thus,
          a pointer to it is passed) that, if the cache line
          being evicted needs to be written back to memory,
          should be assigned the memory address for the evicted
          cache line.
          
evicted_writeback_data: an array of 32 bytes that, if the cache 
          line being evicted needs to be written back to memory,
          should be assigned the cache line data for the evicted
          cache line. Since there are 16 words per cache line, the
          actual parameter should be an array of at least 16 words.

status: this in an 8-bit output parameter (thus, a pointer to it is 
        passed).  The lowest bit of this byte should be set to 
        indicate whether the evicted cache line needs to be
        written back to memory or not, as follows:
            0: no write-back required
            1: evicted cache line needs to be written back.


 The cache replacement algorithm uses an LRU counter alogrithm. 
 A cache entry (among the cache entries in the set) is 
 chosen to be written to in the following order of preference:
    - valid bit = 0
    - counter value is lowest
*********************************************************/


void l2_insert_line(uint32_t address, char *write_data, 
		    uint32_t *evicted_writeback_address, 
		    char evicted_writeback_data[], 
		    uint8_t *status)
{

  //Extract from the address the index of the cache set in the cache.
  //see l2_cache_access above

  //CODE HERE
  uint32_t temp_index = (address & L2_SET_INDEX_MASK) >> L2_SET_INDEX_SHIFT;

  //Extract from the address the tag bits. 
  //see l2_cache_access above.

  //CODE HERE
  uint32_t temp_tag = (address & L2_ADDRESS_TAG_MASK) >> L2_ADDRESS_TAG_SHIFT;


  //In a loop, iterate through each entry in the set.
  int zero_counter = 0;  

  //LOOP STARTS HERE
  for (int i = 0; i < 8; i++) 
  {

  //If the current entry has a zero v bit, then overwrite
  //the cache line in the entry with the data in write_data,
  //set the v bit of the entry, clear the dirty bit, update the 
  //counter to 111, and write the new tag to the entry.
  //Set the low bit of the status output parameter to 0 to indicate 
  //the evicted line does not need to be written back. There is nothing 
  //further to do, the procedure can return.

  //CODE HERE
    if ((l2_cache[temp_index].lines[i].v_d_c_tag & L2_VBIT_MASK) == (0 << 31)) 
    {

      //copying data from write_data to the cache line
      for (int j=0; j<16; j++) 
      {
        l2_cache[temp_index].lines[i].cache_line[j] = write_data[j];
      }

      //setting the valid bit and updating the counter bits (111)
      l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag | L2_VBIT_MASK);
      update_counter_bits(temp_index, i);

      //clearing dirty bit
      l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag & 0xBFFFFFFF);

      //writing the new tag to entry from given address
      l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag | temp_tag);

      
      status = 0;
      return;

    }

  //  Otherwise, we remember the entry we encounter which has counter = 000
    else if (((l2_cache[temp_index].lines[i].v_d_c_tag & L2_COUNTER_MASK) >> 25) == 0x0) 
    {
      zero_counter = i;
    }


  //LOOP ENDS HERE
  }

  //When we're done looping, we choose the entry with the highest preference 
  //on the above list to evict.


  //CODE HERE
  //Don't need

  //if the dirty bit of the cache entry to be evicted is set, then the data in the 
  //cache line needs to be written back. The address to write the current entry 
  //back to is constructed from the entry's tag and the set index in the cache by:
  // (evicted_entry_tag << L2_ADDRESS_TAG_SHIFT) | (set_index << L2_SET_INDEX_SHIFT)
  //This address should be written to the evicted_writeback_address output
  //parameter. The cache line data in the evicted entry should be copied to the
  //evicted_writeback_data array.

  //Also, if the dirty bit of the chosen entry is been set, the low bit of the status byte 
  //should be set to 1 to indicate that the write-back is needed. Otherwise,
  //the low bit of the status byte should be set to 0.

  //CODE HERE
  if ((l2_cache[temp_index].lines[zero_counter].v_d_c_tag & L2_DIRTYBIT_MASK) == (1 << 30)) 
  {
    evicted_writeback_address = (((l2_cache[temp_index].lines[zero_counter].v_d_c_tag & L2_ENTRY_TAG_MASK) << L2_ADDRESS_TAG_SHIFT) | (temp_index << L2_SET_INDEX_SHIFT));
    for (int i = 0; i < 16; i++) 
    {
      evicted_writeback_data = l2_cache[temp_index].lines[zero_counter].cache_line[i];
    }

    status = 1;
  }

  else status = 0;


  //Then, copy the data from write_data to the cache line in the entry, 
  //set the valid bit of the entry, clear the dirty bit of the 
  //entry, and write the tag bits of the address into the tag of 
  //the entry.

  //CODE HERE
  //copying data from write_data to the cache line
  for (int j=0; j<16; j++) 
  {
    l2_cache[temp_index].lines[i].cache_line[j] = write_data[j];
  }

  //setting the valid bit and updating the counter bits (111)
  l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag | L2_VBIT_MASK);
  update_counter_bits(temp_index, i);

  //clearing dirty bit
  l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag & 0xBFFFFFFF);

  //writing the new tag to entry from given address
  l2_cache[temp_index].lines[i].v_d_c_tag = (l2_cache[temp_index].lines[i].v_d_c_tag | temp_tag);

  
  status = 0;
  return;

}

/***********************************************************

                l2_clear_c_bits()

This procedure clear the counter bits of each entry in 
each set of the L2 cache. It is called periodically to 
support the the LRU algorithm.

***********************************************************/
    
void l2_clear_c_bits()
{
  //CODE HERE
  for(int i = 0; i < 128; i++) 
  {
    for(int j = 0; j < 8; j++) 
    {
      
      uint32_t clear_c = 0xC7FFFFFF;
      l2_cache[i].lines[j].v_d_c_tag = (l2_cache[i].lines[j].v_d_c_tag & clear_c);

    }
  }

}

