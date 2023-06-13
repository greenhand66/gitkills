#include"myhead.h"

int main(int argc,const char **argv)
{
    //获取SHM的ID，并将其映射到本进程虚拟内存空间中
    int shmid = shmget(ftok(".",0),SHMSZ,IPC_CREAT|0644);
    char *shmaddr = shmat(shmid,NULL,0);

    //获取SEM的ID，若新建则初始化，否则直接获取其ID
    int semid = semget(ftok(".",1),2,IPC_CREAT|IPC_EXCL|0644);
    if(semid == -1 && errno == EEXIST)
    {
        semid = semget(ftok(".",1),2,0644);//直接获取SEM的ID
    }
    else
    {
        seminit(semid,0,0); //将第0个元素初始化为0，代表数据
        seminit(semid,1,1); //将第1个元素初始化为1，代表空间
    }

    while(1)
    {
        sem_p(semid,1);//向第1个信号量元素申请内存空间资源
        fgets(shmaddr,SHMSZ,stdin);strtok(shmaddr,"\n");//写入数据到内存
        sem_v(semid,0);//增加代表数据资源的第0个信号量元素的值
    }

    return 0;
}