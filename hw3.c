/*
  Javier Mayol 
  netid : cmayol2
  CS361 Computer Systems Fall 2018
  
  An implementataion of a small shell program
  that prints a prompt and asks the user for 
  a command. Supports redirection of I/O unix.

  Performs as follows:

  1. Allocation of memory for needed char * and char**
	arrays.
  2. Infinite loop prints prompt CS361> and waits
     for user to type a command.
  3. 
  parse_str()
 	Parses the string inputed by the user.
	Takes a char *buffer and a char **array.
  4.
  flags : if user inputs any of the operators
          for redirection ";", " >", " <", the flag
          stores values 1, 2, or 3 correspondingly.
	  0 if no redirection is requested.	
  5.
  exeq()
	takes argument char **argsarray
      	calls fork, after a child is reaped
  	executes execv(argsarray[0], argsarray)
  6.	
  r_parsing()
	Search for tokens ";", ">", "<" and change elements to null.
  7.
  split()
	Takes the array populated with the input by user
	as aparameter. Returns array with the second command
	for redirection.
  8.
  r_exeq()
	parameters are input array and splitted array. Executes
	redirection command ; command.
  9
  r_file()
	parameters are input array and splitted array. Executes
	redirection command > command or command < command.
  10.
  shell()
	Driver. initiallizes arrays, declares an endless loop and frees
	memmory allocated. 
   The only builtin command for this shell is "exit" which exits the program. 
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define CMD_BUFFER 500  	//Command buffer maximum 500.
#define ARG_CNT_B 20  		//Argument count buffer max 20.
#define ARG_BUFFER 100		//Arguments buffer max 100.


void sigint_handler(int s){
  fprintf(stderr,"caught sigint.\n");
}

void sigstp_handler(int s){
  fprintf(stderr,"caught sigstp.\n");
}

void sigsegv_handler(int s){
  fprintf(stderr,"caught sigsegv.\n");
  exit(1); 
}

void parse_str(char *line, char **argsarray, char *token, int *index) //[ARG_CNT_B][ARG_BUFFER])
{
  char *word = strtok(line, " \n\t"); //break the string up into words
  int i = 0;

  while (word) {
    argsarray[i] = strdup(word); //initialize argsarray[i].
    strcpy(argsarray[i], word); //copy a word to the argsarray[i]
    word = strtok(NULL, token); //get next word
    i = i + 1; //increase index.
    if( i >= ARG_CNT_B - 1) //If array line has more characters than arguments count buffer, send warning and finish copying.
    {
      printf("ARG_CNT_B reached capacity: last argument parsed : '%s'", argsarray[i - 1]);
      argsarray[i] = NULL; //Always last index terminates in NULL.
      break; //Breaks loop to resume program.
    } 
 }
  argsarray[i] = NULL; //Last index ends in NULL. Needes for second argument in execv function in exeq, r_exeq, r_file methods.
  *index = i;
}

int r_parsing(char **argsarray, int i, int *flag)//redirection parsing.
{
  while(argsarray[i] != NULL)
  {
    if(strpbrk(argsarray[i], ";") != NULL)
    {
      *flag = 1;
      break;
    }
    if(strpbrk(argsarray[i], ">") != NULL)
    {
      *flag = 2;
      break;
    }
    if(strpbrk(argsarray[i], "<") != NULL)
    {
      *flag = 3;
      break;
    }
    i++;
  }
  argsarray[i] = NULL;
  return i;
}

char **split(char **argsarray, int i) //Returns array with copied elements portion specified by int i				     
{ 				      //from parameter array char **argsarray.
  int j, d = 0;
  j = i + 1;
  char **arg_tmp = (char **)malloc(sizeof(char *)*ARG_BUFFER);
  while(argsarray[j] != NULL)
  {
    arg_tmp[d] = strdup(argsarray[j]);
    j++;
    d++;
  }
  arg_tmp[d] = NULL;
  return arg_tmp;
}

void builtin_cmd( char *ch)
{
  char ch_t = *ch;
  if(ch_t == *"exit")
  {  
    printf("Adios!\n");
    exit(0);
  }
}

void r_exeq( char **argsarray, char **r_argsarray) //char **argsarray) //[ARG_CNT_B][ARG_BUFFER])
{
  int fd, i, status, r = 0;
  pid_t pid1, pid2;
  char arg;
  i = 0;
  pid1 = fork();
  if(pid1 < 0)
  { 
    fprintf(stderr,"r_exeq() : Child fork failed.\n");
    exit(1);
  }
  if(pid1 == 0)
  {
    execv(argsarray[i], argsarray);
  }
  else
  {
    wait(&status);
    pid2 = fork();
    if(pid2 < 0)
    { 
      fprintf(stderr,"r_exeq() : Child fork failed.\n");
      exit(1);
    }
    if(pid2 == 0)
    {
      execv(r_argsarray[0], r_argsarray);
    }
    else
      wait(&status);
  }
  kill(pid1, SIGKILL);
  kill(pid2, SIGKILL);
  printf("pid:%d status:%d\n", pid1, WEXITSTATUS(status));
  printf("pid:%d status:%d\n", pid2, WEXITSTATUS(status));
}

void print(char **argsarray)
{
  int i = 0;
  while(argsarray[i] != NULL)
  {
    printf("r_exe arg[%d] %s\n", i, argsarray[i]);
    i++;
  }
}

void exeq( char **argsarray) //char **argsarray) //[ARG_CNT_B][ARG_BUFFER])
{
  int fd, i, status, r = 0;
  int p[100];
  int s[100];
  pid_t pid;
  char arg;
 
  pid = fork();
  if(pid < 0)
  { 
    fprintf(stderr,"exeq() : Child fork failed.\n");
    exit(1);
  }
  if(pid == 0)
  {
    execv(argsarray[0], argsarray);
  }
  else
    wait(&status);
  if(status > 0)
  { 
    fprintf(stderr,"exeq() : child fork fail status %d.\n", status);
  }
  kill(pid, SIGKILL);

  printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
}

void r_file(char **argsarray, char **r_argsarray, int flag)
{
  int fd1, fd2, status;
  pid_t pid;
 
  pid = fork();

  if(pid < 0)
  { 
    fprintf(stderr,"r_file() : child fork failed.\n");
    exit(1);
  }

  if(pid == 0)
  {
    if(flag == 2) // If command output > 
    {
      fd1 = open(*r_argsarray,O_RDWR | O_TRUNC | O_CREAT, 0766);//Permission is set for user RWX and RW for others. 
      dup2(fd1, STDOUT_FILENO);
      close(fd1);
    }
    if(flag == 3)//1 comman bin.TRY implementing a multiple argument binary command.
    {
      char *file = (char *) malloc(sizeof(char) * ARG_BUFFER);
      int s;
      fd2 = open(*r_argsarray, O_RDONLY, 0); //1st argument was "filename" for gradescope.
      if(file != '\0')
        s = read(fd2,file,ARG_BUFFER);//TRY appending to file to leave argsarray[0];
      file[s + 1] = '\0';
      printf("%s\n", file);
      argsarray[1] = file;//Try not hard copying it.
      argsarray[2] = NULL;
      free(file);
      dup2(fd2, STDIN_FILENO);
      close(fd2);
    }
    execv(argsarray[0],argsarray);
    fprintf(stderr, "Execution failed : redirection output %s.\n", argsarray[0]);
    exit(1);
  }
  else
    wait(&status);
  kill(pid, SIGKILL);
  printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
}

void _shell()
{
  char *char_buff = (char *) malloc(sizeof(char) * CMD_BUFFER);
  char **argsarray = (char **)malloc(sizeof(char *) * ARG_CNT_B);
  int i = 0, len = 0, r_pnt = 0, flag = 0;
  for(i = 0; i < ARG_CNT_B; i++) 
    argsarray [i] = (char *)malloc(sizeof(char) * ARG_BUFFER);
  i = 0;
  while(1)
  {
    printf("CS361 >");
    fgets(char_buff, CMD_BUFFER, stdin);

    parse_str(char_buff, argsarray, " \n\t", &len);
    builtin_cmd(argsarray[0]);     
    r_pnt = r_parsing(argsarray, i, &flag);
    if(flag == 1)
    {
      char **r_argsarray = argsarray;
      r_argsarray = split(argsarray, r_pnt); 
      r_exeq(argsarray, r_argsarray);
      flag = 0;
    }
    else if(flag >= 2)
    {
      char **r_argsarray = argsarray;
      r_argsarray = split(argsarray, r_pnt); 
      r_file(argsarray, r_argsarray, flag);
      flag = 0;
    }
    else
      exeq(argsarray);
  } 

  for(i = 0; i < ARG_CNT_B; i++)
     free (argsarray [i]); 
  free(argsarray);
}
int main(int argc, char *argv[]) 
{
/*
  struct sigaction handler, old_handler;
  handler.sa_handler = sigint_handler;
  sigemptyset(&handler.sa_mask);
  handler.sa_flags = 0;
  sigaction(SIGINT, NULL, &old_handler);
  if(old_handler.sa_handler == SIG_ERR);
    sigaction(SIGINT, &handler, NULL);   
*/
//  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigstp_handler);
  signal(SIGSEGV, sigsegv_handler);
  _shell();
  return 0;
}
