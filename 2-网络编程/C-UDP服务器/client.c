#include <stdio.h>
#include <arpa/inet.h>	
#include <stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>

int sockfd;
struct sockaddr_in server_addr;

//如果收到中断信号，就向服务器发送quit让服务器注销账号
void quit(int sig)
{
	char *str = "quit";
	sendto(sockfd,str,strlen(str),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
	close(sockfd);
	exit(0);
}

//如果套接字收到数据产生信号就去读取一遍信息
void recv_data(int sig)
{
	char buf[70];
	bzero(buf,sizeof(buf));
	struct sockaddr_in recv_addr;
	socklen_t recv_addr_size = sizeof(recv_addr);
	recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&recv_addr,&recv_addr_size);
	printf("%s\n",buf);	
}

int socket_init()
{
	// 1、创建UDP套接字
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(-1 == sockfd)
	{
		perror("make socket fail\n");
		return -1;
	}

    struct ip_mreq a;
    a.imr_interface.s_addr = inet_addr("192.168.1.164");
    a.imr_multiaddr.s_addr = inet_addr("224.0.0.10");
    setsockopt(sockfd,IPPROTO_TCP,IP_ADD_MEMBERSHIP,&a,sizeof(a));

    int n = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&n,sizeof(n));
	
	//随机一个端口号
	uint16_t port = (rand()+1024);
	
	//绑定自己的IP地址和端口号
	struct sockaddr_in my_addr;
	bzero(&my_addr,sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd,(struct sockaddr *)&my_addr,sizeof(my_addr));
	
	return sockfd;
}

//./client 192.168.1.99 对方的端口号 
int main(int argc,char *argv[])
{
	sockfd = socket_init();
	
	//声明信号响应函数
	signal(SIGIO,recv_data);//读取数据
	
    //信号驱动IO模型
	//设置套接字的属主
	fcntl(sockfd,F_SETOWN,getpid());
    //获取文件描述符状态
	int status = fcntl(sockfd,F_GETFL);
    //设置文件描述符为信号异步驱动
	status |= O_ASYNC;
	fcntl(sockfd, F_SETFL, status);
	
	//准备发送对象的IP地址和端口号
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&server_addr.sin_addr);
	signal(SIGINT,quit);//强制退出
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		scanf("%s",buf);
		sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		if(!(strncmp(buf,"quit",4)))
			break;
	}
	// 3、关闭套接字
	close(sockfd);
	return 0;
}
