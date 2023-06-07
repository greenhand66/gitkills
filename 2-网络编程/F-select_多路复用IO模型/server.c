#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/select.h>

//自定义存放客户端信息结构体
typedef struct client_info{
    int connfd;
    struct sockaddr_in client_addr;
}info;

//当前客户端数量
int sum = 0;
//线程接收客户端消息并回射
void recv_send(info client)
{
    char buf[50];
    bzero(buf,sizeof(buf));
    recv(client.connfd,buf,sizeof(buf),0);
    printf("[%s]<%hu> data: %s\n",inet_ntoa(client.client_addr.sin_addr),ntohs(client.client_addr.sin_port),buf);
    send(client.connfd,buf,sizeof(buf),0);
    if(!strncmp(buf,"quit",4))
    {
        printf("quit [%s]<%hu> data: %s\n",inet_ntoa(client.client_addr.sin_addr),ntohs(client.client_addr.sin_port),buf);
        close(client.connfd);
        sum--;
        if(sum == 0)
            exit(0);
    }
}

//主线程监听，连接客户端
int main(int argc,char **argv)
{
    //创建套接字
    int tcpfd = socket(AF_INET,SOCK_STREAM,0);
    if(tcpfd == -1)
    {
        perror("socket failed!");
        return -1;
    }
    //定义自己的网络通信用结构体绑定自己的端口号和ip
    //绑定的ip为INADDR_ANY0.0.0.0时对于多IP主机(有多个网卡)，表示可以接受发个任意网卡的消息
    //若只绑定一个则表示仅接收发给本IP的消息
    struct sockaddr_in myaddr;
    inet_pton(AF_INET,argv[1],(void*)&myaddr.sin_addr);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[2]));
    //服务器端绑定套接字
    if(bind(tcpfd,(struct sockaddr*)&myaddr,sizeof(myaddr)))
    {
        perror("bind failed!");
        return -1;
    }

    if(listen(tcpfd,3))
    {
        perror("no client connect!");
        exit(0);
    }

    //最多3个客户端在线
    info client[3];
    int connfd;
    //套接字最大值（在监听套接字和所有已连接客户端套接字中编号最大的）
    int maxfd;
    int n;
    int i;
    socklen_t len = sizeof(struct sockaddr_in);

    // //设置超时时间
    // struct timeval t;
    // t.tv_sec = 30;
    //定义读操作集合
    fd_set r_fds;
    
    while(1)
    {
        printf("sum=%d\n",sum);
        //清空集合
        FD_ZERO(&r_fds);
        //将监听用套接字加入读操作集合
        FD_SET(tcpfd,&r_fds);
        maxfd = tcpfd;
        for(i = 0;i < sum;i++)
        {
            FD_SET(client[i].connfd,&r_fds);
            maxfd = maxfd > client[i].connfd ? maxfd : client[i].connfd;
        }
printf("maxfd = %d\n",maxfd);
		printf("waitting....\n");
        n = select(maxfd+1,&r_fds,NULL,NULL,NULL);
printf("n = %d\n",n);
        //判断是否监听套接字是否在集合（即就绪，不就绪会被踢出）
        if(FD_ISSET(tcpfd,&r_fds))
        {
            if(sum+1 == 4)
            {
                printf("在线用户最多3个！\n");
                break;
            }
            bzero(&client[sum],sizeof(info));
            //若是监听套接字，则与客户端建立连接
            client[sum].connfd = accept(tcpfd,(struct sockaddr*)&client[sum].client_addr,&len);
            printf("connfd = %d\n",connfd);
            if(connfd == -1)
            {
                printf("connect failed!");
                return -1;
            }
    printf("[%s]<%hu>\n",inet_ntoa(client->client_addr.sin_addr),ntohs(client->client_addr.sin_port));
            FD_SET(client[sum].connfd,&r_fds);
            
            maxfd = maxfd > client[sum-1].connfd ? maxfd : client[sum-1].connfd;
printf("sum++ =%d\n",++sum);
        }
        //判断是否是已连接客户端中有用户发送消息
        for(i = 0;i < sum;i++)
        {
            if(FD_ISSET(client[i].connfd,&r_fds))
            // if(n == client[i].connfd)
            {
                recv_send(client[i]);
            }
        }
    }
    
    close(tcpfd);
    return 0;
}