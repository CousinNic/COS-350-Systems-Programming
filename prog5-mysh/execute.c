/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

int execute(char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 *  errors: -1 on fork() or wait() errors
 */
{
  int	pid ;
  int	child_info = -1;
  int bgFlag = 0;

//checks for any bg processes, prints any found
  int npid = waitpid(-1, &child_info, WNOHANG);
  while (npid > 0){
  printf("process: [%d] ended\n",npid);
  npid = waitpid(-1, &child_info, WNOHANG);
  }

  if ( argv[0] == NULL )		/* nothing succeeds	*/
    return 0;

  //checks if exit is typed, then exits
  if(strcmp(argv[0], "exit") == 0){
    //if user does not specify a value, exits with 1 by deafult
    if(argv[1] == NULL){
      exit(0);
      //else exits with specified value
    }else{
      exit(atoi(argv[1]));
    }

//checks if cd command was entered
  }else if(strcmp(argv[0], "cd") == 0){
    //if no dest is specified, set destination to user home
    if(argv[1] == NULL){
      chdir(getenv("HOME"));
      //otherwise sets destination to chosen directory, while checking if invalid directory
    }else if (chdir(argv[1]) <= -1)
      perror("Invalid Directory ");
  }else{
    //checks for '&' at the end of command to indicate bg job
    for(int i = 0; argv[i] != NULL; i ++){
      if((strcmp(argv[i], "&") == 0) && (argv[i+1] == NULL)){
        argv[i] = NULL;
        //sets gb flag
        bgFlag = 1;
      }
    }

    if ( (pid = fork())  == -1 )
      perror("fork");
    else if ( pid == 0 ){
      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      execvp(argv[0], argv);
      perror("cannot execute command");
      exit(1);
    }
    else {
      //only waits if bg flag is false
      if(bgFlag != 1){
        if ( wait(&child_info) == -1 )
          perror("wait");
      }else{
        printf("running process: [%d] in background\n",pid);
      }
    }
  }
  return child_info;
}
