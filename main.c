#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include<stdint.h>

typedef struct va_process{
  uint32_t va;
  uint8_t process_no;
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
      
      //FILE *fp;
      //fp = fopen(filesList[curr_file],"r"); // or this

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

        unsigned int h;
        sscanf(line, "%x", &h);

        //can be used to call other functions
        //We have 32 bit unsigned integer for address and 8 bit int for processno

        call_function.va=h;
        call_function.process_no=curr_file;
      }
      
      curr_file=(curr_file+1)%n;
      if(num_completed==n){
        for (int i = 0; i < n; ++i)
        {
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

    dir = opendir("./");
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

            filesList[i] = (char*) malloc (strlen(entry->d_name)+1);
          strncpy (filesList[i],entry->d_name, strlen(entry->d_name) );
          filesList[i][strlen(entry->d_name)]='\0';
          i++;
        }
  }
  if (closedir(dir) == -1)
    {
        perror("closedir");
        return 1;
    }


  get_input_va(filesList,num_input);
  /*for(i=0;i<num_input;i++){
    printf("%s",filesList[i] );

  }*/
  
  return 0;
}
