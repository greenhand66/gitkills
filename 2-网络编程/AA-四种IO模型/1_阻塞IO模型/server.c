#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct client_info{
	int con_fd;
	struct sockaddr_in con_addr;
};

// 子线程（保姆）：接收新连接的数据，并回射
void *thread_con(void *arg)
{
	struct client_info *temp = (struct client_info *)arg;

	printf("new connection[%d:%s:%hu]!\n", 
				temp->con_fd, 
				inet_ntoa(temp->con_addr.sin_addr), 
				ntohs(temp->con_addr.sin_port));

	char recv_buf[50];
	int n_recv;
	while(1)
	{
		// 1.接受客户端数据
		bzero(recv_buf, sizeof(recv_buf));
		n_recv = recv(temp->con_fd, recv_buf, sizeof(recv_buf), 0);
		if(n_recv == 0)
		{
			printf("[%d:%s:%hu] disconnect.\n", 
						temp->con_fd, 
						inet_ntoa(temp->con_addr.sin_addr), 
						ntohs(temp->con_addr.sin_port));
			close(temp->con_fd);	// 关闭客户端连接（关闭通信）
			free(temp);				// 释放客户端结构体堆空间
			pthread_exit(NULL);		// 结束当前线程
		}
		printf("Get(%d:%s:%hu:%d): %s\n", 
					temp->con_fd, 
					inet_ntoa(temp->con_addr.sin_addr), 
					ntohs(temp->con_addr.sin_port), 
					n_recv, 
					recv_buf);

		// 2.将数据回发
		send(temp->con_fd, recv_buf, n_recv, 0);
	}
}

// ./server 8888(自身端口)
int main(int argc, const char **argv)
{
	// 0.判断用户输入参数
	if(argc != 2)
	{
		printf("Input Error!\n");
		exit(0);
	}

	// 1.创建TCP套接字
	int sock_fd;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
	{
		perror("socket failed");
		exit(1);
	}

	// 2.绑定IP+PORT（一般手动设置，便于客户端连接）
		// 设置IPv4本地地址（服务器地址）
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));	// 本地服务器端口
	inet_aton("0.0.0.0", &my_addr.sin_addr);	// 任意网卡IP

	if(bind(sock_fd, (const struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		perror("bind failed");
		exit(2);
	}

	// 3.设置监听端口(同时接收连接请求数：3+4=7个)
		// 如成功，sock_fd会变成《监听套接字》，只能接收连接请求，不能通信。
	if(listen(sock_fd, 3))
	{
		perror("listen failed");
		exit(3);
	}

	// 4.阻塞接受TCP连接（如不需要对方信息，填NULL即可）
		// 如成功，con_fd变成已连接套接字，用于通信
	// struct client_info temp;		
		// 最好不使用栈空间的局部变量，可替换为堆空间（不使用同一块空间，在下面循环中malloc）
	struct client_info *temp = NULL;
	socklen_t len = sizeof(temp.con_addr);
	pthread_t tid;
	while(1)
	{
		// 4.1 主线程：监听客户端新连接（被监听套接字阻塞）
		printf("waitting for connect...\n");
		temp = malloc(sizeof(struct client_info));
		temp->con_fd = accept(sock_fd, (struct sockaddr *)&temp.con_addr, &len);
		if(temp->con_fd == -1)
		{
			perror("accept failed");
			exit(4);
		}

		// 4.2 创建子线程，处理新连接
		pthread_create(&tid, NULL, thread_con, temp);
	}
	
	// 5.关闭连接
	close(sock_fd);	// 关闭监听（关闭新客户端连接）

	return 0;
}

