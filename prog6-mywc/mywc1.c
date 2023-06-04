/* Authors: Nic Drummey
    *Date: 4-16-22
    *Class: cos-350 Systems Programing*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int totalWords = 0;

/*
 * main method to count words if files listed in argv
 */
int main(int argc, char **argv){
    //loop through each file specified in arguments
    for(int i = 1; i < argc; i++){
        int tempWords = 0;
        FILE  *fp;
        //open files
        if ((fp = fopen(argv[i], "r")) == NULL){
            fprintf(stderr, "File: %s does not exist\n", argv[i]);
        }else{
            char ch;
            char previousChar = ' ';
            //check each char
            while ((ch = fgetc(fp)) != EOF){
                //check if previous char is a space, while current char is not, then add 1 to word count
                if((isspace(previousChar)) && (!isspace(ch))){
                    totalWords++;
                    tempWords++;
                }
                previousChar = ch;
            }
            //print out word count of current file
            printf("%-9d %5s\n",tempWords,argv[i]);
            fflush(stdout);
            fclose(fp);
        }
        
    }
    //print total words id file count is greater than 1
    if(argc > 2)
    printf("total words: %d\n",totalWords);
    exit(1);
}
