#include <stdio.h>
#include <arpa/inet.h>	
#include <stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

int socket_init(char *port)
{
	//创建套接
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("make socket error\n");
		return -1;
	}
	//绑定套接字
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	//将套接字设置为监听套接字
	listen(sockfd,5);
	//等待连接
	struct sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	bzero(&client_addr,client_addr_size);
	int connfd = accept(sockfd,(struct sockaddr *)&client_addr,&client_addr_size);
	if(connfd  < 0)
	{
		perror("connect error\n");
		return -1;
	}
	close(sockfd);
	//返回已连接套接字
	return connfd;
}

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		perror("input error!");
		return -1;
	}
	int connfd = socket_init(argv[1]);
	if(-1 == connfd)
		return -1;
	//定义一个集合
	fd_set set;
	int fdmax = connfd > STDIN_FILENO ? connfd : STDIN_FILENO;
	char rbuf[50];
	char sbuf[50];
	struct timeval wait_time;
	
	while(1)
	{
		//初始化集合
		FD_ZERO(&set);				//清空集合
		FD_SET(connfd,&set);		//将已连接套接字添加到集合
		FD_SET(STDIN_FILENO,&set);	//将标准输入的文件描述符添加到集合

		wait_time.tv_sec = 5;
		//监听集合
		if(select(fdmax+1,&set,NULL,NULL,&wait_time) == 0)
		{
			printf("time out!\n");
			continue;
		}
		//判断集合剩下的成员
		if(FD_ISSET(connfd,&set))//如果是套接字说明有数据发送过来
		{
			bzero(rbuf,sizeof(rbuf));
			recv(connfd,rbuf,sizeof(rbuf),0);
			printf("recv:%s\n",rbuf);
			if(!(strncmp(rbuf,"quit",4)))
				break;
		}
		if(FD_ISSET(STDIN_FILENO,&set))//如果是标准输入说明要发送数据
		{
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			send(connfd,sbuf,strlen(sbuf),0);
			if(!(strncmp(sbuf,"quit",4)))
				break;
		}
	}
	//关闭套接字
	close(connfd);
	return 0;
}









