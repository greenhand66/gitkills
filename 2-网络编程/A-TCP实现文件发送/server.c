#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include <string.h>
#include<unistd.h>
#include<pthread.h>
#include<fcntl.h>
#include<sys/stat.h>

char filename[24];

//./server 192.168.1.66 50001
// 传入服务器IP和端口号，供服务器端与套接字绑定

/*函数功能：
	定义一个套接字，初始化后返回，然后返回与客户连接成功的套接字
参数：
	port:要绑定的端口号
	IP_addr: 要绑定的IP地址
返回值：
	成功： 连接成功的套接字
	失败： -1
*/
int bind_sockfd(char *IP_addr,char *port)
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("make socket failed!");
		return -1;
	}
	//存放IP地址和端口号
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	inet_pton(AF_INET,IP_addr,&server_addr.sin_addr);

	bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	listen(sockfd,5);

	struct sockaddr_in client_addr;
	int client_size = sizeof(client_addr);
	bzero(&client_addr,client_size);
	//等待连接，连接成功返回已连接套接字
	//accept会阻塞等待
	int connfd = accept(sockfd,(struct sockaddr*)&client_addr,&client_size);
	if(connfd == -1)
	{
		perror("connect failed!");
		return -1;
	}
	printf("client is[%d]<%s> %d\n",ntohs(client_addr.sin_port),inet_ntoa(client_addr.sin_addr),connfd);
	close(sockfd);
	return connfd;
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
		printf("server read file:%s\n",buf);
		if(!strcmp(buf,"exit"))
			break;
		ret = write(fd,buf,strlen(buf));
		// printf("write %d\n",ret);
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
		printf("server recv:%s\n", buf);
		if(!strncmp(buf,"1 :",3))
		{
			bzero(filename,sizeof(filename));
			sscanf(buf,"1 : %s",filename);
			printf("将要接收名为%s的文件\n",filename);
			continue;
		}
		if(!strncmp(buf,"2 :",3))
		{
			printf("开始接收\n");
			recv_file(connfd);
			continue;
		}
		if(!strcmp(buf,"quit"))
			break;
	}
	close(connfd);
	exit(0);
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
		ret = read(fd,buf,sizeof(buf));
		//ret存放读取成功的字节数（小于或等于sizeof(buf)）
		send(connfd,buf,sizeof(buf),0);
		if(ret < sizeof(buf))	//文件读取完成退出文件读取
			break;
	}
	bzero(buf,sizeof(buf));
	//文件传输完成，发送端写入exit表示文价传输完成
	strcpy(buf,"exit");
	send(connfd,buf,sizeof(buf),0);
	close(fd);
}

int main(int argc,char *argv[])
{
	int connfd = bind_sockfd(argv[1],argv[2]);
	if(connfd == -1)
	{
		perror("connect failed!");
		return -1;
	}
	
	pthread_t tid;
	//创建线程接收数据
	pthread_create(&tid,NULL,recv_data,(void*)&connfd);
	char buf[50];
	int ret;
	while(1)
	{
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
		ret = send(connfd,buf,sizeof(buf),0);
		printf("connfd %d\n",connfd);
		printf("ret %d\n",ret);
		//当发送数据前缀为“1 ：”时表示一个文件名
		if(!strncmp(buf,"1 :",3))
		{
			bzero(filename,sizeof(filename));
			sscanf(buf,"1 : %s",filename);
			printf("将要发送名为%s的文件\n",filename);
		}
		//当发送数据前缀为“2 ：”时表示开始传输一个文件
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