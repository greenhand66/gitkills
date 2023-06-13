#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>

//消息队列，又称增强型管道

int main()
{
    //ftok参数可以是任意内容，但是需要通信的双方必须相同
    key_t key_id1,key_id2;
    key_id1 = ftok(".",0);//第一个参数为键值，相当于文件路径
    key_id2 = ftok(".",1);
    return 0;
}