#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

struct client_info{
	int con_fd;
	struct sockaddr_in con_addr;
};

#if 0
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

	// 4.循环使用select同时阻塞多个套接字
	int i;
	char recv_buf[50];
	int n_recv;
	int temp[10];	// 最多10个客户端套接字（可替换为链表，增强其可拓展性）
	int sum=0;		// 客户端套接字数量

	int max_fd = sock_fd; 		// 套接字最大值
	fd_set r_set;	// 读集合
	int ret;

	int con_fd;
	struct sockaddr_in con_addr;
	socklen_t len = sizeof(con_addr);
	while(1)
	{
		// 4.1 将需要阻塞的套接字加入到阻塞集
		FD_ZERO(&r_set);	// 清空读集合
		FD_SET(sock_fd, &r_set);	// 将监听套接字加入读集合
			// 循环加入数组中的已连接套接字到读集合中，并修改最大值max_fd
		for(i=0; i<sum; i++)
		{
			FD_SET(temp[i], &r_set);
			max_fd = max_fd>temp[i] ? max_fd : temp[i];
		}

		// 4.2 同时阻塞（读集合），没有超时时间
		printf("waitting....\n");
		ret = select(max_fd+1, &r_set, NULL, NULL, NULL);
		printf("ret: %d\n", ret);

		// 4.3 判断读集合中是否存在监听套接字
		if(FD_ISSET(sock_fd, &r_set))
		{
			// ——》有新连接(因为sock_fd仍然留在读集合，说明它是就绪状态)
			con_fd = accept(sock_fd, (struct sockaddr *)&con_addr, &len);
			printf("new connection[%d:%s:%hu]!\n", 
						con_fd, 
						inet_ntoa(con_addr.sin_addr), 
						ntohs(con_addr.sin_port));
			// 也需要暂存已连接套接字con_fd到数组中
			temp[sum] = con_fd;
			sum++;
		}

		// 4.4 判断读集合中是否存在已连接套接字（for循环数组）
		for(i=0; i<sum; i++)
		{
			if(FD_ISSET(temp[i], &r_set))
			{
				// ——》已连接套接字有数据，recv
				bzero(recv_buf, sizeof(recv_buf));
				n_recv = recv(temp[i], recv_buf, sizeof(recv_buf), 0);
				if(n_recv != -1)    // 接收到数据
					printf("Get[%d:%d]: %s\n", temp[i], n_recv, recv_buf);
				// 如果客户端退出，需要将套接字从数组中删除，sum--
			}
		}
	}
	
	// 5.关闭连接
	close(sock_fd);	// 关闭监听（关闭新客户端连接）

	return 0;
}

