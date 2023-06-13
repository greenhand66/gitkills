#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<string.h>
#include<strings.h>

#define SHM_SIZE 128

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short array;
    struct seminfo *__buf;
};

void sem_p(int semid,int semnum)
{
    struct sembuf op[1];
    op[0].sem_num = semnum;
    op[0].sem_op = -1;
    op[0].sem_flg = 0;
    semop(semid,op,1);
}

void sem_v(int semid,int semnum)
{
    struct sembuf op[1];
    op[0].sem_num = semnum;
    op[0].sem_op = 1;
    op[0].sem_flg = 0;
    semop(semid,op,1);
}

void sem_init(int semid,int semnum,int setval)
{
    union semun a;
    a.val = setval;
    semctl(semid,semnum,SETVAL,a);
}

int main()
{
    int shm_id = shmget(ftok(".",0),SHM_SIZE,IPC_CREAT|0666);
    if(shm_id == -1)
    {
        perror("shmget failed!");
        return -1;
    }

    char *p = shmat(shm_id,NULL,0);
    if(p == (void*)-1)
    {
        perror("shmat failed!");
        return -2;
    }

    int sem_id = semget(ftok(".",1),2,IPC_CREAT|0666);//申请2个元素的信号量组
    if(sem_id == -1)
    {
        perror("semget failed!");
        return -3;
    }

    sem_init(sem_id,0,1);   //将信号量组的第一个元素写资源（空间）初始化为1
    sem_init(sem_id,1,0);   //将信号量组的第二个元素读资源（数据）初始化为0

    //写入前先申请写资源
    sem_p(sem_id,0);
    //写入数据
    fgets(p,SHM_SIZE,stdin);strtok(p,"\n");
    //写完后释放读资源
    sem_v(sem_id,1);

    //撤销内存映射
    shmdt(p);

    return 0;
}