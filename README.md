# DSTN Assignment 1 Group 4
In this project we have implemented Multilevel Cache with Multilevel TLB and Segmentation with Paging as the Memory Subsystem model for the completion of Assignment-1 for the course Data Storage Technologies and Networks (CS F446).

# Our Question:
The memory subsystem [with TLBs, L1 Cache, L2 Cache and Main Memory] has following
configuration:
TLBs: L1 TLB with 12 entries and L2 TLB with 24 entries. Invalidation / Flush takes place at each preemption point.
L1 Cache: 8KB, 16B, 4 Way set associative cache. The cache follows Write buffer and Look Aside. It follows LRU Square Matrix as replacement policy.
L2 Cache: 32KB, 32B, 8 Way set associative cache. The cache follows Write back and Look Through. It follows LRU Counter as replacement policy.
Main Memory with Memory Management: 64MB Main memory with LRU as replacement policy. The memory management scheme used is Segmentation + Paging.

## Group Members:
1) Fenil Suchak : 2016B3A70541G
2) Himanshu Likhar : 2017A7PS0062G
3) Nidhi Zare : 2017A7PS0139G
4) Abhinav Pathak : 2017A7PS0937G

## List of files:

In this project, we have implemented Multilevel Cache with Multilevel TLB and Segmentation with Paging as the Memory Subsystem Model. 

1) tlb.h : This file contains the declarations of the functions that will be used in TLB management (both L1 and L2 TLB).

2) l1_tlb.c : It contains the implementation of functions for L1 TLB, i.e., initializing the L1 TLB, searching for an entry, flushing,  updating the TLB and calling the search function of L2 TLB (if there is a miss in L1 TLB).

3) l2_tlb.c : It contains the implementation of functions for L2 TLB i.e. functions to initialize, search, flush and update L2 TLB in case of L2 TLB miss.

4) segmentation.h : It contains the definition of the structure of a segment and the functions required for the segmentation part of Memory Subsystem.

5) segmentation.c : It contains implementation of the functions used to initialize, make entry and search the Local and Global Descriptor Tables and to generate the Linear Address from the Logical Address.

6) L1_cache.h : It declares the structure for the L1 cache module.

7) L1_cache.c : This file contains the structs for L1 cache entry, set and the write buffer required for the cache. It also contains the functions to initialize the cache, buffer, look through mechanism, read the cache, update the cache, and LRU square matrix replacement of the cache entries.

8) L2_cache.h : This file defines the L2 Cache entry and set and also declares functions for initializing the cache, updating the LRU counter, reading the cache, calling the main memory read, and updating the cache.

9) L2_cache.c : This file has the implementations for various functions that have been mentioned in the L2_cache.h.

10) make_modules.h : The header file contains information about values such as LDTRs, CS, DS and function declarations for getting the linear address, the segment entry and making the descriptor modules.

11) make_modules.c : File for initializing GDT, 5 LDTs with appropriate data filled in. These are structures - 5 local Data Segments and 1 global Code Segment.

12) main_memory.h : Defines starting and ending of GDT, LDTs, CS and DSs, along with structure for main memory. It also declares functions for initializing main memory, page tables for segments, first fit algorithm, accessing data, and internal conversions.

13) main_memory.c : Contains definitions of the functions mentioned in the main_memory.h file. It takes care of initializations, uses a first fit algorithm, implements access of data and updates the occupancy in main memory.

14) main.c : This is the driver file of the simulation. It reads addresses from input text files of the current directory and does a context switch after every 200 reads. The files that it reads contain the addresses on which the simulation will run. This function will continue reading until all the input files are read.

## Structure of the program:

The program is mainly divided into 5 parts:
Main Memory
Segmentation
Paging
TLB
Cache

Each part has a function declaration file and a function implementation file. Components like TLB and Cache have multiple levels and hence multiple files, with their names indicating their levels. All these modules are combined and used in the "main.c" file which uses all the functionality of the 5 core components to form a Memory Subsystem simulation. 
Once a virtual address is given, l1_tlb is accessed. If there is a hit, it passes the physical address. If there is a miss, it goes and checks l2_tlb, which repeats the process. If both miss, then there entries in both the TLBs are updated accordingly. The physical address is passed to L1 cache and search is performed to find the corresponding data. If L1 cache misses, it performs look through operation in L2 and consequently main memory to get the desired data. 

## Compilation and Execution of code:
To compile and execute the code, just write “make run” (without the inverted commas). The output log is stored in a file named “g4_output.txt”.

## Bugs and Implementation Issues:
TLB is currently a per-process TLB. Therefore, It cannot differentiate between segments. In the case of two segments accessing the same page number, the TLB allows one segment to access another segment’s page. Protection bits can be added to resolve this but due to brevity of time, this could not be done.


