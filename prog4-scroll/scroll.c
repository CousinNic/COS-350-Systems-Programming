/* Authors: Nic Drummey
 *Date: 4-3-22
 *Class: cos-350 Systems Programing*/

#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <string.h>
#include <signal.h>

//assigns all text to an array
void assignTextArray(char *name);
//handles user key imput
void input();
//prints one full page when called
void printPage();
//prints one line, with prompt
void scroll();
//removes last line(for prompt)
void removeLastMessage();
//sets scroll speed
void setScrollSpeed(float speedChange);
//sets terminal settings to normal, and frees memory then exits program
void close();

//terminal behaviors
struct termios info;
//to store text file
char **formatedText;
//self explanitory
int screenW;
int screenH;
//line printed
int curentline = 0;
//prompt size saved for removal
int lastMessageSize = 0;
//for scroll
struct sigaction sc;
struct itimerval timer;
//total amout of lines to be printed, (compared to curentline to determine end of text)
int totalLines = 0;
float scrollSpeed = 0;

/* 
 * a function that takes a text file as a param and allows 
 * scrolling or panning through said file
 */
int main(int argc, char **argv){

    if(argc != 2){
        printf("invalid arguments, must be in format: scroll filename\n");
        exit(0);
    }

    sc.sa_handler = scroll;
    sigemptyset(&sc.sa_mask);
    sigaction(SIGALRM, &sc, NULL);

    //sets timer to 0
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    //get and save window size
    struct winsize winsizestruct;
    ioctl(1, TIOCGWINSZ, &winsizestruct);
    screenW = winsizestruct.ws_col;
    screenH = winsizestruct.ws_row;

    FILE *fp;
    fp = stdin;

    //set terminal behaviors
    tcgetattr(0,&info);
    info.c_lflag &= ~ECHO;	
	info.c_lflag &= ~ICANON;
    tcsetattr(0,TCSANOW,&info);
    //block Ctrl-c
    signal(SIGINT, SIG_IGN);

    //set txt file contents to **formatedText
    assignTextArray(argv[1]);
    
    if ( (fp = fopen( *++argv , "r" )) != NULL ) {
        input();
      	fclose(fp);
    }else{
        printf("Error: '%s' file not found.\n", *argv);
        fclose(fp);
    }
    //runs close function
    close();
}

/* 
 * The Function to process user input from keyboard
 */
void input(){
    FILE  *fp_tty;
    char c;
    //prints first page
    printPage();
    
    fp_tty = fopen("/dev/tty", "r" );
    if ( fp_tty == NULL ){
        printf("error reading tty");
        close();
    }else{
        //pressing q breaks loop and consiquently ends program
        while( (c = getc(fp_tty)) != 'q' ){ // Reads input
            //if space
            if(c == ' ' && scrollSpeed == 0){
                removeLastMessage();
                printPage();
                //if enter
            }else if(c == '\n'){
                if(scrollSpeed == 0){
                    //print scroll on message
                    removeLastMessage();
                    printf("\033[7m Scroll on \033[0m");
                    setScrollSpeed(1);
                    
                    lastMessageSize = 61;
                }else{
                    removeLastMessage();
                    setScrollSpeed(0);
                    printf("\033[7m Press space to page down, enter to toggle scroll, q to quit \033[0m");
                    lastMessageSize = 61;
                }
                //if f
            }else if(c == 'f'){
                setScrollSpeed(1.6);
                //if s
            }else if(c == 's'){
                setScrollSpeed(2.4);
            }
            
        }
            
    }
}
/*
 *changes scroll speed dependent on speedChange.
 *speedChange must be 0 to disable, 1 to enable, 2.2 to speed up, or 1.8 to slow down
 */
void setScrollSpeed(float speedChange){
    //adjusts speed based on speedChange
    if(speedChange == 1){
        scrollSpeed = 2;
    }else if(speedChange == 0){
        scrollSpeed = 0;
    }else if(speedChange == (float)1.6){
        //ensures scroll speed doesnt hit 0 and stop
        if(scrollSpeed > .5){
            scrollSpeed -= .4;
        }
    }else if(speedChange == (float)2.4){
        if(speedChange < 3.6){
            scrollSpeed += .4;
        }
    }else{}
    //updates speed
    timer.it_value.tv_sec = scrollSpeed;
    timer.it_value.tv_usec = ((long)(scrollSpeed * 1000000)) % 1000000;
    timer.it_interval.tv_sec = scrollSpeed;
    timer.it_interval.tv_usec = ((long)(scrollSpeed * 1000000)) % 1000000;
    setitimer(ITIMER_REAL, &timer, NULL);
}

/* 
 * A function to print a new line, then a prompt message
 */
void scroll(){
    //removes current prompt
    removeLastMessage();
    //prints line
    printf("%s\n", formatedText[curentline]);
        curentline++;
        //adds new prompt
        printf("\033[7m Scrolling... scroll delay at %2.2f \033[0m", (float)scrollSpeed);
        //records prompt size
        lastMessageSize = 36;
        //last line check for close
        if(totalLines <= curentline)
        close();
}

/* 
 * Removes current line(used to delete prompt)
 * lastMessageSize must be updated after displaying each prompt
 */
void removeLastMessage(){
    for(int i = 0; i < lastMessageSize; i++){
        printf("\b");
    }
    fflush(stdout);
    printf("\033[0K");
}

/* 
 * function to print screenH - 1 lines, also prints prompt and updates 
 * lastMessageSize after
 */
void printPage(){
    for(int i = 0; i < screenH-1; i++){
        printf("%s\n", formatedText[curentline]);
        curentline++;
    }
    //prints prompt then updates lastMessageSize
    printf("\033[7m Press space to page down, press enter to begin scrolling \033[0m");
    lastMessageSize = 58;
    if(totalLines <= curentline)
        close();
}

/* 
 * function to save text file of char *name to char **formatedText;
 * saves each line with a max of screenW characters.
 * also sets characters to bold or underlines if needed
 */
void assignTextArray(char *name){
    //frees up resonable amount of lines
    formatedText = malloc(sizeof(char*) * 250);
    for(int i = 0; i < 250; i++){
        //frees space, while compensating for an all 
        //special chgar senario
        formatedText[i] = malloc(screenW *8);
    }
    //checks if file exists
    //###################################################################
    FILE *fp;
    if ((fp = fopen(name, "r")) == NULL){
        fprintf(stderr, "File: %s does not exist\n", name);
        close();
    }//##################################################################

    char ch;
    int totalchar = 0;
    int i = 0;
    int trackTabs = 8;
    //runs through each char in text file
    while ((ch = fgetc(fp)) != EOF){
        if(trackTabs <= 0)
            trackTabs = 8;

        trackTabs--;
        
        if(ch == '\t')
            totalchar = totalchar + trackTabs-1;
            trackTabs = 8;
        if(ch == '\b'){
            //checks and adjusts for underlined char
            if(formatedText[totalLines][i-1] == '_'){
                formatedText[totalLines][i] = ch;
                i++;
                strcat(formatedText[totalLines], "\033[4m");
                i = i+4;
                formatedText[totalLines][i] = fgetc(fp);
                i++;
                strcat(formatedText[totalLines], "\033[0m");
                i = i+4;
                totalchar++;
                //checks and adjusts for BOLD char
            }else{
                formatedText[totalLines][i] = ch;
                i++;
                strcat(formatedText[totalLines], "\033[1m");
                i = i+4;
                formatedText[totalLines][i] = fgetc(fp);
                i++;
                strcat(formatedText[totalLines], "\033[0m");
                i = i+4;
                totalchar++;
            }
        //checks \n
        }else if(ch != '\n'){
        formatedText[totalLines][i] = ch;
        i++;
        totalchar++;
        }
        //starts new line if \n or screen width was reached
        if(totalchar >= screenW || ch == '\n'){
            i = 0;
            totalchar = 0;
            totalLines++;
            trackTabs = 8;
        }
    }     
    
}
/* 
 * function safely close the program, resetting terminal behaviors 
 * to normal, and freeing up space before exiting
 */
void close(){
    //frees up space
    //############################################################
    for(int i = 0; i < 250; i++){
        free(formatedText[i]);
    }
    removeLastMessage();
    free(formatedText);
    //sets behaviors back
    //############################################################
    tcgetattr(0,&info);
    info.c_lflag |= ECHO;
    info.c_lflag |= ICANON;
    tcsetattr(0,TCSANOW,&info);
    exit(1);
}