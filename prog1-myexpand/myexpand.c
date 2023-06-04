#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
     int i = argc;
//sets default case
int spaceAmount = 8;

if(argc == 1){
//checks arguments
}else if (argv[1][0] == '-'){
     if(argv[1][1] == 't'){
//changes default case to requested amount
          spaceAmount = atoi(argv[2]);
//prints argument errors
     }else{
          fprintf(stderr, "Invalid Argument: -t expected\n");
        return 1;
        exit(1);
     }
}else{
     fprintf(stderr, "Invalid Argument: -t expected\n");
        return 1;
        exit(1);
}

FILE *fp;
char tabs[10];
char ch;
fp = stdin;
//counts current spaces passed in tab
int tabdistance = 0;
//marks if a tab is hit
int isTab = 0;
//reads file contents one char at a time
while ((ch = fgetc(fp)) != EOF){
     //resets tab counter if new line 
     if(ch == '\n'){
          printf("%c",ch); 
          tabdistance = 0;
          isTab = 0;
     }else{
          //checks if spaces are at the end of wanted tab size, if so resets tab counter
          if (tabdistance >= spaceAmount){
          tabdistance = 0;
          isTab = 0;
          }
          //checks for tab then marks isTab if true
     if ('\t' == ch){
               isTab = 1;
     }
     //if isTab is marked, adds a number of spaces
     if (isTab == 1){
          for(int i = tabdistance; i < spaceAmount; i++){
               printf("%c",' ');

          }
	  tabdistance = 0;
          isTab = 0;
          //if isTab is not marked, char will be printed as normal
     }else if(isTab == 0){
          printf("%c",ch); 
	  tabdistance++;
     }
     
     }
}
fclose(fp);
return 1;
}
