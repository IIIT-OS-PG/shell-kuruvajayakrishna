#include<stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#define size 256
#define BUFFER 1024
#define DELIM " \t\r\n\a"

int status;
pid_t process_id;
char commands[size];
char command[size];
char *chd[]={"-","~"};
int i,tempin,tempout;
char *built[]={"cd","exit"};
char *redirec[]={">",">>"};
int count=0,count_pipes=0,count1=0;
char *history[BUFFER];

char **do_parsing(char *command)
{
  //printf("%s command=",command);
  int b_size = BUFFER;
  int count =0,count1=0; 
  char **args = malloc(b_size * sizeof(char*));
  if (!args)
  {
    printf("allocation error\n");
    exit(1);
  }

  char *argument;
 
  argument= strtok(command,DELIM);
  while (argument!= NULL) 
   {
    args[count1] = argument;
    if(strcmp(argument,"|")==0)
    {
      count_pipes++; 
    }
    count++;
    count1++;
    argument = strtok(NULL, DELIM);
  }
  args[count1] = NULL;

  return args;
}

void redirection(char **temp_arr,char **args,int c,int flag)
{
   int file_dis;
   if((process_id=fork())<0)
   printf("fork error");
   else if (process_id == 0) 
    {
    if(flag==1)
      file_dis=open(args[c+1],O_WRONLY|O_TRUNC|O_CREAT,0644);
    else
      file_dis=open(args[c+1],O_APPEND|O_WRONLY|O_CREAT,0644);
    if(file_dis<0)
    printf("error");
    dup2(file_dis,STDOUT_FILENO);
     close(file_dis);
    execvp(temp_arr[0],temp_arr);
    count=0;
    exit(1);
    }
   if ((process_id = waitpid(process_id, &status, 0)) < 0)
    printf("waitpid error");

}

void execute(char **command,int length)
{
if(strcmp(command[0],built[0])==0)
{
  if(chdir(command[1])!=0)
  printf("error");
}

else if(strcmp(command[0],built[1])==0)
  exit(0);

else
{
int j,flag;
  char ** temp=malloc(BUFFER*sizeof(char *));
  for(i=0;i<count;i++)
   {
    if(strcmp(command[i],redirec[0])==0)
    {
     flag=1;
     break;
    }
    else if(strcmp(command[i],redirec[1])==0)
    {
    flag=2;
    break;
    }
   }
   if(i!=count)
   {
     temp=command;
     temp[i]=0;
     redirection(temp,command,i,flag);
   }
  else
   {
  count=0;
  if((process_id=fork())<0)
   printf("fork error");
   else if (process_id == 0) 
    {
    execvp(command[0],command);
    printf("error in execution:");
    
    exit(1);
    }
   if ((process_id = waitpid(process_id, &status, 0)) < 0)
    printf("waitpid error");
   } 
}

}

char **do_pipes_parsing(char *command,int length)
{
  int b_size = BUFFER;
  int count1=0; 
  char **arguments = malloc(b_size * sizeof(char*));
  if (!arguments)
  {
    printf("allocation error\n");
    exit(EXIT_FAILURE);
  }

  char *argument;
  //  printf("%s",command);
  argument= strtok(command,"|");
  while (argument!= NULL) 
   {
   // printf("%s",argument);
    arguments[count1] = argument;
    count1++;
    argument = strtok(NULL, "|");
   }
  arguments[count1] = NULL;
  return arguments;
}

void execute_pipes(char **pipes,int count_pipes)
{
char **temp_arr[5];
for(int i=0;i<=count_pipes;i++)
{
temp_arr[i]=do_parsing(pipes[i]);
}
pid_t process_id;
int filedis[2],i=0,filedis1=0;
while(count_pipes >=0)
{
  pipe(filedis);
  if((process_id=fork())<0)
    printf("fork error");
   else if(process_id==0)
     {
        dup2(filedis1,0);
       if(count_pipes!=0)
        dup2(filedis[1],1);
      close(filedis[1]);
      execvp(temp_arr[i][0],temp_arr[i]);
      exit(1);
     }
   else
    {  
      if ((process_id = waitpid(process_id, &status, 0)) < 0)
      {
      printf("waitpid error");
      } 
    filedis1=filedis[0];
    close(filedis[1]);
   
    }
 count_pipes--;
 i++;
 }  
   
}
void reads()
{
int value=0;
while(fgets(command,size,stdin)!= NULL)
{
  count=0;
  count1=0;
  if (command[strlen(command) - 1] == '\n')
  {
  command[strlen(command) - 1] = 0;
  history[value++]=command;
  } 
  strcpy(commands,command);
  char **command1=do_parsing(command);
  if(count_pipes!=0)
  {
  char **pipes=do_pipes_parsing(commands,count_pipes);
  //printf("%s \n pipes=",pipes[0]);
  execute_pipes(pipes,count_pipes);
  }
  else
  {
  if(command1[0]!=0)
  execute(command1,strlen(command));
  }
  count_pipes=0;
  printf("%%");
}
}


int main()
{
printf("%%");
reads();
exit(0);
}

