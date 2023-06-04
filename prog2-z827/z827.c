/* Authors: Nic Drummey, Mike Belley
    *Date: 2-26-22
    *Class: cos-350 Systems Programing*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
int fdSource = 0;

int compress(int in,int out,int bytes);
int decompress(int in,int out,int bytes, FILE *f);

int main (int argc, char** argv){
    //#################################################################
    //checks for argument errors
    //#################################################################
    if (argc !=  2){
        fprintf(stderr,"Arguments invalid... argument should be a single filename\n");
        exit(0);
    }
    char new_file_name[strlen(argv[1])];
    int fdSource = 0;
    char* srcPath = NULL;
    int fd, sz; 

    int in;
    //finds and saves file length
    in = open(argv[1], O_RDONLY);
    if (in < 0){
        fprintf(stderr,"File name not found\n");
        exit(0);
    }
    unsigned int bytes = lseek(in, 0, SEEK_END);
    close(in);
//#################################################################
//checks if file lacks .z827 to indicate its needs to be compressed
//#################################################################
    if(strstr(argv[1], ".z827") == NULL) {
        printf("Compressing...\n");
        //finds file length and opens/ creates files for compression
        in = open(argv[1], O_RDONLY);
        int out;
        creat(strcat(argv[1],".z827"), 0644);
        out = open(argv[1], O_WRONLY);
        //runs compression function
        int t = compress(in,out,bytes);
        //removes old file
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        remove(argv[1]);
//#################################################################
    }else{//else file is allready compressed
//#################################################################
        printf("Uncompressing...\n");
        in = open(argv[1], O_RDONLY);
        int out;
        //trim .z827 off file name
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        argv[1][strlen(argv[1])-1] = '\0';
        FILE *f;
        f = fopen(argv[1], "w");
        creat(argv[1], 0644);
        out = open(argv[1], O_WRONLY);
        //runs decompression function
        int t = decompress(in,out,bytes,f);
        //removes old file
        remove(strcat(argv[1],".z827"));

    }
    close(in);
    exit(1);
}
/* param: int in, int out, int bytes
* a function to take an in, out, and file size, and compress
* the file by removing the last bit from each byte
*/
int compress(int in,int out,int bytes){
    //creates needed variables
    unsigned char read_buffer[bytes];
    read(in, read_buffer, bytes);
    unsigned char temp = read_buffer[1]<<7;
    unsigned char out_char;

    //writes file size as header
    write(out, &bytes, 4);

    int i;
    int tempi = 0;
    //for loop to compress the file one char at a time
    for(i = 0; i < bytes; i++){
        temp = read_buffer[i+1]<<7-tempi;
        out_char = (read_buffer[i] >> tempi ) | temp;
	    tempi++;
        //resets temp counter every 8 runs
        if (tempi == 8){
            tempi = 0;
        }else{
            write(out, &out_char, 1);
        }
    }
    return 0;
}
/* param: int in, int out, int bytes, *f
* a function to take an in, out, bytes, and file, then decompress by
* re-adding the bit to each byte that was removed in compression. 
*/
int decompress(int in,int out,int bytes,FILE *f){
    //creates needed variables 
    unsigned char read_buffer[bytes];
    read(in, read_buffer, bytes);
    int origionalsize = read_buffer[0]+read_buffer[1]+read_buffer[2]+read_buffer[3];
    unsigned char out_char;
    unsigned char temp = read_buffer[5]>>7;
    int tempi = 0;
    
    //loop to decompress one char at a time
    for (int i = 4; i < bytes; i++){
        out_char = (read_buffer[i] << tempi) | temp ;
        out_char = out_char << 1;
        out_char = out_char >> 1;
        fprintf(f, "%c",out_char);
        temp = read_buffer[i]>>7-tempi;
        tempi++;
        //resets temp counter every 8 runs
        if(tempi == 8){
            tempi = 0;
            i--;
        }else{
            
        }

    }
    fclose(f);
}
    