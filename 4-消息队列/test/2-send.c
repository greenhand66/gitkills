#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

// 消息队列信息结构体
struct msgbuf{
	long mtype;       	/* 消息类型（>0） */
	char mtext[100];    /* 消息正文（长度自定） */
};

int main()
{
	// 1.创建或打开消息队列
	int msgid;
	msgid = msgget(ftok(".", 0), IPC_CREAT|0666);
	if(msgid == -1)
	{
		perror("msgget failed");
		exit(1);
	}

	// 2.向消息队列写入数据
	struct msgbuf data;
	bzero(&data, sizeof(struct msgbuf));
	data.mtype = 123;
	// data.mtext = "nihao";	//错误写法，mtext是数组名
	// strcpy(data.mtext, "nihao");
    fgets(data.mtext,sizeof(data.mtext),stdin);strtok(data.mtext,"\n");

	msgsnd(msgid, &data, strlen(data.mtext), 0);

	// 3.如不需要使用，可删除该消息队列
	// msgctl();

	return 0;
}
