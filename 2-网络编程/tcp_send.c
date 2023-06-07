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
#include<pthread.h>

//./client 192.168.1.66 50001
//客户端运行时传入服务器端的IP地址和端口号
int init_fd(char *ip_addr,char *port)
{
	//创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket sockfd failed!");
		return -1;
	}
	//定义服务器端的结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_port = htons(atoi(port));//将网络字节序的端口号转换成short型的主机字节序
	server_addr.sin_family = AF_INET;//设置协议族为网际层协议
	//将字符串形式的ip地址转换成struct addr_in类型（该结构体只有s_addr一个成员）
	// server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	//将字符串形式的ip地址转换成struct addr_in类型存入 协议族为AF_INET的服务器端的结构体中
	inet_pton(AF_INET,ip_addr,&server_addr.sin_addr);
	int  server_size = sizeof(server_addr);
	int ret;
	//与服务极端进行连接
	while(1)
	{
		ret = connect(sockfd,(struct sockaddr*)&server_addr,server_size);
		//返回值：成功：0 失败：-1；成功后sockfd变成已连接状态，可以进行通信
		if(ret == -1)
		{
			perror("connect failed!");
		}
		else 
			break;
	}
	printf("sockfd: %d\n",sockfd);
	return sockfd;//返回连接成功的套接字
}

void *recv_data(void* argv)
{
    int connfd = *(int*)argv;
    char buff[1024] = {0};
    int ret;
    while(1)
    {
        bzero(buff,sizeof(buff));
        ret =  recv(connfd,buff,sizeof(buff),0);
        printf("recv:%d\n",ret);
        printf("%s\n",buff);
    }
}

int main(int argc,char *argv[])
{
	int connfd = init_fd(argv[1],argv[2]);
    pthread_t tid;
    pthread_create(&tid,NULL,recv_data,(void*)&connfd);
	
	char buf[1024];
	while(1)
	{
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
		send(connfd,buf,sizeof(buf),0);
		if(!strcmp(buf,"quit"))
			break;
	}
	close(connfd);
}