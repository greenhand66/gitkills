#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

#if 0
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
#endif


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

	// 4.将监听套接字sock_fd设置为非阻塞状态
		// 4.1 获取监听套接字属性
	int flag;
	flag = fcntl(sock_fd, F_GETFL);
		// 4.2 添加非阻塞属性，并设置
	flag |= O_NONBLOCK;
	fcntl(sock_fd, F_SETFL, flag);

	// 5.非阻塞接受TCP连接
	int i;
	char recv_buf[50];
	int n_recv;
	int temp[10];	// 最多10个客户端套接字（可替换为链表，增强其可拓展性）
	int sum=0;		// 客户端套接字数量

	int con_fd;
	struct sockaddr_in con_addr;
	socklen_t len = sizeof(con_addr);
	while(1)
	{
		// 5.1 非阻塞监听（通过accept返回值判断）
		// printf("waitting for connect...\n");
		con_fd = accept(sock_fd, (struct sockaddr *)&con_addr, &len);
		// sleep(1);
			// 非阻塞IO模型中，添加睡眠函数sleep，会导致响应不及时。
			//					不添加睡眠函数，会导致进程CPU占用率非常高。
		if(con_fd != -1)
		{
			// 有新连接，则打印信息，并暂存套接字
			printf("new connection[%d:%s:%hu]!\n", 
						con_fd, 
						inet_ntoa(con_addr.sin_addr), 
						ntohs(con_addr.sin_port));
			temp[sum] = con_fd;
			sum++;
			// 设置已连接套接字为非阻塞
			flag = fcntl(con_fd, F_GETFL);
			flag |= O_NONBLOCK;
			fcntl(con_fd, F_SETFL, flag);
		}

		// 5.2 非阻塞接收消息（必须对所有的已连接套接字进行判断：recv() ---> 循环 ）
		for(i=0; i<sum; i++)
		{
			bzero(recv_buf, sizeof(recv_buf));
			n_recv = recv(temp[i], recv_buf, sizeof(recv_buf), 0);
			if(n_recv != -1)	// 接收到数据
				printf("Get[%d:%d]: %s\n", temp[i], n_recv, recv_buf);
		}
	}
	
	// 5.关闭连接
	close(sock_fd);	// 关闭监听（关闭新客户端连接）

	return 0;
}

