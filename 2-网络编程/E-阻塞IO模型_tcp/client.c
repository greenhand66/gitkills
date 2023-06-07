#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

void *recv_data(void *argv)
{
    int connfd = *(int*)argv;
    char buf[50];
    while(1)
    {
        bzero(buf,sizeof(buf));
        recv(connfd,buf,sizeof(buf),0);
        printf("client recv:%s\n",buf);
        if(!strcmp(buf,"quit"))
            break;
    }
    close(connfd);
    exit(0);
}

int main(int argc,char **argv)
{
    //创建套接字
    int tcpfd = socket(AF_INET,SOCK_STREAM,0);
    if(tcpfd == -1)
    {
        perror("socket failed!");
        return -1;
    }
  
    struct sockaddr_in destaddr;
    inet_pton(AF_INET,argv[1],(void*)&destaddr.sin_addr);
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(atoi(argv[2]));

    socklen_t len = sizeof(struct sockaddr_in);
    if(connect(tcpfd,(struct sockaddr*)&destaddr,len) == -1)
    {
        perror("connect failed!");
        return -1;

    }
    char buf[50];
    pthread_t tid;
    pthread_create(&tid,NULL,recv_data,(void*)&tcpfd);
    while(1)
    {
        bzero(buf,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
        send(tcpfd,buf,sizeof(buf),0);
    }
}