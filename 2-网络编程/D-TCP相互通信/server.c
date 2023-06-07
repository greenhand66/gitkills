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
	//存放IP地址和端口号
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	inet_pton(AF_INET,IP_addr,&server_addr.sin_addr);
	//绑定
	bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	//将套接字设置为监听
	listen(sockfd,5);
	//申请存放客户端IP地址等信息的结构体
	struct sockaddr_in client_addr;	
	//获取该结构体大小	
	int client_size = sizeof(client_addr);
	bzero(&client_addr,client_size);//清空结构体
	//等待连接，连接成功返回已连接套接字
	int connfd = accept(sockfd,(struct sockaddr *)&client_addr,&client_size);
	if(-1 == connfd)
	{
		perror("connect fail\n");
		return -1;
	}		
	printf("client is [%d]<%s>\n",client_addr.sin_port,inet_ntoa(client_addr.sin_addr));
	close(sockfd);
	return connfd;
}

void *recv_data(void *arg)
{
	//接收套接字
	int connfd = *(int *)arg;
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		recv(connfd,buf,sizeof(buf),0);
		printf("recv is %s\n",buf);
		if(!(strncmp(buf,"quit",4)))
		{
			close(connfd);
			exit(0);
		}
	}
}

//./server 192.168.1.66 50001
int main(int argc,char *argv[])
{
	int connfd = socket_init(argv[2],argv[1]);
	
	pthread_t tid;
	pthread_create(&tid,NULL,recv_data,(int *)&connfd);
	
	//接收数据
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		scanf("%s",buf);
		send(connfd,buf,strlen(buf),0);
		if(!(strncmp(buf,"quit",4)))
			break;
	}
	//关闭套接字
	close(connfd);

	return 0;
}


