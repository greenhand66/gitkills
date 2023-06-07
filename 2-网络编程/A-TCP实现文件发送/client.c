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
	//与服务极端进行连接
	int ret = connect(sockfd,(struct sockaddr*)&server_addr,server_size);
	//返回值：成功：0 失败：-1；成功后sockfd变成已连接状态，可以进行通信
	if(ret == -1)
	{
		perror("connect failed!");
		return -1;
	}
	printf("sockfd: %d\n",sockfd);
	return sockfd;//返回连接成功的套接字
}

/*
	从服务器端接受文件
传入已来连接的套接字
*/
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
		//读取完成，服务器端写入exit，客户端接收到exit，退出接收文件函数
		if(!strcmp(buf,"exit"))	
			break;
		ret = write(fd,buf,strlen(buf));
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
		int ret = recv(connfd,buf,sizeof(buf),0);//0为发送标志，默认填0
		printf("ret :%d\n",ret);
		printf("server recv:%s\n", buf);
		//当接收到数据前缀为“1 ：”时表示收到了一个文件名
		if(!strncmp(buf,"1 :",3))
		{
			bzero(filename,sizeof(filename));
			//将文件名按格式存储到全局变量filename中
			sscanf(buf,"1 : %s",filename);
			printf("将要会接收名为%s的文件\n",filename);
			//结束本次循环，继续进行下一次的数据接收
			continue;
		}
		if(!strncmp(buf,"2 :",3))
		{
			//当接收数据前缀为“2 ：”时表示开始传输一个文件
			printf("开始接收\n");
			recv_file(connfd);
			//文件传输完成，结束本次循环，继续进行下一次的数据接收
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