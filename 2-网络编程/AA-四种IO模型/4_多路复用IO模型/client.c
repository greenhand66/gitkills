#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// ./client 127.0.0.1(服务器IP) 8888(服务器端口)
int main(int argc, const char **argv)
{
	// 0.判断用户输入参数
	if(argc != 3)
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

	// 2.绑定IP+PORT（可省略，由系统自动分配）

	// 3.与服务器建立连接
		// 设置IPv4对端地址（服务器地址）
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(atoi(argv[2]));	// 对方端口号，主机序转换为网络序
	inet_aton(argv[1], &dest_addr.sin_addr);	// 对方IP地址

	if(connect(sock_fd, (const struct sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		perror("connect failed");
		exit(2);
	}

	// 4.收发数据（recv/send 和 read/write 必须配套使用）
	char send_buf[50];
	char recv_buf[50];
	int n_send;
	while(1)
	{
		// 4.1 键盘输入字符串
		printf("Pls Input: ");
		fgets(send_buf, sizeof(send_buf), stdin); strtok(send_buf, "\n");

		// 4.2 发送字符串到服务器
		n_send = send(sock_fd, send_buf, strlen(send_buf), 0);
		printf("%d send\n", n_send);

		// 4.3 接收服务器回射数据
		// bzero(recv_buf, sizeof(recv_buf));
		// recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
		// printf("From Server: %s\n", recv_buf);
	}

	// 5.关闭连接
	close(sock_fd);

	return 0;
}

