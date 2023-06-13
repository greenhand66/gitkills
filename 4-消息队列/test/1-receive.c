#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>

struct msgbuf{
    long mtype; //固定且必须大于0
    char mtext[100];
};

int main()
{
    int msgid;
    msgid = msgget(ftok(".",0),IPC_CREAT|0666);
    if(msgid == -1)
    {
        perror("msgget failed!");
        return -1;
    }

    struct msgbuf data;
    bzero(&data,sizeof(data));
    data.mtype = 123;

    msgrcv(msgid,&data,sizeof(data.mtext),data.mtype,0);
    // msgrcv(msgid,&data,sizeof(data.mtext),data.mtype,IPC_NOWAIT);
/*
msgid: 接收消息的消息队列ID
&data: 接收消息的存储区域指针(必须是整个数据域的地址)
sizeof(data.mtext): 要接受数据的大小
data.mtype: 要接收消息的标识（类型）
IPC_WAIT: 非阻塞读出消息(当队列中没有指定类型数据时，立即返回)
*/
    printf("get: %s\n",data.mtext);

    // struct msqid_ds *buf;
    // msgctl(msgid,IPC_RMID,&buf);

    return 0;
}

/*
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
接口说明：
    msgqid：MSG对象的ID，由msgget()获取。
    msgp：一个指向等待被发送的消息的指针，由于MSG中的消息最大的特点是必须有一个整数标
识，用以区分MSG中的不同的消息，因此MSG的消息会使用一个特别的结构体来表达，具体如下
所示：
struct msgbuf
{
    long mtype;  // 消息类型（固定）
    // 消息正文（可变）
     // ... 
};
因此一般而言，msgp就是一个指向上述结构体的指针。
msgsz：消息正文的长度（单位字节），注意不含类型长度。
msgflg：发送选项，一般有：
0：默认发送模式，在MSG缓冲区已满的情形下阻塞，直到缓冲区变为可用状态。
IPC_NOWAIT：非阻塞发送模式，在MSG缓冲区已满的情形下直接退出函数并设置错误码为
EAGAIN.
*/


/*
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
接口说明：
msgqid：MSG对象的ID，由msgget()获取。
msgp：存放消息的内存入口。
msgsz：存放消息的内存大小。
msgtyp：欲接收消息的类型：
0：不区分类型，直接读取MSG中的第一个消息。
大于0：读取类型为指定msgtyp的第一个消息（若msgflg被配置了MSG_EXCEPT则读取除了
类型为msgtyp的第一个消息）。
小于0：读取类型小于等于msgtyp绝对值的第一个具有最小类型的消息。例如当MSG对象中
有类型为3、1、5类型消息若干条，当msgtyp为-3时，类型为1的第一个消息将被读取。
msgflg：接收选项：
0：默认接收模式，在MSG中无指定类型消息时阻塞。
IPC_NOWAIT：非阻塞接收模式，在MSG中无指定类型消息时直接退出函数并设置错误码为
ENOMSG.
MSG_EXCEPT：读取除msgtyp之外的第一个消息。
MSG_NOERROR：如果待读取的消息尺寸比msgsz大，直接切割消息并返回msgsz部分，读
不下的部分直接丢弃。若没有设置该项，则函数将出错返回并设置错误码为E2BIG。
*/