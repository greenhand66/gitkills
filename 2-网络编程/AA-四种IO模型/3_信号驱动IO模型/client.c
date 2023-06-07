#include <stdio.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

//UDP通信，发送端

// 子线程
void *thread_recv(void *arg)
{
	int sock_fd = *(int *)arg;

	//====等待接收=============================
	char recv_buf[100];

	struct sockaddr_in dest_addr;
	socklen_t len = sizeof(dest_addr);
	while(1)
	{
		bzero(recv_buf,sizeof(recv_buf));
		recvfrom(sock_fd,recv_buf,sizeof(recv_buf),0,
					(struct sockaddr *)&dest_addr,&len);
		if(strcmp(recv_buf,"exit") == 0)
			break;
		printf("%s(%hu): %s\n",	inet_ntoa(dest_addr.sin_addr),
								ntohs(dest_addr.sin_port),
								recv_buf);
	}

	close(sock_fd);

    pthread_exit(NULL);
}

// ./client 192.168.11.XXX 8888
int main(int argc, const char **argv)
{
	// 0.判断用户输入
	if(argc != 3)
	{
		printf("Input Error!\n");
		exit(0);
	}

    // 1.创建UDP套接字
	int soc_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(soc_fd == -1)
	{
		perror("socket error!");
		exit(1);
	}

    // 2.创建子线程：接收数据
    pthread_t tid;
    pthread_create(&tid, NULL, thread_recv, (void *)&soc_fd);

    // 3.主线程：输入并发送
	struct sockaddr_in dest_addr;	// 对端信息：IP+PORT
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(atoi(argv[2]));	// PORT端口号
	inet_aton(argv[1], &dest_addr.sin_addr);	// IP地址

	char send_buf[100];
    while(1)
    {
		// 3.1 输入字符串
        bzero(send_buf,sizeof(send_buf));
        fprintf(stderr, "Pls Input: ");
        fgets(send_buf, sizeof(send_buf), stdin); strtok(send_buf, "\n");

		// 3.2 发送
        sendto(soc_fd, send_buf, strlen(send_buf), 0,
            (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if(strcmp(send_buf,"exit") == 0)
            break;
    }

	// 4.关闭套接字
    close(soc_fd);
	
    return 0;
}
