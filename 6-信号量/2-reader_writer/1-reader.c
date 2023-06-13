#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>

#define SHM_SIZE 128

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

static void sem_p(int semid,int semnum)
{
    struct sembuf op[1];
    op[0].sem_num = semnum;
    op[0].sem_op = -1;
    op[0].sem_flg = 0;
    semop(semid,op,1);
}

static void sem_v(int semid,int semnum)
{
    struct sembuf op[1];
    op[0].sem_num = semnum;
    op[0].sem_op = 1;
    op[0].sem_flg = 0;
    semop(semid,op,1);
}

static int sem_init(int semid,int semnum,int semval)
{
    union semun a;
    a.val = semval;
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

    int sem_id = semget(ftok(".",1),2,IPC_CREAT|0666);
    if(sem_id == -1)
    {
        perror("semget failed!");
        return -2;
    }
    // sem_init(sem_id,0,1);   //设置第一个信号量元素为写操作的空间资源,初始化为1
    // sem_init(sem_id,1,0);   //设置第二个信号量元素为读操作的数据资源,初始化为0

    char *p = shmat(shm_id,NULL,0);
    if(p == (void*)-1)
    {
        perror("shmat failed!");
        return -3;
    }

    sem_p(sem_id,1);    //读前申请读资源
    printf("%s\n",p);
    sem_v(sem_id,0);    //读后释放写资源

    shmdt(p);

    return 0;

}