#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>

int main(int argc,char **argv)
{
    if(argc != 2)
    {
        perror("input error!");
        return -1;
    }
    int udpfd = socket(AF_INET,SOCK_DGRAM,0);
    if(udpfd == -1)
    {
        perror("socket failed!");
        return -1;
    }

    struct sockaddr_in myaddr;
    // myaddr.sin_addr.s_addr = inet_addr(argv[1]);
    inet_aton("0.0.0.0",&myaddr.sin_addr);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));

    bind(udpfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
    
    struct sockaddr_in destaddr;
   // bzero(&destaddr,sizeof(destaddr));
    socklen_t len = sizeof(destaddr);
    char buf[300];
    while(1)
    {
        bzero(buf,sizeof(buf));
        recvfrom(udpfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,&len);
        printf("[%d]<%s>: %s\n",ntohs(destaddr.sin_port),inet_ntoa(destaddr.sin_addr),buf);
        if(!strcmp(buf,"quit"))
            break;
    }
    close(udpfd);
    return 0;
}