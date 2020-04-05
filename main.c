#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include<stdint.h>

#include "segmentation.h"
#include "main_memory.h"
#include "make_modules.h"
#include "L1_cache.h"
#include "L2_cache.h"
#include "segmentation.h"
#include "tlb.h"


uint8_t square_mat_arr[4];

L1_CACHE_SET l1_cache[NUM_SETS];
L2_CACHE_SET l2_cache[L2_NUM_CACHE_SETS];

tlb_entry *l2_tlb;
tlb_entry *l1_tlb;

uint8_t l1_curr_position;
uint8_t l2_curr_position;


int LDTR_1 = 2;
int LDTR_2 = 3;
int LDTR_3 = 4;
int LDTR_4 = 5;
int LDTR_5 = 6;
int CS = 1; //selector for segment for all process is the same, 1st entry of GDT gives address of code segment. This info is inferred extracted from sir's file.
int DS = 7; //selector for all process is the same, 7th entry of LDT gives address of data segment. This info is inferred extracted from sir's file.

segment* GDT;
segment* LDT_1;
segment* LDT_2;
segment* LDT_3;
segment* LDT_4;
segment* LDT_5;

char main_memory[0x02FFFFFF][2];

int occupancy[0x02FFFFFF];

typedef struct va_process{

  uint32_t va;
  uint32_t process_no;

}va_process;

void get_input_va(char* filesList[],int n){

  //Initialization of Descriptors
  make_descriptor_modules();
  // //Initializing Main Memory
  init_memory();

  l1_tlb_initialize();
  l2_tlb_initialize();

  l1_initialize(); //Initializing L1 cache

  init_square_mat_array();

  l2_initialize(); //Initializing L2 cache

  uint32_t frno;
  uint64_t* address;
  //uint8_t page_num;
  //uint32_t physical_address;
  int file_pos[n];

   for(int i=0;i<n;i++)
     file_pos[i]=0;

   int num_completed=0;
   int curr_file=0;

   FILE *fp[n];

   for(int i=0;i<n;i++){
     fp[i] = fopen(filesList[i],"r");
   }

  // printf("hereeeeee\n");
int count=0;
//uint32_t va;
//uint32_t process_no;
address = malloc(2 * sizeof(uint64_t));
//uint32_t page_offset;
  while(1){
    struct va_process call_function;
    l1_tlb_flush();
    l2_tlb_flush();

    for(int i=0;i<200;i++){
        count+=1;

       if(file_pos[curr_file]==-1){
         break;
       }

       char line[9];
       fseek( fp[curr_file], file_pos[curr_file], SEEK_SET );
       if(fgets(line,9, fp[curr_file])==NULL){
         printf("File completed\n");
         num_completed+=1;
         file_pos[curr_file]=-1;
         break;
       }
       file_pos[curr_file]+=(10);
      
      printf("%s : %d\n",line,curr_file);

      unsigned int curr_address;
      sscanf(line, "%x", &curr_address);

      call_function.va = curr_address; //This is virtual address
      call_function.process_no = curr_file; // This is process numeber.



      //can be used to call other functions
      //We have 32 bit unsigned integer for address and 8 bit int for processn

      address = malloc(2 * sizeof(uint64_t));
      get_linear_address(call_function.va, call_function.process_no + 1, &address);
      
      uint8_t page_num = (address[0] >> 10) & 0xFF;

      uint32_t page_offset = address[0] & 0x3FF;

      frno = l1_tlb_search(page_num);
      if(frno==0){
           printf("Tlb miss occured, Getting Frame From Memory & Updating TLB\n");
           frno = try_accessing_data(address[1], page_num, page_offset);//derive frame number
           l2_tlb_update(frno,page_num);
       }
       printf("TLB Hit has occured, passing physical address to caches\n");
      // frno is frame no got 

      //now we go to cache assumming TLB hit.
      uint32_t physical_address = frno; // This is assuming TLB returned actual physical address of frame & now added to it is page_offset to give physical address.

      
      physical_address = (physical_address << 10) + page_offset;


      int status = l1_cache_read(physical_address);

      if(status == 0){ // not in cache, get from above levels and update.
        printf("Data not found any where, weirdly Incorrect address\n");
      }
  }  
      //sleep(2);
    
     curr_file=(curr_file+1)%n;
     if(num_completed==n){
       for (int i = 0; i < n; ++i){
           fclose(fp[i]);
       }
       return;
     }
  }

}

int main(int argc, char *argv[])
{
    //Write output to text file
    FILE* out = freopen("g4_output.txt","w",stdout);
    DIR *dir;
 
    struct dirent *entry;
 
    int pos;
 
    dir = opendir("./Input_files/");
    int num_input =0;
 
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type != DT_REG)
            continue;
 
        pos = strlen(entry->d_name) - 4;
 
        if (! strcmp(&entry->d_name[pos], ".txt"))
        {
            num_input++;
            printf("%d\n",num_input);
        }
    }
 
    rewinddir(dir);
 
  char *filesList[num_input];
 
  int i=0;
 
  
  
 
  //Put file names into the array
  while ((entry = readdir(dir)) != NULL){
      if (entry->d_type != DT_REG)
            continue;
 
        pos = strlen(entry->d_name) - 4;
 
        if (! strcmp(&entry->d_name[pos], ".txt"))
        {
 
          char path[100] = "Input_files/";
          filesList[i] = (char*) malloc (strlen(entry->d_name)+strlen(path)+1);
          strcat(path,entry->d_name);
          strncpy (filesList[i],path, strlen(path) );
          filesList[i][strlen(path)]='\0';
          i++;
        }
  }
  if (closedir(dir) == -1)
    {
        perror("closedir");
        return 1;
    }
 
 
  l1_tlb_initialize();
  l2_tlb_initialize();
  get_input_va(filesList,num_input);
  fclose(out);
  //printf("Please Check 'g4_output.txt' for output log\n");
  return 0;
}
