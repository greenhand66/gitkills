#include"myhead.h"

int main()
{
    int id = shmget(ftok(".",0),SHM_SIZE,IPC_CREAT|0666);
    char *shmaddr = shmat(id,NULL,0);

    //创建或打开POSIX 有名信号量
    sem_t *s;
    s = sem_open(SEM_NAME,O_CREAT,0777,0);
    
    //每当向共享内存写入数据后，就让信号量的值加1
    while(1)
    {
        fgets(shmaddr,SHM_SIZE,stdin);strtok(shmaddr,"\n");
        sem_post(s);

        if(!strncmp(shmaddr,"quit",4))  //输入quit退出
            break;
    }

    //关闭并且删除信号量
    sem_close(s);
    sem_unlink(SEM_NAME);
    return 0;
}