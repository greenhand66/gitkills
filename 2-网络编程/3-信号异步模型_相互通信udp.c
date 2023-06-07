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
#include<signal.h>

int sockfd;
struct sockaddr_in destaddr;

void recv_data(int signum)
{
    char buf[50];
    bzero(buf,sizeof(buf));
    socklen_t len = sizeof(destaddr);
    bzero(&destaddr,len);
    destaddr.sin_family = AF_INET;
    recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,&len);
    printf("recv data: %s\n",buf);
    if(!strcmp(buf,"quit"))
    {
        close(sockfd);
        exit(0);
    }
}
//输入要发送数据的主机IP和端口号，自己的端口号
int main(int argc,char *argv[])
{
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd == -1)
    {
        perror("socket failed!");
        return -1;
    }

    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);;
    myaddr.sin_port = htons(atoi(argv[3]));
    bind(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr));

    signal(SIGIO,recv_data);
    fcntl(sockfd,F_SETOWN,getpid());
    // int status = fcntl(sockfd,F_GETFL);
    // status |= O_ASYNC;
    fcntl(sockfd,F_SETFL,O_ASYNC);

    destaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr(argv[1]);
    destaddr.sin_port = htons(atoi(argv[2]));
    char buf[50];
    while(1)
    {
        bzero(buf,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
        sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,sizeof(destaddr));
        if(!strcmp(buf,"quit"))
            break;
    }
    close(sockfd);
    return 0;
}