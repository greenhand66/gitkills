#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<stdlib.h>

#define SHM_SIZE 1024

int main()
{
//共享内存的访问权限只有只读和可读可写，没有只写
    int shm_id = shmget(ftok(".",0),SHM_SIZE,IPC_CREAT|0666);
    if(shm_id == -1)
    {
        perror("shmget failed!");
        exit(-1);
    }
    printf("shm_id = %d\n",shm_id);

    char *p = shmat(shm_id,NULL,0);
    if(p == NULL)
    {
        perror("shmat failed!");
        exit(-2);
    }

    while(1)
    {
        fgets(p,SHM_SIZE,stdin);strtok(p,"\n");  
        if(strcmp(p,"exit") == 0)
            break;
    }

    shmdt(p);

    return 0;
}
