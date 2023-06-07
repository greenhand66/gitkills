#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

//UDP通信，接收端

int fd;

// SIGIO信号响应函数
void sigio_catch(int signum)
{
	// 3.坐等数据，接收数据	recvfrom
	char msg[100];
	struct sockaddr_in dest_addr;
	socklen_t len = sizeof(dest_addr);
	bzero(&dest_addr, sizeof(dest_addr));
	bzero(msg, sizeof(msg));

	recvfrom(fd, msg, sizeof(msg), 0, (struct sockaddr *)&dest_addr, &len);

	printf("%s:%d:%s\n", 
		inet_ntoa(dest_addr.sin_addr), 
		ntohs(dest_addr.sin_port), msg);

	sendto(fd, msg, strlen(msg), 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

// ./server 50001（自身端口）
int main(int argc, char const *argv[])
{
	// 1.搞个信箱，创建一个UDP套接字	socket
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd == -1)
	{
		perror("socket failed..");
		exit(1);
	}

	// 2.绑定地址，绑定IP信息（IP地址+端口port）	bind
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	// inet_pton(AF_INET, "192.168.12.116", (void *)&my_addr.sin_addr);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//0.0.0.0

	bind(fd, (const struct sockaddr *)&my_addr, sizeof(my_addr));

	// 2.注册 SIGIO 信号（signal函数）
	signal(SIGIO, sigio_catch);

	// 3.设置信号属主（fcntl函数）
	fcntl(fd, F_SETOWN, getpid());

	// 4.设置套接字为异步信号模式 O_ASYNC （fcntl函数）
	fcntl(fd, F_SETFL, O_ASYNC);

	int i=0;
	//一直等待信号
	while(1)
	{
		printf("%d\n", i);
		i++;
		sleep(1);
	}

	// 6.关闭 close
	close(fd);

	return 0;
}
