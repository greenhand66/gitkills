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

int init_fd(char *port,char *IP_addr)
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
	
    int stu = fcntl(sockfd,F_GETFL);
    stu |= O_NONBLOCK;
    int n = fcntl(sockfd,F_SETFD,stu);
    printf("F_SETFL %d\n",n);

	//将套接字设置为监听
	listen(sockfd,5);
	//申请存放客户端IP地址等信息的结构体
	struct sockaddr_in client_addr;	
	//获取该结构体大小	
	int client_size = sizeof(client_addr);
	bzero(&client_addr,client_size);//清空结构体
	//等待连接，连接成功返回已连接套接字
    int connfd;
    while(1)
    {
        connfd = accept(sockfd,(struct sockaddr *)&client_addr,&client_size);
        if(-1 == connfd)
        {
            perror("connect fail\n");
        }	
        else 
            break;
    }
 	
	printf("client is [%d]<%s>\n",client_addr.sin_port,inet_ntoa(client_addr.sin_addr));
	close(sockfd);
	return connfd;
}

int main(int argc,char *argv[])
{
	int connfd = init_fd(argv[2],argv[1]);
    int stu = fcntl(connfd,F_GETFL);
    stu |= O_NONBLOCK;
    int n = fcntl(connfd,F_SETFD,stu);
    printf("F_SETFL %d\n",n);
	char buf[50];
	while(1)
	{
        bzero(buf,sizeof(buf));
		int ret = recv(connfd,buf,sizeof(buf),0);//0发送标志，默认填0
		printf("server recv:%s\n", buf);
		if(!strcmp(buf,"quit"))
			break;
	}
	close(connfd);
}