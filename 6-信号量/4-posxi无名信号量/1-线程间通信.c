#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>

#define SHM_SIZE 1024

sem_t sem;
int shm_id;
char *p;

void *recv_data(void *argv)
{
    while(1)
    {
        sem_wait(&sem);
        printf("read data: %s\n",p);
        if(!strncmp(p,"quit",4))
            break;
    }
    pthread_exit(NULL);
}

int main()
{
    //将代表数据资源的信号量初始化为0
    sem_init(&sem,0,0);

    shm_id = shmget(ftok(".",0),SHM_SIZE,IPC_CREAT|0666);
    if(shm_id == -1)
    {
        perror("shmget failed!");
        return -1;
    }

    p = shmat(shm_id,NULL,0);
    if(p == (void*)-1)
    {
        perror("shmat failed!");
        return -2;
    }

    pthread_t tid1;
    pthread_create(&tid1,NULL,recv_data,NULL);

    while(1)
    {
        fgets(p,SHM_SIZE,stdin);strtok(p,"\n");
        sem_post(&sem);
        if(!strncmp(p,"quit",4))
            break;
    }

    pthread_join(tid1,NULL);
    shmdt(p);
    shmctl(shm_id,IPC_RMID,NULL);
    sem_destroy(&sem);
    return 0;
}