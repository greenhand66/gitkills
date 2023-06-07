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

char filename[24];

//./client 192.168.1.66 50001
//参数1表示发送方的IP，参数2表示发双方的端口号
int init_fd(char *ip_addr,char *port)
{
	//创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket sockfd failed!");
		return -1;
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_family = AF_INET;
	// server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	inet_pton(AF_INET,ip_addr,&server_addr.sin_addr);
	int  server_size = sizeof(server_addr);
	int ret = connect(sockfd,(struct sockaddr*)&server_addr,server_size);
	//返回值：成功：0 失败：-1；成功后sockfd变成已连接状态，可以进行通信
	if(ret == -1)
	{
		perror("connect failed!");
		return -1;
	}
	printf("sockfd: %d\n",sockfd);
	return sockfd;
}

int recv_file(int connfd)
{
	char buf[50];
	int ret;
	int fd = open(filename,O_WRONLY|O_CREAT,0666);
	if(fd == -1)
	{
		perror("open file failed!");
		return -1;
	}
	while(1)
	{
		bzero(buf,sizeof(buf));
		ret = recv(connfd,buf,sizeof(buf),0);
		if(ret == 0)
			break;
		ret = write(fd,buf,strlen(buf));
		printf("write %d\n",ret);
	}
	close(fd);
}

void *recv_data(void *argv)
{
	int connfd = *(int*)argv;
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		int ret = recv(connfd,buf,sizeof(buf),0);//0发送标志，默认填0
		if(!strncmp(buf,"1 :",3))
		{
			bzero(filename,sizeof(filename));
			sscanf(buf,"1 : %s",filename);
			printf("将要会接收名为%s的文件\n",filename);
			continue;
		}
		if(!strncmp(buf,"2 :",3))
		{
			printf("开始发送\n");
			recv_file(connfd);
			continue;
		}
		printf("server recv:%s\n", buf);
		if(!strcmp(buf,"quit"))
			break;
	}
}

int send_file(int connfd)
{
	char buf[50];
	int ret;
	int fd = open(filename,O_RDONLY);
	if(fd == -1)
	{
		perror("open file failed!");
		return -1;
	}
	while(1)
	{
		bzero(buf,sizeof(buf));
		ret = read(fd,buf,strlen(buf));
		printf("read %d\n",ret);
		ret = send(connfd,buf,sizeof(buf),0);
		if(ret == 0)
			break;
	}
	close(fd);
}

int main(int argc,char *argv[])
{
	int connfd = init_fd(argv[1],argv[2]);
	
	pthread_t tid;
	pthread_create(&tid,NULL,recv_data,(void*)&connfd);
	char buf[50];
	while(1)
	{
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
		send(connfd,buf,sizeof(buf),0);
		if(!strncmp(buf,"1 :",3))
		{
			bzero(filename,sizeof(filename));
			sscanf(buf,"1 : %s",filename);
			printf("将要发送名为%s的文件\n",filename);
		}
		if(!strncmp(buf,"2 :",3))
		{
			printf("开始发送\n");
			send_file(connfd);
			continue;
		}
		if(!strcmp(buf,"quit"))
			break;
	}
	close(connfd);
}