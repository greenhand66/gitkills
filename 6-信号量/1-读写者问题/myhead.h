#ifndef _MYHEAD_H_
#define _MYHEAD_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<strings.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>

#define SHMSZ 1024//申请共享内存大小要是页面大小的倍数，即1k（1024）的倍数

/*
    int semctl(int semid,int semnum,int cmd,...);
使用示例：
    int semctl(semid, 0, GETVAL);     //这时表示我要获取信号量序号(下标)为 0 的值, 成功返回信号量序号为 0 对应的值.
    int semctl(semid, 0, SETVAL , 5); //这时表示我要设置信号量序号(下标)为 0 的值为 5. 成功则返回 0.
    int semctl(semid, 0, IPC_RMID);   //这时表示我要删除信序号为0的信号量.
*/
//semctl()的第四个参数是一个联合体，必须用户自己定义
union semun //自定义信号量操作联合体
{
    int val;  //当cmd为SETVAL时使用
    struct semid_ds *buf;//属性信息结构体（semctl的cmd参数为IPC_STAT和IPC_SET时需要使用）
    unsigned short *array;//当cmd为GETALL或SETALL使用
    struct seminfo *__buf;//当cmd为IPC_INFO和SEM_INFO时要使用
};

/*信号量操作结构体定义
struct sembuf
{
       unsigned short sem_num;  //需要操作的信号量数组下标 
       short          sem_op;   // 该信号量数组下标的值操作,加法(V操作)或减法(P操作) 
       short          sem_flg;  // 操作选项,SEM_UNDO,(还原信号量的值) 
       //你一开始设置了一个值,在做P操作或则V操作时,会改变信号量数组对应元素的值,Linux系统会保留其值,
       //设置这个宏定义之后,下次运行程序时,信号量的值会恢复原始你设置的值.(相当于复位).
}

struct sembuf
{
    unsigned short sem_num; //信号量元素序号（数组下标）,从0开始
    short sem_op;//操作参数
    short sem_flg;//操作选项
};
根据sem_op数值的不同，分为3种情况:
    1)大于0时，进行V（释放资源）操作，则信号量数组中下标为sem_num的元素会变为加上sem_op之后的和
    2）等于0时，若此时要操作的信号量元素的值恰好为0，则（信号量p/v操作函数）semop()立即返回0；
            否则若要操作的信号量元素的值不为0，则使进程进入睡眠，直到以下情况发生:
                (1) 要操作的信号量元素的值变为0
                (2) 信号量被删除  
                (3) 收到信号
            (2)(3)都会导致semop()出错退出，错误码为EINTR
    3）小于0时，进行P（申请资源）操作,则要操作的信号量元素的值变为加上sem_op之后的和(相当于减去sem_op的绝对值)
        若要操作的信号量元素的值 大于或者 等于 sem_op 的绝对值，则立即成功返回
        否则 与 sem_op等于0时情况相同，也是进程一直休眠，知到出现三种情况之一.仅（1）要操作的信号量元素的值变得大于或等于sem_op的绝对值
*/
//信号量组中每个元素代表一种临界（共享）资源

static void sem_p(int semid,int semnum)//p操作
{
    struct sembuf op[1];
    op[0].sem_num = semnum; //设置要操作的信号量元素的下标值
    op[0].sem_op = -1;  //申请资源，资源数-1
    op[0].sem_flg = 0;  //操作选项默认

    semop(semid,op,1);
}
/*信号量p/v操作函数
头文件： #include<sys/types.h>
        #include<sys/ipc.h>
        #include<sys/sem.h>
函数原型:
        int semop(int semid,struct sembuf *spos,size_t nsops);
参数：
        semid ： 信号量id
        sops : 信号量操作结构体数组
        nsops : 需要操作几次.
返回值：
        成功： 0
        失败： -1
注释：
    1.当某一个信号量数组下标的值为 0 时,如果要继续进行 P(减法) 操作,会导致当前进程阻塞.
      信号量的值不可能为负数.假设你将该值设置为负数的话,该数据则不会生效.
      
    2.信号量在做 V(加法) 操作时,永远不会阻塞.
*/
static void sem_v(int semid,int semnum)//v操作
{
    struct sembuf op[1];
    op[0].sem_num = semnum;
    op[0].sem_op = 1;
    op[0].sem_flg = 0;

    semop(semid,op,1);
}

/*
传入信号量id，信号量元素序号，要设置成的值
将信号量中元素设置为指定值
*/
static void seminit(int semid,int semnum,int value)//初始化
{
    union semun a;
    a.val = value;
    semctl(semid,semnum,SETVAL,a);//将第semnum个元素初始化为value
}

#endif