#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
/*
函数功能：
	定义一个TCP套接字，初始化后返回
参数：
	port：要绑定的端口号
	IP_addr：要绑定的IP地址
返回值：
	成功：已连接套接字
	失败：-1
*/
int socket_init(char *port,char *IP_addr)
{
	//创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("make socket fail\n");
		return -1;
	}
    struct timeval wait_time = {5,0};
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(void*)&wait_time,sizeof(wait_time));
	//存放对方的IP地址和端口号
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;			//地址族
	server_addr.sin_port = htons(atoi(port));//端口号
	inet_pton(AF_INET,IP_addr,&server_addr.sin_addr);//IP地址
	//根据对方IP地址进行连接
	connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	return sockfd;	
}


//./client 192.168.1.99 50001
int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		perror("input error!");
		return -1;
	}
	int sockfd = socket_init(argv[2],argv[1]);
	
	//定义一个集合
	fd_set set;
	int fdmax = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;
	char rbuf[50];
	char sbuf[50];
	int ret;
	struct timeval wait_time;
	while(1)
	{
		//初始化集合
		FD_ZERO(&set);				//清空集合
		FD_SET(sockfd,&set);		//将已连接套接字添加到集合
		FD_SET(STDIN_FILENO,&set);	//将标准输入的文件描述符添加到集合
		wait_time.tv_sec = 5;
		//监听集合
		ret = select(fdmax+1,&set,NULL,NULL,&wait_time);
		if(ret == 0)
		{
			printf("time out!\n");
			continue;
		}
		//判断集合剩下的成员
		if(FD_ISSET(sockfd,&set))//如果是套接字说明有数据发送过来
		{
			bzero(rbuf,sizeof(rbuf));
			ret = recv(sockfd,rbuf,sizeof(rbuf),0);
            printf("recv return: %d\n",ret);
			printf("recv:%s\n",rbuf);
			if(!(strncmp(rbuf,"quit",4)))
				break;
		}
		if(FD_ISSET(STDIN_FILENO,&set))//如果是标准输入说明要发送数据
		{
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			send(sockfd,sbuf,strlen(sbuf),0);
			if(!(strncmp(sbuf,"quit",4)))
				break;
		}
	}
	//关闭套接字
	close(sockfd);
	return 0;
}




