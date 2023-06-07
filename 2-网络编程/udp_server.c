#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

//./server 192.168.1.99 50001
int main(int argc,char *argv[])
{
	// 1、创建UDP套接字
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(-1 == sockfd)
	{
		perror("make socket fail\n");
		return -1;
	}
    int on = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&on,sizeof(on));
	// 2、绑定自己的IP地址和端口号
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	
	struct sockaddr_in client_addr;
	int client_size = sizeof(client_addr);
	bzero(&client_addr,client_size);
    // struct sockaddr_in bcast;
    // bcast.sin_family = AF_INET;
    // bcast.sin_addr.s_addr = inet_addr("192.168.1.255");
    // bcast.sin_port = htons(atoi("8080"));
	// 3、接收数据
	char buf[50];
    int ret;
	while(1)
	{
		bzero(buf,sizeof(buf));
		recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&client_addr,&client_size);
		printf("[%s]<%d>: %s\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),buf);
        client_addr.sin_addr.s_addr = inet_addr("192.168.1.255");
        // client_addr.sin_port = htons(atoi("8080"));
        ret = sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&client_addr,sizeof(client_addr));
        // ret =sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&bcast,sizeof(bcast));
		printf("ret: %d\n",ret);
		printf("send to [%s]<%d>: %s\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),buf);
        if(!(strncmp(buf,"quit",4)))
		{
			break;
		}
	}
	// 4、关闭套接字
	close(sockfd);
	
	return 0;
}

