#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
void SToken(char *, char **);
int split(char *,char *,int *);
void fixed(char *);
int execute(char *);
FILE *eror_log;

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
    while(*line != ' ' &&*line != '\t' && *line != '\0' && *line != '\n')
    {
      line++;
    }
    while(*line == ' '|| *line == '\t' )
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
    while(*(line+*ptr) == ' '||*(line+*ptr)  == '\t' )
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
  while( *line!= ' ' && *line!= '\0' &&*line!= '\n'&&*line != '\t' )
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
  int fd1,fd2,mode=0,outputmode,pipemode=0,pipemode1=0,index=0,*ptr,myPipe[2][2];
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
      if( pipe(myPipe[0]) == -1 )
      {
	printf( "pipe : %s\n", strerror( errno ) );
	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"pipe :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
    }
    else if(mode == 33)
    {
       pipemode1=1;
     if(  pipe(myPipe[1]) == -1)
             {
          printf( "pipe : %s\n", strerror( errno ) );
     	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"pipe :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
    }
    pid = fork();
    if( pid < 0)
    {
          printf( "pid : %s\n", strerror( errno ) );
	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"pid :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
    }
    else if(pid == 0)
    {
           
      //input
	if(pipemode == 2)
	{
	 if( dup2(myPipe[0][0],STDIN_FILENO)==-1)
	   	         {
          printf( "dup2 : %s\n", strerror( errno ) );
	 	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	if(  close(myPipe[0][0]) == -1)
	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	}
	else if (pipemode1 == 2)
	{
	  if(dup2(myPipe[1][0],STDIN_FILENO)==-1)
	    	         {
          printf( "dup2 : %s\n", strerror( errno ) );
	  	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	 if( close(myPipe[1][0])==-1)
	   	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
	 	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	}
	else if (*input!='\0')
	{	  
	fixed(input);
	fd1 = open(input,O_RDONLY,0);
	if(fd1 == -1)
	  	         {
          printf( "open : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"open :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	if(dup2(fd1, STDIN_FILENO)==-1)
	  	         {
          printf( "dup2 : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	if(close(fd1)==-1)
	  	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	}
	
      //output
        if(pipemode == 1)
       {
	  if(dup2(myPipe[0][1],STDOUT_FILENO)==-1)
	    	         {
          printf( "dup2 : %s\n", strerror( errno ) );
	  	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	 if( close(myPipe[0][1])==-1)
	   	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
	 	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
       }
       else if(pipemode1 ==1){
	if( dup2(myPipe[1][1],STDOUT_FILENO)==-1)
	  	         {
          printf( "dup2 : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	 if( close(myPipe[1][1])==-1)
	   	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
	 	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
       }
       else if (*output!='\0')
       {
	  fixed(output);
	  switch(outputmode)
	  {
	  case 11:
	  fd2   = open(output,O_WRONLY | O_CREAT | O_TRUNC, 0600);
	  if(fd2==-1)
	  	         {
          printf( "open : %s\n", strerror( errno ) );
	  	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"open :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	if(  dup2(fd2, STDOUT_FILENO)==-1)
	  	         {
          printf( "dup2 : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	if(  close(fd2)==-1)
	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
		eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }	  
	  break;
	  case 55:
	  fd2   = open(output,O_WRONLY | O_APPEND, 0600);
	  if(fd2==-1)
	    	         {
          printf( "open : %s\n", strerror( errno ) );
	  	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"open :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	  if(dup2(fd2, STDOUT_FILENO)==-1)
	    	         {
          printf( "dup2 : %s\n", strerror( errno ) );
	  	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"dup2 :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	 if( close(fd2)==-1)
	 {
          printf( "close : %s\n", strerror( errno ) );
	  	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
	  break;
       }      
      }
     SToken(cur,command);
      if(execvp(*command, command)==-1)
		         {
          printf( "execvp %s : %s\n",*command, strerror( errno ) );
      	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"execvp :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
    }
    else
    {
    if(mode == 44)
    ;
    else
    if(waitpid(pid, NULL, 0)==-1)
    {
                printf( "waitpid : %s\n", strerror( errno ) );
			eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"waitpid :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
    }
    *output='\0';
    *input='\0';
    outputmode=0;
    mode=0;
    if(pipemode ==1)
    {
      pipemode = 2;
     if( close(myPipe[0][1])==-1)
       	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
     	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
    }
    else if(pipemode ==2)
    {
      pipemode=0;
    }
    if(pipemode1 ==1)
    {
      pipemode1 = 2;
    if( close(myPipe[1][1])==-1)
      	  	   	         {
          printf( "close : %s\n", strerror( errno ) );
    	eror_log = fopen ("log_error.txt", "a");
	fprintf(eror_log,"close :%s\n",strerror(errno));
	fclose(eror_log);
          exit( 1 );
      }
    }
    else if(pipemode1 ==2)
    {
      pipemode1=0;
    }
    mode = split(line,cur,ptr);
    }
  }
  return 0;
}
