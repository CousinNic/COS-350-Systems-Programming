/* Authors: Nic Drummey
    *Date: 4-17-22
    *Class: cos-350 Systems Programing*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>	

int totalWords = 0;

/*
 * main method to count words if files listed in argv using forking and pipes
 */
int main(int argc, char **argv){
int filedes[2];
    if (pipe(filedes) == -1) {
            perror("pipe");
            exit(1);
        }
    //loops through each file
    for(int i = 1; i < argc; i++){
        
        int ex = 0;
        //forks process
        if(fork() == 0){
            //creates pipes
            while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) 
            close(filedes[1]);
            close(filedes[0]);
            //forked program runs mywc1 with current filename
            if(execlp("mywc1","./mywc1",argv[i],NULL) < 0)
            exit(1);

        }

        }
        close(filedes[1]);
        int count = 0;
        char *buf = malloc(500);
        char *temp;
        //loops through for argument count
        while (count < argc-1){
            //reads pipe of mywc1 for current file
    	if (read(filedes[0], buf, 500) > 0){
            //prints the pipe of mywc1
            printf("%s",buf);    
            count++;
            temp = strtok(buf," ");
            totalWords += atoi(buf);
            fflush(stdout);
        } 
    }
    //prints total words
    printf("total words: %d\n",totalWords); 
    free(buf);
    exit(1);

}
