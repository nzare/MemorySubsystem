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
#include "segmentation.h"
#include "tlb.h"

typedef struct va_process{

  uint32_t va;
  uint32_t process_no;

}va_process;

void get_input_va(char* filesList[],int n){
  
  int file_pos[n];

    for(int i=0;i<n;i++)
      file_pos[i]=0;

    int num_completed=0;
    int curr_file=0;

    FILE *fp[n];

    for(int i=0;i<n;i++){
      fp[i] = fopen(filesList[i],"r");
    }

    while(1){

      struct va_process call_function; //maybe this was causing the problem

      for(int i=0;i<200;i++){

        if(file_pos[curr_file]==-1){
          break;
        }

        char line[9];

        fseek( fp[curr_file], file_pos[curr_file], SEEK_SET );
        
        if(fgets(line,9, fp[curr_file])==NULL){
          //printf("File completed\n");
          num_completed+=1;
          file_pos[curr_file]=-1;
          break;
        }

        file_pos[curr_file]+=(10);
        
        printf("%s : %d\n",line,curr_file);

        unsigned int curr_address;
        sscanf(line, "%x", &curr_address);

        //can be used to call other functions
        //We have 32 bit unsigned integer for address and 8 bit int for processno

        call_function.va = curr_address; //This is virtual address
        call_function.process_no = curr_file; // This is process numeber.

        //Cache Lite, TLB lite abhi.
        uint8_t pg[3];
        pg[0] = (curr_address & 0xFF000000) >> 24;
        pg[1] = (curr_address & 0x00FF0000) >> 16;
        pg[2] = (curr_address & 0x0000FF00) >> 8;


        uint16_t fr = l1_tlb_search(pg);

        if(fr!=0){
          printf("Frame no found in tlb\n");
        }

        else{
        uint32_t* address;

        address = get_linear_address(call_function.va, call_function.process_no);
        address = get_linear_address(call_function.va, call_function.process_no);

        uint8_t page_num = (address[0] >> 10) & 0xFF;

        uint32_t page_offset = address[0] & 0x3FF;

        try_accessing_data(address[1], page_num, page_offset); //Directly go to main memmory.

        fr = (address[1]& 0x03FFFFFF)  >> 10;

        l2_tlb_update(fr,pg);

      }

       
      
      }
      
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

  for(i=0;i<num_input;i++){
    printf("%s\n",filesList[i] );
  }

  l1_tlb_initialize();
  l2_tlb_initialize();
 get_input_va(filesList,num_input);
  
  return 0;
}
