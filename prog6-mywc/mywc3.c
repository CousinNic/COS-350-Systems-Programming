/* Authors: Nic Drummey
    *Date: 4-22-22
    *Class: cos-350 Systems Programing*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>	
#include <ctype.h>
#include <pthread.h>
#include <time.h>

void *thread(void *arg);

int totalWords = 0;
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * main method to count words if files listed in argv using threads
 */
int main(int argc, char **argv){
    pthread_t t[argc-1];

    //creates a thread for each file
    for(int i = 0; i < argc-1; i++){
        pthread_create(&t[i], NULL, thread, (void *)argv[i+1]);
        //printf("thread %d opened\n", i);
                 //fflush(stdout);
    }
    //joins all threads when finished
    for(int i = 0; i < argc-1; i++){
         pthread_join(t[i], NULL);
         //printf("thread %d closed\n", i);
         //fflush(stdout);
    }
    //prints total words
    if(argc > 2)
    printf("total words: %d\n",totalWords);
    exit(1);
}

/*
 * thread class to count all words in a file specified by arg
 */
void *thread(void *arg){
    
    int tempWords = 0;
    FILE  *fp;
    //opens file
        if ((fp = fopen(arg, "r")) == NULL){
            fprintf(stderr, "File: %s does not exist\n", (char*)arg);
        }else{
            char ch;
            char previousChar = ' ';
            //go through each char
            while ((ch = fgetc(fp)) != EOF){
                //if last char is a space, and current is not, then add to word count.
                if((isspace(previousChar)) && (!isspace(ch))){
                    tempWords++;
                }
                previousChar = ch;
            }
        
        }
        fclose(fp);
        //lock, add temp words to toal words, then unlock
        pthread_mutex_lock(&counter_lock);
        totalWords+=tempWords;
        pthread_mutex_unlock(&counter_lock);
        //print total words
        printf("%-9d %5s\n",tempWords,(char*)arg);
}