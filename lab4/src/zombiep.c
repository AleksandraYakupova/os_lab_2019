#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child = fork();
    if (child > 0) 
    {
        printf("parent id - %d", getpid());
        printf("parent process falls asleep\n");
        sleep(20);
        printf("parent woke up\n");
    }
    else if(child == 0)
    {
        printf("child id - %d", getpid());
        printf("child exits\n");
        exit(0);
    }
    
    wait(NULL);    
    return 0;
}