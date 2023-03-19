#include<stdio.h>
#include<unistd.h>
#include<wait.h>
void fork1()
{
pid_t pid;
    int child_status;
    pid=fork();
    if (pid==0)
    {
        printf("child %d\n",pid);
    }
    else{
        printf("parent %d\n",pid);
        wait(&child_status);
        printf("terminate");
    }
}
void handler(){
    
}
int main(int argc,char *argv[],char*envp[])

{
    // int i;
    // printf("env:\n");
    // for (i=0;envp[i]!=0;i++)
    // {
    //     printf("envp[%2d]:%s\n",i,envp[i]);
    // }   
}