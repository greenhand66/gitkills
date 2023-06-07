#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

//自定义存放客户端信息结构体
typedef struct client_info{
    int connfd;
    struct sockaddr_in client_addr;
    int tid;
}info;

//线程接收客户端消息并回射
void *recv_send(void *argv)
{
    info *client = (info*)argv;
    char buf[50];
    while(1)
    {
        bzero(buf,sizeof(buf));
        recv(client->connfd,buf,sizeof(buf),0);
        printf("[%s]<%hu> data: %s\n",inet_ntoa(client->client_addr.sin_addr),ntohs(client->client_addr.sin_port),buf);
        send(client->connfd,buf,sizeof(buf),0);
        if(!strncmp(buf,"quit",4))
        {
            printf("quit [%s]<%hu> data: %s\n",inet_ntoa(client->client_addr.sin_addr),ntohs(client->client_addr.sin_port),buf);
            close(client->connfd);
            free(client);
            pthread_exit(NULL);
        }
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

    info *client = NULL;
    int connfd;
    int n = 0;
    socklen_t len = sizeof(struct sockaddr_in);
    pthread_t tid[n];
    while(1)
    {
        client = malloc(sizeof(info));
        connfd = accept(tcpfd,(struct sockaddr*)&client->client_addr,&len);
        if(connfd == -1)
        {
            printf("connect failed!");
            return -1;
        }
        client->connfd = connfd;
        client->tid = n;
        pthread_create(&tid[n++],NULL,recv_send,(void*)client);
    }
    for(int i = 0;i < n;i++)
    {
        pthread_join(tid[i],NULL);
        printf("joined pthread%d\n",i);
    }
    close(tcpfd);
    return 0;
}