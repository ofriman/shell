#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_STRING_SIZE 80
#define NORMAL 	00
#define OUTPUT_REDIRECTION 11
#define INPUT_REDIRECTION 22
#define PIPELINE 33
#define BACKGROUND 44
#define OUTPUT_APP 55


int SToken(char *, char **);
int split(char *,char *, char *,char *temp);
int execute(char *);


int main()
{
  char *input;
  size_t length = 256;
  input = (char*)malloc(sizeof(char)*length);
  while(1)
  {
    printf("$ ");
    getline( &input, &length, stdin); 
    if(strcmp(input, "exit\n") == 0)
      exit(0);
    execute(input);  
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
int split(char *input,char *begin, char *end,char *temp)
  {
  int flag =1;
  int ans = 0;
  while(*input != '|' && *input != '>' && *input != '<' && *input != '&' && *input!='\0')
  {
    *begin=*input;
    begin++;
    input++;
  }
  switch(*input)
  {
    case '&':
      ans = 44;
      break;
    case '>':
      ans = 11;
      input++;
      if(*input == '>')
      {
	ans = 55;
	input++;
      }
      break;
    case '<':
      ans = 22;
      break;
    case '|':
      ans = 33;
      break;
  }
  input++;
  if(ans!=0){
    if(ans == 11) input--;

    if(ans==11||ans==22||ans==33||ans==55)
    {
      while (*input == ' '||*input == '\t')
      {
	input++;
      }
    }
    while(*input != '\t' && *input != '\0' && *input != '\n')
    {
      *temp = *input ;
      temp++;

      if(*input != '|' && *input != '>' && *input != '<' && *input != '&' && *input!='\0'&&flag)
      {
	*end = *input ;
	end++;
      }
      else
      flag=0;
      input++;
    }
  }
  *end='\0';
  *temp='\0';
  *begin='\0';
  return ans;
}

int execute(char *input)
  {
  FILE *fp;
  char begin[256], end[256], temp[256],*command[256],*command1[256]; 
  int *status1,pid,commandSize=0,i;
  int mode=0;
  while(strcmp(input,end))
  {
    mode = split(input,begin,end,temp);
    commandSize=SToken(begin,command);
    strcpy(input,temp);
    pid = fork();
    if( pid < 0)
    {
      printf("Error occured");
      exit(-1);
    }
    else if(pid == 0)
    {
      switch(mode)
      {
	case 11:
	  fp = fopen(end, "w+");
	  dup2(fileno(fp), 1);
	  break;
	case 55:
	  fp = fopen(end, "a");
	  dup2(fileno(fp), 1);
	  break;
	case 22:
	  fp = fopen(end, "r");
	  dup2(fileno(fp), 0);
	  break;
      }
      execvp(*command, command);  
    }
    else
    {
      if(mode == 44)
      ;
      else
      {
      if(mode ==0)
	waitpid(pid, &status1, 0);
      else
	waitpid(1, &status1, 0);
      }
    }
  }

  return 0;
}
