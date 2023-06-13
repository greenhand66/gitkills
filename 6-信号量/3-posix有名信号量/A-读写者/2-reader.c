#include"myhead.h"

int main()
{
    int id = shmget(ftok(".",0),SHM_SIZE,IPC_CREAT|0666);
    char *shmaddr = shmat(id,NULL,0);

    //创建或打开POSIX 有名信号量
    sem_t *s;
    s = sem_open(SEM_NAME,O_CREAT,0777,0);
    
    //当取得信号量资源时，才能访问SHM
    while(1)
    {
        sem_wait(s);

        if(!strncmp(shmaddr,"quit",4))  //若对方写入quit，则退出
            break;
    }

    //关闭POSIX有名信号量
    sem_close(s);
    return 0;
}