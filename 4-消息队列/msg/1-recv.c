#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdlib.h>

//消息队列信息结构体
struct msgbuf{
    long mtype;      /*消息类型(>)*/
    char mtext[100];    /*消息正文（长度自定*/
};

int main()
{
    //1.创建或打开消息队列
    int msgid;
    msgid = msgget(ftok(".",0),IPC_CREAT|0666); 
    if(msgid == -1)
    {
        perror("msgget failed");
        exit(1);
    }

    //2.向消息队列读取数据
    struct msgbuf data;
    bzero(&data,sizeof(struct msgbuf));
    data.mtype = 123;
    
        //错误写法：    第2个参数必须是整个数据包的地址
    //msgrcv(msgid,data.mtext,sizeof(data.mtext),data.mtype,0);

    msgrcv(msgid,&data,sizeof(data.mtext),data.mtype,0);
    printf("get: %s\n",data.mtext);


	// 3.如不需要使用，可删除该消息队列
    // msgctl(msgid,IPC_RMID,NULL);
    
    return 0;
}