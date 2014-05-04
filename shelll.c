#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
 
 
 int SToken(char *, char **);
 int execute(char **,int *);
 
 int main()
 {
   char *input,*command[256] ;
   int commandSize = 0,i;
   size_t length = 256;
   input = (char*)malloc(sizeof(char)*length);
   
  while(1)
  {
   printf("$ ");
   getline( &input, &length, stdin); 
   if(strcmp(input, "exit\n") == 0)
       exit(0);
   commandSize=SToken(input,command);
     for (i=0; i<commandSize; ++i)
       printf("argv[%d]: %s\n", i, command[i]);
   execute(command,&commandSize);  

  }
  return 0;
 }
  int SToken(char *input, char **command)
  {
    int size = 0;
    while (*input != '\0')
    {
      	*command = input ;
	size++ ;
      while(*input != ' ' && *input != '\t' && *input != '\0' && *input != '\n')
      {
	input++;
      }
      while(*input == ' ' || *input == '\t' || *input == '\n')
      {
	*input ='\0';
	input++;
      }
      	command++;
    }
    *command = '\0';
    return size;
  }
  int execute(char **command,int *size)
 {
        int *status1,pid;
    	pid = fork();
	if(pid == 0)
	
	       execvp(*command, command);  
	}
	else
    	{
                waitpid(pid, &status1, 0);
	}
     return 0;
  }
    