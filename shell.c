#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

void SToken(char *, char **);
int split(char *,char *,int *);
void fixed(char *);
int execute(char *);

int main()
{
  char *line;
  size_t length = 256;
  line = (char*)malloc(sizeof(char)*length);
  while(1)
  {
    printf("$ ");
    getline( &line, &length, stdin); 
    if(strcmp(line, "exit\n") == 0)
      exit(0);
    execute(line);  
  }
  return 0;
}
void SToken(char *line, char **command)
{
  while (*line != '\0' && *line != '\n')
  {
    *command = line ;
    while(*line != ' ' && *line != '\0' && *line != '\n')
    {
      line++;
    }
    while(*line == ' ')
    {
      *line ='\0';
      line++;
    }
    command++;
  }
  *command = '\0';
}
int split(char *line,char *cmd,int *ptr)
{
  int mode = 0;
  if (*(line+*ptr)!= '\0' &&*(line+*ptr)!= '\n')
  {
    while(*(line+*ptr) == ' ')
      *ptr=*ptr+1;
    while(*(line+*ptr) != '|' && *(line+*ptr) != '>' && *(line+*ptr) != '<' && *(line+*ptr) != '&' && *(line+*ptr) != '\0' && *(line+*ptr)!= '\n' )
    {
      *cmd=*(line+*ptr);
      cmd++;
      *ptr=*ptr+1;
    }
    switch(*(line+*ptr))
    {
      case '&':
	mode = 44;
	*ptr=*ptr+1;
	break;
      case '>':
	mode = 11;
	*ptr=*ptr+1;
	if(*(line+*ptr) == '>')
	{
	  mode = 55;
	  *ptr=*ptr+1;
	}
	break;
      case '<':
	mode = 22;
	*ptr=*ptr+1;
	break;
      case '|':
	mode = 33;
	*ptr=*ptr+1;
	break;
    }
  }
  *cmd='\0';
  return mode;
}
void fixed(char *line)
{
  while( *line!= ' ' && *line!= '\0' &&*line!= '\n')
  {
   line++;
  }
   while(*line!='\0')
  {
    *line='\0';
    line++;
  }
}

int execute(char *line)
{
  char *cur,*input,*output,*command[256];
  int fd1,fd2,mode=0,outputmode,pipemode=0,pipemode1=0,index=0,*ptr,myPipe[2],myPipe1[2];
  pid_t pid;
  ptr = &index;
  cur = (char*)malloc(sizeof(char)*256);
  input = (char*)malloc(sizeof(char)*256);
  output = (char*)malloc(sizeof(char)*256);
  mode = split(line,cur,ptr);
  while (*cur!='\0'){
    
    while(mode==22||mode==11||mode==55){ 
      if(mode==22){
	  mode = split(line,input,ptr);
      }
      else{
	  outputmode = mode ;
	  mode = split(line,output,ptr);
      }
    }
    if(mode == 33 && pipemode==0 )
    {
      pipemode = 1;
       pipe(myPipe);	
    }
    else if(mode == 33)
    {
       pipemode1=1;
       pipe(myPipe1);	
    }
    pid = fork();
    if( pid < 0)
    {
    printf("Error occured");
    exit(-1);
    }
    else if(pid == 0)
    {
      // input output close pipes
     	if(pipemode == 2)
	{
	  close(myPipe[1]); 
	}
	if(pipemode1 == 2)
	{
	  close(myPipe1[1]); 
	}      
      	if(pipemode == 1)
	{
	 close(myPipe[0]); 
	}
	if(pipemode1 == 1)
	{
	   close(myPipe1[0]); 
	}      
      
      
      //input
	if(pipemode == 2)
	{
	  dup2(myPipe[0],STDIN_FILENO);
	  close(myPipe[0]); 
	}
	else if (pipemode1 == 2)
	{
	  dup2(myPipe1[0],STDIN_FILENO);
	  close(myPipe1[0]); 
	}
	else if (*input!='\0')
	{	  
	fixed(input);
	fd1 = open(input,O_RDONLY,0);
	dup2(fd1, STDIN_FILENO);
	close(fd1);	  
	}
	
      //output
        if(pipemode == 1)
       {
	  dup2(myPipe[1],STDOUT_FILENO);
	  close(myPipe[1]); 
       }
       else if(pipemode1 ==1){
	  dup2(myPipe1[1],STDOUT_FILENO);
	  close(myPipe1[1]); 	 
       }
       else if (*output!='\0')
       {
	  fixed(output);
	  switch(outputmode)
	  {
	  case 11:
	  fd2   = open(output,O_WRONLY | O_CREAT | O_TRUNC, 0600);
	  dup2(fd2, STDOUT_FILENO);
	  close(fd2);	  
	  break;
	  case 55:
	  fd2   = open(output,O_WRONLY | O_APPEND, 0600);
	  dup2(fd2, STDOUT_FILENO);
	  close(fd2);	
	  break;
       }
       
      }

     
     SToken(cur,command);
      execvp(*command, command); 
    }
    else
    {
    if(mode == 44)
    ;
    else
    waitpid(pid, NULL, 0); 
    *output='\0';
    *input='\0';
    outputmode=0;
    mode=0;

    if(pipemode ==1)
    {
      pipemode = 2;
      close(myPipe[1]);
    }
    else if(pipemode ==2)
    {
      pipemode=0;
      close(myPipe[0]);
    }
    if(pipemode1 ==1)
    {
      pipemode1 = 2;
     close(myPipe1[1]); 
    }
    else if(pipemode1 ==2)
    {
      pipemode1=0;
     close(myPipe1[0]); 
    }

    mode = split(line,cur,ptr);
    }
  }
  return 0;
}
