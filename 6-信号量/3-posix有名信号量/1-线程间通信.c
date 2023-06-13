#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define SHM_SIZE 1024

int shm_id;
char *p;
sem_t *sem_p;

void *recv_data(void *argv)
{
    while(1)
    {
        sem_wait(sem_p);
        printf("read data: %s\n",p);
        if(!strncmp(p,"quit",4))
            break;
    }
    pthread_exit(NULL);
}

int main(void)
{
    //不存在名为sem1的信号量就以0666权限创建，并初始化为0
    sem_p = sem_open("/sem1",O_CREAT,0666,0);
    if(sem_p ==  SEM_FAILED)
    {
        perror("sem_open failed!");
        return -1;
    }

    shm_id = shmget(ftok(".",0),SHM_SIZE,IPC_CREAT|0666);
    if(shm_id == -1)
    {
        perror("shmget failed!");
        return -2;
    }

    p = shmat(shm_id,NULL,0);
    if(p == (void*)-1)
    {
        perror("shmat failed!");
        return -3;
    }

    pthread_t tid;
    pthread_create(&tid,NULL,recv_data,NULL);

    while(1)
    {

        fgets(p,SHM_SIZE,stdin);strtok(p,"\n");
        sem_post(sem_p);
        if(!strncmp(p,"quit",4))
            break;
    }

    pthread_join(tid,NULL);
    shmdt(p);
    shmctl(shm_id,IPC_CREAT,NULL);
    sem_close(sem_p);
    sem_unlink("/sem1");
    return 0;
}