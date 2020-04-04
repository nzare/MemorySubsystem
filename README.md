# DSTN Assignment 1 Question 7 by Group 4
In this project we have implementated Multilevel Cache with Multilevel TLB and Segmentation with Paging as Memory Management method for the completion of Assignment-1 for the course Data Storage Technologies and Networks (CS F446).

## Group Members:
1) Abhinav Pathak : 2017A7PS0937G
2) Fenil Suchak : 2016B3A70541G
3) Himanshu Likhar : 2017A7PS0062G
4) Nidhi Zare : 2017A7PS0139G

## List of files:
1) L1_cache.h : 

2) tlb.h : Contains declararion of the functions that will be used in TLB management (both L1 and L2 TLB).

3) segmentation.h : It contains the definition of the structure of a segment and the functions required for the segmentation part of Memory Management.

4) L1_cache.c : 

5) L2_cache_char.c

6) l1_tlb.c : It contains the implementation of the funtions for L1 TLB, i.e., initializing the L1 TLB, searching for an entry, flushing the TLB and calling of search function of L2 TLB (if there is a miss in L1 TLB).

7) l2_tlb.c : It contains implementation of functions for L2 TLB i.e. functions to initialize, search, flush and update L2 TLB in case of L2 TLB miss.

8) segmentation.c : It contains implementation of functions used to initialize, make entry and search the Local and Global Descriptor Tables and to generate the Linear Address from the Logical Address.

9) main.c : This is the driver file of the simulation. It reads addresses from input text files of the current directory and does a context switch after every 200 reads. The files that it reads contains the addresses on which the simulation will run. This function will continue reading until all the input files are read.

## Compilation of code:

## Execution of code:

## Structure of the program:
The program is mainly divided into 5 parts:
a) Main Memory
b) Segmentation
c) Paging
d) TLB
e) Cache
Each part has function declaration file and a function description file. Components like TLB and Cache have multiple levels and hence multiple files, their names indicating their levels. All these modules are combined and used in the "main.c" file which uses all the functionality of the 5 parts to form a Memory Subystem simulation.
