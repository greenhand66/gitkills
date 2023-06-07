#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>

int sockfd_init(char *argv);

//传入自己端口号
int main(int argc,char **argv)
{
    int udpfd = sockfd_init(argv[1]);

    struct sockaddr_in destaddr;
    bzero(&destaddr,sizeof(destaddr));
    destaddr.sin_family = AF_INET;
    socklen_t len = sizeof(destaddr);
    
    char buf[50];
    char port[20];
    char ip_addr[20];
    while(1)
    {
        printf("请输入要发送对象的IP和端口号\n");
        scanf("%s %s ",ip_addr,port);
        destaddr.sin_addr.s_addr = inet_addr(ip_addr);
        destaddr.sin_port = htons(atoi(port));
        while(1)
        {
            bzero(buf,sizeof(buf));
            fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
            sendto(udpfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,len);

        }

    }

  


}

int sockfd_init(char *argv)
{
    int udpfd = socket(AF_INET,SOCK_DGRAM,0);
    if(udpfd = -1)
    {
        perror("socket failed!");
        return -1;
    }
/*
    要实现对等通信，则双方都要绑定自己的IP和端口号，
    若不绑定，则端口号每次运行会由系统自行分配，导致端口号的值不确定
    但开始时一方给另一方发送消息的前提是，明确对方的 IP和端口号
*/
    struct sockaddr_in myaddr;
    //INADDR_ANY = 0.0.0.0 表示任意网卡ip，可接收任意IP发来的消息
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(argv);
    bind(udpfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
    return udpfd;
}