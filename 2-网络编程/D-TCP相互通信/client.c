#include <stdio.h>
#include <arpa/inet.h>	
#include <stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

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

void *recv_data(void *arg)
{
	//接收套接字
	int sockfd = *(int *)arg;
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		recv(sockfd,buf,sizeof(buf),0);
		printf("recv is %s\n",buf);
		if(!(strncmp(buf,"quit",4)))
		{
			close(sockfd);
			exit(0);
		}
	}
}

//./client 192.168.1.66 50001
int main(int argc,char *argv[])
{
	int sockfd = socket_init(argv[2],argv[1]);
	
	pthread_t tid;
	pthread_create(&tid,NULL,recv_data,(int *)&sockfd);
	
	//发送消息给服务器
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		scanf("%s",buf);
		send(sockfd,buf,strlen(buf),0);
		if(!(strncmp(buf,"quit",4)))
			break;
	}
	//关闭套接字
	close(sockfd);
	return 0;
}




