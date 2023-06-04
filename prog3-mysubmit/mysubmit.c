/* Author: Nic Drummey
 *Date: 3-13-22
 *Class: cos-350 Systems Programing*/
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>

void print_file_contents(char *filePath);
void copy_files(char *source);

char destination[100]="./";

int main(){
//save current directory
    char curentPath[50];
    getcwd(curentPath,50);
    printf("What course are you submiting to? e.g. cos350\n");

//inpuut course directory
    char course[7];// = input();
    scanf("%6[0-9a-zA-Z ]",course);
    strcat(destination,course);

//verify input directory exists
    struct stat buffer;
    if(stat(destination,&buffer)){
        printf("Course not found\n");
        exit(0);
    }
//######################################################################
//verify input contains submit directory
    strcat(destination,"/submit");
    if(stat(destination,&buffer)){
        printf("Submit file not found\n");
        exit(0);
    }
//######################################################################
//get users name from system
    char *userName = getlogin();
//if users name does not have foler, create one, else append to submitDirectory
    strcat(destination,"/");
    strcat(destination,userName);
    if(stat(destination,&buffer)){
        printf("%s Directory not found,\n%s folder created\n",destination,userName);
        mkdir(destination,0777);
    }
//######################################################################
//ask for project e.g. proj1, if none exists create one
    printf("Please enter the assignment name. e.g. proj1\n");
    char assignmentName[10];// = input();
    scanf(" %9[0-9a-zA-Z ]",assignmentName);
    strcat(destination,"/");
    strcat(destination,assignmentName);
    if(stat(destination,&buffer)){
        printf("%s directory not found, \n%s folder created\n",destination,assignmentName);
        mkdir(destination,0777);
    }
//######################################################################
//display all files in current dir
    printf("\n");
    print_file_contents(".");
//ask for assignment names to submit 
    printf("Choose files to submit, seperate by space (use * for all). e.g. Assignment1 Assignment2\n");
    char filesToCopy[100];
    scanf("  %99[^\n]",filesToCopy);

    printf("Submiting...\n\n");

    //to coppy all files if *
    DIR *i;
    struct dirent *dir;
    i = opendir(".");
    char *tempFile;
    if(strcmp(filesToCopy,"*\n") < 0){
        while ((dir = readdir(i)) != NULL) {
            tempFile = ("%s", dir->d_name);
            if(strcmp(tempFile,".")){
                if(strcmp(tempFile,"..")){
                    //printf("%s ",tempFile);
                    copy_files(tempFile);
                }
            }
        }
    }else{//else coppy files requested
    char * token = strtok(filesToCopy, " ");
//######################################################################
//copy each file requested to the new dir
    while( token != NULL ) {
        copy_files(token);
        token = strtok(NULL, " ");
    }
    }
    printf("\n");
//######################################################################
//display turned in files in submission directory
    printf("Files Submited:\n");
    print_file_contents(destination);
    exit(1);
}

/*
 *param: char *filename
 *a function to copy a file to a chosen directory
 */
void copy_files(char *fileName){ 
    int in_fd, out_fd, n, err;
    unsigned char buffer[4096];
    char path[100];
	  strcpy(path, destination);
	  strcat(path,"/");
	  strcat(path, fileName);

  //opens files
    if((in_fd = open(fileName, O_RDONLY)) == -1){
        printf("error opening %s file\n",fileName);
        exit(1);
    }
    if((out_fd = open(path, O_CREAT | O_WRONLY)) == -1){
        printf("error opening %s file\n",path);
        exit(1);
    }
    chmod (path, 0777);
    //writes files
    while (1) {
        read(in_fd, buffer, 4096);
        if (n == 0) break;
        write(out_fd, buffer, n);
    }
    //closes files
    close(in_fd);
    close(out_fd);
}

/*
 *param: char *filePath
 *a function to display all content in a directory
 */
void print_file_contents(char *filePath){
    printf("SIZE  DATE   TIME  NAME\n");
    DIR *i;
    struct dirent *dir;
    i = opendir(filePath);
    char *tempFile;
    struct stat info;
    if (i) {
    //prints info on each file, excluding . and .. hidden files
        while ((dir = readdir(i)) != NULL) {
            tempFile = ("%s", dir->d_name);
            stat(tempFile, &info);
            if(strcmp(tempFile,".")){
                if(strcmp(tempFile,"..")){
                    printf("%.5lu %.6s %.5s %s\n",info.st_size, ctime(&info.st_mtime) + 4,ctime(&info.st_mtime) + 11,tempFile);
                }
            }
        }
    }
    closedir(i);
}
