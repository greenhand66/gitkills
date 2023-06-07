#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/select.h>

int init_fd(char *port)
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1)
    {
        perror("socket failed!");
        return -1;
    }
    struct sockaddr_in myaddr;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(port));
    
    if(bind(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr)) == -1)
    {
        perror("bind fd failed!");
        return -1;
    }
    if(listen(sockfd,3) == -1)
    {
        perror("listen fd failed!");
        return -1;
    }
    int connfd;
    struct sockaddr_in destaddr;
    socklen_t len = sizeof(destaddr);
    connfd = accept(sockfd,(struct sockaddr*)&destaddr,&len);
    return connfd;
}

int main(int argc,char **argv)
{
    if(argc != 2)
    {
        perror("input error!");
        return -1;
    }
    int connfd = init_fd(argv[1]);
    int maxfd = connfd > STDIN_FILENO ? connfd : STDIN_FILENO;
    fd_set set;
    struct timeval wait_time;
    char rbuf[50];
    char sbuf[50];
    int ret;
    while(1)
    {
        FD_ZERO(&set);
        FD_SET(connfd,&set);
        FD_SET(STDIN_FILENO,&set);
        wait_time.tv_sec = 5;
        ret = select(maxfd+1,&set,NULL,NULL,&wait_time);
        if(ret == 0)
        {
            printf("time out!\n");
            continue;
        }
        //若通信用套接字还在集合中，表明有数据发送过来
        if(FD_ISSET(connfd,&set))
        {
			bzero(rbuf,sizeof(rbuf));
            recv(connfd,rbuf,sizeof(rbuf),0);
            printf("recv:%s\n",rbuf);
            if(!(strncmp(rbuf,"quit",4)))
				break;
        }
        //若标准输入文件描述符还在集合中，表明有数据输入。需要发送
        if(FD_ISSET(STDIN_FILENO,&set))
        {
			bzero(sbuf,sizeof(sbuf));
            fgets(sbuf,sizeof(sbuf),stdin);strtok(sbuf,"\n");
            send(connfd,sbuf,strlen(sbuf),0);
            if(!(strncmp(sbuf,"quit",4)))
				break;
        }
        printf("ret = %d\n",ret);
    }
	close(connfd);
    return 0;
}

