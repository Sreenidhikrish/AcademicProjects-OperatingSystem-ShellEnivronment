#include <stdio.h>
#include <string.h>
#include <stdib.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

#define MAX_LEN 	1024    /* Maximum Length of the command alogwith the arguments */
#define ARG_LEN 	256     /* Maximum Length of Argument */
#define ARG_COUNT	64    	/* Maximum number of arguments */


void parsecmd(char *,char **,int *); //Function to parse commands
void runpipe(int [], char **, char **);
int main(int argc, char *argv[])

{
       int num,x,fl,fl2,p;
       int fd[2];

       printf ("\n");
       printf ("\n");       
       printf ("*******************************************************\n");
       printf ("|                 WELCOME TO SKSH SHELL               |\n"); 
       printf ("*******************************************************\n"); 
       printf ("\n");
       printf ("\n");
       printf ("This simple shell can handle basic terminal commands   \n");
       printf ("and includes the following features                    \n");
       printf ("Basic command      : <command> <flags>                 \n");
       printf ("Output Redirection : <command> <flags> > <filename>    \n");
       printf ("Input  Redirection : <command> <flags> < <filename>    \n");
       printf ("Piping             : <command> <flags> | <command> <flags> \n");
       printf ("Background         : <command> <flags> &               \n");
       printf ("Exit the shell     : exit                              \n");
       printf ("Full path needs to be specified for all the commands   \n");
       printf ("A single whitespace acts as delimiter between all      \n");
       printf ("commands, arguments and special symbols                \n");


       while(1) {
         char 	*arg[ARG_COUNT];
         char 	*arg1[ARG_COUNT];
         char 	*arg2[ARG_COUNT];
         char 	cmd[MAX_LEN];
         char   cmdbuf[MAX_LEN];       
         int 	pid, status;
         num = 1;

  	 printf ("\nsksh%% "); 		

 	 fgets (cmd, MAX_LEN, stdin); 	
         strcpy(cmdbuf,cmd);
	 
         int nargs=0;
         int pos  =0; 
         parsecmd(cmdbuf,arg, &nargs);
         if(arg[0]==NULL) {
          continue;
         }
         if(strcmp(arg[0],"exit")==0) {
          break;
         }
 
         int i=0;
         int j=0;
         do {
          if(strcmp(arg[i],">")==0) {
           pos = i,num=2;
           break;
          }
          if(strcmp(arg[i],"<")==0) {
           pos = i,num=3;
           break;
          }
          if(strcmp(arg[i],"|")==0) {
           pos = i,num=4;
           break;
          }
          if(strcmp(arg[i],"&")==0) {
           pos = i,num=5;
           break;
          }
          i++;          
         } while(i<nargs);
         
         i=0;
         j=0;
         if((num==2)||(num==3)||(num==4))
         {           
          do { 
           arg1[i++]=arg[j++];
           } while(j<pos);
          i=0,j++;
          do { 
           arg2[i++]=arg[j++];
           } while(j<nargs);
          arg2[i]=NULL; 
         }  
           
         if(num==5)
         {
          i=0,j=0;
          do { 
           arg1[i++]=arg[j++];          
          } while(j<pos);
         }
      
         arg1[pos]=NULL;
           
//         printf("arg1 %s %s\n",arg1[0],arg1[1]);
//         printf("arg2 %s %s %s\n",arg2[0],arg2[1],arg2[2]);

         x= pipe(fd);
         if(x<0) {
           printf("\nUnable to Pipe: Error \n");
         }
         else {          
           pid = fork();  /*Creation of Process*/

           switch(pid)
           {
 
              case 0: /*Child process */
                if( num == 1 )
                {
//                 printf("\nExecuting one command \n");
                 execv( arg[0], arg);
                 perror("\nError in executing execl\n"); /*If execv fails */
               
                }
              
                if( num ==2)
                {
//                 printf("\nImplementing output redirection \n");
                 fl = open(arg2[0],2); /*Opening file in Read/Write Mode */ 
                 dup2(fl,1); /*Implementation of redirection of file output & Std out */ 
                 execv( arg1[0], arg1);
                 perror("\nError in executing execl\n"); /*If execl fails */

                }
              
                if( num == 3)
                {
//                 printf("\nImplementing input redirection \n");
                 fl2 = open(arg2[0],2); /*Opening file in Read/Write Mode */     
                 dup2(fl2,0); /*Implementation of redirection of file input & Stdin */ 
                 execv(arg1[0], arg1);
                 perror("\nError in executing execl\n"); /*If execl fails */
                }
              
                if( num == 4 )
                {
//                 printf("\nPreparing for calling Pipe implementation function\n");
                 runpipe(fd,arg1,arg2); /*Pipe implementation function called */              
                }
          
                if( num == 5 )
                {
//                 printf("\nImplementing Background process \n");
                 execv(arg1[0],arg1);
                 perror("\nError in executing execl\n"); /*If execl fails */

                }           

              case -1: printf("\nError in Fork execution\n");
              exit(1);
    
              default: /*Parent process*/
             
                if( num != 5)
                { 
 
//                 printf("\nParent process waits for Child process to terminate\n");

                 while( (p=waitpid(pid, &status,0))!=-1)
//               printf("\nChild exited with status  %d",WEXITSTATUS(status));
                 break;

                }
                else
                {
//                 printf("\nParent process is not waiting for child \n");
                 sleep(1);
                 break;
                }
           }
         }
       }
       printf("bye\n");
       return 0;
}

void parsecmd(char *cmdbuf,char **arg,int *narg )
{     

       int i = 0;
       char *delim=" \n";
       char *strptr;
       arg[i]=strtok_r(cmdbuf,delim,&strptr);
       while(arg[i]!=NULL) {  
         arg[++i]=strtok_r(NULL,delim,&strptr);
         (*narg)++;
       }               
} 

void runpipe(int pfd[], char **arg1, char **arg2)
{
	int t;

	switch(t = fork()) 
              {

	case 0: /* Child process*/
                printf("\nPipe implementation process going on\n");
		dup2(pfd[0], 0); /*Redirection of pipe input and std in */
		close(pfd[1]);	/* Child does not need this end of the pipe */
		execv(arg2[0], arg2);
		perror("\n error in executing execl in child \n");

	default: /* Parent process */
		dup2(pfd[1], 1); /*Redirection of pipe output and std output*/
		close(pfd[0]);	/* Parent does not need this end of the pipe */
		execv(arg1[0], arg1);
		perror("\nerror in executing execl in parent\n");

	case -1:perror("\nError in Fork execution\n");
		exit(-1);
              }
}


