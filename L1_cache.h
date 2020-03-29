#include <stdio.h>
#include <stdlib.h>
#include "L1_cache.h"


//Struct for Cache Module
typedef struct {
  uint32_t v; //Implementing Write through buffer, so no chance of dirty bit.
  uint32_t tag;
  uint32_t cache_line[WORDS_IN_CACHE_LINE];
} L1_CACHE_MODULE;